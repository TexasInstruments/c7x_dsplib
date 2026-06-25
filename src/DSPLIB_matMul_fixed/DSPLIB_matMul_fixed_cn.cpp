// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_matMul_fixed_priv.h"
#include <cstdint>

template <typename dataType>
DSPLIB_STATUS
DSPLIB_matMul_fixed_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;

   int32_t m       = 0;
   int32_t k       = 0;
   int32_t n       = 0;
   int32_t qs      = 0;
   int64_t product = 0;
   int64_t sum     = 0;

   int32_t M         = pKerPrivArgs->M;
   int32_t K         = pKerPrivArgs->K;
   int32_t N         = pKerPrivArgs->N;
   qs                = pKerPrivArgs->qs;
   int32_t strideIn0 = pKerPrivArgs->strideIn0Elements;
   int32_t strideIn1 = pKerPrivArgs->strideIn1Elements;
   int32_t strideOut = pKerPrivArgs->strideOutElements;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_matMul_fixed_exec_cn\n");
#endif

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_matMul_fixed_exec_cn: M = %d, N = %d, K = %d, strideIn0 = %d, strideIn1 = %d, strideOut = %d\n", M, N,
          K, strideIn0, strideIn1, strideOut);
#endif

   dataType *restrict A = (dataType *) pIn0;
   dataType *restrict B = (dataType *) pIn1;

   dataType *restrict C = (dataType *) pOut;

   for (m = 0; m < M; m++) {
      for (n = 0; n < N; n++) {
         sum = 0;
         for (k = 0; k < K; k++) {
            product = A[k + m * strideIn0] * B[n + k * strideIn1];
            sum     = sum + product;
         }
         C[n + m * strideOut] = (int64_t) ((uint64_t) sum >> (uint64_t) qs);
      }
   }
   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_cn<int8_t>(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn0,
                                                           void *restrict pIn1,
                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_cn<int16_t>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn0,
                                                            void *restrict pIn1,
                                                            void *restrict pOut);
