// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_dotprod_idat.h"

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

int DSPLIB_dotprod_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

#if (defined(RTL_TEST) || defined(QT_TEST) || defined(EVM_TEST))
   uint32_t k = 0;
#endif
   uint32_t testNum    = 0;
   uint64_t archCycles = 0;
   uint64_t estCycles  = 0;

   DSPLIB_dotprod_testParams_t *prm;
   DSPLIB_dotprod_testParams_t  currPrm;
   DSPLIB_dotprod_getTestParams(&prm, &test_cases);

   DSPLIB_dotprod_InitArgs kerInitArgs;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 0\n");
#endif

   int32_t             handleSize = DSPLIB_dotprod_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 1\n");
#endif

   TI_profile_init("DSPLIB_dotprod");
#if (!defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_dotprod.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Bit Width, Parameters, Data size, Batch size, Filter size, Arch "
                        "cycles, Loki cycles, Loki/Arch cycles, Pass/Fail\n");
#endif //   #if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

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

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;

         currentTestFail = 0;

         kerInitArgs.dataSize = currPrm.dataSize;

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsIn.dim_x     = currPrm.dataSize;

         bufParamsOut.data_type = currPrm.dataTypeOut;
         bufParamsOut.dim_x     = currPrm.dataSizeOut;

         int32_t dataWidth;
         int32_t dataWidthOut;

         if (currPrm.dataType == DSPLIB_FLOAT32) {
            dataWidth = sizeof(DSPLIB_F32);
         }
         else if (currPrm.dataType == DSPLIB_FLOAT64) {
            dataWidth = sizeof(DSPLIB_D64);
         }
         else {
            dataWidth = DSPLIB_sizeof(currPrm.dataType);
         }

         if (bufParamsOut.data_type == DSPLIB_FLOAT32) {
            dataWidthOut = sizeof(DSPLIB_F32);
         }
         else if (bufParamsOut.data_type == DSPLIB_FLOAT64) {
            dataWidthOut = sizeof(DSPLIB_D64);
         }
         else {
            dataWidthOut = DSPLIB_sizeof(bufParamsOut.data_type);
         }

         void *pIn1 = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.dim_x * dataWidth);
         void *pIn2 = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.dim_x * dataWidth);

         void *pOut, *pOutCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_x * dataWidthOut);
            pOutCn = (void *) malloc(bufParamsOut.dim_x * dataWidthOut);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_x * dataWidthOut);
#else
            pOut              = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_x * dataWidthOut);
#else
            pOut              = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d data_type %d dataSize %d \n", bufParamsIn.data_type,
                kerInitArgs.dataSize);
         printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d pIn1  %p pIn2 %p\n", pIn1, pIn2);
#endif
         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn1 && pIn2 && pOut && pOutCn) {

            /* No need to fill anything here, we just used patter 0xAB
             * for testing purposes.                                    */

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn1, prm[tpi].staticIn0, bufParamsIn.dim_x, 1, 0,
                                   DSPLIB_sizeof(currPrm.dataType), testPatternString);
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn2, prm[tpi].staticIn1, bufParamsIn.dim_x, 1, 0,
                                   DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }
            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pIn1, prm[tpi].staticIn0, bufParamsIn.dim_x, 1, 0,
                             DSPLIB_sizeof(currPrm.dataType), testPatternString);

               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pIn2, prm[tpi].staticIn1, bufParamsIn.dim_x, 1, 0,
                             DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 0\n");
#endif

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 1 status_init %d\n", status_init);
#endif
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init           = DSPLIB_dotprod_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 2 status_init %d\n", status_init);
#endif
            status_opt = DSPLIB_dotprod_exec_checkParams(handle, pIn1, pIn2, pOut);
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 3 status_opt %d\n", status_opt);
#endif
            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_dotprod_exec(handle, pIn1, pIn2, pOut);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
            }

#if (defined(RTL_TEST) || defined(QT_TEST) || defined(EVM_TEST))
            /* The following for loop is to call kernel repeatedly so as to
             * train the branch predictor                                   */
            for (k = 0; k < 4; k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               DSPLIB_asm(" MARK 4");
               status_opt = DSPLIB_dotprod_exec(handle, pIn1, pIn2, pOut);
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
            status_opt = DSPLIB_dotprod_exec(handle, pIn1, pIn2, pOut);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();
#endif

#if (!defined(RTL_TEST))
            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 4 status_opt %d\n", status_opt);
#endif
            // initialize the kernel to use the natural C variant
            DSPLIB_dotprod_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 5\n");
#endif
            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            status_nat = DSPLIB_dotprod_exec(handle, pIn1, pIn2, pOutCn);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 6 status_nat %d\n", status_nat);
#endif

            if (bufParamsOut.data_type == DSPLIB_FLOAT32 || bufParamsOut.data_type == DSPLIB_FLOAT64) {
               status_nat_vs_opt =
                   TI_compare_mem_2D_float((void *) pOut, (void *) pOutCn, 0.001, (double) powf(2, -10),
                                           bufParamsOut.dim_x, 1, 0, DSPLIB_sizeof(bufParamsOut.data_type));
            }
            else {
               status_nat_vs_opt = TI_compare_mem_2D((void *) pOut, (void *) pOutCn, 0, 0, bufParamsOut.dim_x, 1, 0,
                                                     DSPLIB_sizeof(bufParamsOut.data_type));
            }

            comparisonDone = 1;

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 7 comparisonDone %d status_nat_vs_opt %d\n", comparisonDone,
                   status_nat_vs_opt);
#endif
#else
            status_nat_vs_opt = TI_TEST_KERNEL_PASS;
