// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_minerror_priv.h"

// Promote the datatype to handle overflow
template <typename Integer8or16or32BitDataType, typename Integer8or16or32BitPromotedDataType>
inline void minerror_exec_cn_integer8or16or32_inputs(void *restrict pIn,
                                                     void *restrict pErrCoefs,
                                                     const int *restrict pMaxIndex,
                                                     const void *restrict pMaxVal,
                                                     uint32_t vecInSize,
                                                     uint32_t errCoefsSize,
                                                     int32_t  strideIn)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   int32_t                              dataSize       = sizeof(Integer8or16or32BitDataType);
   Integer8or16or32BitDataType         *pInLocal       = (Integer8or16or32BitDataType *) pIn;
   Integer8or16or32BitDataType         *pErrCoefsLocal = (Integer8or16or32BitDataType *) pErrCoefs;
   int                                 *pMaxIndexLocal = (int *) pMaxIndex;
   Integer8or16or32BitPromotedDataType *pMaxValLocal   = (Integer8or16or32BitPromotedDataType *) pMaxVal;

   Integer8or16or32BitPromotedDataType val;
   *pMaxValLocal = std::numeric_limits<Integer8or16or32BitPromotedDataType>::min();

   for (int32_t counter = 0; counter < (int32_t) vecInSize; counter++) {
      val = 0;
      Integer8or16or32BitPromotedDataType product1 =
          ((((Integer8or16or32BitPromotedDataType) pInLocal[counter * (strideIn / dataSize) + 0]) *
            (Integer8or16or32BitPromotedDataType) pErrCoefsLocal[0]));
      Integer8or16or32BitPromotedDataType product2 =
          ((((Integer8or16or32BitPromotedDataType) pInLocal[counter * (strideIn / dataSize) + 1]) *
            (Integer8or16or32BitPromotedDataType) pErrCoefsLocal[1]));
      Integer8or16or32BitPromotedDataType product3 =
          ((((Integer8or16or32BitPromotedDataType) pInLocal[counter * (strideIn / dataSize) + 2]) *
            (Integer8or16or32BitPromotedDataType) pErrCoefsLocal[2]));
      Integer8or16or32BitPromotedDataType product4 =
          ((((Integer8or16or32BitPromotedDataType) pInLocal[counter * (strideIn / dataSize) + 3]) *
            (Integer8or16or32BitPromotedDataType) pErrCoefsLocal[3]));
      Integer8or16or32BitPromotedDataType product5 =
          ((((Integer8or16or32BitPromotedDataType) pInLocal[counter * (strideIn / dataSize) + 4]) *
            (Integer8or16or32BitPromotedDataType) pErrCoefsLocal[4]));
      Integer8or16or32BitPromotedDataType product6 =
          ((((Integer8or16or32BitPromotedDataType) pInLocal[counter * (strideIn / dataSize) + 5]) *
            (Integer8or16or32BitPromotedDataType) pErrCoefsLocal[5]));
      Integer8or16or32BitPromotedDataType product7 =
          ((((Integer8or16or32BitPromotedDataType) pInLocal[counter * (strideIn / dataSize) + 6]) *
            (Integer8or16or32BitPromotedDataType) pErrCoefsLocal[6]));
      Integer8or16or32BitPromotedDataType product8 =
          ((((Integer8or16or32BitPromotedDataType) pInLocal[counter * (strideIn / dataSize) + 7]) *
            (Integer8or16or32BitPromotedDataType) pErrCoefsLocal[7]));
      Integer8or16or32BitPromotedDataType product9 =
          ((((Integer8or16or32BitPromotedDataType) pInLocal[counter * (strideIn / dataSize) + 8]) *
            (Integer8or16or32BitPromotedDataType) pErrCoefsLocal[8]));

      Integer8or16or32BitPromotedDataType acc1 = (product1 >> 1) + (product2 >> 1);
      Integer8or16or32BitPromotedDataType acc2 = (product3 >> 1) + (product4 >> 1);
      Integer8or16or32BitPromotedDataType acc3 = (product5 >> 1) + (product6 >> 1);
      Integer8or16or32BitPromotedDataType acc4 = (product7 >> 1) + (product8 >> 1);
      Integer8or16or32BitPromotedDataType acc5 = (acc1 >> 1) + (acc2 >> 1);
      Integer8or16or32BitPromotedDataType acc6 = (acc3 >> 1) + (acc4 >> 1);
      Integer8or16or32BitPromotedDataType acc7 = (acc5 >> 1) + (acc6 >> 1);
      val                                      = (acc7 >> 1) + (product9 >> 1);

      // Store the max value and the corresponding index
      if (val > (*pMaxValLocal)) {
         *pMaxValLocal   = val;
         *pMaxIndexLocal = counter;
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exit Function");
}

// Convert integer 64 bits to double to handle overflow

template <typename Integer64BitDataType, typename Integer64BitConvertedDataType>
inline void minerror_exec_cn_integer64_inputs(void *restrict pIn,
                                              void *restrict pErrCoefs,
                                              const int *restrict pMaxIndex,
                                              const void *restrict pMaxVal,
                                              uint32_t vecInSize,
                                              uint32_t errCoefsSize,
                                              int32_t  strideIn)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   int32_t                        dataSize       = sizeof(Integer64BitDataType);
   Integer64BitDataType          *pInLocal       = (Integer64BitDataType *) pIn;
   Integer64BitDataType          *pErrCoefsLocal = (Integer64BitDataType *) pErrCoefs;
   int                           *pMaxIndexLocal = (int *) pMaxIndex;
   Integer64BitConvertedDataType *pMaxValLocal   = (Integer64BitConvertedDataType *) pMaxVal;

   Integer64BitConvertedDataType val, pMaxValScaler;
   pMaxValScaler = std::numeric_limits<Integer64BitConvertedDataType>::min();
   *pMaxValLocal = pMaxValScaler;

   for (int32_t counter = 0; counter < (int32_t) vecInSize; counter++) {
      val = 0;
      // Dot product
      for (int32_t j = 0; j < (int32_t) errCoefsSize; j++) {
         val += ((Integer64BitConvertedDataType) pInLocal[counter * (strideIn / dataSize) + j]) *
                (Integer64BitDataType) pErrCoefsLocal[j];
      }

      // Store the max value and the corresponding index
      if (val > (pMaxValScaler)) {
         pMaxValScaler   = val;
         *pMaxIndexLocal = counter;
      }
   }
   *pMaxValLocal = (Integer64BitDataType) pMaxValScaler;
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exit Function");
}

