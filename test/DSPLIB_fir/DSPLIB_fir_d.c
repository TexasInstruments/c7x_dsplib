// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_bufParams.h"
#include "DSPLIB_fir_idat.h"
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

#define EVM_TEST 0

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int DSPLIB_fir_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;
   DSPLIB_bufParams2D_t bufParamsFilter;

   uint32_t k = 0;
   uint32_t testNum;
   uint64_t archCycles = 0;
   uint64_t estCycles  = 0;

   DSPLIB_fir_testParams_t *prm;
   DSPLIB_fir_testParams_t  currPrm;
   DSPLIB_fir_getTestParams(&prm, &test_cases);

   DSPLIB_fir_InitArgs kerInitArgs;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d CP 0\n");
#endif

   int32_t             handleSize = DSPLIB_fir_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d CP 1\n");
#endif

   TI_profile_init("DSPLIB_fir");

#if (!(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_fir.csv", "w+");
   fprintf(fpOutputCSV, "Test ID, Test type, Data Type, Data size, Batch size, Filter size, Enable MMA, Enable Nchan "
                        "Coeffs, Pass/Fail, SNR, Num Cycles\n");
#endif //   #if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         int32_t status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t status_ref_vs_nat = TI_TEST_KERNEL_FAIL;

         DSPLIB_STATUS status_init = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt  = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat  = DSPLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;

         currentTestFail = 0;

         kerInitArgs.dataSize       = currPrm.dataSize;
         kerInitArgs.batchSize      = currPrm.batchSize;
         kerInitArgs.filterSize     = currPrm.filterSize;
         kerInitArgs.enableNchCoefs = currPrm.enableNchCoefs;

         if (currPrm.dataType == DSPLIB_FLOAT32)
            kerInitArgs.MMA_SIZE = DSPLIB_MMA_SIZE_32_BIT;
         else if (currPrm.dataType == DSPLIB_INT32)
            kerInitArgs.MMA_SIZE = DSPLIB_MMA_SIZE_32_BIT;
         else if (currPrm.dataType == DSPLIB_INT16)
            kerInitArgs.MMA_SIZE = DSPLIB_MMA_SIZE_16_BIT;
         else if (currPrm.dataType == DSPLIB_INT8)
            kerInitArgs.MMA_SIZE = DSPLIB_MMA_SIZE_8_BIT;

         // TODO: enable MMA execution mode
         kerInitArgs.enableMMA = currPrm.enableMMA;
         kerInitArgs.enableQ   = currPrm.enableQ;
         kerInitArgs.Q         = currPrm.Q;

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsIn.dim_y     = currPrm.batchSize;
         bufParamsIn.stride_y  = currPrm.dataPitchIn;
         bufParamsIn.dim_x     = currPrm.dataSize;

         bufParamsOut.data_type = currPrm.dataType;
         bufParamsOut.dim_y     = currPrm.batchSize;
         bufParamsOut.stride_y  = currPrm.dataPitchOut;
         bufParamsOut.dim_x     = currPrm.dataPitchOut / DSPLIB_sizeof(bufParamsOut.data_type);

         bufParamsFilter.data_type = currPrm.dataType;
         bufParamsFilter.dim_x     = currPrm.filterSize;
         bufParamsFilter.dim_y     = currPrm.batchSize;
         bufParamsFilter.stride_y  = currPrm.filterPitch;

         /* change buffer sizes and strides according to user input */
         DSPLIB_fir_getSizes(&kerInitArgs, &bufParamsIn, &bufParamsFilter, &bufParamsOut);

         void *pIn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.stride_y * bufParamsIn.dim_y);
         void *pFilter;
         if (currPrm.enableNchCoefs == 0) {
            pFilter = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsFilter.stride_y);
         }
         else {
            pFilter = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsFilter.dim_y * bufParamsFilter.stride_y);
         }
         void *pOut, *pOutCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {

            /* printf("malloc heap\n"); */
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_y * bufParamsOut.stride_y);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_y * bufParamsOut.stride_y);
#else
            pOut = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_y * bufParamsOut.stride_y);
#else
            pOut = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }

         pOutCn = (void *) ddrBuffer;

#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d data_type %d dataSize %d "
                "filterSize %d\n",
                bufParamsIn.data_type, kerInitArgs.dataSize, kerInitArgs.filterSize);
