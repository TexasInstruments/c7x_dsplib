// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_minerror_idat.h"

#define MAX_NUM_TESTS (1000)
#ifdef WIN32
#if defined(__C7504__) || defined(__C7524__)
int8_t l2auxBuffer[DSPLIB_L2_BUFFER_SIZE];
int8_t ddrBuffer[2048 * 1024];
#else
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];
#endif
#else
#if defined(__C7504__) || defined(__C7524__)
__attribute__((section(".l2sramaux"), aligned(64))) int8_t l2auxBuffer[DSPLIB_L2_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t   ddrBuffer[2048 * 1024];
#else
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];
#endif
#endif // WIN32

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int DSPLIB_minerror_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsIn;
   DSPLIB_bufParams1D_t bufParamsErrCoefs, bufParamsOutIndex, bufParamsOutVal;

   uint64_t hwCycles = 0;

   uint32_t testNum;

   minerror_testParams_t *prm;
   minerror_testParams_t  currPrm;
   minerror_getTestParams(&prm, &test_cases);

   DSPLIB_minerror_InitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_minerror_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   TI_profile_init("DSPLIB_minerror");
#if (!defined(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_minerror.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Bit Width, Data size, HW "
                        "cycles, Pass/Fail, Per vector cycle\n");

#endif //    #if (!defined(EVM_TEST))

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         // Natural vs optimized
         int32_t status_nat_vs_opt_index = TI_TEST_KERNEL_FAIL;
         int32_t status_nat_vs_opt_val   = TI_TEST_KERNEL_FAIL;
         int32_t status_nat_vs_opt       = TI_TEST_KERNEL_FAIL;

         // Reference vs optimized
         int32_t status_ref_vs_opt_index = TI_TEST_KERNEL_FAIL;
         int32_t status_ref_vs_opt_val   = TI_TEST_KERNEL_FAIL;
         int32_t status_ref_vs_opt       = TI_TEST_KERNEL_FAIL;

         DSPLIB_STATUS status_init = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt  = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat  = DSPLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not
          * */
         int32_t comparisonDone = 0;

         currentTestFail = 0;

         kerInitArgs.vecInSize    = currPrm.vecInSize;
         kerInitArgs.errCoefsSize = currPrm.errCoefsSize;
         kerInitArgs.strideIn     = currPrm.strideIn;

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsIn.dim_x     = currPrm.errCoefsSize;
         bufParamsIn.dim_y     = currPrm.vecInSize;
         bufParamsIn.stride_y  = currPrm.strideIn;

         bufParamsErrCoefs.data_type = currPrm.dataType;
         bufParamsErrCoefs.dim_x     = currPrm.errCoefsSize;

         bufParamsOutIndex.dim_x = 1;
         bufParamsOutVal.dim_x   = 1;

         int32_t dataWidth;

         // For output Val, datawidth is set based on promoted datatypes
         // for 8, 16 and 32 bit integers
         if (currPrm.dataType == DSPLIB_FLOAT32) {
            dataWidth                 = sizeof(DSPLIB_F32);
            bufParamsOutVal.data_type = DSPLIB_FLOAT32;
         }
         else if (currPrm.dataType == DSPLIB_FLOAT64) {
            dataWidth                 = sizeof(DSPLIB_D64);
            bufParamsOutVal.data_type = DSPLIB_FLOAT64;
         }
         else if (currPrm.dataType == DSPLIB_INT8) {
            dataWidth                 = sizeof(int16_t);
            bufParamsOutVal.data_type = DSPLIB_INT8;
         }
         else if (currPrm.dataType == DSPLIB_UINT8) {
            dataWidth                 = sizeof(uint16_t);
            bufParamsOutVal.data_type = DSPLIB_UINT8;
         }
         else if (currPrm.dataType == DSPLIB_INT16) {
            dataWidth                 = sizeof(int32_t);
            bufParamsOutVal.data_type = DSPLIB_INT16;
         }
         else if (currPrm.dataType == DSPLIB_UINT16) {
            dataWidth                 = sizeof(uint32_t);
            bufParamsOutVal.data_type = DSPLIB_UINT16;
         }
         else if (currPrm.dataType == DSPLIB_INT32) {
            dataWidth                 = sizeof(int64_t);
            bufParamsOutVal.data_type = DSPLIB_INT32;
         }
         else if (currPrm.dataType == DSPLIB_UINT32) {
            dataWidth                 = sizeof(uint64_t);
            bufParamsOutVal.data_type = DSPLIB_UINT32;
         }
         else {
            dataWidth                 = DSPLIB_sizeof(currPrm.dataType);
            bufParamsOutVal.data_type = currPrm.dataType;
         }

         void *pIn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.stride_y * bufParamsIn.dim_y);
         void *pErrCoefs =
             (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, currPrm.errCoefsSize * DSPLIB_sizeof(currPrm.dataType));

         int  *pMaxIndex, *pMaxIndexCn;
         void *pMaxVal, *pMaxValCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pMaxIndex   = (int *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOutIndex.dim_x * sizeof(int));
            pMaxIndexCn = (int *) malloc(sizeof(int) * bufParamsOutVal.dim_x);
            pMaxVal     = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOutVal.dim_x * dataWidth);
            pMaxValCn   = (void *) malloc(bufParamsOutVal.dim_x * dataWidth);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            pMaxIndex = (int *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOutIndex.dim_x * sizeof(int));
            pMaxVal   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOutVal.dim_x * dataWidth);
