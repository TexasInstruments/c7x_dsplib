// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_matMul_idat.h"
#include "DSPLIB_types.h"

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
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];

#endif
#endif // WIN32

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int DSPLIB_matMul_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t              tpi;
   int32_t              k;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsIn0, bufParamsIn1, bufParamsOut;

   uint32_t testNum;
   uint64_t archCycles, estCycles;
   uint32_t comparisonDone = 0;

   DSPLIB_matMul_testParams_t *prm;
   DSPLIB_matMul_testParams_t  currPrm;
   DSPLIB_matMul_getTestParams(&prm, &test_cases);

   DSPLIB_matMul_InitArgs kerInitArgs;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 0\n");
#endif

   int32_t             handleSize = DSPLIB_matMul_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 1\n");
#endif

   TI_profile_init("DSPLIB_matMul");
#if (!defined(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_matMul.csv", "w+");
   fprintf(fpOutputCSV, "Test ID, Bit Width, M, N, K, "
                        "Loki cycles, Pass/Fail\n");
#endif //   #if (!defined(EVM_TEST))

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         int32_t       status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t       status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
         DSPLIB_STATUS status_init       = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt        = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat        = DSPLIB_SUCCESS;

         currentTestFail = 0;

         bufParamsIn0.data_type = currPrm.dataType;
         bufParamsIn0.dim_x     = currPrm.K;
         bufParamsIn0.dim_y     = currPrm.M;
         bufParamsIn0.stride_y  = currPrm.strideIn0;

         if (bufParamsIn0.stride_y % 512 == 0) {
            bufParamsIn0.stride_y += 128;
         }

         bufParamsIn1.data_type = currPrm.dataType;
         bufParamsIn1.dim_x     = currPrm.N;
         bufParamsIn1.dim_y     = currPrm.K;
         bufParamsIn1.stride_y  = currPrm.strideIn1;

         if (bufParamsIn1.stride_y % 512 == 0) {
            bufParamsIn1.stride_y += 128;
         }

         bufParamsOut.data_type = currPrm.dataType;
         bufParamsOut.dim_x     = currPrm.N;
         bufParamsOut.dim_y     = currPrm.M;
         bufParamsOut.stride_y  = currPrm.strideOut;

         if (bufParamsOut.stride_y % 512 == 0) {
            bufParamsOut.stride_y += 128;
         }

         // Here, stride is in bytes
         uint32_t inp0SizeBytes = bufParamsIn0.dim_y * bufParamsIn0.stride_y;
         uint32_t inp1SizeBytes = bufParamsIn1.dim_y * bufParamsIn1.stride_y;
         uint32_t outSizeBytes  = bufParamsOut.dim_y * bufParamsOut.stride_y;

         void *pIn0 = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, inp0SizeBytes);
         void *pIn1 = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, inp1SizeBytes);

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
         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn0 && pIn1 && pOut && pOutCn) {

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn0, prm[tpi].staticIn0, bufParamsIn0.dim_x,
                                   bufParamsIn0.dim_y, bufParamsIn0.stride_y, DSPLIB_sizeof(currPrm.dataType),
                                   testPatternString);
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn1, prm[tpi].staticIn1, bufParamsIn1.dim_x,
                                   bufParamsIn1.dim_y, bufParamsIn1.stride_y, DSPLIB_sizeof(currPrm.dataType),
                                   testPatternString);
            }
            else {
               // TBD for other precisions
            }

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 0\n");
#endif

            status_init =
                DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 1 status_init %d\n", status_init);
#endif
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init = DSPLIB_matMul_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 2 status_init %d\n", status_init);
#endif
            status_opt = DSPLIB_matMul_exec_checkParams(handle, pIn0, pIn1, pOut);

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 3 status_opt %d\n", status_opt);
#endif
            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_matMul_exec(handle, pIn0, pIn1, pOut);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
            }

