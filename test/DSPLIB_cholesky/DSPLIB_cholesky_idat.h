// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_cholesky_IXX_IXX_OXX_IDAT_H

#define DSPLIB_cholesky_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
// #include "../../test/DSPLIB_test.h"
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t  testPatternA; /* 0: consaddt, 1: sequential, 2: random, 3: static array, 4: file, etc */
   uint8_t  testPatternB;
   void    *staticIn;
   void    *staticInB;
   void    *staticOut;
   void    *staticOutX;
   uint32_t dataType;
   uint32_t order;
   uint32_t enableTest;
   uint32_t strideIn;
   uint32_t strideOut;
   uint32_t shift;              // 0-> for float, double, required for integer types
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} DSPLIB_cholesky_testParams_t;

void DSPLIB_cholesky_getTestParams(DSPLIB_cholesky_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_add_IXX_IXX_OXX_IDAT_H */
