// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_common.h"
#include "DSPLIB_lud_inv_priv.h"

#define LUD_INV_HIGH_PRECISION
/**********************************************************************
 *
 * INITIALIZATION
 *
 *********************************************************************/

template <typename dataType> void DSPLIB_lud_inv_permuteRows_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_lud_inv_PrivArgs *pKerPrivArgs = (DSPLIB_lud_inv_PrivArgs *) handle;
   uint8_t                 *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                  order        = pKerPrivArgs->order;
   int32_t                  stride       = pKerPrivArgs->strideOrder;
   int32_t                  colStride    = stride / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;

   __SE_TEMPLATE_v1 seMatReadParams = __gen_SE_TEMPLATE_v1();
   seMatReadParams.ICNT0            = order;
   seMatReadParams.DIM1             = colStride * 2;
   seMatReadParams.DIMFMT           = __SE_DIMFMT_2D;
   seMatReadParams.ELETYPE          = SE_ELETYPE;
   seMatReadParams.VECLEN           = SE_VECLEN;

   __SA_TEMPLATE_v1 saRowWriteParams = __gen_SA_TEMPLATE_v1();
   saRowWriteParams.ICNT0            = order;
   saRowWriteParams.DIM1             = 0;
   saRowWriteParams.DIMFMT           = __SA_DIMFMT_2D;
   saRowWriteParams.VECLEN           = SA_VECLEN;

   __SA_TEMPLATE_v1 saPermParams = __gen_SA_TEMPLATE_v1();
   saPermParams.ICNT0            = order;
   saPermParams.DIMFMT           = __SA_DIMFMT_1D;
   saPermParams.VECLEN           = __SA_VECLEN_1ELEM;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE)) = seMatReadParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE)) = saRowWriteParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE)) = saPermParams;

   typedef typename c7x::make_full_vector<uint16_t>::type vecUINT16;
   SE_ELETYPE         = c7x::se_eletype<vecUINT16>::value;
   SE_VECLEN          = c7x::se_veclen<vecUINT16>::value;
   SA_VECLEN          = c7x::sa_veclen<vecUINT16>::value;
   int32_t pStride    = pKerPrivArgs->strideP;
   int32_t colPStride = pStride / sizeof(uint16_t);

   seMatReadParams         = __gen_SE_TEMPLATE_v1();
   seMatReadParams.ICNT0   = order;
   seMatReadParams.DIM1    = colPStride * 2;
   seMatReadParams.DIMFMT  = __SE_DIMFMT_2D;
   seMatReadParams.ELETYPE = SE_ELETYPE;
   seMatReadParams.VECLEN  = SE_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE)) = seMatReadParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_inv_permuteRows_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_lud_inv_permuteRows_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_lud_inv_opt_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_lud_inv_PrivArgs *pKerPrivArgs   = (DSPLIB_lud_inv_PrivArgs *) handle;
   uint8_t                 *pBlock         = pKerPrivArgs->bufPblock;
   int32_t                  strideOrder    = pKerPrivArgs->strideOrder;
   int32_t                  order          = pKerPrivArgs->order;
   int32_t                  colStrideOrder = strideOrder / sizeof(dataType);

   DSPLIB_lud_identity_matrix_generate_init_ci<dataType>(pBlock, order, strideOrder);

   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   /* Fact copy params*/
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint32_t eleCount = c7x::element_count_of<vec>::value;
   se0Params.ICNT0   = 1;
   se0Params.ICNT1   = eleCount;
   se0Params.DIM1    = colStrideOrder;
   se0Params.DIM2    = colStrideOrder * eleCount;
   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   if (sizeof(dataType) == 4) {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   sa0Params.ICNT0  = order;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE)) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = sa0Params;
   /* Mat-update Params */

   int32_t lenTile8 = 8;
   int32_t nTiles_8 = DSPLIB_ceilingDiv(order, (eleCount * lenTile8));

   __SE_ELEDUP SE_ELEDUP = c7x::se_eledup<dataType>::value;

   __SE_TEMPLATE_v1 seScalarParams = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 seMatrixParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saMatrixParams = __gen_SA_TEMPLATE_v1();

   seScalarParams.DIM1    = 0;
   seScalarParams.ELEDUP  = SE_ELEDUP;
   seScalarParams.DIMFMT  = __SE_DIMFMT_2D;
   seScalarParams.VECLEN  = SE_VECLEN;
   seScalarParams.ELETYPE = SE_ELETYPE;

   seMatrixParams.ICNT0         = (eleCount * lenTile8);
   seMatrixParams.DIM1          = colStrideOrder;
   seMatrixParams.ICNT2         = nTiles_8;
   seMatrixParams.DIM2          = (eleCount * lenTile8);
   seMatrixParams.DIMFMT        = __SE_DIMFMT_3D;
   seMatrixParams.ELETYPE       = SE_ELETYPE;
   seMatrixParams.VECLEN        = SE_VECLEN;
   seMatrixParams.DECDIM1       = __SE_DECDIM_DIM2;
   seMatrixParams.DECDIM1_WIDTH = order;

   saMatrixParams.ICNT0         = (eleCount * lenTile8);
   saMatrixParams.DIM1          = colStrideOrder;
   saMatrixParams.ICNT2         = nTiles_8;
   saMatrixParams.DIM2          = (eleCount * lenTile8);
   saMatrixParams.DIMFMT        = __SA_DIMFMT_3D;
   saMatrixParams.VECLEN        = SA_VECLEN;
   saMatrixParams.DECDIM1       = __SA_DECDIM_DIM2;
   saMatrixParams.DECDIM1_WIDTH = order;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = seScalarParams;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE)) = seMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE)) = saMatrixParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_inv_opt_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_lud_inv_opt_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType>
