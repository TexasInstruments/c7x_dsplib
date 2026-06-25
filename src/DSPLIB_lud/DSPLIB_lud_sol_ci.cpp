// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_sol_priv.h"

/*********************************************************************
 *
 * INITIALIZATION
 *
 *********************************************************************/

template <typename dataType> void DSPLIB_lud_sol_substitution_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_lud_sol_PrivArgs *pKerPrivArgs = (DSPLIB_lud_sol_PrivArgs *) handle;
   uint8_t                 *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                  order        = pKerPrivArgs->order;
   int32_t                  strideMat    = pKerPrivArgs->strideOrder;
   int32_t                  colMatstride = strideMat / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELEDUP  SE_ELEDUP  = c7x::se_eledup<dataType>::value;

   __SE_TEMPLATE_v1 seDiagReadParams                                = __gen_SE_TEMPLATE_v1();
   seDiagReadParams.ICNT0                                           = 1;
   seDiagReadParams.ICNT1                                           = order;
   seDiagReadParams.DIM1                                            = colMatstride + 1;
   seDiagReadParams.DIMFMT                                          = __SE_DIMFMT_2D;
   seDiagReadParams.ELETYPE                                         = SE_ELETYPE;
   seDiagReadParams.VECLEN                                          = __SE_VECLEN_1ELEM;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE)) = seDiagReadParams;

   __SA_TEMPLATE_v1 saWriteXParams                                  = __gen_SA_TEMPLATE_v1();
   saWriteXParams.ICNT0                                             = 1;
   saWriteXParams.ICNT1                                             = order;
   saWriteXParams.DIM1                                              = -1;
   saWriteXParams.DIMFMT                                            = __SA_DIMFMT_2D;
   saWriteXParams.VECLEN                                            = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE)) = saWriteXParams;

   __SE_TEMPLATE_v1 seDivReadParams                                 = __gen_SE_TEMPLATE_v1();
   seDivReadParams.ICNT0                                            = order;
   seDivReadParams.DIMFMT                                           = __SE_DIMFMT_1D;
   seDivReadParams.ELETYPE                                          = SE_ELETYPE;
   seDivReadParams.VECLEN                                           = SE_VECLEN;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = seDivReadParams;

   __SA_TEMPLATE_v1 saDivStoreParams                                = __gen_SA_TEMPLATE_v1();
   saDivStoreParams.ICNT0                                           = order;
   saDivStoreParams.DIMFMT                                          = __SA_DIMFMT_1D;
   saDivStoreParams.VECLEN                                          = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = saDivStoreParams;

   __SE_TEMPLATE_v1 seReadXParams                                   = __gen_SE_TEMPLATE_v1();
   seReadXParams.ICNT0                                              = 1;
   seReadXParams.DIM1                                               = -1;
   seReadXParams.DIMFMT                                             = __SE_DIMFMT_2D;
   seReadXParams.VECLEN                                             = SE_VECLEN;
   seReadXParams.ELETYPE                                            = SE_ELETYPE;
   seReadXParams.ELEDUP                                             = SE_ELEDUP;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE)) = seReadXParams;

   __SE_TEMPLATE_v1 seBlockParams                                   = __gen_SE_TEMPLATE_v1();
   seBlockParams.ICNT0                                              = eleCount;
   seBlockParams.DIM1                                               = -colMatstride;
   seBlockParams.DIMFMT                                             = __SE_DIMFMT_2D;
   seBlockParams.ELETYPE                                            = SE_ELETYPE;
   seBlockParams.VECLEN                                             = SE_VECLEN;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE)) = seBlockParams;

   __SA_TEMPLATE_v1 saReverseParams                                 = __gen_SA_TEMPLATE_v1();
   saReverseParams.ICNT0                                            = eleCount;
   saReverseParams.DIM1                                             = -((int32_t) eleCount);
   saReverseParams.DIMFMT                                           = __SA_DIMFMT_2D;
   saReverseParams.VECLEN                                           = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE)) = saReverseParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_sol_substitution_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_lud_sol_substitution_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_lud_sol_permuteB_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_lud_sol_PrivArgs *pKerPrivArgs = (DSPLIB_lud_sol_PrivArgs *) handle;
   uint8_t                 *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                  order        = pKerPrivArgs->order;

   typedef typename c7x::make_full_vector<uint16_t>::type vecUINT16;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vecUINT16>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vecUINT16>::value;
   int32_t                                                pStride    = pKerPrivArgs->strideP;
   int32_t                                                colPStride = pStride / sizeof(uint16_t);

   __SE_TEMPLATE_v1 seMatReadParams = __gen_SE_TEMPLATE_v1();
   seMatReadParams.ICNT0            = order;
   seMatReadParams.DIM1             = colPStride * 2;
   seMatReadParams.DIMFMT           = __SE_DIMFMT_2D;
   seMatReadParams.ELETYPE          = SE_ELETYPE;
   seMatReadParams.VECLEN           = SE_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE)) = seMatReadParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_sol_permuteB_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_lud_sol_permuteB_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType>
