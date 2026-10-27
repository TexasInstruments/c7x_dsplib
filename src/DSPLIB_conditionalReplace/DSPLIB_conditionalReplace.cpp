// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bufParams.h"
#include "DSPLIB_conditionalReplace_priv.h"

int32_t DSPLIB_conditionalReplace_getHandleSize(DSPLIB_conditionalReplace_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_conditionalReplace_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_conditionalReplace_init_checkParams(DSPLIB_kernelHandle                       handle,
                                           const DSPLIB_bufParams2D_t               *bufParamsIn,
                                           const DSPLIB_bufParams2D_t               *bufParamsOut,
                                           const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_conditionalReplace_init_checkParams \n");

   if ((handle == NULL) || (bufParamsIn == NULL) || (bufParamsOut == NULL) || (pKerInitArgs == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn->data_type != DSPLIB_INT8) && (bufParamsIn->data_type != DSPLIB_UINT8) &&
          (bufParamsIn->data_type != DSPLIB_INT16) && (bufParamsIn->data_type != DSPLIB_UINT16) &&
          (bufParamsIn->data_type != DSPLIB_INT32) && (bufParamsIn->data_type != DSPLIB_UINT32) &&
          (bufParamsIn->data_type != DSPLIB_INT64) && (bufParamsIn->data_type != DSPLIB_UINT64) &&
          (bufParamsIn->data_type != DSPLIB_FLOAT32) && (bufParamsIn->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn->dim_x != bufParamsOut->dim_x) || (bufParamsIn->dim_y != bufParamsOut->dim_y)) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_conditionalReplace_exec_checkParams(DSPLIB_kernelHandle handle,
                                                         const void *restrict pIn,
                                                         const void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_conditionalReplace_exec_checkParams \n");

   if ((handle == NULL) || (pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_conditionalReplace_init(DSPLIB_kernelHandle                       handle,
                                             DSPLIB_bufParams2D_t                     *bufParamsIn,
                                             DSPLIB_bufParams2D_t                     *bufParamsOut,
                                             const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                       status       = DSPLIB_SUCCESS;
   DSPLIB_conditionalReplace_PrivArgs *pKerPrivArgs = (DSPLIB_conditionalReplace_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_conditionalReplace_init \n");

   pKerPrivArgs->dim_x                 = bufParamsIn->dim_x;
   pKerPrivArgs->dim_y                 = bufParamsIn->dim_y;
   pKerPrivArgs->strideIn              = bufParamsIn->stride_y / DSPLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideOut             = bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsOut->data_type);
   pKerPrivArgs->setArgs.pCompareType  = NULL;
   pKerPrivArgs->setArgs.pConstValue   = NULL;
   pKerPrivArgs->setArgs.pReplaceValue = NULL;

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<int8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<uint8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<int16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<uint16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<int32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<uint32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<int64_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<uint64_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<float>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_cn<double>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_INT8) {
         status = DSPLIB_conditionalReplace_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<int8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         status = DSPLIB_conditionalReplace_init_ci<uint8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<uint8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         status = DSPLIB_conditionalReplace_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<int16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         status = DSPLIB_conditionalReplace_init_ci<uint16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<uint16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         status = DSPLIB_conditionalReplace_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<int32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         status = DSPLIB_conditionalReplace_init_ci<uint32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<uint32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         status = DSPLIB_conditionalReplace_init_ci<int64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<int64_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         status = DSPLIB_conditionalReplace_init_ci<uint64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<uint64_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         status = DSPLIB_conditionalReplace_init_ci<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<float>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         status = DSPLIB_conditionalReplace_init_ci<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_conditionalReplace_exec_ci<double>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_conditionalReplace_set(DSPLIB_kernelHandle                      handle,
                                            const DSPLIB_conditionalReplace_SetArgs *pKerSetArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_conditionalReplace_set \n");

   if ((handle == NULL) || (pKerSetArgs == NULL) || (pKerSetArgs->pCompareType == NULL) ||
       (pKerSetArgs->pConstValue == NULL) || (pKerSetArgs->pReplaceValue == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      DSPLIB_conditionalReplace_PrivArgs *pKerPrivArgs = (DSPLIB_conditionalReplace_PrivArgs *) handle;

      /* every channel needs a valid compare type */
      for (uint32_t ch = 0; (ch < pKerPrivArgs->dim_y) && (status == DSPLIB_SUCCESS); ch++) {
         if ((uint32_t) pKerSetArgs->pCompareType[ch] > (uint32_t) DSPLIB_CONDITIONALREPLACE_GE) {
            status = DSPLIB_ERR_INVALID_VALUE;
         }
      }

      if (status == DSPLIB_SUCCESS) {
         pKerPrivArgs->setArgs = *pKerSetArgs;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_conditionalReplace_get(DSPLIB_kernelHandle handle, DSPLIB_conditionalReplace_SetArgs *pKerSetArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if ((handle == NULL) || (pKerSetArgs == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      DSPLIB_conditionalReplace_PrivArgs *pKerPrivArgs = (DSPLIB_conditionalReplace_PrivArgs *) handle;
      *pKerSetArgs                                     = pKerPrivArgs->setArgs;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_conditionalReplace_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_conditionalReplace_exec \n");

   DSPLIB_conditionalReplace_PrivArgs *pKerPrivArgs = (DSPLIB_conditionalReplace_PrivArgs *) handle;

   if ((pKerPrivArgs->setArgs.pCompareType == NULL) || (pKerPrivArgs->setArgs.pConstValue == NULL) ||
       (pKerPrivArgs->setArgs.pReplaceValue == NULL)) {
      /* DSPLIB_conditionalReplace_set has not been called */
      status = DSPLIB_ERR_FAILURE;
   }
   else {
      status = pKerPrivArgs->execute(handle, pIn, pOut);
   }

   return status;
}
