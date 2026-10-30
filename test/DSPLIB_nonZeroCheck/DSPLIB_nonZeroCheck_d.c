// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>
#include <stddef.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_nonZeroCheck_idat.h"
#include "DSPLIB_types.h"

#ifdef WIN32
#if defined(__C7504__) || defined(__C7524__)
int8_t ddrBuffer[2048 * 1024];
#else
__attribute__((section(".msmcData"), aligned(128))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t   ddrBuffer[2048 * 1024];
#endif
#else
#if defined(__C7504__) || defined(__C7524__)
__attribute__((section(".ddrData"), aligned(64))) int8_t ddrBuffer[2048 * 1024];

#else
__attribute__((section(".msmcData"), aligned(128))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t   ddrBuffer[2048 * 1024];

#endif
#endif // WIN32

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int DSPLIB_nonZeroCheck_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   uint64_t             estCycles;
   uint64_t             archCycles;
   DSPLIB_bufParams2D_t bufParamsIn0;
   DSPLIB_bufParams1D_t bufParamsOut;

   uint32_t testNum;
   uint32_t comparisonDone = 0;

   DSPLIB_nonZeroCheck_testParams_t *prm;
   DSPLIB_nonZeroCheck_testParams_t  currPrm;
   DSPLIB_nonZeroCheck_getTestParams(&prm, &test_cases);

   DSPLIB_nonZeroCheck_InitArgs kerInitArgs;

   DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 0\n");

   int32_t             handleSize = DSPLIB_nonZeroCheck_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 1\n");

   TI_profile_init("DSPLIB_nonZeroCheck");

   // file IO for EVM benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_nonZeroCheck.csv", "w+");
   fprintf(fpOutputCSV, "Test ID, Bit Width, inSamples, inChannels, "
                        " EST cycles, ARCH cycles, EVM cycles, Pass/Fail\n");

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
         estCycles                       = 0;
         archCycles                      = 0;
         currentTestFail                 = 0;

         uint32_t opType      = currPrm.opType;
         kerInitArgs.opType   = opType;
         uint32_t axisType    = currPrm.axisType;
         kerInitArgs.axisType = axisType;

         bufParamsIn0.data_type = currPrm.dataType;
         bufParamsIn0.dim_x     = currPrm.inSamples;
         bufParamsIn0.dim_y     = currPrm.inChannels;
         bufParamsIn0.stride_y  = currPrm.strideIn;

         if (axisType == 0) {
            bufParamsOut.data_type = currPrm.dataType;
            bufParamsOut.dim_x     = currPrm.inSamples;
         }
         else {
            bufParamsOut.data_type = currPrm.dataType;
            bufParamsOut.dim_x     = currPrm.inChannels;
         }

         // Here, stride is in bytes
         uint32_t inp0SizeBytes = bufParamsIn0.dim_y * bufParamsIn0.stride_y;
         uint32_t outSizeBytes  = bufParamsOut.dim_x * DSPLIB_sizeof(bufParamsOut.data_type);

         void *pIn0 = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, inp0SizeBytes);

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

         DSPLIB_DEBUGPRINTFN(0, "pIn0: %p pOut: %p pOutCn: %p\n", pIn0, pOut, pOutCn);

         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn0 && pOut && pOutCn) {

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn0, prm[tpi].staticIn0, bufParamsIn0.dim_x,
                                   bufParamsIn0.dim_y, bufParamsIn0.stride_y, DSPLIB_sizeof(currPrm.dataType),
                                   testPatternString);
            }
            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pIn0, prm[tpi].staticIn0, bufParamsIn0.dim_x,
                             bufParamsIn0.dim_y, bufParamsIn0.stride_y, DSPLIB_sizeof(currPrm.dataType),
                             testPatternString);
            }

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 0\n");

            status_init = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 1 status_init %d\n", status_init);

            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init           = DSPLIB_nonZeroCheck_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 2 status_init %d\n", status_init);

            status_opt = DSPLIB_nonZeroCheck_exec_checkParams(handle, pIn0, pOut);

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 3 status_opt %d\n", status_opt);

            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_nonZeroCheck_exec(handle, pIn0, pOut);

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
               status_opt = DSPLIB_nonZeroCheck_exec(handle, pIn0, pOut);
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
            status_opt = DSPLIB_nonZeroCheck_exec(handle, pIn0, pOut);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();

#endif // #if !defined(__C7X_HOSTEM__)
            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 4 status_opt %d\n", status_opt);

            // initialize the kernel to use the natural C variant
            DSPLIB_nonZeroCheck_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 5\n");
