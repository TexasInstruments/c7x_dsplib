// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_cascadeBiquad_idat.h"

#if defined(__C7504__) || defined(__C7524__)
__attribute__((section(".l2sramaux"), aligned(64))) int8_t l2auxBuffer[DSPLIB_L2_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t   ddrBuffer[2048 * 1024];

#else
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];
#endif

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int DSPLIB_cascadeBiquad_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             execRepCount;
   uint32_t             numExecReps;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut, bufParamsFilterVar;
   DSPLIB_bufParams2D_t bufParamsFilterCoeff;   // coefficient buffer descriptor for optimized code
   DSPLIB_bufParams2D_t bufParamsFilterCoeffCn; // coefficient buffer descriptor for natural-C code

   uint32_t k = 0;

   uint32_t testNum;
   uint64_t archCycles, estCycles = 0;

   DSPLIB_cascadeBiquad_testParams_t *prm;
   DSPLIB_cascadeBiquad_testParams_t  currPrm;
   DSPLIB_cascadeBiquad_getTestParams(&prm, &test_cases);

   DSPLIB_cascadeBiquad_InitArgs kerInitArgs;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 0\n");
#endif

   int32_t             handleSize = DSPLIB_cascadeBiquad_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 1\n");
#endif

   TI_profile_init("DSPLIB_cascadeBiquad");

   FILE *fpOutputCSV = fopen("DSPLIB_cascadeBiquad.csv", "w+");
   fprintf(fpOutputCSV,
           "Kernel, Test ID, Test type, Bit Width, Parameters, dataSize, numChannels, numStages, "
           "biquadForm, enableNchCoeffs, Arch cycles, Estimated cycles, Warm Cache cycles, Est/Warm cycles, "
           "Pass/Fail, Cold Cycles, Warm Cycles, Warm Cycles WRB\n");

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         int32_t       status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t       status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
         DSPLIB_STATUS status_coefs      = DSPLIB_ERR_FAILURE;
         DSPLIB_STATUS status_init_opt   = DSPLIB_ERR_FAILURE;
         DSPLIB_STATUS status_exec_opt   = DSPLIB_ERR_FAILURE;
         DSPLIB_STATUS status_init_nat   = DSPLIB_ERR_FAILURE;
         DSPLIB_STATUS status_exec_nat   = DSPLIB_ERR_FAILURE;
         DSPLIB_STATUS status            = DSPLIB_ERR_FAILURE;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;

         currentTestFail = 0;

         numExecReps = currPrm.numExecReps;

         kerInitArgs.dataSize        = currPrm.dataSize;
         kerInitArgs.numChannels     = currPrm.numChannels;
         kerInitArgs.numStages       = currPrm.numStages;
         kerInitArgs.enableNChCoeffs = currPrm.enableNChCoeffs;
         kerInitArgs.biquadForm      = currPrm.biquadForm;

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsIn.dim_x     = currPrm.numChannels;
         bufParamsIn.dim_y     = currPrm.dataSize;
         bufParamsIn.stride_y  = currPrm.dataPitch;

         bufParamsOut.data_type = currPrm.dataType;
         bufParamsOut.dim_x     = currPrm.numChannels;
         bufParamsOut.dim_y     = currPrm.dataSize;
         bufParamsOut.stride_y  = currPrm.dataPitch;

         bufParamsFilterVar.data_type = currPrm.dataType;
         bufParamsFilterVar.dim_x     = currPrm.numChannels;
         bufParamsFilterVar.dim_y     = (currPrm.biquadForm == DSPLIB_BIQUAD_DF1)
                                            ? currPrm.numStages * DSPLIB_NUM_STATE_VARS_DF1
                                            : currPrm.numStages * DSPLIB_NUM_STATE_VARS_DF2;
         bufParamsFilterVar.stride_y  = currPrm.filterVarPitch;

         // Configure Natural-C coefficient buffer descriptor
         // Coefficients are arranged by channel, stage, and coefficient
         bufParamsFilterCoeffCn.data_type = currPrm.dataType;
         bufParamsFilterCoeffCn.dim_x     = currPrm.numStages * DSPLIB_NUM_BIQUAD_COEFS;
         bufParamsFilterCoeffCn.dim_y     = (currPrm.enableNChCoeffs == 0) ? 1 : currPrm.numChannels;
         bufParamsFilterCoeffCn.stride_y =
             bufParamsFilterCoeffCn.dim_x * DSPLIB_sizeof(bufParamsFilterCoeffCn.data_type);

         // Get optimized coefficient buffer descriptor
         status_coefs = DSPLIB_cascadeBiquad_get_coefficientsBufferDesc(currPrm.dataType, currPrm.numStages,
                                                                        currPrm.numChannels, currPrm.biquadForm,
                                                                        currPrm.enableNChCoeffs, &bufParamsFilterCoeff);
         if (status_coefs != DSPLIB_SUCCESS) {
            sprintf(TI_test_print_buffer, "\ntestNum %d: DSPLIB_cascadeBiquad_get_coefficientsBufferDesc FAIL\n",
                    testNum);
            TI_test_print_string(TI_test_print_buffer);
            break;
         }

         void *pIn =
             (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, numExecReps * bufParamsIn.dim_y * bufParamsIn.stride_y);

         void *pFilterCoeff =
             (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsFilterCoeff.dim_y * bufParamsFilterCoeff.stride_y);

         void *pFilterCoeffCn = malloc(bufParamsFilterCoeffCn.dim_y * bufParamsFilterCoeffCn.stride_y);

         void *pFilterCoeffTemp = malloc(bufParamsFilterCoeff.dim_y * bufParamsFilterCoeff.stride_y);

         void *pFilterVar =
             (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsFilterVar.stride_y * bufParamsFilterVar.dim_y);

         void *pFilterVarCn = malloc(bufParamsFilterVar.stride_y * bufParamsFilterVar.dim_y);

         void *pFilterVarTemp = malloc(bufParamsFilterVar.stride_y * bufParamsFilterVar.dim_y);

         void *pOut, *pOutCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pOut =
                (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, numExecReps * bufParamsOut.dim_y * bufParamsOut.stride_y);
            pOutCn = (void *) malloc(numExecReps * bufParamsOut.dim_y * bufParamsOut.stride_y);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            pOut =
                (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, numExecReps * bufParamsOut.dim_y * bufParamsOut.stride_y);
