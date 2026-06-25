// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blkCopy2D_priv.h"

int32_t DSPLIB_blkCopy2D_getHandleSize(DSPLIB_blkCopy2DInitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_blkCopy2D_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS DSPLIB_blkCopy2D_init_checkParams(DSPLIB_kernelHandle             handle,
                                                const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                const DSPLIB_blkCopy2DInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (handle == nullptr || bufParamsIn == nullptr || bufParamsOut == nullptr || pKerInitArgs == nullptr) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn->data_type != DSPLIB_INT8) && (bufParamsIn->data_type != DSPLIB_INT16) &&
          (bufParamsIn->data_type != DSPLIB_INT32) && (bufParamsIn->data_type != DSPLIB_INT64) &&
          (bufParamsIn->data_type != DSPLIB_UINT8) && (bufParamsIn->data_type != DSPLIB_UINT16) &&
          (bufParamsIn->data_type != DSPLIB_UINT32) && (bufParamsIn->data_type != DSPLIB_UINT64) &&
          (bufParamsIn->data_type != DSPLIB_FLOAT32) && (bufParamsIn->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->dim_x != bufParamsOut->dim_x) {
         // Check if width (dim_x) matches
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else if (bufParamsIn->dim_y != bufParamsOut->dim_y) {
         // Check if height (dim_y) matches
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else {
         /* Nothing to do here */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   if ((handle == NULL) || (pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_blkCopy2D_init(DSPLIB_kernelHandle             handle,
                                    DSPLIB_bufParams2D_t           *bufParamsIn,
                                    DSPLIB_bufParams2D_t           *bufParamsOut,
                                    const DSPLIB_blkCopy2DInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS              status       = DSPLIB_SUCCESS;
   DSPLIB_blkCopy2D_PrivArgs *pKerPrivArgs = (DSPLIB_blkCopy2D_PrivArgs *) handle;
   int32_t                    dataWidth;

   if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
      dataWidth = sizeof(DSPLIB_F32);
   }
   else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
      dataWidth = sizeof(DSPLIB_D64);
   }
   else {
      dataWidth = DSPLIB_sizeof(bufParamsIn->data_type);
   }
   // Store the dimensions and strides from buffer parameters
   pKerPrivArgs->width     = bufParamsIn->dim_x;
   pKerPrivArgs->height    = bufParamsIn->dim_y;
   pKerPrivArgs->strideIn  = bufParamsIn->stride_y / dataWidth;
   pKerPrivArgs->strideOut = bufParamsOut->stride_y / dataWidth;

   DSPLIB_DEBUGPRINTFN(0, "pKerInitArgs->funcStyle: %d bufParamsOut->data_type: %d\n", pKerInitArgs->funcStyle,
                       bufParamsOut->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<float>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<double>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<int8_t>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<int16_t>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<int32_t>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<int64_t>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<uint8_t>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<uint16_t>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<uint32_t>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_cn<uint64_t>;
         status                = DSPLIB_blkCopy2D_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<float>;
         status                = DSPLIB_blkCopy2D_init_ci<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<double>;
         status                = DSPLIB_blkCopy2D_init_ci<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<int8_t>;
         status                = DSPLIB_blkCopy2D_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<int16_t>;
         status                = DSPLIB_blkCopy2D_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<int32_t>;
         status                = DSPLIB_blkCopy2D_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<int64_t>;
         status                = DSPLIB_blkCopy2D_init_ci<int64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<uint8_t>;
         status                = DSPLIB_blkCopy2D_init_ci<uint8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<uint16_t>;
         status                = DSPLIB_blkCopy2D_init_ci<uint16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<uint32_t>;
         status                = DSPLIB_blkCopy2D_init_ci<uint32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_blkCopy2D_exec_ci<uint64_t>;
         status                = DSPLIB_blkCopy2D_init_ci<uint64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_blkCopy2D_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   DSPLIB_blkCopy2D_PrivArgs *pKerPrivArgs = (DSPLIB_blkCopy2D_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