#endif
            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            status_nat = DSPLIB_nonZeroCheck_exec(handle, pIn0, pOutCn);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 6 status_nat %d\n", status_nat);
#endif
            status_nat_vs_opt = TI_compare_mem_2D((void *) pOut, (void *) pOutCn, 0, 0, bufParamsOut.dim_x, 1, 0,
                                                  DSPLIB_sizeof(currPrm.dataType));

            comparisonDone = 1;

            DSPLIB_DEBUGPRINTFN(
                0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 7 comparisonDone %d status_nat_vs_opt %d\n",
                comparisonDone, status_nat_vs_opt);

            if (currPrm.staticOut != NULL) {

               status_ref_vs_opt =
                   TI_compare_mem_2DDualStride((void *) pOut, (void *) currPrm.staticOut, 0, 0, bufParamsOut.dim_x, 1,
                                               0, 0, DSPLIB_sizeof(currPrm.dataType));

               comparisonDone = 1;
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

            DSPLIB_DEBUGPRINTFN(0,
                                "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 8 status_nat_vs_opt %d "
                                "status_ref_vs_opt %d currentTestFail "
                                "%d\n",
                                status_nat_vs_opt, status_ref_vs_opt, currentTestFail);

            DSPLIB_DEBUGPRINTFN(
                0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 8 status_init %d status_opt %d status_nat %d\n",
                status_init, status_opt, status_nat);

            /* Set the 'fail' flag based on test vector comparison results */

            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) || (status_nat != DSPLIB_SUCCESS) ||
                 (comparisonDone == 0) || (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

            DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  DSPLIB_nonZeroCheck_d CP 8 fail %d\n", fail);

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            DSPLIB_nonZeroCheck_perfEst(handle, &archCycles, &estCycles);

            sprintf(desc, "%s generated input | inSamples = %d, inChannels = %d", testPatternString, currPrm.inSamples,
                    currPrm.inChannels);

            // write to CSV, must happen prior to write to screen because
            // TI_profile_formula_add clears values in counters
            fprintf(fpOutputCSV, "%d, %d, %d, %d,%ld,%ld,%d , %d\n", currPrm.testID,
                    DSPLIB_sizeof(currPrm.dataType) * 8, currPrm.inSamples, currPrm.inChannels, estCycles, archCycles,
                    pProfile[3 * tpi + 1], !currentTestFail);

            TI_profile_add_test(testNum++, (currPrm.inSamples * currPrm.inChannels), archCycles, estCycles,
                                currentTestFail, desc);
         }
         else {
            sprintf(desc, "%s data does not fit in memory | inSamples = %d, inChannels = %d", testPatternString,
                    currPrm.inSamples, currPrm.inChannels);
            TI_profile_skip_test(desc);

            // clear the counters between runs; normally handled by TI_profile_add_test
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
      TI_memError("DSPLIB_nonZeroCheck");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_nonZeroCheck_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t                      testNum         = 1000;
   int32_t                      currentTestFail = 0;
   DSPLIB_nonZeroCheck_InitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_nonZeroCheck_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_STATUS status_nat;
   DSPLIB_STATUS status_opt;

   DSPLIB_bufParams2D_t bufParamsIn0;
   DSPLIB_bufParams1D_t bufParamsOut;

   int fail = 0;

   int32_t  inSamples  = 16;
   int32_t  inChannels = 16;
   uint32_t opType     = 0;
   uint32_t axisType   = 0;

   bufParamsIn0.data_type = DSPLIB_UINT32;
   bufParamsIn0.dim_x     = inChannels;
   bufParamsIn0.dim_y     = inSamples;
   bufParamsIn0.stride_y  = bufParamsIn0.dim_y * DSPLIB_sizeof(bufParamsIn0.data_type);

   bufParamsOut.data_type = DSPLIB_UINT32;
   bufParamsOut.dim_x     = inChannels;

   DSPLIB_bufParams2D_t bufParamsIn0Temp;
   DSPLIB_bufParams1D_t bufParamsOutTemp;

   while (testNum <= 1014) {

      switch (testNum) {
      case 1000:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = axisType;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(NULL, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_nonZeroCheck_init_checkParams(NULL, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:

         bufParamsIn0Temp.data_type = DSPLIB_UINT8;
         bufParamsOutTemp.data_type = DSPLIB_FLOAT64;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = axisType;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = DSPLIB_FLOAT64;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = axisType;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = 3;
         kerInitArgs.axisType  = axisType;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_VALUE) || (status_opt != DSPLIB_ERR_INVALID_VALUE));
         break;
      case 1004:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x = bufParamsOut.dim_x;

         bufParamsIn0Temp.dim_y = bufParamsIn0.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = 2;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_VALUE) || (status_opt != DSPLIB_ERR_INVALID_VALUE));
         break;
      case 1005:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x    = 0;
         bufParamsOutTemp.dim_x    = 0;
         bufParamsIn0Temp.dim_y    = bufParamsIn0.dim_y;
         bufParamsIn0Temp.stride_y = bufParamsIn0.stride_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = axisType;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1006:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x    = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x    = bufParamsOut.dim_x;
         bufParamsIn0Temp.dim_y    = bufParamsIn0.dim_y;
         bufParamsIn0Temp.stride_y = bufParamsIn0.dim_x * DSPLIB_sizeof(bufParamsIn0.data_type) - 1;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = axisType;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1007:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x    = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x    = bufParamsOut.dim_x + 1;
         bufParamsIn0Temp.dim_y    = bufParamsIn0.dim_y;
         bufParamsIn0Temp.stride_y = bufParamsIn0.stride_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = axisType;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1008:
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = axisType;
         status_nat            = DSPLIB_nonZeroCheck_init_checkParams(handle, NULL, &bufParamsOut, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0, NULL, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1009:
         bufParamsIn0Temp.data_type = DSPLIB_INT128;
         bufParamsOutTemp.data_type = DSPLIB_INT128;

         bufParamsIn0Temp.dim_x    = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x    = bufParamsOut.dim_x;
         bufParamsIn0Temp.dim_y    = bufParamsIn0.dim_y;
         bufParamsIn0Temp.stride_y = bufParamsIn0.stride_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = axisType;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);
         status_opt = DSPLIB_nonZeroCheck_init(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1010:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x    = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x    = bufParamsOut.dim_x;
         bufParamsIn0Temp.dim_y    = 0;
         bufParamsIn0Temp.stride_y = bufParamsIn0.stride_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = axisType;
         status_nat = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1011:
         /* Any non-NULL address serves as a buffer pointer: exec_checkParams only checks the pointers */
         status_nat = DSPLIB_nonZeroCheck_exec_checkParams(NULL, handle, handle);
         status_opt = DSPLIB_nonZeroCheck_exec_checkParams(handle, NULL, handle);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1012:
         status_nat = DSPLIB_nonZeroCheck_exec_checkParams(handle, handle, NULL);
         status_opt = DSPLIB_nonZeroCheck_exec_checkParams(handle, handle, handle);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_SUCCESS));
         break;
      case 1013:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x    = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x    = bufParamsOut.dim_x;
         bufParamsIn0Temp.dim_y    = bufParamsIn0.dim_y;
         bufParamsIn0Temp.stride_y = bufParamsIn0.stride_y;

         /* init without init_checkParams: an invalid opType must not leave the exec function unset */
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = 3;
         kerInitArgs.axisType  = axisType;
         status_nat            = DSPLIB_nonZeroCheck_init(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_nonZeroCheck_init(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_VALUE) || (status_opt != DSPLIB_ERR_INVALID_VALUE));
         break;
      case 1014:
         bufParamsIn0Temp.data_type = bufParamsIn0.data_type;
         bufParamsOutTemp.data_type = bufParamsOut.data_type;

         bufParamsIn0Temp.dim_x    = bufParamsIn0.dim_x;
         bufParamsOutTemp.dim_x    = bufParamsOut.dim_x;
         bufParamsIn0Temp.dim_y    = bufParamsIn0.dim_y;
         bufParamsIn0Temp.stride_y = bufParamsIn0.stride_y;

         /* init without init_checkParams: an invalid axisType must not fall back to the COLUMN variant */
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         kerInitArgs.opType    = opType;
         kerInitArgs.axisType  = 2;
         status_nat            = DSPLIB_nonZeroCheck_init(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_nonZeroCheck_init(handle, &bufParamsIn0Temp, &bufParamsOutTemp, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_VALUE) || (status_opt != DSPLIB_ERR_INVALID_VALUE));
         break;

      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }

   free(handle);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_nonZeroCheck_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_nonZeroCheck_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_nonZeroCheck_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for individual test projects */
#if !defined(__ONESHOTTEST)
int main()
{
   int test_fail = 1, cov_fail = 1;

   uint32_t profile[1024 * 3];

   DSPLIB_TEST_init();

   test_fail = test_main(&profile[0]);
   cov_fail  = coverage_test_main();

#if !defined(NO_PRINTF)
   if ((test_fail == 0) && (cov_fail == 0))
      printf("All Test Pass!\n");
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
