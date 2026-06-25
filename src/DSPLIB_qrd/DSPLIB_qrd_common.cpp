// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_common.h"

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/
template <typename dataType>
void DSPLIB_qrd_identity_matrix_generate_init_ci(int32_t nRows, int32_t stride, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t                                                colStride = stride / sizeof(dataType);
   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SA_VECLEN                                            SA_VECLEN = c7x::sa_veclen<vec>::value;
   __SA_TEMPLATE_v1                                       sa0Params = __gen_SA_TEMPLATE_v1();

   sa0Params.ICNT0         = nRows;
   sa0Params.DIM1          = colStride;
   sa0Params.ICNT1         = nRows;
   sa0Params.DIMFMT        = __SA_DIMFMT_2D;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM1;
   sa0Params.DECDIM1_WIDTH = nRows * colStride;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock) = sa0Params;
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_identity_matrix_generate_init_ci<float>(int32_t nRows, int32_t stride, uint8_t *pBlock);
template void DSPLIB_qrd_identity_matrix_generate_init_ci<double>(int32_t nRows, int32_t strideQ, uint8_t *pBloc);

template <typename dataType>
void DSPLIB_qrd_blk_move_init_ci(int32_t nRows, int32_t nCols, int32_t strideR, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t colStrideOrder = strideR / sizeof(dataType);

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   __SE_TEMPLATE_v1 seMatrixParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saMatrixParams = __gen_SA_TEMPLATE_v1();

   seMatrixParams.ICNT0   = nCols;
   seMatrixParams.ICNT1   = nRows;
   seMatrixParams.DIM1    = colStrideOrder;
   seMatrixParams.DIMFMT  = __SE_DIMFMT_2D;
   seMatrixParams.ELETYPE = SE_ELETYPE;
   seMatrixParams.VECLEN  = SE_VECLEN;

   saMatrixParams.ICNT0  = nCols;
   saMatrixParams.ICNT1  = nRows;
   saMatrixParams.DIM1   = colStrideOrder;
   saMatrixParams.DIMFMT = __SA_DIMFMT_2D;
   saMatrixParams.VECLEN = SA_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock)                   = seMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE)) = saMatrixParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_blk_move_init_ci<float>(int32_t nRows, int32_t nCols, int32_t strideR, uint8_t *pBlock);
template void DSPLIB_qrd_blk_move_init_ci<double>(int32_t nRows, int32_t nCols, int32_t strideR, uint8_t *pBlock);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/
template <typename dataType>
void DSPLIB_qrd_identity_matrix_generate_exec_ci(dataType *pMat, int32_t nRows, int32_t colStride, uint8_t *pBlock)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock);

   int32_t iter1   = DSPLIB_ceilingDiv(nRows, eleCount);
   vec     zeroVec = (vec) 0;

   __SA0_OPEN(sa0Params);

   for (int32_t horizontal = 0; horizontal < iter1 * nRows; horizontal++) {
      __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pMat);
      __vstore_pred(pred, pStoreVec, zeroVec);
   }

   __SA0_CLOSE();

   sa1Params.ICNT0 = 1;
   sa1Params.ICNT1 = nRows / 2;
   sa1Params.DIM1  = 2 * (colStride + 1);

   sa0Params.ICNT0 = 1;
   sa0Params.ICNT1 = nRows - sa1Params.ICNT1;
   sa0Params.DIM1  = 2 * (colStride + 1);

   __SA0_OPEN(sa0Params);
   if (sa1Params.ICNT1 != 0) {
      __SA1_OPEN(sa1Params);
   }

   for (int32_t diag = 0; diag < nRows; diag += 2) {
      dataType *pDiag  = c7x::strm_agen<0, dataType>::get_adv(pMat);
      *pDiag           = 1.0;
      dataType *pDiag1 = c7x::strm_agen<1, dataType>::get_adv(pMat + (colStride + 1));
      *pDiag1          = 1.0;
   }
   __SA0_CLOSE();
   __SA1_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void
DSPLIB_qrd_identity_matrix_generate_exec_ci<float>(float *pMat, int32_t nRows, int32_t colStride, uint8_t *pBlock);
template void
DSPLIB_qrd_identity_matrix_generate_exec_ci<double>(double *pMat, int32_t nRows, int32_t colStride, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_qrd_blk_move_exec_ci(dataType *pOut,
                                 dataType *pIn,
                                 int32_t   Nrows,
                                 int32_t   Ncols,
                                 int32_t   colOutStride,
                                 int32_t   colInStride,
                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t eleCount  = c7x::element_count_of<vec>::value;
   int32_t nVec      = DSPLIB_ceilingDiv(Ncols, eleCount);
   int32_t loopCount = nVec * Nrows;

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
template void DSPLIB_qrd_blk_move_exec_ci<float>(float   *pOut,
                                                 float   *pIn,
                                                 int32_t  Nrows,
                                                 int32_t  Ncols,
                                                 int32_t  colOutStride,
                                                 int32_t  colInStride,
                                                 uint8_t *pBlock);
template void DSPLIB_qrd_blk_move_exec_ci<double>(double  *pOut,
                                                  double  *pIn,
                                                  int32_t  Nrows,
                                                  int32_t  Ncols,
                                                  int32_t  colOutStride,
                                                  int32_t  colInStride,
                                                  uint8_t *pBlock);
