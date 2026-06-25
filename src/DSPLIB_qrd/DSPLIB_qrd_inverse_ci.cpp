// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_common.h"
#include "DSPLIB_qrd_inverse_priv.h"

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/
template <typename dataType> void DSPLIB_qrd_inverse_factor_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_qrd_inverse_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_inverse_PrivArgs *) handle;
   uint8_t                     *pBlock       = pKerPrivArgs->bufPblock;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   int32_t strideR    = pKerPrivArgs->strideR;
   int32_t nCols      = pKerPrivArgs->widthR;
   int32_t colStrideR = strideR / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint32_t     eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   se0Params.ICNT0   = 1;
   se0Params.ICNT1   = eleCount;
   se0Params.DIM1    = colStrideR;
   se0Params.DIM2    = colStrideR * eleCount;
   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   if (sizeof(dataType) == 4) {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   sa0Params.ICNT0  = nCols;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE))     = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_inverse_factor_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_qrd_inverse_factor_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_qrd_inverse_R_invA_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_qrd_inverse_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_inverse_PrivArgs *) handle;
   uint8_t                     *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                      nCols        = pKerPrivArgs->widthR;
   int32_t                      strideR      = pKerPrivArgs->strideR;
   int32_t                      colStrideR   = strideR / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t  lenTile8 = 8;
   uint32_t eleCount = c7x::element_count_of<vec>::value;
   int32_t  nTiles_8 = DSPLIB_ceilingDiv(nCols, (eleCount * lenTile8));

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELEDUP  SE_ELEDUP  = c7x::se_eledup<dataType>::value;

   __SE_TEMPLATE_v1 seScalarParams = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 seMatrixParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saMatrixParams = __gen_SA_TEMPLATE_v1();

   seScalarParams.ICNT1   = nTiles_8;
   seScalarParams.DIM1    = 0;
   seScalarParams.ELEDUP  = SE_ELEDUP;
   seScalarParams.DIMFMT  = __SE_DIMFMT_2D;
   seScalarParams.VECLEN  = SE_VECLEN;
   seScalarParams.ELETYPE = SE_ELETYPE;

   seMatrixParams.ICNT0         = (eleCount * lenTile8);
   seMatrixParams.DIM1          = colStrideR;
   seMatrixParams.ICNT2         = nTiles_8;
   seMatrixParams.DIM2          = (eleCount * lenTile8);
   seMatrixParams.DIMFMT        = __SE_DIMFMT_3D;
   seMatrixParams.ELETYPE       = SE_ELETYPE;
   seMatrixParams.VECLEN        = SE_VECLEN;
   seMatrixParams.DECDIM1       = __SE_DECDIM_DIM2;
   seMatrixParams.DECDIM1_WIDTH = nCols;

   saMatrixParams.ICNT0         = (eleCount * lenTile8);
   saMatrixParams.DIM1          = colStrideR;
   saMatrixParams.ICNT2         = nTiles_8;
   saMatrixParams.DIM2          = (eleCount * lenTile8);
   saMatrixParams.DIMFMT        = __SA_DIMFMT_3D;
   saMatrixParams.VECLEN        = SA_VECLEN;
   saMatrixParams.DECDIM1       = __SA_DECDIM_DIM2;
   saMatrixParams.DECDIM1_WIDTH = nCols;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = seScalarParams;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE)) = seMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE)) = saMatrixParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_inverse_R_invA_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_qrd_inverse_R_invA_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_inverse_init_ci(DSPLIB_kernelHandle          handle,
                                         DSPLIB_bufParams2D_t        *bufParamsQ,
                                         DSPLIB_bufParams2D_t        *bufParamsR,
                                         DSPLIB_bufParams2D_t        *bufParamsInvA,
                                         DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                         const DSPLIB_qrdInvInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS                status               = DSPLIB_SUCCESS;
   DSPLIB_qrd_inverse_PrivArgs *pKerPrivArgs         = (DSPLIB_qrd_inverse_PrivArgs *) handle;
   DSPLIB_matMul_PrivArgs      *pMatMulKerPrivArgs   = &pKerPrivArgs->pMatMulKerPrivArgs;
   DSPLIB_matTrans_PrivArgs    *pMatTransKerPrivArgs = &pKerPrivArgs->pMatTransKerPrivArgs;

   DSPLIB_matMul_InitArgs  kerInitArgsMatMul;
   DSPLIB_matTransInitArgs kerInitArgsMatTrans;

   kerInitArgsMatMul.funcStyle   = pKerInitArgs->funcStyle;
   kerInitArgsMatTrans.funcStyle = pKerInitArgs->funcStyle;
   kerInitArgsMatTrans.dimX      = bufParamsQ->dim_x;
   kerInitArgsMatTrans.dimY      = bufParamsQ->dim_y;

   uint32_t strideIn0Elements = pKerPrivArgs->strideInvA / sizeof(dataType);
   uint32_t strideIn1Elements = pKerPrivArgs->strideR / sizeof(dataType);
   uint32_t strideOutElements = pKerPrivArgs->strideInvA / sizeof(dataType);

   pMatTransKerPrivArgs->widthIn   = bufParamsQ->dim_x;
   pMatTransKerPrivArgs->heightIn  = bufParamsQ->dim_y;
   pMatTransKerPrivArgs->strideIn  = bufParamsQ->stride_y;
   pMatTransKerPrivArgs->strideOut = bufParamsR->stride_y;

   pMatMulKerPrivArgs->M = pKerPrivArgs->heightR;
   pMatMulKerPrivArgs->N = pKerPrivArgs->widthR;
   pMatMulKerPrivArgs->K = pKerPrivArgs->heightR;

   pMatMulKerPrivArgs->strideIn0Elements = strideIn0Elements;
   pMatMulKerPrivArgs->strideIn1Elements = strideIn1Elements;
   pMatMulKerPrivArgs->strideOutElements = strideOutElements;

   DSPLIB_qrd_identity_matrix_generate_init_ci<dataType>(pKerPrivArgs->heightR, pKerPrivArgs->strideR,
                                                         pKerPrivArgs->bufPblock);
   DSPLIB_qrd_inverse_R_invA_init_ci<dataType>(handle);

   DSPLIB_matTrans_init_ci<dataType>(pMatTransKerPrivArgs, bufParamsQ, bufParamsR, &kerInitArgsMatTrans);

   DSPLIB_matMul_init_ci<dataType>(pMatMulKerPrivArgs, bufParamsInvA, bufParamsR, bufParamsInvA, &kerInitArgsMatMul);
   DSPLIB_qrd_inverse_factor_init_ci<dataType>(handle);
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}
template DSPLIB_STATUS DSPLIB_qrd_inverse_init_ci<float>(DSPLIB_kernelHandle          handle,
                                                         DSPLIB_bufParams2D_t        *bufParamsQ,
                                                         DSPLIB_bufParams2D_t        *bufParamsR,
                                                         DSPLIB_bufParams2D_t        *bufParamsInvA,
                                                         DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                                         const DSPLIB_qrdInvInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_qrd_inverse_init_ci<double>(DSPLIB_kernelHandle          handle,
                                                          DSPLIB_bufParams2D_t        *bufParamsQ,
                                                          DSPLIB_bufParams2D_t        *bufParamsR,
                                                          DSPLIB_bufParams2D_t        *bufParamsInvA,
                                                          DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                                          const DSPLIB_qrdInvInitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/

template <typename dataType, typename vec = typename c7x::make_full_vector<dataType>::type>
static inline dataType DSPLIB_qrd_inverse_factor_exec_ci(dataType        *pR,
                                                         int32_t          colStrideR,
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
   dataType sum = 0;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   int32_t nVec     = DSPLIB_ceilingDiv(nRows, eleCount);
   int32_t se0ICNT2 = nVec / 2;
   int32_t se1ICNT2 = nVec - se0ICNT2;

   se0Params.ICNT2 = se0ICNT2;

   se1Params.ICNT2 = se1ICNT2;

   dataType *pSE0 = pR;
   dataType *pSE1 = pR + (se0ICNT2 * colStrideR * eleCount);

   __SE1_OPEN(pSE1, se1Params);
   if (se0ICNT2 > 0) {
      __SE0_OPEN(pSE0, se0Params);
   }

   int32_t vertical;

   sa0Params.ICNT0 = (se0ICNT2 * eleCount);

   sa1Params.ICNT0 = nRows - ((se0ICNT2 * eleCount));

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

      __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pFactor);
      __vstore_pred(pred, pStoreVec, scaleVec * v1);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pFactorHalf);
      __vstore_pred(pred, pStoreVec, scaleVec * v2);

      pred      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pFactor);
      __vstore_pred(pred, pStoreVec, scaleVec * v3);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pFactorHalf);
      __vstore_pred(pred, pStoreVec, scaleVec * v4);
   }

   for (; vertical < se0ICNT2; vertical++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();

      __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pFactor);
      __vstore_pred(pred, pStoreVec, scaleVec * v1);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pFactorHalf);
      __vstore_pred(pred, pStoreVec, scaleVec * v2);
   }
   if (se0ICNT2 != se1ICNT2) {
      vec v1 = c7x::strm_eng<1, vec>::get_adv();

      __vpred pred      = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pFactorHalf);
      __vstore_pred(pred, pStoreVec, scaleVec * v1);
   }

   if (sa0Params.ICNT0) {
      __SA0_CLOSE();
   }

   __SA1_CLOSE();

   __SE1_CLOSE();

   if (se0ICNT2 > 0) {
      __SE0_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return sum;
}
template float DSPLIB_qrd_inverse_factor_exec_ci<float, typename c7x::make_full_vector<float>::type>(
    float                                      *pR,
    int32_t                                     colStrideR,
    int32_t                                     nRows,
    float                                      *pFactor,
    typename c7x::make_full_vector<float>::type scale,
    uint8_t                                    *pBlock,
    __SE_TEMPLATE_v1                            se0Params,
    __SE_TEMPLATE_v1                            se1Params,
    __SA_TEMPLATE_v1                            sa0Params,
    __SA_TEMPLATE_v1                            sa1Params);
