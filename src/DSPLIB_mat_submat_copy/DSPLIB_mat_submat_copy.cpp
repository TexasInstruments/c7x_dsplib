// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_mat_submat_copy_priv.h"

int32_t DSPLIB_mat_submat_copy_getHandleSize(DSPLIB_mat_submat_copyInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t privBufSize = sizeof(DSPLIB_mat_submat_copy_PrivArgs);
   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_mat_submat_copy_init_checkParams(DSPLIB_kernelHandle                   handle,
                                                      const DSPLIB_bufParams2D_t           *bufParamsX,
                                                      const DSPLIB_bufParams2D_t           *bufParamsY,
                                                      const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {

      /* Condition change */
      if ((bufParamsX->data_type != DSPLIB_INT64) && (bufParamsX->data_type != DSPLIB_INT32) &&
          (bufParamsX->data_type != DSPLIB_INT16) && (bufParamsX->data_type != DSPLIB_INT8) &&
          (bufParamsX->data_type != DSPLIB_UINT64) && (bufParamsX->data_type != DSPLIB_UINT32) &&
          (bufParamsX->data_type != DSPLIB_UINT16) && (bufParamsX->data_type != DSPLIB_UINT8) &&
          (bufParamsX->data_type != DSPLIB_FLOAT32) && (bufParamsX->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsX->data_type != bufParamsY->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Do nothing */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pX, const void *restrict pY)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   if ((pX == NULL) || (pY == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_mat_submat_copy_init(DSPLIB_kernelHandle                   handle,
                                          DSPLIB_bufParams2D_t                 *bufParamsX,
                                          DSPLIB_bufParams2D_t                 *bufParamsY,
                                          const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS                    status       = DSPLIB_SUCCESS;
   DSPLIB_mat_submat_copy_PrivArgs *pKerPrivArgs = (DSPLIB_mat_submat_copy_PrivArgs *) handle;

   pKerPrivArgs->widthX  = bufParamsX->dim_x;
   pKerPrivArgs->heightX = bufParamsX->dim_y;
   pKerPrivArgs->strideX = bufParamsX->stride_y;
   pKerPrivArgs->strideY = bufParamsY->stride_y;
   pKerPrivArgs->dir     = pKerInitArgs->dir;
   pKerPrivArgs->stRow   = pKerInitArgs->stRow;
   pKerPrivArgs->stCol   = pKerInitArgs->stCol;
   pKerPrivArgs->nRows   = pKerInitArgs->nRows;
   pKerPrivArgs->nCols   = pKerInitArgs->nCols;

   DSPLIB_DEBUGPRINTFN(0, "pKerInitArgs->funcStyle: %d bufParamsY->data_type: %d\n", pKerInitArgs->funcStyle,
                       bufParamsY->data_type);
   if ((pKerPrivArgs->stCol + pKerPrivArgs->nCols) > pKerPrivArgs->widthX ||
       (pKerPrivArgs->stRow + pKerPrivArgs->nRows) > pKerPrivArgs->heightX) {
      status = DSPLIB_ERR_INVALID_DIMENSION;
   }
   else {
      if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
         if (bufParamsX->data_type == DSPLIB_FLOAT32) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<float>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_FLOAT64) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<double>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_INT8) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<int8_t>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_UINT8) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<uint8_t>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_INT16) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<int16_t>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_UINT16) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<uint16_t>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_INT32) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<int32_t>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_UINT32) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<uint32_t>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_INT64) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<int64_t>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_UINT64) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_cn<uint64_t>;
            status                = DSPLIB_mat_submat_copy_init_cn(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }

         else {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
      else {
         if (bufParamsX->data_type == DSPLIB_FLOAT32) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<float>;
            status                = DSPLIB_mat_submat_copy_init_ci<float>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_FLOAT64) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<double>;
            status = DSPLIB_mat_submat_copy_init_ci<double>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_INT8) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<int8_t>;
            status = DSPLIB_mat_submat_copy_init_ci<int8_t>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_UINT8) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<uint8_t>;
            status = DSPLIB_mat_submat_copy_init_ci<uint8_t>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_INT16) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<int16_t>;
            status = DSPLIB_mat_submat_copy_init_ci<int16_t>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_UINT16) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<uint16_t>;
            status = DSPLIB_mat_submat_copy_init_ci<uint16_t>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_INT32) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<int32_t>;
            status = DSPLIB_mat_submat_copy_init_ci<int32_t>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_UINT32) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<uint32_t>;
            status = DSPLIB_mat_submat_copy_init_ci<uint32_t>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_INT64) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<int64_t>;
            status = DSPLIB_mat_submat_copy_init_ci<int64_t>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else if (bufParamsX->data_type == DSPLIB_UINT64) {
            pKerPrivArgs->execute = DSPLIB_mat_submat_copy_exec_ci<uint64_t>;
            status = DSPLIB_mat_submat_copy_init_ci<uint64_t>(handle, bufParamsX, bufParamsY, pKerInitArgs);
         }
         else {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_mat_submat_copy_exec(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS                    status;
   DSPLIB_mat_submat_copy_PrivArgs *pKerPrivArgs = (DSPLIB_mat_submat_copy_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "widthX: %d heightX: %d strideX: %d strideY: %d\n", pKerPrivArgs->widthX,
                       pKerPrivArgs->heightX, pKerPrivArgs->strideX, pKerPrivArgs->strideY);
   status = pKerPrivArgs->execute(handle, pX, pY);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
