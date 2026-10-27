// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bufParams.h"
#include "DSPLIB_logicalNOT_priv.h"

int32_t DSPLIB_logicalNOT_getHandleSize(DSPLIB_logicalNOT_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_logicalNOT_PrivArgs);
   return privBufSize;
}

static inline bool DSPLIB_logicalNOT_isSupportedType(int32_t dataType)
{
   return (dataType == DSPLIB_UINT8) || (dataType == DSPLIB_INT8) || (dataType == DSPLIB_UINT16) ||
          (dataType == DSPLIB_INT16) || (dataType == DSPLIB_UINT32) || (dataType == DSPLIB_INT32) ||
          (dataType == DSPLIB_UINT64) || (dataType == DSPLIB_INT64) || (dataType == DSPLIB_FLOAT32) ||
          (dataType == DSPLIB_FLOAT64);
}

DSPLIB_STATUS
DSPLIB_logicalNOT_init_checkParams(DSPLIB_kernelHandle               handle,
                                   const DSPLIB_bufParams2D_t       *bufParamsIn,
                                   const DSPLIB_bufParams2D_t       *bufParamsOut,
                                   const DSPLIB_logicalNOT_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_logicalNOT_init_checkParams \n");

   if ((handle == NULL) || (bufParamsIn == NULL) || (bufParamsOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      // Input must be a supported type, and output has the same type as input
      if (!DSPLIB_logicalNOT_isSupportedType(bufParamsIn->data_type) ||
          (bufParamsIn->data_type != bufParamsOut->data_type)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   if (status == DSPLIB_SUCCESS) {
      // Each row must fit in its stride, otherwise rows overlap and exec indexes past the intended buffer
      uint32_t rowBytes = bufParamsIn->dim_x * DSPLIB_sizeof(bufParamsIn->data_type);
      if ((bufParamsIn->stride_y < rowBytes) || (bufParamsOut->stride_y < rowBytes)) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_logicalNOT_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_logicalNOT_exec_checkParams \n");

   if ((handle == NULL) || (pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

/* Binds the natural C (cn) variant. dataType is the element type of the buffers. */
template <typename dataType>
static inline DSPLIB_STATUS DSPLIB_logicalNOT_init_cn_dispatch(DSPLIB_logicalNOT_PrivArgs *pKerPrivArgs)
{
   pKerPrivArgs->execute = DSPLIB_logicalNOT_exec_cn<dataType>;
   return DSPLIB_SUCCESS;
}

/* Binds the optimized (ci) variant.
 * Signed integers are processed by the unsigned instantiation of the same width, since the
 * zero test is identical for both. Floating point types have their own instantiation because
 * -0.0 has a non-zero bit pattern. */
template <typename dataType>
static inline DSPLIB_STATUS DSPLIB_logicalNOT_init_ci_dispatch(DSPLIB_kernelHandle               handle,
                                                               const DSPLIB_bufParams2D_t       *bufParamsIn,
                                                               const DSPLIB_bufParams2D_t       *bufParamsOut,
                                                               const DSPLIB_logicalNOT_InitArgs *pKerInitArgs)
{
   DSPLIB_logicalNOT_PrivArgs *pKerPrivArgs = (DSPLIB_logicalNOT_PrivArgs *) handle;
   DSPLIB_STATUS status  = DSPLIB_logicalNOT_init_ci<dataType>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
   pKerPrivArgs->execute = DSPLIB_logicalNOT_exec_ci<dataType>;
   return status;
}

DSPLIB_STATUS DSPLIB_logicalNOT_init(DSPLIB_kernelHandle               handle,
                                     DSPLIB_bufParams2D_t             *bufParamsIn,
                                     DSPLIB_bufParams2D_t             *bufParamsOut,
                                     const DSPLIB_logicalNOT_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS               status       = DSPLIB_SUCCESS;
   DSPLIB_logicalNOT_PrivArgs *pKerPrivArgs = (DSPLIB_logicalNOT_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_logicalNOT_init \n");

   pKerPrivArgs->dataType  = bufParamsIn->data_type;
   pKerPrivArgs->dim_x     = bufParamsIn->dim_x;
   pKerPrivArgs->dim_y     = bufParamsIn->dim_y;
   pKerPrivArgs->strideIn  = bufParamsIn->stride_y / DSPLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideOut = bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsOut->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      switch (bufParamsIn->data_type) {
      case DSPLIB_UINT8:
         status = DSPLIB_logicalNOT_init_cn_dispatch<uint8_t>(pKerPrivArgs);
         break;
      case DSPLIB_INT8:
         status = DSPLIB_logicalNOT_init_cn_dispatch<int8_t>(pKerPrivArgs);
         break;
      case DSPLIB_UINT16:
         status = DSPLIB_logicalNOT_init_cn_dispatch<uint16_t>(pKerPrivArgs);
         break;
      case DSPLIB_INT16:
         status = DSPLIB_logicalNOT_init_cn_dispatch<int16_t>(pKerPrivArgs);
         break;
      case DSPLIB_UINT32:
         status = DSPLIB_logicalNOT_init_cn_dispatch<uint32_t>(pKerPrivArgs);
         break;
      case DSPLIB_INT32:
         status = DSPLIB_logicalNOT_init_cn_dispatch<int32_t>(pKerPrivArgs);
         break;
      case DSPLIB_UINT64:
         status = DSPLIB_logicalNOT_init_cn_dispatch<uint64_t>(pKerPrivArgs);
         break;
      case DSPLIB_INT64:
         status = DSPLIB_logicalNOT_init_cn_dispatch<int64_t>(pKerPrivArgs);
         break;
      case DSPLIB_FLOAT32:
         status = DSPLIB_logicalNOT_init_cn_dispatch<float>(pKerPrivArgs);
         break;
      case DSPLIB_FLOAT64:
         status = DSPLIB_logicalNOT_init_cn_dispatch<double>(pKerPrivArgs);
         break;
      default:
         status = DSPLIB_ERR_INVALID_TYPE;
         break;
      }
   }
   else {
      switch (bufParamsIn->data_type) {
      case DSPLIB_UINT8:
      case DSPLIB_INT8:
         status = DSPLIB_logicalNOT_init_ci_dispatch<uint8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         break;
      case DSPLIB_UINT16:
      case DSPLIB_INT16:
         status = DSPLIB_logicalNOT_init_ci_dispatch<uint16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         break;
      case DSPLIB_UINT32:
      case DSPLIB_INT32:
         status = DSPLIB_logicalNOT_init_ci_dispatch<uint32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         break;
      case DSPLIB_UINT64:
      case DSPLIB_INT64:
         status = DSPLIB_logicalNOT_init_ci_dispatch<uint64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         break;
      case DSPLIB_FLOAT32:
         status = DSPLIB_logicalNOT_init_ci_dispatch<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         break;
      case DSPLIB_FLOAT64:
         status = DSPLIB_logicalNOT_init_ci_dispatch<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         break;
      default:
         status = DSPLIB_ERR_INVALID_TYPE;
         break;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_logicalNOT_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_logicalNOT_exec \n");

   DSPLIB_logicalNOT_PrivArgs *pKerPrivArgs = (DSPLIB_logicalNOT_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut);

   return status;
}
