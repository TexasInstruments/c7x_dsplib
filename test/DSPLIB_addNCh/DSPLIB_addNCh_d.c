// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_addNCh_idat.h"

#define DSPLIB_ROW_STRIDE(x, y) (((x + y) / y) * y)

#ifdef WIN32
#if defined(__C7504__)
int8_t l2auxBuffer[DSPLIB_L2_BUFFER_SIZE];
int8_t ddrBuffer[2048 * 1024];
#else
int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
int8_t ddrBuffer[2048 * 1024];
#endif
#else
#if defined(__C7504__) || defined(__C7524__)
__attribute__((section(".l2sramaux"), aligned(64))) int8_t l2auxBuffer[DSPLIB_L2_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t   ddrBuffer[2048 * 1024];

#else
__attribute__((section(".msmcData"), aligned(128))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t   ddrBuffer[2048 * 1024];

#endif
#endif // WIN32

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int DSPLIB_addNCh_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams3D_t bufParamsIn;
   DSPLIB_bufParams2D_t bufParamsOut;
   uint64_t             archCycles = 0;
   uint64_t             estCycles  = 0;

   uint32_t testNum;
   uint32_t comparisonDone = 0;

   DSPLIB_addNCh_testParams_t *prm;
   DSPLIB_addNCh_testParams_t  currPrm;
   DSPLIB_addNCh_getTestParams(&prm, &test_cases);

   DSPLIB_addNCh_InitArgs kerInitArgs;

   DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 0\n");

   int32_t             handleSize = DSPLIB_addNCh_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 1\n");

   TI_profile_init("DSPLIB_addNCh");

   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_addNCh.csv", "w+");
   fprintf(fpOutputCSV, "Test ID, Bit Width, inputs, inChannels, inSamples, "
                        "estCycles, Loki cycles, Pass/Fail\n");

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         DSPLIB_DEBUGPRINTFN(0, "Current TestID: %d Current Repetition: %d\n", currPrm.testID, repCount + 1);

         int32_t       status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t       status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t       status_ref_vs_nat = TI_TEST_KERNEL_FAIL;
         DSPLIB_STATUS status_init       = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt        = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat        = DSPLIB_SUCCESS;
         archCycles                      = 0;
         estCycles                       = 0;

         currentTestFail = 0;

         bufParamsIn.data_type  = currPrm.dataType;
         bufParamsIn.dim_x      = currPrm.inSamples;
         bufParamsIn.dim_y      = currPrm.inChannels;
         bufParamsIn.dim_z      = currPrm.inInputs;
         bufParamsIn.stride_y   = currPrm.strideIn;
         bufParamsIn.stride_z   = bufParamsIn.stride_y * bufParamsIn.dim_y;
         bufParamsOut.data_type = currPrm.dataType;
         bufParamsOut.dim_x     = currPrm.inSamples;
         bufParamsOut.dim_y     = currPrm.inChannels;
         bufParamsOut.stride_y  = currPrm.strideIn;

         int32_t dataWidth = DSPLIB_sizeof(currPrm.dataType);

         uint32_t inpSizeBytes = bufParamsIn.dim_z * bufParamsIn.stride_z;
         uint32_t outSizeBytes = bufParamsOut.dim_y * bufParamsOut.stride_y;

         void *pIn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, inpSizeBytes);

         void *pOut, *pOutCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, outSizeBytes);
            pOutCn = (void *) malloc(outSizeBytes);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, outSizeBytes);
#else
            pOut = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, outSizeBytes);
