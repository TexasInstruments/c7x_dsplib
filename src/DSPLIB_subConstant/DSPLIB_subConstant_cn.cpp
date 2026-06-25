// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_subConstant_priv.h"

template <typename dataType, int32_t dataIn>
DSPLIB_STATUS
DSPLIB_subConstant_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_subConstant_PrivArgs *pKerPrivArgs = (DSPLIB_subConstant_PrivArgs *) handle;
   uint32_t                     blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_subConstant_exec_cn\n");
#endif

   dataType *pInLocal1 = (dataType *) pIn1;
   dataType *pInLocal2 = (dataType *) pIn2;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   dataType a, b, out;
   a = *pInLocal1;

   for (int32_t counter = 0; counter < blockSize; counter++) {

      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out = a - b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d y %d\n", counter, out);
#endif

      *pOutLocal++ = out;
   }
   return (status);
}

// explicit inssubConstanttiation for the different data type versions
template DSPLIB_STATUS DSPLIB_subConstant_exec_cn<float, DSPLIB_FLOAT32>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);

template DSPLIB_STATUS DSPLIB_subConstant_exec_cn<double, DSPLIB_FLOAT64>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn1,
                                                                          void *restrict pIn2,
                                                                          void *restrict pOut);
template DSPLIB_STATUS DSPLIB_subConstant_exec_cn<int8_t, DSPLIB_INT8>(DSPLIB_kernelHandle handle,
                                                                       void *restrict pIn1,
                                                                       void *restrict pIn2,
                                                                       void *restrict pOut);
template DSPLIB_STATUS DSPLIB_subConstant_exec_cn<uint8_t, DSPLIB_UINT8>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_subConstant_exec_cn<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_subConstant_exec_cn<uint16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn1,
                                                                           void *restrict pIn2,
                                                                           void *restrict pOut);
template DSPLIB_STATUS DSPLIB_subConstant_exec_cn<int32_t, DSPLIB_INT32>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_subConstant_exec_cn<uint32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn1,
                                                                           void *restrict pIn2,
                                                                           void *restrict pOut);
