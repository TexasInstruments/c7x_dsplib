// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bufParams.h"
#include "DSPLIB_recip.h"
#include "DSPLIB_recip_priv.h"
#include "DSPLIB_types.h"

template <typename dataType, int32_t dataIn>
DSPLIB_STATUS DSPLIB_recip_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;
   uint32_t               blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_recip_exec_cn\n");
#endif

   dataType *pInLocal  = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

   dataType a, out;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d\n", counter, a);
#endif

      out = 1 / a;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d y %d\n", counter, out);
#endif

      *pOutLocal++ = out;
   }
   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_recip_exec_cn<float, DSPLIB_FLOAT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_recip_exec_cn<double, DSPLIB_FLOAT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

// Input Buffer: Q15 format
// Output Buffer: 2x size of input, exp and frac

template <>
DSPLIB_STATUS
DSPLIB_recip_exec_cn<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;
   int32_t                blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_recip_exec_cn\n");
#endif

   int16_t *pInLocal  = (int16_t *) pIn;
   int16_t *pOutLocal = (int16_t *) pOut;

   int32_t a, b;
   int16_t neg, normal;

   // Horizontal stack store pattern
   /* +++++++++++++++++++++++++++++++
      | exp0 | exp1 | frac0 | frac1 |
      +++++++++++++++++++++++++++++++
   */

   if (pKerPrivArgs->initArgs.storeStyle == DSPLIB_HSTACK_ST) {
      for (int32_t i = 0; i < blockSize; i++) {
         a = *pInLocal++;

         /* printf("%d,", a); */
         // take absolute value
         if (a < 0) {
            a   = -a;
            neg = 1;
         }

         else {
            neg = 0;
         }

         // normalize
         normal = __norm(a);
         a      = (int32_t) ((uint32_t) a << (uint32_t) normal);

         // store exponent
         *(pOutLocal) = normal - 15;

         // print exp
         /* printf("%d,", *pOutLocal); */

         // dividend
         b = 0x80000000U;

         // divide
         for (int32_t j = 0; j < 15; j++) {
            b = __sub_cond((uint32_t) b, (uint32_t) a);
         }

         b = (int32_t) ((uint32_t) b & (uint32_t) 0x7FFF);

         if (neg) {
            b = -b;
         }

         // store fraction
         *(pOutLocal + blockSize) = b;

         // advance out pointer
         pOutLocal++;
      }
   }

   // Interleaved store pattern
   /* +++++++++++++++++++++++++++++++
      | exp0 | frac0 | exp1 | frac1 |
      +++++++++++++++++++++++++++++++
   */

   else if (pKerPrivArgs->initArgs.storeStyle == DSPLIB_INTERLEAVE_ST) {
      for (int32_t i = 0; i < blockSize; i++) {
         a = *pInLocal++;

         // take absolute value
         if (a < 0) {
            a   = -a;
            neg = 1;
         }

         else {
            neg = 0;
         }

         // normalize
         normal = __norm(a);
         a      = (int32_t) ((uint32_t) a << (uint32_t) normal);

         // store exponent
         *pOutLocal++ = normal - 15;

         // dividend
         b = 0x80000000U;

         // divide
         for (int32_t j = 0; j < 15; j++) {
            b = __sub_cond((uint32_t) b, (uint32_t) a);
         }

         b = (int32_t) ((uint32_t) b & (uint32_t) 0x7FFF);

         if (neg) {
            b = -b;
         }
         // store fraction
         *pOutLocal++ = b;
      }
   }

   else {
      // return error because fixed point cannot perform "linear store"
      status = DSPLIB_ERR_NOT_IMPLEMENTED;
   }
   return (status);
}
