// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/DSPLIB_test.h"
#include "DSPLIB_absoluteValue_idat.h"
#include <dsplib.h>

#ifdef WIN32
#if defined(__C7504__) || defined(__C7524__)
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

int DSPLIB_absoluteValue_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;
   uint32_t             testNum;
   uint64_t             archCycles = 0;
   uint64_t             estCycles  = 0;

   DSPLIB_absoluteValue_testParams_t *prm;
   DSPLIB_absoluteValue_testParams_t  currPrm;
   DSPLIB_absoluteValue_getTestParams(&prm, &test_cases);

   DSPLIB_absoluteValueInitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_absoluteValue_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   TI_profile_init("DSPLIB_absoluteValue");

   FILE *fpOutputCSV = fopen("DSPLIB_absoluteValue.csv", "w+");
   fprintf(fpOutputCSV, "Test ID, Bit Width, Width, Height, Stride, estCycles, Loki cycles, Pass/Fail\n");

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
         int32_t       dataWidth;
         uint32_t      i;
         archCycles = 0;
         estCycles  = 0;

         if (currPrm.dataType == DSPLIB_FLOAT32) {
            dataWidth = sizeof(DSPLIB_F32);
         }
         else if (currPrm.dataType == DSPLIB_FLOAT64) {
            dataWidth = sizeof(DSPLIB_D64);
         }
         else {
            dataWidth = DSPLIB_sizeof(currPrm.dataType);
         }
         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;
         currentTestFail        = 0;

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsIn.dim_x     = currPrm.width;
         bufParamsIn.dim_y     = currPrm.height;
         bufParamsIn.stride_y  = currPrm.strideIn;

         bufParamsOut.data_type = currPrm.dataType;
         bufParamsOut.dim_x     = currPrm.width;
         bufParamsOut.dim_y     = currPrm.height;
         bufParamsOut.stride_y  = currPrm.strideOut;

         DSPLIB_DEBUGPRINTFN(0, "data_type: %d width: %d height: %d rowStride: %d\n", bufParamsIn.data_type,
                             kerInitArgs.width, kerInitArgs.height, bufParamsIn.stride_y);

         void *pIn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.dim_y * bufParamsIn.stride_y);

         void *pOut, *pOutCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_y * bufParamsOut.stride_y);
            pOutCn = (void *) malloc(bufParamsOut.dim_y * bufParamsOut.stride_y);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_y * bufParamsOut.stride_y);
#else
            pOut   = (void *) msmcBuffer;
#endif

#if defined(DSPLIB_TEST_CN_OUTPUT_L2_HEAP) && defined(__C7504__)
            pOutCn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_y * bufParamsOut.stride_y);
#else
            pOutCn = (void *) ddrBuffer;