DSPLIB_STATUS DSPLIB_lud_inv_init_ci(DSPLIB_kernelHandle           handle,
                                     const DSPLIB_bufParams2D_t   *bufParamsP,
                                     const DSPLIB_bufParams2D_t   *bufParamsL,
                                     const DSPLIB_bufParams2D_t   *bufParamsU,
                                     const DSPLIB_bufParams2D_t   *bufParamsinvA,
                                     const DSPLIB_lud_invInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_lud_inv_PrivArgs *pKerPrivArgs   = (DSPLIB_lud_inv_PrivArgs *) handle;
   int32_t                  colStrideOrder = pKerPrivArgs->strideOrder / sizeof(dataType);

   DSPLIB_matMul_PrivArgs   *pMatMulKerPrivArgs   = &pKerPrivArgs->pMatMulKerPrivArgs;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs = &pKerPrivArgs->pMatTransKerPrivArgs;

   DSPLIB_matMul_InitArgs  kerInitArgsMatMul;
   DSPLIB_matTransInitArgs kerInitArgsMatTrans;

   kerInitArgsMatMul.funcStyle   = pKerInitArgs->funcStyle;
   kerInitArgsMatTrans.funcStyle = pKerInitArgs->funcStyle;
   kerInitArgsMatTrans.dimX      = bufParamsU->dim_x;
   kerInitArgsMatTrans.dimY      = bufParamsU->dim_y;

   pMatMulKerPrivArgs->strideIn0Elements = colStrideOrder;
   pMatMulKerPrivArgs->strideIn1Elements = colStrideOrder;
   pMatMulKerPrivArgs->strideOutElements = colStrideOrder;

   pMatTransKerPrivArgs->widthIn   = bufParamsU->dim_x;
   pMatTransKerPrivArgs->heightIn  = bufParamsU->dim_y;
   pMatTransKerPrivArgs->strideIn  = bufParamsU->stride_y;
   pMatTransKerPrivArgs->strideOut = bufParamsinvA->stride_y;

   pMatMulKerPrivArgs->M = pKerPrivArgs->order;
   pMatMulKerPrivArgs->N = pKerPrivArgs->order;
   pMatMulKerPrivArgs->K = pKerPrivArgs->order;

   DSPLIB_matTrans_init_ci<dataType>(pMatTransKerPrivArgs, bufParamsU, bufParamsinvA, &kerInitArgsMatTrans);
   DSPLIB_matMul_init_ci<dataType>(pMatMulKerPrivArgs, bufParamsL, bufParamsinvA, bufParamsU, &kerInitArgsMatMul);

   DSPLIB_lud_inv_opt_init_ci<dataType>(handle);
   DSPLIB_lud_inv_permuteRows_init_ci<dataType>(handle);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_lud_inv_init_ci<float>(DSPLIB_kernelHandle           handle,
                                                     const DSPLIB_bufParams2D_t   *bufParamsP,
                                                     const DSPLIB_bufParams2D_t   *bufParamsL,
                                                     const DSPLIB_bufParams2D_t   *bufParamsU,
                                                     const DSPLIB_bufParams2D_t   *bufParamsinvA,
                                                     const DSPLIB_lud_invInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_lud_inv_init_ci<double>(DSPLIB_kernelHandle           handle,
                                                      const DSPLIB_bufParams2D_t   *bufParamsP,
                                                      const DSPLIB_bufParams2D_t   *bufParamsL,
                                                      const DSPLIB_bufParams2D_t   *bufParamsU,
                                                      const DSPLIB_bufParams2D_t   *bufParamsinvA,
                                                      const DSPLIB_lud_invInitArgs *pKerInitArgs);

/**********************************************************************
 *
 * IMPLEMENTATION
 *
 *********************************************************************/

template <typename dataType>
void DSPLIB_lud_inv_permuteRows_ci(dataType *pIn,
                                   int32_t   order,
                                   int32_t   colStride,
                                   uint32_t *permuteOrder,
                                   dataType *pOut,
                                   uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 se0Params, se1Params;
   __SA_TEMPLATE_v1 sa0Params, sa1Params, sa2Params;
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE));

   int32_t nVec     = DSPLIB_ceilingDiv(order, eleCount);
   int32_t se1ICNT1 = order / 2;
   int32_t se0ICNT1 = order - se1ICNT1;
   se0Params.ICNT1 = sa0Params.ICNT1 = se0ICNT1;
   se1Params.ICNT1 = sa1Params.ICNT1 = se1ICNT1;

   __SE0_OPEN(pIn, se0Params);
   __SA0_OPEN(sa0Params);
   __SA2_OPEN(sa2Params);

   if (se1ICNT1 > 0) {
      __SE1_OPEN(pIn + colStride, se1Params);
      __SA1_OPEN(sa1Params);

      for (int32_t vertical = 0; vertical < se1ICNT1; vertical++) {
         uint32_t *loadPerm1 = c7x::strm_agen<2, uint32_t>::get_adv(permuteOrder);
         uint32_t  offset1   = *loadPerm1 * colStride;

         uint32_t *loadPerm2 = c7x::strm_agen<2, uint32_t>::get_adv(permuteOrder);
         uint32_t  offset2   = *loadPerm2 * colStride;

         for (int32_t horizontal = 0; horizontal < nVec; horizontal++) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            __vpred pred1   = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pStore1 = c7x::strm_agen<0, vec>::get_adv(pOut + offset1);
            __vstore_pred(pred1, pStore1, v1);

            __vpred pred2   = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStore2 = c7x::strm_agen<1, vec>::get_adv(pOut + offset2);
            __vstore_pred(pred2, pStore2, v2);
         }
      }
   }

   if (se0ICNT1 != se1ICNT1) {
      uint32_t *loadPerm1 = c7x::strm_agen<2, uint32_t>::get_adv(permuteOrder);
      uint32_t  offset1   = *loadPerm1 * colStride;

      for (int32_t horizontal = 0; horizontal < nVec; horizontal++) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();

         __vpred pred1   = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStore1 = c7x::strm_agen<0, vec>::get_adv(pOut + offset1);
         __vstore_pred(pred1, pStore1, v1);
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_lud_inv_permuteRows_ci<float>(float    *pIn,
                                                   int32_t   order,
                                                   int32_t   colStride,
                                                   uint32_t *permuteOrder,
                                                   float    *pOut,
                                                   uint8_t  *pBlock);
template void DSPLIB_lud_inv_permuteRows_ci<double>(double   *pIn,
                                                    int32_t   order,
                                                    int32_t   colStride,
                                                    uint32_t *permuteOrder,
                                                    double   *pOut,
                                                    uint8_t  *pBlock);

void DSPLIB_lud_inv_permuteIndex_ci(unsigned short *pIn,
                                    int32_t         order,
                                    int32_t         colPStride,
                                    uint32_t       *permuteOrder,
                                    uint8_t        *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<uint16_t>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 se0Params, se1Params;
   __SA_TEMPLATE_v1 sa2Params;
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE));
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE));

   int32_t nVec     = DSPLIB_ceilingDiv(order, eleCount);
   int32_t se1ICNT1 = order / 2;
   int32_t se0ICNT1 = order - se1ICNT1;

   se0Params.ICNT1 = se0ICNT1;
   se1Params.ICNT1 = se1ICNT1;

   __SE0_OPEN(pIn, se0Params);
   __SA2_OPEN(sa2Params);

   vec vecZero = (vec) 0;
   vec vecOne  = (vec) 1;

   vec idx_0_to_eleCount;