DSPLIB_STATUS DSPLIB_lud_sol_init_ci(DSPLIB_kernelHandle           handle,
                                     const DSPLIB_bufParams2D_t   *bufParamsP,
                                     const DSPLIB_bufParams2D_t   *bufParamsL,
                                     const DSPLIB_bufParams2D_t   *bufParamsU,
                                     const DSPLIB_bufParams1D_t   *bufParamsB,
                                     const DSPLIB_bufParams1D_t   *bufParamsX,
                                     const DSPLIB_bufParams2D_t   *bufParamsVecScratch,
                                     const DSPLIB_bufParams2D_t   *bufParamsScratchTrans,
                                     const DSPLIB_lud_solInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_lud_sol_PrivArgs  *pKerPrivArgs         = (DSPLIB_lud_sol_PrivArgs *) handle;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs = &pKerPrivArgs->pMatTransKerPrivArgs;

   DSPLIB_matTransInitArgs kerInitArgsMatTrans;

   kerInitArgsMatTrans.funcStyle = pKerInitArgs->funcStyle;
   kerInitArgsMatTrans.dimX      = bufParamsU->dim_x;
   kerInitArgsMatTrans.dimY      = bufParamsU->dim_y;

   pMatTransKerPrivArgs->widthIn   = bufParamsU->dim_x;
   pMatTransKerPrivArgs->heightIn  = bufParamsU->dim_y;
   pMatTransKerPrivArgs->strideIn  = bufParamsU->stride_y;
   pMatTransKerPrivArgs->strideOut = bufParamsScratchTrans->stride_y;

   DSPLIB_matTrans_init_ci<dataType>(pMatTransKerPrivArgs, bufParamsU, bufParamsScratchTrans, &kerInitArgsMatTrans);
   DSPLIB_lud_sol_substitution_init_ci<dataType>(handle);
   DSPLIB_lud_sol_permuteB_init_ci<dataType>(handle);
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_lud_sol_init_ci<float>(DSPLIB_kernelHandle           handle,
                                                     const DSPLIB_bufParams2D_t   *bufParamsP,
                                                     const DSPLIB_bufParams2D_t   *bufParamsL,
                                                     const DSPLIB_bufParams2D_t   *bufParamsU,
                                                     const DSPLIB_bufParams1D_t   *bufParamsB,
                                                     const DSPLIB_bufParams1D_t   *bufParamsX,
                                                     const DSPLIB_bufParams2D_t   *bufParamsVecScratch,
                                                     const DSPLIB_bufParams2D_t   *bufParamsScratchTrans,
                                                     const DSPLIB_lud_solInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_lud_sol_init_ci<double>(DSPLIB_kernelHandle           handle,
                                                      const DSPLIB_bufParams2D_t   *bufParamsP,
                                                      const DSPLIB_bufParams2D_t   *bufParamsL,
                                                      const DSPLIB_bufParams2D_t   *bufParamsU,
                                                      const DSPLIB_bufParams1D_t   *bufParamsB,
                                                      const DSPLIB_bufParams1D_t   *bufParamsX,
                                                      const DSPLIB_bufParams2D_t   *bufParamsVecScratch,
                                                      const DSPLIB_bufParams2D_t   *bufParamsScratchTrans,
                                                      const DSPLIB_lud_solInitArgs *pKerInitArgs);

/*********************************************************************
 *
 * IMPLEMENTATION
 *
 *********************************************************************/
template <typename dataType>
void DSPLIB_lud_sol_permuteB_ci(unsigned short *pIn,
                                dataType       *B,
                                dataType       *B_Mod,
                                int32_t         order,
                                int32_t         colPStride,
                                uint32_t       *permuteOrder,
                                uint8_t        *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<uint16_t>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 se0Params, se1Params;
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));

   int32_t nVec     = DSPLIB_ceilingDiv(order, eleCount);
   int32_t se1ICNT1 = order / 2;
   int32_t se0ICNT1 = order - se1ICNT1;

   se0Params.ICNT1 = se0ICNT1;
   se1Params.ICNT1 = se1ICNT1;

   __SE0_OPEN(pIn, se0Params);

   vec vecZero = (vec) 0;
   vec vecOne  = (vec) 1;

   vec idx_0_to_eleCount;

#if (__C7X_VEC_SIZE_BITS__ == 256)
   idx_0_to_eleCount = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
#elif (__C7X_VEC_SIZE_BITS__ == 512)
   idx_0_to_eleCount = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                                       23, 24, 25, 26, 27, 28, 29, 30, 31);
