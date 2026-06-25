// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_deinterleave.h"
#include "../DSPLIB_matTrans/DSPLIB_matTrans_priv.h"
#include "DSPLIB_deinterleave_priv.h"

int32_t DSPLIB_deinterleave_getHandleSize(DSPLIB_deinterleaveInitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_deinterleave_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS DSPLIB_deinterleave_init_checkParams(DSPLIB_kernelHandle                handle,
                                                   const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                   const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                   const DSPLIB_deinterleaveInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_deinterleave_init_checkParams");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      /* Condition change */
      if ((bufParamsIn->dim_x == 1u) ||
          (bufParamsIn->dim_x == 2u &&
           (uint32_t) bufParamsIn->dim_x == (uint32_t) bufParamsIn->stride_y / DSPLIB_sizeof(bufParamsIn->data_type))) {
         /* Kernel support for 1 channel and 2 channel is added*/
      }
      else {
         if ((bufParamsIn->data_type != DSPLIB_INT64) && (bufParamsIn->data_type != DSPLIB_INT32) &&
             (bufParamsIn->data_type != DSPLIB_INT16) && (bufParamsIn->data_type != DSPLIB_INT8) &&
             (bufParamsIn->data_type != DSPLIB_UINT64) && (bufParamsIn->data_type != DSPLIB_UINT32) &&
             (bufParamsIn->data_type != DSPLIB_UINT16) && (bufParamsIn->data_type != DSPLIB_UINT8) &&
             (bufParamsIn->data_type != DSPLIB_FLOAT32) && (bufParamsIn->data_type != DSPLIB_FLOAT64)) {
            status = DSPLIB_ERR_INVALID_TYPE;
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
               /* Do nothing */
            }
         }
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting DSPLIB_deinterleave_init_checkParams with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS
DSPLIB_deinterleave_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_deinterleave_exec_checkParams");

   DSPLIB_STATUS status;

   if ((pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting DSPLIB_deinterleave_exec_checkParams with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_deinterleave_init(DSPLIB_kernelHandle                handle,
                                       DSPLIB_bufParams2D_t              *bufParamsIn,
                                       DSPLIB_bufParams2D_t              *bufParamsOut,
                                       const DSPLIB_deinterleaveInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_deinterleave_init");

   DSPLIB_STATUS                status                   = DSPLIB_SUCCESS;
   DSPLIB_matTrans_PrivArgs    *pKerPrivArgs             = (DSPLIB_matTrans_PrivArgs *) handle;
   DSPLIB_matTransInitArgs     *pKerInitArgsTrnsMat      = (DSPLIB_matTransInitArgs *) pKerInitArgs;
   DSPLIB_deinterleaveInitArgs *pKerInitArgsdeinterleave = (DSPLIB_deinterleaveInitArgs *) pKerInitArgs;

   pKerPrivArgs->widthIn   = bufParamsIn->dim_x;
   pKerPrivArgs->heightIn  = bufParamsIn->dim_y;
   pKerPrivArgs->strideIn  = bufParamsIn->stride_y;
   pKerPrivArgs->strideOut = bufParamsOut->stride_y;

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<float>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<double>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<int8_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<uint8_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<int16_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<uint16_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<int32_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<uint32_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<int64_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_matTrans_exec_cn<uint64_t>;
         status                = DSPLIB_matTrans_init_cn(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
      }

      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {

      if ((bufParamsIn->dim_x == 1u) ||
          (bufParamsIn->dim_x == 2u &&
           (uint32_t) bufParamsIn->dim_x == (uint32_t) bufParamsIn->stride_y / DSPLIB_sizeof(bufParamsIn->data_type))) {
         if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<float>;
            status = DSPLIB_deinterleave_init_ci<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<double>;
            status = DSPLIB_deinterleave_init_ci<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT32) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<int32_t>;
            status = DSPLIB_deinterleave_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT32) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<uint32_t>;
            status = DSPLIB_deinterleave_init_ci<uint32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT64) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<uint64_t>;
            status = DSPLIB_deinterleave_init_ci<uint64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT64) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<int64_t>;
            status = DSPLIB_deinterleave_init_ci<int64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT16) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<uint16_t>;
            status = DSPLIB_deinterleave_init_ci<uint16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT16) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<int16_t>;
            status = DSPLIB_deinterleave_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT8) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<uint8_t>;
            status = DSPLIB_deinterleave_init_ci<uint8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT8) {
            pKerPrivArgs->execute = DSPLIB_deinterleave_exec_ci<int8_t>;
            status = DSPLIB_deinterleave_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsdeinterleave);
         }
         else {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }

      else {
         if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<float>;
            status = DSPLIB_matTrans_init_ci<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<double>;
            status = DSPLIB_matTrans_init_ci<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT8) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<int8_t>;
            status = DSPLIB_matTrans_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT8) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<uint8_t>;
            status = DSPLIB_matTrans_init_ci<uint8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT16) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<int16_t>;
            status = DSPLIB_matTrans_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT16) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<uint16_t>;
            status = DSPLIB_matTrans_init_ci<uint16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT32) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<int32_t>;
            status = DSPLIB_matTrans_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT32) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<uint32_t>;
            status = DSPLIB_matTrans_init_ci<uint32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else if (bufParamsIn->data_type == DSPLIB_INT64) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<int64_t>;
            status = DSPLIB_matTrans_init_ci<int64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else if (bufParamsIn->data_type == DSPLIB_UINT64) {
            pKerPrivArgs->execute = DSPLIB_matTrans_exec_ci<uint64_t>;
            status = DSPLIB_matTrans_init_ci<uint64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgsTrnsMat);
         }
         else {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting DSPLIB_deinterleave_init with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_deinterleave_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_deinterleave_exec");

   DSPLIB_STATUS status;

   DSPLIB_matTrans_PrivArgs *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting DSPLIB_deinterleave_exec with return status: %d\n", status);

   return status;
}