template <typename FloatingPointDataType, typename FloatingPointPromotedDataType>
inline void minerror_exec_cn_float_inputs(void *restrict pIn,
                                          void *restrict pErrCoefs,
                                          const int *restrict pMaxIndex,
                                          const void *restrict pMaxVal,
                                          uint32_t vecInSize,
                                          uint32_t errCoefsSize,
                                          int32_t  strideIn)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   int32_t                        dataSize       = sizeof(FloatingPointDataType);
   FloatingPointDataType         *pInLocal       = (FloatingPointDataType *) pIn;
   FloatingPointPromotedDataType *pErrCoefsLocal = (FloatingPointPromotedDataType *) pErrCoefs;
   int                           *pMaxIndexLocal = (int *) pMaxIndex;
   FloatingPointPromotedDataType *pMaxValLocal   = (FloatingPointPromotedDataType *) pMaxVal;

   FloatingPointPromotedDataType val;
   *pMaxValLocal = std::numeric_limits<FloatingPointPromotedDataType>::min();

   for (int32_t counter = 0; counter < (int32_t) vecInSize; counter++) {
      val = 0;
      // Dot product
      for (int32_t j = 0; j < (int32_t) errCoefsSize; j++) {
         val += ((FloatingPointPromotedDataType) pInLocal[counter * (strideIn / dataSize) + j]) * pErrCoefsLocal[j];
      }

      // Store the max value and the corresponding index
      if (val > (*pMaxValLocal)) {
         *pMaxValLocal   = val;
         *pMaxIndexLocal = counter;
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exit Function");
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<int8_t>(DSPLIB_kernelHandle handle,
                                              void *restrict pIn,
                                              void *restrict pErrCoefs,
                                              const int *restrict pMaxIndex,
                                              const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_integer8or16or32_inputs<int8_t, int16_t>(
       pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<uint8_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_integer8or16or32_inputs<uint8_t, uint16_t>(
       pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<int16_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_integer8or16or32_inputs<int16_t, int32_t>(
       pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<uint16_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pErrCoefs,
                                                const int *restrict pMaxIndex,
                                                const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_integer8or16or32_inputs<uint16_t, uint32_t>(
       pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<int32_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_integer8or16or32_inputs<int32_t, int64_t>(
       pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<uint32_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pErrCoefs,
                                                const int *restrict pMaxIndex,
                                                const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_integer8or16or32_inputs<uint32_t, uint64_t>(
       pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<int64_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_integer64_inputs<int64_t, double>(pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize,
                                                      pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<uint64_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pErrCoefs,
                                                const int *restrict pMaxIndex,
                                                const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_integer64_inputs<uint64_t, double>(pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize,
                                                       pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<float>(DSPLIB_kernelHandle handle,
                                             void *restrict pIn,
                                             void *restrict pErrCoefs,
                                             const int *restrict pMaxIndex,
                                             const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_float_inputs<float, float>(pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize,
                                               pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}

template <>
DSPLIB_STATUS DSPLIB_minerror_exec_cn<double>(DSPLIB_kernelHandle handle,
                                              void *restrict pIn,
                                              void *restrict pErrCoefs,
                                              const int *restrict pMaxIndex,
                                              const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   minerror_exec_cn_float_inputs<double, double>(pIn, pErrCoefs, pMaxIndex, pMaxVal, pKerPrivArgs->vecInSize,
                                                 pKerPrivArgs->errCoefsSize, pKerPrivArgs->strideIn);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return (status);
}