#else
            pMaxIndex = (int *) msmcBuffer;
            pMaxVal   = (void *) (msmcBuffer + (sizeof(int) * bufParamsOutIndex.dim_x));
#endif
            pMaxIndexCn = (int *) ddrBuffer;
            pMaxValCn   = (void *) (ddrBuffer + (sizeof(int) * bufParamsOutIndex.dim_x));
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            pMaxIndex = (int *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOutIndex.dim_x * sizeof(int));
            pMaxVal   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOutVal.dim_x * dataWidth);
#else
            pMaxIndex = (int *) msmcBuffer;
            pMaxVal   = (void *) (msmcBuffer + (sizeof(int) * bufParamsOutIndex.dim_x));
#endif
            pMaxIndexCn = (int *) ddrBuffer;
            pMaxValCn   = (void *) (ddrBuffer + (sizeof(int) * bufParamsOutIndex.dim_x));
         }

         DSPLIB_DEBUGPRINTFN(0, "data_type %d vecInSize %d strideIn %d \n", bufParamsIn.data_type,
                             kerInitArgs.vecInSize, kerInitArgs.strideIn);
         DSPLIB_DEBUGPRINTFN(0, "pIn  %p pErrCoefs %p\n", pIn, pErrCoefs);

         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn && pErrCoefs && pMaxIndex && pMaxIndexCn) {

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x,
                                   bufParamsIn.dim_y, bufParamsIn.stride_y, dataWidth, testPatternString);

               TI_fillBuffer_float(prm[tpi].testPattern, 0, pErrCoefs, prm[tpi].staticIn, bufParamsErrCoefs.dim_x, 1, 0,
                                   dataWidth, testPatternString);
            }
            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pIn, prm[tpi].staticIn, bufParamsIn.dim_x,
                             bufParamsIn.dim_y, bufParamsIn.stride_y, DSPLIB_sizeof(currPrm.dataType),
                             testPatternString);

               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pErrCoefs, prm[tpi].staticIn, bufParamsErrCoefs.dim_x,
                             1, 0, DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }

            status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                           &bufParamsOutVal, &kerInitArgs);

            DSPLIB_DEBUGPRINTFN(0, "status_init %d\n", status_init);

            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                  &bufParamsOutVal, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }

            DSPLIB_DEBUGPRINTFN(0, "status_init %d\n", status_init);

            status_opt = DSPLIB_minerror_exec_checkParams(handle, pIn, pErrCoefs, pMaxIndex, pMaxVal);

            DSPLIB_DEBUGPRINTFN(0, "status_opt %d\n", status_opt);

            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_minerror_exec(handle, pIn, pErrCoefs, pMaxIndex, pMaxVal);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
            }

