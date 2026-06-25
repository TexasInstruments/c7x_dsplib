// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/DSPLIB_inlines.h"
#include "DSPLIB_minerror_priv.h"
#include <c7x_scalable.h>
#include <float.h>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define CURR_IDX_VEC_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)

template <typename dataType>
DSPLIB_STATUS DSPLIB_minerror_init_ci(DSPLIB_kernelHandle             handle,
                                      const DSPLIB_bufParams2D_t     *bufParamsIn,
                                      const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                      const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                      const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                      const DSPLIB_minerror_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   uint8_t *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t vecInSize    = pKerPrivArgs->vecInSize;
   uint32_t errCoefsSize = pKerPrivArgs->errCoefsSize;
   uint32_t strideIn     = pKerPrivArgs->strideIn;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   SE_VECLEN                                                       = c7x::se_veclen<vec>::value;
   SE_ELETYPE                                                      = c7x::se_eletype<vec>::value;

   int32_t dataSize = 8;

   DSPLIB_DEBUGPRINTFN(0, "Enter eleCount %d datasize %d\n", eleCount, dataSize);

   // Initialize current index with promoted/conversion datatype
   if (bufParamsIn->data_type == DSPLIB_INT64 || bufParamsIn->data_type == DSPLIB_UINT64) {
      pKerPrivArgs->mainLoopCount = (int32_t) (((vecInSize + (eleCount * 2) - 1) / (eleCount * 2)));
      double *vCurrIdx            = (double *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET);

      for (int j = 0; j < eleCount; j++) {
         vCurrIdx[j] = j;
      }
   }
   else if (bufParamsIn->data_type == DSPLIB_INT32 || bufParamsIn->data_type == DSPLIB_UINT32) {
      pKerPrivArgs->mainLoopCount = (int32_t) (((vecInSize + (eleCount * 2) - 1) / (eleCount * 2)) * 2);
      int64_t *vCurrIdx           = (int64_t *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET);

      for (int j = 0; j < eleCount / 2; j++) {
         vCurrIdx[j] = j;
      }
   }
   else if (bufParamsIn->data_type == DSPLIB_INT16 || bufParamsIn->data_type == DSPLIB_UINT16) {
      pKerPrivArgs->mainLoopCount = (int32_t) (((vecInSize + eleCount - 1) / eleCount) * 2);
      int32_t *vCurrIdx           = (int32_t *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET);

      for (int j = 0; j < eleCount / 2; j++) {
         vCurrIdx[j] = j;
      }
   }
   else if (bufParamsIn->data_type == DSPLIB_INT8 || bufParamsIn->data_type == DSPLIB_UINT8) {

#if __C7X_VEC_SIZE_BITS__ == 512
      int32_t outEleCount = eleCount / 2;
#else
      int32_t outEleCount = eleCount;
#endif
      pKerPrivArgs->mainLoopCount = (int32_t) (((vecInSize + outEleCount - 1) / outEleCount));
      dataType *vCurrIdx          = (dataType *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET);

      for (int j = 0; j < eleCount; j++) {
         vCurrIdx[j] = j;
      }
   }
   else {
      dataType *vCurrIdx          = (dataType *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET);
      pKerPrivArgs->mainLoopCount = (int32_t) ((vecInSize + (eleCount * 2) - 1) / (eleCount * 2));

      for (int j = 0; j < eleCount; j++) {
         vCurrIdx[j] = j;
      }
   }

   se0Params = __gen_SE_TEMPLATE_v1();
   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch the input */
   /**********************************************************************/
   /* SET TRANSPOSE BOUNDARY */
   if (bufParamsIn->data_type == DSPLIB_INT8 || bufParamsIn->data_type == DSPLIB_UINT8 ||
       bufParamsIn->data_type == DSPLIB_INT16 || bufParamsIn->data_type == DSPLIB_UINT16 ||
       bufParamsIn->data_type == DSPLIB_INT32 || bufParamsIn->data_type == DSPLIB_UINT32 ||
       bufParamsIn->data_type == DSPLIB_FLOAT32) {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   /* INITIALIZE SE PARAMETERS */
   if (bufParamsIn->data_type == DSPLIB_INT64 || bufParamsIn->data_type == DSPLIB_UINT64 ||
       bufParamsIn->data_type == DSPLIB_FLOAT32 || bufParamsIn->data_type == DSPLIB_FLOAT64) {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         dataSize = 4;
      }

      int32_t temp;
      se0Params.ICNT0 = errCoefsSize; // col; // reach till end of columns
      se0Params.ICNT1 = temp =
          (vecInSize > (uint32_t) eleCount)
              ? eleCount
              : vecInSize; //(dataSize > eleCount) ? eleCount : dataSize; // total elements that can be read in columns
      se0Params.DIM1 = strideIn / dataSize; // col; // upto col

      se0Params.ICNT2 = pKerPrivArgs->mainLoopCount; // need row/eleCount complete sweep of columns.

      se0Params.DIM2    = (strideIn / dataSize * temp * 2); // total elements in one sweep of columns
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DIMFMT  = __SE_DIMFMT_3D;
   }
   else if (bufParamsIn->data_type == DSPLIB_INT32 || bufParamsIn->data_type == DSPLIB_UINT32) {
      dataSize = 4;
      int32_t temp;
      se0Params.ICNT0 = errCoefsSize; // col; // reach till end of columns
      se0Params.ICNT1 = temp =
          (vecInSize > (uint32_t) eleCount / 2)
              ? eleCount / 2
              : vecInSize; //(dataSize > eleCount) ? eleCount : dataSize; // total elements that can be read in columns
      se0Params.DIM1 = strideIn / dataSize; // col; // upto col

      se0Params.ICNT2 = pKerPrivArgs->mainLoopCount; // need row/eleCount complete sweep of columns.

      se0Params.DIM2 = (strideIn / dataSize * temp * 2); // total elements in one sweep of columns
      if (bufParamsIn->data_type == DSPLIB_INT32) {
         se0Params.PROMOTE = __SE_PROMOTE_2X_SIGNEXT;
      }
      else {
         se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
      }
      se0Params.ELETYPE = SE_ELETYPE;
      typedef typename c7x::make_full_vector<int64_t>::type vec64;
      SE_VECLEN         = c7x::se_veclen<vec64>::value;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DIMFMT  = __SE_DIMFMT_3D;
   }
   else if (bufParamsIn->data_type == DSPLIB_INT16 || bufParamsIn->data_type == DSPLIB_UINT16) {
      dataSize        = 2;
      se0Params.ICNT0 = errCoefsSize; // col; // reach till end of columns
      se0Params.ICNT1 = (vecInSize > (uint32_t) (eleCount / 4))
                            ? (eleCount / 4)
                            : vecInSize; // total elements that can be read in columns

      se0Params.DIM1  = strideIn / dataSize;                  // col; // upto col
      se0Params.ICNT2 = pKerPrivArgs->mainLoopCount;          // need row/eleCount complete sweep of columns.
      se0Params.DIM2  = (strideIn / dataSize * eleCount / 2); // total elements in one sweep of columns

      if (bufParamsIn->data_type == DSPLIB_INT16) {
         se0Params.PROMOTE = __SE_PROMOTE_2X_SIGNEXT;
      }
      else {
         se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
      }
      se0Params.ELETYPE = SE_ELETYPE;
      typedef typename c7x::make_full_vector<int32_t>::type vec32;
      SE_VECLEN        = c7x::se_veclen<vec32>::value;
      se0Params.VECLEN = SE_VECLEN;
      se0Params.DIMFMT = __SE_DIMFMT_3D;
   }
   else {
      dataSize          = 1;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.ICNT0   = errCoefsSize;                // col; // reach till end of columns
      se0Params.ICNT1   = 16;                          // total elements that can be read in columns
      se0Params.DIM1    = strideIn;                    // col; // upto col
      se0Params.ICNT2   = pKerPrivArgs->mainLoopCount; // need row/eleCount complete sweep of columns.
      se0Params.DIM2    = strideIn * eleCount;         // total elements in one sweep of columns
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.DIMFMT  = __SE_DIMFMT_3D;

#if __C7X_VEC_SIZE_BITS__ == 512
      se0Params.DIM2 = strideIn * eleCount / 2; // total elements in one sweep of columns
#endif
   }

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(uint32_t *) ((uint8_t *) pBlock + (SE_SE1_PARAM_OFFSET))       = bufParamsIn->data_type;

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return status;
}

template DSPLIB_STATUS DSPLIB_minerror_init_ci<int64_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                        const DSPLIB_minerror_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minerror_init_ci<uint64_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                         const DSPLIB_minerror_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minerror_init_ci<int32_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                        const DSPLIB_minerror_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minerror_init_ci<uint32_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                         const DSPLIB_minerror_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minerror_init_ci<int16_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                        const DSPLIB_minerror_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minerror_init_ci<uint16_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                         const DSPLIB_minerror_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minerror_init_ci<int8_t>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                       const DSPLIB_minerror_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minerror_init_ci<uint8_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                        const DSPLIB_minerror_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minerror_init_ci<float>(DSPLIB_kernelHandle             handle,
                                                      const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                      const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                      const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                      const DSPLIB_minerror_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minerror_init_ci<double>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                                       const DSPLIB_minerror_InitArgs *pKerInitArgs);