#else
            pOut = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }

         DSPLIB_DEBUGPRINTFN(0, "pIn: %p pOut: %p pOutCn: %p\n", pIn, pOut, pOutCn);

         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn && pOut && pOutCn) {

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {

               TI_fillBuffer3D_float(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x,
                                     bufParamsIn.dim_y, bufParamsIn.dim_z, bufParamsIn.stride_y, bufParamsIn.stride_z,
                                     dataWidth, testPatternString);
            }
            else {
            }

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 0\n");

            status_init = DSPLIB_addNCh_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 1 status_init %d\n", status_init);

            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init           = DSPLIB_addNCh_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 2 status_init %d\n", status_init);

            status_opt = DSPLIB_addNCh_exec_checkParams(handle, pIn, pOut);

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 3 status_opt %d\n", status_opt);

            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_addNCh_exec(handle, pIn, pOut);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
            }

#if !defined(__C7X_HOSTEM__)

            /* The following for loop is to call kernel repeatedly so as to
             * train the branch predictor                                   */
            uint32_t k;
            for (k = 0; k < 4; k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               DSPLIB_asm(" MARK 4");
               status_opt = DSPLIB_addNCh_exec(handle, pIn, pOut);
               DSPLIB_asm(" MARK 5");
               TI_profile_stop();
            }

            // get output to L1D
            int16_t outSum   = 0;
            int8_t *pOutTemp = (int8_t *) pOut; // treat output as bytes to be data type agnostic
            for (k = 0; k < bufParamsOut.dim_x; k++) {
               outSum += *pOutTemp;
               pOutTemp++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum = outSum;

            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            DSPLIB_asm(" MARK 6");
            status_opt = DSPLIB_addNCh_exec(handle, pIn, pOut);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();

#endif // #if !defined(__C7X_HOSTEM__)
            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 4 status_opt %d\n", status_opt);

            // initialize the kernel to use the natural C variant
            DSPLIB_addNCh_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 5\n", status_init);

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            status_nat = DSPLIB_addNCh_exec(handle, pIn, pOutCn);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 6 status_nat %d\n", status_nat);

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               status_nat_vs_opt =
                   TI_compare_mem_2D_float((void *) pOut, (void *) pOutCn, 0, (double) powf(2, -10), bufParamsOut.dim_x,
                                           bufParamsOut.dim_y, bufParamsOut.stride_y, DSPLIB_sizeof(currPrm.dataType));
            }

            comparisonDone = 1;

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 7 comparisonDone %d status_nat_vs_opt %d\n",
                                comparisonDone, status_nat_vs_opt);

            if (currPrm.staticOut != NULL) {

               if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {

                  status_ref_vs_opt = TI_compare_mem_2DDualStride_float(
                      (void *) pOut, (void *) currPrm.staticOut, 0, (double) powf(2, -10), bufParamsOut.dim_x,
                      bufParamsOut.dim_y, bufParamsOut.stride_y, bufParamsOut.dim_x * DSPLIB_sizeof(currPrm.dataType),
                      DSPLIB_sizeof(currPrm.dataType));

                  status_ref_vs_nat = TI_compare_mem_2DDualStride_float(
                      (void *) pOutCn, (void *) currPrm.staticOut, 0, (double) powf(2, -10), bufParamsOut.dim_x,
                      bufParamsOut.dim_y, bufParamsOut.stride_y, bufParamsOut.dim_x * DSPLIB_sizeof(currPrm.dataType),
                      DSPLIB_sizeof(currPrm.dataType));
               }

               comparisonDone = 1;
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

            DSPLIB_DEBUGPRINTFN(0,
                                "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 8 status_nat_vs_opt %d status_ref_vs_opt "
                                "%d currentTestFail "
                                "%d\n",
                                status_nat_vs_opt, status_ref_vs_opt, currentTestFail);

            DSPLIB_DEBUGPRINTFN(0,
                                "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 8 status_init %d status_opt %d status_nat %d\n",
                                status_init, status_opt, status_nat);

            /* Set the 'fail' flag based on test vector comparison results */

            if (currPrm.testPattern == RANDOM) {
               status_ref_vs_nat = TI_TEST_KERNEL_PASS;
            }

            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_nat == TI_TEST_KERNEL_FAIL) || (status_init != DSPLIB_SUCCESS) ||
                 (status_opt != DSPLIB_SUCCESS) || (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0) ||
                 (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_addNCh_d CP 8 fail %d\n", fail);

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | nInputs = %d, inSamples = %d, inChannels = %d", testPatternString,
                    currPrm.inInputs, currPrm.inSamples, currPrm.inChannels);

            // write to CSV, must happen prior to write to screen because
            // TI_profile_formula_add clears values in counters
            DSPLIB_addNCh_perfEst(handle, &archCycles, &estCycles, currPrm.dataType);

            fprintf(fpOutputCSV, "%d, %d, %d, %d, %d, %ld,%d , %d\n", currPrm.testID,
                    DSPLIB_sizeof(currPrm.dataType) * 8, currPrm.inInputs, currPrm.inChannels, currPrm.inSamples,
                    estCycles, pProfile[3 * tpi + 2], !currentTestFail);

            TI_profile_add_test(testNum++, (currPrm.inInputs * currPrm.inSamples * currPrm.inChannels), archCycles,
                                estCycles, currentTestFail, desc);
         }
         else {
            sprintf(desc, "%s data does not fit in memory | inSamples = %d, inChannels = %d", testPatternString,
                    currPrm.inSamples, currPrm.inChannels);
            TI_profile_skip_test(desc);

            // clear the counters between runs; normally handled by TI_profile_addNCh_test
            TI_profile_clear_run_stats();

         } // end of memory allocation successful?

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            TI_align_free(pOut);
            free(pOutCn);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
            TI_align_free(pOut);
         }
         else {
            TI_align_free(pOut);
         }

         TI_align_free(pIn);

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
      TI_memError("DSPLIB_addNCh");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_addNCh_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t                testNum         = 1000;
   int32_t                currentTestFail = 0;
   DSPLIB_STATUS          status_nat;
   DSPLIB_STATUS          status_opt;
   DSPLIB_bufParams3D_t   bufParamsIn;
   DSPLIB_bufParams2D_t   bufParamsOut;
   DSPLIB_addNCh_InitArgs kerInitArgs;
   int32_t                handleSize = DSPLIB_addNCh_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle    handle     = malloc(handleSize);
   int                    fail       = 0;
   void                  *pIn        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                  *pOut       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);

   bufParamsIn.dim_x  = 16;
   bufParamsIn.dim_y  = 16;
   bufParamsIn.dim_z  = 16;
   bufParamsOut.dim_x = 16;
   while (testNum <= 1006) {

      switch (testNum) {
      case 1000:
         bufParamsIn.data_type  = DSPLIB_INT32;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_addNCh_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_addNCh_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         bufParamsIn.data_type  = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_addNCh_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_addNCh_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsIn.data_type  = DSPLIB_FLOAT32;
         bufParamsOut.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_addNCh_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_addNCh_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         status_nat      = DSPLIB_addNCh_exec_checkParams(handle, NULL, pOut);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1004:
         status_nat      = DSPLIB_addNCh_exec_checkParams(handle, pIn, NULL);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1005:
         bufParamsIn.data_type  = DSPLIB_INT64;
         bufParamsOut.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_addNCh_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_addNCh_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   TI_align_free(pIn);
   TI_align_free(pOut);
   free(handle);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_addNCh_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_addNCh_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_addNCh_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}
/* Main call for inaddNChidual test projects */
#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
int main()
{
   int test_fail = 1, cov_fail = 1;

   uint32_t profile[1024 * 3];

   DSPLIB_TEST_init();

   test_fail = test_main(&profile[0]);
   cov_fail  = coverage_test_main();

#if !defined(NO_PRINTF)
   if ((test_fail == 0) && (cov_fail == 0))
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");

   int i;
   for (i = 0; i < test_cases; i++) {
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", i, profile[3 * i],
             profile[3 * i + 1], profile[3 * i + 2]);
   }
#endif

   return test_fail && cov_fail;
}
#endif
