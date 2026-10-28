// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_absoluteValue_priv.h"
#include <limits>
template <typename dataType>
DSPLIB_STATUS DSPLIB_absoluteValue_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS                  status       = DSPLIB_SUCCESS;
   DSPLIB_absoluteValue_PrivArgs *pKerPrivArgs = (DSPLIB_absoluteValue_PrivArgs *) handle;

   dataType *pInLocal  = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

   // Get dimensions and strides from the private args
   uint32_t width     = pKerPrivArgs->width;
   uint32_t height    = pKerPrivArgs->height;
   uint32_t strideIn  = pKerPrivArgs->strideIn;
   uint32_t strideOut = pKerPrivArgs->strideOut;

   // Perform 2D absolute value operation
   for (uint32_t row = 0; row < height; row++) {
      for (uint32_t col = 0; col < width; col++) {
         // Calculate source and destination indices using strides in elements
         uint32_t srcIdx = row * strideIn + col;
         uint32_t dstIdx = row * strideOut + col;
         // Apply ABS and store result. Integers equal to the type minimum saturate to the type maximum, as the C7x
         // VABS instructions do (-(minimum) would overflow).
         dataType inValue  = pInLocal[srcIdx];
         dataType absValue = (inValue < 0) ? -inValue : inValue;
         pOutLocal[dstIdx] =
             (inValue == std::numeric_limits<dataType>::lowest()) ? std::numeric_limits<dataType>::max() : absValue;
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

// Template instantiations for supported data types

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