template <typename FloatingPointDataType>
inline void minerror_exec_ci_float_inputs(void *restrict pErrCoefs,
                                          const int *restrict pMaxIndex,
                                          const void *restrict pMaxVal,
                                          uint8_t *restrict pBlock,
                                          uint32_t vecInSize,
                                          uint32_t errCoefsSize,
                                          int32_t  mainLoopCount)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   FloatingPointDataType *restrict pErrCoefsLocal = (FloatingPointDataType *) pErrCoefs;
   int *restrict pMaxIndexLocal                   = (int *) pMaxIndex;
   FloatingPointDataType *restrict pMaxValLocal   = (FloatingPointDataType *) pMaxVal;

   typedef typename c7x::make_full_vector<FloatingPointDataType>::type vec;
   int32_t                                                             eleCount = c7x::element_count_of<vec>::value;

   int32_t i;
   *pMaxValLocal = (FloatingPointDataType) (std::numeric_limits<FloatingPointDataType>::min());

   vec errCoefs1 = vec(pErrCoefsLocal[0]);
   vec errCoefs2 = vec(pErrCoefsLocal[1]);
   vec errCoefs3 = vec(pErrCoefsLocal[2]);
   vec errCoefs4 = vec(pErrCoefsLocal[3]);
   vec errCoefs5 = vec(pErrCoefsLocal[4]);
   vec errCoefs6 = vec(pErrCoefsLocal[5]);
   vec errCoefs7 = vec(pErrCoefsLocal[6]);
   vec errCoefs8 = vec(pErrCoefsLocal[7]);
   vec errCoefs9 = vec(pErrCoefsLocal[8]);

   vec dotProduct = vec(0);
   vec maxValVec  = vec(*pMaxValLocal);

   vec vIdx     = vec(0);
   vec vCurrIdx = *((vec *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET));

   __vpred vpMask;

   DSPLIB_DEBUGPRINTFN(0, "mainLoopCount %d errCoefsSize %d vecInSize %d\n", mainLoopCount, errCoefsSize, vecInSize);

   for (i = 0; i < mainLoopCount; i++) {
      dotProduct = vec(0);

      // 2SE fetch implementation
      vec tmp1_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp2_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp3_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp4_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp5_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp6_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp7_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp8_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp9_0 = c7x::strm_eng<0, vec>::get_adv();

      vec tmp1_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp2_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp3_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp4_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp5_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp6_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp7_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp8_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp9_1 = c7x::strm_eng<1, vec>::get_adv();

      vec dotProduct1 = (tmp1_0 * errCoefs1);
      vec dotProduct2 = (tmp2_0 * errCoefs2);
      vec dotProduct3 = (tmp3_0 * errCoefs3);
      vec dotProduct4 = (tmp4_0 * errCoefs4);
      vec dotProduct5 = (tmp5_0 * errCoefs5);
      vec dotProduct6 = (tmp6_0 * errCoefs6);
      vec dotProduct7 = (tmp7_0 * errCoefs7);
      vec dotProduct8 = (tmp8_0 * errCoefs8);
      vec dotProduct9 = (tmp9_0 * errCoefs9);

      dotProduct = dotProduct + dotProduct1;
      dotProduct = dotProduct + dotProduct2;
      dotProduct = dotProduct + dotProduct3;
      dotProduct = dotProduct + dotProduct4;
      dotProduct = dotProduct + dotProduct5;
      dotProduct = dotProduct + dotProduct6;
      dotProduct = dotProduct + dotProduct7;
      dotProduct = dotProduct + dotProduct8;
      dotProduct = dotProduct + dotProduct9;

      // Vertical max comparison once the vector dot products are computed for se0 fetched data
      vpMask    = __cmp_lt_pred(dotProduct, maxValVec);
      maxValVec = __select(vpMask, maxValVec, dotProduct);
      vIdx      = __select(vpMask, vIdx, vCurrIdx);
      vCurrIdx  = vCurrIdx + (eleCount);

      dotProduct  = vec(0);
      dotProduct1 = (tmp1_1 * errCoefs1);
      dotProduct2 = (tmp2_1 * errCoefs2);
      dotProduct3 = (tmp3_1 * errCoefs3);
      dotProduct4 = (tmp4_1 * errCoefs4);
      dotProduct5 = (tmp5_1 * errCoefs5);
      dotProduct6 = (tmp6_1 * errCoefs6);
      dotProduct7 = (tmp7_1 * errCoefs7);
      dotProduct8 = (tmp8_1 * errCoefs8);
      dotProduct9 = (tmp9_1 * errCoefs9);

      dotProduct = dotProduct + dotProduct1;
      dotProduct = dotProduct + dotProduct2;
      dotProduct = dotProduct + dotProduct3;
      dotProduct = dotProduct + dotProduct4;
      dotProduct = dotProduct + dotProduct5;
      dotProduct = dotProduct + dotProduct6;
      dotProduct = dotProduct + dotProduct7;
      dotProduct = dotProduct + dotProduct8;
      dotProduct = dotProduct + dotProduct9;

      // Vertical max comparison once the vector dot products are computed for se1 fetched data
      vpMask    = __cmp_lt_pred(dotProduct, maxValVec);
      maxValVec = __select(vpMask, maxValVec, dotProduct);
      vIdx      = __select(vpMask, vIdx, vCurrIdx);
      vCurrIdx  = vCurrIdx + (eleCount);
   }

   // Horizontal max computation
   c7x_horizontal_max_with_index(maxValVec, vIdx, pMaxValLocal, pMaxIndexLocal);

   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}

