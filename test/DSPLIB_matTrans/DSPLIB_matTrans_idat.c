// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_matTrans_idat.h"
#define ALIGN_64BYTES DSPLIB_ALIGN_SHIFT_64BYTES

#if defined(__C7504__) || defined(__C7524__)
#define OUTPUT_ALIGN_BYTES DSPLIB_ALIGN_SHIFT_64BYTES
#else
#define OUTPUT_ALIGN_BYTES DSPLIB_ALIGN_SHIFT_128BYTES
#endif

static matTrans_testParams_t testParams[] =

    {
/*********************************************************************************************
{
  testPattern;
  *staticIn;
  *staticOut;
  dataType;
  widthIn;
  heightIn;
  strideIn;
  widthOut;
  heightOut;
  strideOut;
  shift;
  outputDataLocation;
  numReps;
  testID;
},
 *********************************************************************************************/
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 1) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_INT8,                                                // dataType
            1,                                                          // widthIn
            1,                                                          // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            1,                                                          // widthOut
            1,                                                          // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            1                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 2) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_INT8,                                                // dataType
            16,                                                         // widthIn
            1,                                                          // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(int8_t), ALIGN_64BYTES),     // strideIn
            1,                                                          // widthOut
            16,                                                         // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            2                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 3) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_INT8,                                                // dataType
            30,                                                         // widthIn
            2,                                                          // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(int8_t), ALIGN_64BYTES),     // strideIn
            2,                                                          // widthOut
            30,                                                         // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            3                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 4) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_INT8,                                                // dataType
            64,                                                         // widthIn
            4,                                                          // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int8_t), ALIGN_64BYTES),     // strideIn
            4,                                                          // widthOut
            64,                                                         // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            4                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 5) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_INT8,                                                // dataType
            160,                                                        // widthIn
            7,                                                          // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(int8_t), ALIGN_64BYTES),    // strideIn
            7,                                                          // widthOut
            160,                                                        // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            5                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 6) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_INT8,                                                // dataType
            288,                                                        // widthIn
            8,                                                          // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(int8_t), ALIGN_64BYTES),    // strideIn
            8,                                                          // widthOut
            288,                                                        // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            6                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 7) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT8,                                                 // dataType
            512,                                                         // widthIn
            16,                                                          // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(int8_t), ALIGN_64BYTES),     // strideIn
            16,                                                          // widthOut
            512,                                                         // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            7                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 8) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT8,                                                 // dataType
            1000,                                                        // widthIn
            24,                                                          // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(int8_t), ALIGN_64BYTES),    // strideIn
            24,                                                          // widthOut
            1000,                                                        // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            8                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 9) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT8,                                                 // dataType
            32,                                                          // widthIn
            32,                                                          // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            32,                                                          // widthOut
            32,                                                          // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            9                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 10) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT8,                                                 // dataType
            434,                                                         // widthIn
            39,                                                          // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(int8_t), ALIGN_64BYTES),     // strideIn
            39,                                                          // widthOut
            434,                                                         // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            10                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 11) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT8,                                                 // dataType
            192,                                                         // widthIn
            48,                                                          // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(int8_t), ALIGN_64BYTES),     // strideIn
            48,                                                          // widthOut
            192,                                                         // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            11                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 12) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT8,                                                 // dataType
            64,                                                          // widthIn
            64,                                                          // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            64,                                                          // widthOut
            64,                                                          // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            12                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 13) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT8,                                                 // dataType
            192,                                                         // widthIn
            96,                                                          // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(int8_t), ALIGN_64BYTES),     // strideIn
            96,                                                          // widthOut
            192,                                                         // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            13                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 14) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            128,                                                          // widthIn
            128,                                                          // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            128,                                                          // widthOut
            128,                                                          // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            14                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 15) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            112,                                                          // widthIn
            176,                                                          // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            176,                                                          // widthOut
            112,                                                          // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            15                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 16) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            132,                                                          // widthIn
            192,                                                          // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            192,                                                          // widthOut
            132,                                                          // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            16                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 17) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            224,                                                          // widthIn
            224,                                                          // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            224,                                                          // widthOut
            224,                                                          // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            17                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 18) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            256,                                                          // widthIn
            256,                                                          // heightIn
            DSPLIB_CALC_STRIDE(256 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            256,                                                          // widthOut
            256,                                                          // heightOut
            DSPLIB_CALC_STRIDE(256 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            18                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 19) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            52,                                                           // widthIn
            262,                                                          // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(int8_t), ALIGN_64BYTES),       // strideIn
            262,                                                          // widthOut
            52,                                                           // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            19                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 20) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            144,                                                          // widthIn
            312,                                                          // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            312,                                                          // widthOut
            144,                                                          // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            20                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 21) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            32,                                                           // widthIn
            344,                                                          // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int8_t), ALIGN_64BYTES),       // strideIn
            344,                                                          // widthOut
            32,                                                           // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            21                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 22) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            64,                                                           // widthIn
            377,                                                          // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int8_t), ALIGN_64BYTES),       // strideIn
            377,                                                          // widthOut
            64,                                                           // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            22                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 23) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_INT8,                                                        // dataType
            68,                                                                 // widthIn
            448,                                                                // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(int8_t), ALIGN_64BYTES),             // strideIn
            448,                                                                // widthOut
            68,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(int8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            23                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 24) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_INT8,                                                        // dataType
            512,                                                                // widthIn
            512,                                                                // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(int8_t), ALIGN_64BYTES),            // strideIn
            512,                                                                // widthOut
            512,                                                                // heightOut
            DSPLIB_CALC_STRIDE(512 * sizeof(int8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            24                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 25) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            16,                                                           // widthIn
            640,                                                          // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(int8_t), ALIGN_64BYTES),       // strideIn
            640,                                                          // widthOut
            16,                                                           // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            25                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 26) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            128,                                                          // widthIn
            700,                                                          // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(int8_t), ALIGN_64BYTES),      // strideIn
            700,                                                          // widthOut
            128,                                                          // heightOut
            DSPLIB_CALC_STRIDE(700 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            26                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 27) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            32,                                                           // widthIn
            750,                                                          // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int8_t), ALIGN_64BYTES),       // strideIn
            750,                                                          // widthOut
            32,                                                           // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            27                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 28) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT8,                                                  // dataType
            64,                                                           // widthIn
            800,                                                          // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int8_t), ALIGN_64BYTES),       // strideIn
            800,                                                          // widthOut
            64,                                                           // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(int8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            28                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 29) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_INT8,                                                        // dataType
            48,                                                                 // widthIn
            900,                                                                // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(int8_t), ALIGN_64BYTES),             // strideIn
            900,                                                                // widthOut
            48,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(int8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            29                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 30) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_INT8,                                                        // dataType
            40,                                                                 // widthIn
            950,                                                                // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(int8_t), ALIGN_64BYTES),             // strideIn
            950,                                                                // widthOut
            40,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(int8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            30                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 31) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT8,                                                         // dataType
            128,                                                                 // widthIn
            1000,                                                                // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(int8_t), ALIGN_64BYTES),             // strideIn
            1000,                                                                // widthOut
            128,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(1000 * sizeof(int8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            31                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 32) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT8,                                                         // dataType
            256,                                                                 // widthIn
            1000,                                                                // heightIn
            DSPLIB_CALC_STRIDE(256 * sizeof(int8_t), ALIGN_64BYTES),             // strideIn
            1000,                                                                // widthOut
            256,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(1000 * sizeof(int8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            32                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 33) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT16,                                                // dataType
            1,                                                           // widthIn
            1,                                                           // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(int16_t), ALIGN_64BYTES),      // strideIn
            1,                                                           // widthOut
            1,                                                           // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            33                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 34) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT16,                                                // dataType
            16,                                                          // widthIn
            1,                                                           // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(int16_t), ALIGN_64BYTES),     // strideIn
            1,                                                           // widthOut
            16,                                                          // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            34                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 35) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT16,                                                // dataType
            30,                                                          // widthIn
            2,                                                           // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(int16_t), ALIGN_64BYTES),     // strideIn
            2,                                                           // widthOut
            30,                                                          // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            35                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 36) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT16,                                                // dataType
            64,                                                          // widthIn
            4,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int16_t), ALIGN_64BYTES),     // strideIn
            4,                                                           // widthOut
            64,                                                          // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            36                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 37) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT16,                                                // dataType
            160,                                                         // widthIn
            7,                                                           // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(int16_t), ALIGN_64BYTES),    // strideIn
            7,                                                           // widthOut
            160,                                                         // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            37                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 38) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT16,                                                // dataType
            288,                                                         // widthIn
            8,                                                           // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(int16_t), ALIGN_64BYTES),    // strideIn
            8,                                                           // widthOut
            288,                                                         // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            38                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 39) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT16,                                                 // dataType
            512,                                                          // widthIn
            16,                                                           // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(int16_t), ALIGN_64BYTES),     // strideIn
            16,                                                           // widthOut
            512,                                                          // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            39                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 40) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT16,                                                 // dataType
            1000,                                                         // widthIn
            24,                                                           // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(int16_t), ALIGN_64BYTES),    // strideIn
            24,                                                           // widthOut
            1000,                                                         // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            40                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 41) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT16,                                                 // dataType
            32,                                                           // widthIn
            32,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int16_t), ALIGN_64BYTES),      // strideIn
            32,                                                           // widthOut
            32,                                                           // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            41                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 42) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT16,                                                 // dataType
            434,                                                          // widthIn
            39,                                                           // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(int16_t), ALIGN_64BYTES),     // strideIn
            39,                                                           // widthOut
            434,                                                          // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            42                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 43) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT16,                                                 // dataType
            192,                                                          // widthIn
            48,                                                           // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(int16_t), ALIGN_64BYTES),     // strideIn
            48,                                                           // widthOut
            192,                                                          // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            43                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 44) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT16,                                                 // dataType
            64,                                                           // widthIn
            64,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int16_t), ALIGN_64BYTES),      // strideIn
            64,                                                           // widthOut
            64,                                                           // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            44                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 45) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT16,                                                 // dataType
            192,                                                          // widthIn
            96,                                                           // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(int16_t), ALIGN_64BYTES),     // strideIn
            96,                                                           // widthOut
            192,                                                          // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            45                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 46) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            128,                                                           // widthIn
            128,                                                           // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(int16_t), ALIGN_64BYTES),      // strideIn
            128,                                                           // widthOut
            128,                                                           // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            46                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 47) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            112,                                                           // widthIn
            176,                                                           // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(int16_t), ALIGN_64BYTES),      // strideIn
            176,                                                           // widthOut
            112,                                                           // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            47                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 48) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            132,                                                           // widthIn
            192,                                                           // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(int16_t), ALIGN_64BYTES),      // strideIn
            192,                                                           // widthOut
            132,                                                           // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            48                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 49) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT16,                                                        // dataType
            224,                                                                 // widthIn
            224,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(int16_t), ALIGN_64BYTES),            // strideIn
            224,                                                                 // widthOut
            224,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(int16_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            49                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 50) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT16,                                                        // dataType
            256,                                                                 // widthIn
            256,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(256 * sizeof(int16_t), ALIGN_64BYTES),            // strideIn
            256,                                                                 // widthOut
            256,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(256 * sizeof(int16_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            50                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 51) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            52,                                                            // widthIn
            262,                                                           // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(int16_t), ALIGN_64BYTES),       // strideIn
            262,                                                           // widthOut
            52,                                                            // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            51                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 52) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            144,                                                           // widthIn
            312,                                                           // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(int16_t), ALIGN_64BYTES),      // strideIn
            312,                                                           // widthOut
            144,                                                           // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            52                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 53) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            32,                                                            // widthIn
            344,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int16_t), ALIGN_64BYTES),       // strideIn
            344,                                                           // widthOut
            32,                                                            // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            53                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 54) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            64,                                                            // widthIn
            377,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int16_t), ALIGN_64BYTES),       // strideIn
            377,                                                           // widthOut
            64,                                                            // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            54                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 55) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            68,                                                            // widthIn
            448,                                                           // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(int16_t), ALIGN_64BYTES),       // strideIn
            448,                                                           // widthOut
            68,                                                            // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            55                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 56) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            16,                                                            // widthIn
            640,                                                           // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(int16_t), ALIGN_64BYTES),       // strideIn
            640,                                                           // widthOut
            16,                                                            // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            56                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 57) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            128,                                                           // widthIn
            700,                                                           // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(int16_t), ALIGN_64BYTES),      // strideIn
            700,                                                           // widthOut
            128,                                                           // heightOut
            DSPLIB_CALC_STRIDE(700 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            57                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 58) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT16,                                                        // dataType
            32,                                                                  // widthIn
            750,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int16_t), ALIGN_64BYTES),             // strideIn
            750,                                                                 // widthOut
            32,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(int16_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            58                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 59) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            64,                                                            // widthIn
            800,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int16_t), ALIGN_64BYTES),       // strideIn
            800,                                                           // widthOut
            64,                                                            // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            59                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 60) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            48,                                                            // widthIn
            900,                                                           // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(int16_t), ALIGN_64BYTES),       // strideIn
            900,                                                           // widthOut
            48,                                                            // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            60                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 61) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT16,                                                  // dataType
            40,                                                            // widthIn
            950,                                                           // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(int16_t), ALIGN_64BYTES),       // strideIn
            950,                                                           // widthOut
            40,                                                            // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(int16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            61                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 62) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_INT16,                                                         // dataType
            128,                                                                  // widthIn
            1000,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(int16_t), ALIGN_64BYTES),             // strideIn
            1000,                                                                 // widthOut
            128,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(1000 * sizeof(int16_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            62                                                                    // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 63) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT32,                                                // dataType
            1,                                                           // widthIn
            1,                                                           // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(int32_t), ALIGN_64BYTES),      // strideIn
            1,                                                           // widthOut
            1,                                                           // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            63                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 64) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT32,                                                // dataType
            16,                                                          // widthIn
            1,                                                           // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(int32_t), ALIGN_64BYTES),     // strideIn
            1,                                                           // widthOut
            16,                                                          // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            64                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 65) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT32,                                                // dataType
            30,                                                          // widthIn
            2,                                                           // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(int32_t), ALIGN_64BYTES),     // strideIn
            2,                                                           // widthOut
            30,                                                          // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            65                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 66) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT32,                                                // dataType
            64,                                                          // widthIn
            4,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int32_t), ALIGN_64BYTES),     // strideIn
            4,                                                           // widthOut
            64,                                                          // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            66                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 67) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT32,                                                // dataType
            160,                                                         // widthIn
            7,                                                           // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(int32_t), ALIGN_64BYTES),    // strideIn
            7,                                                           // widthOut
            160,                                                         // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            67                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 68) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT32,                                                // dataType
            288,                                                         // widthIn
            8,                                                           // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(int32_t), ALIGN_64BYTES),    // strideIn
            8,                                                           // widthOut
            288,                                                         // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            68                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 69) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT32,                                                 // dataType
            512,                                                          // widthIn
            16,                                                           // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(int32_t), ALIGN_64BYTES),     // strideIn
            16,                                                           // widthOut
            512,                                                          // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            69                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 70) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT32,                                                 // dataType
            1000,                                                         // widthIn
            24,                                                           // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(int32_t), ALIGN_64BYTES),    // strideIn
            24,                                                           // widthOut
            1000,                                                         // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            70                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 71) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT32,                                                 // dataType
            32,                                                           // widthIn
            32,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int32_t), ALIGN_64BYTES),      // strideIn
            32,                                                           // widthOut
            32,                                                           // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            71                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 72) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT32,                                                 // dataType
            434,                                                          // widthIn
            39,                                                           // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(int32_t), ALIGN_64BYTES),     // strideIn
            39,                                                           // widthOut
            434,                                                          // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            72                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 73) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT32,                                                 // dataType
            192,                                                          // widthIn
            48,                                                           // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(int32_t), ALIGN_64BYTES),     // strideIn
            48,                                                           // widthOut
            192,                                                          // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            73                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 74) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT32,                                                 // dataType
            64,                                                           // widthIn
            64,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int32_t), ALIGN_64BYTES),      // strideIn
            64,                                                           // widthOut
            64,                                                           // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            74                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 75) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT32,                                                 // dataType
            192,                                                          // widthIn
            96,                                                           // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(int32_t), ALIGN_64BYTES),     // strideIn
            96,                                                           // widthOut
            192,                                                          // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            75                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 76) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT32,                                                        // dataType
            128,                                                                 // widthIn
            128,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(int32_t), ALIGN_64BYTES),            // strideIn
            128,                                                                 // widthOut
            128,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(int32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            76                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 77) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            112,                                                           // widthIn
            176,                                                           // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(int32_t), ALIGN_64BYTES),      // strideIn
            176,                                                           // widthOut
            112,                                                           // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            77                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 78) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            132,                                                           // widthIn
            192,                                                           // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(int32_t), ALIGN_64BYTES),      // strideIn
            192,                                                           // widthOut
            132,                                                           // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            78                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 79) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            224,                                                           // widthIn
            224,                                                           // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(int32_t), ALIGN_64BYTES),      // strideIn
            224,                                                           // widthOut
            224,                                                           // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            79                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 80) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT32,                                                        // dataType
            256,                                                                 // widthIn
            256,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(256 * sizeof(int32_t), ALIGN_64BYTES),            // strideIn
            256,                                                                 // widthOut
            256,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(256 * sizeof(int32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            80                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 81) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            52,                                                            // widthIn
            262,                                                           // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(int32_t), ALIGN_64BYTES),       // strideIn
            262,                                                           // widthOut
            52,                                                            // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            81                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 82) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            144,                                                           // widthIn
            312,                                                           // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(int32_t), ALIGN_64BYTES),      // strideIn
            312,                                                           // widthOut
            144,                                                           // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            82                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 83) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            32,                                                            // widthIn
            344,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int32_t), ALIGN_64BYTES),       // strideIn
            344,                                                           // widthOut
            32,                                                            // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            83                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 84) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT32,                                                        // dataType
            64,                                                                  // widthIn
            377,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int32_t), ALIGN_64BYTES),             // strideIn
            377,                                                                 // widthOut
            64,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(int32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            84                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 85) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            68,                                                            // widthIn
            448,                                                           // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(int32_t), ALIGN_64BYTES),       // strideIn
            448,                                                           // widthOut
            68,                                                            // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            85                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 86) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT32,                                                        // dataType
            16,                                                                  // widthIn
            640,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(int32_t), ALIGN_64BYTES),             // strideIn
            640,                                                                 // widthOut
            16,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(int32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            86                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 87) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            128,                                                           // widthIn
            700,                                                           // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(int32_t), ALIGN_64BYTES),      // strideIn
            700,                                                           // widthOut
            128,                                                           // heightOut
            DSPLIB_CALC_STRIDE(700 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            87                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 88) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT32,                                                        // dataType
            32,                                                                  // widthIn
            750,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int32_t), ALIGN_64BYTES),             // strideIn
            750,                                                                 // widthOut
            32,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(int32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            88                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 89) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            64,                                                            // widthIn
            800,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int32_t), ALIGN_64BYTES),       // strideIn
            800,                                                           // widthOut
            64,                                                            // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            89                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 90) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            48,                                                            // widthIn
            900,                                                           // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(int32_t), ALIGN_64BYTES),       // strideIn
            900,                                                           // widthOut
            48,                                                            // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            90                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 91) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT32,                                                  // dataType
            40,                                                            // widthIn
            950,                                                           // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(int32_t), ALIGN_64BYTES),       // strideIn
            950,                                                           // widthOut
            40,                                                            // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(int32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            91                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 92) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT64,                                                // dataType
            1,                                                           // widthIn
            1,                                                           // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(int64_t), ALIGN_64BYTES),      // strideIn
            1,                                                           // widthOut
            1,                                                           // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            92                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 93) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT64,                                                // dataType
            16,                                                          // widthIn
            1,                                                           // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(int64_t), ALIGN_64BYTES),     // strideIn
            1,                                                           // widthOut
            16,                                                          // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            93                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 94) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT64,                                                // dataType
            30,                                                          // widthIn
            2,                                                           // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(int64_t), ALIGN_64BYTES),     // strideIn
            2,                                                           // widthOut
            30,                                                          // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            94                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 95) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT64,                                                // dataType
            64,                                                          // widthIn
            4,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int64_t), ALIGN_64BYTES),     // strideIn
            4,                                                           // widthOut
            64,                                                          // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            95                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 96) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT64,                                                // dataType
            160,                                                         // widthIn
            7,                                                           // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(int64_t), ALIGN_64BYTES),    // strideIn
            7,                                                           // widthOut
            160,                                                         // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            96                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 97) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_INT64,                                                // dataType
            288,                                                         // widthIn
            8,                                                           // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(int64_t), ALIGN_64BYTES),    // strideIn
            8,                                                           // widthOut
            288,                                                         // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            97                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 98) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT64,                                                 // dataType
            512,                                                          // widthIn
            16,                                                           // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(int64_t), ALIGN_64BYTES),     // strideIn
            16,                                                           // widthOut
            512,                                                          // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            98                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 99) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT64,                                                 // dataType
            1000,                                                         // widthIn
            24,                                                           // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(int64_t), ALIGN_64BYTES),    // strideIn
            24,                                                           // widthOut
            1000,                                                         // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            99                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 100) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT64,                                                 // dataType
            32,                                                           // widthIn
            32,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int64_t), ALIGN_64BYTES),      // strideIn
            32,                                                           // widthOut
            32,                                                           // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            100                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 101) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT64,                                                 // dataType
            434,                                                          // widthIn
            39,                                                           // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(int64_t), ALIGN_64BYTES),     // strideIn
            39,                                                           // widthOut
            434,                                                          // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            101                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 102) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT64,                                                 // dataType
            192,                                                          // widthIn
            48,                                                           // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(int64_t), ALIGN_64BYTES),     // strideIn
            48,                                                           // widthOut
            192,                                                          // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            102                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 103) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_INT64,                                                       // dataType
            64,                                                                 // widthIn
            64,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int64_t), ALIGN_64BYTES),            // strideIn
            64,                                                                 // widthOut
            64,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(int64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            103                                                                 // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 104) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_INT64,                                                 // dataType
            192,                                                          // widthIn
            96,                                                           // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(int64_t), ALIGN_64BYTES),     // strideIn
            96,                                                           // widthOut
            192,                                                          // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            104                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 105) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT64,                                                        // dataType
            128,                                                                 // widthIn
            128,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(int64_t), ALIGN_64BYTES),            // strideIn
            128,                                                                 // widthOut
            128,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(int64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            105                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 106) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT64,                                                  // dataType
            112,                                                           // widthIn
            176,                                                           // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(int64_t), ALIGN_64BYTES),      // strideIn
            176,                                                           // widthOut
            112,                                                           // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            106                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 107) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT64,                                                        // dataType
            132,                                                                 // widthIn
            192,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(int64_t), ALIGN_64BYTES),            // strideIn
            192,                                                                 // widthOut
            132,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(int64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            107                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 108) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT64,                                                  // dataType
            224,                                                           // widthIn
            224,                                                           // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(int64_t), ALIGN_64BYTES),      // strideIn
            224,                                                           // widthOut
            224,                                                           // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            108                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 109) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT64,                                                  // dataType
            52,                                                            // widthIn
            262,                                                           // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(int64_t), ALIGN_64BYTES),       // strideIn
            262,                                                           // widthOut
            52,                                                            // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            109                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 110) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT64,                                                        // dataType
            144,                                                                 // widthIn
            312,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(int64_t), ALIGN_64BYTES),            // strideIn
            312,                                                                 // widthOut
            144,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(int64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            110                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 111) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT64,                                                  // dataType
            32,                                                            // widthIn
            344,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int64_t), ALIGN_64BYTES),       // strideIn
            344,                                                           // widthOut
            32,                                                            // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            111                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 112) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT64,                                                        // dataType
            64,                                                                  // widthIn
            377,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int64_t), ALIGN_64BYTES),             // strideIn
            377,                                                                 // widthOut
            64,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(int64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            112                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 113) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT64,                                                        // dataType
            68,                                                                  // widthIn
            448,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(int64_t), ALIGN_64BYTES),             // strideIn
            448,                                                                 // widthOut
            68,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(int64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            113                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 114) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT64,                                                        // dataType
            16,                                                                  // widthIn
            640,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(int64_t), ALIGN_64BYTES),             // strideIn
            640,                                                                 // widthOut
            16,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(int64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            114                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 115) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT64,                                                  // dataType
            32,                                                            // widthIn
            750,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(int64_t), ALIGN_64BYTES),       // strideIn
            750,                                                           // widthOut
            32,                                                            // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            115                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 116) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT64,                                                  // dataType
            64,                                                            // widthIn
            800,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(int64_t), ALIGN_64BYTES),       // strideIn
            800,                                                           // widthOut
            64,                                                            // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            116                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 117) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_INT64,                                                  // dataType
            48,                                                            // widthIn
            900,                                                           // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(int64_t), ALIGN_64BYTES),       // strideIn
            900,                                                           // widthOut
            48,                                                            // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(int64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            117                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 118) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_INT64,                                                        // dataType
            40,                                                                  // widthIn
            950,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(int64_t), ALIGN_64BYTES),             // strideIn
            950,                                                                 // widthOut
            40,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(int64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            118                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 119) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_UINT8,                                                // dataType
            1,                                                           // widthIn
            1,                                                           // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            1,                                                           // widthOut
            1,                                                           // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            119                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 120) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_UINT8,                                                // dataType
            16,                                                          // widthIn
            1,                                                           // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(uint8_t), ALIGN_64BYTES),     // strideIn
            1,                                                           // widthOut
            16,                                                          // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            120                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 121) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_UINT8,                                                // dataType
            30,                                                          // widthIn
            2,                                                           // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(uint8_t), ALIGN_64BYTES),     // strideIn
            2,                                                           // widthOut
            30,                                                          // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            121                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 122) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_UINT8,                                                // dataType
            64,                                                          // widthIn
            4,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint8_t), ALIGN_64BYTES),     // strideIn
            4,                                                           // widthOut
            64,                                                          // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            122                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 123) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_UINT8,                                                // dataType
            160,                                                         // widthIn
            7,                                                           // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(uint8_t), ALIGN_64BYTES),    // strideIn
            7,                                                           // widthOut
            160,                                                         // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            123                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 124) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_UINT8,                                                // dataType
            288,                                                         // widthIn
            8,                                                           // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(uint8_t), ALIGN_64BYTES),    // strideIn
            8,                                                           // widthOut
            288,                                                         // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            124                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 125) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT8,                                                 // dataType
            512,                                                          // widthIn
            16,                                                           // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(uint8_t), ALIGN_64BYTES),     // strideIn
            16,                                                           // widthOut
            512,                                                          // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            125                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 126) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT8,                                                 // dataType
            1000,                                                         // widthIn
            24,                                                           // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(uint8_t), ALIGN_64BYTES),    // strideIn
            24,                                                           // widthOut
            1000,                                                         // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            126                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 127) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT8,                                                 // dataType
            32,                                                           // widthIn
            32,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            32,                                                           // widthOut
            32,                                                           // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            127                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 128) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT8,                                                 // dataType
            434,                                                          // widthIn
            39,                                                           // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(uint8_t), ALIGN_64BYTES),     // strideIn
            39,                                                           // widthOut
            434,                                                          // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            128                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 129) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT8,                                                 // dataType
            192,                                                          // widthIn
            48,                                                           // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(uint8_t), ALIGN_64BYTES),     // strideIn
            48,                                                           // widthOut
            192,                                                          // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            129                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 130) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT8,                                                 // dataType
            64,                                                           // widthIn
            64,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            64,                                                           // widthOut
            64,                                                           // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            130                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 131) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT8,                                                 // dataType
            192,                                                          // widthIn
            96,                                                           // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(uint8_t), ALIGN_64BYTES),     // strideIn
            96,                                                           // widthOut
            192,                                                          // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            131                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 132) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            128,                                                           // widthIn
            128,                                                           // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            128,                                                           // widthOut
            128,                                                           // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            132                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 133) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            112,                                                           // widthIn
            176,                                                           // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            176,                                                           // widthOut
            112,                                                           // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            133                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 134) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            132,                                                           // widthIn
            192,                                                           // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            192,                                                           // widthOut
            132,                                                           // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            134                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 135) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            224,                                                           // widthIn
            224,                                                           // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            224,                                                           // widthOut
            224,                                                           // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            135                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 136) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            256,                                                           // widthIn
            256,                                                           // heightIn
            DSPLIB_CALC_STRIDE(256 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            256,                                                           // widthOut
            256,                                                           // heightOut
            DSPLIB_CALC_STRIDE(256 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            136                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 137) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            52,                                                            // widthIn
            262,                                                           // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(uint8_t), ALIGN_64BYTES),       // strideIn
            262,                                                           // widthOut
            52,                                                            // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            137                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 138) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            144,                                                           // widthIn
            312,                                                           // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            312,                                                           // widthOut
            144,                                                           // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            138                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 139) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            32,                                                            // widthIn
            344,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint8_t), ALIGN_64BYTES),       // strideIn
            344,                                                           // widthOut
            32,                                                            // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            139                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 140) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            64,                                                            // widthIn
            377,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint8_t), ALIGN_64BYTES),       // strideIn
            377,                                                           // widthOut
            64,                                                            // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            140                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 141) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_UINT8,                                                        // dataType
            68,                                                                  // widthIn
            448,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(uint8_t), ALIGN_64BYTES),             // strideIn
            448,                                                                 // widthOut
            68,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            141                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 142) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_UINT8,                                                        // dataType
            512,                                                                 // widthIn
            512,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(uint8_t), ALIGN_64BYTES),            // strideIn
            512,                                                                 // widthOut
            512,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(512 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            142                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 143) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            16,                                                            // widthIn
            640,                                                           // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(uint8_t), ALIGN_64BYTES),       // strideIn
            640,                                                           // widthOut
            16,                                                            // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            143                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 144) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            128,                                                           // widthIn
            700,                                                           // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(uint8_t), ALIGN_64BYTES),      // strideIn
            700,                                                           // widthOut
            128,                                                           // heightOut
            DSPLIB_CALC_STRIDE(700 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            144                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 145) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            32,                                                            // widthIn
            750,                                                           // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint8_t), ALIGN_64BYTES),       // strideIn
            750,                                                           // widthOut
            32,                                                            // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            145                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 146) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT8,                                                  // dataType
            64,                                                            // widthIn
            800,                                                           // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint8_t), ALIGN_64BYTES),       // strideIn
            800,                                                           // widthOut
            64,                                                            // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            146                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 147) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_UINT8,                                                        // dataType
            48,                                                                  // widthIn
            900,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(uint8_t), ALIGN_64BYTES),             // strideIn
            900,                                                                 // widthOut
            48,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            147                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 148) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_UINT8,                                                        // dataType
            40,                                                                  // widthIn
            950,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(uint8_t), ALIGN_64BYTES),             // strideIn
            950,                                                                 // widthOut
            40,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            148                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 149) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT8,                                                         // dataType
            128,                                                                  // widthIn
            1000,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(uint8_t), ALIGN_64BYTES),             // strideIn
            1000,                                                                 // widthOut
            128,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(1000 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            149                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 150) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT8,                                                         // dataType
            256,                                                                  // widthIn
            1000,                                                                 // heightIn
            DSPLIB_CALC_STRIDE(256 * sizeof(uint8_t), ALIGN_64BYTES),             // strideIn
            1000,                                                                 // widthOut
            256,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(1000 * sizeof(uint8_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            150                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 151) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT16,                                                // dataType
            1,                                                            // widthIn
            1,                                                            // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(uint16_t), ALIGN_64BYTES),      // strideIn
            1,                                                            // widthOut
            1,                                                            // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            151                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 152) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT16,                                                // dataType
            16,                                                           // widthIn
            1,                                                            // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(uint16_t), ALIGN_64BYTES),     // strideIn
            1,                                                            // widthOut
            16,                                                           // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            152                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 153) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT16,                                                // dataType
            30,                                                           // widthIn
            2,                                                            // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(uint16_t), ALIGN_64BYTES),     // strideIn
            2,                                                            // widthOut
            30,                                                           // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            153                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 154) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT16,                                                // dataType
            64,                                                           // widthIn
            4,                                                            // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint16_t), ALIGN_64BYTES),     // strideIn
            4,                                                            // widthOut
            64,                                                           // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            154                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 155) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT16,                                                // dataType
            160,                                                          // widthIn
            7,                                                            // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(uint16_t), ALIGN_64BYTES),    // strideIn
            7,                                                            // widthOut
            160,                                                          // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            155                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 156) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT16,                                                // dataType
            288,                                                          // widthIn
            8,                                                            // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(uint16_t), ALIGN_64BYTES),    // strideIn
            8,                                                            // widthOut
            288,                                                          // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            156                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 157) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT16,                                                 // dataType
            512,                                                           // widthIn
            16,                                                            // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(uint16_t), ALIGN_64BYTES),     // strideIn
            16,                                                            // widthOut
            512,                                                           // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            157                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 158) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT16,                                                 // dataType
            1000,                                                          // widthIn
            24,                                                            // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(uint16_t), ALIGN_64BYTES),    // strideIn
            24,                                                            // widthOut
            1000,                                                          // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            158                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 159) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT16,                                                 // dataType
            32,                                                            // widthIn
            32,                                                            // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint16_t), ALIGN_64BYTES),      // strideIn
            32,                                                            // widthOut
            32,                                                            // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            159                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 160) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT16,                                                 // dataType
            434,                                                           // widthIn
            39,                                                            // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(uint16_t), ALIGN_64BYTES),     // strideIn
            39,                                                            // widthOut
            434,                                                           // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            160                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 161) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT16,                                                 // dataType
            192,                                                           // widthIn
            48,                                                            // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(uint16_t), ALIGN_64BYTES),     // strideIn
            48,                                                            // widthOut
            192,                                                           // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            161                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 162) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT16,                                                 // dataType
            64,                                                            // widthIn
            64,                                                            // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint16_t), ALIGN_64BYTES),      // strideIn
            64,                                                            // widthOut
            64,                                                            // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            162                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 163) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT16,                                                 // dataType
            192,                                                           // widthIn
            96,                                                            // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(uint16_t), ALIGN_64BYTES),     // strideIn
            96,                                                            // widthOut
            192,                                                           // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            163                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 164) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            128,                                                            // widthIn
            128,                                                            // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(uint16_t), ALIGN_64BYTES),      // strideIn
            128,                                                            // widthOut
            128,                                                            // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            164                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 165) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            112,                                                            // widthIn
            176,                                                            // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(uint16_t), ALIGN_64BYTES),      // strideIn
            176,                                                            // widthOut
            112,                                                            // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            165                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 166) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            132,                                                            // widthIn
            192,                                                            // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(uint16_t), ALIGN_64BYTES),      // strideIn
            192,                                                            // widthOut
            132,                                                            // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            166                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 167) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT16,                                                        // dataType
            224,                                                                  // widthIn
            224,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(uint16_t), ALIGN_64BYTES),            // strideIn
            224,                                                                  // widthOut
            224,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            167                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 168) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT16,                                                        // dataType
            256,                                                                  // widthIn
            256,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(256 * sizeof(uint16_t), ALIGN_64BYTES),            // strideIn
            256,                                                                  // widthOut
            256,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(256 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            168                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 169) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            52,                                                             // widthIn
            262,                                                            // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(uint16_t), ALIGN_64BYTES),       // strideIn
            262,                                                            // widthOut
            52,                                                             // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            169                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 170) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            144,                                                            // widthIn
            312,                                                            // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(uint16_t), ALIGN_64BYTES),      // strideIn
            312,                                                            // widthOut
            144,                                                            // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            170                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 171) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            32,                                                             // widthIn
            344,                                                            // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint16_t), ALIGN_64BYTES),       // strideIn
            344,                                                            // widthOut
            32,                                                             // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            171                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 172) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            64,                                                             // widthIn
            377,                                                            // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint16_t), ALIGN_64BYTES),       // strideIn
            377,                                                            // widthOut
            64,                                                             // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            172                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 173) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            68,                                                             // widthIn
            448,                                                            // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(uint16_t), ALIGN_64BYTES),       // strideIn
            448,                                                            // widthOut
            68,                                                             // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            173                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 174) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            16,                                                             // widthIn
            640,                                                            // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(uint16_t), ALIGN_64BYTES),       // strideIn
            640,                                                            // widthOut
            16,                                                             // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            174                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 175) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            128,                                                            // widthIn
            700,                                                            // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(uint16_t), ALIGN_64BYTES),      // strideIn
            700,                                                            // widthOut
            128,                                                            // heightOut
            DSPLIB_CALC_STRIDE(700 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            175                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 176) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT16,                                                        // dataType
            32,                                                                   // widthIn
            750,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint16_t), ALIGN_64BYTES),             // strideIn
            750,                                                                  // widthOut
            32,                                                                   // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            176                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 177) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            64,                                                             // widthIn
            800,                                                            // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint16_t), ALIGN_64BYTES),       // strideIn
            800,                                                            // widthOut
            64,                                                             // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            177                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 178) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            48,                                                             // widthIn
            900,                                                            // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(uint16_t), ALIGN_64BYTES),       // strideIn
            900,                                                            // widthOut
            48,                                                             // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            178                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 179) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT16,                                                  // dataType
            40,                                                             // widthIn
            950,                                                            // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(uint16_t), ALIGN_64BYTES),       // strideIn
            950,                                                            // widthOut
            40,                                                             // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            179                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 180) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                                // testPattern
            NULL,                                                                  //*staticIn
            NULL,                                                                  //*staticOut
            DSPLIB_UINT16,                                                         // dataType
            128,                                                                   // widthIn
            1000,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(uint16_t), ALIGN_64BYTES),             // strideIn
            1000,                                                                  // widthOut
            128,                                                                   // heightOut
            DSPLIB_CALC_STRIDE(1000 * sizeof(uint16_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                     // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                               // outputDataLocation
            1,                                                                     // numReps
            180                                                                    // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 181) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT32,                                                // dataType
            1,                                                            // widthIn
            1,                                                            // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(uint32_t), ALIGN_64BYTES),      // strideIn
            1,                                                            // widthOut
            1,                                                            // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            181                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 182) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT32,                                                // dataType
            16,                                                           // widthIn
            1,                                                            // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(uint32_t), ALIGN_64BYTES),     // strideIn
            1,                                                            // widthOut
            16,                                                           // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            182                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 183) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT32,                                                // dataType
            30,                                                           // widthIn
            2,                                                            // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(uint32_t), ALIGN_64BYTES),     // strideIn
            2,                                                            // widthOut
            30,                                                           // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            183                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 184) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT32,                                                // dataType
            64,                                                           // widthIn
            4,                                                            // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint32_t), ALIGN_64BYTES),     // strideIn
            4,                                                            // widthOut
            64,                                                           // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            184                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 185) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT32,                                                // dataType
            160,                                                          // widthIn
            7,                                                            // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(uint32_t), ALIGN_64BYTES),    // strideIn
            7,                                                            // widthOut
            160,                                                          // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            185                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 186) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT32,                                                // dataType
            288,                                                          // widthIn
            8,                                                            // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(uint32_t), ALIGN_64BYTES),    // strideIn
            8,                                                            // widthOut
            288,                                                          // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            186                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 187) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT32,                                                 // dataType
            512,                                                           // widthIn
            16,                                                            // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(uint32_t), ALIGN_64BYTES),     // strideIn
            16,                                                            // widthOut
            512,                                                           // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            187                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 188) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT32,                                                 // dataType
            1000,                                                          // widthIn
            24,                                                            // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(uint32_t), ALIGN_64BYTES),    // strideIn
            24,                                                            // widthOut
            1000,                                                          // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            188                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 189) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT32,                                                 // dataType
            32,                                                            // widthIn
            32,                                                            // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint32_t), ALIGN_64BYTES),      // strideIn
            32,                                                            // widthOut
            32,                                                            // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            189                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 190) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT32,                                                 // dataType
            434,                                                           // widthIn
            39,                                                            // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(uint32_t), ALIGN_64BYTES),     // strideIn
            39,                                                            // widthOut
            434,                                                           // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            190                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 191) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT32,                                                 // dataType
            192,                                                           // widthIn
            48,                                                            // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(uint32_t), ALIGN_64BYTES),     // strideIn
            48,                                                            // widthOut
            192,                                                           // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            191                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 192) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT32,                                                 // dataType
            64,                                                            // widthIn
            64,                                                            // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint32_t), ALIGN_64BYTES),      // strideIn
            64,                                                            // widthOut
            64,                                                            // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            192                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 193) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT32,                                                 // dataType
            192,                                                           // widthIn
            96,                                                            // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(uint32_t), ALIGN_64BYTES),     // strideIn
            96,                                                            // widthOut
            192,                                                           // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            193                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 194) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT32,                                                        // dataType
            128,                                                                  // widthIn
            128,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(uint32_t), ALIGN_64BYTES),            // strideIn
            128,                                                                  // widthOut
            128,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            194                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 195) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            112,                                                            // widthIn
            176,                                                            // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(uint32_t), ALIGN_64BYTES),      // strideIn
            176,                                                            // widthOut
            112,                                                            // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            195                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 196) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            132,                                                            // widthIn
            192,                                                            // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(uint32_t), ALIGN_64BYTES),      // strideIn
            192,                                                            // widthOut
            132,                                                            // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            196                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 197) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            224,                                                            // widthIn
            224,                                                            // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(uint32_t), ALIGN_64BYTES),      // strideIn
            224,                                                            // widthOut
            224,                                                            // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            197                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 198) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT32,                                                        // dataType
            256,                                                                  // widthIn
            256,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(256 * sizeof(uint32_t), ALIGN_64BYTES),            // strideIn
            256,                                                                  // widthOut
            256,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(256 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            198                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 199) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            52,                                                             // widthIn
            262,                                                            // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(uint32_t), ALIGN_64BYTES),       // strideIn
            262,                                                            // widthOut
            52,                                                             // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            199                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 200) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            144,                                                            // widthIn
            312,                                                            // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(uint32_t), ALIGN_64BYTES),      // strideIn
            312,                                                            // widthOut
            144,                                                            // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            200                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 201) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            32,                                                             // widthIn
            344,                                                            // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint32_t), ALIGN_64BYTES),       // strideIn
            344,                                                            // widthOut
            32,                                                             // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            201                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 202) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT32,                                                        // dataType
            64,                                                                   // widthIn
            377,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint32_t), ALIGN_64BYTES),             // strideIn
            377,                                                                  // widthOut
            64,                                                                   // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            202                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 203) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            68,                                                             // widthIn
            448,                                                            // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(uint32_t), ALIGN_64BYTES),       // strideIn
            448,                                                            // widthOut
            68,                                                             // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            203                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 204) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT32,                                                        // dataType
            16,                                                                   // widthIn
            640,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(uint32_t), ALIGN_64BYTES),             // strideIn
            640,                                                                  // widthOut
            16,                                                                   // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            204                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 205) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            128,                                                            // widthIn
            700,                                                            // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(uint32_t), ALIGN_64BYTES),      // strideIn
            700,                                                            // widthOut
            128,                                                            // heightOut
            DSPLIB_CALC_STRIDE(700 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            205                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 206) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT32,                                                        // dataType
            32,                                                                   // widthIn
            750,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint32_t), ALIGN_64BYTES),             // strideIn
            750,                                                                  // widthOut
            32,                                                                   // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            206                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 207) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            64,                                                             // widthIn
            800,                                                            // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint32_t), ALIGN_64BYTES),       // strideIn
            800,                                                            // widthOut
            64,                                                             // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            207                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 208) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            48,                                                             // widthIn
            900,                                                            // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(uint32_t), ALIGN_64BYTES),       // strideIn
            900,                                                            // widthOut
            48,                                                             // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            208                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 209) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT32,                                                  // dataType
            40,                                                             // widthIn
            950,                                                            // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(uint32_t), ALIGN_64BYTES),       // strideIn
            950,                                                            // widthOut
            40,                                                             // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(uint32_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            209                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 210) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT64,                                                // dataType
            1,                                                            // widthIn
            1,                                                            // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(uint64_t), ALIGN_64BYTES),      // strideIn
            1,                                                            // widthOut
            1,                                                            // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            210                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 211) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT64,                                                // dataType
            16,                                                           // widthIn
            1,                                                            // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(uint64_t), ALIGN_64BYTES),     // strideIn
            1,                                                            // widthOut
            16,                                                           // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            211                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 212) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT64,                                                // dataType
            30,                                                           // widthIn
            2,                                                            // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(uint64_t), ALIGN_64BYTES),     // strideIn
            2,                                                            // widthOut
            30,                                                           // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            212                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 213) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT64,                                                // dataType
            64,                                                           // widthIn
            4,                                                            // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint64_t), ALIGN_64BYTES),     // strideIn
            4,                                                            // widthOut
            64,                                                           // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            213                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 214) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT64,                                                // dataType
            160,                                                          // widthIn
            7,                                                            // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(uint64_t), ALIGN_64BYTES),    // strideIn
            7,                                                            // widthOut
            160,                                                          // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            214                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 215) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_UINT64,                                                // dataType
            288,                                                          // widthIn
            8,                                                            // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(uint64_t), ALIGN_64BYTES),    // strideIn
            8,                                                            // widthOut
            288,                                                          // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            215                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 216) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT64,                                                 // dataType
            512,                                                           // widthIn
            16,                                                            // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(uint64_t), ALIGN_64BYTES),     // strideIn
            16,                                                            // widthOut
            512,                                                           // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            216                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 217) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT64,                                                 // dataType
            1000,                                                          // widthIn
            24,                                                            // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(uint64_t), ALIGN_64BYTES),    // strideIn
            24,                                                            // widthOut
            1000,                                                          // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            217                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 218) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT64,                                                 // dataType
            32,                                                            // widthIn
            32,                                                            // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint64_t), ALIGN_64BYTES),      // strideIn
            32,                                                            // widthOut
            32,                                                            // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            218                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 219) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT64,                                                 // dataType
            434,                                                           // widthIn
            39,                                                            // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(uint64_t), ALIGN_64BYTES),     // strideIn
            39,                                                            // widthOut
            434,                                                           // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            219                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 220) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT64,                                                 // dataType
            192,                                                           // widthIn
            48,                                                            // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(uint64_t), ALIGN_64BYTES),     // strideIn
            48,                                                            // widthOut
            192,                                                           // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            220                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 221) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                              // testPattern
            NULL,                                                                //*staticIn
            NULL,                                                                //*staticOut
            DSPLIB_UINT64,                                                       // dataType
            64,                                                                  // widthIn
            64,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint64_t), ALIGN_64BYTES),            // strideIn
            64,                                                                  // widthOut
            64,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                   // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                             // outputDataLocation
            1,                                                                   // numReps
            221                                                                  // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 222) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                        // testPattern
            NULL,                                                          //*staticIn
            NULL,                                                          //*staticOut
            DSPLIB_UINT64,                                                 // dataType
            192,                                                           // widthIn
            96,                                                            // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(uint64_t), ALIGN_64BYTES),     // strideIn
            96,                                                            // widthOut
            192,                                                           // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                             // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                       // outputDataLocation
            1,                                                             // numReps
            222                                                            // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 223) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT64,                                                        // dataType
            128,                                                                  // widthIn
            128,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(uint64_t), ALIGN_64BYTES),            // strideIn
            128,                                                                  // widthOut
            128,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            223                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 224) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT64,                                                  // dataType
            112,                                                            // widthIn
            176,                                                            // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(uint64_t), ALIGN_64BYTES),      // strideIn
            176,                                                            // widthOut
            112,                                                            // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            224                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 225) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT64,                                                        // dataType
            132,                                                                  // widthIn
            192,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(uint64_t), ALIGN_64BYTES),            // strideIn
            192,                                                                  // widthOut
            132,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            225                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 226) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT64,                                                  // dataType
            224,                                                            // widthIn
            224,                                                            // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(uint64_t), ALIGN_64BYTES),      // strideIn
            224,                                                            // widthOut
            224,                                                            // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            226                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 227) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT64,                                                  // dataType
            52,                                                             // widthIn
            262,                                                            // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(uint64_t), ALIGN_64BYTES),       // strideIn
            262,                                                            // widthOut
            52,                                                             // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            227                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 228) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT64,                                                        // dataType
            144,                                                                  // widthIn
            312,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(uint64_t), ALIGN_64BYTES),            // strideIn
            312,                                                                  // widthOut
            144,                                                                  // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            228                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 229) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT64,                                                  // dataType
            32,                                                             // widthIn
            344,                                                            // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint64_t), ALIGN_64BYTES),       // strideIn
            344,                                                            // widthOut
            32,                                                             // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            229                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 230) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT64,                                                        // dataType
            64,                                                                   // widthIn
            377,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint64_t), ALIGN_64BYTES),             // strideIn
            377,                                                                  // widthOut
            64,                                                                   // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            230                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 231) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT64,                                                        // dataType
            68,                                                                   // widthIn
            448,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(uint64_t), ALIGN_64BYTES),             // strideIn
            448,                                                                  // widthOut
            68,                                                                   // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            231                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 232) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT64,                                                        // dataType
            16,                                                                   // widthIn
            640,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(uint64_t), ALIGN_64BYTES),             // strideIn
            640,                                                                  // widthOut
            16,                                                                   // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            232                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 233) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT64,                                                  // dataType
            32,                                                             // widthIn
            750,                                                            // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(uint64_t), ALIGN_64BYTES),       // strideIn
            750,                                                            // widthOut
            32,                                                             // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            233                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 234) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT64,                                                  // dataType
            64,                                                             // widthIn
            800,                                                            // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(uint64_t), ALIGN_64BYTES),       // strideIn
            800,                                                            // widthOut
            64,                                                             // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            234                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 235) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                         // testPattern
            NULL,                                                           //*staticIn
            NULL,                                                           //*staticOut
            DSPLIB_UINT64,                                                  // dataType
            48,                                                             // widthIn
            900,                                                            // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(uint64_t), ALIGN_64BYTES),       // strideIn
            900,                                                            // widthOut
            48,                                                             // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                              // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                        // outputDataLocation
            1,                                                              // numReps
            235                                                             // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 236) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                               // testPattern
            NULL,                                                                 //*staticIn
            NULL,                                                                 //*staticOut
            DSPLIB_UINT64,                                                        // dataType
            40,                                                                   // widthIn
            950,                                                                  // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(uint64_t), ALIGN_64BYTES),             // strideIn
            950,                                                                  // widthOut
            40,                                                                   // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(uint64_t), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                    // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                              // outputDataLocation
            1,                                                                    // numReps
            236                                                                   // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 237) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                    // testPattern
            NULL,                                                      //*staticIn
            NULL,                                                      //*staticOut
            DSPLIB_FLOAT32,                                            // dataType
            1,                                                         // widthIn
            1,                                                         // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(float), ALIGN_64BYTES),      // strideIn
            1,                                                         // widthOut
            1,                                                         // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                         // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                   // outputDataLocation
            1,                                                         // numReps
            237                                                        // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 238) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                    // testPattern
            NULL,                                                      //*staticIn
            NULL,                                                      //*staticOut
            DSPLIB_FLOAT32,                                            // dataType
            16,                                                        // widthIn
            1,                                                         // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(float), ALIGN_64BYTES),     // strideIn
            1,                                                         // widthOut
            16,                                                        // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                         // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                   // outputDataLocation
            1,                                                         // numReps
            238                                                        // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 239) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                    // testPattern
            NULL,                                                      //*staticIn
            NULL,                                                      //*staticOut
            DSPLIB_FLOAT32,                                            // dataType
            30,                                                        // widthIn
            2,                                                         // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(float), ALIGN_64BYTES),     // strideIn
            2,                                                         // widthOut
            30,                                                        // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                         // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                   // outputDataLocation
            1,                                                         // numReps
            239                                                        // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 240) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                    // testPattern
            NULL,                                                      //*staticIn
            NULL,                                                      //*staticOut
            DSPLIB_FLOAT32,                                            // dataType
            64,                                                        // widthIn
            4,                                                         // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(float), ALIGN_64BYTES),     // strideIn
            4,                                                         // widthOut
            64,                                                        // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                         // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                   // outputDataLocation
            1,                                                         // numReps
            240                                                        // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 241) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                    // testPattern
            NULL,                                                      //*staticIn
            NULL,                                                      //*staticOut
            DSPLIB_FLOAT32,                                            // dataType
            160,                                                       // widthIn
            7,                                                         // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(float), ALIGN_64BYTES),    // strideIn
            7,                                                         // widthOut
            160,                                                       // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                         // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                   // outputDataLocation
            1,                                                         // numReps
            241                                                        // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 242) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                    // testPattern
            NULL,                                                      //*staticIn
            NULL,                                                      //*staticOut
            DSPLIB_FLOAT32,                                            // dataType
            288,                                                       // widthIn
            8,                                                         // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(float), ALIGN_64BYTES),    // strideIn
            8,                                                         // widthOut
            288,                                                       // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                         // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                   // outputDataLocation
            1,                                                         // numReps
            242                                                        // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 243) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT32,                                             // dataType
            512,                                                        // widthIn
            16,                                                         // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(float), ALIGN_64BYTES),     // strideIn
            16,                                                         // widthOut
            512,                                                        // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            243                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 244) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT32,                                             // dataType
            1000,                                                       // widthIn
            24,                                                         // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(float), ALIGN_64BYTES),    // strideIn
            24,                                                         // widthOut
            1000,                                                       // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            244                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 245) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT32,                                             // dataType
            32,                                                         // widthIn
            32,                                                         // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(float), ALIGN_64BYTES),      // strideIn
            32,                                                         // widthOut
            32,                                                         // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            245                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 246) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT32,                                             // dataType
            434,                                                        // widthIn
            39,                                                         // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(float), ALIGN_64BYTES),     // strideIn
            39,                                                         // widthOut
            434,                                                        // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            246                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 247) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT32,                                             // dataType
            192,                                                        // widthIn
            48,                                                         // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(float), ALIGN_64BYTES),     // strideIn
            48,                                                         // widthOut
            192,                                                        // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            247                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 248) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT32,                                             // dataType
            64,                                                         // widthIn
            64,                                                         // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(float), ALIGN_64BYTES),      // strideIn
            64,                                                         // widthOut
            64,                                                         // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            248                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 249) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT32,                                             // dataType
            192,                                                        // widthIn
            96,                                                         // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(float), ALIGN_64BYTES),     // strideIn
            96,                                                         // widthOut
            192,                                                        // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            249                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 250) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                            // testPattern
            NULL,                                                              //*staticIn
            NULL,                                                              //*staticOut
            DSPLIB_FLOAT32,                                                    // dataType
            128,                                                               // widthIn
            128,                                                               // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(float), ALIGN_64BYTES),            // strideIn
            128,                                                               // widthOut
            128,                                                               // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(float), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                 // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                           // outputDataLocation
            1,                                                                 // numReps
            250                                                                // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 251) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            112,                                                         // widthIn
            176,                                                         // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(float), ALIGN_64BYTES),      // strideIn
            176,                                                         // widthOut
            112,                                                         // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            251                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 252) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            132,                                                         // widthIn
            192,                                                         // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(float), ALIGN_64BYTES),      // strideIn
            192,                                                         // widthOut
            132,                                                         // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            252                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 253) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            224,                                                         // widthIn
            224,                                                         // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(float), ALIGN_64BYTES),      // strideIn
            224,                                                         // widthOut
            224,                                                         // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            253                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 254) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                            // testPattern
            NULL,                                                              //*staticIn
            NULL,                                                              //*staticOut
            DSPLIB_FLOAT32,                                                    // dataType
            256,                                                               // widthIn
            256,                                                               // heightIn
            DSPLIB_CALC_STRIDE(256 * sizeof(float), ALIGN_64BYTES),            // strideIn
            256,                                                               // widthOut
            256,                                                               // heightOut
            DSPLIB_CALC_STRIDE(256 * sizeof(float), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                 // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                           // outputDataLocation
            1,                                                                 // numReps
            254                                                                // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 255) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            52,                                                          // widthIn
            262,                                                         // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(float), ALIGN_64BYTES),       // strideIn
            262,                                                         // widthOut
            52,                                                          // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            255                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 256) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            144,                                                         // widthIn
            312,                                                         // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(float), ALIGN_64BYTES),      // strideIn
            312,                                                         // widthOut
            144,                                                         // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            256                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 257) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            32,                                                          // widthIn
            344,                                                         // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(float), ALIGN_64BYTES),       // strideIn
            344,                                                         // widthOut
            32,                                                          // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            257                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 258) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                            // testPattern
            NULL,                                                              //*staticIn
            NULL,                                                              //*staticOut
            DSPLIB_FLOAT32,                                                    // dataType
            64,                                                                // widthIn
            377,                                                               // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(float), ALIGN_64BYTES),             // strideIn
            377,                                                               // widthOut
            64,                                                                // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(float), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                 // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                           // outputDataLocation
            1,                                                                 // numReps
            258                                                                // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 259) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            68,                                                          // widthIn
            448,                                                         // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(float), ALIGN_64BYTES),       // strideIn
            448,                                                         // widthOut
            68,                                                          // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            259                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 260) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                            // testPattern
            NULL,                                                              //*staticIn
            NULL,                                                              //*staticOut
            DSPLIB_FLOAT32,                                                    // dataType
            16,                                                                // widthIn
            640,                                                               // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(float), ALIGN_64BYTES),             // strideIn
            640,                                                               // widthOut
            16,                                                                // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(float), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                 // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                           // outputDataLocation
            1,                                                                 // numReps
            260                                                                // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 261) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            128,                                                         // widthIn
            700,                                                         // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(float), ALIGN_64BYTES),      // strideIn
            700,                                                         // widthOut
            128,                                                         // heightOut
            DSPLIB_CALC_STRIDE(700 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            261                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 262) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                            // testPattern
            NULL,                                                              //*staticIn
            NULL,                                                              //*staticOut
            DSPLIB_FLOAT32,                                                    // dataType
            32,                                                                // widthIn
            750,                                                               // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(float), ALIGN_64BYTES),             // strideIn
            750,                                                               // widthOut
            32,                                                                // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(float), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                 // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                           // outputDataLocation
            1,                                                                 // numReps
            262                                                                // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 263) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            64,                                                          // widthIn
            800,                                                         // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(float), ALIGN_64BYTES),       // strideIn
            800,                                                         // widthOut
            64,                                                          // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            263                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 264) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            48,                                                          // widthIn
            900,                                                         // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(float), ALIGN_64BYTES),       // strideIn
            900,                                                         // widthOut
            48,                                                          // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            264                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 265) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT32,                                              // dataType
            40,                                                          // widthIn
            950,                                                         // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(float), ALIGN_64BYTES),       // strideIn
            950,                                                         // widthOut
            40,                                                          // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(float), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            265                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 266) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT64,                                             // dataType
            1,                                                          // widthIn
            1,                                                          // heightIn
            DSPLIB_CALC_STRIDE(1 * sizeof(double), ALIGN_64BYTES),      // strideIn
            1,                                                          // widthOut
            1,                                                          // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            266                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 267) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT64,                                             // dataType
            16,                                                         // widthIn
            1,                                                          // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(double), ALIGN_64BYTES),     // strideIn
            1,                                                          // widthOut
            16,                                                         // heightOut
            DSPLIB_CALC_STRIDE(1 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            267                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 268) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT64,                                             // dataType
            30,                                                         // widthIn
            2,                                                          // heightIn
            DSPLIB_CALC_STRIDE(30 * sizeof(double), ALIGN_64BYTES),     // strideIn
            2,                                                          // widthOut
            30,                                                         // heightOut
            DSPLIB_CALC_STRIDE(2 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            268                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 269) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT64,                                             // dataType
            64,                                                         // widthIn
            4,                                                          // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(double), ALIGN_64BYTES),     // strideIn
            4,                                                          // widthOut
            64,                                                         // heightOut
            DSPLIB_CALC_STRIDE(4 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            269                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 270) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT64,                                             // dataType
            160,                                                        // widthIn
            7,                                                          // heightIn
            DSPLIB_CALC_STRIDE(160 * sizeof(double), ALIGN_64BYTES),    // strideIn
            7,                                                          // widthOut
            160,                                                        // heightOut
            DSPLIB_CALC_STRIDE(7 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            270                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 271) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                     // testPattern
            NULL,                                                       //*staticIn
            NULL,                                                       //*staticOut
            DSPLIB_FLOAT64,                                             // dataType
            288,                                                        // widthIn
            8,                                                          // heightIn
            DSPLIB_CALC_STRIDE(288 * sizeof(double), ALIGN_64BYTES),    // strideIn
            8,                                                          // widthOut
            288,                                                        // heightOut
            DSPLIB_CALC_STRIDE(8 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                          // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                    // outputDataLocation
            1,                                                          // numReps
            271                                                         // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 272) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT64,                                              // dataType
            512,                                                         // widthIn
            16,                                                          // heightIn
            DSPLIB_CALC_STRIDE(512 * sizeof(double), ALIGN_64BYTES),     // strideIn
            16,                                                          // widthOut
            512,                                                         // heightOut
            DSPLIB_CALC_STRIDE(16 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            272                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 273) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT64,                                              // dataType
            1000,                                                        // widthIn
            24,                                                          // heightIn
            DSPLIB_CALC_STRIDE(1000 * sizeof(double), ALIGN_64BYTES),    // strideIn
            24,                                                          // widthOut
            1000,                                                        // heightOut
            DSPLIB_CALC_STRIDE(24 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            273                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 274) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT64,                                              // dataType
            32,                                                          // widthIn
            32,                                                          // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(double), ALIGN_64BYTES),      // strideIn
            32,                                                          // widthOut
            32,                                                          // heightOut
            DSPLIB_CALC_STRIDE(32 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            274                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 275) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT64,                                              // dataType
            434,                                                         // widthIn
            39,                                                          // heightIn
            DSPLIB_CALC_STRIDE(434 * sizeof(double), ALIGN_64BYTES),     // strideIn
            39,                                                          // widthOut
            434,                                                         // heightOut
            DSPLIB_CALC_STRIDE(39 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            275                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 276) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT64,                                              // dataType
            192,                                                         // widthIn
            48,                                                          // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(double), ALIGN_64BYTES),     // strideIn
            48,                                                          // widthOut
            192,                                                         // heightOut
            DSPLIB_CALC_STRIDE(48 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            276                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 277) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                            // testPattern
            NULL,                                                              //*staticIn
            NULL,                                                              //*staticOut
            DSPLIB_FLOAT64,                                                    // dataType
            64,                                                                // widthIn
            64,                                                                // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(double), ALIGN_64BYTES),            // strideIn
            64,                                                                // widthOut
            64,                                                                // heightOut
            DSPLIB_CALC_STRIDE(64 * sizeof(double), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                 // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                           // outputDataLocation
            1,                                                                 // numReps
            277                                                                // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 278) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                      // testPattern
            NULL,                                                        //*staticIn
            NULL,                                                        //*staticOut
            DSPLIB_FLOAT64,                                              // dataType
            192,                                                         // widthIn
            96,                                                          // heightIn
            DSPLIB_CALC_STRIDE(192 * sizeof(double), ALIGN_64BYTES),     // strideIn
            96,                                                          // widthOut
            192,                                                         // heightOut
            DSPLIB_CALC_STRIDE(96 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                           // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                     // outputDataLocation
            1,                                                           // numReps
            278                                                          // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 279) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_FLOAT64,                                                     // dataType
            128,                                                                // widthIn
            128,                                                                // heightIn
            DSPLIB_CALC_STRIDE(128 * sizeof(double), ALIGN_64BYTES),            // strideIn
            128,                                                                // widthOut
            128,                                                                // heightOut
            DSPLIB_CALC_STRIDE(128 * sizeof(double), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            279                                                                 // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 280) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_FLOAT64,                                               // dataType
            112,                                                          // widthIn
            176,                                                          // heightIn
            DSPLIB_CALC_STRIDE(112 * sizeof(double), ALIGN_64BYTES),      // strideIn
            176,                                                          // widthOut
            112,                                                          // heightOut
            DSPLIB_CALC_STRIDE(176 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            280                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 281) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_FLOAT64,                                                     // dataType
            132,                                                                // widthIn
            192,                                                                // heightIn
            DSPLIB_CALC_STRIDE(132 * sizeof(double), ALIGN_64BYTES),            // strideIn
            192,                                                                // widthOut
            132,                                                                // heightOut
            DSPLIB_CALC_STRIDE(192 * sizeof(double), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            281                                                                 // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 282) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_FLOAT64,                                               // dataType
            224,                                                          // widthIn
            224,                                                          // heightIn
            DSPLIB_CALC_STRIDE(224 * sizeof(double), ALIGN_64BYTES),      // strideIn
            224,                                                          // widthOut
            224,                                                          // heightOut
            DSPLIB_CALC_STRIDE(224 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            282                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 283) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_FLOAT64,                                               // dataType
            52,                                                           // widthIn
            262,                                                          // heightIn
            DSPLIB_CALC_STRIDE(52 * sizeof(double), ALIGN_64BYTES),       // strideIn
            262,                                                          // widthOut
            52,                                                           // heightOut
            DSPLIB_CALC_STRIDE(262 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            283                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 284) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_FLOAT64,                                                     // dataType
            144,                                                                // widthIn
            312,                                                                // heightIn
            DSPLIB_CALC_STRIDE(144 * sizeof(double), ALIGN_64BYTES),            // strideIn
            312,                                                                // widthOut
            144,                                                                // heightOut
            DSPLIB_CALC_STRIDE(312 * sizeof(double), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            284                                                                 // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 285) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_FLOAT64,                                               // dataType
            32,                                                           // widthIn
            344,                                                          // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(double), ALIGN_64BYTES),       // strideIn
            344,                                                          // widthOut
            32,                                                           // heightOut
            DSPLIB_CALC_STRIDE(344 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            285                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 286) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_FLOAT64,                                                     // dataType
            64,                                                                 // widthIn
            377,                                                                // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(double), ALIGN_64BYTES),             // strideIn
            377,                                                                // widthOut
            64,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(377 * sizeof(double), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            286                                                                 // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 287) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_FLOAT64,                                                     // dataType
            68,                                                                 // widthIn
            448,                                                                // heightIn
            DSPLIB_CALC_STRIDE(68 * sizeof(double), ALIGN_64BYTES),             // strideIn
            448,                                                                // widthOut
            68,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(448 * sizeof(double), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            287                                                                 // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 288) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_FLOAT64,                                                     // dataType
            16,                                                                 // widthIn
            640,                                                                // heightIn
            DSPLIB_CALC_STRIDE(16 * sizeof(double), ALIGN_64BYTES),             // strideIn
            640,                                                                // widthOut
            16,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(640 * sizeof(double), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            288                                                                 // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 289) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_FLOAT64,                                               // dataType
            32,                                                           // widthIn
            750,                                                          // heightIn
            DSPLIB_CALC_STRIDE(32 * sizeof(double), ALIGN_64BYTES),       // strideIn
            750,                                                          // widthOut
            32,                                                           // heightOut
            DSPLIB_CALC_STRIDE(750 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            289                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 290) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_FLOAT64,                                               // dataType
            64,                                                           // widthIn
            800,                                                          // heightIn
            DSPLIB_CALC_STRIDE(64 * sizeof(double), ALIGN_64BYTES),       // strideIn
            800,                                                          // widthOut
            64,                                                           // heightOut
            DSPLIB_CALC_STRIDE(800 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            290                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 291) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                       // testPattern
            NULL,                                                         //*staticIn
            NULL,                                                         //*staticOut
            DSPLIB_FLOAT64,                                               // dataType
            48,                                                           // widthIn
            900,                                                          // heightIn
            DSPLIB_CALC_STRIDE(48 * sizeof(double), ALIGN_64BYTES),       // strideIn
            900,                                                          // widthOut
            48,                                                           // heightOut
            DSPLIB_CALC_STRIDE(900 * sizeof(double), OUTPUT_ALIGN_BYTES), // strideOut
            0,                                                            // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                      // outputDataLocation
            1,                                                            // numReps
            291                                                           // testID
        },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CASE == 292) || (TEST_CATEGORY == 100))
        {
            RANDOM,                                                             // testPattern
            NULL,                                                               //*staticIn
            NULL,                                                               //*staticOut
            DSPLIB_FLOAT64,                                                     // dataType
            40,                                                                 // widthIn
            950,                                                                // heightIn
            DSPLIB_CALC_STRIDE(40 * sizeof(double), ALIGN_64BYTES),             // strideIn
            950,                                                                // widthOut
            40,                                                                 // heightOut
            DSPLIB_CALC_STRIDE(950 * sizeof(double), OUTPUT_ALIGN_BYTES) + 128, // strideOut
            0,                                                                  // shift
            DSPLIB_TEST_OUTPUT_MSMC,                                            // outputDataLocation
            1,                                                                  // numReps
            292                                                                 // testID
        },
#endif
};

/*
 *  Sends the test parameter structure and number of tests
 */
void matTrans_getTestParams(matTrans_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(matTrans_testParams_t);
}

/* ======================================================================== */
