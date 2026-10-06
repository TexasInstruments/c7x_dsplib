// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0


#ifndef DSPLIB_COMPLEXMATRIXMULTIPLY_IXX_IXX_OXX_IDAT_H
#define DSPLIB_COMPLEXMATRIXMULTIPLY_IXX_IXX_OXX_IDAT_H

#include "../common/DSPLIB_test.h"
#include "DSPLIB_types.h"
#include <dsplib.h>
#include "DSPLIB_complexMatrixMultiply.h"

typedef struct
{
    uint8_t testPattern; /* 0: consaddt, 1: sequential, 2: random, 3: static
                            array, 4: file, etc */
    uint32_t dType;
    void *staticInA;
    void *staticInB;
    void *params;
    void *staticOut;
    size_t M;
    size_t K;
    size_t N;
    size_t nBatch;
    uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
    uint32_t numReps;
    uint32_t testID;
} DSPLIB_complexMatrixMultiply_testParams_t;

void DSPLIB_complexMatrixMultiply_getTestParams(DSPLIB_complexMatrixMultiply_testParams_t **params, int32_t *numTests);

#endif // DSPLIB_COMPLEXMATRIXMULTIPLY_IXX_IXX_OXX_IDAT_H