template <typename Integer64BitDataType, typename Integer64BitConversionDataType>
inline void minerror_exec_ci_signed_integer64_inputs(void *restrict pErrCoefs,
                                                     const int *restrict pMaxIndex,
                                                     const void *restrict pMaxVal,
                                                     uint8_t *restrict pBlock,
                                                     uint32_t vecInSize,
                                                     uint32_t errCoefsSize,
                                                     int32_t  mainLoopCount)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   // convert int64 to double to handle overflow and precision accuracy during multiplication and accumulation
   // eg: for 64bits, multiply and accumulate in double precision
   Integer64BitDataType *restrict pErrCoefsLocal         = (Integer64BitDataType *) pErrCoefs;
   int *restrict pMaxIndexLocal                          = (int *) pMaxIndex;
   Integer64BitConversionDataType *restrict pMaxValLocal = (Integer64BitConversionDataType *) pMaxVal;

   typedef typename c7x::make_full_vector<Integer64BitDataType>::type vec;
   int32_t                                                            eleCount = c7x::element_count_of<vec>::value;

   typedef typename c7x::make_full_vector<Integer64BitConversionDataType>::type vecConverted;
   typedef typename c7x::make_full_vector<int32_t>::type                        vecIntermediate;

   int32_t i;
   *pMaxValLocal = (Integer64BitConversionDataType) (std::numeric_limits<Integer64BitConversionDataType>::min());

   vecConverted errCoefs1 = vecConverted(pErrCoefsLocal[0]);
   vecConverted errCoefs2 = vecConverted(pErrCoefsLocal[1]);
   vecConverted errCoefs3 = vecConverted(pErrCoefsLocal[2]);
   vecConverted errCoefs4 = vecConverted(pErrCoefsLocal[3]);
   vecConverted errCoefs5 = vecConverted(pErrCoefsLocal[4]);
   vecConverted errCoefs6 = vecConverted(pErrCoefsLocal[5]);
   vecConverted errCoefs7 = vecConverted(pErrCoefsLocal[6]);
   vecConverted errCoefs8 = vecConverted(pErrCoefsLocal[7]);
   vecConverted errCoefs9 = vecConverted(pErrCoefsLocal[8]);

   vecConverted dotProduct = vecConverted(0);
   vecConverted maxValVec  = vecConverted(*pMaxValLocal);

   vecConverted vIdx     = vecConverted(0);
   vecConverted vCurrIdx = *((vecConverted *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET));

   vecConverted mulFactor = vecConverted(4294967296.00);

   __vpred vpMask;

   DSPLIB_DEBUGPRINTFN(0, "mainLoopCount %d errCoefsSize %d vecInSize %d \n", mainLoopCount, errCoefsSize, vecInSize);

   for (i = 0; i < mainLoopCount; i++) {
      dotProduct = vecConverted(0);

      // 2SE fetch implementation

      vec tmp1_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp2_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp3_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp4_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp5_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp6_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp7_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp8_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp9_0 = c7x::strm_eng<0, vec>::get_adv();

      vec tmp1_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp2_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp3_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp4_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp5_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp6_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp7_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp8_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp9_1 = c7x::strm_eng<1, vec>::get_adv();

      vecIntermediate v16bits1_0 = convert_long_to_int<vecIntermediate, vec>(tmp1_0);
      vecIntermediate v16bits2_0 = convert_long_to_int<vecIntermediate, vec>(tmp2_0);
      vecIntermediate v16bits3_0 = convert_long_to_int<vecIntermediate, vec>(tmp3_0);
      vecIntermediate v16bits4_0 = convert_long_to_int<vecIntermediate, vec>(tmp4_0);
      vecIntermediate v16bits5_0 = convert_long_to_int<vecIntermediate, vec>(tmp5_0);
      vecIntermediate v16bits6_0 = convert_long_to_int<vecIntermediate, vec>(tmp6_0);
      vecIntermediate v16bits7_0 = convert_long_to_int<vecIntermediate, vec>(tmp7_0);
      vecIntermediate v16bits8_0 = convert_long_to_int<vecIntermediate, vec>(tmp8_0);
      vecIntermediate v16bits9_0 = convert_long_to_int<vecIntermediate, vec>(tmp9_0);

      vecConverted vhigh16bits1_0 = __high_int_to_double(v16bits1_0);
      vecConverted vlow16bits1_0  = __low_int_to_double(v16bits1_0);
      vecConverted vSum1_0        = vhigh16bits1_0 * mulFactor;
      vSum1_0                     = vSum1_0 + vlow16bits1_0;

      vecConverted vhigh16bits2_0 = __high_int_to_double(v16bits2_0);
      vecConverted vlow16bits2_0  = __low_int_to_double(v16bits2_0);
      vecConverted vSum2_0        = vhigh16bits2_0 * mulFactor;
      vSum2_0                     = vSum2_0 + vlow16bits2_0;

      vecConverted vhigh16bits3_0 = __high_int_to_double(v16bits3_0);
      vecConverted vlow16bits3_0  = __low_int_to_double(v16bits3_0);
      vecConverted vSum3_0        = vhigh16bits3_0 * mulFactor;
      vSum3_0                     = vSum3_0 + vlow16bits3_0;

      vecConverted vhigh16bits4_0 = __high_int_to_double(v16bits4_0);
      vecConverted vlow16bits4_0  = __low_int_to_double(v16bits4_0);
      vecConverted vSum4_0        = vhigh16bits4_0 * mulFactor;
      vSum4_0                     = vSum4_0 + vlow16bits4_0;

      vecConverted vhigh16bits5_0 = __high_int_to_double(v16bits5_0);
      vecConverted vlow16bits5_0  = __low_int_to_double(v16bits5_0);
      vecConverted vSum5_0        = vhigh16bits5_0 * mulFactor;
      vSum5_0                     = vSum5_0 + vlow16bits5_0;

      vecConverted vhigh16bits6_0 = __high_int_to_double(v16bits6_0);
      vecConverted vlow16bits6_0  = __low_int_to_double(v16bits6_0);
      vecConverted vSum6_0        = vhigh16bits6_0 * mulFactor;
      vSum6_0                     = vSum6_0 + vlow16bits6_0;

      vecConverted vhigh16bits7_0 = __high_int_to_double(v16bits7_0);
      vecConverted vlow16bits7_0  = __low_int_to_double(v16bits7_0);
      vecConverted vSum7_0        = vhigh16bits7_0 * mulFactor;
      vSum7_0                     = vSum7_0 + vlow16bits7_0;

      vecConverted vhigh16bits8_0 = __high_int_to_double(v16bits8_0);
      vecConverted vlow16bits8_0  = __low_int_to_double(v16bits8_0);
      vecConverted vSum8_0        = vhigh16bits8_0 * mulFactor;
      vSum8_0                     = vSum8_0 + vlow16bits8_0;

      vecConverted vhigh16bits9_0 = __high_int_to_double(v16bits9_0);
      vecConverted vlow16bits9_0  = __low_int_to_double(v16bits9_0);
      vecConverted vSum9_0        = vhigh16bits9_0 * mulFactor;
      vSum9_0                     = vSum9_0 + vlow16bits9_0;

      vecConverted dotProduct1 = (vSum1_0 * errCoefs1);
      vecConverted dotProduct2 = (vSum2_0 * errCoefs2);
      vecConverted dotProduct3 = (vSum3_0 * errCoefs3);
      vecConverted dotProduct4 = (vSum4_0 * errCoefs4);
      vecConverted dotProduct5 = (vSum5_0 * errCoefs5);
      vecConverted dotProduct6 = (vSum6_0 * errCoefs6);
      vecConverted dotProduct7 = (vSum7_0 * errCoefs7);
      vecConverted dotProduct8 = (vSum8_0 * errCoefs8);
      vecConverted dotProduct9 = (vSum9_0 * errCoefs9);

      dotProduct = dotProduct + dotProduct1;
      dotProduct = dotProduct + dotProduct2;
      dotProduct = dotProduct + dotProduct3;
      dotProduct = dotProduct + dotProduct4;
      dotProduct = dotProduct + dotProduct5;
      dotProduct = dotProduct + dotProduct6;
      dotProduct = dotProduct + dotProduct7;
      dotProduct = dotProduct + dotProduct8;
      dotProduct = dotProduct + dotProduct9;

      // Vertical max comparison once the vector dot products are computed for se0 feteched data
      vpMask    = __cmp_lt_pred(dotProduct, maxValVec);
      maxValVec = __select(vpMask, maxValVec, dotProduct);
      vIdx      = __select(vpMask, vIdx, vCurrIdx);
      vCurrIdx  = vCurrIdx + (eleCount);

      dotProduct = vecConverted(0);

      vecIntermediate v16bits1_1 = convert_long_to_int<vecIntermediate, vec>(tmp1_1);
      vecIntermediate v16bits2_1 = convert_long_to_int<vecIntermediate, vec>(tmp2_1);
      vecIntermediate v16bits3_1 = convert_long_to_int<vecIntermediate, vec>(tmp3_1);
      vecIntermediate v16bits4_1 = convert_long_to_int<vecIntermediate, vec>(tmp4_1);
      vecIntermediate v16bits5_1 = convert_long_to_int<vecIntermediate, vec>(tmp5_1);
      vecIntermediate v16bits6_1 = convert_long_to_int<vecIntermediate, vec>(tmp6_1);
      vecIntermediate v16bits7_1 = convert_long_to_int<vecIntermediate, vec>(tmp7_1);
      vecIntermediate v16bits8_1 = convert_long_to_int<vecIntermediate, vec>(tmp8_1);
      vecIntermediate v16bits9_1 = convert_long_to_int<vecIntermediate, vec>(tmp9_1);

      vecConverted vhigh16bits1_1 = __high_int_to_double(v16bits1_1);
      vecConverted vlow16bits1_1  = __low_int_to_double(v16bits1_1);
      vecConverted vSum1_1        = vhigh16bits1_1 * mulFactor;
      vSum1_1                     = vSum1_1 + vlow16bits1_1;

      vecConverted vhigh16bits2_1 = __high_int_to_double(v16bits2_1);
      vecConverted vlow16bits2_1  = __low_int_to_double(v16bits2_1);
      vecConverted vSum2_1        = vhigh16bits2_1 * mulFactor;
      vSum2_1                     = vSum2_1 + vlow16bits2_1;

      vecConverted vhigh16bits3_1 = __high_int_to_double(v16bits3_1);
      vecConverted vlow16bits3_1  = __low_int_to_double(v16bits3_1);
      vecConverted vSum3_1        = vhigh16bits3_1 * mulFactor;
      vSum3_1                     = vSum3_1 + vlow16bits3_1;

      vecConverted vhigh16bits4_1 = __high_int_to_double(v16bits4_1);
      vecConverted vlow16bits4_1  = __low_int_to_double(v16bits4_1);
      vecConverted vSum4_1        = vhigh16bits4_1 * mulFactor;
      vSum4_1                     = vSum4_1 + vlow16bits4_1;

      vecConverted vhigh16bits5_1 = __high_int_to_double(v16bits5_1);
      vecConverted vlow16bits5_1  = __low_int_to_double(v16bits5_1);
      vecConverted vSum5_1        = vhigh16bits5_1 * mulFactor;
      vSum5_1                     = vSum5_1 + vlow16bits5_1;

      vecConverted vhigh16bits6_1 = __high_int_to_double(v16bits6_1);
      vecConverted vlow16bits6_1  = __low_int_to_double(v16bits6_1);
      vecConverted vSum6_1        = vhigh16bits6_1 * mulFactor;
      vSum6_1                     = vSum6_1 + vlow16bits6_1;

      vecConverted vhigh16bits7_1 = __high_int_to_double(v16bits7_1);
      vecConverted vlow16bits7_1  = __low_int_to_double(v16bits7_1);
      vecConverted vSum7_1        = vhigh16bits7_1 * mulFactor;
      vSum7_1                     = vSum7_1 + vlow16bits7_1;

      vecConverted vhigh16bits8_1 = __high_int_to_double(v16bits8_1);
      vecConverted vlow16bits8_1  = __low_int_to_double(v16bits8_1);
      vecConverted vSum8_1        = vhigh16bits8_1 * mulFactor;
      vSum8_1                     = vSum8_1 + vlow16bits8_1;

      vecConverted vhigh16bits9_1 = __high_int_to_double(v16bits9_1);
      vecConverted vlow16bits9_1  = __low_int_to_double(v16bits9_1);
      vecConverted vSum9_1        = vhigh16bits9_1 * mulFactor;
      vSum9_1                     = vSum9_1 + vlow16bits9_1;

      dotProduct1 = (vSum1_1 * errCoefs1);
      dotProduct2 = (vSum2_1 * errCoefs2);
      dotProduct3 = (vSum3_1 * errCoefs3);
      dotProduct4 = (vSum4_1 * errCoefs4);
      dotProduct5 = (vSum5_1 * errCoefs5);
      dotProduct6 = (vSum6_1 * errCoefs6);
      dotProduct7 = (vSum7_1 * errCoefs7);
      dotProduct8 = (vSum8_1 * errCoefs8);
      dotProduct9 = (vSum9_1 * errCoefs9);

      dotProduct = dotProduct + dotProduct1;
      dotProduct = dotProduct + dotProduct2;
      dotProduct = dotProduct + dotProduct3;
      dotProduct = dotProduct + dotProduct4;
      dotProduct = dotProduct + dotProduct5;
      dotProduct = dotProduct + dotProduct6;
      dotProduct = dotProduct + dotProduct7;
      dotProduct = dotProduct + dotProduct8;
      dotProduct = dotProduct + dotProduct9;

      // Vertical max comparison once the vector dot products are computed for se1 feteched data
      vpMask    = __cmp_lt_pred(dotProduct, maxValVec);
      maxValVec = __select(vpMask, maxValVec, dotProduct);
      vIdx      = __select(vpMask, vIdx, vCurrIdx);
      vCurrIdx  = vCurrIdx + (eleCount);
   }

   // Horizontal max computation
   c7x_horizontal_max_with_index(maxValVec, vIdx, pMaxValLocal, pMaxIndexLocal);
   double maxval = *pMaxValLocal;
   *pMaxValLocal = ((Integer64BitDataType) maxval);

   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}