#endif

            if (currPrm.staticOut != NULL) {
               if (bufParamsOut.data_type == DSPLIB_FLOAT32 || bufParamsOut.data_type == DSPLIB_FLOAT64) {
                  status_ref_vs_opt =
                      TI_compare_mem_2D_float((void *) currPrm.staticOut, (void *) pOut, 0.001, (double) powf(2, -10),
                                              1, 1, 0, DSPLIB_sizeof(bufParamsOut.data_type));
               }
               else {
                  status_ref_vs_opt =
                      TI_compare_mem_2D((void *) currPrm.staticOut, (void *) pOut, 2, 65535, bufParamsOut.dim_x, 1, 0,
                                        DSPLIB_sizeof(bufParamsOut.data_type));
               }

               comparisonDone = 1;
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 8 status_nat_vs_opt %d status_ref_vs_opt %d currentTestFail "
                   "%d\n",
                   status_nat_vs_opt, status_ref_vs_opt, currentTestFail);
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 8 status_init %d status_opt %d status_nat %d\n", status_init,
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
            printf("DSPLIB_DEBUGPRINT  DSPLIB_dotprod_d CP 8 fail %d\n", fail);
#endif

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);
#if defined(QT_TEST)
            __ESTP_GS = pProfile[3 * tpi];
            __FSR     = pProfile[3 * tpi + 1];
            __ESTP_S  = pProfile[3 * tpi + 2];
            __GPLY    = !fail;
            __DBGCTXT = testNum;
#endif // #if defined(QT_TEST)

#if !defined(RTL_TEST)
            sprintf(desc, "%s generated input | Data size = %d", testPatternString, currPrm.dataSize);

#if (!defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
            // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            // fprintf(fpOutputCSV, "add, %d, %d, %d, Data size:%d, %d, %d, %d, %lu, %lu, %.2f, %d\n", testNum,
            //         currPrm.testPattern, DSPLIB_sizeof(bufParamsIn.data_type) * 8, currPrm.dataSize,
            //         currPrm.dataSize, archCycles, cycles[TI_PROFILE_KERNEL_OPT],
            //         ((DSPLIB_F32) cycles[TI_PROFILE_KERNEL_OPT]) / ((DSPLIB_F32) archCycles), !currentTestFail);
#endif // #if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

            TI_profile_add_test(testNum++, currPrm.dataSize, 0, 0, currentTestFail, desc);
         }
         else {
            sprintf(desc, "Data size=%d", currPrm.dataSize);
            TI_profile_skip_test(desc);
#else
            // clear the counters between runs; normally handled by TI_profile_add_test
            TI_profile_clear_run_stats();
#endif
         } // end of memory allocation successful?

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         TI_align_free(pIn1);
         TI_align_free(pIn2);

#if defined(RTL_TEST)
         // for RTL testing, request to return after addst fail
         if (fail) {
            free(handle);
            return fail;
         }
#endif
      } // end repetitions
   }    // end idat test cases

   free(handle);

#if (!defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif //#if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_dotprod");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_dotprod_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t testNum         = 1001;
   int32_t currentTestFail = 0;
   int32_t fail            = 0;
   int32_t status_init     = 0;

   DSPLIB_bufParams1D_t    bufParamsIn, bufParamsOut;
   DSPLIB_dotprod_InitArgs kerInitArgs;
   int32_t                 handleSize = DSPLIB_dotprod_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle     handle     = malloc(handleSize);
   void                   *pIn1       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 4);
   void                   *pIn2       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 4);
   void                   *pOut       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 4);

   /* CASE 1001 - NULL HANDLE CASE*/
   status_init = DSPLIB_dotprod_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1002 - Invalid Type case*/
   bufParamsIn.data_type = DSPLIB_INT64;
   status_init           = DSPLIB_dotprod_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1003 - Invalid Type case*/
   bufParamsIn.data_type  = DSPLIB_INT16;
   bufParamsOut.data_type = DSPLIB_INT64;
   status_init            = DSPLIB_dotprod_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1004 - Invalid Type case*/
   bufParamsIn.data_type = DSPLIB_INT64;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
   status_init           = DSPLIB_dotprod_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1005 - Invalid Type case*/
   bufParamsIn.data_type = DSPLIB_INT64;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
   status_init           = DSPLIB_dotprod_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1006 - Null pointer case*/
   status_init     = DSPLIB_dotprod_exec_checkParams(handle, NULL, pIn2, pOut);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1007 - Null pointer case*/
   status_init     = DSPLIB_dotprod_exec_checkParams(handle, pIn1, NULL, pOut);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1008 - Null pointer case*/
   status_init     = DSPLIB_dotprod_exec_checkParams(handle, pIn1, pIn2, NULL);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1009 - Invalid Type case*/
   bufParamsIn.data_type  = DSPLIB_INT16;
   bufParamsOut.data_type = DSPLIB_INT16;
   status_init            = DSPLIB_dotprod_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_SUCCESS);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1010 - Branch coverage case*/
   bufParamsIn.data_type  = DSPLIB_INT32;
   bufParamsOut.data_type = DSPLIB_INT32;
   status_init            = DSPLIB_dotprod_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_SUCCESS);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1011 - Branch coverage case*/
   bufParamsIn.data_type  = DSPLIB_INT8;
   bufParamsOut.data_type = DSPLIB_INT8;
   status_init            = DSPLIB_dotprod_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_SUCCESS);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   TI_align_free(pIn1);
   TI_align_free(pIn2);
   TI_align_free(pOut);
   free(handle);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_dotprod_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_dotprod_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_dotprod_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for inaddidual test projects */
#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
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
   if (fail == 0) {
      printf("Coverage Test Pass!\n");
   }
   else {
      printf("Coverage Test Fail!\n");
   }
   return fail;
}
#endif
