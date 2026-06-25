// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cholesky_priv.h"

int32_t DSPLIB_cholesky_getHandleSize(DSPLIB_cholesky_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_cholesky_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_cholesky_init_checkParams(DSPLIB_kernelHandle             handle,
                                               const DSPLIB_bufParams2D_t     *bufParamsIn,
                                               const DSPLIB_bufParams2D_t     *bufParamsOut,
                                               const DSPLIB_cholesky_InitArgs *pKerInitArgs)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
      DSPLIB_DEBUGPRINTFN(0, "NULL pointer status: %d\n", status);
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn->data_type != DSPLIB_FLOAT32) && (bufParamsIn->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "Invalid type status: %d\n", status);
      }
      else if ((bufParamsIn->data_type != bufParamsOut->data_type)) {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "Invalid type status: %d\n", status);
      }
      else {
         /* Nothing to do here */
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_cholesky_init(DSPLIB_kernelHandle             handle,
                                   DSPLIB_bufParams2D_t           *bufParamsIn,
                                   DSPLIB_bufParams2D_t           *bufParamsOut,
                                   const DSPLIB_cholesky_InitArgs *pKerInitArgs)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS             status       = DSPLIB_SUCCESS;
   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;

   pKerPrivArgs->order      = pKerInitArgs->order;
   pKerPrivArgs->stride     = pKerInitArgs->stride;
   pKerPrivArgs->strideCn   = pKerInitArgs->strideCn;
   pKerPrivArgs->enableTest = pKerInitArgs->enableTest;

   DSPLIB_DEBUGPRINTFN(
       0, "pKerPrivArgs->order: %d pKerPrivArgs->stride: %d pKerPrivArgs->strideCn: %d pKerPrivArgs->enableTest: %d\n",
       pKerPrivArgs->order, pKerPrivArgs->stride, pKerPrivArgs->strideCn, pKerPrivArgs->enableTest);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_cholesky_exec_cn<float>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_cholesky_exec_cn<double>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;

         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_cholesky_exec_ci<float>;
         status                = DSPLIB_cholesky_init_ci<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_cholesky_exec_ci<double>;
         status                = DSPLIB_cholesky_init_ci<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_cholesky_exec_checkParams(DSPLIB_kernelHandle handle,
                                               const void *restrict pInA,
                                               const void *restrict pOutU,
                                               const void *restrict pMul)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   if ((pInA == NULL) || (pOutU == NULL) || (pMul == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
      DSPLIB_DEBUGPRINTFN(0, "NULL pointer status: %d\n", status);
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS
DSPLIB_cholesky_exec(DSPLIB_kernelHandle handle, void *restrict pInA, void *restrict pOutU, void *restrict pMul)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->enableTest: %d pKerPrivArgs->order: %d\n", pKerPrivArgs->enableTest,
                       pKerPrivArgs->order);
   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->stride: %d pKerPrivArgs->strideCn: %d\n", pKerPrivArgs->stride,
                       pKerPrivArgs->strideCn);

   status = pKerPrivArgs->execute(handle, pInA, pOutU, pMul);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}