template <typename Integer64BitDataType, typename Integer64BitConversionDataType>
inline void minerror_exec_ci_unsigned_integer64_inputs(void *restrict pErrCoefs,
                                                       const int *restrict pMaxIndex,
                                                       const void *restrict pMaxVal,
                                                       uint8_t *restrict pBlock,
                                                       uint32_t vecInSize,
                                                       uint32_t errCoefsSize,
                                                       int32_t  mainLoopCount)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   // convert uint64 to double to handle overflow and precision accuracy during multiplication and accumulation
   // eg: for 64bits, multiply and accumulate in double precision

   Integer64BitDataType *restrict pErrCoefsLocal         = (Integer64BitDataType *) pErrCoefs;
   int *restrict pMaxIndexLocal                          = (int *) pMaxIndex;
   Integer64BitConversionDataType *restrict pMaxValLocal = (Integer64BitConversionDataType *) pMaxVal;

   typedef typename c7x::make_full_vector<Integer64BitDataType>::type vec;
   int32_t                                                            eleCount = c7x::element_count_of<vec>::value;

   typedef typename c7x::make_full_vector<Integer64BitConversionDataType>::type vecConverted;

   typedef typename c7x::make_full_vector<uint32_t>::type vecIntermediate;

   int32_t i;
   *pMaxValLocal = (Integer64BitConversionDataType) (std::numeric_limits<Integer64BitConversionDataType>::min());

   vecConverted errCoefs1 = vecConverted(pErrCoefsLocal[0]);
   vecConverted errCoefs2 = vecConverted(pErrCoefsLocal[1]);
   vecConverted errCoefs3 = vecConverted(pErrCoefsLocal[2]);
   vecConverted errCoefs4 = vecConverted(pErrCoefsLocal[3]);
   vecConverted errCoefs5 = vecConverted(pErrCoefsLocal[4]);
   vecConverted errCoefs6 = vecConverted(pErrCoefsLocal[5]);
   vecConverted errCoefs7 = vecConverted(pErrCoefsLocal[6]);
   vecConverted errCoefs8 = vecConverted(pErrCoefsLocal[7]);
   vecConverted errCoefs9 = vecConverted(pErrCoefsLocal[8]);

   vecConverted dotProduct = vecConverted(0);
   vecConverted maxValVec  = vecConverted(*pMaxValLocal);

   vecConverted vIdx     = vecConverted(0);
   vecConverted vCurrIdx = *((vecConverted *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET));

   vecConverted mulFactor = vecConverted(4294967296.00);

   __vpred vpMask;

   DSPLIB_DEBUGPRINTFN(0, "mainLoopCount %d errCoefsSize %d vecInSize %d \n", mainLoopCount, errCoefsSize, vecInSize);

   for (i = 0; i < mainLoopCount; i++) {
      dotProduct = vecConverted(0);

      // 2SE fetch implementation

      vec tmp1_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp2_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp3_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp4_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp5_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp6_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp7_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp8_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp9_0 = c7x::strm_eng<0, vec>::get_adv();

      vec tmp1_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp2_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp3_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp4_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp5_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp6_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp7_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp8_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp9_1 = c7x::strm_eng<1, vec>::get_adv();

      vecIntermediate v16bits1_0 = convert_long_to_int<vecIntermediate, vec>(tmp1_0);
      vecIntermediate v16bits2_0 = convert_long_to_int<vecIntermediate, vec>(tmp2_0);
      vecIntermediate v16bits3_0 = convert_long_to_int<vecIntermediate, vec>(tmp3_0);
      vecIntermediate v16bits4_0 = convert_long_to_int<vecIntermediate, vec>(tmp4_0);
      vecIntermediate v16bits5_0 = convert_long_to_int<vecIntermediate, vec>(tmp5_0);
      vecIntermediate v16bits6_0 = convert_long_to_int<vecIntermediate, vec>(tmp6_0);
      vecIntermediate v16bits7_0 = convert_long_to_int<vecIntermediate, vec>(tmp7_0);
      vecIntermediate v16bits8_0 = convert_long_to_int<vecIntermediate, vec>(tmp8_0);
      vecIntermediate v16bits9_0 = convert_long_to_int<vecIntermediate, vec>(tmp9_0);

      vecConverted vhigh16bits1_0 = __high_int_to_double(v16bits1_0);
      vecConverted vlow16bits1_0  = __low_int_to_double(v16bits1_0);
      vecConverted vSum1_0        = vhigh16bits1_0 * mulFactor;
      vSum1_0                     = vSum1_0 + vlow16bits1_0;

      vecConverted vhigh16bits2_0 = __high_int_to_double(v16bits2_0);
      vecConverted vlow16bits2_0  = __low_int_to_double(v16bits2_0);
      vecConverted vSum2_0        = vhigh16bits2_0 * mulFactor;
      vSum2_0                     = vSum2_0 + vlow16bits2_0;

      vecConverted vhigh16bits3_0 = __high_int_to_double(v16bits3_0);
      vecConverted vlow16bits3_0  = __low_int_to_double(v16bits3_0);
      vecConverted vSum3_0        = vhigh16bits3_0 * mulFactor;
      vSum3_0                     = vSum3_0 + vlow16bits3_0;

      vecConverted vhigh16bits4_0 = __high_int_to_double(v16bits4_0);
      vecConverted vlow16bits4_0  = __low_int_to_double(v16bits4_0);
      vecConverted vSum4_0        = vhigh16bits4_0 * mulFactor;
      vSum4_0                     = vSum4_0 + vlow16bits4_0;

      vecConverted vhigh16bits5_0 = __high_int_to_double(v16bits5_0);
      vecConverted vlow16bits5_0  = __low_int_to_double(v16bits5_0);
      vecConverted vSum5_0        = vhigh16bits5_0 * mulFactor;
      vSum5_0                     = vSum5_0 + vlow16bits5_0;

      vecConverted vhigh16bits6_0 = __high_int_to_double(v16bits6_0);
      vecConverted vlow16bits6_0  = __low_int_to_double(v16bits6_0);
      vecConverted vSum6_0        = vhigh16bits6_0 * mulFactor;
      vSum6_0                     = vSum6_0 + vlow16bits6_0;

      vecConverted vhigh16bits7_0 = __high_int_to_double(v16bits7_0);
      vecConverted vlow16bits7_0  = __low_int_to_double(v16bits7_0);
      vecConverted vSum7_0        = vhigh16bits7_0 * mulFactor;
      vSum7_0                     = vSum7_0 + vlow16bits7_0;

      vecConverted vhigh16bits8_0 = __high_int_to_double(v16bits8_0);
      vecConverted vlow16bits8_0  = __low_int_to_double(v16bits8_0);
      vecConverted vSum8_0        = vhigh16bits8_0 * mulFactor;
      vSum8_0                     = vSum8_0 + vlow16bits8_0;

      vecConverted vhigh16bits9_0 = __high_int_to_double(v16bits9_0);
      vecConverted vlow16bits9_0  = __low_int_to_double(v16bits9_0);
      vecConverted vSum9_0        = vhigh16bits9_0 * mulFactor;
      vSum9_0                     = vSum9_0 + vlow16bits9_0;

      vecConverted dotProduct1 = (vSum1_0 * errCoefs1);
      vecConverted dotProduct2 = (vSum2_0 * errCoefs2);
      vecConverted dotProduct3 = (vSum3_0 * errCoefs3);
      vecConverted dotProduct4 = (vSum4_0 * errCoefs4);
      vecConverted dotProduct5 = (vSum5_0 * errCoefs5);
      vecConverted dotProduct6 = (vSum6_0 * errCoefs6);
      vecConverted dotProduct7 = (vSum7_0 * errCoefs7);
      vecConverted dotProduct8 = (vSum8_0 * errCoefs8);
      vecConverted dotProduct9 = (vSum9_0 * errCoefs9);

      dotProduct = dotProduct + dotProduct1;
      dotProduct = dotProduct + dotProduct2;
      dotProduct = dotProduct + dotProduct3;
      dotProduct = dotProduct + dotProduct4;
      dotProduct = dotProduct + dotProduct5;
      dotProduct = dotProduct + dotProduct6;
      dotProduct = dotProduct + dotProduct7;
      dotProduct = dotProduct + dotProduct8;
      dotProduct = dotProduct + dotProduct9;

      // Vertical max comparison once the vector dot products are computed for se0 feteched data
      vpMask    = __cmp_lt_pred(dotProduct, maxValVec);
      maxValVec = __select(vpMask, maxValVec, dotProduct);
      vIdx      = __select(vpMask, vIdx, vCurrIdx);
      vCurrIdx  = vCurrIdx + (eleCount);

      dotProduct = vecConverted(0);

      vecIntermediate v16bits1_1 = convert_long_to_int<vecIntermediate, vec>(tmp1_1);
      vecIntermediate v16bits2_1 = convert_long_to_int<vecIntermediate, vec>(tmp2_1);
      vecIntermediate v16bits3_1 = convert_long_to_int<vecIntermediate, vec>(tmp3_1);
      vecIntermediate v16bits4_1 = convert_long_to_int<vecIntermediate, vec>(tmp4_1);
      vecIntermediate v16bits5_1 = convert_long_to_int<vecIntermediate, vec>(tmp5_1);
      vecIntermediate v16bits6_1 = convert_long_to_int<vecIntermediate, vec>(tmp6_1);
      vecIntermediate v16bits7_1 = convert_long_to_int<vecIntermediate, vec>(tmp7_1);
      vecIntermediate v16bits8_1 = convert_long_to_int<vecIntermediate, vec>(tmp8_1);
      vecIntermediate v16bits9_1 = convert_long_to_int<vecIntermediate, vec>(tmp9_1);

      vecConverted vhigh16bits1_1 = __high_int_to_double(v16bits1_1);
      vecConverted vlow16bits1_1  = __low_int_to_double(v16bits1_1);
      vecConverted vSum1_1        = vhigh16bits1_1 * mulFactor;
      vSum1_1                     = vSum1_1 + vlow16bits1_1;

      vecConverted vhigh16bits2_1 = __high_int_to_double(v16bits2_1);
      vecConverted vlow16bits2_1  = __low_int_to_double(v16bits2_1);
      vecConverted vSum2_1        = vhigh16bits2_1 * mulFactor;
      vSum2_1                     = vSum2_1 + vlow16bits2_1;

      vecConverted vhigh16bits3_1 = __high_int_to_double(v16bits3_1);
      vecConverted vlow16bits3_1  = __low_int_to_double(v16bits3_1);
      vecConverted vSum3_1        = vhigh16bits3_1 * mulFactor;
      vSum3_1                     = vSum3_1 + vlow16bits3_1;

      vecConverted vhigh16bits4_1 = __high_int_to_double(v16bits4_1);
      vecConverted vlow16bits4_1  = __low_int_to_double(v16bits4_1);
      vecConverted vSum4_1        = vhigh16bits4_1 * mulFactor;
      vSum4_1                     = vSum4_1 + vlow16bits4_1;

      vecConverted vhigh16bits5_1 = __high_int_to_double(v16bits5_1);
      vecConverted vlow16bits5_1  = __low_int_to_double(v16bits5_1);
      vecConverted vSum5_1        = vhigh16bits5_1 * mulFactor;
      vSum5_1                     = vSum5_1 + vlow16bits5_1;

      vecConverted vhigh16bits6_1 = __high_int_to_double(v16bits6_1);
      vecConverted vlow16bits6_1  = __low_int_to_double(v16bits6_1);
      vecConverted vSum6_1        = vhigh16bits6_1 * mulFactor;
      vSum6_1                     = vSum6_1 + vlow16bits6_1;

      vecConverted vhigh16bits7_1 = __high_int_to_double(v16bits7_1);
      vecConverted vlow16bits7_1  = __low_int_to_double(v16bits7_1);
      vecConverted vSum7_1        = vhigh16bits7_1 * mulFactor;
      vSum7_1                     = vSum7_1 + vlow16bits7_1;

      vecConverted vhigh16bits8_1 = __high_int_to_double(v16bits8_1);
      vecConverted vlow16bits8_1  = __low_int_to_double(v16bits8_1);
      vecConverted vSum8_1        = vhigh16bits8_1 * mulFactor;
      vSum8_1                     = vSum8_1 + vlow16bits8_1;

      vecConverted vhigh16bits9_1 = __high_int_to_double(v16bits9_1);
      vecConverted vlow16bits9_1  = __low_int_to_double(v16bits9_1);
      vecConverted vSum9_1        = vhigh16bits9_1 * mulFactor;
      vSum9_1                     = vSum9_1 + vlow16bits9_1;

      dotProduct1 = (vSum1_1 * errCoefs1);
      dotProduct2 = (vSum2_1 * errCoefs2);
      dotProduct3 = (vSum3_1 * errCoefs3);
      dotProduct4 = (vSum4_1 * errCoefs4);
      dotProduct5 = (vSum5_1 * errCoefs5);
      dotProduct6 = (vSum6_1 * errCoefs6);
      dotProduct7 = (vSum7_1 * errCoefs7);
      dotProduct8 = (vSum8_1 * errCoefs8);
      dotProduct9 = (vSum9_1 * errCoefs9);

      dotProduct = dotProduct + dotProduct1;
      dotProduct = dotProduct + dotProduct2;
      dotProduct = dotProduct + dotProduct3;
      dotProduct = dotProduct + dotProduct4;
      dotProduct = dotProduct + dotProduct5;
      dotProduct = dotProduct + dotProduct6;
      dotProduct = dotProduct + dotProduct7;
      dotProduct = dotProduct + dotProduct8;
      dotProduct = dotProduct + dotProduct9;

      // Vertical max comparison once the vector dot products are computed for se1 feteched data
      vpMask    = __cmp_lt_pred(dotProduct, maxValVec);
      maxValVec = __select(vpMask, maxValVec, dotProduct);
      vIdx      = __select(vpMask, vIdx, vCurrIdx);
      vCurrIdx  = vCurrIdx + (eleCount);
   }

   // Horizontal max computation
   c7x_horizontal_max_with_index(maxValVec, vIdx, pMaxValLocal, pMaxIndexLocal);
   double maxval = *pMaxValLocal;
   *pMaxValLocal = ((Integer64BitDataType) maxval);
   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}