#if defined(__C7504__) || defined(__C7524__)
   idx_0_to_eleCount = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

#else
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

         permuteOrder[vertical + 0] = finalIdx1;
         permuteOrder[vertical + 1] = finalIdx2;
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

      permuteOrder[vertical + 0] = finalIdx1;
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}

template <typename dataType, typename vec = typename c7x::make_full_vector<dataType>::type>
static inline dataType DSPLIB_lud_inv_factor_exec_ci(dataType        *pCol,
                                                     int32_t          colStride,
                                                     int32_t          nRows,
                                                     dataType        *pFactor,
                                                     vec              scaleVec,
                                                     uint8_t         *pBlock,
                                                     __SE_TEMPLATE_v1 se0Params,
                                                     __SE_TEMPLATE_v1 se1Params,
                                                     __SA_TEMPLATE_v1 sa0Params,
                                                     __SA_TEMPLATE_v1 sa1Params)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   dataType sum      = 0;
   uint32_t eleCount = c7x::element_count_of<vec>::value;
   int32_t  nVec     = DSPLIB_ceilingDiv(nRows, eleCount);
   int32_t  se0ICNT2 = nVec / 2;
   int32_t  se1ICNT2 = nVec - se0ICNT2;
   se0Params.ICNT2   = se0ICNT2;
   se1Params.ICNT2   = se1ICNT2;
   dataType *pSE0    = pCol;
   dataType *pSE1    = pCol + (se0ICNT2 * colStride * eleCount);

   __SE1_OPEN(pSE1, se1Params);
   if (se0ICNT2 > 0) {
      __SE0_OPEN(pSE0, se0Params);
   }

   int32_t vertical;
   sa0Params.ICNT0       = (se0ICNT2 * eleCount);
   sa1Params.ICNT0       = nRows - ((se0ICNT2 * eleCount));
   dataType *pFactorHalf = pFactor + (se0ICNT2 * eleCount);

   if (sa0Params.ICNT0) {
      __SA0_OPEN(sa0Params);
   }
   __SA1_OPEN(sa1Params);

   for (vertical = 0; vertical < se0ICNT2 - 1; vertical += 2) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<1, vec>::get_adv();

      v1 *= scaleVec;
      v2 *= scaleVec;
      v3 *= scaleVec;
      v4 *= scaleVec;
      __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pFactor);
      __vstore_pred(pred, pStoreVec, v1);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pFactorHalf);
      __vstore_pred(pred, pStoreVec, v2);

      pred      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pFactor);
      __vstore_pred(pred, pStoreVec, v3);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pFactorHalf);
      __vstore_pred(pred, pStoreVec, v4);
   }

   for (; vertical < se0ICNT2; vertical++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();

      v1 *= scaleVec;
      v2 *= scaleVec;

      __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pFactor);
      __vstore_pred(pred, pStoreVec, v1);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pFactorHalf);
      __vstore_pred(pred, pStoreVec, v2);
   }
   if (se0ICNT2 != se1ICNT2) {
      vec v1 = c7x::strm_eng<1, vec>::get_adv();

      v1 *= scaleVec;

      __vpred pred      = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pFactorHalf);
      __vstore_pred(pred, pStoreVec, v1);
   }

   if (sa0Params.ICNT0) {
      __SA0_CLOSE();
   }
   __SA1_CLOSE();
   __SE1_CLOSE();

   if (se0ICNT2 > 0) {
      __SE0_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return sum: %15.10f\n", sum);

   return sum;
}
template float DSPLIB_lud_inv_factor_exec_ci<float, typename c7x::make_full_vector<float>::type>(
    float                                      *pCol,
    int32_t                                     colStride,
    int32_t                                     nRows,
    float                                      *pFactor,
    typename c7x::make_full_vector<float>::type scale,
    uint8_t                                    *pBlock,
    __SE_TEMPLATE_v1                            se0Params,
    __SE_TEMPLATE_v1                            se1Params,
    __SA_TEMPLATE_v1                            sa0Params,
    __SA_TEMPLATE_v1                            sa1Params);
