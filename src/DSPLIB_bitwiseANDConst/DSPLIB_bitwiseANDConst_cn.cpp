// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bitwiseANDConst_priv.h"
#include <cstdint>

template <typename dataType>
DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_cn(DSPLIB_kernelHandle handle,
                                             void *restrict pIn0,
                                             void *restrict pIn1,
                                             void *restrict pOut)
{
   DSPLIB_STATUS                    status       = DSPLIB_SUCCESS;
   DSPLIB_bitwiseANDConst_PrivArgs *pKerPrivArgs = (DSPLIB_bitwiseANDConst_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_bitwiseANDConst_and_exec_cn");

   dataType *restrict pIn0Local = (dataType *) pIn0;
   dataType *restrict pIn1Local = (dataType *) pIn1;
   dataType *restrict pOutLocal = (dataType *) pOut;

   dataType value     = *pIn1Local;
   uint32_t dim_x     = pKerPrivArgs->dim_x;
   uint32_t dim_y     = pKerPrivArgs->dim_y;
   int32_t  strideIn  = pKerPrivArgs->strideIn;
   int32_t  strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      for (uint32_t n = 0; n < dim_x; n++) {
         pOutLocal[m * strideOut + n] = pIn0Local[m * strideIn + n] & value;
      }
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_cn<uint8_t>(DSPLIB_kernelHandle handle,
                                                               void *restrict pIn0,
                                                               void *restrict pIn1,
                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_cn<uint16_t>(DSPLIB_kernelHandle handle,
                                                                void *restrict pIn0,
                                                                void *restrict pIn1,
                                                                void *restrict pOut);

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_cn<uint32_t>(DSPLIB_kernelHandle handle,
                                                                void *restrict pIn0,
                                                                void *restrict pIn1,
                                                                void *restrict pOut);

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_cn<uint64_t>(DSPLIB_kernelHandle handle,
                                                                void *restrict pIn0,
                                                                void *restrict pIn1,
                                                                void *restrict pOut);