#if !defined(__C7X_HOSTEM__)
            /* The following for loop is to call kernel repeatedly so as to
             * train the branch predictor                                   */
            for (k = 0; k < 1; k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               DSPLIB_asm(" MARK 4");
               status_opt = DSPLIB_matMul_exec(handle, pIn0, pIn1, pOut);
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
            status_opt = DSPLIB_matMul_exec(handle, pIn0, pIn1, pOut);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();

#endif // #if !defined(__C7X_HOSTEM__)
            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 4 status_opt %d\n", status_opt);
#endif
            // initialize the kernel to use the natural C variant
            DSPLIB_matMul_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 5\n");
#endif
            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            status_nat = DSPLIB_matMul_exec(handle, pIn0, pIn1, pOutCn);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 6 status_nat %d\n", status_nat);
#endif
            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               status_nat_vs_opt = TI_compare_mem_2D_float((void *) pOut, (void *) pOutCn, 0.001, (double) powf(2, -10),
                                                           bufParamsOut.dim_x, bufParamsOut.dim_y,
                                                           bufParamsOut.stride_y, DSPLIB_sizeof(currPrm.dataType));
            }
            else {
               // TBD
               /* status_nat_vs_opt = */
               /*     TI_compare_mem_2D((void *) pOut, (void *) pOutCn, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y, */
               /*                       bufParamsOut.stride_y, 0, DSPLIB_sizeof(currPrm.dataType)); */
            }

            comparisonDone = 1;
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 7 comparisonDone %d status_nat_vs_opt %d\n", comparisonDone,
                   status_nat_vs_opt);
#endif

            if (currPrm.staticOut != NULL) {
               if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                  status_ref_vs_opt =
                      TI_compare_mem_2D_float((void *) currPrm.staticOut, (void *) pOut, 0.001, (double) powf(2, -10),
                                              bufParamsOut.dim_x, 1, 0, DSPLIB_sizeof(bufParamsOut.data_type));
               }
               else {
                  status_ref_vs_opt =
                      TI_compare_mem_2D((void *) currPrm.staticOut, (void *) pOut, 2, 65535, bufParamsOut.dim_x, 1,
                                        bufParamsOut.dim_x * DSPLIB_sizeof(bufParamsOut.data_type),
                                        DSPLIB_sizeof(bufParamsOut.data_type));
               }

               comparisonDone = 1;
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 8 status_nat_vs_opt %d status_ref_vs_opt %d currentTestFail "
                   "%d\n",
                   status_nat_vs_opt, status_ref_vs_opt, currentTestFail);
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 8 status_init %d status_opt %d status_nat %d\n", status_init,
                   status_opt, status_nat);
#endif
            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) || (status_nat != DSPLIB_SUCCESS) ||
                 (comparisonDone == 0) || (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_matMul_d CP 8 fail %d\n", fail);
#endif

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | M = %d, N = %d, K = %d", testPatternString, currPrm.M, currPrm.N,
                    currPrm.K);
#if (!defined(EVM_TEST))
            // write to CSV, must happen prior to write to screen because
            // TI_profile_formula_add clears values in counters
            fprintf(fpOutputCSV, "%d, %d, %d, %d, %d , %d, %d\n", currPrm.testID, DSPLIB_sizeof(currPrm.dataType) * 8,
                    currPrm.M, currPrm.N, currPrm.K, pProfile[3 * tpi], !currentTestFail);
#endif // #if (!defined(EVM_TEST))

            TI_profile_add_test(testNum++, currPrm.M, currPrm.N, currPrm.K, currentTestFail, desc);
         }
         else {
            sprintf(desc, "%s data does not fit in memory | M = %d, N = %d, K = %d", testPatternString, currPrm.M,
                    currPrm.N, currPrm.K);
            TI_profile_skip_test(desc);

            // clear the counters between runs; normally handled by TI_profile_matMul_test
            TI_profile_clear_run_stats();

         } // end of memory allocation successful?

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
         TI_align_free(pIn0);
         TI_align_free(pIn1);

      } // end repetitions
   }    // end idat test cases

   free(handle);