template double DSPLIB_lud_inv_factor_exec_ci<double, typename c7x::make_full_vector<double>::type>(
    double                                      *pCol,
    int32_t                                      colStride,
    int32_t                                      nRows,
    double                                      *pFactor,
    typename c7x::make_full_vector<double>::type scale,
    uint8_t                                     *pBlock,
    __SE_TEMPLATE_v1                             se0Params,
    __SE_TEMPLATE_v1                             se1Params,
    __SA_TEMPLATE_v1                             sa0Params,
    __SA_TEMPLATE_v1                             sa1Params);

template <typename dataType>
void DSPLIB_lud_inv_invU_exec_ci(dataType *pLocalU,
                                 dataType *pLocalInvU,
                                 int32_t   nCols,
                                 int32_t   colStrideOrder,
                                 int32_t   colInvAStride,
                                 dataType *factArray,
                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;

   /* Factor copy params */
   __SE_TEMPLATE_v1 se0ParamsFact = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 se1ParamsFact = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0ParamsFact = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa1ParamsFact = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   /* Updation params */
   __SE_TEMPLATE_v1 seScalarParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 seMatrixParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saMatrixParams   = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saRefParams      = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saRefStoreParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   int32_t lenTile8 = 8;
   int32_t lenTile4 = 4;
   int32_t lenTile2 = 2;
   int32_t lenTile1 = 1;

   int32_t nTiles1 = DSPLIB_ceilingDiv(nCols, (eleCount));
   int32_t nTiles8 = nTiles1 / lenTile8; // left shift
   nTiles1 -= nTiles8 * lenTile8;
   int32_t nTiles4 = nTiles1 / lenTile4;
   nTiles1 -= nTiles4 * lenTile4;
   int32_t nTiles2 = nTiles1 / lenTile2;
   nTiles1 -= nTiles2 * lenTile2;

   int32_t remainingCols = nCols; // comment
   int32_t colLimit8     = nTiles8 * lenTile8 * eleCount;
   colLimit8             = (remainingCols < colLimit8) ? remainingCols : colLimit8;

   remainingCols     = remainingCols - colLimit8;
   int32_t colLimit4 = nTiles4 * lenTile4 * eleCount;
   colLimit4         = (remainingCols < colLimit4) ? remainingCols : colLimit4;

   remainingCols     = remainingCols - colLimit4;
   int32_t colLimit2 = nTiles2 * lenTile2 * eleCount;
   colLimit2         = (remainingCols < colLimit2) ? remainingCols : colLimit2;

   int32_t colLimit1    = remainingCols - colLimit2;
   seScalarParams.ICNT1 = 2 * (nTiles8 + nTiles4 + nTiles2 + nTiles1);

   for (int32_t col = nCols - 1; col >= 0; col--) {
      dataType *pLastU    = pLocalU + (colStrideOrder * col);
      dataType *pLastInvU = pLocalInvU + (colStrideOrder * col);

      dataType diag = pLocalU[col + col * colStrideOrder];
#ifdef LUD_INV_HIGH_PRECISION
      vec divVec = (vec) (1 / diag);
#else
      dataType recipScalar = __recip(diag);
      dataType twoP0       = 2.0;

      recipScalar = recipScalar * (twoP0 - (diag * recipScalar));
      recipScalar = recipScalar * (twoP0 - (diag * recipScalar));
      recipScalar = recipScalar * (twoP0 - (diag * recipScalar));
      recipScalar = recipScalar * (twoP0 - (diag * recipScalar));

      vec      divVec      = (vec) (recipScalar);
#endif
      if (col > 0) {
         DSPLIB_lud_inv_factor_exec_ci<dataType, vec>(&pLocalU[col], colStrideOrder, col, factArray, divVec, pBlock,
                                                      se0ParamsFact, se1ParamsFact, sa0ParamsFact, sa1ParamsFact);
         seScalarParams.ICNT0 = col;
         __SE0_OPEN(factArray, seScalarParams);
      }

      __SA0_OPEN(saRefParams);
      __SA2_OPEN(saRefStoreParams);

      if (nTiles8 > 0) {
         /* 1 X (8 * eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile8;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = col;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles8;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile8;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit8;

         if (col) {
            __SE1_OPEN(pLocalU, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles8; tile++) {

            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastU);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastU);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastU);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastU);
            vec sV4  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastU);
            vec sV5  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastU);
            vec sV6  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastU);
            vec sV7  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastU);
            vec sV8  = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < col; vertical++) {

               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();
               vec v3 = c7x::strm_eng<1, vec>::get_adv();
               vec v4 = c7x::strm_eng<1, vec>::get_adv();
               vec v5 = c7x::strm_eng<1, vec>::get_adv();
               vec v6 = c7x::strm_eng<1, vec>::get_adv();
               vec v7 = c7x::strm_eng<1, vec>::get_adv();
               vec v8 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;
               v3 -= sV3 * scalarDup;
               v4 -= sV4 * scalarDup;
               v5 -= sV5 * scalarDup;
               v6 -= sV6 * scalarDup;
               v7 -= sV7 * scalarDup;
               v8 -= sV8 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalU);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalU);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalU);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalU);
               __vstore_pred(sPred, pStoreVec, v4);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalU);
               __vstore_pred(sPred, pStoreVec, v5);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalU);
               __vstore_pred(sPred, pStoreVec, v6);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalU);
               __vstore_pred(sPred, pStoreVec, v7);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalU);
               __vstore_pred(sPred, pStoreVec, v8);
            }

            sV1 *= divVec;
            sV2 *= divVec;
            sV3 *= divVec;
            sV4 *= divVec;
            sV5 *= divVec;
            sV6 *= divVec;
            sV7 *= divVec;
            sV8 *= divVec;

            lPred    = c7x::strm_agen<2, vec>::get_vpred();
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pLastU);
            __vstore_pred(lPred, psV, sV1);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastU);
            __vstore_pred(lPred, psV, sV2);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastU);
            __vstore_pred(lPred, psV, sV3);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastU);
            __vstore_pred(lPred, psV, sV4);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastU);
            __vstore_pred(lPred, psV, sV5);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastU);
            __vstore_pred(lPred, psV, sV6);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastU);
            __vstore_pred(lPred, psV, sV7);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastU);
            __vstore_pred(lPred, psV, sV8);
         }
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles4 > 0) {
         /* 1 X (4 * eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile4;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = col;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles4;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile4;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit4;

         dataType *pSE1 = pLocalU + colLimit8;
         dataType *pSA1 = pLocalU + colLimit8;
         dataType *pSA0 = pLastU;
         dataType *pSA2 = pLastU;

         if (col) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles4; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV4  = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < col; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();
               vec v3 = c7x::strm_eng<1, vec>::get_adv();
               vec v4 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;
               v3 -= sV3 * scalarDup;
               v4 -= sV4 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v4);
            }

            sV1 *= divVec;
            sV2 *= divVec;
            sV3 *= divVec;
            sV4 *= divVec;

            lPred    = c7x::strm_agen<2, vec>::get_vpred();
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV1);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV2);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV3);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV4);
         }
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles2 > 0) {
         /* 1 X (2*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile2;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = col;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles2;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile2;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit2;

         dataType *pSE1 = pLocalU + colLimit8 + colLimit4;
         dataType *pSA1 = pLocalU + colLimit8 + colLimit4;
         dataType *pSA0 = pLastU;
         dataType *pSA2 = pLastU;

         if (col) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles2; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < col; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v2);
            }

            sV1 *= divVec;
            sV2 *= divVec;

            lPred    = c7x::strm_agen<2, vec>::get_vpred();
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV1);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV2);
         }

         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles1 > 0) {
         /* 1 X (1*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile1;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = col;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles1;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile1;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit1;

         dataType *pSE1 = pLocalU + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA1 = pLocalU + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA0 = pLastU;
         dataType *pSA2 = pLastU;

         if (col) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles1; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < col; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);
            }

            sV1 *= divVec;

            lPred    = c7x::strm_agen<2, vec>::get_vpred();
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV1);
         }

         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      __SA0_CLOSE();
      __SA2_CLOSE();

      __SA0_OPEN(saRefParams);
      __SA2_OPEN(saRefStoreParams);

      if (nTiles8 > 0) {
         /* 1 X (8*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile8;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = col;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles8;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile8;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit8;

         if (col) {
            __SE1_OPEN(pLocalInvU, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles8; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvU);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvU);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvU);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvU);
            vec sV4  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvU);
            vec sV5  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvU);
            vec sV6  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvU);
            vec sV7  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvU);
            vec sV8  = __vload_pred(lPred, pLoadVec);
            for (int32_t vertical = 0; vertical < col; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();
               vec v3 = c7x::strm_eng<1, vec>::get_adv();
               vec v4 = c7x::strm_eng<1, vec>::get_adv();
               vec v5 = c7x::strm_eng<1, vec>::get_adv();
               vec v6 = c7x::strm_eng<1, vec>::get_adv();
               vec v7 = c7x::strm_eng<1, vec>::get_adv();
               vec v8 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;
               v3 -= sV3 * scalarDup;
               v4 -= sV4 * scalarDup;
               v5 -= sV5 * scalarDup;
               v6 -= sV6 * scalarDup;
               v7 -= sV7 * scalarDup;
               v8 -= sV8 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvU);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvU);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvU);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvU);
               __vstore_pred(sPred, pStoreVec, v4);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvU);
               __vstore_pred(sPred, pStoreVec, v5);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvU);
               __vstore_pred(sPred, pStoreVec, v6);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvU);
               __vstore_pred(sPred, pStoreVec, v7);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvU);
               __vstore_pred(sPred, pStoreVec, v8);
            }

            sV1 *= divVec;
            sV2 *= divVec;
            sV3 *= divVec;
            sV4 *= divVec;
            sV5 *= divVec;
            sV6 *= divVec;
            sV7 *= divVec;
            sV8 *= divVec;

            lPred    = c7x::strm_agen<2, vec>::get_vpred();
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pLastInvU);
            __vstore_pred(lPred, psV, sV1);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvU);
            __vstore_pred(lPred, psV, sV2);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvU);
            __vstore_pred(lPred, psV, sV3);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvU);
            __vstore_pred(lPred, psV, sV4);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvU);
            __vstore_pred(lPred, psV, sV5);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvU);
            __vstore_pred(lPred, psV, sV6);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvU);
            __vstore_pred(lPred, psV, sV7);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvU);
            __vstore_pred(lPred, psV, sV8);
         }
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles4 > 0) {
         /* 1 X (4*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile4;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = col;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles4;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile4;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit4;

         dataType *pSE1 = pLocalInvU + colLimit8;
         dataType *pSA1 = pLocalInvU + colLimit8;
         dataType *pSA0 = pLastInvU;
         dataType *pSA2 = pLastInvU;

         if (col) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles4; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV4  = __vload_pred(lPred, pLoadVec);
            for (int32_t vertical = 0; vertical < col; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();
               vec v3 = c7x::strm_eng<1, vec>::get_adv();
               vec v4 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;
               v3 -= sV3 * scalarDup;
               v4 -= sV4 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v4);
            }

            sV1 *= divVec;
            sV2 *= divVec;
            sV3 *= divVec;
            sV4 *= divVec;

            lPred    = c7x::strm_agen<2, vec>::get_vpred();
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV1);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV2);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV3);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV4);
         }
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles2 > 0) {
         /* 1 X (2*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile2;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = col;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles2;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile2;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit2;

         dataType *pSE1 = pLocalInvU + colLimit8 + colLimit4;
         dataType *pSA1 = pLocalInvU + colLimit8 + colLimit4;
         dataType *pSA0 = pLastInvU;
         dataType *pSA2 = pLastInvU;

         if (col) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles2; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < col; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v2);
            }

            sV1 *= divVec;
            sV2 *= divVec;

            lPred    = c7x::strm_agen<2, vec>::get_vpred();
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV1);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV2);
         }

         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles1 > 0) {
         /* 1 X (1*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile1;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = col;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles1;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile1;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit1;

         dataType *pSE1 = pLocalInvU + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA1 = pLocalInvU + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA0 = pLastInvU;
         dataType *pSA2 = pLastInvU;

         if (col) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles1; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < col; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);
            }

            sV1 *= divVec;

            lPred    = c7x::strm_agen<2, vec>::get_vpred();
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pSA2);
            __vstore_pred(lPred, psV, sV1);
         }

         __SE1_CLOSE();
         __SA1_CLOSE();
      }
      __SE0_CLOSE();
      __SA0_CLOSE();
      __SA2_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_inv_invU_exec_ci<float>(float   *pLocalU,
                                                 float   *pLocalInvU,
                                                 int32_t  nCols,
                                                 int32_t  colStrideOrder,
                                                 int32_t  colInvAStride,
                                                 float   *factArray,
                                                 uint8_t *pBlock);
template void DSPLIB_lud_inv_invU_exec_ci<double>(double  *pLocalU,
                                                  double  *pLocalInvU,
                                                  int32_t  nCols,
                                                  int32_t  colStrideOrder,
                                                  int32_t  colInvAStride,
                                                  double  *factArray,
                                                  uint8_t *pBlock);

template <typename dataType>
void DSPLIB_lud_inv_invL_exec_ci(dataType *pLocalL,
                                 dataType *pLocalInvL,
                                 int32_t   nCols,
                                 int32_t   colStrideOrder,
                                 int32_t   colInvLStride,
                                 dataType *factArray,
                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   typedef typename c7x::make_full_vector<dataType>::type vec;

   /* Factor copy params */
   __SE_TEMPLATE_v1 se0ParamsFact = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 se1ParamsFact = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0ParamsFact = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa1ParamsFact = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   /* Updation params */
   __SE_TEMPLATE_v1 seScalarParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 seMatrixParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saMatrixParams   = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saRefParams      = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saRefStoreParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   int32_t lenTile8 = 8;
   int32_t lenTile4 = 4;
   int32_t lenTile2 = 2;
   int32_t lenTile1 = 1;

   int32_t nTiles1 = DSPLIB_ceilingDiv(nCols, (eleCount));
   int32_t nTiles8 = nTiles1 / lenTile8;
   nTiles1 -= nTiles8 * lenTile8;
   int32_t nTiles4 = nTiles1 / lenTile4;
   nTiles1 -= nTiles4 * lenTile4;
   int32_t nTiles2 = nTiles1 / lenTile2;
   nTiles1 -= nTiles2 * lenTile2;

   int32_t remainingCols = nCols;
   int32_t colLimit8     = nTiles8 * lenTile8 * eleCount;
   colLimit8             = (remainingCols < (colLimit8)) ? remainingCols : colLimit8;

   remainingCols     = remainingCols - colLimit8;
   int32_t colLimit4 = nTiles4 * lenTile4 * eleCount;
   colLimit4         = (remainingCols < (colLimit4)) ? remainingCols : colLimit4;

   remainingCols     = remainingCols - colLimit4;
   int32_t colLimit2 = nTiles2 * lenTile2 * eleCount;
   colLimit2         = (remainingCols < (colLimit2)) ? remainingCols : colLimit2;

   int32_t colLimit1    = remainingCols - colLimit2;
   seScalarParams.ICNT1 = 2 * (nTiles8 + nTiles4 + nTiles2 + nTiles1);

   for (int32_t col = 0; col < nCols; col++) {
      dataType *pRefL      = pLocalL + (colStrideOrder * col);
      dataType *pRefInvL   = pLocalInvL + (colInvLStride * col);
      dataType *pStartL    = pLocalL + (colStrideOrder * (col + 1));
      dataType *pStartInvL = pLocalInvL + (colStrideOrder * (col + 1));
      int32_t   nRows      = (nCols - 1) - col;

      dataType diag = pLocalL[col + col * colStrideOrder];
#ifdef LUD_INV_HIGH_PRECISION
      vec divVec = (vec) (1 / diag);
#else
      dataType recipScalar = __recip(diag);
      dataType twoP0       = 2.0;

      recipScalar = recipScalar * (twoP0 - (diag * recipScalar));
      recipScalar = recipScalar * (twoP0 - (diag * recipScalar));

      vec divVec = (vec) recipScalar;
#endif
      if (nRows > 0) {
         DSPLIB_lud_inv_factor_exec_ci<dataType, vec>(&pStartL[col], colStrideOrder, nRows, factArray, divVec, pBlock,
                                                      se0ParamsFact, se1ParamsFact, sa0ParamsFact, sa1ParamsFact);
         seScalarParams.ICNT0 = nRows;
         __SE0_OPEN(factArray, seScalarParams);
      }

      __SA0_OPEN(saRefParams);
      __SA2_OPEN(saRefStoreParams);

      if (nTiles8 > 0) {
         /* 1 X (8*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile8;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles8;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile8;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit8;

         if (nRows) {
            __SE1_OPEN(pStartL, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles8; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
            vec sV4  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
            vec sV5  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
            vec sV6  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
            vec sV7  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
            vec sV8  = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < nRows; vertical++) {

               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();
               vec v3 = c7x::strm_eng<1, vec>::get_adv();
               vec v4 = c7x::strm_eng<1, vec>::get_adv();
               vec v5 = c7x::strm_eng<1, vec>::get_adv();
               vec v6 = c7x::strm_eng<1, vec>::get_adv();
               vec v7 = c7x::strm_eng<1, vec>::get_adv();
               vec v8 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;
               v3 -= sV3 * scalarDup;
               v4 -= sV4 * scalarDup;
               v5 -= sV5 * scalarDup;
               v6 -= sV6 * scalarDup;
               v7 -= sV7 * scalarDup;
               v8 -= sV8 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
               __vstore_pred(sPred, pStoreVec, v4);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
               __vstore_pred(sPred, pStoreVec, v5);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
               __vstore_pred(sPred, pStoreVec, v6);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
               __vstore_pred(sPred, pStoreVec, v7);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
               __vstore_pred(sPred, pStoreVec, v8);
            }
         }
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles4 > 0) {
         /* 1 X (4*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile4;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles4;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile4;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit4;

         dataType *pSE1 = pStartL + colLimit8;
         dataType *pSA1 = pStartL + colLimit8;
         dataType *pSA0 = pRefL;

         if (nRows) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles4; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV4  = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < nRows; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();
               vec v3 = c7x::strm_eng<1, vec>::get_adv();
               vec v4 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;
               v3 -= sV3 * scalarDup;
               v4 -= sV4 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v4);
            }
         }
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles2 > 0) {
         /* 1 X (2*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile2;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles2;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile2;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit2;

         dataType *pSE1 = pStartL + colLimit8 + colLimit4;
         dataType *pSA1 = pStartL + colLimit8 + colLimit4;
         dataType *pSA0 = pRefL;

         if (nRows) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles2; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < nRows; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v2);
            }
         }

         __SE1_CLOSE();
         __SA1_CLOSE();
      }
      if (nTiles1 > 0) {
         /* 1 X (1*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile1;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles1;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile1;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit1;

         dataType *pSE1 = pStartL + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA1 = pStartL + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA0 = pRefL;

         if (nRows) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles1; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < nRows; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);
            }
         }

         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      __SA0_CLOSE();
      __SA2_CLOSE();

      __SA0_OPEN(saRefParams);
      __SA2_OPEN(saRefStoreParams);

      if (nTiles8 > 0) {
         /* 1 X (8*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile8;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles8;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile8;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit8;

         if (nRows) {
            __SE1_OPEN(pStartInvL, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles8; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefInvL);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefInvL);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefInvL);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefInvL);
            vec sV4  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefInvL);
            vec sV5  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefInvL);
            vec sV6  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefInvL);
            vec sV7  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefInvL);
            vec sV8  = __vload_pred(lPred, pLoadVec);
            for (int32_t vertical = 0; vertical < nRows; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();
               vec v3 = c7x::strm_eng<1, vec>::get_adv();
               vec v4 = c7x::strm_eng<1, vec>::get_adv();
               vec v5 = c7x::strm_eng<1, vec>::get_adv();
               vec v6 = c7x::strm_eng<1, vec>::get_adv();
               vec v7 = c7x::strm_eng<1, vec>::get_adv();
               vec v8 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;
               v3 -= sV3 * scalarDup;
               v4 -= sV4 * scalarDup;
               v5 -= sV5 * scalarDup;
               v6 -= sV6 * scalarDup;
               v7 -= sV7 * scalarDup;
               v8 -= sV8 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartInvL);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartInvL);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartInvL);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartInvL);
               __vstore_pred(sPred, pStoreVec, v4);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartInvL);
               __vstore_pred(sPred, pStoreVec, v5);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartInvL);
               __vstore_pred(sPred, pStoreVec, v6);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartInvL);
               __vstore_pred(sPred, pStoreVec, v7);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartInvL);
               __vstore_pred(sPred, pStoreVec, v8);
            }
         }
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles4 > 0) {

         /* 1 X (4*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile4;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles4;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile4;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit4;

         dataType *pSE1 = pStartInvL + colLimit8;
         dataType *pSA1 = pStartInvL + colLimit8;
         dataType *pSA0 = pRefInvL;

         if (nRows) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles4; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV4  = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < nRows; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();
               vec v3 = c7x::strm_eng<1, vec>::get_adv();
               vec v4 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;
               v3 -= sV3 * scalarDup;
               v4 -= sV4 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v4);
            }
         }
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles2 > 0) {
         /* 1 X (2*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile2;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles2;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile2;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit2;

         dataType *pSE1 = pStartInvL + colLimit8 + colLimit4;
         dataType *pSA1 = pStartInvL + colLimit8 + colLimit4;
         dataType *pSA0 = pRefInvL;

         if (nRows) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles2; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < nRows; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();
               vec v2 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;
               v2 -= sV2 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v2);
            }
         }

         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      if (nTiles1 > 0) {
         /* 1 X (1*eleCount) TILE */

         seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile1;
         seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
         seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles1;
         seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile1;
         seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit1;

         dataType *pSE1 = pStartInvL + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA1 = pStartInvL + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA0 = pRefInvL;

         if (nRows) {
            __SE1_OPEN(pSE1, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles1; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            for (int32_t vertical = 0; vertical < nRows; vertical++) {
               vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

               vec v1 = c7x::strm_eng<1, vec>::get_adv();

               v1 -= sV1 * scalarDup;

               __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
               __vstore_pred(sPred, pStoreVec, v1);
            }
         }

         __SE1_CLOSE();
         __SA1_CLOSE();
      }
      __SE0_CLOSE();
      __SA0_CLOSE();
      __SA2_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_inv_invL_exec_ci<float>(float   *pLocalL,
                                                 float   *pLocalInvL,
                                                 int32_t  nCols,
                                                 int32_t  colStrideOrder,
                                                 int32_t  colInvLStride,
                                                 float   *factArray,
                                                 uint8_t *pBlock);
template void DSPLIB_lud_inv_invL_exec_ci<double>(double  *pLocalL,
                                                  double  *pLocalInvL,
                                                  int32_t  nCols,
                                                  int32_t  colStrideOrder,
                                                  int32_t  colInvLStride,
                                                  double  *factArray,
                                                  uint8_t *pBlock);

template <typename dataType>
DSPLIB_STATUS DSPLIB_lud_inv_exec_ci(DSPLIB_kernelHandle handle,
                                     void *restrict pP,
                                     void *restrict pL,
                                     void *restrict pU,
                                     void *restrict pinvA,
                                     void *restrict pStratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_lud_inv_PrivArgs *pKerPrivArgs = (DSPLIB_lud_inv_PrivArgs *) handle;

   int32_t order       = pKerPrivArgs->order;
   int32_t strideOrder = pKerPrivArgs->strideOrder;
   int32_t strideP     = pKerPrivArgs->strideP;
   int32_t dataSize    = sizeof(dataType);
   int32_t dataSizeP   = sizeof(uint16_t);

   int32_t orderStride  = strideOrder / dataSize;
   int32_t orderPStride = strideP / dataSizeP;

   /* Typecast void pointers to respective data type */
   unsigned short         *pPLocal            = (unsigned short *) pP;
   dataType               *pLLocal            = (dataType *) pL;
   dataType               *pULocal            = (dataType *) pU;
   dataType               *pinvALocal         = (dataType *) pinvA;
   dataType               *pFactArray         = (dataType *) pStratch;
   uint8_t                *pBlock             = pKerPrivArgs->bufPblock;
   DSPLIB_matMul_PrivArgs *pMatMulKerPrivArgs = &pKerPrivArgs->pMatMulKerPrivArgs;

   DSPLIB_DEBUGPRINTFN(0, "pPLocal: %p pLLocal: %p pULocal: %p pinvALocal: %p order: %d\n", pPLocal, pLLocal, pULocal,
                       pinvALocal, order);

   dataType *invL, *invU, *invU_x_invL;
   /* set inv_A matrix to identity */
   invL = &pinvALocal[0];
   DSPLIB_lud_identity_matrix_generate_exec_ci<dataType>(invL, order, orderStride, pBlock);
   DSPLIB_lud_inv_invL_exec_ci<dataType>(pLLocal, invL, order, orderStride, orderStride, pFactArray, pBlock);

   /* set invU matrix to identity */
   invU = &pLLocal[0];
   DSPLIB_lud_identity_matrix_generate_exec_ci<dataType>(invU, order, orderStride, pBlock);

   /* use Gauss Jordan algorithm to invert U whose result is in L */
   DSPLIB_lud_inv_invU_exec_ci(pULocal, invU, order, orderStride, orderStride, pFactArray, pBlock);

   /* -----------------------------------------------------------------------------------
    To compute "inv_A=inv(U)*inv(L)*P".
    - Multiply invU and invL
    - As P is permutation matrix we use indices of "1" present in each row of Permutation Matrix (pPLocaL)to shuffle the
    columns of invU_x_invL
    --------------------------------------------------------------------------------------*/
   invU_x_invL = &pULocal[0];
   /* Multiply invU * invL */
   DSPLIB_matMul_exec_ci<dataType>(pMatMulKerPrivArgs, invU, invL, invU_x_invL);

   uint32_t *permuteOrder = (uint32_t *) pFactArray;
   /* Get the indices of "1" present in each row of "pPLocal" and store in "permuteOrder" */
   DSPLIB_lud_inv_permuteIndex_ci(pPLocal, order, orderPStride, permuteOrder, pBlock);

   /* To shuffle the columns first transpose the matrix invU_x_invL*/
   DSPLIB_matTrans_exec_ci<dataType>(&pKerPrivArgs->pMatTransKerPrivArgs, invU_x_invL, pinvALocal);

   /* Based on indices present in "permuteOrder" shuffle the rows of transposed invU_x_invL */
   DSPLIB_lud_inv_permuteRows_ci<dataType>(pinvALocal, order, orderStride, permuteOrder, invU_x_invL, pBlock);

   /* Transpose back invU_x_invL to get the final result i.e. shuffled columns */
   DSPLIB_matTrans_exec_ci<dataType>(&pKerPrivArgs->pMatTransKerPrivArgs, invU_x_invL, pinvALocal);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_lud_inv_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                     void *restrict pP,
                                                     void *restrict pL,
                                                     void *restrict pU,
                                                     void *restrict pinvA,
                                                     void *restrict pStratch);

template DSPLIB_STATUS DSPLIB_lud_inv_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                      void *restrict pP,
                                                      void *restrict pL,
                                                      void *restrict pU,
                                                      void *restrict pinvA,
                                                      void *restrict pStratch);