#endif
         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn && pOut && pFilter && pOutCn) {

            /* No need to fill anything here, we just used patter 0xAB
             * for testing purposes.                                    */
            if (bufParamsIn.data_type == DSPLIB_FLOAT32) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, currPrm.dataSize, currPrm.batchSize,
                                   currPrm.dataPitchIn, sizeof(DSPLIB_F32), testPatternString);
            }

            else {

               TI_fillBuffer(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, currPrm.dataSize, currPrm.batchSize,
                             currPrm.dataPitchIn, DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }

            printf("filter dimx = %d, dimy = %d, stridey = %d\n", bufParamsFilter.dim_x, bufParamsFilter.dim_y,
                   bufParamsFilter.stride_y);

            if (kerInitArgs.enableMMA) {

               /* fill filter with all zeros */
               /* fillConst(0, pFilter, bufParamsFilter.dim_x, bufParamsFilter.dim_y, bufParamsFilter.str, */
               /*           DSPLIB_sizeof(bufParamsFilter.data_type)); */

               memset(pFilter, 0, bufParamsFilter.stride_y * bufParamsFilter.dim_y);

               /* fill relevant parts with coefficients */
               TI_fillBuffer_float(prm[tpi].testPattern, 0,
                                   (pFilter + 2 * kerInitArgs.MMA_SIZE * DSPLIB_sizeof(bufParamsFilter.data_type)),
                                   prm[tpi].filter, currPrm.filterSize, bufParamsFilter.dim_y, bufParamsFilter.stride_y,
                                   DSPLIB_sizeof(bufParamsFilter.data_type), testPatternString);
            }

            else {

               if (bufParamsFilter.data_type == DSPLIB_FLOAT32) {
                  TI_fillBuffer_float(prm[tpi].testPattern, 0, pFilter, prm[tpi].filter, bufParamsFilter.dim_x,
                                      bufParamsFilter.dim_y, bufParamsFilter.stride_y, sizeof(DSPLIB_F32),
                                      testPatternString);
               }

               else {

                  TI_fillBuffer(prm[tpi].testPattern, 0, pFilter, prm[tpi].filter, currPrm.filterSize,
                                currPrm.batchSize, currPrm.filterPitch, DSPLIB_sizeof(bufParamsFilter.data_type),
                                testPatternString);
               }
            }

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d CP 0\n");
#endif

            status_init =
                DSPLIB_fir_init_checkParams(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d CP 1 status_init %d\n", status_init);
#endif
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d CP 2 status_init %d\n", status_init);
#endif
            status_opt = DSPLIB_fir_exec_checkParams(handle, pIn, pFilter, pOut);
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d CP 3 status_opt %d\n", status_opt);
#endif
            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_fir_exec(handle, pIn, pFilter, pOut);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
            }

            /* The following for loop is to call kernel repeatedly so as to
             * train the branch predictor                                   */
            for (k = 0; k < DSPLIB_NUM_WARM_REPS; k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               DSPLIB_asm(" MARK 4");
               status_opt = DSPLIB_fir_exec(handle, pIn, pFilter, pOut);
               DSPLIB_asm(" MARK 5");
               TI_profile_stop();
            }

            /* get output to L1D  */
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
            status_opt = DSPLIB_fir_exec(handle, pIn, pFilter, pOut);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();

            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d CP 4 status_opt %d\n", status_opt);
#endif

            // initialize the kernel to use the natural C variant
            DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d CP 5\n");
#endif
            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            if (kerInitArgs.enableMMA)
               status_nat = DSPLIB_fir_exec(
                   handle, pIn, pFilter + 2 * kerInitArgs.MMA_SIZE * DSPLIB_sizeof(bufParamsFilter.data_type), pOutCn);
            else
               status_nat = DSPLIB_fir_exec(handle, pIn, pFilter, pOutCn);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_fir_d CP 6 status_nat %d\n", status_nat);
#endif

            printf("natc vs opt\n");
            status_nat_vs_opt =
                TI_compare_mem_2D_float((void *) pOut, (void *) pOutCn, 0, (double) powf(2, -10), bufParamsOut.dim_x,
                                        bufParamsOut.dim_y, bufParamsOut.stride_y, sizeof(DSPLIB_F32));

            comparisonDone = 1;

            if (currPrm.staticOut != NULL) {

               printf("ref vs opt\n");
               status_ref_vs_opt = TI_compare_mem_2D_float(
                   (void *) pOut, (void *) currPrm.staticOut, 0, (double) powf(2, -10), bufParamsOut.dim_x,
                   bufParamsOut.dim_y, bufParamsOut.stride_y, sizeof(DSPLIB_F32));

               printf("ref vs natc\n");

               status_ref_vs_nat = TI_compare_mem_2D_float(
                   (void *) pOutCn, (void *) currPrm.staticOut, 0, (double) powf(2, -10), bufParamsOut.dim_x,
                   bufParamsOut.dim_y, bufParamsOut.stride_y, sizeof(DSPLIB_F32));

               comparisonDone = 1;
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
               status_ref_vs_nat = TI_TEST_KERNEL_PASS;
            }

            /* Calculate SNR */
            double resultSNR =
                calculateSNR((float *) currPrm.staticOut, (float *) pOut, bufParamsOut.dim_x, bufParamsOut.dim_y,
                             bufParamsOut.stride_y / DSPLIB_sizeof(bufParamsOut.data_type));

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_nat == TI_TEST_KERNEL_FAIL) || (status_init != DSPLIB_SUCCESS) ||
                 (status_opt != DSPLIB_SUCCESS) || (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0) ||
                 (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | Data size=%d, Batch size=%d, Filter size=%d, Enable MMA=%d",
                    testPatternString, currPrm.dataSize, currPrm.batchSize, currPrm.filterSize, currPrm.enableMMA);

            DSPLIB_fir_perfEst(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &archCycles, &estCycles);

