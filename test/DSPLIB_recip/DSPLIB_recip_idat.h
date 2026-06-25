// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_recip_IXX_IXX_OXX_IDAT_H

#define DSPLIB_recip_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t                 testPattern; /* 0: conssqrt, 1: sequential, 2: random, 3: static array, 4: file, etc */
   void                   *staticIn;
   void                   *staticOut;
   uint32_t                dataType;
   uint32_t                dataSize;
   DSPLIB_recip_storeStyle storeStyle;
   uint32_t                outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t                numReps;
   uint32_t                testID;
} recip_testParams_t;

void recip_getTestParams(recip_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_recip_IXX_IXX_OXX_IDAT_H */