#else
            pOut = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            pOut =
                (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, numExecReps * bufParamsOut.dim_y * bufParamsOut.stride_y);
#else
            pOut = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }

         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn && pFilterCoeff && pFilterVar && pOut && pFilterCoeffCn && pFilterVarCn && pOutCn &&
             pFilterCoeffTemp && pFilterVarTemp) {

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d pIn %p pFilterCoeff %p pFilterCoeffCn %p pFilterVar %p "
                   "pFilterVarCn %p pOut %p pOutCn %p\n",
                   pIn, pFilterCoeff, pFilterCoeffCn, pFilterVar, pFilterVarCn, pOut, pOutCn);
#endif

            /* Set up the test patterns */

            // Initialize input buffer
            TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x,
                                numExecReps * bufParamsIn.dim_y, bufParamsIn.stride_y, sizeof(DSPLIB_F32),
                                testPatternString);

            // Initialize Natural-C coefficient buffer
            TI_fillBuffer_float(prm[tpi].testPattern, 0, pFilterCoeffCn, prm[tpi].staticFilterCoeff,
                                bufParamsFilterCoeffCn.dim_x, bufParamsFilterCoeffCn.dim_y,
                                bufParamsFilterCoeffCn.stride_y, sizeof(DSPLIB_F32), testPatternString);

            // Initialize optimized coefficient buffer
            if (prm[tpi].enableCalcCoeffs == 0) {
               // Initialize coefficients with static values

               if ((prm[tpi].biquadForm == DSPLIB_BIQUAD_TIF) || (prm[tpi].enableNChCoeffs == 1)) {
                  // Using pre-calculated optimized coefficients
                  TI_fillBuffer_float(prm[tpi].testPattern, 0, pFilterCoeff, prm[tpi].staticFilterCoeffOpt,
                                      bufParamsFilterCoeff.dim_x, bufParamsFilterCoeff.dim_y,
                                      bufParamsFilterCoeff.stride_y, sizeof(DSPLIB_F32), testPatternString);
               }
               else {
                  // Natural-C and optimized coefficients are the same
                  TI_fillBuffer_float(prm[tpi].testPattern, 0, pFilterCoeff, prm[tpi].staticFilterCoeff,
                                      bufParamsFilterCoeff.dim_x, bufParamsFilterCoeff.dim_y,
                                      bufParamsFilterCoeff.stride_y, sizeof(DSPLIB_F32), testPatternString);
               }
            }
            else { // (prm[tpi].enableCalcCoeffs == 1)
               // Get optimized coefficients using helper function
               status_coefs = DSPLIB_cascadeBiquad_get_coefficients(currPrm.biquadForm, currPrm.enableNChCoeffs,
                                                                    &bufParamsFilterCoeffCn, &bufParamsFilterCoeff,
                                                                    pFilterCoeffCn, pFilterCoeff);
               if (status_coefs != DSPLIB_SUCCESS) {
                  sprintf(TI_test_print_buffer, "\ntestNum %d: DSPLIB_cascadeBiquad_get_coefficients FAIL\n", testNum);
                  TI_test_print_string(TI_test_print_buffer);
                  break;
               }
            }

            memset(pFilterVar, 0, bufParamsFilterVar.dim_y * bufParamsFilterVar.stride_y);
            memcpy(pFilterVarCn, pFilterVar, bufParamsFilterVar.dim_y * bufParamsFilterVar.stride_y);
            memcpy(pFilterVarTemp, pFilterVar, bufParamsFilterVar.dim_y * bufParamsFilterVar.stride_y);
            memcpy(pFilterCoeffTemp, pFilterCoeff, bufParamsFilterCoeff.dim_y * bufParamsFilterCoeff.stride_y);

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 0\n");
#endif

            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init_opt       = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                                          &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 1 status_init %d\n", status_init);
