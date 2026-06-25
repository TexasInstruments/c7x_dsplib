// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_priv.h"

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */

template <typename dataType>
void DSPLIB_svd_matTrans_init_ci(DSPLIB_kernelHandle handle, const DSPLIB_svdInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_svd_PrivArgs *pKerPrivArgs = (DSPLIB_svd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   __SA_TEMPLATE_v1     sa1Params    = __gen_SA_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;
   sa1Params.DIMFMT                                                  = __SA_DIMFMT_2D;
   sa1Params.VECLEN                                                  = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (25 * SE_PARAM_SIZE)) = sa1Params;

   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs1 = &pKerPrivArgs->pMatTransKerPrivArgs1;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs2 = &pKerPrivArgs->pMatTransKerPrivArgs2;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs3 = &pKerPrivArgs->pMatTransKerPrivArgs3;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs4 = &pKerPrivArgs->pMatTransKerPrivArgs4;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs5 = &pKerPrivArgs->pMatTransKerPrivArgs5;

   DSPLIB_matTransInitArgs kerInitArgsMatTrans;
   kerInitArgsMatTrans.funcStyle = pKerInitArgs->funcStyle;
   kerInitArgsMatTrans.dimX      = pKerInitArgs->dimX;
   kerInitArgsMatTrans.dimY      = pKerInitArgs->dimY;

   uint32_t Nrows  = pKerPrivArgs->heightIn;
   uint32_t Ncols  = pKerPrivArgs->widthIn;
   int      Nrows1 = 0;
   int      Ncols1 = 0;
   if (Nrows >= Ncols) {
      Nrows1 = Nrows;
      Ncols1 = Ncols;
   }
   else {
      Nrows1 = Ncols;
      Ncols1 = Nrows;
   }

   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;

   /* First matTrans Init */
   bufParamsIn.dim_y = bufParamsOut.dim_x = Ncols1;
   bufParamsIn.dim_x = bufParamsOut.dim_y = Nrows1;
   bufParamsIn.stride_y                   = pKerPrivArgs->strideIn;
   bufParamsOut.stride_y                  = pKerPrivArgs->strideU;
   pMatTransKerPrivArgs1->heightIn        = Ncols1;
   pMatTransKerPrivArgs1->widthIn         = Nrows1;
   pMatTransKerPrivArgs1->strideIn        = pKerPrivArgs->strideIn;
   pMatTransKerPrivArgs1->strideOut       = pKerPrivArgs->strideU;
   DSPLIB_matTrans_init_ci<dataType>(pMatTransKerPrivArgs1, &bufParamsIn, &bufParamsOut, &kerInitArgsMatTrans);

   /* Second matTrans Init */
   bufParamsIn.dim_y = bufParamsOut.dim_x = Nrows1;
   bufParamsIn.dim_x = bufParamsOut.dim_y = Nrows1;
   bufParamsIn.stride_y                   = pKerPrivArgs->strideURows;
   bufParamsOut.stride_y                  = pKerPrivArgs->strideURows;
   pMatTransKerPrivArgs2->heightIn        = Nrows1;
   pMatTransKerPrivArgs2->widthIn         = Nrows1;
   pMatTransKerPrivArgs2->strideIn        = pKerPrivArgs->strideURows;
   pMatTransKerPrivArgs2->strideOut       = pKerPrivArgs->strideURows;
   DSPLIB_matTrans_init_ci<dataType>(pMatTransKerPrivArgs2, &bufParamsIn, &bufParamsOut, &kerInitArgsMatTrans);

   /* Third matTrans Init */
   bufParamsIn.dim_y = bufParamsOut.dim_x = Nrows1;
   bufParamsIn.dim_x = bufParamsOut.dim_y = Ncols1;
   bufParamsIn.stride_y                   = pKerPrivArgs->strideU;
   bufParamsOut.stride_y                  = pKerPrivArgs->strideURows;
   pMatTransKerPrivArgs3->heightIn        = Nrows1;
   pMatTransKerPrivArgs3->widthIn         = Ncols1;
   pMatTransKerPrivArgs3->strideIn        = pKerPrivArgs->strideU;
   pMatTransKerPrivArgs3->strideOut       = pKerPrivArgs->strideURows;
   DSPLIB_matTrans_init_ci<dataType>(pMatTransKerPrivArgs3, &bufParamsIn, &bufParamsOut, &kerInitArgsMatTrans);

   /* Fourth matTrans Init */
   bufParamsIn.dim_y = bufParamsOut.dim_x = Ncols1;
   bufParamsIn.dim_x = bufParamsOut.dim_y = Nrows1;
   bufParamsIn.stride_y                   = pKerPrivArgs->strideURows;
   bufParamsOut.stride_y                  = pKerPrivArgs->strideU;
   pMatTransKerPrivArgs4->heightIn        = Ncols1;
   pMatTransKerPrivArgs4->widthIn         = Nrows1;
   pMatTransKerPrivArgs4->strideIn        = pKerPrivArgs->strideURows;
   pMatTransKerPrivArgs4->strideOut       = pKerPrivArgs->strideU;
   DSPLIB_matTrans_init_ci<dataType>(pMatTransKerPrivArgs4, &bufParamsIn, &bufParamsOut, &kerInitArgsMatTrans);

   /* Fifth matTrans Init */
   bufParamsIn.dim_y = bufParamsOut.dim_x = Ncols1;
   bufParamsIn.dim_x = bufParamsOut.dim_y = Ncols1;
   bufParamsIn.stride_y                   = pKerPrivArgs->strideVRows;
   bufParamsOut.stride_y                  = pKerPrivArgs->strideV;
   pMatTransKerPrivArgs5->heightIn        = Ncols1;
   pMatTransKerPrivArgs5->widthIn         = Ncols1;
   pMatTransKerPrivArgs5->strideIn        = pKerPrivArgs->strideVRows;
   pMatTransKerPrivArgs5->strideOut       = pKerPrivArgs->strideV;
   DSPLIB_matTrans_init_ci<dataType>(pMatTransKerPrivArgs5, &bufParamsIn, &bufParamsOut, &kerInitArgsMatTrans);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_svd_matTrans_init_ci<float>(DSPLIB_kernelHandle handle, const DSPLIB_svdInitArgs *pKerInitArgs);
template void DSPLIB_svd_matTrans_init_ci<double>(DSPLIB_kernelHandle handle, const DSPLIB_svdInitArgs *pKerInitArgs);

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */

/*!
 *  @brief       Copy input matrix pIn to pOut.
 */
template <typename dataType>
void DSPLIB_svd_blk_move_ci(dataType *pOut,
                            dataType *pIn,
                            int32_t   Nrows,
                            int32_t   Ncols,
                            int32_t   colOutStride,
                            int32_t   colInStride,
                            uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (22 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (25 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount  = c7x::element_count_of<vec>::value;
   int32_t                                                nVec      = DSPLIB_ceilingDiv(Ncols, eleCount);
   int32_t                                                loopCount = nVec * Nrows;

   se0Params.ICNT0 = Ncols;
   se0Params.ICNT1 = Nrows;
   se0Params.DIM1  = colInStride;

   sa0Params.ICNT0 = Ncols;
   sa0Params.ICNT1 = Nrows;
   sa0Params.DIM1  = colOutStride;

   __SE0_OPEN(pIn, se0Params);
   __SA0_OPEN(sa0Params);
   for (int32_t i = 0; i < loopCount; i++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();

      __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStore = c7x::strm_agen<0, vec>::get_adv(pOut);
      __vstore_pred(pred, pStore, v1);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_svd_blk_move_ci<float>(float   *pOut,
                                            float   *pIn,
                                            int32_t  Nrows,
                                            int32_t  Ncols,
                                            int32_t  colOutStride,
                                            int32_t  colInStride,
                                            uint8_t *pBlock);
template void DSPLIB_svd_blk_move_ci<double>(double  *pOut,
                                             double  *pIn,
                                             int32_t  Nrows,
                                             int32_t  Ncols,
                                             int32_t  colOutStride,
                                             int32_t  colInStride,
                                             uint8_t *pBlock);