// supports both signed and unsigned datatypes
template <typename Integer32BitDataType, typename Integer32BitPromotedDataType>
inline void minerror_exec_ci_integer32_inputs(void *restrict pErrCoefs,
                                              const int *restrict pMaxIndex,
                                              const void *restrict pMaxVal,
                                              uint8_t *restrict pBlock,
                                              uint32_t vecInSize,
                                              uint32_t errCoefsSize,
                                              int32_t  mainLoopCount)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   // SE promoted to 64 bits in transpose mode to handle overflow
   // eg: for 32bits, multiply and accumulate with right shift in 64bits

   Integer32BitDataType *restrict pErrCoefsLocal       = (Integer32BitDataType *) pErrCoefs;
   int *restrict pMaxIndexLocal                        = (int *) pMaxIndex;
   Integer32BitPromotedDataType *restrict pMaxValLocal = (Integer32BitPromotedDataType *) pMaxVal;

   typedef typename c7x::make_full_vector<Integer32BitPromotedDataType>::type vec;
   int32_t eleCount = c7x::element_count_of<vec>::value;

   int32_t i;
   *pMaxValLocal = (Integer32BitPromotedDataType) (std::numeric_limits<Integer32BitPromotedDataType>::min());

   vec errCoefs1 = vec(pErrCoefsLocal[0]);
   vec errCoefs2 = vec(pErrCoefsLocal[1]);
   vec errCoefs3 = vec(pErrCoefsLocal[2]);
   vec errCoefs4 = vec(pErrCoefsLocal[3]);
   vec errCoefs5 = vec(pErrCoefsLocal[4]);
   vec errCoefs6 = vec(pErrCoefsLocal[5]);
   vec errCoefs7 = vec(pErrCoefsLocal[6]);
   vec errCoefs8 = vec(pErrCoefsLocal[7]);
   vec errCoefs9 = vec(pErrCoefsLocal[8]);

   vec dotProduct = vec(0);
   vec maxValVec  = vec(*pMaxValLocal);

   vec     vIdx     = vec(0);
   vec     vCurrIdx = *((vec *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET));
   __vpred vpMask;

   DSPLIB_DEBUGPRINTFN(0, "mainLoopCount %d errCoefsSize %d vecInSize %d \n", mainLoopCount, errCoefsSize, vecInSize);

   for (i = 0; i < mainLoopCount; i++) {
      dotProduct = vec(0);

      // 2SE fetch implementation

      vec tmp1_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp2_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp3_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp4_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp5_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp6_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp7_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp8_0 = c7x::strm_eng<0, vec>::get_adv();
      vec tmp9_0 = c7x::strm_eng<0, vec>::get_adv();

      vec tmp1_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp2_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp3_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp4_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp5_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp6_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp7_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp8_1 = c7x::strm_eng<1, vec>::get_adv();
      vec tmp9_1 = c7x::strm_eng<1, vec>::get_adv();

      vec dotProduct1 = (tmp1_0 * errCoefs1);
      vec dotProduct2 = (tmp2_0 * errCoefs2);
      vec dotProduct3 = (tmp3_0 * errCoefs3);
      vec dotProduct4 = (tmp4_0 * errCoefs4);
      vec dotProduct5 = (tmp5_0 * errCoefs5);
      vec dotProduct6 = (tmp6_0 * errCoefs6);
      vec dotProduct7 = (tmp7_0 * errCoefs7);
      vec dotProduct8 = (tmp8_0 * errCoefs8);
      vec dotProduct9 = (tmp9_0 * errCoefs9);

      vec acc1   = (__shift_right(dotProduct1, vec(1))) + (__shift_right(dotProduct2, vec(1)));
      vec acc2   = (__shift_right(dotProduct3, vec(1))) + (__shift_right(dotProduct4, vec(1)));
      vec acc3   = (__shift_right(dotProduct5, vec(1))) + (__shift_right(dotProduct6, vec(1)));
      vec acc4   = (__shift_right(dotProduct7, vec(1))) + (__shift_right(dotProduct8, vec(1)));
      vec acc5   = (__shift_right(acc1, vec(1))) + (__shift_right(acc2, vec(1)));
      vec acc6   = (__shift_right(acc3, vec(1))) + (__shift_right(acc4, vec(1)));
      vec acc7   = (__shift_right(acc5, vec(1))) + (__shift_right(acc6, vec(1)));
      dotProduct = (__shift_right(acc7, vec(1))) + (__shift_right(dotProduct9, vec(1)));

      // Vertical max comparison once the vector dot products are computed for se0 feteched data
      __max_index(dotProduct, maxValVec, vpMask);
      vIdx     = __select(vpMask, vCurrIdx, vIdx);
      vCurrIdx = vCurrIdx + (eleCount);

      dotProduct = vec(0);

      dotProduct1 = (tmp1_1 * errCoefs1);
      dotProduct2 = (tmp2_1 * errCoefs2);
      dotProduct3 = (tmp3_1 * errCoefs3);
      dotProduct4 = (tmp4_1 * errCoefs4);
      dotProduct5 = (tmp5_1 * errCoefs5);
      dotProduct6 = (tmp6_1 * errCoefs6);
      dotProduct7 = (tmp7_1 * errCoefs7);
      dotProduct8 = (tmp8_1 * errCoefs8);
      dotProduct9 = (tmp9_1 * errCoefs9);

      acc1       = (__shift_right(dotProduct1, vec(1))) + (__shift_right(dotProduct2, vec(1)));
      acc2       = (__shift_right(dotProduct3, vec(1))) + (__shift_right(dotProduct4, vec(1)));
      acc3       = (__shift_right(dotProduct5, vec(1))) + (__shift_right(dotProduct6, vec(1)));
      acc4       = (__shift_right(dotProduct7, vec(1))) + (__shift_right(dotProduct8, vec(1)));
      acc5       = (__shift_right(acc1, vec(1))) + (__shift_right(acc2, vec(1)));
      acc6       = (__shift_right(acc3, vec(1))) + (__shift_right(acc4, vec(1)));
      acc7       = (__shift_right(acc5, vec(1))) + (__shift_right(acc6, vec(1)));
      dotProduct = (__shift_right(acc7, vec(1))) + (__shift_right(dotProduct9, vec(1)));

      // Vertical max comparison once the vector dot products are computed for se1 feteched data
      __max_index(dotProduct, maxValVec, vpMask);
      vIdx     = __select(vpMask, vCurrIdx, vIdx);
      vCurrIdx = vCurrIdx + (eleCount);
   }

   // Horizontal max computation
   c7x_horizontal_max_with_index(maxValVec, vIdx, pMaxValLocal, pMaxIndexLocal);

   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}