#endif
            if (status_init_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               status_init_opt = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                           &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 2 status_init %d\n", status_init);
#endif

            if (status_init_opt == DSPLIB_SUCCESS) {
               status_exec_opt = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, pFilterCoeff, pFilterVar, pOut);
#if DSPLIB_DEBUGPRINT
               printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 3 status_opt %d\n", status_opt);
#endif
               if (status_exec_opt == DSPLIB_SUCCESS) {
                  for (execRepCount = 0; execRepCount < numExecReps; execRepCount++) {
                     TI_profile_start(TI_PROFILE_KERNEL_OPT);
                     DSPLIB_asm(" MARK 2");
                     status_exec_opt = DSPLIB_cascadeBiquad_exec(
                         handle, (uint8_t *) pIn + execRepCount * bufParamsIn.dim_y * bufParamsIn.stride_y,
                         pFilterCoeff, pFilterVar,
                         (uint8_t *) pOut + execRepCount * bufParamsOut.dim_y * bufParamsOut.stride_y);
                     DSPLIB_asm(" MARK 3");
                     TI_profile_stop();

                     if (status_exec_opt != DSPLIB_SUCCESS) {
                        break;
                     }
                  }
               }
            }
            status = ((status_init_opt == DSPLIB_SUCCESS) && (status_exec_opt == DSPLIB_SUCCESS)) ? DSPLIB_SUCCESS
                                                                                                  : DSPLIB_ERR_FAILURE;

            if (status == DSPLIB_SUCCESS) {
               /* The following for loop is to call kernel repeatedly so as to
                * train the branch predictor                                   */
               memcpy(pFilterVar, pFilterVarTemp, bufParamsFilterVar.dim_y * bufParamsFilterVar.stride_y);
               memcpy(pFilterCoeff, pFilterCoeffTemp, bufParamsFilterCoeff.dim_y * bufParamsFilterCoeff.stride_y);
               status_init_opt = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                           &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);

               if (status_init_opt == DSPLIB_SUCCESS) {
                  // run warm instruction cache test
                  TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);

                  for (execRepCount = 0; execRepCount < numExecReps; execRepCount++) {
                     TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
                     DSPLIB_asm(" MARK 4");
                     status_exec_opt = DSPLIB_cascadeBiquad_exec(
                         handle, (uint8_t *) pIn + execRepCount * bufParamsIn.dim_y * bufParamsIn.stride_y,
                         pFilterCoeff, pFilterVar,
                         (uint8_t *) pOut + execRepCount * bufParamsOut.dim_y * bufParamsOut.stride_y);
                     DSPLIB_asm(" MARK 5");
                     TI_profile_stop();

                     if (status_exec_opt != DSPLIB_SUCCESS) {
                        break;
                     }
                  }
               }
            }
            status = ((status_init_opt == DSPLIB_SUCCESS) && (status_exec_opt == DSPLIB_SUCCESS)) ? DSPLIB_SUCCESS
                                                                                                  : DSPLIB_ERR_FAILURE;

            if (status == DSPLIB_SUCCESS) {
               // get output to L1D
               int16_t outSum   = 0;
               int8_t *pOutTemp = (int8_t *) pOut; // treat output as bytes to be data type agnostic
               for (k = 0; k < bufParamsOut.dim_x; k++) {
                  outSum += *pOutTemp;
                  pOutTemp++;
               }

               // dummy store of outSum to insure that the compiler does not remove it.
               memcpy(pFilterVar, pFilterVarTemp, bufParamsFilterVar.dim_y * bufParamsFilterVar.stride_y);
               memcpy(pFilterCoeff, pFilterCoeffTemp, bufParamsFilterCoeff.dim_y * bufParamsFilterCoeff.stride_y);

               volatileSum = outSum;

               status_init_opt = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                           &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
               if (status_init_opt == DSPLIB_SUCCESS) {
                  // run warm instruction cache test

                  for (execRepCount = 0; execRepCount < numExecReps; execRepCount++) {
                     TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);
                     DSPLIB_asm(" MARK 6");
                     status_exec_opt = DSPLIB_cascadeBiquad_exec(
                         handle, (uint8_t *) pIn + execRepCount * bufParamsIn.dim_y * bufParamsIn.stride_y,
                         pFilterCoeff, pFilterVar,
                         (uint8_t *) pOut + execRepCount * bufParamsOut.dim_y * bufParamsOut.stride_y);
                     DSPLIB_asm(" MARK 7");
                     TI_profile_stop();

                     if (status_exec_opt != DSPLIB_SUCCESS) {
                        break;
                     }
                  }

#if DSPLIB_DEBUGPRINT
                  printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 4 status_opt %d\n", status_opt);
#endif
               }
            }
            status = ((status_init_opt == DSPLIB_SUCCESS) && (status_exec_opt == DSPLIB_SUCCESS)) ? DSPLIB_SUCCESS
                                                                                                  : DSPLIB_ERR_FAILURE;

            if (status == DSPLIB_SUCCESS) {
               /* Test _cn kernel */
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
               status_init_nat       = DSPLIB_cascadeBiquad_init_checkParams(
                         handle, &bufParamsIn, &bufParamsFilterCoeffCn, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);

               if (status_init_nat == DSPLIB_SUCCESS) {
                  // initialize the kernel to use the natural C variant
                  status_init_nat = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeffCn,
                                                              &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
               }

               if (status_init_nat == DSPLIB_SUCCESS) {
#if DSPLIB_DEBUGPRINT
                  printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 5\n");
#endif
                  for (execRepCount = 0; execRepCount < numExecReps; execRepCount++) {
                     TI_profile_start(TI_PROFILE_KERNEL_CN);
                     DSPLIB_asm(" MARK 8");
                     status_exec_nat = DSPLIB_cascadeBiquad_exec(
                         handle, (uint8_t *) pIn + execRepCount * bufParamsIn.dim_y * bufParamsIn.stride_y,
                         pFilterCoeffCn, pFilterVarCn,
                         (uint8_t *) pOutCn + execRepCount * bufParamsOut.dim_y * bufParamsOut.stride_y);
                     DSPLIB_asm(" MARK 9");
                     TI_profile_stop();

                     if (status_exec_nat != DSPLIB_SUCCESS) {
                        break;
                     }
                  }
#if DSPLIB_DEBUGPRINT
                  printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 6 status_nat %d pOut %p\n", status_nat, pOut);
#endif
               }
            }

            if ((status_init_nat == DSPLIB_SUCCESS) && (status_exec_nat == DSPLIB_SUCCESS)) {
               /* Compare Natural-C and Optimized outputs */
               status_nat_vs_opt =
                   TI_compare_mem_2D_float((void *) pOut, (void *) pOutCn, 0, (double) powf(2, -10), bufParamsOut.dim_x,
                                           numExecReps * bufParamsOut.dim_y, bufParamsOut.stride_y, sizeof(DSPLIB_F32));

               comparisonDone = 1;
            }

            if ((status_init_opt == DSPLIB_SUCCESS) && (status_exec_opt == DSPLIB_SUCCESS)) {
               /* Compare Reference and Optimized outputs */
               if (currPrm.staticOut != NULL) {
                  status_ref_vs_opt = TI_compare_mem_roi2DStatic_float(
                      (void *) pOut, (void *) currPrm.staticOut, 0, (double) powf(2, -10), bufParamsOut.dim_x,
                      numExecReps * bufParamsOut.dim_y, bufParamsOut.stride_y, sizeof(DSPLIB_F32));

                  comparisonDone = 1;
               }
               else {
                  /* Set to pass since it wasn't supposed to run. */
                  status_ref_vs_opt = TI_TEST_KERNEL_PASS;
               }
            }

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail = ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
                               (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) || (status_init_opt != DSPLIB_SUCCESS) ||
                               (status_exec_opt != DSPLIB_SUCCESS) || (status_init_nat != DSPLIB_SUCCESS) ||
                               (status_exec_nat != DSPLIB_SUCCESS) || (comparisonDone == 0) || (currentTestFail == 1))
                                  ? 1
                                  : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc,
                    "%s generated input | Data size = %d, numChannels = %d, numStages = %d, biquadForm = %d, "
                    "enableNChCoeffs = %d, numExecReps = %d",
                    testPatternString, currPrm.dataSize, currPrm.numChannels, currPrm.numStages, currPrm.biquadForm,
                    currPrm.enableNChCoeffs, currPrm.numExecReps);

            // Calculate estimated cycles
            // NOTE: scaling by number of execution repetitions
            DSPLIB_cascadeBiquad_perfEst(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar,
                                         &bufParamsOut, &archCycles, &estCycles);
            archCycles = archCycles * currPrm.numExecReps;
            estCycles  = estCycles * currPrm.numExecReps;

            // Write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            fprintf(fpOutputCSV,
                    "cascadeBiquad, %d, %d, %d, Data size:%d numChannels:%d numStages:%d biquadForm:%d "
                    "enableNChCoeffs:%d, %d, %d, %d, %d, %d, %lu, %lu, %lu, %.2f, %d, "
                    "%d, %d, %d\n",
                    testNum, currPrm.testPattern, DSPLIB_sizeof(bufParamsIn.data_type) * 8, currPrm.dataSize,
                    currPrm.numChannels, currPrm.numStages, currPrm.biquadForm, currPrm.enableNChCoeffs,
                    currPrm.dataSize, currPrm.numChannels, currPrm.numStages, currPrm.biquadForm,
                    currPrm.enableNChCoeffs, archCycles, estCycles, cycles[TI_PROFILE_KERNEL_OPT_WARM],
                    ((DSPLIB_F32) estCycles) / ((DSPLIB_F32) cycles[TI_PROFILE_KERNEL_OPT_WARM]), !currentTestFail,
                    pProfile[3 * tpi], pProfile[3 * tpi + 1], pProfile[3 * tpi + 2]);

            TI_profile_add_test(testNum++, currPrm.dataSize * currPrm.numChannels * currPrm.numStages, archCycles,
                                estCycles, currentTestFail, desc);
         }
         else {
            sprintf(desc, "dataSize=%d, numChannels=%d, numStages=%d, biquadForm=%d, enableNChCoeffs=%d",
                    currPrm.dataSize, currPrm.numChannels, currPrm.numStages, currPrm.biquadForm,
                    currPrm.enableNChCoeffs);
            TI_profile_skip_test(desc);
            // clear the counters between runs; normally handled by TI_profile_add_test
         } // end of memory allocation successful?

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         TI_align_free(pIn);
         TI_align_free(pFilterVar);
         free(pFilterVarCn);
         free(pFilterVarTemp);
         TI_align_free(pFilterCoeff);
         free(pFilterCoeffCn);
         free(pFilterCoeffTemp);

      } // end repetitions
   }    // end idat test cases

   free(handle);

   /* Close results CSV */
   fclose(fpOutputCSV);

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_cascadeBiquad");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_cascadeBiquad_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t                       testNum         = 1000;
   int32_t                       currentTestFail = 0;
   DSPLIB_STATUS                 status_nat, status_init, status_nat_init, status_opt_init;
   DSPLIB_bufParams2D_t          bufParamsIn, bufParamsOut, bufParamsFilterVar;
   DSPLIB_bufParams2D_t          bufParamsFilterCoeff, bufParamsFilterCoeffCn;
   DSPLIB_cascadeBiquad_InitArgs kerInitArgs;
   int32_t                       handleSize = DSPLIB_cascadeBiquad_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle           handle     = malloc(handleSize);
   int                           fail       = 0;

   int32_t  dataSize        = 32;
   int32_t  dataPitch       = 4 * 4;
   int32_t  filterVarPitch  = 4 * 4;
   int32_t  numChannels     = 2;
   int32_t  numStages       = 1;
   uint32_t enableNChCoeffs = 0;
   uint32_t biquadForm      = DSPLIB_BIQUAD_TIF;

   kerInitArgs.dataSize        = dataSize;
   kerInitArgs.numChannels     = numChannels;
   kerInitArgs.numStages       = numStages;
   kerInitArgs.enableNChCoeffs = enableNChCoeffs;
   kerInitArgs.biquadForm      = DSPLIB_BIQUAD_TIF;

   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = numChannels;
   bufParamsIn.dim_y     = dataSize;
   bufParamsIn.stride_y  = dataPitch;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = numChannels;
   bufParamsOut.dim_y     = dataSize;
   bufParamsOut.stride_y  = dataPitch;

   bufParamsFilterVar.data_type = DSPLIB_FLOAT32;
   bufParamsFilterVar.dim_x     = numChannels;
   bufParamsFilterVar.dim_y     = numStages * DSPLIB_NUM_STATE_VARS_DF2;
   bufParamsFilterVar.stride_y  = filterVarPitch;

   bufParamsFilterCoeffCn.data_type = DSPLIB_FLOAT32;
   bufParamsFilterCoeffCn.dim_x     = numStages * DSPLIB_NUM_BIQUAD_COEFS;
   bufParamsFilterCoeffCn.dim_y     = (enableNChCoeffs == 0) ? 1 : numChannels;
   bufParamsFilterCoeffCn.stride_y  = bufParamsFilterCoeffCn.dim_x * DSPLIB_sizeof(bufParamsFilterCoeffCn.data_type);

   DSPLIB_cascadeBiquad_get_coefficientsBufferDesc(DSPLIB_FLOAT32, numStages, numChannels, enableNChCoeffs, biquadForm,
                                                   &bufParamsFilterCoeff);

   void *pIn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.stride_y * bufParamsIn.dim_y);

   void *pFilterCoeff =
       (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsFilterCoeff.dim_y * bufParamsFilterCoeff.stride_y);

   void *pFilterCoeffCn = malloc(bufParamsFilterCoeffCn.dim_y * bufParamsFilterCoeffCn.stride_y);

   void *pFilterVar =
       (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsFilterVar.stride_y * bufParamsFilterVar.dim_y);

   void *pOut   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_y * bufParamsOut.stride_y);
   void *pOutCn = (void *) malloc(bufParamsOut.dim_y * bufParamsOut.stride_y);

   while (testNum <= 1016) {

      switch (testNum) {
      case 1000:
         status_init     = DSPLIB_cascadeBiquad_init_checkParams(NULL, &bufParamsIn, &bufParamsFilterCoeff,
                                                                 &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
         break;

      case 1001:
         bufParamsIn.data_type          = DSPLIB_INT64;
         bufParamsOut.data_type         = DSPLIB_INT64;
         bufParamsFilterCoeff.data_type = DSPLIB_INT64;
         kerInitArgs.funcStyle          = DSPLIB_FUNCTION_NATC;
         status_init     = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeffCn,
                                                                 &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
         status_nat_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeffCn, &bufParamsFilterVar,
                                                     &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar,
                                                     &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE) || (status_nat_init != DSPLIB_ERR_INVALID_TYPE) ||
                            (status_opt_init != DSPLIB_ERR_INVALID_TYPE));
         break;

      case 1002:
         bufParamsIn.data_type          = DSPLIB_FLOAT32;
         bufParamsOut.data_type         = DSPLIB_INT64;
         bufParamsFilterCoeff.data_type = DSPLIB_INT64;
         status_init     = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                                 &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE));
         break;

      case 1003:
         bufParamsIn.data_type          = DSPLIB_FLOAT32;
         bufParamsOut.data_type         = DSPLIB_FLOAT32;
         bufParamsFilterCoeff.data_type = DSPLIB_INT64;
         status_init     = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                                 &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE));
         break;

      case 1004:
         status_init     = DSPLIB_cascadeBiquad_exec_checkParams(handle, NULL, pFilterCoeff, pFilterVar, pOut);
         currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
         break;

      case 1005:
         status_init     = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, NULL, pFilterVar, pOut);
         currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
         break;

      case 1006:
         status_init     = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, pFilterCoeff, NULL, pOut);
         currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
         break;

      case 1007:
         status_init     = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, pFilterCoeff, pFilterVar, NULL);
         currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);

         break;
      case 1008: {

         bufParamsIn.data_type          = DSPLIB_FLOAT32;
         bufParamsOut.data_type         = DSPLIB_FLOAT32;
         bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
         bufParamsFilterVar.data_type   = DSPLIB_FLOAT32;
         kerInitArgs.numStages          = 1;
         kerInitArgs.funcStyle          = DSPLIB_FUNCTION_NATC;

         status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeffCn,
                                                             &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
         if (status_init == DSPLIB_SUCCESS) {
            status_nat_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeffCn,
                                                        &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
            if (status_nat_init == DSPLIB_SUCCESS) {
               status_nat = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, pFilterCoeffCn, pFilterVar, pOut);

               if (status_nat == DSPLIB_SUCCESS) {
                  status_nat = DSPLIB_cascadeBiquad_exec(handle, pIn, pFilterCoeffCn, pFilterVar, pOut);
               }
            }
         }
         currentTestFail = (status_nat != DSPLIB_SUCCESS);
      }

      case 1009:
         bufParamsIn.data_type          = DSPLIB_INT8;
         bufParamsOut.data_type         = DSPLIB_INT8;
         bufParamsFilterCoeff.data_type = DSPLIB_INT8;
         status_init     = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                                 &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE));
         break;

      case 1010:
         bufParamsIn.data_type          = DSPLIB_INT16;
         bufParamsOut.data_type         = DSPLIB_INT16;
         bufParamsFilterCoeff.data_type = DSPLIB_INT16;
         status_init     = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                                 &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE));
         break;

      case 1011:
         bufParamsIn.data_type          = DSPLIB_INT32;
         bufParamsOut.data_type         = DSPLIB_INT32;
         bufParamsFilterCoeff.data_type = DSPLIB_INT32;
         status_init     = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                                 &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE));
         break;

      case 1012:
         bufParamsIn.data_type          = DSPLIB_FLOAT32;
         bufParamsOut.data_type         = DSPLIB_FLOAT32;
         bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
         kerInitArgs.numStages          = 7;
         kerInitArgs.numChannels        = 0;
         kerInitArgs.funcStyle          = DSPLIB_FUNCTION_OPTIMIZED;
         status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                             &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);

         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_VALUE));
         break;

      case 1013:
         bufParamsIn.data_type          = DSPLIB_FLOAT32;
         bufParamsOut.data_type         = DSPLIB_FLOAT32;
         bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
         kerInitArgs.numStages          = 3;
         kerInitArgs.numChannels        = 0;
         kerInitArgs.funcStyle          = DSPLIB_FUNCTION_OPTIMIZED;
         status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                             &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);

         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_VALUE));
         break;

      case 1014:
         bufParamsIn.data_type          = DSPLIB_FLOAT32;
         bufParamsOut.data_type         = DSPLIB_FLOAT32;
         bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
         kerInitArgs.numStages          = 3;
         kerInitArgs.numChannels        = 8;
         kerInitArgs.funcStyle          = DSPLIB_FUNCTION_OPTIMIZED;
         kerInitArgs.enableNChCoeffs    = 2;
         status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                             &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);

         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_VALUE));
         break;

      case 1015:
         bufParamsIn.data_type          = DSPLIB_FLOAT32;
         bufParamsOut.data_type         = DSPLIB_FLOAT32;
         bufParamsIn.stride_y           = dataPitch;
         bufParamsOut.stride_y          = 2 * dataPitch;
         bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
         kerInitArgs.numStages          = 3;
         kerInitArgs.numChannels        = 8;
         kerInitArgs.funcStyle          = DSPLIB_FUNCTION_OPTIMIZED;
         kerInitArgs.enableNChCoeffs    = 0;
         status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                             &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);

         currentTestFail = ((status_init != DSPLIB_ERR_NOT_IMPLEMENTED));
         break;

      case 1016:
         bufParamsIn.data_type          = DSPLIB_FLOAT32;
         bufParamsOut.data_type         = DSPLIB_FLOAT32;
         bufParamsIn.stride_y           = dataPitch;
         bufParamsOut.stride_y          = 2 * dataPitch;
         bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
         kerInitArgs.numStages          = 3;
         kerInitArgs.numChannels        = 8;
         kerInitArgs.funcStyle          = DSPLIB_FUNCTION_OPTIMIZED;
         kerInitArgs.enableNChCoeffs    = 0;
         status_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar,
                                                 &bufParamsOut, &kerInitArgs);

         currentTestFail = ((status_init != DSPLIB_ERR_NOT_IMPLEMENTED));
         break;

      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }

   TI_align_free(pIn);
   TI_align_free(pFilterCoeff);
   free(pFilterCoeffCn);
   TI_align_free(pFilterVar);
   TI_align_free(pOut);
   free(pOutCn);
   free(handle);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_cascadeBiquad_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_cascadeBiquad_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_cascadeBiquad_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for individual test projects */
#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
int main()
{
   int fail = 1;

   uint32_t profile[1024 * 3]; // up to 1024 test supported

   DSPLIB_TEST_init();

   fail = test_main(&profile[0]);

#if !defined(NO_PRINTF)
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");

   int i;
   for (i = 0; i < test_cases; i++) {
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", i, profile[3 * i],
             profile[3 * i + 1], profile[3 * i + 2]);
   }
#endif

   fail = coverage_test_main();
   if (fail == 0) {
      printf("Test Pass!\n");
   }
   else {
      printf("Test Fail!\n");
   }

   return fail;
}
#endif