#endif

   int32_t vertical = 0;
   if (se1ICNT1 > 0) {
      __SE1_OPEN(pIn + colPStride, se1Params);

      for (vertical = 0; vertical < order - 1; vertical += 2) {

         vec maxValVec1 = (vec) 0;
         vec maxValVec2 = (vec) 0;
         vec vMaxIdx1; // = idx_0_to_eleCount;
         vec vMaxIdx2; // = idx_0_to_eleCount;
         vec vCurrIdx1 = idx_0_to_eleCount;
         vec vCurrIdx2 = idx_0_to_eleCount;

         for (int32_t horizontal = 0; horizontal < nVec; horizontal++) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            __vpred cmpPred1 = __cmp_eq_pred(vecZero, v1);
            __vpred cmpPred2 = __cmp_eq_pred(vecZero, v2);

            maxValVec1 = __select(cmpPred1, maxValVec1, v1);
            maxValVec2 = __select(cmpPred2, maxValVec2, v2);

            vMaxIdx1 = __select(cmpPred1, vMaxIdx1, vCurrIdx1);
            vMaxIdx2 = __select(cmpPred2, vMaxIdx2, vCurrIdx2);

            vCurrIdx1 = vCurrIdx1 + (uint16_t) eleCount;
            vCurrIdx2 = vCurrIdx2 + (uint16_t) eleCount;
         }

         __vpred  cmpPredFinal1 = __cmp_eq_pred(vecOne, maxValVec1);
         uint32_t tempIdx1      = __rightmost_bit_detect_short(cmpPredFinal1) >> 1;
         uint32_t finalIdx1     = __vgetuh_vrd(vMaxIdx1, tempIdx1);

         __vpred  cmpPredFinal2 = __cmp_eq_pred(vecOne, maxValVec2);
         uint32_t tempIdx2      = __rightmost_bit_detect_short(cmpPredFinal2) >> 1;
         uint32_t finalIdx2     = __vgetuh_vrd(vMaxIdx2, tempIdx2);

         B_Mod[vertical + 0] = B[finalIdx1];
         B_Mod[vertical + 1] = B[finalIdx2];
      }
   }

   if (se0ICNT1 != se1ICNT1) {

      vec maxValVec1 = (vec) 0;
      vec vMaxIdx1   = idx_0_to_eleCount;
      vec vCurrIdx1  = idx_0_to_eleCount;

      for (int32_t horizontal = 0; horizontal < nVec; horizontal++) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();

         __vpred cmpPred1 = __cmp_eq_pred(vecZero, v1);

         maxValVec1 = __select(cmpPred1, maxValVec1, v1);

         vMaxIdx1 = __select(cmpPred1, vMaxIdx1, vCurrIdx1);

         vCurrIdx1 = vCurrIdx1 + (uint16_t) eleCount;
      }

      __vpred  cmpPredFinal1 = __cmp_eq_pred(vecOne, maxValVec1);
      uint32_t tempIdx1      = __rightmost_bit_detect_short(cmpPredFinal1) >> 1;
      uint32_t finalIdx1     = __vgetuh_vrd(vMaxIdx1, tempIdx1);

      B_Mod[vertical + 0] = B[finalIdx1];
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}

template void DSPLIB_lud_sol_permuteB_ci<float>(unsigned short *pIn,
                                                float          *B,
                                                float          *B_Mod,
                                                int32_t         order,
                                                int32_t         colPStride,
                                                uint32_t       *permuteOrder,
                                                uint8_t        *pBlock);
template void DSPLIB_lud_sol_permuteB_ci<double>(unsigned short *pIn,
                                                 double         *B,
                                                 double         *B_Mod,
                                                 int32_t         order,
                                                 int32_t         colPStride,
                                                 uint32_t       *permuteOrder,
                                                 uint8_t        *pBlock);