// supports both signed and unsigned datatypes
template <typename Integer16BitDataType, typename Integer16BitPromotedDataType>
inline void minerror_exec_ci_integer16_inputs(void *restrict pErrCoefs,
                                              const int *restrict pMaxIndex,
                                              const void *restrict pMaxVal,
                                              uint8_t *restrict pBlock,
                                              uint32_t vecInSize,
                                              uint32_t errCoefsSize,
                                              int32_t  mainLoopCount)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   // SE promoted to 32 bits in transpose mode to handle overflow
   // eg: for 16bits, multiply and accumulate with right shift in 32bits

   Integer16BitDataType *restrict pErrCoefsLocal       = (Integer16BitDataType *) pErrCoefs;
   int *restrict pMaxIndexLocal                        = (int *) pMaxIndex;
   Integer16BitPromotedDataType *restrict pMaxValLocal = (Integer16BitPromotedDataType *) pMaxVal;

   typedef typename c7x::make_full_vector<Integer16BitPromotedDataType>::type vec;
   int16_t eleCount = (c7x::element_count_of<vec>::value);

   int32_t i;
   *pMaxValLocal = (Integer16BitPromotedDataType) (std::numeric_limits<Integer16BitPromotedDataType>::min());

   vec errCoefs1 = vec(pErrCoefsLocal[0]);
   vec errCoefs2 = vec(pErrCoefsLocal[1]);
   vec errCoefs3 = vec(pErrCoefsLocal[2]);
   vec errCoefs4 = vec(pErrCoefsLocal[3]);
   vec errCoefs5 = vec(pErrCoefsLocal[4]);
   vec errCoefs6 = vec(pErrCoefsLocal[5]);
   vec errCoefs7 = vec(pErrCoefsLocal[6]);
   vec errCoefs8 = vec(pErrCoefsLocal[7]);
   vec errCoefs9 = vec(pErrCoefsLocal[8]);

   vec dotProduct = vec(0);
   vec maxValVec  = vec(*pMaxValLocal);

   vec     vIdx     = vec(0);
   vec     vCurrIdx = *((vec *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET));
   __vpred vpMask;

   DSPLIB_DEBUGPRINTFN(0, " mainLoopCount %d errCoefsSize %d vecInSize %d \n", mainLoopCount, errCoefsSize, vecInSize);

   for (i = 0; i < mainLoopCount; i++) {
      dotProduct = vec(0);

      vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec2 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven1 = __pack_consec_low(loadVec2, loadVec1);
      vec checkOdd1  = __pack_consec_high(loadVec2, loadVec1);

      vec loadVec3 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec4 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven2 = __pack_consec_low(loadVec4, loadVec3);
      vec checkOdd2  = __pack_consec_high(loadVec4, loadVec3);

      vec loadVec5 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec6 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven3 = __pack_consec_low(loadVec6, loadVec5);
      vec checkOdd3  = __pack_consec_high(loadVec6, loadVec5);

      vec loadVec7 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec8 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven4 = __pack_consec_low(loadVec8, loadVec7);
      vec checkOdd4  = __pack_consec_high(loadVec8, loadVec7);

      vec loadVec9  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec10 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven5 = __pack_consec_low(loadVec10, loadVec9);

      vec dotProduct1 = (checkEven1 * errCoefs1);
      vec dotProduct2 = (checkOdd1 * errCoefs2);
      vec dotProduct3 = (checkEven2 * errCoefs3);
      vec dotProduct4 = (checkOdd2 * errCoefs4);
      vec dotProduct5 = (checkEven3 * errCoefs5);
      vec dotProduct6 = (checkOdd3 * errCoefs6);
      vec dotProduct7 = (checkEven4 * errCoefs7);
      vec dotProduct8 = (checkOdd4 * errCoefs8);
      vec dotProduct9 = (checkEven5 * errCoefs9);

      vec acc1   = (__shift_right(dotProduct1, vec(1))) + (__shift_right(dotProduct2, vec(1)));
      vec acc2   = (__shift_right(dotProduct3, vec(1))) + (__shift_right(dotProduct4, vec(1)));
      vec acc3   = (__shift_right(dotProduct5, vec(1))) + (__shift_right(dotProduct6, vec(1)));
      vec acc4   = (__shift_right(dotProduct7, vec(1))) + (__shift_right(dotProduct8, vec(1)));
      vec acc5   = (__shift_right(acc1, vec(1))) + (__shift_right(acc2, vec(1)));
      vec acc6   = (__shift_right(acc3, vec(1))) + (__shift_right(acc4, vec(1)));
      vec acc7   = (__shift_right(acc5, vec(1))) + (__shift_right(acc6, vec(1)));
      dotProduct = (__shift_right(acc7, vec(1))) + (__shift_right(dotProduct9, vec(1)));

      // Vertical max comparison once the vector dot products are computed for se1 feteched data
      __max_index(dotProduct, maxValVec, vpMask);
      vIdx     = __select(vpMask, vCurrIdx, vIdx);
      vCurrIdx = vCurrIdx + (eleCount);
   }

   // Horizontal max computation
   c7x_horizontal_max_with_index(maxValVec, vIdx, pMaxValLocal, pMaxIndexLocal);

   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}

// supports both signed and unsigned datatypes
template <typename Integer8BitDataType, typename Integer8BitPromotedDataType>
inline void minerror_exec_ci_integer8_inputs(void *restrict pErrCoefs,
                                             const int *restrict pMaxIndex,
                                             const void *restrict pMaxVal,
                                             uint8_t *restrict pBlock,
                                             uint32_t vecInSize,
                                             uint32_t errCoefsSize,
                                             int32_t  mainLoopCount)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   // explicit promotion to 16 bit to handle overflow
   // eg: for 8bits, multiply and accumulate with right shift in 16bits

   Integer8BitDataType *restrict pErrCoefsLocal       = (Integer8BitDataType *) pErrCoefs;
   int *restrict pMaxIndexLocal                       = (int *) pMaxIndex;
   Integer8BitPromotedDataType *restrict pMaxValLocal = (Integer8BitPromotedDataType *) pMaxVal;

   typedef typename c7x::make_full_vector<Integer8BitDataType>::type vec;
   uint8_t                                                           eleCount = c7x::element_count_of<vec>::value;

   typedef typename c7x::make_full_vector<Integer8BitPromotedDataType>::type vecPromoted;

   typedef typename c7x::make_vector<Integer8BitDataType, __C7X_VEC_SIZE_BYTES__ / 2>::type vecPartial;

   *pMaxValLocal = (Integer8BitPromotedDataType) (std::numeric_limits<Integer8BitPromotedDataType>::min());

   vecPromoted dotProductEven = vecPromoted(0);
   vecPromoted dotProductOdd  = vecPromoted(0);
   vecPromoted maxValVec      = vecPromoted(*pMaxValLocal);

   vecPromoted vIdx     = vecPromoted(0);
   vec         vCurrIdx = *((vec *) ((uint8_t *) pBlock + CURR_IDX_VEC_OFFSET));

   __vpred vpMask;

   DSPLIB_DEBUGPRINTFN(0, "mainLoopCount %d errCoefsSize %d vecInSize %d \n", mainLoopCount, errCoefsSize, vecInSize);

