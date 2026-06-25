// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blk_eswap_idat.h"

static blk_eswap_testParams_t testParams[] = {
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
        DSPLIB_INT16,            // dataType
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
        DSPLIB_INT16,            // dataType
        DSPLIB_INT16,            // dataType
        4,                       // dataSize
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
        DSPLIB_INT16,            // dataType
        4,                       // dataSize
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
        DSPLIB_INT16,            // dataType
        8,                       // dataSize
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
        DSPLIB_INT16,            // dataType
        18,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        32,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        64,                      // dataSize
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
        DSPLIB_INT16,            // dataType
        128,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        257,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        514,                     // dataSize
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
        DSPLIB_INT16,            // dataType
        1024,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        2050,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        4098,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        8194,                    // dataSize
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
        DSPLIB_INT16,            // dataType
        16384,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        32768,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        65536,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        131074,                  // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        2,                       // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        3,                       // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        5,                       // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        9,                       // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        17,                      // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        32,                      // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        64,                      // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        130,                     // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        257,                     // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        514,                     // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        1024,                    // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        2050,                    // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        4096,                    // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        8194,                    // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        16384,                   // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        32768,                   // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        65537,                   // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        1,                       // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        2,                       // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        6,                       // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        10,                      // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        18,                      // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        32,                      // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        65,                      // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        128,                     // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        256,                     // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        513,                     // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        1026,                    // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        2048,                    // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        4096,                    // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        8194,                    // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        16385,                   // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        32768,                   // dataSize
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
        DSPLIB_UINT16,           // dataType
        DSPLIB_UINT16,           // dataType
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
        DSPLIB_UINT16,           // dataType
        DSPLIB_UINT16,           // dataType
        3,                       // dataSize
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
        DSPLIB_UINT16,           // dataType
        DSPLIB_UINT16,           // dataType
        6,                       // dataSize
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
        DSPLIB_UINT16,           // dataType
        10,                      // dataSize
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
        DSPLIB_UINT16,           // dataType
        16,                      // dataSize
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
        DSPLIB_UINT16,           // dataType
        DSPLIB_UINT16,           // dataType
        64,                      // dataSize
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
        DSPLIB_UINT16,           // dataType
        130,                     // dataSize
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
        DSPLIB_UINT16,           // dataType
        258,                     // dataSize
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
        DSPLIB_UINT16,           // dataType
        514,                     // dataSize
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
        DSPLIB_UINT16,           // dataType
        1024,                    // dataSize
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
        DSPLIB_UINT16,           // dataType
        2049,                    // dataSize
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
        DSPLIB_UINT16,           // dataType
        4096,                    // dataSize
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
        DSPLIB_UINT16,           // dataType
        8193,                    // dataSize
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
        DSPLIB_UINT16,           // dataType
        16385,                   // dataSize
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
        DSPLIB_UINT16,           // dataType
        32768,                   // dataSize
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
        DSPLIB_UINT16,           // dataType
        65538,                   // dataSize
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
        DSPLIB_UINT16,           // dataType
        131074,                  // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        2,                       // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        4,                       // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        5,                       // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        9,                       // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        17,                      // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        34,                      // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        64,                      // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        129,                     // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        257,                     // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        512,                     // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        1026,                    // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        2049,                    // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        4097,                    // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        8193,                    // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        16385,                   // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        32769,                   // dataSize
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
        DSPLIB_UINT32,           // dataType
        DSPLIB_UINT32,           // dataType
        65538,                   // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        2,                       // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        4,                       // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        5,                       // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        8,                       // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        17,                      // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        32,                      // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        64,                      // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        130,                     // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        258,                     // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        514,                     // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        1024,                    // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        2050,                    // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        4098,                    // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        8192,                    // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        16384,                   // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        32769,                   // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        3,                       // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        3,                       // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        4,                       // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        9,                       // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        18,                      // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        34,                      // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        66,                      // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        130,                     // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        258,                     // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        512,                     // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        1025,                    // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        2048,                    // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        4098,                    // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        8193,                    // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        16385,                   // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        32768,                   // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        65538,                   // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        2,                       // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        2,                       // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        5,                       // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        10,                      // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        18,                      // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        32,                      // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        65,                      // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        128,                     // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        256,                     // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        514,                     // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        1025,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        2048,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        4096,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        8192,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        16386,                   // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        32769,                   // dataSize
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
        DSPLIB_INT16,            // dataType
        DSPLIB_INT16,            // dataType
        32768,                   // dataSize
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
        DSPLIB_UINT16,           // dataType
        DSPLIB_UINT16,           // dataType
        32768,                   // dataSize
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
        DSPLIB_INT32,            // dataType
        DSPLIB_INT32,            // dataType
        32768,                   // dataSize
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
        DSPLIB_UINT32,           // dataType
        32768,                   // dataSize
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
        DSPLIB_INT64,            // dataType
        DSPLIB_INT64,            // dataType
        32768,                   // dataSize
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
        DSPLIB_UINT64,           // dataType
        DSPLIB_UINT64,           // dataType
        32768,                   // dataSize
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
        DSPLIB_FLOAT32,          // dataType
        DSPLIB_FLOAT32,          // dataType
        32768,                   // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        DSPLIB_FLOAT64,          // dataType
        32768,                   // dataSize
        0,                       // shift
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        143                      // testID
    }
#endif

};

/*
 *  Sends the test parameter structure and number of tests
 */
void blk_eswap_getTestParams(blk_eswap_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(blk_eswap_testParams_t);
}

/* ======================================================================== */
