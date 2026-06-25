// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_LUD_IXX_IXX_OXX_IDAT_H

#define DSPLIB_LUD_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

#if defined(__C7504__) || defined(__C7524__)
#define OUTPUT_ALIGN_BYTES DSPLIB_ALIGN_SHIFT_64BYTES
#else
#define OUTPUT_ALIGN_BYTES DSPLIB_ALIGN_SHIFT_128BYTES
#endif

typedef struct {
   uint8_t  testPattern; /* 0: constant, 1: sequential, 2: sequential_negative, 3: random etc.*/
   void    *staticInA;
   void    *staticInB;
   void    *staticOutP;
   void    *staticOutL;
   void    *staticOutU;
   void    *staticOutX;
   void    *staticOutInvA;
   uint32_t dataType;
   int32_t  order;
   int32_t  strideOrder;
   int32_t  strideP;
   uint32_t shift;              // 0-> for float, double, required for integer types
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} DSPLIB_lud_testParams_t;

void DSPLIB_lud_getTestParams(DSPLIB_lud_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_LUD_IXX_IXX_OXX_IDAT_H */