#if __C7X_VEC_SIZE_BITS__ == 256

   vecPromoted errCoefs1 = vecPromoted(pErrCoefsLocal[0]);
   vecPromoted errCoefs2 = vecPromoted(pErrCoefsLocal[1]);
   vecPromoted errCoefs3 = vecPromoted(pErrCoefsLocal[2]);
   vec         errCoefs4 = vec(pErrCoefsLocal[3]);
   vec         errCoefs5 = vec(pErrCoefsLocal[4]);
   vec         errCoefs6 = vec(pErrCoefsLocal[5]);
   vec         errCoefs7 = vec(pErrCoefsLocal[6]);
   vec         errCoefs8 = vec(pErrCoefsLocal[7]);
   vec         errCoefs9 = vec(pErrCoefsLocal[8]);

   for (int i = 0; i < mainLoopCount; i++) {
      // 2SE fetch implementation
      vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec2 = c7x::strm_eng<0, vec>::get_adv();

      vec loadVec3 = c7x::strm_eng<1, vec>::get_adv();
      vec loadVec4 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven1 = __pack_consec_low(loadVec2, loadVec1);
      vec checkOdd1  = __pack_consec_high(loadVec2, loadVec1);
      vec checkEven2 = __pack_consec_low(loadVec4, loadVec3);
      vec checkOdd2  = __pack_consec_high(loadVec4, loadVec3);

      vec iEven1 = __pack_consec_low(checkEven2, checkEven1);
      vec iOdd1  = __pack_consec_low(checkOdd2, checkOdd1);
      vec iEven2 = __pack_consec_high(checkEven2, checkEven1);
      vec iOdd2  = __pack_consec_high(checkOdd2, checkOdd1);

      vec loadVec5 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec6 = c7x::strm_eng<0, vec>::get_adv();

      vec loadVec7 = c7x::strm_eng<1, vec>::get_adv();
      vec loadVec8 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven3 = __pack_consec_low(loadVec6, loadVec5);
      vec checkOdd3  = __pack_consec_high(loadVec6, loadVec5);
      vec checkEven4 = __pack_consec_low(loadVec8, loadVec7);
      vec checkOdd4  = __pack_consec_high(loadVec8, loadVec7);

      vec iEven3 = __pack_consec_low(checkEven4, checkEven3);
      vec iOdd3  = __pack_consec_low(checkOdd4, checkOdd3);
      vec iEven4 = __pack_consec_high(checkEven4, checkEven3);
      vec iOdd4  = __pack_consec_high(checkOdd4, checkOdd3);

      vec loadVec9  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec10 = c7x::strm_eng<0, vec>::get_adv();

      vec loadVec11 = c7x::strm_eng<1, vec>::get_adv();
      vec loadVec12 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven5 = __pack_consec_low(loadVec10, loadVec9);
      vec checkEven6 = __pack_consec_low(loadVec12, loadVec11);

      vec iEven5 = __pack_consec_low(checkEven6, checkEven5);

      vecPromoted dotProductEven1 = vecPromoted(0);
      vecPromoted dotProductEven2 = vecPromoted(0);
      vecPromoted dotProductEven3 = vecPromoted(0);
      vecPromoted dotProductEven4 = vecPromoted(0);
      vecPromoted dotProductEven5 = vecPromoted(0);
      vecPromoted dotProductEven6 = vecPromoted(0);
      vecPromoted dotProductEven7 = vecPromoted(0);
      vecPromoted dotProductEven8 = vecPromoted(0);
      vecPromoted dotProductEven9 = vecPromoted(0);

      vecPromoted dotProductOdd1 = vecPromoted(0);
      vecPromoted dotProductOdd2 = vecPromoted(0);
      vecPromoted dotProductOdd3 = vecPromoted(0);
      vecPromoted dotProductOdd4 = vecPromoted(0);
      vecPromoted dotProductOdd5 = vecPromoted(0);
      vecPromoted dotProductOdd6 = vecPromoted(0);
      vecPromoted dotProductOdd7 = vecPromoted(0);
      vecPromoted dotProductOdd8 = vecPromoted(0);
      vecPromoted dotProductOdd9 = vecPromoted(0);

      vecPromoted iEvenEvenShort1 = vecPromoted(0);
      vecPromoted iEvenOddShort1  = vecPromoted(0);
      vecPromoted iOddEvenShort1  = vecPromoted(0);
      vecPromoted iOddOddShort1   = vecPromoted(0);
      vecPromoted iEvenEvenShort2 = vecPromoted(0);
      vecPromoted iEvenOddShort2  = vecPromoted(0);

      iEvenEvenShort1 = convert_char_to_short<vecPromoted, vecPartial>(iEven1.even(), false);

      dotProductEven1 = (iEvenEvenShort1 * errCoefs1);
      iEvenOddShort1  = convert_char_to_short<vecPromoted, vecPartial>(iEven1.odd(), false);
      dotProductOdd1  = (iEvenOddShort1 * errCoefs1);

      iOddEvenShort1  = convert_char_to_short<vecPromoted, vecPartial>(iOdd1.even(), false);
      dotProductEven2 = (iOddEvenShort1 * errCoefs2);
      iOddOddShort1   = convert_char_to_short<vecPromoted, vecPartial>(iOdd1.odd(), false);
      dotProductOdd2  = (iOddOddShort1 * errCoefs2);

      iEvenEvenShort2 = convert_char_to_short<vecPromoted, vecPartial>(iEven2.even(), false);
      dotProductEven3 = (iEvenEvenShort2 * errCoefs3);
      iEvenOddShort2  = convert_char_to_short<vecPromoted, vecPartial>(iEven2.odd(), false);
      dotProductOdd3  = (iEvenOddShort2 * errCoefs3);

      mul_char_to_short<vecPromoted &, vec>(iOdd2, errCoefs4, dotProductEven4, dotProductOdd4);
      mul_char_to_short<vecPromoted &, vec>(iEven3, errCoefs5, dotProductEven5, dotProductOdd5);
      mul_char_to_short<vecPromoted &, vec>(iOdd3, errCoefs6, dotProductEven6, dotProductOdd6);
      mul_char_to_short<vecPromoted &, vec>(iEven4, errCoefs7, dotProductEven7, dotProductOdd7);
      mul_char_to_short<vecPromoted &, vec>(iOdd4, errCoefs8, dotProductEven8, dotProductOdd8);
      mul_char_to_short<vecPromoted &, vec>(iEven5, errCoefs9, dotProductEven9, dotProductOdd9);

      vecPromoted accEven1 =
          (__shift_right(dotProductEven1, vecPromoted(1))) + (__shift_right(dotProductEven2, vecPromoted(1)));
      vecPromoted accEven2 =
          (__shift_right(dotProductEven3, vecPromoted(1))) + (__shift_right(dotProductEven4, vecPromoted(1)));
      vecPromoted accEven3 =
          (__shift_right(dotProductEven5, vecPromoted(1))) + (__shift_right(dotProductEven6, vecPromoted(1)));
      vecPromoted accEven4 =
          (__shift_right(dotProductEven7, vecPromoted(1))) + (__shift_right(dotProductEven8, vecPromoted(1)));
      vecPromoted accEven5 = (__shift_right(accEven1, vecPromoted(1))) + (__shift_right(accEven2, vecPromoted(1)));
      vecPromoted accEven6 = (__shift_right(accEven3, vecPromoted(1))) + (__shift_right(accEven4, vecPromoted(1)));
      vecPromoted accEven7 = (__shift_right(accEven5, vecPromoted(1))) + (__shift_right(accEven6, vecPromoted(1)));
      dotProductEven = (__shift_right(accEven7, vecPromoted(1))) + (__shift_right(dotProductEven9, vecPromoted(1)));

      // Vertical max comparison once the vector dot products are computed for se0 feteched data
      __max_index(dotProductEven, maxValVec, vpMask);
      vecPromoted vCurrIdxPrmt;
      vCurrIdxPrmt = convert_char_to_short<vecPromoted, vecPartial>(vCurrIdx.even(), true);
      vIdx         = __select(vpMask, vCurrIdxPrmt, vIdx);

      vecPromoted accOdd1 =
          (__shift_right(dotProductOdd1, vecPromoted(1))) + (__shift_right(dotProductOdd2, vecPromoted(1)));
      vecPromoted accOdd2 =
          (__shift_right(dotProductOdd3, vecPromoted(1))) + (__shift_right(dotProductOdd4, vecPromoted(1)));
      vecPromoted accOdd3 =
          (__shift_right(dotProductOdd5, vecPromoted(1))) + (__shift_right(dotProductOdd6, vecPromoted(1)));
      vecPromoted accOdd4 =
          (__shift_right(dotProductOdd7, vecPromoted(1))) + (__shift_right(dotProductOdd8, vecPromoted(1)));
      vecPromoted accOdd5 = (__shift_right(accOdd1, vecPromoted(1))) + (__shift_right(accOdd2, vecPromoted(1)));
      vecPromoted accOdd6 = (__shift_right(accOdd3, vecPromoted(1))) + (__shift_right(accOdd4, vecPromoted(1)));
      vecPromoted accOdd7 = (__shift_right(accOdd5, vecPromoted(1))) + (__shift_right(accOdd6, vecPromoted(1)));
      dotProductOdd       = (__shift_right(accOdd7, vecPromoted(1))) + (__shift_right(dotProductOdd9, vecPromoted(1)));

      // Vertical max comparison once the vector dot products are computed for se1 feteched data
      __max_index(dotProductOdd, maxValVec, vpMask);
      vCurrIdxPrmt = convert_char_to_short<vecPromoted, vecPartial>(vCurrIdx.odd(), true);
      vIdx         = __select(vpMask, vCurrIdxPrmt, vIdx);
      vCurrIdx     = vCurrIdx + vec(eleCount);
   }

