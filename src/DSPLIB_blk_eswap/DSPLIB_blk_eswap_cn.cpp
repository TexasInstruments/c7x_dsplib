// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blk_eswap_priv.h"

DSPLIB_STATUS DSPLIB_blk_eswap_init_cn(DSPLIB_kernelHandle              handle,
                                       DSPLIB_bufParams1D_t            *bufParamsIn,
                                       DSPLIB_bufParams1D_t            *bufParamsOut,
                                       const DSPLIB_blk_eswap_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}

template <uint32_t implementationType>
inline void blk_eswap_compute(uint8_t *restrict srcPtr, uint8_t *restrict dstPtr, uint32_t blockSize);

template <>
inline void blk_eswap_compute<DSPLIB_UINT16>(uint8_t *restrict srcPtr, uint8_t *restrict dstPtr, uint32_t blockSize)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for 16-bit data");

   uint8_t tempBuff[2];
   for (uint32_t cntr = 0; cntr < blockSize; cntr++) {
      tempBuff[0] = srcPtr[cntr * 2 + 1];
      tempBuff[1] = srcPtr[cntr * 2 + 0];

      dstPtr[cntr * 2 + 0] = tempBuff[0];
      dstPtr[cntr * 2 + 1] = tempBuff[1];
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for 16-bit data");
}

template <>
inline void blk_eswap_compute<DSPLIB_UINT32>(uint8_t *restrict srcPtr, uint8_t *restrict dstPtr, uint32_t blockSize)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for 32-bit data");

   uint8_t tempBuff[4];
   for (uint32_t cntr = 0; cntr < blockSize; cntr++) {
      tempBuff[0] = srcPtr[cntr * 4 + 3];
      tempBuff[1] = srcPtr[cntr * 4 + 2];
      tempBuff[2] = srcPtr[cntr * 4 + 1];
      tempBuff[3] = srcPtr[cntr * 4 + 0];

      dstPtr[cntr * 4 + 0] = tempBuff[0];
      dstPtr[cntr * 4 + 1] = tempBuff[1];
      dstPtr[cntr * 4 + 2] = tempBuff[2];
      dstPtr[cntr * 4 + 3] = tempBuff[3];
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for 32-bit data");
}

template <>
inline void blk_eswap_compute<DSPLIB_UINT64>(uint8_t *restrict srcPtr, uint8_t *restrict dstPtr, uint32_t blockSize)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for 64-bit data");

   uint8_t tempBuff[8];
   for (uint32_t cntr = 0; cntr < blockSize; cntr++) {
      tempBuff[0] = srcPtr[cntr * 8 + 7];
      tempBuff[1] = srcPtr[cntr * 8 + 6];
      tempBuff[2] = srcPtr[cntr * 8 + 5];
      tempBuff[3] = srcPtr[cntr * 8 + 4];
      tempBuff[4] = srcPtr[cntr * 8 + 3];
      tempBuff[5] = srcPtr[cntr * 8 + 2];
      tempBuff[6] = srcPtr[cntr * 8 + 1];
      tempBuff[7] = srcPtr[cntr * 8 + 0];

      dstPtr[cntr * 8 + 0] = tempBuff[0];
      dstPtr[cntr * 8 + 1] = tempBuff[1];
      dstPtr[cntr * 8 + 2] = tempBuff[2];
      dstPtr[cntr * 8 + 3] = tempBuff[3];
      dstPtr[cntr * 8 + 4] = tempBuff[4];
      dstPtr[cntr * 8 + 5] = tempBuff[5];
      dstPtr[cntr * 8 + 6] = tempBuff[6];
      dstPtr[cntr * 8 + 7] = tempBuff[7];
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for 64-bit data");
}

template <typename dataType, uint32_t implementationType>
DSPLIB_STATUS DSPLIB_blk_eswap_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_blk_eswap_PrivArgs *pKerPrivArgs = (DSPLIB_blk_eswap_PrivArgs *) handle;
   uint32_t                   blockSize    = pKerPrivArgs->blockSize;

   uint8_t *srcPtr = (uint8_t *) pIn;
   uint8_t *dstPtr = (uint8_t *) pOut;
   if (!dstPtr) {
      dstPtr = srcPtr; /* In-place swaping */
   }

   DSPLIB_DEBUGPRINTFN(0, "srcPtr: %p dstPtr: %p blockSize: %d\n", srcPtr, dstPtr, blockSize);

   blk_eswap_compute<implementationType>(srcPtr, dstPtr, blockSize);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

// explicit instantiation for the different data type versions

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_cn<uint16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_cn<uint32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_cn<uint64_t, DSPLIB_UINT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_cn<int16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_cn<int32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_cn<int64_t, DSPLIB_UINT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_cn<float, DSPLIB_UINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_cn<double, DSPLIB_UINT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