template double DSPLIB_qrd_inverse_factor_exec_ci<double, typename c7x::make_full_vector<double>::type>(
    double                                      *pR,
    int32_t                                      colStrideR,
    int32_t                                      nRows,
    double                                      *pFactor,
    typename c7x::make_full_vector<double>::type scale,
    uint8_t                                     *pBlock,
    __SE_TEMPLATE_v1                             se0Params,
    __SE_TEMPLATE_v1                             se1Params,
    __SA_TEMPLATE_v1                             sa0Params,
    __SA_TEMPLATE_v1                             sa1Params);

template <typename dataType>
void DSPLIB_qrd_inverse_R_invA_exec_ci(dataType *pLocalR,
                                       dataType *pLocalInvA,
                                       int32_t   nCols,
                                       int32_t   colStrideR,
                                       int32_t   colInvAStride,
                                       dataType *factArray,
                                       uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0ParamsFact = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 se1ParamsFact = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0ParamsFact = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa1ParamsFact = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 seScalarParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 seMatrixParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saMatrixParams   = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saRefParams      = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saRefStoreParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   int32_t lenTile8 = 8; // hard code
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
      dataType *pLastR    = pLocalR + (colStrideR * col);
      dataType *pLastInvA = pLocalInvA + (colStrideR * col);

      dataType diag = pLocalR[col + col * colStrideR];

      dataType recipScalar = __recip(diag);
      dataType twoP0       = 2.0;
      recipScalar          = recipScalar * (twoP0 - (diag * recipScalar));
      recipScalar          = recipScalar * (twoP0 - (diag * recipScalar));

      vec divVec = (vec) recipScalar;

      if (col > 0) {
         DSPLIB_qrd_inverse_factor_exec_ci<dataType, vec>(&pLocalR[col], colStrideR, col, factArray, divVec, pBlock,
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
            __SE1_OPEN(pLocalR, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles8; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastR);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastR);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastR);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastR);
            vec sV4  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastR);
            vec sV5  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastR);
            vec sV6  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastR);
            vec sV7  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastR);
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
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalR);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalR);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalR);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalR);
               __vstore_pred(sPred, pStoreVec, v4);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalR);
               __vstore_pred(sPred, pStoreVec, v5);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalR);
               __vstore_pred(sPred, pStoreVec, v6);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalR);
               __vstore_pred(sPred, pStoreVec, v7);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalR);
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
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pLastR);
            __vstore_pred(lPred, psV, sV1);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastR);
            __vstore_pred(lPred, psV, sV2);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastR);
            __vstore_pred(lPred, psV, sV3);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastR);
            __vstore_pred(lPred, psV, sV4);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastR);
            __vstore_pred(lPred, psV, sV5);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastR);
            __vstore_pred(lPred, psV, sV6);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastR);
            __vstore_pred(lPred, psV, sV7);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastR);
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

         dataType *pSE1 = pLocalR + colLimit8;
         dataType *pSA1 = pLocalR + colLimit8;
         dataType *pSA0 = pLastR; // + colLimit8;
         dataType *pSA2 = pLastR; // + colLimit8;

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

         dataType *pSE1 = pLocalR + colLimit8 + colLimit4;
         dataType *pSA1 = pLocalR + colLimit8 + colLimit4;
         dataType *pSA0 = pLastR; // + colLimit8 + colLimit4;
         dataType *pSA2 = pLastR; // + colLimit8 + colLimit4;

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

         dataType *pSE1 = pLocalR + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA1 = pLocalR + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA0 = pLastR; // + colLimit8 + colLimit4;
         dataType *pSA2 = pLastR; // + colLimit8 + colLimit4;

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
            __SE1_OPEN(pLocalInvA, seMatrixParams);
            __SA1_OPEN(saMatrixParams);
         }

         for (int32_t tile = 0; tile < nTiles8; tile++) {
            __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvA);
            vec     sV1      = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvA);
            vec sV2  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvA);
            vec sV3  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvA);
            vec sV4  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvA);
            vec sV5  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvA);
            vec sV6  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvA);
            vec sV7  = __vload_pred(lPred, pLoadVec);

            lPred    = c7x::strm_agen<0, vec>::get_vpred();
            pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLastInvA);
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
               vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvA);
               __vstore_pred(sPred, pStoreVec, v1);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvA);
               __vstore_pred(sPred, pStoreVec, v2);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvA);
               __vstore_pred(sPred, pStoreVec, v3);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvA);
               __vstore_pred(sPred, pStoreVec, v4);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvA);
               __vstore_pred(sPred, pStoreVec, v5);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvA);
               __vstore_pred(sPred, pStoreVec, v6);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvA);
               __vstore_pred(sPred, pStoreVec, v7);

               sPred     = c7x::strm_agen<1, vec>::get_vpred();
               pStoreVec = c7x::strm_agen<1, vec>::get_adv(pLocalInvA);
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
            vec *psV = c7x::strm_agen<2, vec>::get_adv(pLastInvA);
            __vstore_pred(lPred, psV, sV1);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvA);
            __vstore_pred(lPred, psV, sV2);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvA);
            __vstore_pred(lPred, psV, sV3);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvA);
            __vstore_pred(lPred, psV, sV4);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvA);
            __vstore_pred(lPred, psV, sV5);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvA);
            __vstore_pred(lPred, psV, sV6);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvA);
            __vstore_pred(lPred, psV, sV7);

            lPred = c7x::strm_agen<2, vec>::get_vpred();
            psV   = c7x::strm_agen<2, vec>::get_adv(pLastInvA);
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

         dataType *pSE1 = pLocalInvA + colLimit8;
         dataType *pSA1 = pLocalInvA + colLimit8;
         dataType *pSA0 = pLastInvA; // + colLimit8;
         dataType *pSA2 = pLastInvA; // + colLimit8;

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

         dataType *pSE1 = pLocalInvA + colLimit8 + colLimit4;
         dataType *pSA1 = pLocalInvA + colLimit8 + colLimit4;
         dataType *pSA0 = pLastInvA; // + colLimit8;
         dataType *pSA2 = pLastInvA; // + colLimit8;

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

         dataType *pSE1 = pLocalInvA + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA1 = pLocalInvA + colLimit8 + colLimit4 + colLimit2;
         dataType *pSA0 = pLastInvA; // + colLimit8;
         dataType *pSA2 = pLastInvA; // + colLimit8;

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
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_inverse_R_invA_exec_ci<float>(float   *pLocalR,
                                                       float   *pLocalInvA,
                                                       int32_t  nCols,
                                                       int32_t  colStrideR,
                                                       int32_t  colInvAStride,
                                                       float   *factArray,
                                                       uint8_t *pBlock);
