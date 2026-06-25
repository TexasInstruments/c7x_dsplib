// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_cascadeBiquad_IDAT_H

#define DSPLIB_cascadeBiquad_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t  testPattern; /* 0: constant, 1: sequential, 2: random, 3: static array, 4: file, etc */
   void    *staticIn;
   void    *staticFilterCoeff;
   void    *staticOut;
   void    *staticFilterCoeffOpt;
   uint32_t dataType;
   uint32_t dataSize;
   uint32_t numChannels;
   uint32_t numStages;
   uint32_t biquadForm;
   uint32_t dataPitch;
   uint32_t filterVarPitch;
   uint32_t enableNChCoeffs;
   uint32_t enableCalcCoeffs;
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numExecReps;
   uint32_t numReps;
   uint32_t testID;
} DSPLIB_cascadeBiquad_testParams_t;

void DSPLIB_cascadeBiquad_getTestParams(DSPLIB_cascadeBiquad_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_cascadeBiquad_IDAT_H */