template <typename dataType, typename V = typename c7x::make_full_vector<dataType>::type>
inline void                       getElement(V inVec, uint32_t index, dataType *element);
template <typename V> inline void getElement(V inVec, uint32_t index, float *element)
{
   *element = __as_float(__vgetw_vrd(c7x::as_int_vec(inVec), index));
}

template <typename V> inline void getElement(V inVec, uint32_t index, double *element)
{
   *element = __as_double(__vgetd_vrd(c7x::as_long_vec(inVec), index));
}

template <typename dataType>
static inline void DSPLIB_lud_sol_forwardSubstitution_ci(dataType *pL,
                                                         dataType *pY,
                                                         dataType *pB,
                                                         dataType *pDiv,
                                                         int32_t   order,
                                                         int32_t   colLstride,
                                                         uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 seDivReadParams;
   __SA_TEMPLATE_v1 saDivStoreParams;
   __SE_TEMPLATE_v1 seDiagReadParams;
   __SA_TEMPLATE_v1 saDiagWriteParams;
   seDiagReadParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   saDiagWriteParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   seDivReadParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   saDivStoreParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));

   saDiagWriteParams.DIM1 = 1;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   int32_t                                                nVec     = DSPLIB_ceilingDiv(order, eleCount);

   /* Calculate reciprocals of Diagonal Elements */

   __SE1_OPEN(pL, seDiagReadParams);
   __SA1_OPEN(saDiagWriteParams);
   int32_t row = 0;

   for (row = 0; row < order; row++) {
      vec vecDiag = c7x::strm_eng<1, vec>::get_adv();

      __vpred predDiag   = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreDiag = c7x::strm_agen<1, vec>::get_adv(pDiv);
      __vstore_pred(predDiag, pStoreDiag, vecDiag);
   }
   __SE1_CLOSE();
   __SA1_CLOSE();

   __SE0_OPEN(pDiv, seDivReadParams);
   __SA0_OPEN(saDivStoreParams);
   dataType TwoP0 = 2.0;
   int32_t  ii    = 0;

   for (ii = 0; ii < nVec - 3; ii += 4) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<0, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<0, vec>::get_adv();

      vec yy1 = __recip(v1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);

      vec yy2 = __recip(v2);
      yy2     = yy2 * (TwoP0 - v2 * yy2);
      yy2     = yy2 * (TwoP0 - v2 * yy2);

      vec yy3 = __recip(v3);
      yy3     = yy3 * (TwoP0 - v3 * yy3);
      yy3     = yy3 * (TwoP0 - v3 * yy3);

      vec yy4 = __recip(v4);
      yy4     = yy4 * (TwoP0 - v4 * yy4);
      yy4     = yy4 * (TwoP0 - v4 * yy4);

      __vpred predDiv1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv1 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv1, pStoreDiv1, yy1);

      __vpred predDiv2   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv2 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv2, pStoreDiv2, yy2);

      __vpred predDiv3   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv3 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv3, pStoreDiv3, yy3);

      __vpred predDiv4   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv4 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv4, pStoreDiv4, yy4);
   }

   for (; ii < nVec; ii++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();

      vec yy1 = __recip(v1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);

      __vpred predDiv1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv1 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv1, pStoreDiv1, yy1);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   /*----------------------------------------------------------------------
    Forward Substitution
    -----------------------------------------------------------------------*/

   __SE_TEMPLATE_v1 seBlockParams;
   __SE_TEMPLATE_v1 seReadYParams;
   __SA_TEMPLATE_v1 saWriteYParams;
   __SA_TEMPLATE_v1 sa1DReadParams;

   saWriteYParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   seReadYParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   seBlockParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   sa1DReadParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   saWriteYParams.DIM1 = 1;
   seReadYParams.DIM1  = 1;
   seBlockParams.DIM1  = colLstride;

   dataType *pSE0 = pL;
   dataType *pSA1 = pY;
   dataType *pSA2 = pB;
   dataType *pSA3 = pDiv;

   __SA1_OPEN(saWriteYParams);
   __SA2_OPEN(sa1DReadParams);
   __SA3_OPEN(sa1DReadParams);

   for (int32_t block = 0; block < nVec; block++) {
      __vpred predB  = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pLoadB = c7x::strm_agen<2, vec>::get_adv(pSA2);
      vec     vecB   = __vload_pred(predB, pLoadB);

      __vpred predDiv  = c7x::strm_agen<3, vec>::get_vpred();
      vec    *pLoadDiv = c7x::strm_agen<3, vec>::get_adv(pSA3);
      vec     vecDiv   = __vload_pred(predDiv, pLoadDiv);

      int32_t sumRows   = block * eleCount;
      int32_t totalRows = sumRows + eleCount;

      seBlockParams.ICNT1 = totalRows;
      seReadYParams.ICNT1 = sumRows;

      __SE0_OPEN(pSE0, seBlockParams);
      if (sumRows > 0) {
         __SE1_OPEN(pY, seReadYParams);
      }

      /* Calculate sum */
      vec     vecSum   = (vec) 0;
      vec     vecSum1  = (vec) 0;
      vec     vecSum2  = (vec) 0;
      vec     vecSum3  = (vec) 0;
      vec     vecSum4  = (vec) 0;
      int32_t vertical = 0;
      /* Square Part- Accumulate sum */ /* 7 + trip_cnt * 4 */
      for (vertical = 0; vertical < sumRows - 3; vertical += 4) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec y1 = c7x::strm_eng<1, vec>::get_adv();
         vecSum1 += v1 * y1;

         vec v2 = c7x::strm_eng<0, vec>::get_adv();
         vec y2 = c7x::strm_eng<1, vec>::get_adv();
         vecSum2 += v2 * y2;

         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec y3 = c7x::strm_eng<1, vec>::get_adv();
         vecSum3 += v3 * y3;

         vec v4 = c7x::strm_eng<0, vec>::get_adv();
         vec y4 = c7x::strm_eng<1, vec>::get_adv();
         vecSum4 += v4 * y4;
      }

      vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;

      /* Diagonal Part- Calculate values */ /* 1 + tri_cnt *21 = 337 */
      dataType resultEle1;
      for (vertical = 0; vertical < eleCount; vertical++) {
         vec v1      = c7x::strm_eng<0, vec>::get_adv();
         vec result1 = (vecB - vecSum) * vecDiv;

         getElement(result1, vertical, &resultEle1);

         vecSum += v1 * (resultEle1);

         __vpred predYCalc = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreY   = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(predYCalc, pStoreY, (vec) resultEle1);
      }

      pSE0 += eleCount;
      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   __SA2_CLOSE();
   __SA3_CLOSE();
   __SA1_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_lud_sol_forwardSubstitution_ci<float>(float   *pL,
                                                           float   *pX,
                                                           float   *pY,
                                                           float   *pDiv,
                                                           int32_t  order,
                                                           int32_t  colLstride,
                                                           uint8_t *pBlock);
