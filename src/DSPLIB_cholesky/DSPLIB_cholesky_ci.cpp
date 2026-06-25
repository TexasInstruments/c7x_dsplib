// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cholesky_priv.h"

#define UNROLL_COUNT 4
#define MIN_HORIZONTAL_COLUMNS_FOR_UNROLL 2
#define NUM_VECS_IN_TILE 6

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/

#if defined(__C7100__) || defined(__C7120__)
template <typename dataType> DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong_init(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 seParamFetchL;   // Left fetch
   __SE_TEMPLATE_v1 seParamFetchR;   // Right fetch
   __SA_TEMPLATE_v1 saParamMulStore; // Stores the multiplier values
   __SA_TEMPLATE_v1 saParamLStore;   // Stores the L values

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   pKerPrivArgs->shiftForVecLenDiv = -1;
   uint32_t vecLenValue            = eleCount;
   while (vecLenValue != 0) {
      vecLenValue >>= 1;
      pKerPrivArgs->shiftForVecLenDiv++;
   }

   int32_t yStride = pKerPrivArgs->stride / sizeof(dataType);
   /**********************************************************************/
   /* Prepare streaming engine for fetching L values(Left) Merge approach*/
   /**********************************************************************/
   seParamFetchL = __gen_SE_TEMPLATE_v1();

   seParamFetchL.ICNT0 = eleCount;
   seParamFetchL.ICNT1 = 0;       // No of rows to process
   seParamFetchL.DIM1  = yStride; // order
   seParamFetchL.ICNT2 = 0;       // No of left fetches
   seParamFetchL.DIM2  = eleCount << 1;

   seParamFetchL.ELETYPE = SE_ELETYPE;
   seParamFetchL.VECLEN  = SE_VECLEN;
   seParamFetchL.DIMFMT  = __SE_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare streaming engine for fetching L values(Right) Merge approach*/
   /**********************************************************************/
   seParamFetchR = __gen_SE_TEMPLATE_v1();

   seParamFetchR.ICNT0 = eleCount;
   seParamFetchR.ICNT1 = 0; // No of rows to process
   seParamFetchR.DIM1  = yStride;
   seParamFetchR.ICNT2 = 0; // No of right fetches
   seParamFetchR.DIM2  = eleCount << 1;

   seParamFetchR.ELETYPE = SE_ELETYPE;
   seParamFetchR.VECLEN  = SE_VECLEN;
   seParamFetchR.DIMFMT  = __SE_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare Address generator to store the multipliers                 */
   /**********************************************************************/
   saParamMulStore = __gen_SA_TEMPLATE_v1();

   saParamMulStore.ICNT0  = pKerPrivArgs->order;
   saParamMulStore.VECLEN = __SA_VECLEN_1ELEM;
   saParamMulStore.DIMFMT = __SA_DIMFMT_1D;

   /**********************************************************************/
   /* Prepare Address generator to store the L values                    */
   /**********************************************************************/
   saParamLStore = __gen_SA_TEMPLATE_v1();

   saParamLStore.ICNT0  = pKerPrivArgs->order;
   saParamLStore.VECLEN = SA_VECLEN;
   saParamLStore.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE2_PARAM_OFFSET) = seParamFetchL;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE3_PARAM_OFFSET) = seParamFetchR;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA0_PARAM_OFFSET) = saParamMulStore;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA1_PARAM_OFFSET) = saParamLStore;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong_init<float>(DSPLIB_kernelHandle handle);
template DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong_init<double>(DSPLIB_kernelHandle handle);

#elif defined(__C7504__) || defined(__C7524__)

