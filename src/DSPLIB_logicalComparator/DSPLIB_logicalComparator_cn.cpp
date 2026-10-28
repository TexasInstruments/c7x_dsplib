// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_logicalComparator_priv.h"
#include <cstdint>

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn0,
                                                  void *restrict pIn1,
                                                  void *restrict pOut)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_logicalComparator_PrivArgs *pKerPrivArgs = (DSPLIB_logicalComparator_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_logicalComparator_eq_exec_cn");

   dataTypeIn *restrict pIn0Local  = (dataTypeIn *) pIn0;
   dataTypeIn *restrict pIn1Local  = (dataTypeIn *) pIn1;
   dataTypeOut *restrict pOutLocal = (dataTypeOut *) pOut;

   uint32_t dim_x     = pKerPrivArgs->dim_x;
   uint32_t dim_y     = pKerPrivArgs->dim_y;
   int32_t  strideIn  = pKerPrivArgs->strideIn;
   int32_t  strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      for (uint32_t n = 0; n < dim_x; n++) {
         pOutLocal[m * strideOut + n] = pIn0Local[m * strideIn + n] == pIn1Local[m * strideIn + n];
      }
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<int8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<uint8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<int16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<uint16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<int32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<uint32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<int64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<uint64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<float, uint32_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_eq_exec_cn<double, uint64_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn0,
                                                  void *restrict pIn1,
                                                  void *restrict pOut)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_logicalComparator_PrivArgs *pKerPrivArgs = (DSPLIB_logicalComparator_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_logicalComparator_ne_exec_cn");

   dataTypeIn *restrict pIn0Local  = (dataTypeIn *) pIn0;
   dataTypeIn *restrict pIn1Local  = (dataTypeIn *) pIn1;
   dataTypeOut *restrict pOutLocal = (dataTypeOut *) pOut;

   uint32_t dim_x     = pKerPrivArgs->dim_x;
   uint32_t dim_y     = pKerPrivArgs->dim_y;
   int32_t  strideIn  = pKerPrivArgs->strideIn;
   int32_t  strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      for (uint32_t n = 0; n < dim_x; n++) {
         pOutLocal[m * strideOut + n] = pIn0Local[m * strideIn + n] != pIn1Local[m * strideIn + n];
      }
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<int8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<uint8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<int16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<uint16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<int32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<uint32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<int64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<uint64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<float, uint32_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ne_exec_cn<double, uint64_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn0,
                                                  void *restrict pIn1,
                                                  void *restrict pOut)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_logicalComparator_PrivArgs *pKerPrivArgs = (DSPLIB_logicalComparator_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_logicalComparator_lt_exec_cn");

   dataTypeIn *restrict pIn0Local  = (dataTypeIn *) pIn0;
   dataTypeIn *restrict pIn1Local  = (dataTypeIn *) pIn1;
   dataTypeOut *restrict pOutLocal = (dataTypeOut *) pOut;

   uint32_t dim_x     = pKerPrivArgs->dim_x;
   uint32_t dim_y     = pKerPrivArgs->dim_y;
   int32_t  strideIn  = pKerPrivArgs->strideIn;
   int32_t  strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      for (uint32_t n = 0; n < dim_x; n++) {
         pOutLocal[m * strideOut + n] = pIn0Local[m * strideIn + n] < pIn1Local[m * strideIn + n];
      }
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<int8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<uint8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<int16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<uint16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<int32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<uint32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<int64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<uint64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<float, uint32_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_lt_exec_cn<double, uint64_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn0,
                                                  void *restrict pIn1,
                                                  void *restrict pOut)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_logicalComparator_PrivArgs *pKerPrivArgs = (DSPLIB_logicalComparator_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_logicalComparator_le_exec_cn");

   dataTypeIn *restrict pIn0Local  = (dataTypeIn *) pIn0;
   dataTypeIn *restrict pIn1Local  = (dataTypeIn *) pIn1;
   dataTypeOut *restrict pOutLocal = (dataTypeOut *) pOut;

   uint32_t dim_x     = pKerPrivArgs->dim_x;
   uint32_t dim_y     = pKerPrivArgs->dim_y;
   int32_t  strideIn  = pKerPrivArgs->strideIn;
   int32_t  strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      for (uint32_t n = 0; n < dim_x; n++) {
         pOutLocal[m * strideOut + n] = pIn0Local[m * strideIn + n] <= pIn1Local[m * strideIn + n];
      }
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<int8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<uint8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<int16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<uint16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<int32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<uint32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<int64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<uint64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<float, uint32_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_le_exec_cn<double, uint64_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn0,
                                                  void *restrict pIn1,
                                                  void *restrict pOut)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_logicalComparator_PrivArgs *pKerPrivArgs = (DSPLIB_logicalComparator_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_logicalComparator_gt_exec_cn");

   dataTypeIn *restrict pIn0Local  = (dataTypeIn *) pIn0;
   dataTypeIn *restrict pIn1Local  = (dataTypeIn *) pIn1;
   dataTypeOut *restrict pOutLocal = (dataTypeOut *) pOut;

   uint32_t dim_x     = pKerPrivArgs->dim_x;
   uint32_t dim_y     = pKerPrivArgs->dim_y;
   int32_t  strideIn  = pKerPrivArgs->strideIn;
   int32_t  strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      for (uint32_t n = 0; n < dim_x; n++) {
         pOutLocal[m * strideOut + n] = pIn0Local[m * strideIn + n] > pIn1Local[m * strideIn + n];
      }
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<int8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<uint8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<int16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<uint16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<int32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<uint32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<int64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<uint64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<float, uint32_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_gt_exec_cn<double, uint64_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn0,
                                                  void *restrict pIn1,
                                                  void *restrict pOut)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_logicalComparator_PrivArgs *pKerPrivArgs = (DSPLIB_logicalComparator_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Enter DSPLIB_logicalComparator_ge_exec_cn");

   dataTypeIn *restrict pIn0Local  = (dataTypeIn *) pIn0;
   dataTypeIn *restrict pIn1Local  = (dataTypeIn *) pIn1;
   dataTypeOut *restrict pOutLocal = (dataTypeOut *) pOut;

   uint32_t dim_x     = pKerPrivArgs->dim_x;
   uint32_t dim_y     = pKerPrivArgs->dim_y;
   int32_t  strideIn  = pKerPrivArgs->strideIn;
   int32_t  strideOut = pKerPrivArgs->strideOut;

   for (uint32_t m = 0; m < dim_y; m++) {
      for (uint32_t n = 0; n < dim_x; n++) {
         pOutLocal[m * strideOut + n] = pIn0Local[m * strideIn + n] >= pIn1Local[m * strideIn + n];
      }
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<int8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<uint8_t, uint8_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<int16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<uint16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<int32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<uint32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<int64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pIn1,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<uint64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<float, uint32_t>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pIn1,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_logicalComparator_ge_exec_cn<double, uint64_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pIn1,
                                                                             void *restrict pOut);
