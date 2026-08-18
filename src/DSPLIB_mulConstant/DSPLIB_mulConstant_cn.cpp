// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_mulConstant_priv.h"

template <typename dataType, int32_t dataIn>
DSPLIB_STATUS
DSPLIB_mulConstant_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;
   uint32_t                     blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_mulConstant_exec_cn\n");
#endif

   dataType *pInLocal1 = (dataType *) pIn1;
   dataType *pInLocal2 = (dataType *) pIn2;
   dataType *pOutLocal = (dataType *) pOut;

   dataType a, b, out;
   a = *pInLocal1;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      b      = *pInLocal2++;
      out    = a * b;
      *pOutLocal++ = out;
   }
   return (status);
}

/* Complex float: full complex multiply via float arithmetic */
template <>
DSPLIB_STATUS
DSPLIB_mulConstant_exec_cn<cfloat, DSPLIB_CFLOAT32>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pOut)
{
   DSPLIB_STATUS                status       = DSPLIB_SUCCESS;
   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;
   uint32_t                     blockSize    = pKerPrivArgs->blockSize;

   cfloat *pInLocal1 = (cfloat *) pIn1;
   cfloat *pInLocal2 = (cfloat *) pIn2;
   cfloat *pOutLocal = (cfloat *) pOut;

   cfloat a = *pInLocal1;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      cfloat b        = *pInLocal2++;
      *pOutLocal++    = a * b;   /* cfloat * cfloat operator */
   }
   return status;
}

/* Complex short (cshort): Q15 complex multiply matching __cmpy_fx
 * TI complex types have no member access — treat as int16_t[2] arrays */
template <>
DSPLIB_STATUS
DSPLIB_mulConstant_exec_cn<cshort, DSPLIB_CINT16>(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn1,
                                                   void *restrict pIn2,
                                                   void *restrict pOut)
{
   DSPLIB_STATUS                status       = DSPLIB_SUCCESS;
   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;
   uint32_t                     blockSize    = pKerPrivArgs->blockSize;

   int16_t *pInLocal1 = (int16_t *) pIn1;
   int16_t *pInLocal2 = (int16_t *) pIn2;
   int16_t *pOutLocal = (int16_t *) pOut;

   int32_t aRe = (int32_t) pInLocal1[0];
   int32_t aIm = (int32_t) pInLocal1[1];

   for (int32_t counter = 0; counter < blockSize; counter++) {
      int32_t bRe = (int32_t) pInLocal2[0];
      int32_t bIm = (int32_t) pInLocal2[1];

      /* Q15 multiply with rounding: (a*b + 0x4000) >> 15, clamped to int16 */
      int32_t outRe32 = aRe * bRe - aIm * bIm;
      int32_t outIm32 = aRe * bIm + aIm * bRe;
      int32_t outRe   = (outRe32 + (1 << 14)) >> 15;
      int32_t outIm   = (outIm32 + (1 << 14)) >> 15;

      /* Saturate to int16 range */
      if (outRe > INT16_MAX)  {
         outRe = INT16_MAX;
      }
      if (outRe < INT16_MIN) {
         outRe = INT16_MIN;
      }
      if (outIm > INT16_MAX){
         outIm = INT16_MAX;
      }
      if (outIm < INT16_MIN){
         outIm = INT16_MIN;
      }

      pOutLocal[0] = (int16_t) outRe;
      pOutLocal[1] = (int16_t) outIm;
      pInLocal2 += 2;
      pOutLocal += 2;
   }
   return status;
}

/* Complex int (cint): Q31 complex multiply matching __cmpy_fx / VCMPYR1WW.
 * TI complex types have no member access — treat as int32_t[2] arrays. */
template <>
DSPLIB_STATUS
DSPLIB_mulConstant_exec_cn<cint, DSPLIB_CINT32>(DSPLIB_kernelHandle handle,
                                                 void *restrict pIn1,
                                                 void *restrict pIn2,
                                                 void *restrict pOut)
{
   DSPLIB_STATUS                status       = DSPLIB_SUCCESS;
   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;
   uint32_t                     blockSize    = pKerPrivArgs->blockSize;

   int32_t *pInLocal1 = (int32_t *) pIn1;
   int32_t *pInLocal2 = (int32_t *) pIn2;
   int32_t *pOutLocal = (int32_t *) pOut;

   int64_t aRe = (int64_t) pInLocal1[0];
   int64_t aIm = (int64_t) pInLocal1[1];

   for (int32_t counter = 0; counter < blockSize; counter++) {
      int64_t bRe   = (int64_t) pInLocal2[0];
      int64_t bIm   = (int64_t) pInLocal2[1];
      int64_t outRe = (aRe * bRe - aIm * bIm + (1LL << 30)) >> 31;
      int64_t outIm = (aRe * bIm + aIm * bRe + (1LL << 30)) >> 31;

      /* Saturate to int32 */
      if (outRe > INT32_MAX) {
          outRe = INT32_MAX;
      }
      if (outRe < INT32_MIN) {
          outRe = INT32_MIN;
      }
      if (outIm > INT32_MAX) {
          outIm = INT32_MAX;
      }
      if (outIm < INT32_MIN) {
          outIm = INT32_MIN;
      }

      pOutLocal[0] = (int32_t) outRe;
      pOutLocal[1] = (int32_t) outIm;
      pInLocal2 += 2;
      pOutLocal += 2;
   }
   return status;
}

// explicit instantiations for real types
template DSPLIB_STATUS DSPLIB_mulConstant_exec_cn<float, DSPLIB_FLOAT32>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);

template DSPLIB_STATUS DSPLIB_mulConstant_exec_cn<double, DSPLIB_FLOAT64>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn1,
                                                                          void *restrict pIn2,
                                                                          void *restrict pOut);
template DSPLIB_STATUS DSPLIB_mulConstant_exec_cn<int8_t, DSPLIB_INT8>(DSPLIB_kernelHandle handle,
                                                                       void *restrict pIn1,
                                                                       void *restrict pIn2,
                                                                       void *restrict pOut);
template DSPLIB_STATUS DSPLIB_mulConstant_exec_cn<uint8_t, DSPLIB_UINT8>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_mulConstant_exec_cn<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_mulConstant_exec_cn<uint16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn1,
                                                                           void *restrict pIn2,
                                                                           void *restrict pOut);
template DSPLIB_STATUS DSPLIB_mulConstant_exec_cn<int32_t, DSPLIB_INT32>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_mulConstant_exec_cn<uint32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn1,
                                                                           void *restrict pIn2,
                                                                           void *restrict pOut);
