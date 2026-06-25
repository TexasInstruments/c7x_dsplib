// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_priv.h"

int32_t DSPLIB_svd_getHandleSize(DSPLIB_svdInitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_svd_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS DSPLIB_svd_init_checkParams(DSPLIB_kernelHandle         handle,
                                          const DSPLIB_bufParams2D_t *bufParamsA,
                                          const DSPLIB_bufParams2D_t *bufParamsU,
                                          const DSPLIB_bufParams2D_t *bufParamsV,
                                          const DSPLIB_bufParams1D_t *bufParamsDiag,
                                          const DSPLIB_bufParams1D_t *bufParamsSuperDiag,
                                          const DSPLIB_svdInitArgs   *pKerInitArgs)
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
      else if (bufParamsA->data_type != bufParamsU->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsA->data_type != bufParamsV->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsA->data_type != bufParamsDiag->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsA->data_type != bufParamsSuperDiag->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Do nothing */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_svd_exec_checkParams(DSPLIB_kernelHandle handle,
                                          const void *restrict pA,
                                          const void *restrict pU,
                                          const void *restrict pV,
                                          const void *restrict pDiag,
                                          const void *restrict pSuperDiag,
                                          const void *restrict pU1,
                                          const void *restrict pV1,
                                          const void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   if ((pA == NULL) || (pU == NULL) || (pV == NULL) || (pDiag == NULL) || (pSuperDiag == NULL) || (pU1 == NULL) ||
       (pV1 == NULL) || (pScratch == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_svd_init(DSPLIB_kernelHandle         handle,
                              const DSPLIB_bufParams2D_t *bufParamsA,
                              const DSPLIB_bufParams2D_t *bufParamsU,
                              const DSPLIB_bufParams2D_t *bufParamsV,
                              const DSPLIB_bufParams1D_t *bufParamsDiag,
                              const DSPLIB_bufParams1D_t *bufParamsSuperDiag,
                              const DSPLIB_svdInitArgs   *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS        status       = DSPLIB_SUCCESS;
   DSPLIB_svd_PrivArgs *pKerPrivArgs = (DSPLIB_svd_PrivArgs *) handle;

   pKerPrivArgs->widthIn           = bufParamsA->dim_x;
   pKerPrivArgs->heightIn          = bufParamsA->dim_y;
   pKerPrivArgs->strideIn          = bufParamsA->stride_y;
   pKerPrivArgs->strideU           = bufParamsU->stride_y;
   pKerPrivArgs->strideV           = bufParamsV->stride_y;
   pKerPrivArgs->enableReducedForm = pKerInitArgs->enableReducedForm;
   pKerPrivArgs->strideURows       = pKerInitArgs->strideURows;
   pKerPrivArgs->strideVRows       = pKerInitArgs->strideVRows;

   DSPLIB_DEBUGPRINTFN(0, "pKerInitArgs->funcStyle: %d bufParamsA->data_type: %d\n", pKerInitArgs->funcStyle,
                       bufParamsA->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsA->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_svd_exec_cn<float>;
         status = DSPLIB_svd_init_cn(handle, bufParamsA, bufParamsU, bufParamsV, bufParamsDiag, bufParamsSuperDiag,
                                     pKerInitArgs);
      }
      else if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_svd_exec_cn<double>;
         status = DSPLIB_svd_init_cn(handle, bufParamsA, bufParamsU, bufParamsV, bufParamsDiag, bufParamsSuperDiag,
                                     pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsA->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_svd_exec_ci<float>;
         status                = DSPLIB_svd_init_ci<float>(handle, bufParamsA, bufParamsU, bufParamsV, bufParamsDiag,
                                            bufParamsSuperDiag, pKerInitArgs);
      }
      else if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_svd_exec_ci<double>;
         status                = DSPLIB_svd_init_ci<double>(handle, bufParamsA, bufParamsU, bufParamsV, bufParamsDiag,
                                             bufParamsSuperDiag, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_svd_exec(DSPLIB_kernelHandle handle,
                              void *restrict pA,
                              void *restrict pU,
                              void *restrict pV,
                              void *restrict pDiag,
                              void *restrict pSuperDiag,
                              void *restrict pU1,
                              void *restrict pV1,
                              void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   DSPLIB_svd_PrivArgs *pKerPrivArgs = (DSPLIB_svd_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "widthIn: %d heightIn: %d strideIn: %d strideU: %d srideV: %d\n", pKerPrivArgs->widthIn,
                       pKerPrivArgs->heightIn, pKerPrivArgs->strideIn, pKerPrivArgs->strideU, pKerPrivArgs->strideV);

   status = pKerPrivArgs->execute(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
