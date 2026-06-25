// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_priv.h"

int32_t DSPLIB_qrd_getHandleSize(DSPLIB_qrdInitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_qrd_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS DSPLIB_qrd_init_checkParams(DSPLIB_kernelHandle       handle,
                                          DSPLIB_bufParams2D_t     *bufParamsA,
                                          DSPLIB_bufParams2D_t     *bufParamsQ,
                                          DSPLIB_bufParams2D_t     *bufParamsR,
                                          DSPLIB_bufParams1D_t     *bufParamsU,
                                          const DSPLIB_qrdInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {

      /* Condition change */
      if ((bufParamsA->data_type != DSPLIB_FLOAT32) && (bufParamsA->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsA->data_type != bufParamsQ->data_type || bufParamsA->data_type != bufParamsR->data_type ||
               bufParamsA->data_type != bufParamsU->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }

      else {
         /* Do nothing */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_qrd_exec_checkParams(DSPLIB_kernelHandle handle,
                                          const void *restrict pA,
                                          const void *restrict pQ,
                                          const void *restrict pR,
                                          const void *restrict pU,
                                          const void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   if ((pA == NULL) || (pQ == NULL) || (pR == NULL) || (pU == NULL) || (pScratch == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_qrd_init(DSPLIB_kernelHandle       handle,
                              DSPLIB_bufParams2D_t     *bufParamsA,
                              DSPLIB_bufParams2D_t     *bufParamsQ,
                              DSPLIB_bufParams2D_t     *bufParamsR,
                              DSPLIB_bufParams1D_t     *bufParamsU,
                              const DSPLIB_qrdInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS        status       = DSPLIB_SUCCESS;
   DSPLIB_qrd_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_PrivArgs *) handle;

   pKerPrivArgs->widthA  = bufParamsA->dim_x;
   pKerPrivArgs->heightA = bufParamsA->dim_y;
   pKerPrivArgs->strideA = bufParamsA->stride_y;
   pKerPrivArgs->strideQ = bufParamsQ->stride_y;
   pKerPrivArgs->strideR = bufParamsR->stride_y;

   DSPLIB_DEBUGPRINTFN(0, "pKerInitArgs->funcStyle: %d bufParamsA->data_type: %d\n", pKerInitArgs->funcStyle,
                       bufParamsA->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsA->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_qrd_exec_cn<float>;
         status = DSPLIB_qrd_init_cn<float>(handle, bufParamsA, bufParamsQ, bufParamsR, bufParamsU, pKerInitArgs);
      }
      else if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_qrd_exec_cn<double>;
         status = DSPLIB_qrd_init_cn<double>(handle, bufParamsA, bufParamsQ, bufParamsR, bufParamsU, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsA->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_qrd_exec_ci<float>;
         status = DSPLIB_qrd_init_ci<float>(handle, bufParamsA, bufParamsQ, bufParamsR, bufParamsU, pKerInitArgs);
      }
      else if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_qrd_exec_ci<double>;
         status = DSPLIB_qrd_init_ci<double>(handle, bufParamsA, bufParamsQ, bufParamsR, bufParamsU, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_qrd_exec(DSPLIB_kernelHandle handle,
                              const void *restrict pA,
                              const void *restrict pQ,
                              const void *restrict pR,
                              const void *restrict pU,
                              const void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   DSPLIB_qrd_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "widthA: %d heightA: %d strideA: %d strideQ: %d strideR: %d\n", pKerPrivArgs->widthA,
                       pKerPrivArgs->heightA, pKerPrivArgs->strideA, pKerPrivArgs->strideQ, pKerPrivArgs->strideR);

   status = pKerPrivArgs->execute(handle, pA, pQ, pR, pU, pScratch);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