#if (!(EVM_TEST))
            // write to CSV, must happen prior to write to screen because
            // TI_profile_formula_add clears values in counters
            fprintf(fpOutputCSV, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %d\n", currPrm.testID, currPrm.testPattern,
                    currPrm.dataType, currPrm.dataSize, currPrm.batchSize, currPrm.filterSize, currPrm.enableMMA,
                    currPrm.enableNchCoefs, !currentTestFail, resultSNR, pProfile[3 * tpi + 1]);
#endif // #if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

            TI_profile_add_test(testNum++, currPrm.dataSize * currPrm.batchSize * currPrm.filterSize, 0, 0,
                                currentTestFail, desc);
         }
         else {

            printf("Could not allocate all mem\n");
            sprintf(desc, "Data size=%d, Batch size=%d, Filter size=%d, Enable MMA= %d", currPrm.dataSize,
                    currPrm.batchSize, currPrm.filterSize, currPrm.enableMMA);
            TI_profile_skip_test(desc);
            // clear the counters between runs; normally handled by
            // TI_profile_add_test
            TI_profile_clear_run_stats();
         } // end of memory allocation successful?
         /* Free buffers for each test vector */
         TI_align_free(pIn);
         TI_align_free(pFilter);
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            /* free(pOutCn); */
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
      } // end repetitions
   }    // end idat test cases

   free(handle);

