// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_maxIndex_idat.h"

static maxIndex_testParams_t testParams[] = {
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
        DSPLIB_FLOAT32,          // dataType
        256,                     // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        512,                     // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        1024,                    // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        2048,                    // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        10240,                   // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        5                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 11) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_INT32,            // dataType
        256,                     // dataSize
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
        DSPLIB_INT32,            // dataType
        512,                     // dataSize
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
        DSPLIB_INT32,            // dataType
        1024,                    // dataSize
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
        DSPLIB_UINT32,           // dataType
        2048,                    // dataSize
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
        DSPLIB_UINT32,           // dataType
        10240,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        256,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        512,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        1024,                    // dataSize
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
        DSPLIB_UINT16,           // dataType
        2048,                    // dataSize
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
        DSPLIB_UINT16,           // dataType
        10240,                   // dataSize
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
        DSPLIB_INT8,             // dataType
        256,                     // dataSize
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
        DSPLIB_INT8,             // dataType
        512,                     // dataSize
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
        DSPLIB_INT8,             // dataType
        1024,                    // dataSize
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
        2048,                    // dataSize
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
        10240,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        20,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        75,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        1024,                    // dataSize
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
        DSPLIB_UINT16,           // dataType
        1160,                    // dataSize
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
        DSPLIB_UINT16,           // dataType
        1178,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        256,                     // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        512,                     // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        1024,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        2048,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        16,                      // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        70,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        75,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        33,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        125,                     // dataSize
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
        DSPLIB_UINT16,           // dataType
        75,                      // dataSize
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
        DSPLIB_UINT16,           // dataType
        33,                      // dataSize
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
        DSPLIB_UINT16,           // dataType
        125,                     // dataSize
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
        DSPLIB_INT32,            // dataType
        33,                      // dataSize
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
        DSPLIB_INT32,            // dataType
        63,                      // dataSize
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
        DSPLIB_INT32,            // dataType
        17,                      // dataSize
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
        DSPLIB_UINT32,           // dataType
        33,                      // dataSize
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
        DSPLIB_UINT32,           // dataType
        63,                      // dataSize
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
        DSPLIB_UINT32,           // dataType
        17,                      // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        33,                      // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        63,                      // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        17,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        3,                       // dataSize
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
        DSPLIB_INT16,            // dataType
        17,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        33,                      // dataSize
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
        DSPLIB_UINT16,           // dataType
        3,                       // dataSize
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
        DSPLIB_UINT16,           // dataType
        17,                      // dataSize
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
        33,                      // dataSize
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
        DSPLIB_INT8,             // dataType
        3,                       // dataSize
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
        DSPLIB_INT8,             // dataType
        33,                      // dataSize
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
        DSPLIB_INT8,             // dataType
        65,                      // dataSize
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
        DSPLIB_UINT8,            // dataType
        3,                       // dataSize
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
        DSPLIB_UINT8,            // dataType
        33,                      // dataSize
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
        DSPLIB_UINT8,            // dataType
        65,                      // dataSize
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
        DSPLIB_INT32,            // dataType
        3,                       // dataSize
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
        DSPLIB_INT32,            // dataType
        9,                       // dataSize
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
        DSPLIB_INT32,            // dataType
        17,                      // dataSize
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
        DSPLIB_UINT32,           // dataType
        3,                       // dataSize
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
        DSPLIB_UINT32,           // dataType
        9,                       // dataSize
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
        DSPLIB_UINT32,           // dataType
        17,                      // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        3,                       // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        9,                       // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        17,                      // dataSize
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
        DSPLIB_INT32,            // dataType
        20,                      // dataSize
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
        DSPLIB_INT8,             // dataType
        255,                     // dataSize
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
        DSPLIB_INT8,             // dataType
        156,                     // dataSize
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
        DSPLIB_INT8,             // dataType
        65,                      // dataSize
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
        DSPLIB_UINT8,            // dataType
        255,                     // dataSize
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
        DSPLIB_UINT8,            // dataType
        156,                     // dataSize
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
        DSPLIB_UINT8,            // dataType
        65,                      // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        3,                       // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        7,                       // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        11,                      // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        15,                      // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        31,                      // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        84                       // testID
    },
#endif

};
/*
 *  Sends the test parameter structure and number of tests
 */
void maxIndex_getTestParams(maxIndex_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(maxIndex_testParams_t);
}

/* ======================================================================== */
