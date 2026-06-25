// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_MIN_IXX_IXX_OXX_IDAT_H

#define DSPLIB_MIN_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t  testPattern; /* 0: constant, 1: sequential, 2: random, 3: static array, 4: file, etc */
   void    *staticIn;
   void    *staticOut;
   uint32_t dataType;
   uint32_t dataSize;
   uint32_t shift;              // 0-> for float, double, required for integer types
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} min_testParams_t;

void min_getTestParams(min_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_MIN_IXX_IXX_OXX_IDAT_H */
