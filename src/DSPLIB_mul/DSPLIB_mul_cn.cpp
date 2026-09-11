// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_mul_priv.h"

template <typename dataType>
DSPLIB_STATUS
DSPLIB_mul_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_mul_PrivArgs *pKerPrivArgs = (DSPLIB_mul_PrivArgs *) handle;
   uint32_t             blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_mul_exec_cn\n");
#endif

   dataType *pInLocal1 = (dataType *) pIn1;
   dataType *pInLocal2 = (dataType *) pIn2;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   dataType a, b, out;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out = a * b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d y %d\n", counter, out);
#endif

      *pOutLocal++ = out;
   }
   return (status);
}

/* Complex float: full complex multiply via float arithmetic */
template <>
DSPLIB_STATUS
DSPLIB_mul_exec_cn<cfloat>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_STATUS        status       = DSPLIB_SUCCESS;
   DSPLIB_mul_PrivArgs *pKerPrivArgs = (DSPLIB_mul_PrivArgs *) handle;
   uint32_t             blockSize    = pKerPrivArgs->blockSize;

   cfloat *pInLocal1 = (cfloat *) pIn1;
   cfloat *pInLocal2 = (cfloat *) pIn2;
   cfloat *pOutLocal = (cfloat *) pOut;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      *pOutLocal++ = (*pInLocal1++) * (*pInLocal2++);
   }
   return status;
}

/* Complex short (cshort): Q15 complex multiply matching __cmpy_fx
 * TI complex types have no member access — treat as int16_t[2] arrays */
template <>
DSPLIB_STATUS
DSPLIB_mul_exec_cn<cshort>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_STATUS        status       = DSPLIB_SUCCESS;
   DSPLIB_mul_PrivArgs *pKerPrivArgs = (DSPLIB_mul_PrivArgs *) handle;
   uint32_t             blockSize    = pKerPrivArgs->blockSize;

   int16_t *pInLocal1 = (int16_t *) pIn1;
   int16_t *pInLocal2 = (int16_t *) pIn2;
   int16_t *pOutLocal = (int16_t *) pOut;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      int32_t a_re = (int32_t) pInLocal1[0];
      int32_t a_im = (int32_t) pInLocal1[1];
      int32_t b_re = (int32_t) pInLocal2[0];
      int32_t b_im = (int32_t) pInLocal2[1];

      /* Q15 multiply with rounding: (a*b + 0x4000) >> 15, clamped to int16
      matches the hardware ops taken by C7x */
      int32_t out_re32 = a_re * b_re - a_im * b_im;
      int32_t out_im32 = a_re * b_im + a_im * b_re;
      int32_t out_re   = (out_re32 + (1 << 14)) >> 15;
      int32_t out_im   = (out_im32 + (1 << 14)) >> 15;

      /* Saturate to int16 range */
      if (out_re > INT16_MAX)  out_re = INT16_MAX;
      if (out_re < INT16_MIN) out_re = INT16_MIN;
      if (out_im > INT16_MAX)  out_im = INT16_MAX;
      if (out_im < INT16_MIN) out_im = INT16_MIN;

      pOutLocal[0] = (int16_t) out_re;
      pOutLocal[1] = (int16_t) out_im;
      pInLocal1 += 2;
      pInLocal2 += 2;
      pOutLocal += 2;
   }
   return status;
}

/* Complex int (cint): Q31 complex multiply matching __cmpy_fx / VCMPYR1WW.
 * TI complex types have no member access — treat as int32_t[2] arrays. */
template <>
DSPLIB_STATUS
DSPLIB_mul_exec_cn<cint>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_STATUS        status       = DSPLIB_SUCCESS;
   DSPLIB_mul_PrivArgs *pKerPrivArgs = (DSPLIB_mul_PrivArgs *) handle;
   uint32_t             blockSize    = pKerPrivArgs->blockSize;

   int32_t *pInLocal1 = (int32_t *) pIn1;
   int32_t *pInLocal2 = (int32_t *) pIn2;
   int32_t *pOutLocal = (int32_t *) pOut;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      int64_t a_re = (int64_t) pInLocal1[0];
      int64_t a_im = (int64_t) pInLocal1[1];
      int64_t b_re = (int64_t) pInLocal2[0];
      int64_t b_im = (int64_t) pInLocal2[1];
      int64_t out_re = (a_re * b_re - a_im * b_im + (1LL << 30)) >> 31;
      int64_t out_im = (a_re * b_im + a_im * b_re + (1LL << 30)) >> 31;

      /* Saturate to int32 */
      if (out_re > INT32_MAX)  out_re = INT32_MAX;
      if (out_re < INT32_MIN)  out_re = INT32_MIN;
      if (out_im > INT32_MAX)  out_im = INT32_MAX;
      if (out_im < INT32_MIN)  out_im = INT32_MIN;

      pOutLocal[0] = (int32_t) out_re;
      pOutLocal[1] = (int32_t) out_im;
      pInLocal1 += 2;
      pInLocal2 += 2;
      pOutLocal += 2;
   }
   return status;
}

// explicit instantiations for real types
template DSPLIB_STATUS
DSPLIB_mul_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_mul_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_mul_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_mul_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_mul_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_mul_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_mul_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_mul_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);
