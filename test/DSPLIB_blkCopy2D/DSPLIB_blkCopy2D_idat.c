// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blkCopy2D_idat.h"

static blkCopy2D_testParams_t testParams[] = {
/********************************************
 {
 testPattern,
 staticIn,
 staticOut,
 dataType,
 width,
 height,
 strideIn,
 strideOut,
 outputDataLocation,
 numReps, testID
 },
 *********************************************/

/* Test cases for width = 1 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 1) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        1,                       // width
        1,                       // height
        1 * sizeof(float),       // strideIn (bytes)
        1 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        1,                       // width
        1,                       // height
        1 * sizeof(int32_t),     // strideIn (bytes)
        1 * sizeof(int32_t),     // strideOut (bytes)
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
        1,                       // width
        2,                       // height
        1 * sizeof(float),       // strideIn (bytes)
        1 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        1,                       // width
        2,                       // height
        1 * sizeof(int32_t),     // strideIn (bytes)
        1 * sizeof(int32_t),     // strideOut (bytes)
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
        1,                       // width
        4,                       // height
        1 * sizeof(float),       // strideIn (bytes)
        1 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        1,                       // width
        4,                       // height
        1 * sizeof(int32_t),     // strideIn (bytes)
        1 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        1,                       // width
        8,                       // height
        1 * sizeof(float),       // strideIn (bytes)
        1 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        1,                       // width
        8,                       // height
        1 * sizeof(int32_t),     // strideIn (bytes)
        1 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        1,                       // width
        16,                      // height
        1 * sizeof(float),       // strideIn (bytes)
        1 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        1,                       // width
        16,                      // height
        1 * sizeof(int32_t),     // strideIn (bytes)
        1 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        1,                       // width
        32,                      // height
        1 * sizeof(float),       // strideIn (bytes)
        1 * sizeof(float),       // strideOut (bytes)
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
        1,                       // width
        32,                      // height
        1 * sizeof(int32_t),     // strideIn (bytes)
        1 * sizeof(int32_t),     // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        12                       // testID
    },
#endif

/* Test cases for width = 2 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 13) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        2,                       // width
        1,                       // height
        2 * sizeof(float),       // strideIn (bytes)
        2 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        2,                       // width
        1,                       // height
        2 * sizeof(int32_t),     // strideIn (bytes)
        2 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        2,                       // width
        2,                       // height
        2 * sizeof(float),       // strideIn (bytes)
        2 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        2,                       // width
        2,                       // height
        2 * sizeof(int32_t),     // strideIn (bytes)
        2 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        2,                       // width
        4,                       // height
        2 * sizeof(float),       // strideIn (bytes)
        2 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        2,                       // width
        4,                       // height
        2 * sizeof(int32_t),     // strideIn (bytes)
        2 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        2,                       // width
        8,                       // height
        2 * sizeof(float),       // strideIn (bytes)
        2 * sizeof(float),       // strideOut (bytes)
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
        2,                       // width
        8,                       // height
        2 * sizeof(int32_t),     // strideIn (bytes)
        2 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        2,                       // width
        16,                      // height
        2 * sizeof(float),       // strideIn (bytes)
        2 * sizeof(float),       // strideOut (bytes)
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
        2,                       // width
        16,                      // height
        2 * sizeof(int32_t),     // strideIn (bytes)
        2 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        2,                       // width
        32,                      // height
        2 * sizeof(float),       // strideIn (bytes)
        2 * sizeof(float),       // strideOut (bytes)
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
        2,                       // width
        32,                      // height
        2 * sizeof(int32_t),     // strideIn (bytes)
        2 * sizeof(int32_t),     // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        24                       // testID
    },
#endif

/* Test cases for width = 4 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 25) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        4,                       // width
        1,                       // height
        4 * sizeof(float),       // strideIn (bytes)
        4 * sizeof(float),       // strideOut (bytes)
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
        4,                       // width
        1,                       // height
        4 * sizeof(int32_t),     // strideIn (bytes)
        4 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        4,                       // width
        2,                       // height
        4 * sizeof(float),       // strideIn (bytes)
        4 * sizeof(float),       // strideOut (bytes)
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
        4,                       // width
        2,                       // height
        4 * sizeof(int32_t),     // strideIn (bytes)
        4 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        4,                       // width
        4,                       // height
        4 * sizeof(float),       // strideIn (bytes)
        4 * sizeof(float),       // strideOut (bytes)
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
        4,                       // width
        4,                       // height
        4 * sizeof(int32_t),     // strideIn (bytes)
        4 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        4,                       // width
        8,                       // height
        4 * sizeof(float),       // strideIn (bytes)
        4 * sizeof(float),       // strideOut (bytes)
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
        4,                       // width
        8,                       // height
        4 * sizeof(int32_t),     // strideIn (bytes)
        4 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        4,                       // width
        16,                      // height
        4 * sizeof(float),       // strideIn (bytes)
        4 * sizeof(float),       // strideOut (bytes)
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
        4,                       // width
        16,                      // height
        4 * sizeof(int32_t),     // strideIn (bytes)
        4 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        4,                       // width
        32,                      // height
        4 * sizeof(float),       // strideIn (bytes)
        4 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        4,                       // width
        32,                      // height
        4 * sizeof(int32_t),     // strideIn (bytes)
        4 * sizeof(int32_t),     // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        36                       // testID
    },
#endif

/* Test cases for width = 6 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 37) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        6,                       // width
        1,                       // height
        6 * sizeof(float),       // strideIn (bytes)
        6 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        6,                       // width
        1,                       // height
        6 * sizeof(int32_t),     // strideIn (bytes)
        6 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        6,                       // width
        2,                       // height
        6 * sizeof(float),       // strideIn (bytes)
        6 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        6,                       // width
        2,                       // height
        6 * sizeof(int32_t),     // strideIn (bytes)
        6 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        6,                       // width
        4,                       // height
        6 * sizeof(float),       // strideIn (bytes)
        6 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        6,                       // width
        4,                       // height
        6 * sizeof(int32_t),     // strideIn (bytes)
        6 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        6,                       // width
        8,                       // height
        6 * sizeof(float),       // strideIn (bytes)
        6 * sizeof(float),       // strideOut (bytes)
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
        6,                       // width
        8,                       // height
        6 * sizeof(int32_t),     // strideIn (bytes)
        6 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        6,                       // width
        16,                      // height
        6 * sizeof(float),       // strideIn (bytes)
        6 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        6,                       // width
        16,                      // height
        6 * sizeof(int32_t),     // strideIn (bytes)
        6 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        6,                       // width
        32,                      // height
        6 * sizeof(float),       // strideIn (bytes)
        6 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        6,                       // width
        32,                      // height
        6 * sizeof(int32_t),     // strideIn (bytes)
        6 * sizeof(int32_t),     // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        48                       // testID
    },
#endif

/* Test cases for width = 8 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 49) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        8,                       // width
        1,                       // height
        8 * sizeof(float),       // strideIn (bytes)
        8 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        8,                       // width
        1,                       // height
        8 * sizeof(int32_t),     // strideIn (bytes)
        8 * sizeof(int32_t),     // strideOut (bytes)
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
        8,                       // width
        2,                       // height
        8 * sizeof(float),       // strideIn (bytes)
        8 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        8,                       // width
        2,                       // height
        8 * sizeof(int32_t),     // strideIn (bytes)
        8 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        8,                       // width
        4,                       // height
        8 * sizeof(float),       // strideIn (bytes)
        8 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        8,                       // width
        4,                       // height
        8 * sizeof(int32_t),     // strideIn (bytes)
        8 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        8,                       // width
        8,                       // height
        8 * sizeof(float),       // strideIn (bytes)
        8 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        8,                       // width
        8,                       // height
        8 * sizeof(int32_t),     // strideIn (bytes)
        8 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        8,                       // width
        16,                      // height
        8 * sizeof(float),       // strideIn (bytes)
        8 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        8,                       // width
        16,                      // height
        8 * sizeof(int32_t),     // strideIn (bytes)
        8 * sizeof(int32_t),     // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        8,                       // width
        32,                      // height
        8 * sizeof(float),       // strideIn (bytes)
        8 * sizeof(float),       // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        8,                       // width
        32,                      // height
        8 * sizeof(int32_t),     // strideIn (bytes)
        8 * sizeof(int32_t),     // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        60                       // testID
    },
#endif

/* Test cases for width = 10 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 61) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        10,                      // width
        1,                       // height
        10 * sizeof(float),      // strideIn (bytes)
        10 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        10,                      // width
        1,                       // height
        10 * sizeof(int32_t),    // strideIn (bytes)
        10 * sizeof(int32_t),    // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        10,                      // width
        2,                       // height
        10 * sizeof(float),      // strideIn (bytes)
        10 * sizeof(float),      // strideOut (bytes)
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
        10,                      // width
        2,                       // height
        10 * sizeof(int32_t),    // strideIn (bytes)
        10 * sizeof(int32_t),    // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        10,                      // width
        4,                       // height
        10 * sizeof(float),      // strideIn (bytes)
        10 * sizeof(float),      // strideOut (bytes)
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
        10,                      // width
        4,                       // height
        10 * sizeof(int32_t),    // strideIn (bytes)
        10 * sizeof(int32_t),    // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        10,                      // width
        8,                       // height
        10 * sizeof(float),      // strideIn (bytes)
        10 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        10,                      // width
        8,                       // height
        10 * sizeof(int32_t),    // strideIn (bytes)
        10 * sizeof(int32_t),    // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        10,                      // width
        16,                      // height
        10 * sizeof(float),      // strideIn (bytes)
        10 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        10,                      // width
        16,                      // height
        10 * sizeof(int32_t),    // strideIn (bytes)
        10 * sizeof(int32_t),    // strideOut (bytes)
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
        10,                      // width
        32,                      // height
        10 * sizeof(float),      // strideIn (bytes)
        10 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        10,                      // width
        32,                      // height
        10 * sizeof(int32_t),    // strideIn (bytes)
        10 * sizeof(int32_t),    // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        72                       // testID
    },
#endif

/* Test cases for width = 12 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 73) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        12,                      // width
        1,                       // height
        12 * sizeof(float),      // strideIn (bytes)
        12 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        12,                      // width
        1,                       // height
        12 * sizeof(int32_t),    // strideIn (bytes)
        12 * sizeof(int32_t),    // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        12,                      // width
        2,                       // height
        12 * sizeof(float),      // strideIn (bytes)
        12 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        12,                      // width
        2,                       // height
        12 * sizeof(int32_t),    // strideIn (bytes)
        12 * sizeof(int32_t),    // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        12,                      // width
        4,                       // height
        12 * sizeof(float),      // strideIn (bytes)
        12 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        12,                      // width
        4,                       // height
        12 * sizeof(int32_t),    // strideIn (bytes)
        12 * sizeof(int32_t),    // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        12,                      // width
        8,                       // height
        12 * sizeof(float),      // strideIn (bytes)
        12 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        12,                      // width
        8,                       // height
        12 * sizeof(int32_t),    // strideIn (bytes)
        12 * sizeof(int32_t),    // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        12,                      // width
        16,                      // height
        12 * sizeof(float),      // strideIn (bytes)
        12 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        12,                      // width
        16,                      // height
        12 * sizeof(int32_t),    // strideIn (bytes)
        12 * sizeof(int32_t),    // strideOut (bytes)
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
        DSPLIB_FLOAT32,          // dataType
        12,                      // width
        32,                      // height
        12 * sizeof(float),      // strideIn (bytes)
        12 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        12,                      // width
        32,                      // height
        12 * sizeof(int32_t),    // strideIn (bytes)
        12 * sizeof(int32_t),    // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        84                       // testID
    },
#endif

/* Test cases for width = 16 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 85) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        16,                      // width
        2,                       // height
        16 * sizeof(float),      // strideIn (bytes)
        16 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_FLOAT64,          // dataType
        16,                      // width
        2,                       // height
        16 * sizeof(double),     // strideIn (bytes)
        16 * sizeof(double),     // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        16,                      // width
        2,                       // height
        16 * sizeof(int32_t),    // strideIn (bytes)
        16 * sizeof(int32_t),    // strideOut (bytes)
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
        16,                      // width
        2,                       // height
        16 * sizeof(int16_t),    // strideIn (bytes)
        16 * sizeof(int16_t),    // strideOut (bytes)
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
        DSPLIB_INT8,             // dataType
        16,                      // width
        2,                       // height
        16 * sizeof(int8_t),     // strideIn (bytes)
        16 * sizeof(int8_t),     // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        89                       // testID
    },
#endif

/* Test cases for width = 32 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 90) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        32,                      // width
        2,                       // height
        32 * sizeof(float),      // strideIn (bytes)
        32 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_FLOAT64,          // dataType
        32,                      // width
        2,                       // height
        32 * sizeof(double),     // strideIn (bytes)
        32 * sizeof(double),     // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        32,                      // width
        2,                       // height
        32 * sizeof(int32_t),    // strideIn (bytes)
        32 * sizeof(int32_t),    // strideOut (bytes)
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
        32,                      // width
        2,                       // height
        32 * sizeof(int16_t),    // strideIn (bytes)
        32 * sizeof(int16_t),    // strideOut (bytes)
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
        DSPLIB_INT8,             // dataType
        32,                      // width
        2,                       // height
        32 * sizeof(int8_t),     // strideIn (bytes)
        32 * sizeof(int8_t),     // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        94                       // testID
    },
#endif

/* Test cases for width = 64 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 95) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        64,                      // width
        4,                       // height
        64 * sizeof(float),      // strideIn (bytes)
        64 * sizeof(float),      // strideOut (bytes)
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
        DSPLIB_FLOAT64,          // dataType
        64,                      // width
        4,                       // height
        64 * sizeof(double),     // strideIn (bytes)
        64 * sizeof(double),     // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        64,                      // width
        4,                       // height
        64 * sizeof(int32_t),    // strideIn (bytes)
        64 * sizeof(int32_t),    // strideOut (bytes)
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
        64,                      // width
        4,                       // height
        64 * sizeof(int16_t),    // strideIn (bytes)
        64 * sizeof(int16_t),    // strideOut (bytes)
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
        DSPLIB_INT8,             // dataType
        64,                      // width
        4,                       // height
        64 * sizeof(int8_t),     // strideIn (bytes)
        64 * sizeof(int8_t),     // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        99                       // testID
    },
#endif

/* Test cases for width = 128 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 100) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        128,                     // width
        8,                       // height
        128 * sizeof(float),     // strideIn (bytes)
        128 * sizeof(float),     // strideOut (bytes)
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
        DSPLIB_FLOAT64,          // dataType
        128,                     // width
        8,                       // height
        128 * sizeof(double),    // strideIn (bytes)
        128 * sizeof(double),    // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        128,                     // width
        8,                       // height
        128 * sizeof(int32_t),   // strideIn (bytes)
        128 * sizeof(int32_t),   // strideOut (bytes)
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
        128,                     // width
        8,                       // height
        128 * sizeof(int16_t),   // strideIn (bytes)
        128 * sizeof(int16_t),   // strideOut (bytes)
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
        DSPLIB_INT8,             // dataType
        128,                     // width
        8,                       // height
        128 * sizeof(int8_t),    // strideIn (bytes)
        128 * sizeof(int8_t),    // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        104                      // testID
    },
#endif

/* Test cases for width = 256 */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 105) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        256,                     // width
        16,                      // height
        256 * sizeof(float),     // strideIn (bytes)
        256 * sizeof(float),     // strideOut (bytes)
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
        DSPLIB_FLOAT64,          // dataType
        256,                     // width
        16,                      // height
        256 * sizeof(double),    // strideIn (bytes)
        256 * sizeof(double),    // strideOut (bytes)
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
        DSPLIB_INT32,            // dataType
        256,                     // width
        16,                      // height
        256 * sizeof(int32_t),   // strideIn (bytes)
        256 * sizeof(int32_t),   // strideOut (bytes)
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
        256,                     // width
        16,                      // height
        256 * sizeof(int16_t),   // strideIn (bytes)
        256 * sizeof(int16_t),   // strideOut (bytes)
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
        DSPLIB_INT8,             // dataType
        256,                     // width
        16,                      // height
        256 * sizeof(int8_t),    // strideIn (bytes)
        256 * sizeof(int8_t),    // strideOut (bytes)
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        109                      // testID
    },
#endif

/* Custom stride test cases */
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 110) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                        //*staticIn
        NULL,                        //*staticOut
        DSPLIB_INT32,                // dataType
        64,                          // width
        8,                           // height
        (64 * sizeof(int32_t)) + 16, // strideIn (bytes) - with padding
        (64 * sizeof(int32_t)) + 16, // strideOut (bytes) - with padding
        DSPLIB_TEST_OUTPUT_MSMC,     // outputDataLocation
        1,                           // numReps
        110                          // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 111) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                      //*staticIn
        NULL,                      //*staticOut
        DSPLIB_FLOAT32,            // dataType
        32,                        // width
        4,                         // height
        (32 * sizeof(float)) + 32, // strideIn (bytes) - with padding
        (32 * sizeof(float)) + 64, // strideOut (bytes) - with different padding
        DSPLIB_TEST_OUTPUT_MSMC,   // outputDataLocation
        1,                         // numReps
        111                        // testID
    },
#endif
};

/*
 *  Sends the test parameter structure and number of tests
 */
void blkCopy2D_getTestParams(blkCopy2D_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(blkCopy2D_testParams_t);
}

/* ======================================================================== */
