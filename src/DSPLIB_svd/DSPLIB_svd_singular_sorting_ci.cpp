// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_priv.h"

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */

template <typename dataType> void DSPLIB_singularSort_swap_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_svd_PrivArgs *pKerPrivArgs = (DSPLIB_svd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   __SE_TEMPLATE_v1     se0Params    = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1     sa2Params    = __gen_SA_TEMPLATE_v1();
   __SE_TEMPLATE_v1     se1Params    = __gen_SE_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;

   se0Params.DIMFMT  = __SE_DIMFMT_2D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;

   se1Params.ICNT0   = eleCount;
   se1Params.DIM1    = eleCount;
   se1Params.DIM2    = 0;
   se1Params.DIMFMT  = __SE_DIMFMT_3D;
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN  = SE_VECLEN;
   se1Params.DECDIM1 = __SE_DECDIM_DIM1;

   sa2Params.DIMFMT = __SA_DIMFMT_1D;
   sa2Params.VECLEN = __SA_VECLEN_1ELEM;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (22 * SE_PARAM_SIZE)) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (23 * SE_PARAM_SIZE)) = sa2Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (24 * SE_PARAM_SIZE)) = se1Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_singularSort_swap_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_singularSort_swap_init_ci<double>(DSPLIB_kernelHandle handle);

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */

/*!
 *  @brief       This function sorts the singular values in descending order and also
 *               records the max index values for shuffling of U and V matrix columns.
 */
