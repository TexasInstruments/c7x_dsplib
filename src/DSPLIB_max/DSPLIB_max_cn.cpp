// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_max_priv.h"

template <typename dataType, int32_t dataIn>
DSPLIB_STATUS DSPLIB_max_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_max_PrivArgs *pKerPrivArgs = (DSPLIB_max_PrivArgs *) handle;
   int32_t              blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_max_exec_cn\n");
#endif

   dataType *pInLocal  = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal %p  pOut %p\n", pInLocal, pOut);
#endif

   dataType a, out;

   out = pInLocal[0];

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a);
#endif

      out = (a > out) ? a : out;

#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d y %d\n", counter, out);
#endif
   }

   *pOutLocal = out;
   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_max_exec_cn<float, DSPLIB_FLOAT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_max_exec_cn<double, DSPLIB_FLOAT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_max_exec_cn<int8_t, DSPLIB_INT8>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_max_exec_cn<uint8_t, DSPLIB_UINT8>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_max_exec_cn<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_max_exec_cn<uint16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_max_exec_cn<int32_t, DSPLIB_INT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_max_exec_cn<uint32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
