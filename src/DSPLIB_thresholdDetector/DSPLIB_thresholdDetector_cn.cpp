// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_thresholdDetector_priv.h"
#include <cstdint>
#include <type_traits>

// Define the compare templates (tag dispatch on opType)
template <uint32_t opType> using opTagCn = std::integral_constant<uint32_t, opType>;

template <typename dataTypeIn, typename dataTypeOut>
inline dataTypeOut thresholdDetector_op_cn(opTagCn<DSPLIB_THRESHOLDDETECTOR_EQ>, dataTypeIn a, dataTypeIn value)
{
   return a == value;
}

template <typename dataTypeIn, typename dataTypeOut>
inline dataTypeOut thresholdDetector_op_cn(opTagCn<DSPLIB_THRESHOLDDETECTOR_NE>, dataTypeIn a, dataTypeIn value)
{
   return a != value;
}

template <typename dataTypeIn, typename dataTypeOut>
inline dataTypeOut thresholdDetector_op_cn(opTagCn<DSPLIB_THRESHOLDDETECTOR_LT>, dataTypeIn a, dataTypeIn value)
{
   return a < value;
}

template <typename dataTypeIn, typename dataTypeOut>
inline dataTypeOut thresholdDetector_op_cn(opTagCn<DSPLIB_THRESHOLDDETECTOR_LE>, dataTypeIn a, dataTypeIn value)
{
   return a <= value;
}

template <typename dataTypeIn, typename dataTypeOut>
inline dataTypeOut thresholdDetector_op_cn(opTagCn<DSPLIB_THRESHOLDDETECTOR_GT>, dataTypeIn a, dataTypeIn value)
{
   return a > value;
}

template <typename dataTypeIn, typename dataTypeOut>
inline dataTypeOut thresholdDetector_op_cn(opTagCn<DSPLIB_THRESHOLDDETECTOR_GE>, dataTypeIn a, dataTypeIn value)
{
   return a >= value;
}

template <typename dataTypeIn, typename dataTypeOut, uint32_t opType>
DSPLIB_STATUS DSPLIB_thresholdDetector_exec_cn(DSPLIB_kernelHandle handle,
                                               void *restrict pIn0,
                                               void *restrict pIn1,
                                               void *restrict pOut)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_thresholdDetector_PrivArgs *pKerPrivArgs = (DSPLIB_thresholdDetector_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_thresholdDetector_exec_cn");

   dataTypeIn *restrict pIn0Local  = (dataTypeIn *) pIn0;
   dataTypeIn *restrict pIn1Local  = (dataTypeIn *) pIn1;
   dataTypeOut *restrict pOutLocal = (dataTypeOut *) pOut;

   dataTypeIn value     = *pIn1Local;
   uint32_t   dim_x     = pKerPrivArgs->dim_x;
   uint32_t   dim_y     = pKerPrivArgs->dim_y;
   int32_t    strideIn  = pKerPrivArgs->strideIn;
   int32_t    strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      for (uint32_t n = 0; n < dim_x; n++) {
         pOutLocal[m * strideOut + n] =
             thresholdDetector_op_cn<dataTypeIn, dataTypeOut>(opTagCn<opType>(), pIn0Local[m * strideIn + n], value);
      }
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_NE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_LT>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_LE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_GT>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_GE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);
