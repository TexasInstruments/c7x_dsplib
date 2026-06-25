// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_negate_priv.h"
#include <limits>

template <typename dataType, int32_t dataIn>
DSPLIB_STATUS DSPLIB_negate_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_negate_PrivArgs *pKerPrivArgs = (DSPLIB_negate_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_negate_exec_cn\n");
#endif

   dataType *pInLocal  = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal %p pOut %p\n", pInLocal, pOut);
#endif

   dataType a;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %f\n", counter, a);
#endif

      // if input == min, output = max
      if (a == std::numeric_limits<dataType>::lowest()) {
         (*pOutLocal++) = std::numeric_limits<dataType>::max();
      }
      else {
         (*pOutLocal++) = -a;
      }
   }

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_negate_exec_cn<int32_t, DSPLIB_INT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_negate_exec_cn<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_negate_exec_cn<int8_t, DSPLIB_INT8>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
