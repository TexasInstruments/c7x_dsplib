// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_nonZeroCheck_priv.h"
#include <cstdint>

template <typename dataType>
DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_nonZeroCheck_all_row_exec_cn");

   dataType *restrict pIn0Local = (dataType *) pIn0;
   dataType *restrict pOutLocal = (dataType *) pOut;

   uint32_t dim_x    = pKerPrivArgs->dim_x;
   uint32_t dim_y    = pKerPrivArgs->dim_y;
   int32_t  strideIn = pKerPrivArgs->strideIn;

   for (size_t j = 0; j < dim_x; j++) {
      dataType value = (dataType) 1;
      for (size_t i = 0; i < dim_y; i++) {
         value = (dataType) ((value != (dataType) 0) && (pIn0Local[i * strideIn + j] != (dataType) 0));
      }
      pOutLocal[j] = value;
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_nonZeroCheck_all_column_exec_cn");

   dataType *restrict pIn0Local = (dataType *) pIn0;
   dataType *restrict pOutLocal = (dataType *) pOut;

   uint32_t dim_x    = pKerPrivArgs->dim_x;
   uint32_t dim_y    = pKerPrivArgs->dim_y;
   int32_t  strideIn = pKerPrivArgs->strideIn;

   for (int j = 0; j < dim_y; j++) {
      dataType value = (dataType) 1;
      for (int i = 0; i < dim_x; i++) {
         value = (dataType) ((value != (dataType) 0) && (pIn0Local[j * strideIn + i] != (dataType) 0));
      }
      pOutLocal[j] = value;
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_nonZeroCheck_any_row_exec_cn");

   dataType *restrict pIn0Local = (dataType *) pIn0;
   dataType *restrict pOutLocal = (dataType *) pOut;

   uint32_t dim_x    = pKerPrivArgs->dim_x;
   uint32_t dim_y    = pKerPrivArgs->dim_y;
   int32_t  strideIn = pKerPrivArgs->strideIn;

   for (int j = 0; j < dim_x; j++) {
      dataType value = (dataType) 0;
      for (int i = 0; i < dim_y; i++) {
         value = (dataType) ((value != (dataType) 0) || (pIn0Local[i * strideIn + j] != (dataType) 0));
      }
      pOutLocal[j] = value;
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_nonZeroCheck_any_column_exec_cn");

   dataType *restrict pIn0Local = (dataType *) pIn0;
   dataType *restrict pOutLocal = (dataType *) pOut;

   uint32_t dim_x    = pKerPrivArgs->dim_x;
   uint32_t dim_y    = pKerPrivArgs->dim_y;
   int32_t  strideIn = pKerPrivArgs->strideIn;

   for (int j = 0; j < dim_y; j++) {
      dataType value = (dataType) 0;
      for (int i = 0; i < dim_x; i++) {
         value = (dataType) ((value != (dataType) 0) || (pIn0Local[j * strideIn + i] != (dataType) 0));
      }
      pOutLocal[j] = value;
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);