template <typename dataType> DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong_init(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 seParamFetchU;   // Left fetch
   __SE_TEMPLATE_v1 seParamFetchD;   // Right fetch
   __SA_TEMPLATE_v1 saParamMulFetch; // Stores the multiplier values
   __SA_TEMPLATE_v1 saParamALoad;    // Stores the L (Left half) values
   __SA_TEMPLATE_v1 saParamLStore;   // Stores the L (Right half) values

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   uint32_t yStride = pKerPrivArgs->stride / sizeof(dataType);

   /**********************************************************************/
   /* Prepare streaming engine for fetching L values(Up)                 */
   /**********************************************************************/
   seParamFetchU = __gen_SE_TEMPLATE_v1();

   seParamFetchU.ICNT0 = NUM_VECS_IN_TILE * eleCount;
   seParamFetchU.ICNT1 = 0; // (row+1)/2 or (row+1)>>1; To be
                            // reconfigured in the loop

   seParamFetchU.DIM1 = (yStride) << 1; // yStride * 2  (row+1)<<1; How much
                                        // the vecc start should shift so that
                                        // it fetches the correct next element

   seParamFetchU.ICNT2 = 0; // Number of tiles; To be recofnigured in
                            // the loop. This will be ceil(order/vecLen)

   seParamFetchU.DIM2          = NUM_VECS_IN_TILE * eleCount; // Distance from one tile to the next tile
   seParamFetchU.DECDIM1       = __SE_DECDIM_DIM2;
   seParamFetchU.DECDIM1_WIDTH = pKerPrivArgs->order; // No of (validElemsPerRow)

   seParamFetchU.ELETYPE = SE_ELETYPE;
   seParamFetchU.VECLEN  = SE_VECLEN;
   seParamFetchU.DIMFMT  = __SE_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare streaming engine for fetching L values(Down)               */
   /**********************************************************************/
   seParamFetchD = __gen_SE_TEMPLATE_v1();

   seParamFetchD.ICNT0         = NUM_VECS_IN_TILE * eleCount;
   seParamFetchD.ICNT1         = 0;              // (row)/2 or (row)>>1
   seParamFetchD.DIM1          = (yStride) << 1; // yStride * 2  (row+1)<<1
   seParamFetchD.ICNT2         = 0;              // 2
   seParamFetchD.DIM2          = NUM_VECS_IN_TILE * eleCount;
   seParamFetchD.DECDIM1       = __SE_DECDIM_DIM2;
   seParamFetchD.DECDIM1_WIDTH = pKerPrivArgs->order; // No of (validElemsPerRow)

   seParamFetchD.ELETYPE = SE_ELETYPE;
   seParamFetchD.VECLEN  = SE_VECLEN;
   seParamFetchD.DIMFMT  = __SE_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare Address generator to fetch the multipliers                 */
   /**********************************************************************/
   saParamMulFetch = __gen_SA_TEMPLATE_v1();

   saParamMulFetch.ICNT0 = 1;       // Number of multiplier for a row is 1
   saParamMulFetch.ICNT1 = 0;       // To be reconfigured in the loop
   saParamMulFetch.DIM1  = yStride; // Distance to the next multiplier value
   saParamMulFetch.ICNT2 = 0;       // To be reconfigured to number of tiles
   saParamMulFetch.DIM2  = 0;       // Distance to the next multiplier value

   saParamMulFetch.VECLEN = __SA_VECLEN_1ELEM; // We need 1 element
   saParamMulFetch.DIMFMT = __SA_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare Address generator to store the L values               */
   /**********************************************************************/
   saParamLStore = __gen_SA_TEMPLATE_v1();

   saParamLStore.ICNT0 =
       0; // Valid elements in a row without the junk values. To be reconfigured every row => order - row
   saParamLStore.VECLEN = SA_VECLEN;
   saParamLStore.DIMFMT = __SA_DIMFMT_1D;

   /**********************************************************************/
   /* Prepare Address generator to store the Right L values              */
   /**********************************************************************/
   saParamALoad = __gen_SA_TEMPLATE_v1();

   saParamALoad.ICNT0 =
       0; // Valid elements in a row without the junk values. To be reconfigured every row => order - row
   saParamALoad.VECLEN = SA_VECLEN;
   saParamALoad.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE2_PARAM_OFFSET) = seParamFetchU;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE3_PARAM_OFFSET) = seParamFetchD;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA0_PARAM_OFFSET) = saParamMulFetch;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA1_PARAM_OFFSET) = saParamLStore;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA2_PARAM_OFFSET) = saParamALoad;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong_init<float>(DSPLIB_kernelHandle handle);
template DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong_init<double>(DSPLIB_kernelHandle handle);

#else
#error invalid target
#endif