#endif
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_y * bufParamsOut.stride_y);
#else
            pOut   = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }
         // bufParamsIn.stride_y
         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn && pOut && pOutCn) {
            /* Fill input data */
            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, currPrm.width, currPrm.height,
                                   bufParamsIn.stride_y, dataWidth, testPatternString);
            }
            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pIn, prm[tpi].staticIn, currPrm.width, currPrm.height,
                             bufParamsIn.stride_y, dataWidth, testPatternString);
            }
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init = DSPLIB_absoluteValue_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init           = DSPLIB_absoluteValue_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }

            status_opt = DSPLIB_absoluteValue_exec_checkParams(handle, pIn, pOut);

            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_absoluteValue_exec(handle, pIn, pOut);
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
               status_opt = DSPLIB_absoluteValue_exec(handle, pIn, pOut);
               DSPLIB_asm(" MARK 5");
               TI_profile_stop();
            }

            // get output to L1D
            int16_t outSum   = 0;
            int8_t *pOutTemp = (int8_t *) pOut; // treat output as bytes to be data type agnostic
            for (k = 0; k < bufParamsOut.dim_x * bufParamsOut.dim_y; k++) {
               outSum += *pOutTemp;
               pOutTemp++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum = outSum;

            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            DSPLIB_asm(" MARK 6");
            status_opt = DSPLIB_absoluteValue_exec(handle, pIn, pOut);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)

            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
            status_init = DSPLIB_absoluteValue_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 8");
               status_init = DSPLIB_absoluteValue_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 9");
               TI_profile_stop();
            }

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            status_nat = DSPLIB_absoluteValue_exec(handle, pIn, pOutCn);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               status_nat_vs_opt =
                   TI_compare_mem_2D_float((void *) pOut, (void *) pOutCn, 0.001, (double) powf(2, -10),
                                           bufParamsOut.dim_x, bufParamsOut.dim_y, bufParamsOut.stride_y, dataWidth);
            }
            else {
               status_nat_vs_opt = TI_compare_mem_2D((void *) pOut, (void *) pOutCn, 0, 0, bufParamsOut.dim_x,
                                                     bufParamsOut.dim_y, bufParamsOut.stride_y, dataWidth);
            }

            comparisonDone = 1;

            if (currPrm.staticOut != NULL) {
               if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                  status_ref_vs_opt = TI_compare_mem_2DDualStride_float(
                      (void *) currPrm.staticOut, (void *) pOutCn, 0.001, (double) powf(2, -10), bufParamsOut.dim_x,
                      bufParamsOut.dim_y, bufParamsOut.dim_x * dataWidth, bufParamsOut.stride_y, dataWidth);
               }
               else {
                  status_ref_vs_opt = TI_compare_mem_2DDualStride(
                      (void *) currPrm.staticOut, (void *) pOut, 2, 65535, bufParamsOut.dim_x, bufParamsOut.dim_y,
                      bufParamsOut.dim_x * dataWidth, bufParamsOut.stride_y, dataWidth);
               }

               comparisonDone = 1;
            }
            else {
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

            sprintf(desc, "%s generated input | width = %d, height = %d, rowStride = %d, Data Type %d",
                    testPatternString, currPrm.width, currPrm.height, bufParamsIn.stride_y, currPrm.dataType);

            uint64_t num_se_fetches =
                ((currPrm.width * currPrm.height * dataWidth) + (__C7X_VEC_SIZE_BYTES__ - 1)) / __C7X_VEC_SIZE_BYTES__;

            DSPLIB_absoluteValue_perfEst(handle, &archCycles, &estCycles, currPrm.dataType);
            fprintf(fpOutputCSV, "%d, %d, %d, %d, %d, %llu, %d, %d\n", testNum, DSPLIB_sizeof(currPrm.dataType) * 8,
                    currPrm.width, currPrm.height, bufParamsIn.stride_y, estCycles, pProfile[3 * tpi + 2],
                    !currentTestFail);
            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum++, (currPrm.width * currPrm.height), archCycles, estCycles, currentTestFail,
                                desc);
         }
         else {
            sprintf(desc, "width=%d, height=%d", currPrm.width, currPrm.height);

            DSPLIB_DEBUGPRINTFN(0, "%s\n", "Skipping Test");

            TI_profile_skip_test(desc);
            TI_profile_clear_run_stats();
         }

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pOut);
#endif

#if defined(DSPLIB_TEST_CN_OUTPUT_L2_HEAP) && defined(__C7504__)
            TI_align_free(pOutCn);
#endif
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pOut);
#endif
         }
         TI_align_free(pIn);
      }
   }

   free(handle);
   fclose(fpOutputCSV);
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return fail: %d\n", fail);

   return fail;
}

int coverage_test_main()
{
   int32_t                      testNum         = 1000;
   int32_t                      currentTestFail = 0;
   DSPLIB_STATUS                status_nat;
   DSPLIB_STATUS                status_opt;
   DSPLIB_bufParams2D_t         bufParamsIn, bufParamsOut;
   DSPLIB_absoluteValueInitArgs kerInitArgs;
   int32_t                      handleSize = DSPLIB_absoluteValue_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle          handle     = malloc(handleSize);
   int                          fail       = 0;
   void                        *pIn        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                        *pOut       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);

   bufParamsIn.dim_x     = 16;
   bufParamsIn.dim_y     = 2;
   bufParamsIn.stride_y  = 32;
   bufParamsOut.dim_x    = 16;
   bufParamsOut.dim_y    = 2;
   bufParamsOut.stride_y = 32;

   while (testNum <= 1005) {
      switch (testNum) {
      case 1000:
         bufParamsIn.data_type  = DSPLIB_INT32;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_absoluteValue_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_absoluteValue_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail       = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         bufParamsIn.data_type  = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_absoluteValue_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_absoluteValue_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsIn.data_type  = DSPLIB_INT32;
         bufParamsOut.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_absoluteValue_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_absoluteValue_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         status_nat      = DSPLIB_absoluteValue_exec_checkParams(handle, NULL, pOut);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1004:
         status_nat      = DSPLIB_absoluteValue_exec_checkParams(handle, pIn, NULL);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1005:
         bufParamsIn.data_type  = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_absoluteValue_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_absoluteValue_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
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
   int32_t handleSize_LDRA = DSPLIB_absoluteValue_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_absoluteValue");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_absoluteValue_d(&pProfile[0], 0);
   }
}

#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
int main()
{
   int test_fail = 1, cov_fail = 1;

   uint32_t profile[256 * 3];

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
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = "
             "%8d\n",
             i, profile[3 * i], profile[3 * i + 1], profile[3 * i + 2]);
   }
#endif

   return test_fail && cov_fail;
}
#endif