template void DSPLIB_lud_sol_forwardSubstitution_ci<double>(double  *pL,
                                                            double  *pX,
                                                            double  *pY,
                                                            double  *pDiv,
                                                            int32_t  order,
                                                            int32_t  colLstride,
                                                            uint8_t *pBlock);

template <typename dataType>
static inline void DSPLIB_lud_sol_backSubstitution_ci(dataType *pL,
                                                      dataType *pX,
                                                      dataType *pY,
                                                      dataType *pDiv,
                                                      int32_t   order,
                                                      int32_t   colLstride,
                                                      uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   /* Calculate reciprocals of Diagonal Elements */

   __SE_TEMPLATE_v1 seDivReadParams;
   __SA_TEMPLATE_v1 saDivStoreParams;
   __SE_TEMPLATE_v1 seDiagReadParams;
   __SA_TEMPLATE_v1 saDiagWriteParams;
   seDiagReadParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   saDiagWriteParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   seDivReadParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   saDivStoreParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));

   saDiagWriteParams.DIM1 = 1;

   int32_t nVec = DSPLIB_ceilingDiv(order, eleCount);

   /* Calculate reciprocals of Diagonal Elements */
   __SE1_OPEN(pL, seDiagReadParams);
   __SA1_OPEN(saDiagWriteParams);
   int32_t row = 0;

   for (row = 0; row < order; row++) {
      vec vecDiag = c7x::strm_eng<1, vec>::get_adv();

      __vpred predDiag   = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreDiag = c7x::strm_agen<1, vec>::get_adv(pDiv);
      __vstore_pred(predDiag, pStoreDiag, vecDiag);
   }
   __SE1_CLOSE();
   __SA1_CLOSE();

   __SE0_OPEN(pDiv, seDivReadParams);
   __SA0_OPEN(saDivStoreParams);
   dataType TwoP0 = 2.0;
   int32_t  ii    = 0;

   for (ii = 0; ii < nVec - 3; ii += 4) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<0, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<0, vec>::get_adv();

      vec yy1 = __recip(v1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);

      vec yy2 = __recip(v2);
      yy2     = yy2 * (TwoP0 - v2 * yy2);
      yy2     = yy2 * (TwoP0 - v2 * yy2);

      vec yy3 = __recip(v3);
      yy3     = yy3 * (TwoP0 - v3 * yy3);
      yy3     = yy3 * (TwoP0 - v3 * yy3);

      vec yy4 = __recip(v4);
      yy4     = yy4 * (TwoP0 - v4 * yy4);
      yy4     = yy4 * (TwoP0 - v4 * yy4);

      __vpred predDiv1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv1 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv1, pStoreDiv1, yy1);

      __vpred predDiv2   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv2 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv2, pStoreDiv2, yy2);

      __vpred predDiv3   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv3 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv3, pStoreDiv3, yy3);

      __vpred predDiv4   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv4 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv4, pStoreDiv4, yy4);
   }

   for (; ii < nVec; ii++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();

      vec yy1 = __recip(v1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);

      __vpred predDiv1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv1 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv1, pStoreDiv1, yy1);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   int32_t totalBlocks  = order / eleCount;
   int32_t remainingEle = order - (totalBlocks * eleCount);

   __SE_TEMPLATE_v1 seBlockParams;
   __SE_TEMPLATE_v1 seReadXParams;
   __SA_TEMPLATE_v1 saWriteXParams;
   __SA_TEMPLATE_v1 saReverseParams;

   saWriteXParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   seReadXParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   seBlockParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   saReverseParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));

   seReadXParams.ICNT0   = 1;
   seReadXParams.DIM1    = -1;
   saReverseParams.ICNT1 = totalBlocks;

   dataType *pLLastElem = &pL[(order - 1) + ((order - 1) * colLstride)];
   dataType *pXLastElem = &pX[order - 1];
   dataType *pSE0       = pLLastElem - (eleCount - 1);
   dataType *pSA1       = pX + order - 1;
   dataType *pSA2       = pY + order - eleCount;
   dataType *pSA3       = pDiv + order - eleCount;

   __SA1_OPEN(saWriteXParams);

   if (totalBlocks > 0) {
      __SA2_OPEN(saReverseParams);
      __SA3_OPEN(saReverseParams);

      for (int32_t block = 0; block < totalBlocks; block++) {
         __vpred predY  = c7x::strm_agen<2, vec>::get_vpred();
         vec    *pLoadY = c7x::strm_agen<2, vec>::get_adv(pSA2);
         vec     vecY   = __vload_pred(predY, pLoadY);

         __vpred predDiv  = c7x::strm_agen<3, vec>::get_vpred();
         vec    *pLoadDiv = c7x::strm_agen<3, vec>::get_adv(pSA3);
         vec     vecDiv   = __vload_pred(predDiv, pLoadDiv);

         int32_t sumRows   = block * eleCount;
         int32_t totalRows = sumRows + eleCount;

         seBlockParams.ICNT1 = totalRows;
         seReadXParams.ICNT1 = sumRows;

         __SE0_OPEN(pSE0, seBlockParams);
         if (sumRows > 0) {
            __SE1_OPEN(pXLastElem, seReadXParams);
         }

         /* Calculate sum */
         vec     vecSum   = (vec) 0;
         vec     vecSum1  = (vec) 0;
         vec     vecSum2  = (vec) 0;
         vec     vecSum3  = (vec) 0;
         vec     vecSum4  = (vec) 0;
         int32_t vertical = 0;

         for (vertical = 0; vertical < sumRows - 3; vertical += 4) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec x1 = c7x::strm_eng<1, vec>::get_adv();
            vecSum1 += v1 * x1;

            vec v2 = c7x::strm_eng<0, vec>::get_adv();
            vec x2 = c7x::strm_eng<1, vec>::get_adv();
            vecSum2 += v2 * x2;

            vec v3 = c7x::strm_eng<0, vec>::get_adv();
            vec x3 = c7x::strm_eng<1, vec>::get_adv();
            vecSum3 += v3 * x3;

            vec v4 = c7x::strm_eng<0, vec>::get_adv();
            vec x4 = c7x::strm_eng<1, vec>::get_adv();
            vecSum4 += v4 * x4;
         }

         vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;

         /* Calculate values */
         uint32_t vecIndex = eleCount - 1;
         dataType resultEle1;

         for (vertical = 0; vertical < eleCount; vertical++) {
            vec v1      = c7x::strm_eng<0, vec>::get_adv();
            vec result1 = (vecY - vecSum) * vecDiv;
            getElement(result1, vecIndex--, &resultEle1);
            vecSum += v1 * (resultEle1);
            dataType *pStoreX1 = c7x::strm_agen<1, dataType>::get_adv(pSA1);
            *pStoreX1          = resultEle1;
         }

         pSE0 -= eleCount;
         __SE0_CLOSE();
         __SE1_CLOSE();
      }
      __SA2_CLOSE();
      __SA3_CLOSE();
   }
   /* Processing Remaining Elements */
   if (remainingEle > 0) {
      seBlockParams.ICNT0 = remainingEle;
      seBlockParams.ICNT1 = order;

      seReadXParams.ICNT1 = order;

      saReverseParams.ICNT0 = remainingEle;
      saReverseParams.ICNT1 = 1;
      saReverseParams.DIM1  = 0;

      pSE0 = &pL[(order - 1) * colLstride];
      __SE0_OPEN(pSE0, seBlockParams);
      __SE1_OPEN(pXLastElem, seReadXParams);
      __SA2_OPEN(saReverseParams);
      __SA3_OPEN(saReverseParams);

      int32_t sumRows = totalBlocks * eleCount;

      __vpred predY  = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pLoadY = c7x::strm_agen<2, vec>::get_adv(pY);
      vec     vecY   = __vload_pred(predY, pLoadY);

      __vpred predDiv  = c7x::strm_agen<3, vec>::get_vpred();
      vec    *pLoadDiv = c7x::strm_agen<3, vec>::get_adv(pDiv);
      vec     vecDiv   = __vload_pred(predDiv, pLoadDiv);

      vec     vecSum   = (vec) 0;
      vec     vecSum1  = (vec) 0;
      vec     vecSum2  = (vec) 0;
      vec     vecSum3  = (vec) 0;
      vec     vecSum4  = (vec) 0;
      int32_t vertical = 0;

      for (vertical = 0; vertical < sumRows - 3; vertical += 4) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec x1 = c7x::strm_eng<1, vec>::get_adv();
         vecSum1 += v1 * x1;

         vec v2 = c7x::strm_eng<0, vec>::get_adv();
         vec x2 = c7x::strm_eng<1, vec>::get_adv();
         vecSum2 += v2 * x2;

         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec x3 = c7x::strm_eng<1, vec>::get_adv();
         vecSum3 += v3 * x3;

         vec v4 = c7x::strm_eng<0, vec>::get_adv();
         vec x4 = c7x::strm_eng<1, vec>::get_adv();
         vecSum4 += v4 * x4;
      }

      vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;

      int32_t  vecIndex = remainingEle - 1;
      dataType resultEle;

      for (vertical = 0; vertical < remainingEle; vertical++) {
         vec v1     = c7x::strm_eng<0, vec>::get_adv();
         vec result = (vecY - vecSum) * vecDiv;

         getElement(result, vecIndex--, &resultEle);
         vecSum += v1 * (resultEle);

         dataType *pStoreX = c7x::strm_agen<1, dataType>::get_adv(pSA1);
         *pStoreX          = resultEle;
      }

      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA2_CLOSE();
      __SA3_CLOSE();
   }
   __SA1_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_lud_sol_backSubstitution_ci<float>(float   *pL,
                                                        float   *pX,
                                                        float   *pY,
                                                        float   *pDiv,
                                                        int32_t  order,
                                                        int32_t  colLstride,
                                                        uint8_t *pBlock);
