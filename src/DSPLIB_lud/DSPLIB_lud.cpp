// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_priv.h"

int32_t DSPLIB_lud_getHandleSize(DSPLIB_ludInitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_lud_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS DSPLIB_lud_init_checkParams(DSPLIB_kernelHandle         handle,
                                          const DSPLIB_bufParams2D_t *bufParamsA,
                                          const DSPLIB_bufParams2D_t *bufParamsL,
                                          const DSPLIB_bufParams2D_t *bufParamsU,
                                          const DSPLIB_bufParams2D_t *bufParamsP,
                                          const DSPLIB_ludInitArgs   *pKerInitArgs)
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
      else if (bufParamsP->data_type != DSPLIB_UINT16) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsA->data_type != bufParamsL->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsA->data_type != bufParamsU->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsA->dim_x != bufParamsA->dim_y) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else if (bufParamsL->dim_x != bufParamsL->dim_y) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }

      else if (bufParamsU->dim_x != bufParamsU->dim_y) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }

      else if (bufParamsP->dim_x != bufParamsP->dim_y) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else {
         /* Do nothing */
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_lud_exec_checkParams(DSPLIB_kernelHandle handle,
                                          const void *restrict pA,
                                          const void *restrict pL,
                                          const void *restrict pU,
                                          const void *restrict pP)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   if ((pA == NULL) || (pL == NULL) || (pU == NULL) || (pP == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_lud_init(DSPLIB_kernelHandle         handle,
                              const DSPLIB_bufParams2D_t *bufParamsA,
                              const DSPLIB_bufParams2D_t *bufParamsL,
                              const DSPLIB_bufParams2D_t *bufParamsU,
                              const DSPLIB_bufParams2D_t *bufParamsP,
                              const DSPLIB_ludInitArgs   *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS        status       = DSPLIB_SUCCESS;
   DSPLIB_lud_PrivArgs *pKerPrivArgs = (DSPLIB_lud_PrivArgs *) handle;

   pKerPrivArgs->order       = bufParamsA->dim_x;
   pKerPrivArgs->strideOrder = bufParamsA->stride_y;
   pKerPrivArgs->strideP     = bufParamsP->stride_y;

   DSPLIB_DEBUGPRINTFN(0, "pKerInitArgs->funcStyle: %d bufParamsA->data_type: %d\n", pKerInitArgs->funcStyle,
                       bufParamsA->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsA->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_lud_exec_cn<float>;
         status = DSPLIB_lud_init_cn(handle, bufParamsA, bufParamsL, bufParamsU, bufParamsP, pKerInitArgs);
      }
      else if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_lud_exec_cn<double>;
         status = DSPLIB_lud_init_cn(handle, bufParamsA, bufParamsL, bufParamsU, bufParamsP, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsA->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_lud_exec_ci<float>;
         status = DSPLIB_lud_init_ci<float>(handle, bufParamsA, bufParamsL, bufParamsU, bufParamsP, pKerInitArgs);
      }
      else if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_lud_exec_ci<double>;
         status = DSPLIB_lud_init_ci<double>(handle, bufParamsA, bufParamsL, bufParamsU, bufParamsP, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS
DSPLIB_lud_exec(DSPLIB_kernelHandle handle, void *restrict pA, void *restrict pL, void *restrict pU, void *restrict pP)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   DSPLIB_lud_PrivArgs *pKerPrivArgs = (DSPLIB_lud_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "order: %d \n", pKerPrivArgs->order);

   status = pKerPrivArgs->execute(handle, pA, pL, pU, pP);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
