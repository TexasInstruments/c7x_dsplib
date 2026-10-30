// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_peakAbsoluteValue_priv.h"
#include <limits>

template <typename dataType>
DSPLIB_STATUS DSPLIB_peakAbsoluteValue_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_peakAbsoluteValue_PrivArgs *pKerPrivArgs = (DSPLIB_peakAbsoluteValue_PrivArgs *) handle;
   int32_t                            width        = pKerPrivArgs->width;
   int32_t                            height       = pKerPrivArgs->height;
   int32_t                            strideIn     = pKerPrivArgs->strideInElements;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_peakAbsoluteValue_exec_cn\n");
#endif

   dataType *pInLocal  = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal %p  pOut %p\n", pInLocal, pOut);
#endif

   /* One peak per row: the largest absolute value of the row is written to the output at the row index */
   for (int32_t row = 0; row < height; row++) {
      dataType out = 0;

      for (int32_t col = 0; col < width; col++) {
         dataType inValue = pInLocal[(size_t) row * strideIn + col];
         dataType a;

         /* Integers equal to the type minimum saturate to the type maximum, as the C7x VABS instructions do */
         if (inValue == std::numeric_limits<dataType>::lowest()) {
            a = std::numeric_limits<dataType>::max();
         }
         else {
            a = (inValue < 0) ? -inValue : inValue;
         }

         out = (a > out) ? a : out;
      }

      pOutLocal[row] = out;
   }
   return (status);
}

// explicit instantiation for the different data type versions

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
