// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cholesky_inplace_priv.h"

int32_t DSPLIB_cholesky_inplace_getHandleSize(DSPLIB_cholesky_inplace_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_cholesky_inplace_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_cholesky_inplace_init_checkParams(DSPLIB_kernelHandle                     handle,
                                                       DSPLIB_bufParams2D_t                   *bufParamsA,
                                                       DSPLIB_bufParams1D_t                   *bufParamsMul,
                                                       const DSPLIB_cholesky_inplace_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsA->data_type != DSPLIB_FLOAT32) && (bufParamsA->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
      else if (bufParamsA->dim_x != bufParamsA->dim_y) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
      else if (bufParamsMul->dim_x != bufParamsA->dim_x) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
      else {
         /* Nothing to do here */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_cholesky_inplace_init(DSPLIB_kernelHandle                     handle,
                                           DSPLIB_bufParams2D_t                   *bufParamsA,
                                           DSPLIB_bufParams1D_t                   *bufParamsMul,
                                           const DSPLIB_cholesky_inplace_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS                     status       = DSPLIB_SUCCESS;
   DSPLIB_cholesky_inplace_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_inplace_PrivArgs *) handle;

   pKerPrivArgs->order      = pKerInitArgs->order;
   pKerPrivArgs->stride     = pKerInitArgs->stride;
   pKerPrivArgs->enableTest = pKerInitArgs->enableTest;

   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->order: %d pKerPrivArgs->stride: %d pKerPrivArgs->enableTest: %d\n",
                       pKerPrivArgs->order, pKerPrivArgs->stride, pKerPrivArgs->enableTest);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsA->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_cholesky_inplace_exec_cn<float>;
      }
      else if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_cholesky_inplace_exec_cn<double>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
   }
   else {
      if (bufParamsA->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_cholesky_inplace_exec_ci<float>;
         status                = DSPLIB_cholesky_inplace_init_ci<float>(handle, bufParamsA, bufParamsMul, pKerInitArgs);
      }
      else if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_cholesky_inplace_exec_ci<double>;
         status = DSPLIB_cholesky_inplace_init_ci<double>(handle, bufParamsA, bufParamsMul, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS
DSPLIB_cholesky_inplace_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pA, const void *restrict pMul)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status;

   if ((pA == NULL) || (pMul == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
      DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_cholesky_inplace_exec(DSPLIB_kernelHandle handle, void *restrict pA, void *restrict pMul)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status;

   DSPLIB_cholesky_inplace_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_inplace_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->enableTest: %d pKerPrivArgs->order: %d\n", pKerPrivArgs->enableTest,
                       pKerPrivArgs->order);
   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->stride: %d\n", pKerPrivArgs->stride);

   status = pKerPrivArgs->execute(handle, pA, pMul);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}