// Initialize SE params
template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_init_ci(DSPLIB_kernelHandle             handle,
                                      const DSPLIB_bufParams2D_t     *bufParamsIn,
                                      const DSPLIB_bufParams2D_t     *bufParamsOut,
                                      const DSPLIB_cholesky_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS             status       = DSPLIB_SUCCESS;
   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;
   uint8_t                  *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                   order        = pKerPrivArgs->order;
   int32_t                   strideA      = pKerPrivArgs->stride;
   int32_t                   colAStride   = strideA / sizeof(dataType);

   DSPLIB_cholesky_c7x_PingPong_init<dataType>(handle);
   DSPLIB_cholesky_inplace_isPosDefinite_init<dataType>(order, colAStride, pBlock);

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");

   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_init_ci<float>(DSPLIB_kernelHandle             handle,
                                                      const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                      const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                      const DSPLIB_cholesky_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_cholesky_init_ci<double>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                       const DSPLIB_cholesky_InitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/

template <typename dataType> inline dataType getRecipSqrt(dataType a)
{

   const dataType Half  = 0.5f;
   const dataType OneP5 = 1.5f;
   dataType       x;

   x = __recip_sqrt(a); // compute square root reciprocal

   x = x * (OneP5 - (a * x * x * Half));
   x = x * (OneP5 - (a * x * x * Half));
   // PRAGMA: do not unroll this loop
   // #pragma UNROLL(1)

   return x;
}

#if defined(__C7100__) || defined(__C7120__)
template <typename dataType> inline c7x::uchar_vec DSPLIB_cholesky_getMaskIncrement();
template <> inline c7x::uchar_vec DSPLIB_cholesky_getMaskIncrement<float>() { return (c7x::uchar_vec) 4; };
template <> inline c7x::uchar_vec DSPLIB_cholesky_getMaskIncrement<double>() { return (c7x::uchar_vec) 8; };

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong(int                       enable_test,
                                           DSPLIB_cholesky_PrivArgs *pKerPrivArgs,
                                           dataType *restrict pInALocal,
                                           dataType *restrict pOutULocal,
                                           dataType *restrict pMulBuffer)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 seParamFetchL = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE2_PARAM_OFFSET);
   __SE_TEMPLATE_v1 seParamFetchR = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE3_PARAM_OFFSET);

   __SA_TEMPLATE_v1 saParamLStore = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 saParamALoad  = saParamLStore;

   int32_t order  = pKerPrivArgs->order;
   int32_t vecLen = eleCount;

   int32_t row, fetch, lRow;
   int32_t shiftForVecLenDiv = pKerPrivArgs->shiftForVecLenDiv;
   int32_t stride            = pKerPrivArgs->stride;
   int32_t yStride           = stride / sizeof(dataType);

   dataType *pLFirstRow = pOutULocal;
   dataType  recipDiagValue;

   c7x::uchar_vec vMask, vMaskInit;
   vMaskInit.s[0] = 0;
   vMaskInit.s[1] = 1;
   vMaskInit.s[2] = 2;
   vMaskInit.s[3] = 3;
   vMaskInit.s[4] = 4;
   vMaskInit.s[5] = 5;
   vMaskInit.s[6] = 6;
   vMaskInit.s[7] = 7;

   c7x::uchar_vec vMaskIncrement = DSPLIB_cholesky_getMaskIncrement<dataType>();

   int32_t blockMax  = int32_t((uint32_t) (order + vecLen - 1) >> (uint32_t) shiftForVecLenDiv);
   int32_t extraRows = vecLen - (int32_t) ((uint32_t) order & (uint32_t) (vecLen - 1)); // gives extra rows needed
                                                                                        // to make the height of matrix
                                                                                        // integral multiple of vecLen
   if (extraRows == vecLen) {
      extraRows = 0;
   }

   int32_t elemsPerRow = order;
   int32_t offset      = 0;
   int32_t rowNumber   = 0;
   int32_t block;
   int32_t elemsPerRowCeil = elemsPerRow + vecLen - 1;

   int32_t   lezrCount[UNROLL_COUNT];
   __SE_LEZR lezrDim[UNROLL_COUNT];

   int32_t   *lezrCountPtr = lezrCount;
   __SE_LEZR *lezrDimPtr   = lezrDim;

   *lezrCountPtr = 0;
   *lezrDimPtr   = __SE_LEZR_OFF;
   lezrCountPtr++;
   lezrDimPtr++;

   for (int32_t i = UNROLL_COUNT - 1; i > 0; i--) {
      *lezrCountPtr = i;
      *lezrDimPtr   = __SE_LEZR_ICNT1;
      lezrCountPtr++;
      lezrDimPtr++;
   }

   for (block = 0; block < blockMax - 2; block++) {
      // configuration for sa
      saParamLStore.ICNT0 = elemsPerRow;
      saParamALoad.ICNT0  = elemsPerRow;

      vMask = vMaskInit;
      for (row = 0; row < vecLen; row++) {
         int32_t fetchesPerRow =
             (int32_t) ((uint32_t) elemsPerRowCeil >> (uint32_t) shiftForVecLenDiv); // number of vector fetches per row
         int32_t leftFetchesPerRow  = (int32_t) ((uint32_t) (fetchesPerRow + 1) >> 1u); // number of fetches by SE0
         int32_t rightFetchesPerRow = fetchesPerRow - leftFetchesPerRow;                // number of fetches by SE1
         int32_t lezrIndex =
             (int32_t) ((uint32_t) rowNumber & (uint32_t) (UNROLL_COUNT - 1)); // rowNumber%4=>4 is unroll count

         // configuration for SE
         seParamFetchL.ICNT1    = rowNumber;
         seParamFetchL.ICNT2    = leftFetchesPerRow;
         seParamFetchL.LEZR     = lezrDim[lezrIndex];
         seParamFetchL.LEZR_CNT = lezrCount[lezrIndex];

         seParamFetchR.ICNT1    = rowNumber;
         seParamFetchR.ICNT2    = rightFetchesPerRow;
         seParamFetchR.LEZR     = lezrDim[lezrIndex];
         seParamFetchR.LEZR_CNT = lezrCount[lezrIndex];

         if (rowNumber > 0) {
            __SE0_OPEN(pLFirstRow, seParamFetchL);
            __SE1_OPEN(pLFirstRow + vecLen, seParamFetchR);
         }

         __SA1_OPEN(saParamLStore);
         __SA2_OPEN(saParamALoad);

         dataType *ptrL = pOutULocal + offset;
         dataType *ptrA = pInALocal + offset;

         vec vLSum0 = vec(0); // Holds sum of all LxLy
         vec vLSum1 = vec(0);
         vec vLSum2 = vec(0);
         vec vLSum3 = vec(0);

         vec vLA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

         vec vRSum0 = vec(0); // Holds sum of all LxLy
         vec vRSum1 = vec(0);
         vec vRSum2 = vec(0);
         vec vRSum3 = vec(0);

         vec vRA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

         dataType *pMulStore = pMulBuffer;
         for (lRow = 0; lRow < rowNumber; lRow += UNROLL_COUNT) {
            vec vLL0     = c7x::strm_eng<0, vec>::get_adv(); // L value vector from each row
            vec vLL0Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL0)));
            vLSum0 += vLL0 * vLL0Temp.s[0];
            vec vLR0 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum0 += vLR0 * vLL0Temp.s[0];
            *pMulStore = vLL0Temp.s[0];
            pMulStore++;

            vec vLL1     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL1Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL1)));
            vLSum1 += vLL1 * vLL1Temp.s[0];
            vec vLR1 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum1 += vLR1 * vLL1Temp.s[0];
            *pMulStore = vLL1Temp.s[0];
            pMulStore++;

            vec vLL2     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL2Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL2)));
            vLSum2 += vLL2 * vLL2Temp.s[0];
            vec vLR2 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum2 += vLR2 * vLL2Temp.s[0];
            *pMulStore = vLL2Temp.s[0];
            pMulStore++;

            vec vLL3     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL3Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL3)));
            vLSum3 += vLL3 * vLL3Temp.s[0];
            vec vLR3 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum3 += vLR3 * vLL3Temp.s[0];
            *pMulStore = vLL3Temp.s[0];
            pMulStore++;
         }
         vLSum0 += vLSum1;
         vLSum2 += vLSum3;

         vRSum0 += vRSum1;
         vRSum2 += vRSum3;

         vLA -= vLSum2;
         vec vLDiff = vLA - vLSum0;

         vRA -= vRSum2;
         vec vRDiff = vRA - vRSum0;

         vec vLDiffTemp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLDiff)));

         recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

         __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

         __vpred vpStoreR   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *outVecPtrR = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpStoreR, outVecPtrR, vRDiff * recipDiagValue);

         /* Handling all the pong fetches */
         for (fetch = 0; fetch < leftFetchesPerRow - 1; fetch++) {
            vLSum0 = vec(0);
            vLSum1 = vec(0);
            vLSum2 = vec(0);
            vLSum3 = vec(0);

            vLA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

            vRSum0 = vec(0);
            vRSum1 = vec(0);
            vRSum2 = vec(0);
            vRSum3 = vec(0);

            vRA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

            pMulStore = pMulBuffer;
            for (lRow = 0; lRow < rowNumber; lRow += UNROLL_COUNT) {
               vec multiplier0 = __vload_dup(pMulStore);
               pMulStore++;
               vec vLL0 = c7x::strm_eng<0, vec>::get_adv();
               vec vLR0 = c7x::strm_eng<1, vec>::get_adv();
               vLSum0 += vLL0 * multiplier0;
               vRSum0 += vLR0 * multiplier0;

               vec multiplier1 = __vload_dup(pMulStore);
               pMulStore++;
               vec vLL1 = c7x::strm_eng<0, vec>::get_adv();
               vec vLR1 = c7x::strm_eng<1, vec>::get_adv();
               vLSum1 += vLL1 * multiplier1;
               vRSum1 += vLR1 * multiplier1;

               vec multiplier2 = __vload_dup(pMulStore);
               pMulStore++;
               vec vLL2 = c7x::strm_eng<0, vec>::get_adv();
               vec vLR2 = c7x::strm_eng<1, vec>::get_adv();
               vLSum2 += vLL2 * multiplier2;
               vRSum2 += vLR2 * multiplier2;

               vec multiplier3 = __vload_dup(pMulStore);
               pMulStore++;
               vec vLL3 = c7x::strm_eng<0, vec>::get_adv();
               vec vLR3 = c7x::strm_eng<1, vec>::get_adv();
               vLSum3 += vLL3 * multiplier3;
               vRSum3 += vLR3 * multiplier3;
            }
            vLSum0 += vLSum1;
            vLSum2 += vLSum3;

            vRSum0 += vRSum1;
            vRSum2 += vRSum3;

            vLA -= vLSum2;
            vec vLDiff1 = vLA - vLSum0;

            vRA -= vRSum2;
            vec vRDiff1 = vRA - vRSum0;

            __vpred vpStoreL1   = c7x::strm_agen<1, vec>::get_vpred();
            vec    *outVecPtrL1 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpStoreL1, outVecPtrL1, vLDiff1 * recipDiagValue);

            __vpred vpStoreR1   = c7x::strm_agen<1, vec>::get_vpred();
            vec    *outVecPtrR1 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpStoreR1, outVecPtrR1, vRDiff1 * recipDiagValue);
         }

         offset += yStride;
         rowNumber++;
         vMask += vMaskIncrement; // uchar(4);
      }
      pLFirstRow += vecLen;
      offset += vecLen;
      elemsPerRow -= vecLen;
      elemsPerRowCeil -= vecLen;
   }

   for (; block < blockMax - 1; block++) {
      // configuration for sa
      saParamLStore.ICNT0 = elemsPerRow;
      saParamALoad.ICNT0  = elemsPerRow;

      vMask = vMaskInit;
      for (row = 0; row < vecLen; row++) {
         int32_t lezrIndex =
             (int32_t) ((uint32_t) rowNumber & (uint32_t) (UNROLL_COUNT - 1)); // rowNumber%4=>4 is unroll count

         // configuration for SE
         seParamFetchL.ICNT1    = rowNumber;
         seParamFetchL.ICNT2    = 1;
         seParamFetchL.LEZR     = lezrDim[lezrIndex];
         seParamFetchL.LEZR_CNT = lezrCount[lezrIndex];

         seParamFetchR.ICNT1    = rowNumber;
         seParamFetchR.ICNT2    = 1;
         seParamFetchR.LEZR     = lezrDim[lezrIndex];
         seParamFetchR.LEZR_CNT = lezrCount[lezrIndex];

         if (rowNumber > 0) {
            __SE0_OPEN(pLFirstRow, seParamFetchL);
            __SE1_OPEN(pLFirstRow + vecLen, seParamFetchR);
         }

         __SA1_OPEN(saParamLStore);
         __SA2_OPEN(saParamALoad);
         dataType *ptrL = pOutULocal + offset;
         dataType *ptrA = pInALocal + offset;

         vec vLSum0 = vec(0); // Holds sum of all LxLy
         vec vLSum1 = vec(0);
         vec vLSum2 = vec(0);
         vec vLSum3 = vec(0);

         vec vLA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

         vec vRSum0 = vec(0); // Holds sum of all LxLy
         vec vRSum1 = vec(0);
         vec vRSum2 = vec(0);
         vec vRSum3 = vec(0);

         vec vRA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));
         for (lRow = 0; lRow < rowNumber; lRow += UNROLL_COUNT) {
            vec vLL0     = c7x::strm_eng<0, vec>::get_adv(); // L value vector from each row
            vec vLL0Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL0)));
            vLSum0 += vLL0 * vLL0Temp.s[0];
            vec vLR0 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum0 += vLR0 * vLL0Temp.s[0];

            vec vLL1     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL1Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL1)));
            vLSum1 += vLL1 * vLL1Temp.s[0];
            vec vLR1 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum1 += vLR1 * vLL1Temp.s[0];

            vec vLL2     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL2Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL2)));
            vLSum2 += vLL2 * vLL2Temp.s[0];
            vec vLR2 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum2 += vLR2 * vLL2Temp.s[0];

            vec vLL3     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL3Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL3)));
            vLSum3 += vLL3 * vLL3Temp.s[0];
            vec vLR3 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum3 += vLR3 * vLL3Temp.s[0];
         }
         vLSum0 += vLSum1;
         vLSum2 += vLSum3;

         vRSum0 += vRSum1;
         vRSum2 += vRSum3;

         vLA -= vLSum2;
         vec vLDiff = vLA - vLSum0;

         vRA -= vRSum2;
         vec vRDiff = vRA - vRSum0;

         vec vLDiffTemp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLDiff)));

         recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

         __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

         __vpred vpStoreR   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *outVecPtrR = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpStoreR, outVecPtrR, vRDiff * recipDiagValue);

         offset += yStride;
         rowNumber++;
         vMask += vMaskIncrement; // uchar(4);
      }

      pLFirstRow += vecLen;
      offset += vecLen;
      elemsPerRow -= vecLen;
   }

   for (; block < blockMax; block++) {
      // configuration for sa
      saParamLStore.ICNT0 = elemsPerRow;
      saParamALoad.ICNT0  = elemsPerRow;

      /*seParamFetchL => Up fetch  seParamFetchR => Down fetch*/
      seParamFetchL.ICNT0 = vecLen;
      seParamFetchL.DIM1  = (int32_t) ((uint32_t) yStride << 1u); // order
      seParamFetchR.ICNT0 = vecLen;
      seParamFetchR.DIM1  = (int32_t) ((uint32_t) yStride << 1u); // order

      seParamFetchL.DIMFMT = __SE_DIMFMT_2D;
      seParamFetchR.DIMFMT = __SE_DIMFMT_2D;

      vMask = vMaskInit;

      for (row = 0; row < vecLen - extraRows; row++) {

         if (rowNumber < 2) {

            // configuration for SE
            seParamFetchL.ICNT1 = rowNumber;

            if (rowNumber > 0) {
               __SE0_OPEN(pLFirstRow, seParamFetchL);

               __SA1_OPEN(saParamLStore);
               __SA2_OPEN(saParamALoad);

               dataType *ptrA = pInALocal + offset;

               vec vLA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

               vec       vLSum0 = vec(0); // Holds sum of all LxLy
               dataType *ptrL   = pOutULocal + offset;

               vec vLL0     = c7x::strm_eng<0, vec>::get(); // L value vector from each row
               vec vLL0Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL0)));
               vLSum0 += vLL0 * vLL0Temp.s[0];

               vec vLDiff = vLA - vLSum0;

               vec vLDiffTemp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLDiff)));

               recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

               __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
               vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
               __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

               offset += yStride;
               rowNumber++;
            }
            else {

               __SA1_OPEN(saParamLStore);
               __SA2_OPEN(saParamALoad);

               dataType *ptrA = pInALocal + offset;
               vec       vLA  = *(c7x::strm_agen<2, vec>::get_adv(ptrA));
               dataType *ptrL = pOutULocal + offset;

               vec vLDiff     = vLA; // - vLSum0;
               vec vLDiffTemp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLDiff)));
               recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

               __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
               vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
               __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

               offset += yStride;
               rowNumber++;
            }
         }
         else {
            int32_t upFetchesPerRow   = (int32_t) ((uint32_t) (rowNumber + 1) >> 1u);
            int32_t downFetchesPerRow = (int32_t) ((uint32_t) rowNumber >> 1u);

            // configuration for SE
            seParamFetchL.ICNT1 = upFetchesPerRow;
            seParamFetchR.ICNT1 = downFetchesPerRow;

            __SE0_OPEN(pLFirstRow, seParamFetchL);
            __SE1_OPEN(pLFirstRow + yStride, seParamFetchR);

            __SA1_OPEN(saParamLStore);
            __SA2_OPEN(saParamALoad);
            dataType *ptrL = pOutULocal + offset;
            dataType *ptrA = pInALocal + offset;

            vec vLSum0 = vec(0); // Holds sum of all LxLy
            vec vLSum1 = vec(0);
            vec vLSum2 = vec(0);
            vec vLSum3 = vec(0);

            vec vRSum0 = vec(0); // Holds sum of all LxLy
            vec vRSum1 = vec(0);
            vec vRSum2 = vec(0);
            vec vRSum3 = vec(0);
            vec vLA    = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

            for (lRow = 0; lRow < upFetchesPerRow; lRow += UNROLL_COUNT) {
               vec vLL0     = c7x::strm_eng<0, vec>::get_adv(); // L value vector from each row
               vec vLL0Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL0)));
               vLSum0 += vLL0 * vLL0Temp.s[0];

               vec vLR0     = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
               vec vLR0Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLR0)));
               vRSum0 += vLR0 * vLR0Temp.s[0];

               vec vLL1     = c7x::strm_eng<0, vec>::get_adv();
               vec vLL1Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL1)));
               vLSum1 += vLL1 * vLL1Temp.s[0];
               vec vLR1     = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
               vec vLR1Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLR1)));
               vRSum1 += vLR1 * vLR1Temp.s[0];

               vec vLL2     = c7x::strm_eng<0, vec>::get_adv();
               vec vLL2Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL2)));
               vLSum2 += vLL2 * vLL2Temp.s[0];
               vec vLR2     = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
               vec vLR2Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLR2)));
               vRSum2 += vLR2 * vLR2Temp.s[0];

               vec vLL3     = c7x::strm_eng<0, vec>::get_adv();
               vec vLL3Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLL3)));
               vLSum3 += vLL3 * vLL3Temp.s[0];
               vec vLR3     = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
               vec vLR3Temp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLR3)));
               vRSum3 += vLR3 * vLR3Temp.s[0];
            }
            vLSum0 += vLSum1;
            vLSum2 += vLSum3;

            vRSum0 += vRSum1;
            vRSum2 += vRSum3;

            vLSum2 += vLSum0;
            vRSum2 += vRSum0;

            vec vLDiff = vLA - vLSum2 - vRSum2;

            vec vLDiffTemp = c7x::reinterpret<vec>(__permute(vMask, __as_uchar64(vLDiff)));

            recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

            __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
            vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

            offset += yStride;
            rowNumber++;
         }
         vMask += vMaskIncrement; // uchar(4);
      }

      pLFirstRow += vecLen;
      offset += vecLen;
      elemsPerRow -= vecLen;
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return DSPLIB_SUCCESS;
}

