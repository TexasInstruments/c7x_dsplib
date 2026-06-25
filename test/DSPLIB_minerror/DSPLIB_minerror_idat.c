// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_minerror_idat.h"
#define ALIGN_64BYTES DSPLIB_ALIGN_SHIFT_64BYTES

static minerror_testParams_t testParams[] = {
/********************************************
 {
    testPattern,
    staticIn,
    staticFilter,
    staticOut,
    dataType,
    vecInSize,
    errCoefsSize,
    strideIn,
    shift,
    outputDataLocation,
    numReps,
    testID
 },
 *********************************************/

#if (defined(ALL_TEST_CASES) || (TEST_CASE == 1) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_FLOAT32,                                         // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int32_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        1                                                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 2) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_FLOAT64,                                         // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int64_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        2                                                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 3) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_INT32,                                           // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int32_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        3                                                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 4) || (TEST_CATEGORY == 100))
    {
        SEQUENTIAL,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_INT32,                                           // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int32_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        4                                                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 5) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_INT64,                                           // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int64_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        5                                                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 6) || (TEST_CATEGORY == 100))
    {
        SEQUENTIAL,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_INT64,                                           // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int64_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        6                                                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 7) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_UINT32,                                          // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int32_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        7                                                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 8) || (TEST_CATEGORY == 100))
    {
        SEQUENTIAL,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_UINT32,                                          // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int32_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        8                                                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 9) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_UINT64,                                          // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int64_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        9                                                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 10) || (TEST_CATEGORY == 100))
    {
        SEQUENTIAL,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_UINT64,                                          // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int64_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        10                                                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 11) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                  //*staticIn
        NULL,                                                  //*staticFilter
        NULL,                                                  //*staticOut
        DSPLIB_INT8,                                           // dataType
        256,                                                   // vecInSize
        9,                                                     // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int8_t), ALIGN_64BYTES), // strideIn
        0,                                                     // shift
        DSPLIB_TEST_OUTPUT_MSMC,                               // outputDataLocation
        10,                                                    // numReps
        11                                                     // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 12) || (TEST_CATEGORY == 100))
    {
        SEQUENTIAL,
        NULL,                                                  //*staticIn
        NULL,                                                  //*staticFilter
        NULL,                                                  //*staticOut
        DSPLIB_INT8,                                           // dataType
        256,                                                   // vecInSize
        9,                                                     // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int8_t), ALIGN_64BYTES), // strideIn
        0,                                                     // shift
        DSPLIB_TEST_OUTPUT_MSMC,                               // outputDataLocation
        10,                                                    // numReps
        12                                                     // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 13) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                  //*staticIn
        NULL,                                                  //*staticFilter
        NULL,                                                  //*staticOut
        DSPLIB_UINT8,                                          // dataType
        256,                                                   // vecInSize
        9,                                                     // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int8_t), ALIGN_64BYTES), // strideIn
        0,                                                     // shift
        DSPLIB_TEST_OUTPUT_MSMC,                               // outputDataLocation
        10,                                                    // numReps
        13                                                     // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 14) || (TEST_CATEGORY == 100))
    {
        SEQUENTIAL,
        NULL,                                                  //*staticIn
        NULL,                                                  //*staticFilter
        NULL,                                                  //*staticOut
        DSPLIB_UINT8,                                          // dataType
        256,                                                   // vecInSize
        9,                                                     // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int8_t), ALIGN_64BYTES), // strideIn
        0,                                                     // shift
        DSPLIB_TEST_OUTPUT_MSMC,                               // outputDataLocation
        10,                                                    // numReps
        14                                                     // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 15) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_INT16,                                           // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int16_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        15                                                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 16) || (TEST_CATEGORY == 100))
    {
        SEQUENTIAL,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_INT16,                                           // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int16_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        16                                                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 17) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_UINT16,                                          // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int16_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        17                                                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 18) || (TEST_CATEGORY == 100))
    {
        SEQUENTIAL,
        NULL,                                                   //*staticIn
        NULL,                                                   //*staticFilter
        NULL,                                                   //*staticOut
        DSPLIB_UINT16,                                          // dataType
        256,                                                    // vecInSize
        9,                                                      // errCoefsSize
        DSPLIB_CALC_STRIDE(9 * sizeof(int16_t), ALIGN_64BYTES), // strideIn
        0,                                                      // shift
        DSPLIB_TEST_OUTPUT_MSMC,                                // outputDataLocation
        10,                                                     // numReps
        18                                                      // testID
    },
#endif
};

/*
 *  Sends the test parameter structure and number of tests
 */
void minerror_getTestParams(minerror_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(minerror_testParams_t);
}

/* ======================================================================== */
