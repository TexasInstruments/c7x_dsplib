// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_mul_IXX_IXX_OXX_IDAT_H

#define DSPLIB_mul_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t  testPattern;
   void    *staticIn1;          /* pointer to input vector 1 */
   void    *staticIn2;          /* pointer to input vector 2 */
   void    *staticOut;
   uint32_t dataType;
   uint32_t dataSize;
   uint32_t outputDataLocation;
   uint32_t numReps;
   uint32_t testID;
} DSPLIB_mul_testParams_t;

void DSPLIB_mul_getTestParams(DSPLIB_mul_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_mul_IXX_IXX_OXX_IDAT_H */