#if !defined(__C7X_HOSTEM__)
            /* The following for loop is to call kernel repeatedly so as to
             * train the branch predictor
             */
            uint32_t k = 0;
            for (k = 0; k < 4; k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               DSPLIB_asm(" MARK 4");
               status_opt = DSPLIB_minerror_exec(handle, pIn, pErrCoefs, pMaxIndex, pMaxVal);
               DSPLIB_asm(" MARK 5");
               TI_profile_stop();
            }

            // get output to L1D
            int16_t outSum   = 0;
            int8_t *pOutTemp = (int8_t *) pMaxIndex; // treat output as bytes to be data type agnostic
            for (k = 0; k < 1; k++) {
               outSum += *pOutTemp;
               pOutTemp++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum = outSum;

            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            DSPLIB_asm(" MARK 6");
            status_opt = DSPLIB_minerror_exec(handle, pIn, pErrCoefs, pMaxIndex, pMaxVal);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)

            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
            DSPLIB_DEBUGPRINTFN(0, "status_opt %d\n", status_opt);

            // initialize the kernel to use the natural C variant
            DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex, &bufParamsOutVal,
                                 &kerInitArgs);

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            status_nat = DSPLIB_minerror_exec(handle, pIn, pErrCoefs, pMaxIndexCn, pMaxValCn);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();

            DSPLIB_DEBUGPRINTFN(0, "status_nat %d\n", status_nat);

            DSPLIB_DEBUGPRINTFN(0, "maxIndex %d maxIndexCn %d \n", *((int32_t *) pMaxIndex),
                                *((int32_t *) pMaxIndexCn));

            if (currPrm.dataType == DSPLIB_FLOAT32) {
               DSPLIB_DEBUGPRINTFN(0, "float pMaxVal %f pMaxValCn %f \n", *((float *) pMaxVal), *((float *) pMaxValCn));
            }
            else if (currPrm.dataType == DSPLIB_FLOAT64) {
               DSPLIB_DEBUGPRINTFN(0, "float64 pMaxVal %f pMaxValCn %f \n", *((float *) pMaxVal),
                                   *((float *) pMaxValCn));
            }
            else if (currPrm.dataType == DSPLIB_INT64) {
               DSPLIB_DEBUGPRINTFN(0, "int64 pMaxVal %jd pMaxValCn %jd \n", *((int64_t *) pMaxVal),
                                   *((int64_t *) pMaxValCn));
            }
            else if (currPrm.dataType == DSPLIB_INT32) {
               DSPLIB_DEBUGPRINTFN(0, "int pMaxVal %jd pMaxValCn %jd \n", *((int64_t *) pMaxVal),
                                   *((int64_t *) pMaxValCn));
            }
            else if (currPrm.dataType == DSPLIB_INT16) {
               DSPLIB_DEBUGPRINTFN(0, "int pMaxVal %d pMaxValCn %d \n", *((int32_t *) pMaxVal),
                                   *((int32_t *) pMaxValCn));
            }
            else if (currPrm.dataType == DSPLIB_INT8) {
               DSPLIB_DEBUGPRINTFN(0, "int pMaxVal %d pMaxValCn %d \n", *((int16_t *) pMaxVal),
                                   *((int16_t *) pMaxValCn));
            }
            else if (currPrm.dataType == DSPLIB_UINT64) {
               DSPLIB_DEBUGPRINTFN(0, "uint64 pMaxVal %ju pMaxValCn %ju \n", *((uint64_t *) pMaxVal),
                                   *((uint64_t *) pMaxValCn));
            }
            else if (currPrm.dataType == DSPLIB_UINT32) {
               DSPLIB_DEBUGPRINTFN(0, "int pMaxVal %ju pMaxValCn %ju \n", *((uint64_t *) pMaxVal),
                                   *((uint64_t *) pMaxValCn));
            }
            else if (currPrm.dataType == DSPLIB_UINT16) {
               DSPLIB_DEBUGPRINTFN(0, "int pMaxVal %u pMaxValCn %u \n", *((uint32_t *) pMaxVal),
                                   *((uint32_t *) pMaxValCn));
            }
            else if (currPrm.dataType == DSPLIB_UINT8) {
               DSPLIB_DEBUGPRINTFN(0, "int pMaxVal %u pMaxValCn %u \n", *((uint16_t *) pMaxVal),
                                   *((uint16_t *) pMaxValCn));
            }
            else {
               // do nothing
            }

            status_nat_vs_opt_index = TI_compare_mem((void *) pMaxIndex, (void *) pMaxIndexCn, sizeof(int32_t));

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {

               status_nat_vs_opt_val =
                   TI_compare_mem_2D_float((void *) pMaxVal, (void *) pMaxValCn, 0, (double) powf(2, -10),
                                           bufParamsOutVal.dim_x, 1, 0, dataWidth);
            }
            else {
               status_nat_vs_opt_val = TI_compare_mem((void *) pMaxVal, (void *) pMaxValCn, dataWidth);
            }

            DSPLIB_DEBUGPRINTFN(0, "status_nat_vs_opt_val %d status_nat_vs_opt_index %d\n", status_nat_vs_opt_val,
                                status_nat_vs_opt_index);

            if (status_nat_vs_opt_index == TI_TEST_KERNEL_FAIL || status_nat_vs_opt_val == TI_TEST_KERNEL_FAIL)
               status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
            else
               status_nat_vs_opt = TI_TEST_KERNEL_PASS;

            comparisonDone = 1;
            DSPLIB_DEBUGPRINTFN(0, "comparisonDone %d status_nat_vs_opt %d\n", comparisonDone, status_nat_vs_opt);

            if (currPrm.staticOut != NULL) {
               status_ref_vs_opt_index =
                   TI_compare_mem((void *) currPrm.staticOut, (void *) pMaxIndex, sizeof(int32_t));

               if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {

                  status_ref_vs_opt_val =
                      TI_compare_mem_2D_float((void *) currPrm.staticOut, (void *) pMaxVal, 0, (double) powf(2, -10),
                                              bufParamsOutVal.dim_x, 1, 0, dataWidth);
               }
               else {
                  status_ref_vs_opt_val = TI_compare_mem((void *) currPrm.staticOut, (void *) pMaxVal, dataWidth);
               }

               DSPLIB_DEBUGPRINTFN(0, "status_nat_vs_opt_val %d status_nat_vs_opt_index %d\n", status_nat_vs_opt_val,
                                   status_nat_vs_opt_index);

               if (status_ref_vs_opt_index == TI_TEST_KERNEL_FAIL || status_ref_vs_opt_val == TI_TEST_KERNEL_FAIL)
                  status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
               else
                  status_ref_vs_opt = TI_TEST_KERNEL_PASS;

               comparisonDone = 1;
               DSPLIB_DEBUGPRINTFN(0, "comparisonDone %d status_nat_vs_opt %d\n", comparisonDone, status_nat_vs_opt);
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

            DSPLIB_DEBUGPRINTFN(0, "status_nat_vs_opt %d status_ref_vs_opt %d currentTestFail %d\n", status_nat_vs_opt,
                                status_ref_vs_opt, currentTestFail);
            DSPLIB_DEBUGPRINTFN(0, "status_init %d status_opt %d status_nat %d\n", status_init, status_opt, status_nat);

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) || (status_nat != DSPLIB_SUCCESS) ||
                 (comparisonDone == 0) || (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

            DSPLIB_DEBUGPRINTFN(0, "fail %d\n", fail);

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);
            DSPLIB_DEBUGPRINTFN(0, "%s", "Profiling completed\n");

            sprintf(desc, "%s generated input | Data Type = %d", testPatternString, currPrm.dataType);

            DSPLIB_DEBUGPRINTFN(0, "%s", "Writing CSV\n");