#elif defined(__C7504__) || defined(__C7524__)

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong(int                       enable_test,
                                           DSPLIB_cholesky_PrivArgs *pKerPrivArgs,
                                           dataType *restrict pInALocal,
                                           dataType *restrict pOutULocal,
                                           dataType *restrict pMulBuffer)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   // SE Template loading
   __SE_TEMPLATE_v1 seParamFetchU = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE2_PARAM_OFFSET);
   __SE_TEMPLATE_v1 seParamFetchD = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE3_PARAM_OFFSET);

   // SA Template loading
   __SA_TEMPLATE_v1 saParamMulFetch = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 saParamLStore   = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 saParamALoad    = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA2_PARAM_OFFSET);

   int32_t order  = pKerPrivArgs->order;
   int32_t vecLen = eleCount;

   int32_t row, lRow, tile_i;
   int32_t stride  = pKerPrivArgs->stride;
   int32_t yStride = stride / sizeof(dataType);

   dataType *pLFirstRow = pOutULocal;
   dataType *ptrL       = pOutULocal;
   dataType *ptrA       = pInALocal;

   dataType *pMultiplier = NULL;
   dataType  recipDiagValue;

   bool    toggle             = 0;
   int32_t validElemsPerRow   = order;
   int32_t tile_width         = NUM_VECS_IN_TILE * vecLen;
   int32_t tileWidthCeilValue = validElemsPerRow + tile_width - 1;

   __SE_ELEDUP SE_ELEDUP;
   SE_ELEDUP = c7x::se_eledup<dataType, vec>::value;

   vec vec00, vec01, vec02, vec03, vec04, vec05; // vec for holding se0 fetches
   vec vec10, vec11, vec12, vec13, vec14, vec15; // vec for holding se1 fetches
   vec acc00, acc01, acc02, acc03, acc04, acc05; // vec for holding the accumulated values
   vec acc10, acc11, acc12, acc13, acc14, acc15; // vec for holding the accumulated values

   int32_t lRowSE0;
   int32_t lRowSE1;

   for (row = 0; row < order; row++) {
      pMultiplier = pLFirstRow;
      /**
       * SA configurations for A load and L store go here as it depends only on the current row
       */
      saParamLStore.ICNT0 = validElemsPerRow;
      saParamALoad.ICNT0  = validElemsPerRow;

      __SA1_OPEN(saParamLStore);
      __SA2_OPEN(saParamALoad);

      if (validElemsPerRow > MIN_HORIZONTAL_COLUMNS_FOR_UNROLL * vecLen) {

         lRowSE0 = (int32_t) (((uint32_t) row + 1U) >> 1U);
         lRowSE1 = (int32_t) (((uint32_t) row) >> 1U);

         seParamFetchU.DECDIM1_WIDTH = validElemsPerRow;
         seParamFetchD.DECDIM1_WIDTH = validElemsPerRow;

         // SE and SA Configurations
         int32_t nTiles = (tileWidthCeilValue) / tile_width;

         // SA configurations depends on each tile
         saParamMulFetch.ICNT1 = row; //((row + toggle) == 0) ? 1 : (row + toggle);
         saParamMulFetch.ICNT2 = nTiles;
         if (row > 0) {
            __SA0_OPEN(saParamMulFetch);
         }

         seParamFetchU.ICNT1 = lRowSE0;
         seParamFetchU.ICNT2 = nTiles;

         seParamFetchD.ICNT1 = lRowSE1;
         seParamFetchD.ICNT2 = nTiles;

         if (lRowSE0 > 0) {
            __SE0_OPEN(pLFirstRow, seParamFetchU);
         }
         if (lRowSE1 > 0) {
            __SE1_OPEN(pLFirstRow + yStride, seParamFetchD);
         }

         // Loops to iterate through the tiles

         for (tile_i = 0; tile_i < nTiles; tile_i++) {
            acc00 = vec(0);
            acc01 = vec(0);
            acc02 = vec(0);
            acc03 = vec(0);
            acc04 = vec(0);
            acc05 = vec(0);

            acc10 = vec(0);
            acc11 = vec(0);
            acc12 = vec(0);
            acc13 = vec(0);
            acc14 = vec(0);
            acc15 = vec(0);
            for (lRow = 0; lRow < lRowSE1; lRow++) {
               dataType *pMultiplierUp = c7x::strm_agen<0, dataType>::get_adv(pMultiplier);
               vec       multiplier0   = __vload_dup(pMultiplierUp);

               dataType *pMultiplierDown = c7x::strm_agen<0, dataType>::get_adv(pMultiplier);
               vec       multiplier1     = __vload_dup(pMultiplierDown);
               vec00                     = c7x::strm_eng<0, vec>::get_adv();
               vec10                     = c7x::strm_eng<1, vec>::get_adv();
               acc00 += (vec00 * multiplier0);
               acc10 += (vec10 * multiplier1);

               vec01 = c7x::strm_eng<0, vec>::get_adv();
               vec11 = c7x::strm_eng<1, vec>::get_adv();
               acc01 += (vec01 * multiplier0);
               acc11 += (vec11 * multiplier1);

               vec02 = c7x::strm_eng<0, vec>::get_adv();
               vec12 = c7x::strm_eng<1, vec>::get_adv();
               acc02 += (vec02 * multiplier0);
               acc12 += (vec12 * multiplier1);

               vec03 = c7x::strm_eng<0, vec>::get_adv();
               vec13 = c7x::strm_eng<1, vec>::get_adv();
               acc03 += (vec03 * multiplier0);
               acc13 += (vec13 * multiplier1);

               vec04 = c7x::strm_eng<0, vec>::get_adv();
               vec14 = c7x::strm_eng<1, vec>::get_adv();
               acc04 += (vec04 * multiplier0);
               acc14 += (vec14 * multiplier1);

               vec05 = c7x::strm_eng<0, vec>::get_adv();
               vec15 = c7x::strm_eng<1, vec>::get_adv();
               acc05 += (vec05 * multiplier0);
               acc15 += (vec15 * multiplier1);
            }

            if (lRowSE0 != lRowSE1) {
               dataType *pMultiplierUp = c7x::strm_agen<0, dataType>::get_adv(pMultiplier);
               vec       multiplier0   = __vload_dup(pMultiplierUp);

               vec00 = c7x::strm_eng<0, vec>::get_adv();
               acc00 += (vec00 * multiplier0);

               vec01 = c7x::strm_eng<0, vec>::get_adv();
               acc01 += (vec01 * multiplier0);

               vec02 = c7x::strm_eng<0, vec>::get_adv();
               acc02 += (vec02 * multiplier0);

               vec03 = c7x::strm_eng<0, vec>::get_adv();
               acc03 += (vec03 * multiplier0);

               vec04 = c7x::strm_eng<0, vec>::get_adv();
               acc04 += (vec04 * multiplier0);

               vec05 = c7x::strm_eng<0, vec>::get_adv();
               acc05 += (vec05 * multiplier0);
            }
            acc00 += acc10;
            acc01 += acc11;
            acc02 += acc12;
            acc03 += acc13;
            acc04 += acc14;
            acc05 += acc15;

            // Using vec0x to hold A values
            vec00 = *(c7x::strm_agen<2, vec>::get_adv(ptrA));
            vec01 = *(c7x::strm_agen<2, vec>::get_adv(ptrA));
            vec02 = *(c7x::strm_agen<2, vec>::get_adv(ptrA));
            vec03 = *(c7x::strm_agen<2, vec>::get_adv(ptrA));
            vec04 = *(c7x::strm_agen<2, vec>::get_adv(ptrA));
            vec05 = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

            // Using vec1x to hold the difference
            vec10 = vec00 - acc00;
            vec11 = vec01 - acc01;
            vec12 = vec02 - acc02;
            vec13 = vec03 - acc03;
            vec14 = vec04 - acc04;
            vec15 = vec05 - acc05;

            if (tile_i == 0) {
               recipDiagValue = getRecipSqrt(vec10.s[0]);
            }

            __vpred vpred0    = c7x::strm_agen<1, vec>::get_vpred();
            vec    *storePtr0 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpred0, storePtr0, vec10 * recipDiagValue);

            __vpred vpred1    = c7x::strm_agen<1, vec>::get_vpred();
            vec    *storePtr1 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpred1, storePtr1, vec11 * recipDiagValue);

            __vpred vpred2    = c7x::strm_agen<1, vec>::get_vpred();
            vec    *storePtr2 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpred2, storePtr2, vec12 * recipDiagValue);

            __vpred vpred3    = c7x::strm_agen<1, vec>::get_vpred();
            vec    *storePtr3 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpred3, storePtr3, vec13 * recipDiagValue);

            __vpred vpred4    = c7x::strm_agen<1, vec>::get_vpred();
            vec    *storePtr4 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpred4, storePtr4, vec14 * recipDiagValue);

            __vpred vpred5    = c7x::strm_agen<1, vec>::get_vpred();
            vec    *storePtr5 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpred5, storePtr5, vec15 * recipDiagValue);
         }
      }
      else {

         lRowSE0              = (int32_t) (((uint32_t) row + 1U) >> 1U);
         lRowSE1              = (int32_t) (((uint32_t) row) >> 1U);
         seParamFetchU.ICNT0  = 1;
         seParamFetchU.ICNT1  = row;
         seParamFetchU.ICNT2  = 1;
         seParamFetchU.DIM1   = (yStride);
         seParamFetchU.DIM2   = 0;
         seParamFetchU.ELEDUP = SE_ELEDUP;

         seParamFetchD.ICNT0 = MIN_HORIZONTAL_COLUMNS_FOR_UNROLL * eleCount;
         seParamFetchD.ICNT1 = row;
         seParamFetchD.ICNT2 = 1;
         seParamFetchD.DIM1  = (yStride);
         seParamFetchD.DIM2  = 0;

         if (row > 0) {
            __SE0_OPEN(pLFirstRow, seParamFetchU);
            __SE1_OPEN(pLFirstRow, seParamFetchD);
         }

         acc00     = vec(0);
         acc01     = vec(0);
         acc10     = vec(0);
         acc11     = vec(0);
         vec acc20 = vec(0);
         vec acc21 = vec(0);
         vec acc30 = vec(0);
         vec acc31 = vec(0);

         vec vecMul0, vecMul1, vecMul2, vecMul3;
         vec vecMul4, vecMul5, vecMul6, vecMul7;
         vec vec0, vec1;
         vec vec2, vec3;
         vec vec4, vec5;
         vec vec6, vec7;
         for (lRow = 0; lRow < row; lRow += 4) {
            // UNROLL 1
            // SE0 fetch and duplicate
            vecMul0 = c7x::strm_eng<0, vec>::get();
            vec0    = c7x::strm_eng<1, vec>::get_adv();

            acc00 += (vec0 * vecMul0);

            vecMul1 = c7x::strm_eng<0, vec>::get_adv();
            vec1    = c7x::strm_eng<1, vec>::get_adv();
            acc01 += (vec1 * vecMul1);

            // UNROLL 2
            // SE0 fetch and duplicate
            vecMul2 = c7x::strm_eng<0, vec>::get();
            vec2    = c7x::strm_eng<1, vec>::get_adv();
            acc10 += (vec2 * vecMul2);

            vecMul3 = c7x::strm_eng<0, vec>::get_adv();
            vec3    = c7x::strm_eng<1, vec>::get_adv();
            acc11 += (vec3 * vecMul3);

            // UNROLL 3
            // SE0 fetch and duplicate
            vecMul4 = c7x::strm_eng<0, vec>::get();
            vec4    = c7x::strm_eng<1, vec>::get_adv();
            acc20 += (vec4 * vecMul4);

            vecMul5 = c7x::strm_eng<0, vec>::get_adv();
            vec5    = c7x::strm_eng<1, vec>::get_adv();
            acc21 += (vec5 * vecMul5);

            // UNROLL 4
            // SE0 fetch and duplicate
            vecMul6 = c7x::strm_eng<0, vec>::get();
            vec6    = c7x::strm_eng<1, vec>::get_adv();
            acc30 += (vec6 * vecMul6);

            vecMul7 = c7x::strm_eng<0, vec>::get_adv();
            vec7    = c7x::strm_eng<1, vec>::get_adv();
            acc31 += (vec7 * vecMul7);
         }
         acc00 += acc10;
         acc01 += acc11;

         vec accLTmp = acc20 + acc30;
         vec accRTmp = acc21 + acc31;

         acc00 += accLTmp;
         acc01 += accRTmp;

         // Using vec0x to hold A values
         vec00 = *(c7x::strm_agen<2, vec>::get_adv(ptrA));
         vec01 = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

         // Using vec1x to hold the difference
         vec10 = vec00 - acc00;
         vec11 = vec01 - acc01;

         recipDiagValue = getRecipSqrt(vec10.s[0]);

         __vpred vpred0    = c7x::strm_agen<1, vec>::get_vpred();
         vec    *storePtr0 = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpred0, storePtr0, vec10 * recipDiagValue);

         __vpred vpred1    = c7x::strm_agen<1, vec>::get_vpred();
         vec    *storePtr1 = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpred1, storePtr1, vec11 * recipDiagValue);
      }

      // Updating pointers for L, A and multiplier pointer
      ptrL += yStride + 1;
      ptrA += yStride + 1;
      pLFirstRow++;

      // Toggle to see if odd or even number of L rows to be fetched
      toggle = !toggle;
      validElemsPerRow--;
      tileWidthCeilValue--;
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();
   __SA3_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return DSPLIB_SUCCESS;
}

