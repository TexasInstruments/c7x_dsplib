// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_fltoq15_priv.h"

DSPLIB_STATUS DSPLIB_fltoq15_init_cn(DSPLIB_kernelHandle            handle,
                                     DSPLIB_bufParams1D_t          *bufParamsIn,
                                     DSPLIB_bufParams1D_t          *bufParamsOut,
                                     const DSPLIB_fltoq15_InitArgs *pKerInitArgs)
{
   return DSPLIB_SUCCESS;
}

template <typename dataTypeIn>
DSPLIB_STATUS DSPLIB_fltoq15_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_fltoq15_PrivArgs *pKerPrivArgs = (DSPLIB_fltoq15_PrivArgs *) handle;
   uint32_t                 blockSize    = pKerPrivArgs->blockSize;

   dataTypeIn *pInLocal  = (dataTypeIn *) pIn;
   int16_t    *pOutLocal = (int16_t *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p blockSize: %d\n", pInLocal, pOutLocal, blockSize);

   dataTypeIn a;
   dataTypeIn mulFactor = 32768.00;
   int32_t    aInt      = 0;
   int16_t    out       = 0;

   for (int32_t counter = 0; counter < (int32_t) blockSize; counter++) {
      a                 = *pInLocal++;
      dataTypeIn mulVal = a * mulFactor;

      int32_t    intPart  = (int32_t) mulVal;
      dataTypeIn fracPart = mulVal - intPart;

      if ((intPart % 2 == 0) && (abs(fracPart) == 0.5)) {
         aInt = intPart;
      }
      else if (a < 0) {
         aInt = (int32_t) (mulVal - 0.5f);
      }
      else {
         aInt = (int32_t) (mulVal + 0.5f);
      }

      /* saturate to 16-bit */
      if (aInt > 32767) {
         aInt = 32767;
      }
      if (aInt < -32768) {
         aInt = -32768;
      }

      out          = (short) aInt;
      *pOutLocal++ = out;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_fltoq15_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_fltoq15_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