template void DSPLIB_lud_sol_backSubstitution_ci<double>(double  *pL,
                                                         double  *pX,
                                                         double  *pY,
                                                         double  *pDiv,
                                                         int32_t  order,
                                                         int32_t  colLstride,
                                                         uint8_t *pBlock);

template <typename dataType>
int DSPLIB_lud_sol_ci(DSPLIB_lud_sol_PrivArgs *pKerPrivArgs,
                      unsigned short          *P,
                      dataType                *L,
                      dataType                *U,
                      dataType                *B,
                      dataType                *B_Mod,
                      dataType                *Y,
                      dataType                *X,
                      dataType                *pDiv,
                      dataType                *pScratchTrans)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs = &pKerPrivArgs->pMatTransKerPrivArgs;

   int32_t  order       = pKerPrivArgs->order;
   int32_t  strideOrder = pKerPrivArgs->strideOrder;
   int32_t  strideP     = pKerPrivArgs->strideP;
   uint8_t *pBlock      = pKerPrivArgs->bufPblock;

   int32_t dataSize  = sizeof(dataType);
   int32_t dataSizeP = sizeof(unsigned short);

   int32_t orderStride  = strideOrder / dataSize;
   int32_t orderPStride = strideP / dataSizeP;

   /* -----------------------------------------------------------------------
    modify b based on permutation matrix P
    ------------------------------------------------------------------------- */
   DSPLIB_lud_sol_permuteB_ci<dataType>(P, B, B_Mod, order, orderPStride, NULL, pBlock);

   /*-----------------------------------------------------------------------
   solve L*y=b for y using forward substitution
   -------------------------------------------------------------------------*/

   DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs, L, pScratchTrans);

   DSPLIB_lud_sol_forwardSubstitution_ci<dataType>(pScratchTrans, Y, B_Mod, pDiv, order, orderStride, pBlock);

   /* -----------------------------------------------------------------------------
    solve U*X=y for x using backward substitution
    ------------------------------------------------------------------------------ */

   DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs, U, pScratchTrans);
   DSPLIB_lud_sol_backSubstitution_ci<dataType>(pScratchTrans, X, Y, pDiv, order, orderStride, pBlock);

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return 0;
}

