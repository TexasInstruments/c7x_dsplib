// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_interleave_IXX_IXX_OXX_IDAT_H

#define DSPLIB_interleave_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t  testPattern; /* 0: constant, 1: sequential, 2: sequential_negative, 3: random etc.*/
   void    *staticIn;
   void    *staticOut;
   uint32_t dataType;
   uint32_t widthIn;
   uint32_t heightIn;
   int32_t  strideIn;
   uint32_t widthOut;
   uint32_t heightOut;
   int32_t  strideOut;
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} DSPLIB_interleave_testParams_t;

void DSPLIB_interleave_getTestParams(DSPLIB_interleave_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_interleave_IXX_IXX_OXX_IDAT_H */
