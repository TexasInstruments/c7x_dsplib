// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_diag_priv.h"

template <typename dataType, int32_t dataIn>
DSPLIB_STATUS
DSPLIB_diag_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_diag_PrivArgs *pKerPrivArgs = (DSPLIB_diag_PrivArgs *) handle;
   uint32_t order = pKerPrivArgs->order;
   uint32_t advance = order + 1;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_diag_exec_cn\n");
#endif

   dataType *pInLocal = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal %p pOut %p\n", pInLocal, pOut);
#endif

   dataType *ptr = pInLocal;

   for (int32_t counter = 0; counter < (int32_t)order; counter++) {
      *pOutLocal++ = *ptr;
      ptr += advance;
   }
   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_diag_exec_cn<float, DSPLIB_FLOAT32>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<double, DSPLIB_FLOAT64>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<int8_t, DSPLIB_INT8>(DSPLIB_kernelHandle handle,
                                                                       void *restrict pIn,
                                                                       void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<uint8_t, DSPLIB_UINT8>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<uint16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn,
                                                                           void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<int32_t, DSPLIB_INT32>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<uint32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn,
                                                                           void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<cfloat, DSPLIB_CFLOAT32>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn,
                                                                           void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<cshort, DSPLIB_CINT16>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn,
                                                                           void *restrict pOut);
template DSPLIB_STATUS DSPLIB_diag_exec_cn<cint, DSPLIB_CINT32>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn,
                                                                           void *restrict pOut);