#else
   vecPromoted errCoefs1 = vecPromoted(pErrCoefsLocal[0]);
   vecPromoted errCoefs2 = vecPromoted(pErrCoefsLocal[1]);
   vecPromoted errCoefs3 = vecPromoted(pErrCoefsLocal[2]);
   vecPartial errCoefs4 = vecPartial(pErrCoefsLocal[3]);
   vecPartial errCoefs5 = vecPartial(pErrCoefsLocal[4]);
   vecPartial errCoefs6 = vecPartial(pErrCoefsLocal[5]);
   vecPartial errCoefs7 = vecPartial(pErrCoefsLocal[6]);
   vecPartial errCoefs8 = vecPartial(pErrCoefsLocal[7]);
   vecPartial errCoefs9 = vecPartial(pErrCoefsLocal[8]);

   for (int i = 0; i < mainLoopCount; i++) {
      dotProductEven = vecPromoted(0);
      dotProductOdd = vecPromoted(0);

      // 2SE fetch implementation

      vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec2 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven1 = __pack_consec_low(loadVec2, loadVec1);
      vec checkOdd1 = __pack_consec_high(loadVec2, loadVec1);

      vecPartial iEven1 = checkEven1.even();
      vecPartial iOdd1 = checkOdd1.even();

      vecPartial iEven2 = checkEven1.odd();
      vecPartial iOdd2 = checkOdd1.odd();

      vec loadVec3 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec4 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven2 = __pack_consec_low(loadVec4, loadVec3);
      vec checkOdd2 = __pack_consec_high(loadVec4, loadVec3);

      vecPartial iEven3 = checkEven2.even();
      vecPartial iOdd3 = checkOdd2.even();
      vecPartial iEven4 = checkEven2.odd();
      vecPartial iOdd4 = checkOdd2.odd();

      vec loadVec5 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec6 = c7x::strm_eng<1, vec>::get_adv();

      vec checkEven3 = __pack_consec_low(loadVec6, loadVec5);

      vecPartial iEven5 = checkEven3.even();

      vecPromoted dotProduct1 = vecPromoted(0);
      vecPromoted dotProduct2 = vecPromoted(0);
      vecPromoted dotProduct3 = vecPromoted(0);
      vecPromoted dotProduct4 = vecPromoted(0);
      vecPromoted dotProduct5 = vecPromoted(0);
      vecPromoted dotProduct6 = vecPromoted(0);
      vecPromoted dotProduct7 = vecPromoted(0);
      vecPromoted dotProduct8 = vecPromoted(0);
      vecPromoted dotProduct9 = vecPromoted(0);

      vecPromoted iColShort1 = vecPromoted(0);
      vecPromoted iColShort2 = vecPromoted(0);
      vecPromoted iColShort3 = vecPromoted(0);

      iColShort1 = convert_char_to_short<vecPromoted, vecPartial>(iEven1, false);
      dotProduct1 = (iColShort1 * errCoefs1);

      iColShort2 = convert_char_to_short<vecPromoted, vecPartial>(iOdd1, false);
      dotProduct2 = (iColShort2 * errCoefs2);

      iColShort3 = convert_char_to_short<vecPromoted, vecPartial>(iEven2, false);
      dotProduct3 = (iColShort3 * errCoefs3);

      mul_char_to_short<vecPromoted &, vecPartial>(iOdd2, errCoefs4, dotProduct4, dotProduct);
      mul_char_to_short<vecPromoted &, vecPartial>(iEven3, errCoefs5, dotProduct5, dotProduct);
      mul_char_to_short<vecPromoted &, vecPartial>(iOdd3, errCoefs6, dotProduct6, dotProduct);
      mul_char_to_short<vecPromoted &, vecPartial>(iEven4, errCoefs7, dotProduct7, dotProduct);
      mul_char_to_short<vecPromoted &, vecPartial>(iOdd4, errCoefs8, dotProduct8, dotProduct);
      mul_char_to_short<vecPromoted &, vecPartial>(iEven5, errCoefs9, dotProduct9, dotProduct);

      vecPromoted acc1 = (__shift_right(dotProduct1, vecPromoted(1))) + (__shift_right(dotProduct2, vecPromoted(1)));
      vecPromoted acc2 = (__shift_right(dotProduct3, vecPromoted(1))) + (__shift_right(dotProduct4, vecPromoted(1)));
      vecPromoted acc3 = (__shift_right(dotProduct5, vecPromoted(1))) + (__shift_right(dotProduct6, vecPromoted(1)));
      vecPromoted acc4 = (__shift_right(dotProduct7, vecPromoted(1))) + (__shift_right(dotProduct8, vecPromoted(1)));
      vecPromoted acc5 = (__shift_right(acc1, vecPromoted(1))) + (__shift_right(acc2, vecPromoted(1)));
      vecPromoted acc6 = (__shift_right(acc3, vecPromoted(1))) + (__shift_right(acc4, vecPromoted(1)));
      vecPromoted acc7 = (__shift_right(acc5, vecPromoted(1))) + (__shift_right(acc6, vecPromoted(1)));
      dotProduct = (__shift_right(acc7, vecPromoted(1))) + (__shift_right(dotProduct9, vecPromoted(1)));

      vecPromoted vCurrIdxPrmt;

      // Vertical max comparison once the vector dot products are computed for se feteched data
      __max_index(dotProduct, maxValVec, vpMask);
      vCurrIdxPrmt = convert_char_to_short<vecPromoted, vecPartial>(vCurrIdx.lo(), true);
      vIdx = __select(vpMask, vCurrIdxPrmt, vIdx);
      vCurrIdx = vCurrIdx + vec(eleCount / 2);
   }
#endif

   // Horizontal max computation
   c7x_horizontal_max_with_index(maxValVec, vIdx, pMaxValLocal, pMaxIndexLocal);

   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<int32_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                              dataSize = 4;
   int32_t                                               strideIn = pKerPrivArgs->strideIn;

   __SE_TEMPLATE_v1 se0Params;

   int32_t *restrict pInLocal = (int32_t *) pIn;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * eleCount / 2), se0Params);

   minerror_exec_ci_integer32_inputs<int32_t, int64_t>(pErrCoefs, pMaxIndex, pMaxVal, pBlock, pKerPrivArgs->vecInSize,
                                                       pKerPrivArgs->errCoefsSize, pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<uint32_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pErrCoefs,
                                                const int *restrict pMaxIndex,
                                                const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<uint32_t>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                               dataSize = 4;
   int32_t                                                strideIn = pKerPrivArgs->strideIn;
   __SE_TEMPLATE_v1                                       se0Params;

   uint32_t *restrict pInLocal = (uint32_t *) pIn;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * eleCount / 2), se0Params);

   minerror_exec_ci_integer32_inputs<uint32_t, uint64_t>(pErrCoefs, pMaxIndex, pMaxVal, pBlock, pKerPrivArgs->vecInSize,
                                                         pKerPrivArgs->errCoefsSize, pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<int64_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs                             *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;
   typedef typename c7x::make_full_vector<int64_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                              dataSize = 8;
   int32_t                                               strideIn = pKerPrivArgs->strideIn;

   __SE_TEMPLATE_v1 se0Params;

   int64_t *restrict pInLocal = (int64_t *) pIn;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * eleCount), se0Params);

   minerror_exec_ci_signed_integer64_inputs<int64_t, double>(pErrCoefs, pMaxIndex, pMaxVal, pBlock,
                                                             pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize,
                                                             pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<uint64_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pErrCoefs,
                                                const int *restrict pMaxIndex,
                                                const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs                              *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;
   typedef typename c7x::make_full_vector<uint64_t>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                               dataSize = 8;
   int32_t                                                strideIn = pKerPrivArgs->strideIn;

   __SE_TEMPLATE_v1 se0Params;

   uint64_t *restrict pInLocal = (uint64_t *) pIn;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * eleCount), se0Params);

   minerror_exec_ci_unsigned_integer64_inputs<uint64_t, double>(pErrCoefs, pMaxIndex, pMaxVal, pBlock,
                                                                pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize,
                                                                pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<float>(DSPLIB_kernelHandle handle,
                                             void *restrict pIn,
                                             void *restrict pErrCoefs,
                                             const int *restrict pMaxIndex,
                                             const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<float>::type vec;
   int32_t                                             eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                            dataSize = 4;
   int32_t                                             strideIn = pKerPrivArgs->strideIn;

   __SE_TEMPLATE_v1 se0Params;

   float *restrict pInLocal = (float *) pIn;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * eleCount), se0Params);

   minerror_exec_ci_float_inputs<float>(pErrCoefs, pMaxIndex, pMaxVal, pBlock, pKerPrivArgs->vecInSize,
                                        pKerPrivArgs->errCoefsSize, pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<double>(DSPLIB_kernelHandle handle,
                                              void *restrict pIn,
                                              void *restrict pErrCoefs,
                                              const int *restrict pMaxIndex,
                                              const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<double>::type vec;
   int32_t                                              eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                             dataSize = 8;
   int32_t                                              strideIn = pKerPrivArgs->strideIn;

   __SE_TEMPLATE_v1 se0Params;

   double *restrict pInLocal = (double *) pIn;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * eleCount), se0Params);

   minerror_exec_ci_float_inputs<double>(pErrCoefs, pMaxIndex, pMaxVal, pBlock, pKerPrivArgs->vecInSize,
                                         pKerPrivArgs->errCoefsSize, pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                              void *restrict pIn,
                                              void *restrict pErrCoefs,
                                              const int *restrict pMaxIndex,
                                              const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   __SE_TEMPLATE_v1 se0Params;

   int8_t *restrict pInLocal = (int8_t *) pIn;

   uint8_t                                             *pBlock = pKerPrivArgs->bufPblock;
   typedef typename c7x::make_full_vector<int8_t>::type vec;
   int32_t                                              eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                             dataSize = 1;
   int32_t                                              strideIn = pKerPrivArgs->strideIn;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);

#if __C7X_VEC_SIZE_BITS__ == 256
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * (eleCount / 2)), se0Params);
#else
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * (eleCount / 4)), se0Params);
#endif

   minerror_exec_ci_integer8_inputs<int8_t, int16_t>(pErrCoefs, pMaxIndex, pMaxVal, pBlock, pKerPrivArgs->vecInSize,
                                                     pKerPrivArgs->errCoefsSize, pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<uint8_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   __SE_TEMPLATE_v1 se0Params;

   uint8_t *restrict pInLocal = (uint8_t *) pIn;

   uint8_t                                              *pBlock = pKerPrivArgs->bufPblock;
   typedef typename c7x::make_full_vector<uint8_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                              dataSize = 1;
   int32_t                                               strideIn = pKerPrivArgs->strideIn;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);

#if __C7X_VEC_SIZE_BITS__ == 256
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * (eleCount / 2)), se0Params);
#else
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * (eleCount / 4)), se0Params);
#endif

   minerror_exec_ci_integer8_inputs<uint8_t, uint16_t>(pErrCoefs, pMaxIndex, pMaxVal, pBlock, pKerPrivArgs->vecInSize,
                                                       pKerPrivArgs->errCoefsSize, pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   __SE_TEMPLATE_v1 se0Params;

   int16_t *restrict pInLocal = (int16_t *) pIn;

   uint8_t                                              *pBlock = pKerPrivArgs->bufPblock;
   typedef typename c7x::make_full_vector<int16_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                              dataSize = 2;
   int32_t                                               strideIn = pKerPrivArgs->strideIn;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * (eleCount / 4)), se0Params);

   minerror_exec_ci_integer16_inputs<int16_t, int32_t>(pErrCoefs, pMaxIndex, pMaxVal, pBlock, pKerPrivArgs->vecInSize,
                                                       pKerPrivArgs->errCoefsSize, pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_ci<uint16_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pErrCoefs,
                                                const int *restrict pMaxIndex,
                                                const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   __SE_TEMPLATE_v1 se0Params;

   uint16_t *restrict pInLocal = (uint16_t *) pIn;

   uint8_t                                               *pBlock = pKerPrivArgs->bufPblock;
   typedef typename c7x::make_full_vector<uint16_t>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                               dataSize = 2;
   int32_t                                                strideIn = pKerPrivArgs->strideIn;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * (eleCount / 4)), se0Params);

   minerror_exec_ci_integer16_inputs<uint16_t, uint32_t>(pErrCoefs, pMaxIndex, pMaxVal, pBlock, pKerPrivArgs->vecInSize,
                                                         pKerPrivArgs->errCoefsSize, pKerPrivArgs->mainLoopCount);

   __SE1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exit function with %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}
