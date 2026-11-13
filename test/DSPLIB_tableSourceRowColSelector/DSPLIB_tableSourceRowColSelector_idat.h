// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_IDAT_H
#define DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t  testPattern; /* 0: constant, 1: sequential, 2: sequential_negative, 3: random etc.*/
   void    *staticIn0;   // Indices
   void    *staticIn1;   // Table
   void    *staticOut;
   uint32_t dataType;
   uint32_t numRows;
   uint32_t numCols;
   uint32_t numIndices;
   uint32_t strideOut;          // Output stride in bytes
   uint32_t isColSelect;        // Column select, otherwise row select
   uint32_t isOutputInterleave; // Interleaved output, otherwise non-interleaved
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} DSPLIB_tableSourceRowColSelector_testParams_t;

void DSPLIB_tableSourceRowColSelector_getTestParams(DSPLIB_tableSourceRowColSelector_testParams_t **params,
                                                    int32_t                                        *numTests);

#endif /* define DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_IDAT_H */
