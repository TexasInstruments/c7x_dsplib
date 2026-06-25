// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_q15tofl_idat.h"

static q15tofl_testParams_t testParams[] = {
/********************************************
 {
 testPattern,
 staticIn,
 staticOut,
 dataTypeIn,
 dataTypeOut,
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        19,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        121,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        128,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        251,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        256,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        264,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        367,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        512,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        516,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        1024,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        1152,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        1847,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        2381,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        3001,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        4091,                    // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        5381,                    // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        6250,                    // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        7210,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        8192,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        8527,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        12600,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        15299,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        19577,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        27457,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT32,          // dataType
        32000,                   // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        32768,                   // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        51200,                   // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        19,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        121,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        128,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        251,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        256,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        264,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        367,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        512,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        516,                     // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        1024,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        1152,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        1847,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        2381,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        3001,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        4091,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        5381,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        6250,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        7210,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        8192,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        8527,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        12600,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        15299,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        19577,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_FLOAT64,          // dataType
        27457,                   // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        32000,                   // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        32768,                   // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        51200,                   // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        54                       // testID
    },
#endif
};

/*
 *  Sends the test parameter structure and number of tests
 */
void q15tofl_getTestParams(q15tofl_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(q15tofl_testParams_t);
}

/* ======================================================================== */