#else
#error invalid target
#endif

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_exec_ci(DSPLIB_kernelHandle handle,
                                      void *restrict pInA,
                                      void *restrict pOutU,
                                      void *restrict pMulBuffer)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;
   dataType                 *pLocalA      = (dataType *) pInA;
   dataType                 *pOutULocal   = (dataType *) pOutU;

   dataType                                              *pLocalMul   = (dataType *) pMulBuffer;
   uint8_t                                               *pBlock      = pKerPrivArgs->bufPblock;
   int32_t                                                order       = pKerPrivArgs->order;
   int32_t                                                enable_test = pKerPrivArgs->enableTest;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   DSPLIB_DEBUGPRINTFN(0, "pLocalA: %p pOutUocal: %p\n", pLocalA, pOutULocal);
   if (enable_test) {
      dataType sum = DSPLIB_cholesky_inplace_isPosDefinite(pLocalA, order, eleCount, pBlock);

      if (sum <= 0) {
         status = DSPLIB_ERR_FAILURE;
      }
      else {
         status = DSPLIB_cholesky_c7x_PingPong(enable_test, pKerPrivArgs, pLocalA, pOutULocal, pLocalMul);
      }
   }
   else {
      status = DSPLIB_cholesky_c7x_PingPong(enable_test, pKerPrivArgs, pLocalA, pOutULocal, pLocalMul);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                      void *restrict pInA,
                                                      void *restrict pOutU,
                                                      void *restrict pMulBuffer);

template DSPLIB_STATUS DSPLIB_cholesky_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                       void *restrict pInA,
                                                       void *restrict pOutU,
                                                       void *restrict pMulBuffer);