#if (!defined(EVM_TEST))

            float    per_vector_cycles;
            uint64_t num_se_fetches = ((bufParamsIn.dim_x * bufParamsIn.dim_y * DSPLIB_sizeof(currPrm.dataType)) +
                                       (__C7X_VEC_SIZE_BYTES__ - 1)) /
                                      __C7X_VEC_SIZE_BYTES__;
            hwCycles          = pProfile[3 * tpi];
            per_vector_cycles = (float_t) ((double_t) hwCycles / (double_t) num_se_fetches);
            // write to CSV, must happen prior to write to screen because TI_profile_formula clears values in counters
            fprintf(fpOutputCSV, "minerror, %d, %d, %d, %d, %d, %d,%3.2f\n", testNum, currPrm.testPattern,
                    DSPLIB_sizeof(bufParamsIn.data_type) * 8, currPrm.vecInSize, (int) hwCycles, !currentTestFail,
                    per_vector_cycles);

#endif // #if (!defined(EVM_TEST))
            DSPLIB_DEBUGPRINTFN(0, "%s", "Writing CSV complted\n");
            TI_profile_add_test(testNum, currPrm.vecInSize, hwCycles, 0, currentTestFail, desc);
         }
         else {
            sprintf(desc, "Vector size=%d", currPrm.vecInSize);
            TI_profile_skip_test(desc);

            // clear the counters between runs; normally handled by TI_profile_add_test
            TI_profile_clear_run_stats();

         } // end of memory allocation successful?
         DSPLIB_DEBUGPRINTFN(0, "%s", "Freeing buffers\n");
         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pMaxIndexCn);
            free(pMaxValCn);

            TI_align_free(pMaxIndex);
            TI_align_free(pMaxVal);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pMaxIndex);
            TI_align_free(pMaxVal);
#endif
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pMaxIndex);
            TI_align_free(pMaxVal);
#endif
         }
         TI_align_free(pIn);
         TI_align_free(pErrCoefs);
         DSPLIB_DEBUGPRINTFN(0, "%s", "Freeing buffers completed\n");
      } // end repetitions
   }    // end idat test cases

   free(handle);

#if (!defined(EVM_TEST))
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif // #if (!defined(EVM_TEST))

   return fail;
}

