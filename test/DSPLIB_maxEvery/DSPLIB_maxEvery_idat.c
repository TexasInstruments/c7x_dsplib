// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_maxEvery_idat.h"

static maxEvery_testParams_t testParams[] = {
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
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 6) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*staticOut
        DSPLIB_FLOAT64,          // dataType
        256,                     // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        512,                     // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        1024,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        2048,                    // dataSize
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
        DSPLIB_FLOAT64,          // dataType
        10240,                   // dataSize
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
};

/*
 *  Sends the test parameter structure and number of tests
 */
void maxEvery_getTestParams(maxEvery_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(maxEvery_testParams_t);
}

/* ======================================================================== */
