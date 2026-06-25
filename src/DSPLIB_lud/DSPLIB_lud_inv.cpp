// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_inv_priv.h"

int32_t DSPLIB_lud_inv_getHandleSize(DSPLIB_lud_invInitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_lud_inv_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS DSPLIB_lud_inv_init_checkParams(DSPLIB_kernelHandle           handle,
                                              const DSPLIB_bufParams2D_t   *bufParamsP,
                                              const DSPLIB_bufParams2D_t   *bufParamsL,
                                              const DSPLIB_bufParams2D_t   *bufParamsU,
                                              const DSPLIB_bufParams2D_t   *bufParamsinvA,
                                              const DSPLIB_lud_invInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {

      /* Condition change */
      if (((bufParamsL->data_type != DSPLIB_FLOAT32) && (bufParamsL->data_type != DSPLIB_FLOAT64)) ||
          ((bufParamsU->data_type != DSPLIB_FLOAT32) && (bufParamsU->data_type != DSPLIB_FLOAT64)) ||
          ((bufParamsP->data_type != DSPLIB_UINT16))) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsL->data_type != bufParamsinvA->data_type || bufParamsU->data_type != bufParamsinvA->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
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

DSPLIB_STATUS DSPLIB_lud_inv_exec_checkParams(DSPLIB_kernelHandle handle,
                                              void *restrict pP,
                                              void *restrict pL,
                                              void *restrict pU,
                                              void *restrict pinvA,
                                              void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   if ((pP == NULL) || (pL == NULL) || (pU == NULL) || (pinvA == NULL) || (pScratch == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_lud_inv_init(DSPLIB_kernelHandle           handle,
                                  const DSPLIB_bufParams2D_t   *bufParamsP,
                                  const DSPLIB_bufParams2D_t   *bufParamsL,
                                  const DSPLIB_bufParams2D_t   *bufParamsU,
                                  const DSPLIB_bufParams2D_t   *bufParamsinvA,
                                  const DSPLIB_lud_invInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS            status       = DSPLIB_SUCCESS;
   DSPLIB_lud_inv_PrivArgs *pKerPrivArgs = (DSPLIB_lud_inv_PrivArgs *) handle;

   pKerPrivArgs->order       = bufParamsL->dim_x;
   pKerPrivArgs->strideOrder = bufParamsL->stride_y;
   pKerPrivArgs->strideP     = bufParamsP->stride_y;

   DSPLIB_DEBUGPRINTFN(0, "pKerInitArgs->funcStyle: %d bufParamsinvA->data_type: %d\n", pKerInitArgs->funcStyle,
                       bufParamsinvA->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if ((bufParamsL->data_type == DSPLIB_FLOAT32) || (bufParamsU->data_type == DSPLIB_FLOAT32)) {
         pKerPrivArgs->execute = DSPLIB_lud_inv_exec_cn<float>;
         status = DSPLIB_lud_inv_init_cn(handle, bufParamsP, bufParamsL, bufParamsU, bufParamsinvA, pKerInitArgs);
      }
      else if ((bufParamsL->data_type == DSPLIB_FLOAT64) || (bufParamsU->data_type == DSPLIB_FLOAT64)) {
         pKerPrivArgs->execute = DSPLIB_lud_inv_exec_cn<double>;
         status = DSPLIB_lud_inv_init_cn(handle, bufParamsP, bufParamsL, bufParamsU, bufParamsinvA, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if ((bufParamsL->data_type == DSPLIB_FLOAT32) || (bufParamsU->data_type == DSPLIB_FLOAT32)) {
         pKerPrivArgs->execute = DSPLIB_lud_inv_exec_ci<float>;
         status =
             DSPLIB_lud_inv_init_ci<float>(handle, bufParamsP, bufParamsL, bufParamsU, bufParamsinvA, pKerInitArgs);
      }
      else if ((bufParamsL->data_type == DSPLIB_FLOAT64) || (bufParamsU->data_type == DSPLIB_FLOAT64)) {
         pKerPrivArgs->execute = DSPLIB_lud_inv_exec_ci<double>;
         status =
             DSPLIB_lud_inv_init_ci<double>(handle, bufParamsP, bufParamsL, bufParamsU, bufParamsinvA, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_lud_inv_exec(DSPLIB_kernelHandle handle,
                                  void *restrict pP,
                                  void *restrict pL,
                                  void *restrict pU,
                                  void *restrict pinvA,
                                  void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   DSPLIB_lud_inv_PrivArgs *pKerPrivArgs = (DSPLIB_lud_inv_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "order: %d\n", pKerPrivArgs->order);

   status = pKerPrivArgs->execute(handle, pP, pL, pU, pinvA, pScratch);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
