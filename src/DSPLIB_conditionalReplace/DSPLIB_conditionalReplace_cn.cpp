// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_conditionalReplace_priv.h"
#include <cstdint>

template <typename dataType>
DSPLIB_STATUS DSPLIB_conditionalReplace_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS                       status       = DSPLIB_SUCCESS;
   DSPLIB_conditionalReplace_PrivArgs *pKerPrivArgs = (DSPLIB_conditionalReplace_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_conditionalReplace_exec_cn");

   const dataType *restrict pInLocal      = (const dataType *) pIn;
   const int32_t *restrict pCompareLocal  = pKerPrivArgs->setArgs.pCompareType;
   const dataType *restrict pConstLocal   = (const dataType *) pKerPrivArgs->setArgs.pConstValue;
   const dataType *restrict pReplaceLocal = (const dataType *) pKerPrivArgs->setArgs.pReplaceValue;
   dataType *restrict pOutLocal           = (dataType *) pOut;

   uint32_t dim_x     = pKerPrivArgs->dim_x;
   uint32_t dim_y     = pKerPrivArgs->dim_y;
   int32_t  strideIn  = pKerPrivArgs->strideIn;
   int32_t  strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      int32_t  compareType = pCompareLocal[m];
      dataType constValue  = pConstLocal[m];
      dataType replaceVal  = pReplaceLocal[m];

      for (uint32_t n = 0; n < dim_x; n++) {
         dataType in   = pInLocal[m * strideIn + n];
         bool     cond = false;

         switch (compareType) {
         case DSPLIB_CONDITIONALREPLACE_EQ:
            cond = (in == constValue);
            break;
         case DSPLIB_CONDITIONALREPLACE_NE:
            cond = (in != constValue);
            break;
         case DSPLIB_CONDITIONALREPLACE_LT:
            cond = (in < constValue);
            break;
         case DSPLIB_CONDITIONALREPLACE_LE:
            cond = (in <= constValue);
            break;
         case DSPLIB_CONDITIONALREPLACE_GT:
            cond = (in > constValue);
            break;
         case DSPLIB_CONDITIONALREPLACE_GE:
            cond = (in >= constValue);
            break;
         default:
            /* unreachable: invalid compare types are rejected by DSPLIB_conditionalReplace_set */
            break;
         }

         pOutLocal[m * strideOut + n] = cond ? replaceVal : in;
      }
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