int test_main(uint32_t *pProfile)
{
   printf(" Test Main Start \n");
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_minerror");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_minerror_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{

   minerror_testParams_t    currPrm;
   int32_t                  testNum         = 1000;
   int32_t                  currentTestFail = 0;
   DSPLIB_STATUS            status_nat;
   DSPLIB_STATUS            status_opt;
   DSPLIB_bufParams2D_t     bufParamsIn;
   DSPLIB_bufParams1D_t     bufParamsErrCoefs, bufParamsOutIndex, bufParamsOutVal;
   DSPLIB_minerror_InitArgs kerInitArgs;
   int32_t                  handleSize = DSPLIB_minerror_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle      handle     = malloc(handleSize);
   int                      fail       = 0;
   void                    *pIn        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                    *pErrCoefs  = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   int                     *pMaxIndex  = (int *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                    *pMaxVal    = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);

   while (testNum <= 1011) {

      switch (testNum) {
      case 1000:
         bufParamsIn.data_type     = DSPLIB_INT32;
         bufParamsOutVal.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_minerror_init_checkParams(NULL, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                       &bufParamsOutVal, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_minerror_init_checkParams(NULL, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                            &bufParamsOutVal, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         bufParamsIn.data_type     = DSPLIB_INT128;
         bufParamsOutVal.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                       &bufParamsOutVal, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                       &bufParamsOutVal, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsIn.data_type     = DSPLIB_INT32;
         bufParamsOutVal.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                       &bufParamsOutVal, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                       &bufParamsOutVal, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         status_nat      = DSPLIB_minerror_exec_checkParams(handle, NULL, pErrCoefs, pMaxIndex, pMaxVal);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1004:
         status_nat      = DSPLIB_minerror_exec_checkParams(handle, pIn, NULL, NULL, NULL);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1005:
         bufParamsIn.data_type     = DSPLIB_INT128;
         bufParamsOutVal.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_nat                = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                          &bufParamsOutVal, &kerInitArgs);
         kerInitArgs.funcStyle     = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt                = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                          &bufParamsOutVal, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1006: {
         currPrm.vecInSize    = 4;
         currPrm.errCoefsSize = 2;
         currPrm.dataType     = DSPLIB_INT16;
         currPrm.strideIn     = DSPLIB_CALC_STRIDE(9 * sizeof(int8_t), DSPLIB_ALIGN_SHIFT_64BYTES); // strideIn

         kerInitArgs.vecInSize    = currPrm.vecInSize;
         kerInitArgs.errCoefsSize = currPrm.errCoefsSize;
         kerInitArgs.strideIn     = currPrm.strideIn;

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsIn.dim_x     = currPrm.errCoefsSize;
         bufParamsIn.dim_y     = currPrm.vecInSize;
         bufParamsIn.stride_y  = currPrm.strideIn;

         bufParamsErrCoefs.data_type = currPrm.dataType;
         bufParamsErrCoefs.dim_x     = currPrm.errCoefsSize;

         bufParamsOutIndex.dim_x = 1;
         bufParamsOutVal.dim_x   = 1;

         int32_t dataWidth         = DSPLIB_sizeof(currPrm.dataType);
         bufParamsOutVal.data_type = currPrm.dataType;
         void *pInTemp             = (void *) malloc(bufParamsIn.stride_y * bufParamsIn.dim_y);
         void *pErrCoefsTemp       = (void *) malloc(currPrm.errCoefsSize * DSPLIB_sizeof(currPrm.dataType));
         int  *pMaxIndexTemp       = (int *) malloc(sizeof(int) * bufParamsOutVal.dim_x);
         void *pMaxValTemp         = (void *) malloc(bufParamsOutVal.dim_x * dataWidth);

         int  *pMaxIndexCn = (int *) (ddrBuffer);
         void *pMaxValCn   = (void *) (ddrBuffer + (sizeof(int) * bufParamsOutIndex.dim_x));
         TI_fillBuffer_float(currPrm.testPattern, 0, pInTemp, NULL, bufParamsIn.dim_x, bufParamsIn.dim_y,
                             bufParamsIn.stride_y, dataWidth, testPatternString);
         TI_fillBuffer_float(currPrm.testPattern, 0, pErrCoefsTemp, NULL, bufParamsErrCoefs.dim_x, 1, 0, dataWidth,
                             testPatternString);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

         int32_t status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs,
                                                                &bufParamsOutIndex, &bufParamsOutVal, &kerInitArgs);
         status_init         = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                    &bufParamsOutVal, &kerInitArgs);
         int32_t status_opt =
             DSPLIB_minerror_exec_checkParams(handle, pInTemp, pErrCoefsTemp, pMaxIndexTemp, pMaxValTemp);
         status_opt = DSPLIB_minerror_exec(handle, pInTemp, pErrCoefsTemp, pMaxIndexTemp, pMaxValTemp);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                        &bufParamsOutVal, &kerInitArgs);
         status_init = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                            &bufParamsOutVal, &kerInitArgs);
         status_opt  = DSPLIB_minerror_exec_checkParams(handle, pInTemp, pErrCoefsTemp, pMaxIndexCn, pMaxValCn);
         status_opt  = DSPLIB_minerror_exec(handle, pInTemp, pErrCoefsTemp, pMaxIndexCn, pMaxValCn);

         free(pInTemp);
         free(pErrCoefsTemp);
         free(pMaxIndexTemp);
         free(pMaxValTemp);

         currentTestFail = ((status_opt != DSPLIB_SUCCESS) || (status_init != DSPLIB_SUCCESS));
      } break;
      case 1007: {
         currPrm.vecInSize    = 4;
         currPrm.errCoefsSize = 3;
         currPrm.dataType     = DSPLIB_INT32;
         currPrm.strideIn     = DSPLIB_CALC_STRIDE(4 * sizeof(int8_t), DSPLIB_ALIGN_SHIFT_64BYTES); // strideIn

         kerInitArgs.vecInSize    = currPrm.vecInSize;
         kerInitArgs.errCoefsSize = currPrm.errCoefsSize;
         kerInitArgs.strideIn     = currPrm.strideIn;

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsIn.dim_x     = currPrm.errCoefsSize;
         bufParamsIn.dim_y     = currPrm.vecInSize;
         bufParamsIn.stride_y  = currPrm.strideIn;

         bufParamsErrCoefs.data_type = currPrm.dataType;
         bufParamsErrCoefs.dim_x     = currPrm.errCoefsSize;

         bufParamsOutIndex.dim_x = 1;
         bufParamsOutVal.dim_x   = 1;

         int32_t dataWidth         = DSPLIB_sizeof(currPrm.dataType);
         bufParamsOutVal.data_type = currPrm.dataType;
         void *pInTemp             = (void *) malloc(bufParamsIn.stride_y * bufParamsIn.dim_y);
         void *pErrCoefsTemp       = (void *) malloc(currPrm.errCoefsSize * DSPLIB_sizeof(currPrm.dataType));
         int  *pMaxIndexTemp       = (int *) malloc(sizeof(int) * bufParamsOutVal.dim_x);
         void *pMaxValTemp         = (void *) malloc(bufParamsOutVal.dim_x * dataWidth);

         int  *pMaxIndexCn = (int *) (ddrBuffer);
         void *pMaxValCn   = (void *) (ddrBuffer + (sizeof(int) * bufParamsOutIndex.dim_x));
         TI_fillBuffer_float(currPrm.testPattern, 0, pInTemp, NULL, bufParamsIn.dim_x, bufParamsIn.dim_y,
                             bufParamsIn.stride_y, dataWidth, testPatternString);
         TI_fillBuffer_float(currPrm.testPattern, 0, pErrCoefsTemp, NULL, bufParamsErrCoefs.dim_x, 1, 0, dataWidth,
                             testPatternString);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

         int32_t status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs,
                                                                &bufParamsOutIndex, &bufParamsOutVal, &kerInitArgs);
         status_init         = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                    &bufParamsOutVal, &kerInitArgs);
         int32_t status_opt =
             DSPLIB_minerror_exec_checkParams(handle, pInTemp, pErrCoefsTemp, pMaxIndexTemp, pMaxValTemp);
         status_opt = DSPLIB_minerror_exec(handle, pInTemp, pErrCoefsTemp, pMaxIndexTemp, pMaxValTemp);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                        &bufParamsOutVal, &kerInitArgs);
         status_init = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                            &bufParamsOutVal, &kerInitArgs);
         status_opt  = DSPLIB_minerror_exec_checkParams(handle, pInTemp, pErrCoefsTemp, pMaxIndexCn, pMaxValCn);
         status_opt  = DSPLIB_minerror_exec(handle, pInTemp, pErrCoefsTemp, pMaxIndexCn, pMaxValCn);

         free(pInTemp);
         free(pErrCoefsTemp);
         free(pMaxIndexTemp);
         free(pMaxValTemp);

         currentTestFail = ((status_opt != DSPLIB_SUCCESS) || (status_init != DSPLIB_SUCCESS));
      } break;
      case 1008: {
         currPrm.vecInSize    = 3;
         currPrm.errCoefsSize = 1;
         currPrm.dataType     = DSPLIB_INT64;
         currPrm.strideIn     = DSPLIB_CALC_STRIDE(1 * sizeof(int8_t), DSPLIB_ALIGN_SHIFT_64BYTES); // strideIn

         kerInitArgs.vecInSize    = currPrm.vecInSize;
         kerInitArgs.errCoefsSize = currPrm.errCoefsSize;
         kerInitArgs.strideIn     = currPrm.strideIn;

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsIn.dim_x     = currPrm.errCoefsSize;
         bufParamsIn.dim_y     = currPrm.vecInSize;
         bufParamsIn.stride_y  = currPrm.strideIn;

         bufParamsErrCoefs.data_type = currPrm.dataType;
         bufParamsErrCoefs.dim_x     = currPrm.errCoefsSize;

         bufParamsOutIndex.dim_x = 1;
         bufParamsOutVal.dim_x   = 1;

         int32_t dataWidth         = DSPLIB_sizeof(currPrm.dataType);
         bufParamsOutVal.data_type = currPrm.dataType;
         void *pInTemp             = (void *) malloc(bufParamsIn.stride_y * bufParamsIn.dim_y);
         void *pErrCoefsTemp       = (void *) malloc(currPrm.errCoefsSize * DSPLIB_sizeof(currPrm.dataType));
         int  *pMaxIndexTemp       = (int *) malloc(sizeof(int) * bufParamsOutVal.dim_x);
         void *pMaxValTemp         = (void *) malloc(bufParamsOutVal.dim_x * dataWidth);

         int  *pMaxIndexCn = (int *) (ddrBuffer);
         void *pMaxValCn   = (void *) (ddrBuffer + (sizeof(int) * bufParamsOutIndex.dim_x));
         TI_fillBuffer_float(currPrm.testPattern, 0, pInTemp, NULL, bufParamsIn.dim_x, bufParamsIn.dim_y,
                             bufParamsIn.stride_y, dataWidth, testPatternString);
         TI_fillBuffer_float(currPrm.testPattern, 0, pErrCoefsTemp, NULL, bufParamsErrCoefs.dim_x, 1, 0, dataWidth,
                             testPatternString);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

         int32_t status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs,
                                                                &bufParamsOutIndex, &bufParamsOutVal, &kerInitArgs);
         status_init         = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                    &bufParamsOutVal, &kerInitArgs);
         int32_t status_opt =
             DSPLIB_minerror_exec_checkParams(handle, pInTemp, pErrCoefsTemp, pMaxIndexTemp, pMaxValTemp);
         status_opt = DSPLIB_minerror_exec(handle, pInTemp, pErrCoefsTemp, pMaxIndexTemp, pMaxValTemp);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                        &bufParamsOutVal, &kerInitArgs);
         status_init = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                            &bufParamsOutVal, &kerInitArgs);
         status_opt  = DSPLIB_minerror_exec_checkParams(handle, pInTemp, pErrCoefsTemp, pMaxIndexCn, pMaxValCn);
         status_opt  = DSPLIB_minerror_exec(handle, pInTemp, pErrCoefsTemp, pMaxIndexCn, pMaxValCn);

         free(pInTemp);
         free(pErrCoefsTemp);
         free(pMaxIndexTemp);
         free(pMaxValTemp);

         currentTestFail = ((status_opt != DSPLIB_SUCCESS) || (status_init != DSPLIB_SUCCESS));
      } break;
      case 1009: {
         bufParamsIn.data_type       = DSPLIB_INT32;
         bufParamsErrCoefs.data_type = DSPLIB_INT16;
         bufParamsOutVal.data_type   = DSPLIB_INT16;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                       &bufParamsOutVal, &kerInitArgs);

         bufParamsErrCoefs.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle       = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                       &bufParamsOutVal, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
      } break;
      case 1010: {
         currPrm.vecInSize    = 256;
         currPrm.errCoefsSize = 9;
         currPrm.dataType     = DSPLIB_INT32;
         currPrm.strideIn     = DSPLIB_CALC_STRIDE(9 * sizeof(int8_t), DSPLIB_ALIGN_SHIFT_64BYTES); // strideIn

         bufParamsIn.data_type     = DSPLIB_INT32;
         bufParamsOutVal.data_type = DSPLIB_INT32;

         kerInitArgs.vecInSize    = currPrm.vecInSize;
         kerInitArgs.errCoefsSize = currPrm.errCoefsSize;
         kerInitArgs.strideIn     = currPrm.strideIn;

         bufParamsErrCoefs.data_type = currPrm.dataType;
         bufParamsErrCoefs.dim_x     = currPrm.errCoefsSize;

         bufParamsOutIndex.dim_x = 1;
         bufParamsOutVal.dim_x   = 1;

         int32_t dataWidth     = 1;
         void   *pInTemp       = (void *) malloc(bufParamsIn.stride_y * bufParamsIn.dim_y);
         void   *pErrCoefsTemp = (void *) malloc(currPrm.errCoefsSize * DSPLIB_sizeof(currPrm.dataType));
         int    *pMaxIndexTemp = (int *) malloc(sizeof(int) * bufParamsOutVal.dim_x);
         void   *pMaxValTemp   = (void *) malloc(bufParamsOutVal.dim_x * dataWidth);

         void *pIn       = NULL;
         void *pErrCoefs = NULL;

         int  *pMaxIndex = NULL;
         void *pMaxVal   = NULL;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         int32_t status_init   = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs,
                                                                  &bufParamsOutIndex, &bufParamsOutVal, &kerInitArgs);
         status_init           = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                      &bufParamsOutVal, &kerInitArgs);
         int32_t status_nat = DSPLIB_minerror_exec_checkParams(handle, pInTemp, pErrCoefsTemp, pMaxIndexTemp, pMaxVal);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                        &bufParamsOutVal, &kerInitArgs);
         status_init = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                            &bufParamsOutVal, &kerInitArgs);
         int32_t status_opt = DSPLIB_minerror_exec_checkParams(handle, pInTemp, pErrCoefsTemp, pMaxIndex, pMaxValTemp);
         currentTestFail    = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));

         free(pInTemp);
         free(pErrCoefsTemp);
         free(pMaxIndexTemp);
         free(pMaxValTemp);

      } break;

      case 1011: {
         currPrm.vecInSize    = 256;
         currPrm.errCoefsSize = 9;
         currPrm.dataType     = DSPLIB_INT32;
         currPrm.strideIn     = DSPLIB_CALC_STRIDE(9 * sizeof(int8_t), DSPLIB_ALIGN_SHIFT_64BYTES); // strideIn

         bufParamsIn.data_type     = DSPLIB_INT32;
         bufParamsOutVal.data_type = DSPLIB_INT32;

         kerInitArgs.vecInSize    = currPrm.vecInSize;
         kerInitArgs.errCoefsSize = currPrm.errCoefsSize;
         kerInitArgs.strideIn     = currPrm.strideIn;

         bufParamsErrCoefs.data_type = currPrm.dataType;
         bufParamsErrCoefs.dim_x     = currPrm.errCoefsSize;

         bufParamsOutIndex.dim_x = 1;
         bufParamsOutVal.dim_x   = 1;

         int32_t dataWidth     = 1;
         void   *pInTemp       = (void *) malloc(bufParamsIn.stride_y * bufParamsIn.dim_y);
         void   *pErrCoefsTemp = (void *) malloc(currPrm.errCoefsSize * DSPLIB_sizeof(currPrm.dataType));
         int    *pMaxIndexTemp = (int *) malloc(sizeof(int) * bufParamsOutVal.dim_x);
         void   *pMaxValTemp   = (void *) malloc(bufParamsOutVal.dim_x * dataWidth);

         void *pIn       = NULL;
         void *pErrCoefs = NULL;

         int  *pMaxIndex = NULL;
         void *pMaxVal   = NULL;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         int32_t status_init   = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs,
                                                                  &bufParamsOutIndex, &bufParamsOutVal, &kerInitArgs);
         status_init           = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                      &bufParamsOutVal, &kerInitArgs);
         int32_t status_nat = DSPLIB_minerror_exec_checkParams(handle, pIn, pErrCoefsTemp, pMaxIndexTemp, pMaxValTemp);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                        &bufParamsOutVal, &kerInitArgs);
         status_init = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                            &bufParamsOutVal, &kerInitArgs);
         int32_t status_opt = DSPLIB_minerror_exec_checkParams(handle, pInTemp, pErrCoefs, pMaxIndexTemp, pMaxValTemp);
         currentTestFail    = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));

         free(pInTemp);
         free(pErrCoefsTemp);
         free(pMaxIndexTemp);
         free(pMaxValTemp);

      } break;

      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   TI_align_free(pIn);
   TI_align_free(pErrCoefs);
   TI_align_free(pMaxIndex);
   TI_align_free(pMaxVal);
   free(handle);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_minerror_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_minerror_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_minerror_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for inaddidual test projects */
#if !defined(__ONESHOTTEST)
int main()
{
   int fail = 1;

   uint32_t profile[256 * 3];

   printf(" Main Start \n");
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
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");
   return fail;
}
#endif
