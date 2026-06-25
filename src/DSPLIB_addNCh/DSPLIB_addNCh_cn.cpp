// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_addNCh_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_addNCh_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_addNCh_PrivArgs *pKerPrivArgs = (DSPLIB_addNCh_PrivArgs *) handle;

   uint32_t samples           = pKerPrivArgs->samples;
   uint32_t channels          = pKerPrivArgs->channels;
   uint32_t inputs            = pKerPrivArgs->inputs;
   int32_t  strideElementsY   = pKerPrivArgs->strideInElementsY;
   int32_t  strideElementsZ   = pKerPrivArgs->strideInElementsZ;
   int32_t  strideElementsOut = pKerPrivArgs->strideOutElements;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_addNCh_exec_cn\n", status);

   dataType *pInLocal  = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

   dataType temp = 0

       DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_addNCh_exec_cn pInLocal %p , pOut %p\n", pInLocal, pOut);

   for (size_t row = 0; row < channels; row++) {
      for (size_t col = 0; col < samples; col++) {
         temp = 0;
         for (size_t layer = 0; layer < inputs; layer++) {
            temp += pInLocal[(row * strideElementsY) + col + (strideElementsZ * layer)];
         }
         pOutLocal[(row * strideElementsOut) + col] = temp;
      }
   }

   return (status);
}

template DSPLIB_STATUS
DSPLIB_addNCh_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_addNCh_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
