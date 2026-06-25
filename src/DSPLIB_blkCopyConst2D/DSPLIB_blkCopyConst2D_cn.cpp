// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blkCopyConst2D_priv.h"

DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_cn(DSPLIB_kernelHandle                  handle,
                                            DSPLIB_bufParams1D_t                *bufParamsIn1,
                                            DSPLIB_bufParams2D_t                *bufParamsOut,
                                            const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_blkCopyConst2D_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS                   status       = DSPLIB_SUCCESS;
   DSPLIB_blkCopyConst2D_PrivArgs *pKerPrivArgs = (DSPLIB_blkCopyConst2D_PrivArgs *) handle;

   dataType *restrict pInLocal  = (dataType *) pIn1;
   dataType *restrict pOutLocal = (dataType *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p\n", pInLocal, pOutLocal);

   // Get dimensions and strides from the private args
   uint32_t width     = pKerPrivArgs->width;
   uint32_t height    = pKerPrivArgs->height;
   uint32_t strideOut = pKerPrivArgs->strideOut;
   dataType constant  = *pInLocal;

   // Perform 2D copy operation
   for (uint32_t row = 0; row < height; row++) {
      for (uint32_t col = 0; col < width; col++) {
         pOutLocal[row * strideOut + col] = constant;
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

// Template instantiations for supported data types
template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);
