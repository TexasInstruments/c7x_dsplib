// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_fltoq15_idat.h"

#ifdef WIN32
#if defined(__C7504__) || defined(__C7524__)
int8_t l2auxBuffer[DSPLIB_L2_BUFFER_SIZE];
int8_t ddrBuffer[2048 * 1024];
#else
__attribute__((section(".msmcData"), aligned(128))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t   ddrBuffer[2048 * 1024];
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

int DSPLIB_fltoq15_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

#if !defined(EVM_TEST)
   uint32_t k;
   uint64_t hwCycles;
#endif //#if !defined(EVM_TEST)

   uint32_t              testNum;
   fltoq15_testParams_t *prm;
   fltoq15_testParams_t  currPrm;
   fltoq15_getTestParams(&prm, &test_cases);

   DSPLIB_fltoq15_InitArgs kerInitArgs;
   int32_t                 handleSize = DSPLIB_fltoq15_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle     handle     = malloc(handleSize);

   TI_profile_init("DSPLIB_fltoq15");

#if !defined(EVM_TEST)
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_fltoq15.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Bit Width, Parameters, Data size, Batch size, Filter size, HW "
                        "cycles, Pass/Fail, Per vector cycle\n");
#endif //#if !defined(EVM_TEST)

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         DSPLIB_DEBUGPRINTFN(0, "Current TestID: %d Current Repetition: %d\n", currPrm.testID, repCount + 1);

         int32_t       status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t       status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
         DSPLIB_STATUS status_init       = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt        = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat        = DSPLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;

         currentTestFail = 0;

         kerInitArgs.dataSize = currPrm.dataSize;

         bufParamsIn.data_type = currPrm.dataTypeIn;
         bufParamsIn.dim_x     = currPrm.dataSize;

         bufParamsOut.data_type = currPrm.dataTypeOut;
         bufParamsOut.dim_x     = currPrm.dataSize;

         DSPLIB_DEBUGPRINTFN(0, "I/P data_type: %d O/P data_type: %d dataSize: %d\n", bufParamsIn.data_type,
                             bufParamsOut.data_type, kerInitArgs.dataSize);

         int32_t dataWidth;

         if (currPrm.dataTypeIn == DSPLIB_FLOAT32) {
            dataWidth = sizeof(DSPLIB_F32);
         }
         else if (currPrm.dataTypeIn == DSPLIB_FLOAT64) {
            dataWidth = sizeof(DSPLIB_D64);
         }
         else {
            dataWidth = DSPLIB_sizeof(currPrm.dataTypeIn);
         }

         void *pIn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.dim_x * dataWidth);

         void *pOut, *pOutCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_x * dataWidth);
            pOutCn = (void *) malloc(bufParamsOut.dim_x * dataWidth);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_x * dataWidth);
#else
            pOut = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_x * dataWidth);
#else
            pOut = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }

         DSPLIB_DEBUGPRINTFN(0, "pIn: %p pOut: %p pOutCn: %p\n", pIn, pOut, pOutCn);

         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn && pOut && pOutCn) {
            if (currPrm.dataTypeIn == DSPLIB_FLOAT32 || currPrm.dataTypeIn == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x, 1, 0,
                                   DSPLIB_sizeof(currPrm.dataTypeIn), testPatternString);
            }
            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pIn, prm[tpi].staticIn, bufParamsIn.dim_x, 1, 0,
                             DSPLIB_sizeof(currPrm.dataTypeIn), testPatternString);
            }

            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init           = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               status_init = DSPLIB_fltoq15_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }
            status_opt = DSPLIB_fltoq15_exec_checkParams(handle, pIn, pOut);

            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_fltoq15_exec(handle, pIn, pOut);
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
               status_opt = DSPLIB_fltoq15_exec(handle, pIn, pOut);
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
            status_opt = DSPLIB_fltoq15_exec(handle, pIn, pOut);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();
#endif //#if !defined(__C7X_HOSTEM__)

            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
            status_init           = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 8");
               status_init = DSPLIB_fltoq15_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 9");
               TI_profile_stop();
            }

            status_opt = DSPLIB_fltoq15_exec_checkParams(handle, pIn, pOutCn);
            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CN);
               DSPLIB_asm(" MARK 10");
               status_opt = DSPLIB_fltoq15_exec(handle, pIn, pOutCn);
               DSPLIB_asm(" MARK 11");
               TI_profile_stop();
            }

            if (currPrm.dataTypeOut == DSPLIB_FLOAT32 || currPrm.dataTypeOut == DSPLIB_FLOAT64) {
               status_nat_vs_opt =
                   TI_compare_mem_2D_float((void *) pOut, (void *) pOutCn, 0.0001, (double) powf(2, -10),
                                           bufParamsOut.dim_x, 1, 0, DSPLIB_sizeof(currPrm.dataTypeOut));
            }
            else {
               status_nat_vs_opt = TI_compare_mem_2D((void *) pOut, (void *) pOutCn, 0, 0, bufParamsOut.dim_x, 1, 0,
                                                     DSPLIB_sizeof(currPrm.dataTypeOut));
            }

            comparisonDone = 1;

            if (currPrm.staticOut != NULL) {
               status_ref_vs_opt = TI_compare_mem_2D(
                   (void *) currPrm.staticOut, (void *) pOut, 2, 65535, bufParamsOut.dim_x, 1,
                   bufParamsOut.dim_x * DSPLIB_sizeof(bufParamsOut.data_type), DSPLIB_sizeof(bufParamsOut.data_type));

               comparisonDone = 1;
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

            DSPLIB_DEBUGPRINTFN(0, "status_nat_vs_opt: %d status_ref_vs_opt: %d currentTestFail: %d\n",
                                status_nat_vs_opt, status_ref_vs_opt, currentTestFail);

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) || (status_nat != DSPLIB_SUCCESS) ||
                 (comparisonDone == 0) || (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | Data size = %d Data Type %d", testPatternString, currPrm.dataSize,
                    currPrm.dataTypeIn);
            uint64_t hwCycles_local = 0;

