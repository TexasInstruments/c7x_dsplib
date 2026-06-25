// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cholesky_solver_priv.h"

int32_t DSPLIB_cholesky_solver_getHandleSize(DSPLIB_cholesky_solver_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_cholesky_solver_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_cholesky_solver_init_checkParams(DSPLIB_kernelHandle                    handle,
                                                      DSPLIB_bufParams2D_t                  *bufParamsU,
                                                      DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                                      DSPLIB_bufParams1D_t                  *bufParamsY,
                                                      DSPLIB_bufParams1D_t                  *bufParamsB,
                                                      DSPLIB_bufParams1D_t                  *bufParamsX,
                                                      DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                                      const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsU->data_type != DSPLIB_FLOAT32) && (bufParamsU->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
      else if ((bufParamsY->data_type != bufParamsU->data_type) || (bufParamsB->data_type != bufParamsU->data_type) ||
               (bufParamsX->data_type != bufParamsU->data_type) || (bufParamsDiv->data_type != bufParamsU->data_type)) {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
      else {
         /* Nothing to do here */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_cholesky_solver_init(DSPLIB_kernelHandle                    handle,
                                          DSPLIB_bufParams2D_t                  *bufParamsU,
                                          DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                          DSPLIB_bufParams1D_t                  *bufParamsY,
                                          DSPLIB_bufParams1D_t                  *bufParamsB,
                                          DSPLIB_bufParams1D_t                  *bufParamsX,
                                          DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                          const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS                    status       = DSPLIB_SUCCESS;
   DSPLIB_cholesky_solver_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_solver_PrivArgs *) handle;

   pKerPrivArgs->order    = pKerInitArgs->order;
   pKerPrivArgs->stride   = pKerInitArgs->stride;
   pKerPrivArgs->strideCn = pKerInitArgs->strideCn;

   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->order: %d pKerPrivArgs->stride: %d pKerPrivArgs->strideCn: %d\n",
                       pKerPrivArgs->order, pKerPrivArgs->stride, pKerPrivArgs->strideCn);
   DSPLIB_DEBUGPRINTFN(0, "bufParamsU->data_type: %d d\n", bufParamsU->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsU->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_cholesky_solver_exec_cn<float>;
      }
      else if (bufParamsU->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_cholesky_solver_exec_cn<double>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
   }
   else {
      if (bufParamsU->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_cholesky_solver_exec_ci<float>;
         status = DSPLIB_cholesky_solver_init_ci<float>(handle, bufParamsU, bufParamsScratch, bufParamsY, bufParamsB,
                                                        bufParamsX, bufParamsDiv, pKerInitArgs);
      }
      else if (bufParamsU->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_cholesky_solver_exec_ci<double>;
         status = DSPLIB_cholesky_solver_init_ci<double>(handle, bufParamsU, bufParamsScratch, bufParamsY, bufParamsB,
                                                         bufParamsX, bufParamsDiv, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_cholesky_solver_exec_checkParams(DSPLIB_kernelHandle handle,
                                                      const void *restrict pU,
                                                      const void *restrict pScratch,
                                                      const void *restrict pY,
                                                      const void *restrict pB,
                                                      const void *restrict pX,
                                                      const void *restrict pDiv)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status;

   if ((pU == NULL) || (pScratch == NULL) || (pY == NULL) || (pB == NULL) || (pX == NULL) || (pDiv == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
      DSPLIB_DEBUGPRINTFN(0, "status: %d\n", status);
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_cholesky_solver_exec(DSPLIB_kernelHandle handle,
                                          void *restrict pU,
                                          void *restrict pScratch,
                                          void *restrict pY,
                                          void *restrict pB,
                                          void *restrict pX,
                                          void *restrict pDiv)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   DSPLIB_cholesky_solver_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_solver_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->stride: %d pKerPrivArgs->order: %d\n", pKerPrivArgs->stride,
                       pKerPrivArgs->order);

   status = pKerPrivArgs->execute(handle, pU, pScratch, pY, pB, pX, pDiv);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}
