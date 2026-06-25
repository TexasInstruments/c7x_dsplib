// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_mat_submat_copy_priv.h"

/*******************************************************************************
 *
 * INITIALIZATION
 *
 *******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci(DSPLIB_kernelHandle                   handle,
                                             const DSPLIB_bufParams2D_t           *bufParamsX,
                                             const DSPLIB_bufParams2D_t           *bufParamsY,
                                             const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   DSPLIB_mat_submat_copy_PrivArgs *pKerPrivArgs = (DSPLIB_mat_submat_copy_PrivArgs *) handle;

   uint8_t *pBlock     = pKerPrivArgs->bufPblock;
   int32_t  nCols      = pKerPrivArgs->nCols;
   int32_t  nRows      = pKerPrivArgs->nRows;
   int32_t  colStrideX = pKerPrivArgs->strideX / sizeof(dataType);
   int32_t  colStrideY = pKerPrivArgs->strideY / sizeof(dataType);
   int32_t  dir        = pKerPrivArgs->dir;

   int32_t outMatStride = 0;
   int32_t inMatStride  = 0;

   if (dir == 0) {
      outMatStride = colStrideY;
      inMatStride  = colStrideX;
   }
   else {
      outMatStride = colStrideX;
      inMatStride  = colStrideY;
   }

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   __SE_TEMPLATE_v1 se0MatParams = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1MatParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0MatParams = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa1MatParams = __gen_SA_TEMPLATE_v1();

   int32_t blocks   = DSPLIB_ceilingDiv(nRows, (2));
   int32_t se0ICNT1 = blocks;
   int32_t se1ICNT1 = nRows - blocks;

   se0MatParams.ICNT0         = nCols;
   se0MatParams.DIM1          = 2 * inMatStride;
   se0MatParams.ICNT1         = se0ICNT1;
   se0MatParams.DIMFMT        = __SE_DIMFMT_2D;
   se0MatParams.ELETYPE       = SE_ELETYPE;
   se0MatParams.VECLEN        = SE_VECLEN;
   se0MatParams.DECDIM1       = __SE_DECDIM_DIM1;
   se0MatParams.DECDIM1_WIDTH = nRows * inMatStride;

   se1MatParams.ICNT0         = nCols;
   se1MatParams.DIM1          = 2 * inMatStride;
   se1MatParams.ICNT1         = se1ICNT1;
   se1MatParams.DIMFMT        = __SE_DIMFMT_2D;
   se1MatParams.ELETYPE       = SE_ELETYPE;
   se1MatParams.VECLEN        = SE_VECLEN;
   se1MatParams.DECDIM1       = __SE_DECDIM_DIM1;
   se1MatParams.DECDIM1_WIDTH = nRows * inMatStride;

   sa0MatParams.ICNT0         = nCols;
   sa0MatParams.DIM1          = 2 * outMatStride;
   sa0MatParams.ICNT1         = se0ICNT1;
   sa0MatParams.DIMFMT        = __SA_DIMFMT_2D;
   sa0MatParams.VECLEN        = SA_VECLEN;
   sa0MatParams.DECDIM1       = __SA_DECDIM_DIM1;
   sa0MatParams.DECDIM1_WIDTH = nRows * outMatStride;

   sa1MatParams.ICNT0         = nCols;
   sa1MatParams.DIM1          = 2 * outMatStride;
   sa1MatParams.ICNT1         = se1ICNT1;
   sa1MatParams.DIMFMT        = __SA_DIMFMT_2D;
   sa1MatParams.VECLEN        = SA_VECLEN;
   sa1MatParams.DECDIM1       = __SA_DECDIM_DIM1;
   sa1MatParams.DECDIM1_WIDTH = nRows * outMatStride;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock)                       = se0MatParams;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE)) = se1MatParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = sa0MatParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = sa1MatParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}
template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<int8_t>(DSPLIB_kernelHandle                   handle,
                                                              const DSPLIB_bufParams2D_t           *bufParamsX,
                                                              const DSPLIB_bufParams2D_t           *bufParamsY,
                                                              const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<uint8_t>(DSPLIB_kernelHandle                   handle,
                                                               const DSPLIB_bufParams2D_t           *bufParamsX,
                                                               const DSPLIB_bufParams2D_t           *bufParamsY,
                                                               const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<int16_t>(DSPLIB_kernelHandle                   handle,
                                                               const DSPLIB_bufParams2D_t           *bufParamsX,
                                                               const DSPLIB_bufParams2D_t           *bufParamsY,
                                                               const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<uint16_t>(DSPLIB_kernelHandle                   handle,
                                                                const DSPLIB_bufParams2D_t           *bufParamsX,
                                                                const DSPLIB_bufParams2D_t           *bufParamsY,
                                                                const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<int32_t>(DSPLIB_kernelHandle                   handle,
                                                               const DSPLIB_bufParams2D_t           *bufParamsX,
                                                               const DSPLIB_bufParams2D_t           *bufParamsY,
                                                               const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<uint32_t>(DSPLIB_kernelHandle                   handle,
                                                                const DSPLIB_bufParams2D_t           *bufParamsX,
                                                                const DSPLIB_bufParams2D_t           *bufParamsY,
                                                                const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<uint64_t>(DSPLIB_kernelHandle                   handle,
                                                                const DSPLIB_bufParams2D_t           *bufParamsX,
                                                                const DSPLIB_bufParams2D_t           *bufParamsY,
                                                                const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<int64_t>(DSPLIB_kernelHandle                   handle,
                                                               const DSPLIB_bufParams2D_t           *bufParamsX,
                                                               const DSPLIB_bufParams2D_t           *bufParamsY,
                                                               const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<float>(DSPLIB_kernelHandle                   handle,
                                                             const DSPLIB_bufParams2D_t           *bufParamsX,
                                                             const DSPLIB_bufParams2D_t           *bufParamsY,
                                                             const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci<double>(DSPLIB_kernelHandle                   handle,
                                                              const DSPLIB_bufParams2D_t           *bufParamsX,
                                                              const DSPLIB_bufParams2D_t           *bufParamsY,
                                                              const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);

/*******************************************************************************/
/* IMPLEMENTATION                                                              */
/*******************************************************************************/
template <typename dataType>
DSPLIB_STATUS DSPLIB_mat_submat_copy_exec_ci(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS                    status       = DSPLIB_SUCCESS;
   DSPLIB_mat_submat_copy_PrivArgs *pKerPrivArgs = (DSPLIB_mat_submat_copy_PrivArgs *) handle;
   int32_t                          strideX      = pKerPrivArgs->strideX;
   int32_t                          strideY      = pKerPrivArgs->strideY;
   int32_t                          dir          = pKerPrivArgs->dir;
   uint8_t                         *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                          stRow        = pKerPrivArgs->stRow;
   int32_t                          stCol        = pKerPrivArgs->stCol;
   int32_t                          nCols        = pKerPrivArgs->nCols;
   int32_t                          colStrideX   = strideX / sizeof(dataType);
   int32_t                          colStrideY   = strideY / sizeof(dataType);

   /* Typecast void pointers to respective data type */
   dataType *yLocal = (dataType *) pY;
   dataType *xLocal = (dataType *) pX;

   DSPLIB_DEBUGPRINTFN(0, "yLocal: %p xLocal: %p nRows : %d nCols: %d stRow: %d stCol:%d dir: %d \n", yLocal, xLocal,
                       pKerPrivArgs->nRows, nCols, stRow, stCol, dir);
   uint32_t i, j;
   /* ------------------------------------------------------------------- */
   /*  Copy sub matrix                                                    */
   /* ------------------------------------------------------------------- */

   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   dataType *ptrSA0 = NULL;
   dataType *ptrSA1 = NULL;
   dataType *ptrSE0 = NULL;
   dataType *ptrSE1 = NULL;

   __SE_TEMPLATE_v1 se0MatParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock);
   __SE_TEMPLATE_v1 se1MatParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0MatParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa1MatParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));

   if (dir == 0) {
      ptrSA0 = &yLocal[0];
      ptrSA1 = &yLocal[colStrideY];
      ptrSE0 = &xLocal[stRow * colStrideX + stCol];
      ptrSE1 = &xLocal[(stRow + 1) * colStrideX + stCol];
   }
   else {
      ptrSE0 = &yLocal[0];
      ptrSE1 = &yLocal[colStrideY];
      ptrSA0 = &xLocal[stRow * colStrideX + stCol];
      ptrSA1 = &xLocal[(stRow + 1) * colStrideX + stCol];
   }

   int32_t nTile   = DSPLIB_ceilingDiv(nCols, eleCount);
   int32_t loopCnt = se1MatParams.ICNT1 * nTile;

   __SE0_OPEN(ptrSE0, se0MatParams);
   __SA0_OPEN(sa0MatParams);

   vec v1;
   vec v2;

   if (se1MatParams.ICNT1) {
      __SE1_OPEN(ptrSE1, se1MatParams);
      __SA1_OPEN(sa1MatParams);
   }

   for (i = 0; i < (uint32_t) loopCnt; i++) {
      /* 1 + trip_cnt * 2 */
      v1 = c7x::strm_eng<0, vec>::get_adv();
      v2 = c7x::strm_eng<1, vec>::get_adv();

      __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(ptrSA0);
      __vstore_pred(pred, pStoreVec, v1);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv(ptrSA1);
      __vstore_pred(pred, pStoreVec, v2);
   }
   /* To handle odd number of rows */
   if (se0MatParams.ICNT1 != se1MatParams.ICNT1) {
      for (j = 0; j < (uint32_t) nTile; j++) {
         v1 = c7x::strm_eng<0, vec>::get_adv();

         __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(ptrSA0);
         __vstore_pred(pred, pStoreVec, v1);
      }
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
   if (se1MatParams.ICNT1) {
      __SE1_CLOSE();
      __SA1_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
