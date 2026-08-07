// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_binaryLogicGate_priv.h"
#include <cstdint>
#include <type_traits>

// Define the op templates (tag dispatch on opType)
template <uint32_t opType> using opTagCn = std::integral_constant<uint32_t, opType>;

template <typename dataType>
inline dataType binaryLogicGate_op_cn(opTagCn<DSPLIB_BINARYLOGICGATE_AND>, dataType a, dataType b)
{
   return a & b;
}

template <typename dataType>
inline dataType binaryLogicGate_op_cn(opTagCn<DSPLIB_BINARYLOGICGATE_OR>, dataType a, dataType b)
{
   return a | b;
}

template <typename dataType>
inline dataType binaryLogicGate_op_cn(opTagCn<DSPLIB_BINARYLOGICGATE_XOR>, dataType a, dataType b)
{
   return a ^ b;
}

template <typename dataType, uint32_t opType>
DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn(DSPLIB_kernelHandle handle,
                                             void *restrict pIn0,
                                             void *restrict pIn1,
                                             void *restrict pOut)
{
   DSPLIB_STATUS                    status       = DSPLIB_SUCCESS;
   DSPLIB_binaryLogicGate_PrivArgs *pKerPrivArgs = (DSPLIB_binaryLogicGate_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_binaryLogicGate_exec_cn");

   dataType *restrict pIn0Local = (dataType *) pIn0;
   dataType *restrict pIn1Local = (dataType *) pIn1;
   dataType *restrict pOutLocal = (dataType *) pOut;

   uint32_t dim_x     = pKerPrivArgs->dim_x;
   uint32_t dim_y     = pKerPrivArgs->dim_y;
   int32_t  strideIn  = pKerPrivArgs->strideIn;
   int32_t  strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      for (uint32_t n = 0; n < dim_x; n++) {
         pOutLocal[m * strideOut + n] =
             binaryLogicGate_op_cn(opTagCn<opType>(), pIn0Local[m * strideIn + n], pIn1Local[m * strideIn + n]);
      }
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint8_t, DSPLIB_BINARYLOGICGATE_AND>(DSPLIB_kernelHandle handle,
                                                                                           void *restrict pIn0,
                                                                                           void *restrict pIn1,
                                                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint8_t, DSPLIB_BINARYLOGICGATE_OR>(DSPLIB_kernelHandle handle,
                                                                                          void *restrict pIn0,
                                                                                          void *restrict pIn1,
                                                                                          void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint8_t, DSPLIB_BINARYLOGICGATE_XOR>(DSPLIB_kernelHandle handle,
                                                                                           void *restrict pIn0,
                                                                                           void *restrict pIn1,
                                                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint16_t, DSPLIB_BINARYLOGICGATE_AND>(DSPLIB_kernelHandle handle,
                                                                                            void *restrict pIn0,
                                                                                            void *restrict pIn1,
                                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint16_t, DSPLIB_BINARYLOGICGATE_OR>(DSPLIB_kernelHandle handle,
                                                                                           void *restrict pIn0,
                                                                                           void *restrict pIn1,
                                                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint16_t, DSPLIB_BINARYLOGICGATE_XOR>(DSPLIB_kernelHandle handle,
                                                                                            void *restrict pIn0,
                                                                                            void *restrict pIn1,
                                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint32_t, DSPLIB_BINARYLOGICGATE_AND>(DSPLIB_kernelHandle handle,
                                                                                            void *restrict pIn0,
                                                                                            void *restrict pIn1,
                                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint32_t, DSPLIB_BINARYLOGICGATE_OR>(DSPLIB_kernelHandle handle,
                                                                                           void *restrict pIn0,
                                                                                           void *restrict pIn1,
                                                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint32_t, DSPLIB_BINARYLOGICGATE_XOR>(DSPLIB_kernelHandle handle,
                                                                                            void *restrict pIn0,
                                                                                            void *restrict pIn1,
                                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint64_t, DSPLIB_BINARYLOGICGATE_AND>(DSPLIB_kernelHandle handle,
                                                                                            void *restrict pIn0,
                                                                                            void *restrict pIn1,
                                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint64_t, DSPLIB_BINARYLOGICGATE_OR>(DSPLIB_kernelHandle handle,
                                                                                           void *restrict pIn0,
                                                                                           void *restrict pIn1,
                                                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn<uint64_t, DSPLIB_BINARYLOGICGATE_XOR>(DSPLIB_kernelHandle handle,
                                                                                            void *restrict pIn0,
                                                                                            void *restrict pIn1,
                                                                                            void *restrict pOut);