template <typename dataType>
void DSPLIB_singularSort_index_ci(dataType *singular_values,
                                  dataType *singularBuffer,
                                  int32_t  *maxIndArr,
                                  int32_t   Ncols,
                                  uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (24 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   int32_t nVec = DSPLIB_ceilingDiv(Ncols, eleCount);

   se0Params.ICNT1         = nVec;
   se0Params.ICNT2         = Ncols;
   se0Params.DECDIM1_WIDTH = Ncols;

   __SE0_OPEN(singularBuffer, se0Params);

   dataType maxVal;
   int32_t  maxInd;
   vec      idx_0_to_eleCount;
   for (int32_t i = 0; i < eleCount; i++) {
      idx_0_to_eleCount.s[i] = i;
   }

   for (int32_t i = 0; i < Ncols; i++) {
      __SE0_OPEN(singularBuffer, se0Params);
      vec vCurrIdx  = idx_0_to_eleCount;
      vec vIdx      = idx_0_to_eleCount;
      vec maxValVec = (vec) (-std::numeric_limits<dataType>::max());

      for (int32_t k = 0; k < nVec; k++) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();

         __vpred vpMask = __cmp_le_pred(v1, maxValVec);
         maxValVec      = __select(vpMask, maxValVec, v1);
         vIdx           = __select(vpMask, vIdx, vCurrIdx);

         vCurrIdx = vCurrIdx + (eleCount);
      }
      c7x_horizontal_max_with_index(maxValVec, vIdx, &maxVal, &maxInd);

      __vpred minIdxPred  = __cmp_eq_pred((vec) maxVal, maxValVec);
      vec     equalValIdx = __select(minIdxPred, vIdx, (vec) std::numeric_limits<dataType>::max());
      maxInd              = (int32_t) c7x_horizontal_min_fp<dataType, vec>(equalValIdx);

      singularBuffer[maxInd] = -std::numeric_limits<dataType>::max();
      maxIndArr[maxInd]      = i;
      singular_values[i]     = maxVal;
      __SE0_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}

template void DSPLIB_singularSort_index_ci<float>(float   *singular_values,
                                                  float   *singularBuffer,
                                                  int32_t *maxIndArr,
                                                  int32_t  Ncols,
                                                  uint8_t *pBlock);
template void DSPLIB_singularSort_index_ci<double>(double  *singular_values,
                                                   double  *singularBuffer,
                                                   int32_t *maxIndArr,
                                                   int32_t  Ncols,
                                                   uint8_t *pBlock);

/*!
 *  @brief       This function uses the max index values calculated from
 *               @ref DSPLIB_singularSort_index_ci to shuffle the rows of U' and V' matrix
 *               (i.e. colums of U and V matrix).
 */
template <typename dataType>
void DSPLIB_singularSort_swap_ci(dataType *V,
                                 int32_t   Nrows,
                                 int32_t   Ncols,
                                 int32_t   rowVStride,
                                 int32_t  *sortIndex,
                                 dataType *vBuff,
                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 se0Params, se1Params;
   __SA_TEMPLATE_v1 sa0Params, sa1Params, sa2Params, sa3Params;
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (22 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (22 * SE_PARAM_SIZE));
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (23 * SE_PARAM_SIZE));
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (23 * SE_PARAM_SIZE));

   int32_t nVec     = DSPLIB_ceilingDiv(Ncols, eleCount);
   int32_t se0ICNT1 = Nrows / 2;
   int32_t se1ICNT1 = Nrows - se0ICNT1;

   se0Params.ICNT0 = sa0Params.ICNT0 = Ncols;
   se0Params.ICNT1 = sa0Params.ICNT1 = se0ICNT1;

   se1Params.ICNT0 = sa1Params.ICNT0 = Ncols;
   se1Params.ICNT1 = sa1Params.ICNT1 = se1ICNT1;

   se0Params.DIM1 = se1Params.DIM1 = rowVStride;

   sa2Params.ICNT0 = se0ICNT1;
   sa3Params.ICNT0 = se1ICNT1;

   __SE1_OPEN(V + (rowVStride * se0ICNT1), se1Params);
   __SA1_OPEN(sa1Params);
   __SA3_OPEN(sa3Params);

   if (se0ICNT1 > 0) {
      __SE0_OPEN(V, se0Params);
      __SA0_OPEN(sa0Params);
      __SA2_OPEN(sa2Params);

      for (int32_t vertical = 0; vertical < se0ICNT1; vertical++) {
         int32_t *pIndex1 = c7x::strm_agen<2, int32_t>::get_adv(sortIndex);
         int32_t *pIndex2 = c7x::strm_agen<3, int32_t>::get_adv(sortIndex + se0ICNT1);
         int32_t  offset1 = *pIndex1 * rowVStride;
         int32_t  offset2 = *pIndex2 * rowVStride;

         for (int32_t horizontal = 0; horizontal < nVec; horizontal++) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            __vpred pred1   = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pStore1 = c7x::strm_agen<0, vec>::get_adv(vBuff + offset1);
            __vstore_pred(pred1, pStore1, v1);

            __vpred pred2   = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStore2 = c7x::strm_agen<1, vec>::get_adv(vBuff + offset2);
            __vstore_pred(pred2, pStore2, v2);
         }
      }
   }

   if (se0ICNT1 != se1ICNT1) {
      int32_t *pIndex2 = c7x::strm_agen<3, int32_t>::get_adv(sortIndex + se0ICNT1);
      int32_t  offset2 = *pIndex2 * rowVStride;
      for (int32_t horizontal = 0; horizontal < nVec; horizontal++) {
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         __vpred pred2   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStore2 = c7x::strm_agen<1, vec>::get_adv(vBuff + offset2);
         __vstore_pred(pred2, pStore2, v2);
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_singularSort_swap_ci<float>(float   *V,
                                                 int32_t  Nrows,
                                                 int32_t  Ncols,
                                                 int32_t  rowVStride,
                                                 int32_t *sortIndex,
                                                 float   *vBuff,
                                                 uint8_t *pBlock);
template void DSPLIB_singularSort_swap_ci<double>(double  *V,
                                                  int32_t  Nrows,
                                                  int32_t  Ncols,
                                                  int32_t  rowVStride,
                                                  int32_t *sortIndex,
                                                  double  *vBuff,
                                                  uint8_t *pBlock);
