// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_FLTOQ15_IXX_IXX_OXX_IDAT_H

#define DSPLIB_FLTOQ15_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t  testPattern; /* 0: constant, 1: sequential, 2: sequential_negative, 3: random etc.*/
   void    *staticIn;
   void    *staticOut;
   uint32_t dataTypeIn;
   uint32_t dataTypeOut;
   uint32_t dataSize;
   uint32_t shift;              // 0-> for float, double, required for integer types
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} fltoq15_testParams_t;

void fltoq15_getTestParams(fltoq15_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_FLTOQ15_IXX_IXX_OXX_IDAT_H */