template int DSPLIB_lud_sol_ci<float>(DSPLIB_lud_sol_PrivArgs *pKerPrivArgs,
                                      unsigned short          *P,
                                      float                   *L,
                                      float                   *U,
                                      float                   *B,
                                      float                   *B_Mod,
                                      float                   *Y,
                                      float                   *X,
                                      float                   *pDiv,
                                      float                   *pScratchTrans);
template int DSPLIB_lud_sol_ci<double>(DSPLIB_lud_sol_PrivArgs *pKerPrivArgs,
                                       unsigned short          *P,
                                       double                  *L,
                                       double                  *U,
                                       double                  *B,
                                       double                  *B_Mod,
                                       double                  *Y,
                                       double                  *X,
                                       double                  *pDiv,
                                       double                  *pScratchTrans);

template <typename dataType>
DSPLIB_STATUS DSPLIB_lud_sol_exec_ci(DSPLIB_kernelHandle handle,
                                     void *restrict pP,
                                     void *restrict pL,
                                     void *restrict pU,
                                     void *restrict pB,
                                     void *restrict pX,
                                     void *restrict pVecScratch,
                                     void *restrict pScratchTrans)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS            status       = DSPLIB_SUCCESS;
   DSPLIB_lud_sol_PrivArgs *pKerPrivArgs = (DSPLIB_lud_sol_PrivArgs *) handle;

   int32_t strideVec = pKerPrivArgs->strideVec;

   /* Typecast void pointers to respective data type */
   unsigned short *pPLocal     = (unsigned short *) pP;
   dataType       *pLLocal     = (dataType *) pL;
   dataType       *pULocal     = (dataType *) pU;
   dataType       *pBLocal     = (dataType *) pB;
   dataType       *pB_ModLocal = (dataType *) (pVecScratch) + (0 * strideVec / sizeof(dataType));
   dataType       *pYLocal     = (dataType *) (pVecScratch) + (1 * strideVec / sizeof(dataType));
   dataType       *pXLocal     = (dataType *) pX;
   dataType       *pDivLocal   = (dataType *) (pVecScratch) + (2 * strideVec / sizeof(dataType));
   dataType       *pTransLocal = (dataType *) pScratchTrans;

   DSPLIB_DEBUGPRINTFN(
       0, "pPLocal: %p pLLocal: %p pULocal: %p pBLocal: %p  pB_ModLocal: %p  pYLocal: %p  pXLocal: %p  order: %d\n",
       pPLocal, pLLocal, pULocal, pBLocal, pB_ModLocal, pYLocal, pXLocal, pKerPrivArgs->order);

   DSPLIB_lud_sol_ci<dataType>(pKerPrivArgs, pPLocal, pLLocal, pULocal, pBLocal, pB_ModLocal, pYLocal, pXLocal,
                               pDivLocal, pTransLocal);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_lud_sol_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                     void *restrict pP,
                                                     void *restrict pL,
                                                     void *restrict pU,
                                                     void *restrict pB,
                                                     void *restrict pX,
                                                     void *restrict pVecScratch,
                                                     void *restrict pScratchTrans);

template DSPLIB_STATUS DSPLIB_lud_sol_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                      void *restrict pP,
                                                      void *restrict pL,
                                                      void *restrict pU,
                                                      void *restrict pB,
                                                      void *restrict pX,
                                                      void *restrict pVecScratch,
                                                      void *restrict pScratchTrans);
