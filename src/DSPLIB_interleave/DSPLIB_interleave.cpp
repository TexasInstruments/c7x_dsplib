// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_interleave.h"
#include "../DSPLIB_matTrans/DSPLIB_matTrans_priv.h"
#include "DSPLIB_Interleave_priv.h"

int32_t DSPLIB_interleave_getHandleSize(DSPLIB_interleaveInitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_interleave_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS DSPLIB_interleave_init_checkParams(DSPLIB_kernelHandle              handle,
                                                 const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                 const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                 const DSPLIB_interleaveInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_interleave_init_checkParams");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {

      uint32_t strideElements = bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsOut->data_type);

      /* Condition change */

      if ((bufParamsIn->data_type != (uint32_t) DSPLIB_INT32) && (bufParamsIn->data_type != (uint32_t) DSPLIB_INT16) &&
          (bufParamsIn->data_type != (uint32_t) DSPLIB_UINT32) &&
          (bufParamsIn->data_type != (uint32_t) DSPLIB_UINT16) &&
          (bufParamsIn->data_type != (uint32_t) DSPLIB_FLOAT32) &&
          (bufParamsIn->data_type != (uint32_t) DSPLIB_INT64) && (bufParamsIn->data_type != (uint32_t) DSPLIB_INT8) &&
          (bufParamsIn->data_type != (uint32_t) DSPLIB_UINT64) && (bufParamsIn->data_type != (uint32_t) DSPLIB_UINT8) &&
          (bufParamsIn->data_type != (uint32_t) DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      if (((bufParamsIn->dim_y == 1U) || (bufParamsIn->dim_y == 2U)) && (strideElements == bufParamsOut->dim_x)) {
         /* Do nothing */
      }
      else {
         if (bufParamsIn->data_type != bufParamsOut->data_type) {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
         else if ((bufParamsIn->data_type == DSPLIB_UINT16 || bufParamsIn->data_type == DSPLIB_INT16) &&
                  (bufParamsIn->dim_x % 2 != 0)) {
            status = DSPLIB_ERR_INVALID_VALUE;
         }
         else if ((bufParamsIn->data_type == DSPLIB_UINT8 || bufParamsIn->data_type == DSPLIB_INT8) &&
                  (bufParamsIn->dim_x % 4 != 0)) {
            status = DSPLIB_ERR_INVALID_VALUE;
         }
         else {
            status = DSPLIB_SUCCESS;
         }
      }

      DSPLIB_DEBUGPRINTFN(0, "Exiting DSPLIB_interleave_init_checkParams with return status: %d\n", status);
   }
   return status;
}

DSPLIB_STATUS
DSPLIB_interleave_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_interleave_exec_checkParams");

   DSPLIB_STATUS status;

   if ((pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting DSPLIB_interleave_exec_checkParams with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_interleave_init(DSPLIB_kernelHandle              handle,
                                     DSPLIB_bufParams2D_t            *bufParamsIn,
                                     DSPLIB_bufParams2D_t            *bufParamsOut,
                                     const DSPLIB_interleaveInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_interleave_init");

   DSPLIB_STATUS               status                 = DSPLIB_SUCCESS;
   DSPLIB_matTrans_PrivArgs   *pKerPrivArgs           = (DSPLIB_matTrans_PrivArgs *) handle;
   DSPLIB_interleave_PrivArgs *pKerPrivArgsinterleave = (DSPLIB_interleave_PrivArgs *) handle;

   DSPLIB_matTransInitArgs   *matTransInitArgs   = (DSPLIB_matTransInitArgs *) pKerInitArgs;
   DSPLIB_interleaveInitArgs *interleaveInitArgs = (DSPLIB_interleaveInitArgs *) pKerInitArgs;

   pKerPrivArgs->widthIn            = bufParamsIn->dim_x;
   pKerPrivArgs->heightIn           = bufParamsIn->dim_y;
   pKerPrivArgs->strideIn           = bufParamsIn->stride_y;
   pKerPrivArgs->strideOut          = bufParamsOut->stride_y;
   pKerPrivArgsinterleave->dataType = bufParamsIn->data_type;

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<float>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<double>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<int8_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<uint8_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<int16_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<uint16_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<int32_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<uint32_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<int64_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<uint64_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {

      uint32_t strideElements = (uint32_t) (bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsOut->data_type));

      if (((bufParamsIn->dim_y == 1u) || (bufParamsIn->dim_y == 2u)) && (strideElements == bufParamsOut->dim_x)) {
         if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci_fp<float>;
            status = DSPLIB_interleave_init_ci<float>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci_fp<double>;
            status = DSPLIB_interleave_init_ci<double>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT32) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci<int32_t>;
            status = DSPLIB_interleave_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT32) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci<uint32_t>;
            status = DSPLIB_interleave_init_ci<uint32_t>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT64) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci<uint64_t>;
            status = DSPLIB_interleave_init_ci<uint64_t>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT64) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci<int64_t>;
            status = DSPLIB_interleave_init_ci<int64_t>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT16) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci<uint16_t>;
            status = DSPLIB_interleave_init_ci<uint16_t>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT16) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci<int16_t>;
            status = DSPLIB_interleave_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT8) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci<uint8_t>;
            status = DSPLIB_interleave_init_ci<uint8_t>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT8) {
            pKerPrivArgs->execute = DSPLIB_interleave_exec_ci<int8_t>;
            status = DSPLIB_interleave_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, interleaveInitArgs);
         }
         else {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
      else {
         if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<float>;
            status                = DSPLIB_matTrans_init_ci<float>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<double>;
            status = DSPLIB_matTrans_init_ci<double>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT8) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<int8_t>;
            status = DSPLIB_matTrans_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT8) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<uint8_t>;
            status = DSPLIB_matTrans_init_ci<uint8_t>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT16) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<int16_t>;
            status = DSPLIB_matTrans_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT16) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<uint16_t>;
            status = DSPLIB_matTrans_init_ci<uint16_t>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }

         else if (bufParamsIn->data_type == DSPLIB_INT32) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<int32_t>;
            status = DSPLIB_matTrans_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT32) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<uint32_t>;
            status = DSPLIB_matTrans_init_ci<uint32_t>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT64) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<int64_t>;
            status = DSPLIB_matTrans_init_ci<int64_t>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT64) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<uint64_t>;
            status = DSPLIB_matTrans_init_ci<uint64_t>(handle, bufParamsIn, bufParamsOut, matTransInitArgs);
         }
         else {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting DSPLIB_interleave_init with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_interleave_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_interleave_exec");

   DSPLIB_STATUS status;

   DSPLIB_matTrans_PrivArgs *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting DSPLIB_interleave_exec with return status: %d\n", status);

   return status;
}