#if !defined(EVM_TEST)
            float    per_vector_cycles;
            uint64_t num_se_fetches =
                ((currPrm.dataSize * DSPLIB_sizeof(currPrm.dataTypeIn)) + (__C7X_VEC_SIZE_BYTES__ - 1)) /
                __C7X_VEC_SIZE_BYTES__;
            hwCycles          = pProfile[3 * tpi + 1];
            hwCycles_local    = hwCycles;
            per_vector_cycles = (float_t) ((double_t) hwCycles / (double_t) num_se_fetches);
            fprintf(fpOutputCSV, "fltoq15, %d, %d, %d, -, %d, 1, 1, %d, %d, %3.2f\n", testNum, currPrm.testPattern,
                    DSPLIB_sizeof(bufParamsIn.data_type) * 8, currPrm.dataSize, (int) hwCycles, !currentTestFail,
                    per_vector_cycles);
#endif //#if !defined(EVM_TEST)
            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum++, currPrm.dataSize, hwCycles_local, 0, currentTestFail, desc);
         }
         else {
            sprintf(desc, "Data size=%d", currPrm.dataSize);

            DSPLIB_DEBUGPRINTFN(0, "%s\n", "Skipping Test");

            TI_profile_skip_test(desc);
            // clear the counters between runs;
            TI_profile_clear_run_stats();
         } // end of memory allocation not successful

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pOut);
#endif
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pOut);

#endif
         }
         TI_align_free(pIn);

      } // end repetitions
   }    // end idat test cases

   free(handle);

#if !defined(EVM_TEST)
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif // #if !defined(EVM_TEST)

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return fail: %d\n", fail);

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_fltoq15");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_fltoq15_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t                 testNum         = 1000;
   int32_t                 currentTestFail = 0;
   DSPLIB_STATUS           status_nat;
   DSPLIB_STATUS           status_opt;
   DSPLIB_bufParams1D_t    bufParamsIn, bufParamsOut;
   DSPLIB_fltoq15_InitArgs kerInitArgs;
   int32_t                 handleSize = DSPLIB_fltoq15_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle     handle     = malloc(handleSize);
   int                     fail       = 0;
   void                   *pIn        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                   *pOut       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);

   bufParamsIn.dim_x  = 16;
   bufParamsOut.dim_x = 16;
   while (testNum <= 1007) {

      switch (testNum) {
      case 1000:
         bufParamsIn.data_type  = DSPLIB_INT32;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_fltoq15_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_fltoq15_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         bufParamsIn.data_type  = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsIn.data_type  = DSPLIB_INT32;
         bufParamsOut.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         status_nat      = DSPLIB_fltoq15_exec_checkParams(handle, NULL, pOut);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1004:
         status_nat      = DSPLIB_fltoq15_exec_checkParams(handle, pIn, NULL);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1005:
         bufParamsIn.data_type  = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_fltoq15_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_fltoq15_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1006:
         bufParamsIn.data_type  = DSPLIB_FLOAT32;
         bufParamsOut.data_type = DSPLIB_INT32;
         status_nat             = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = (status_nat != DSPLIB_ERR_INVALID_TYPE);
         break;
      case 1007: {
         bufParamsIn.dim_x      = 8;
         bufParamsIn.data_type  = DSPLIB_FLOAT32;
         bufParamsOut.dim_x     = 8;
         bufParamsOut.data_type = DSPLIB_INT16;

         void *pIn =
             (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.dim_x * DSPLIB_sizeof(bufParamsIn.data_type));
         void *pOut =
             (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_x * DSPLIB_sizeof(bufParamsOut.data_type));

         float  *pInFill = (float *) pIn;
         int32_t i       = 0;
         for (i = 0; i < (int32_t) bufParamsIn.dim_x; i++) {
            pInFill[i] = (-0.2) * i;
         }
         pInFill[7]            = 2.0000152587890f;
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.dataSize  = bufParamsIn.dim_x;
         status_nat            = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         if (status_nat == DSPLIB_SUCCESS) {
            status_nat = DSPLIB_fltoq15_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         }
         status_opt = DSPLIB_fltoq15_exec_checkParams(handle, pIn, pOut);
         if (status_opt == DSPLIB_SUCCESS) {
            status_opt = DSPLIB_fltoq15_exec(handle, pIn, pOut);
         }
         currentTestFail = ((status_nat != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS));
      } break;
      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_fltoq15_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_fltoq15_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_fltoq15_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for fltoq15 test projects */
#if !defined(__ONESHOTTEST)
int main()
{
   int fail = 1;

   uint32_t profile[256 * 3];

   DSPLIB_TEST_init();

   fail = test_main(&profile[0]);

#if !defined(NO_PRINTF)
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");

   int i;
   for (i = 0; i < test_cases; i++) {
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = "
             "%8d\n",
             i, profile[3 * i], profile[3 * i + 1], profile[3 * i + 2]);
   }
#endif

   fail = coverage_test_main();
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");
   return fail;
}
#endif