template void DSPLIB_qrd_inverse_R_invA_exec_ci<double>(double  *pLocalR,
                                                        double  *pLocalInvA,
                                                        int32_t  nCols,
                                                        int32_t  colStrideR,
                                                        int32_t  colInvAStride,
                                                        double  *factArray,
                                                        uint8_t *pBlock);

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_inverse_exec_ci(DSPLIB_kernelHandle handle,
                                         void *restrict pQ,
                                         void *restrict pR,
                                         void *restrict pInvA,
                                         void *restrict pInvAScratch,
                                         void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_qrd_inverse_PrivArgs *pKerPrivArgs         = (DSPLIB_qrd_inverse_PrivArgs *) handle;
   int32_t                      strideR              = pKerPrivArgs->strideR;
   int32_t                      heightR              = pKerPrivArgs->heightR;
   int32_t                      widthR               = pKerPrivArgs->widthR;
   int32_t                      strideInvA           = pKerPrivArgs->strideInvA;
   int32_t                      dataSize             = sizeof(dataType);
   uint8_t                     *pBlock               = pKerPrivArgs->bufPblock;
   DSPLIB_matMul_PrivArgs      *pMatMulKerPrivArgs   = &pKerPrivArgs->pMatMulKerPrivArgs;
   DSPLIB_matTrans_PrivArgs    *pMatTransKerPrivArgs = &pKerPrivArgs->pMatTransKerPrivArgs;

   /* Typecast void pointers to respective data type */
   dataType *pLocalQ           = (dataType *) pQ;
   dataType *pLocalR           = (dataType *) pR;
   dataType *pLocalInvA        = (dataType *) pInvA;
   dataType *pLocalInvAScratch = (dataType *) pInvAScratch;
   dataType *pFactArray        = (dataType *) pScratch;

   int32_t colStrideR    = strideR / dataSize;
   int32_t colInvAStride = strideInvA / dataSize;

   DSPLIB_DEBUGPRINTFN(0, "pLocalQ: %p pLocalR: %p pLocalInvA: %p  widthR: %d heightR: %d\n", pLocalQ, pLocalR,
                       pLocalInvA, widthR, heightR);

   /* ------------------------------------------------------------------- */
   /*  Write each column of 'pLocal' to a row of 'pLocalInvA'.           */
   /* ------------------------------------------------------------------- */

   /* set pLocalInvA matrix to identity */

   DSPLIB_qrd_identity_matrix_generate_exec_ci<dataType>(pLocalInvAScratch, heightR, colInvAStride, pBlock);
   DSPLIB_qrd_inverse_R_invA_exec_ci<dataType>(pLocalR, pLocalInvAScratch, widthR, colStrideR, colInvAStride,
                                               pFactArray, pBlock);
   /* pLocalInvA = inv_R * Q' */
   DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs, pLocalQ, pLocalR);
   DSPLIB_matMul_exec_ci<dataType>(pMatMulKerPrivArgs, pLocalInvAScratch, pLocalR, pLocalInvA);
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_qrd_inverse_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                         void *restrict pQ,
                                                         void *restrict pR,
                                                         void *restrict pInvA,
                                                         void *restrict pInvScratch,
                                                         void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_qrd_inverse_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                          void *restrict pQ,
                                                          void *restrict pR,
                                                          void *restrict pInvA,
                                                          void *restrict pInvScratch,
                                                          void *restrict pScratch);