#if !defined(EVM_TEST)
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif // #if !defined(EVM_TEST)

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_matMul");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_matMul_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t                testNum         = 1000;
   int32_t                currentTestFail = 0;
   DSPLIB_matMul_InitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_matMul_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_STATUS status_nat;
   DSPLIB_STATUS status_opt;

   DSPLIB_bufParams2D_t bufParamsIn0, bufParamsIn1, bufParamsOut;

   int   fail = 0;
   void *pIn0, *pIn1, *pOut;

   int32_t M = 16;
   int32_t K = 16;
   int32_t N = 16;

   bufParamsIn0.data_type = DSPLIB_FLOAT32;
   bufParamsIn0.dim_x     = K;
   bufParamsIn0.dim_y     = M;
   bufParamsIn0.stride_y  = bufParamsIn0.dim_y * DSPLIB_sizeof(bufParamsIn0.data_type);

   bufParamsIn1.data_type = DSPLIB_FLOAT32;
   bufParamsIn1.dim_x     = N;
   bufParamsIn1.dim_y     = K;
   bufParamsIn1.stride_y  = bufParamsIn1.dim_y * DSPLIB_sizeof(bufParamsIn1.data_type);

   bufParamsOut.data_type = bufParamsIn0.data_type;
   bufParamsOut.dim_x     = N;
   bufParamsOut.dim_y     = M;
   bufParamsOut.stride_y  = bufParamsOut.dim_y * DSPLIB_sizeof(bufParamsOut.data_type);

   uint64_t In0Size = bufParamsIn0.stride_y * bufParamsIn0.dim_y;
   uint64_t In1Size = bufParamsIn1.stride_y * bufParamsIn1.dim_y;
   uint64_t OutSize = bufParamsOut.stride_y * bufParamsOut.dim_y;

   DSPLIB_bufParams2D_t bufParamsIn0Temp, bufParamsIn1Temp, bufParamsOutTemp;

   pIn0 = (void *) malloc(In0Size);
   pIn1 = (void *) malloc(In1Size);
   pOut = (void *) malloc(OutSize);

   while (testNum <= 1013) {

      switch (testNum) {
      case 1000:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init_checkParams(NULL, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_matMul_init_checkParams(NULL, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                          &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:

         bufParamsIn0Temp.data_type = DSPLIB_UINT32;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = DSPLIB_UINT32;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsIn1Temp.data_type = DSPLIB_UINT32;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = 11;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1004:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = 11;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = 11;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1005:
         bufParamsIn0Temp.data_type = DSPLIB_INT8;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);

         bufParamsIn0Temp.data_type = DSPLIB_UINT8;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;

      case 1006:
         bufParamsIn0Temp.data_type = DSPLIB_INT16;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);

         bufParamsIn0Temp.data_type = DSPLIB_UINT16;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;

      case 1007:
         bufParamsIn0Temp.data_type = DSPLIB_INT32;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);

         bufParamsIn0Temp.data_type = DSPLIB_UINT32;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;

      case 1008:
         bufParamsIn0Temp.data_type = DSPLIB_INT64;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);

         bufParamsIn0Temp.data_type = DSPLIB_UINT64;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1009:
         bufParamsIn0Temp.data_type = DSPLIB_INT32;
         bufParamsIn1Temp.data_type = 123;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);

         bufParamsIn0Temp.data_type = DSPLIB_FLOAT16;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1010:
         bufParamsIn0Temp.data_type = DSPLIB_INT128;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);

         bufParamsIn0Temp.data_type = DSPLIB_UINT128;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1011:
         bufParamsIn0Temp.data_type = DSPLIB_UINT24;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         bufParamsIn0Temp.data_type = 123;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         currentTestFail = currentTestFail || (status_opt != DSPLIB_ERR_INVALID_TYPE);
         break;
      case 1012:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         status_nat = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         status_nat = DSPLIB_matMul_exec_checkParams(handle, NULL, pIn1, pOut);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         status_opt = DSPLIB_matMul_exec_checkParams(handle, pIn0, NULL, pOut);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1013:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsIn1Temp.data_type = bufParamsIn1.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsIn1Temp.dim_x = bufParamsIn1.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;
         bufParamsIn1Temp.dim_y = bufParamsIn1.dim_y;
         bufParamsOutTemp.dim_y = bufParamsOut.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         status_nat = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         status_nat = DSPLIB_matMul_exec_checkParams(handle, pIn0, pIn1, NULL);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp,
                                                     &kerInitArgs);
         status_opt = DSPLIB_matMul_init(handle, &bufParamsIn0Temp, &bufParamsIn1Temp, &bufParamsOutTemp, &kerInitArgs);
         status_opt = DSPLIB_matMul_exec_checkParams(handle, pIn0, pIn1, NULL);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;

      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   free(pIn0);
   free(pIn1);

   free(pOut);

   free(handle);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_matMul_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_matMul_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_matMul_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for inmatMulidual test projects */
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
