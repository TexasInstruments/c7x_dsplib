// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_BLKCOPY2D_IDAT_H
#define DSPLIB_BLKCOPY2D_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t  testPattern; /* 0: constant, 1: sequential, 2: sequential_negative, 3: random etc.*/
   void    *staticIn;
   void    *staticOut;
   uint32_t dataType;
   uint32_t width;
   uint32_t height;
   uint32_t strideIn;           // Input stride in bytes
   uint32_t strideOut;          // Output stride in bytes
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} blkCopy2D_testParams_t;

void blkCopy2D_getTestParams(blkCopy2D_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_BLKCOPY2D_IDAT_H */
