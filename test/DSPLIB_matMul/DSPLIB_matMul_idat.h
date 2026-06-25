// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_matMul_IXX_IXX_OXX_IDAT_H

#define DSPLIB_matMul_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t testPattern; /* 0: consmatMult, 1: sequential, 2: random, 3: static
                           array, 4: file, etc */
   void    *staticIn0;
   void    *staticIn1;
   void    *staticOut;
   uint32_t M;
   uint32_t N;
   uint32_t K;
   uint16_t strideIn0;
   uint16_t strideIn1;
   uint16_t strideOut;
   uint32_t shift;
   uint32_t dataType;
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} DSPLIB_matMul_testParams_t;

void DSPLIB_matMul_getTestParams(DSPLIB_matMul_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_matMul_IXX_IXX_OXX_IDAT_H */