#if (!(EVM_TEST))
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif //#if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_fir");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_fir_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t              testNum         = 1000;
   int32_t              currentTestFail = 0;
   DSPLIB_STATUS        status_nat, status_init, status_nat_init, status_opt_init, status_fir;
   DSPLIB_STATUS        status_opt;
   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;
   DSPLIB_bufParams2D_t bufParamsFilter;
   DSPLIB_fir_InitArgs  kerInitArgs;

   memset(&kerInitArgs, 0, sizeof(kerInitArgs));
   // OR initialize specific fields:
   kerInitArgs.dataSize       = 16;
   kerInitArgs.filterSize     = 4;
   kerInitArgs.batchSize      = 1;
   kerInitArgs.enableMMA      = 0;
   kerInitArgs.MMA_SIZE       = DSPLIB_MMA_SIZE_32_BIT;
   kerInitArgs.enableNchCoefs = 0;
   kerInitArgs.enableQ        = 0;
   kerInitArgs.Q              = 0;
   kerInitArgs.funcStyle      = DSPLIB_FUNCTION_NATC;

   int32_t             handleSize = DSPLIB_fir_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);
   int                 fail       = 0;
   void               *pIn        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void               *pFilter    = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void               *pOut       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);

   bufParamsIn.dim_x  = 16;
   bufParamsOut.dim_x = 16;

   while (testNum <= 1010) {

      switch (testNum) {
      case 1000:
         status_init = DSPLIB_fir_init_checkParams(NULL, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
         break;

      case 1001:
         bufParamsIn.data_type     = DSPLIB_INT16;
         bufParamsOut.data_type    = DSPLIB_INT64;
         bufParamsFilter.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle     = DSPLIB_FUNCTION_NATC;

         status_init = DSPLIB_fir_init_checkParams(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_nat_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_fir            = DSPLIB_fir_getSizes(&kerInitArgs, &bufParamsIn, &bufParamsFilter, &bufParamsOut);
         currentTestFail       = ((status_init != DSPLIB_ERR_INVALID_TYPE) || (status_nat_init != DSPLIB_SUCCESS) ||
                            (status_opt_init != DSPLIB_SUCCESS) || (status_fir != DSPLIB_SUCCESS));
         break;

      case 1002:
         bufParamsIn.data_type     = DSPLIB_INT32;
         bufParamsOut.data_type    = DSPLIB_INT32;
         bufParamsFilter.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_init = DSPLIB_fir_init_checkParams(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_nat_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_fir            = DSPLIB_fir_getSizes(&kerInitArgs, &bufParamsIn, &bufParamsFilter, &bufParamsOut);
         currentTestFail       = ((status_init != DSPLIB_ERR_INVALID_TYPE) || (status_nat_init != DSPLIB_SUCCESS) ||
                            (status_opt_init != DSPLIB_SUCCESS) || (status_fir != DSPLIB_SUCCESS));
         break;

      case 1003:
         bufParamsIn.data_type = DSPLIB_INT8;
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_init = DSPLIB_fir_init_checkParams(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_nat_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_fir            = DSPLIB_fir_getSizes(&kerInitArgs, &bufParamsIn, &bufParamsFilter, &bufParamsOut);
         currentTestFail       = ((status_init != DSPLIB_ERR_INVALID_TYPE) || (status_nat_init != DSPLIB_SUCCESS) ||
                            (status_opt_init != DSPLIB_SUCCESS) || (status_fir != DSPLIB_SUCCESS));
         break;

      case 1004:
         bufParamsIn.data_type = DSPLIB_UINT64;
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_init = DSPLIB_fir_init_checkParams(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_nat_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_fir            = DSPLIB_fir_getSizes(&kerInitArgs, &bufParamsIn, &bufParamsFilter, &bufParamsOut);
         currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE) || (status_nat_init != DSPLIB_ERR_INVALID_TYPE) ||
                            (status_opt_init != DSPLIB_ERR_INVALID_TYPE) || (status_fir != DSPLIB_ERR_NOT_IMPLEMENTED));
         break;

      case 1005:
         status_init     = DSPLIB_fir_exec_checkParams(handle, NULL, pFilter, pOut);
         currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
         break;

      case 1006:
         status_init     = DSPLIB_fir_exec_checkParams(handle, pIn, NULL, pOut);
         currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
         break;

      case 1007:
         status_init     = DSPLIB_fir_exec_checkParams(handle, pIn, pFilter, NULL);
         currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
         break;

      case 1008:
         bufParamsIn.data_type  = DSPLIB_FLOAT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         kerInitArgs.dataSize   = 0;
         kerInitArgs.filterSize = 4;
         bufParamsOut.dim_x     = 0;
         status_nat_init        = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         currentTestFail        = (status_nat_init != DSPLIB_SUCCESS);
         break;

      case 1009:
         bufParamsIn.data_type  = DSPLIB_FLOAT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         kerInitArgs.dataSize   = 260;
         kerInitArgs.filterSize = 2;
         bufParamsOut.dim_x     = 0;
         status_nat_init        = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         currentTestFail        = (status_nat_init != DSPLIB_SUCCESS);
         break;

      case 1010:
         bufParamsIn.data_type     = DSPLIB_INT16;
         bufParamsOut.data_type    = DSPLIB_INT16;
         bufParamsFilter.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle     = DSPLIB_FUNCTION_NATC;

         status_init = DSPLIB_fir_init_checkParams(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_nat_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt_init       = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_fir            = DSPLIB_fir_getSizes(&kerInitArgs, &bufParamsIn, &bufParamsFilter, &bufParamsOut);
         currentTestFail       = ((status_init != DSPLIB_ERR_INVALID_TYPE) || (status_nat_init != DSPLIB_SUCCESS) ||
                            (status_opt_init != DSPLIB_SUCCESS) || (status_fir != DSPLIB_SUCCESS));
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
   TI_align_free(pFilter);
   free(handle);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_fir_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_fir_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_fir_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for individual test projects */
#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
int main()
{
   int fail = 1;

   uint32_t profile[300 * 3];

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
   if (fail == 0) {
      printf("Test Pass!\n");
   }
   else {
      printf("Test Fail!\n");
   }
   return fail;
}
#endif
