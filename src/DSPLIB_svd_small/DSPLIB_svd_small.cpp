// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_small_priv.h"

int32_t DSPLIB_svd_small_getHandleSize(DSPLIB_svd_small_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_svd_small_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS DSPLIB_svd_small_init_checkParams(DSPLIB_kernelHandle              handle,
                                                const DSPLIB_bufParams2D_t      *bufParamsA,
                                                const DSPLIB_bufParams2D_t      *bufParamsU,
                                                const DSPLIB_bufParams2D_t      *bufParamsV,
                                                const DSPLIB_bufParams1D_t      *bufParamsDiag,
                                                const DSPLIB_bufParams1D_t      *bufParamsSuperDiag,
                                                const DSPLIB_svd_small_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if (__C7X_VEC_SIZE_BITS__ == 512)

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {

      /* Condition change */
      if (bufParamsA->data_type != DSPLIB_FLOAT64) {
         DSPLIB_DEBUGPRINTFN(0, "%s\n", "Only double precision data is supported");
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

      bool condWidth  = (bufParamsA->dim_y == 6);
      bool condHeight = (bufParamsA->dim_x == 6) || (bufParamsA->dim_x == 7);

      if (!(bool) ((uint32_t) condWidth & (uint32_t) condHeight)) {
         DSPLIB_DEBUGPRINTFN(0, "%s\n", "Valid dimensions are 6 X 6 and 6 X 7");
         status = DSPLIB_ERR_NOT_IMPLEMENTED;
      }
   }

#else
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "The Kernel is NOT IMPLEMENTED for __C7X_VEC_SIZE_BITS__ != 512 ");
   status = DSPLIB_ERR_NOT_IMPLEMENTED;
#endif
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_svd_small_exec_checkParams(DSPLIB_kernelHandle handle,
                                                const void *restrict pA,
                                                const void *restrict pU,
                                                const void *restrict pV,
                                                const void *restrict pDiag,
                                                const void *restrict pSuperDiag,
                                                const void *restrict pU1)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
#if (__C7X_VEC_SIZE_BITS__ == 512)

   if ((pA == NULL) || (pU == NULL) || (pV == NULL) || (pDiag == NULL) || (pSuperDiag == NULL) || (pU1 == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

#else
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "The Kernel is NOT IMPLEMENTED for __C7X_VEC_SIZE_BITS__ != 512 ");
   status = DSPLIB_ERR_NOT_IMPLEMENTED;
#endif

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_svd_small_init(DSPLIB_kernelHandle              handle,
                                    const DSPLIB_bufParams2D_t      *bufParamsA,
                                    const DSPLIB_bufParams2D_t      *bufParamsU,
                                    const DSPLIB_bufParams2D_t      *bufParamsV,
                                    const DSPLIB_bufParams1D_t      *bufParamsDiag,
                                    const DSPLIB_bufParams1D_t      *bufParamsSuperDiag,
                                    const DSPLIB_svd_small_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if (__C7X_VEC_SIZE_BITS__ == 512)

   DSPLIB_svd_small_PrivArgs *pKerPrivArgs = (DSPLIB_svd_small_PrivArgs *) handle;

   pKerPrivArgs->widthIn           = bufParamsA->dim_x;
   pKerPrivArgs->heightIn          = bufParamsA->dim_y;
   pKerPrivArgs->strideIn          = bufParamsA->stride_y;
   pKerPrivArgs->strideU           = bufParamsU->stride_y;
   pKerPrivArgs->strideV           = bufParamsV->stride_y;
   pKerPrivArgs->enableReducedForm = pKerInitArgs->enableReducedForm;
   pKerPrivArgs->strideURows       = pKerInitArgs->strideURows;
   pKerPrivArgs->strideVRows       = pKerInitArgs->strideVRows;
   pKerPrivArgs->data_type         = bufParamsA->data_type;

   DSPLIB_DEBUGPRINTFN(0, "pKerInitArgs->funcStyle: %d bufParamsA->data_type: %d\n", pKerInitArgs->funcStyle,
                       bufParamsA->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      //    status                = DSPLIB_svd_small_init_cn(handle, bufParamsA, bufParamsU, bufParamsV, bufParamsDiag,
      //                                                     bufParamsSuperDiag, pKerInitArgs);
      if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_svd_small_exec_cn<double>;
         status                = DSPLIB_svd_small_init_cn(handle, bufParamsA, bufParamsU, bufParamsV, bufParamsDiag,
                                                          bufParamsSuperDiag, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      //    status = DSPLIB_svd_small_init_ci<float>(handle, bufParamsA, bufParamsU, bufParamsV, bufParamsDiag,
      //                                             bufParamsSuperDiag, pKerInitArgs);
      if (bufParamsA->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_svd_small_exec_ci<double>;
         status = DSPLIB_svd_small_init_ci<double>(handle, bufParamsA, bufParamsU, bufParamsV, bufParamsDiag,
                                                   bufParamsSuperDiag, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

#else
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "The Kernel is NOT IMPLEMENTED for __C7X_VEC_SIZE_BITS__ != 512 ");
   status = DSPLIB_ERR_NOT_IMPLEMENTED;
#endif

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_svd_small_exec(DSPLIB_kernelHandle handle,
                                    void *restrict pA,
                                    void *restrict pU,
                                    void *restrict pV,
                                    void *restrict pDiag,
                                    void *restrict pSuperDiag,
                                    void *restrict pU1)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if (__C7X_VEC_SIZE_BITS__ == 512)

   DSPLIB_svd_small_PrivArgs *pKerPrivArgs = (DSPLIB_svd_small_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "widthIn: %d heightIn: %d strideIn: %d strideU: %d srideV: %d\n", pKerPrivArgs->widthIn,
                       pKerPrivArgs->heightIn, pKerPrivArgs->strideIn, pKerPrivArgs->strideU, pKerPrivArgs->strideV);

   status = pKerPrivArgs->execute(handle, pA, pU, pV, pDiag, pSuperDiag, pU1);

#else
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "The Kernel is NOT IMPLEMENTED for __C7X_VEC_SIZE_BITS__ != 512 ");
   status = DSPLIB_ERR_NOT_IMPLEMENTED;
#endif

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
