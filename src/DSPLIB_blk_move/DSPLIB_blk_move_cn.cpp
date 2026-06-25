// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blk_move_priv.h"

DSPLIB_STATUS DSPLIB_blk_move_init_cn(DSPLIB_kernelHandle             handle,
                                      DSPLIB_bufParams1D_t           *bufParamsIn,
                                      DSPLIB_bufParams1D_t           *bufParamsOut,
                                      const DSPLIB_blk_move_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_blk_move_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS             status       = DSPLIB_SUCCESS;
   DSPLIB_blk_move_PrivArgs *pKerPrivArgs = (DSPLIB_blk_move_PrivArgs *) handle;
   uint32_t                  blockSize    = pKerPrivArgs->blockSize;

   dataType *pInLocal  = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p blockSize: %d\n", pInLocal, pOutLocal, blockSize);

   for (uint32_t counter = 0; counter < blockSize; counter++) {
      pOutLocal[counter] = pInLocal[counter];
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

// explicit blk_move for the different data type versions
template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
