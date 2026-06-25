// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bexp_idat.h"

static bexp_testParams_t testParams[] = {
/********************************************
{
testPattern,
staticIn,
staticOut,
dataType,
dataSize,
shift,
outputDataLocation,
numReps, testID
},
*********************************************/

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 1) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        1,                       // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        1                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 2) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        16,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        2                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 3) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        27,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        3                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 4) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        22,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        4                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 5) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        17,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        5                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 6) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        21,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        6                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 7) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        256,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        7                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 8) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        426,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        8                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 9) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        421,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        9                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 10) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        487,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        10                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 11) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        407,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        11                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 12) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        444,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        12                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 13) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        460,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        13                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 14) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        324,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        14                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 15) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        495,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        15                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 16) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        4096,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        16                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 17) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        4622,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        17                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 18) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        5112,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        18                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 19) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        6876,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        19                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 20) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        7138,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        20                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 21) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        5392,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        21                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 22) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        6912,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        22                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 23) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        6554,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        23                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 24) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        6715,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        24                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 25) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        6094,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        25                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 26) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        5819,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        26                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 27) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        4727,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        27                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 28) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT8,            // dataType
        6721,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        28                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 29) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        1,                       // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        29                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 30) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        16,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        30                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 31) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        23,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        31                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 32) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        21,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        32                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 33) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        23,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        33                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 34) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        29,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        34                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 35) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        256,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        35                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 36) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        409,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        36                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 37) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        423,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        37                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 38) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        381,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        38                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 39) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        358,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        39                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 40) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        511,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        40                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 41) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        433,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        41                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 42) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        429,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        42                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 43) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        362,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        43                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 44) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        4096,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        44                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 45) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        5675,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        45                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 46) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        6410,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        46                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 47) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        5433,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        47                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 48) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        7581,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        48                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 49) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        5803,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        49                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 50) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        7374,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        50                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 51) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        6360,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        51                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 52) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        7741,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        52                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 53) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        5067,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        53                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 54) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        7610,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        54                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 55) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        5846,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        55                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 56) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT8,             // dataType
        5664,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        56                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 57) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        1,                       // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        57                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 58) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        16,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        58                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 59) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        20,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        59                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 60) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        31,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        60                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 61) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        27,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        61                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 62) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        23,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        62                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 63) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        256,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        63                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 64) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        508,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        64                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 65) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        477,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        65                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 66) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        490,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        66                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 67) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        311,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        67                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 68) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        452,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        68                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 69) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        415,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        69                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 70) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        272,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        70                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 71) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        285,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        71                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 72) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        4096,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        72                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 73) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        4168,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        73                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 74) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        6472,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        74                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 75) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        4868,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        75                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 76) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        5903,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        76                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 77) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        6103,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        77                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 78) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        6530,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        78                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 79) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        4947,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        79                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 80) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        5467,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        80                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 81) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        5943,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        81                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 82) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        5717,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        82                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 83) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        6868,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        83                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 84) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT16,           // dataType
        7046,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        84                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 85) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        1,                       // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        85                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 86) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        16,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        86                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 87) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        25,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        87                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 88) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        28,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        88                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 89) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        20,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        89                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 90) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        19,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        90                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 91) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        256,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        91                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 92) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        335,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        92                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 93) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        330,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        93                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 94) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        392,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        94                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 95) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        510,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        95                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 96) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        499,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        96                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 97) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        387,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        97                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 98) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        307,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        98                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 99) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        493,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        99                       // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 100) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        4096,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        100                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 101) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        6594,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        101                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 102) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        4525,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        102                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 103) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        4184,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        103                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 104) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        6277,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        104                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 105) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        8012,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        105                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 106) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        7440,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        106                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 107) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        7518,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        107                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 108) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        4940,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        108                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 109) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        7025,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        109                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 110) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        6851,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        110                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 111) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        6453,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        111                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 112) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT16,            // dataType
        6432,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        112                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 113) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        1,                       // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        113                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 114) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        16,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        114                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 115) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        20,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        115                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 116) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        17,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        116                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 117) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        31,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        117                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 118) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        21,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        118                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 119) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        256,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        119                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 120) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        323,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        120                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 121) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        336,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        121                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 122) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        468,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        122                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 123) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        318,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        123                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 124) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        297,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        124                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 125) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        383,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        125                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 126) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        476,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        126                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 127) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        345,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        127                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 128) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        4096,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        128                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 129) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        4481,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        129                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 130) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        4643,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        130                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 131) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        7250,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        131                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 132) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        4266,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        132                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 133) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        8004,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        133                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 134) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        6465,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        134                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 135) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        6200,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        135                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 136) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        6464,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        136                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 137) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        4254,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        137                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 138) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        7491,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        138                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 139) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        5362,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        139                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 140) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT32,           // dataType
        7062,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        140                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 141) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        1,                       // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        141                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 142) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        16,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        142                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 143) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        24,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        143                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 144) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        26,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        144                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 145) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        28,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        145                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 146) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        29,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        146                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 147) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        256,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        147                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 148) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        393,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        148                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 149) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        423,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        149                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 150) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        344,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        150                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 151) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        481,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        151                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 152) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        360,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        152                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 153) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        485,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        153                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 154) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        468,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        154                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 155) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        496,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        155                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 156) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        4096,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        156                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 157) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        7768,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        157                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 158) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        7446,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        158                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 159) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        6584,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        159                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 160) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        7846,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        160                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 161) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        5488,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        161                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 162) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        8105,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        162                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 163) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        4172,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        163                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 164) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        4781,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        164                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 165) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        7008,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        165                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 166) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        5871,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        166                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 167) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        7940,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        167                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 168) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        6932,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        168                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 169) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        1,                       // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        169                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 170) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        16,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        170                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 171) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        24,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        171                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 172) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        27,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        172                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 173) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        23,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        173                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 174) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        19,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        174                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 175) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        256,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        175                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 176) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        347,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        176                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 177) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        466,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        177                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 178) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        382,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        178                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 179) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        452,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        179                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 180) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        506,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        180                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 181) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        288,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        181                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 182) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        263,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        182                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 183) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        301,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        183                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 184) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        4096,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        184                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 185) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        6393,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        185                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 186) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        8151,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        186                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 187) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        4140,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        187                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 188) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        5605,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        188                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 189) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        7287,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        189                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 190) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        6763,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        190                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 191) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        6013,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        191                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 192) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        7605,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        192                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 193) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        4834,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        193                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 194) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        4284,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        194                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 195) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        4786,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        195                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 196) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT64,            // dataType
        5277,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        196                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 197) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        1,                       // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        197                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 198) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        16,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        198                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 199) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        18,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        199                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 200) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        17,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        200                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 201) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        21,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        201                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 202) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        16,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        202                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 203) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        256,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        203                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 204) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        317,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        204                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 205) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        440,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        205                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 206) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        318,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        206                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 207) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        444,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        207                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 208) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        418,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        208                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 209) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        449,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        209                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 210) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        411,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        210                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 211) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        282,                     // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        211                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 212) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        4096,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        212                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 213) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        5110,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        213                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 214) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        7734,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        214                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 215) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        5435,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        215                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 216) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        6835,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        216                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 217) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        7377,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        217                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 218) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        5294,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        218                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 219) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        5943,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        219                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 220) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        5255,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        220                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 221) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        5948,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        221                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 222) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        5885,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        222                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 223) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        5883,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        223                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 224) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_UINT64,           // dataType
        6729,                    // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        224                      // testID
    },
#endif

};

/*
 *  Sends the test parameter structure and number of tests
 */
void bexp_getTestParams(bexp_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(bexp_testParams_t);
}

/* ======================================================================== */
