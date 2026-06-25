// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_qrd_IXX_IXX_OXX_IDAT_H

#define DSPLIB_qrd_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>
// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// #define ENABLE_REDUCED_FORM 1

typedef struct {
   uint8_t  testPattern; /* 0: constant, 1: sequential, 2: sequential_negative, 3: random etc.*/
   void    *staticInA;
   void    *staticInB;
   void    *staticOutQ;
   void    *staticOutR;
   void    *staticOutX;
   void    *staticOutInvA;
   uint32_t dataType;
   uint32_t widthIn;
   uint32_t heightIn;
   int32_t  strideIn;
   uint32_t shift;              // 0-> for float, double, required for integer types
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} DSPLIB_qrd_testParams_t;

void DSPLIB_qrd_getTestParams(DSPLIB_qrd_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_qrd_IXX_IXX_OXX_IDAT_H */
