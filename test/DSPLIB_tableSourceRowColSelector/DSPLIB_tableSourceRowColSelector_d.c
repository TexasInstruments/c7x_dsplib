// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/DSPLIB_test.h"
#include "DSPLIB_tableSourceRowColSelector_idat.h"
#include <dsplib.h>
#include <string.h>

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
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];
#endif
#endif                            // WIN32
int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int DSPLIB_tableSourceRowColSelector_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams1D_t bufParamsIn0;
   DSPLIB_bufParams2D_t bufParamsIn1;
   DSPLIB_bufParams2D_t bufParamsOut;
   uint32_t             testNum;
   uint64_t             archCycles = 0;
   uint64_t             estCycles  = 0;

   DSPLIB_tableSourceRowColSelector_testParams_t *prm;
   DSPLIB_tableSourceRowColSelector_testParams_t  currPrm;
   DSPLIB_tableSourceRowColSelector_getTestParams(&prm, &test_cases);

   DSPLIB_tableSourceRowColSelector_InitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_tableSourceRowColSelector_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   if (handle == NULL) {
      printf("Failed to allocate kernel handle\n");
      return 1;
   }

   TI_profile_init("DSPLIB_tableSourceRowColSelector");

   FILE *fpOutputCSV = fopen("DSPLIB_tableSourceRowColSelector.csv", "w+");
   if (fpOutputCSV == NULL) {
      printf("Failed to open DSPLIB_tableSourceRowColSelector.csv\n");
      free(handle);
      return 1;
   }
   fprintf(fpOutputCSV, "Kernel, Test ID, Bit Width, Num rows, Num cols, Num indices, estCycles, EVM "
                        "cycles, Pass/Fail, Per vector cycle\n");

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

         bufParamsIn0.data_type = DSPLIB_INT32;
         bufParamsIn0.dim_x     = currPrm.numIndices;

         bufParamsIn1.data_type = currPrm.dataType;
         bufParamsIn1.dim_x     = currPrm.numCols;
         bufParamsIn1.dim_y     = currPrm.numRows;
         bufParamsIn1.stride_y  = bufParamsIn1.dim_x * dataWidth;

         uint32_t numSel = (currPrm.isColSelect == 0U) ? currPrm.numRows : currPrm.numCols;
         uint32_t selLen = (currPrm.isColSelect == 0U) ? currPrm.numCols : currPrm.numRows;

         bufParamsOut.data_type = currPrm.dataType;
         if (currPrm.isOutputInterleave == 1U) {
            bufParamsOut.dim_x = currPrm.numIndices;
            bufParamsOut.dim_y = selLen;
         }
         else {
            bufParamsOut.dim_x = selLen;
            bufParamsOut.dim_y = currPrm.numIndices;
         }
         bufParamsOut.stride_y = currPrm.strideOut;

         kerInitArgs.isColSelect        = (uint8_t) currPrm.isColSelect;
         kerInitArgs.isOutputInterleave = (uint8_t) currPrm.isOutputInterleave;

         DSPLIB_DEBUGPRINTFN(0, "data_type: %d numRows: %d numCols: %d numIndices: %d\n", bufParamsOut.data_type,
                             currPrm.numRows, currPrm.numCols, currPrm.numIndices);

         // Here, stride is in bytes
         uint32_t inp0SizeBytes = bufParamsIn0.dim_x * sizeof(int32_t);
         uint32_t inp1SizeBytes = bufParamsIn1.dim_y * bufParamsIn1.stride_y;
         uint32_t outSizeBytes  = bufParamsOut.dim_y * bufParamsOut.stride_y;

         void *pIn0 = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, inp0SizeBytes);
         void *pIn1 = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, inp1SizeBytes);

         /* Scratch rows and columns are padded to a full vector, the same as scratchStride in init_ci */
         uint32_t eleCount      = __C7X_VEC_SIZE_BYTES__ / (uint32_t) dataWidth;
         uint32_t scratchStride = DSPLIB_ceilingDiv(selLen, eleCount) * eleCount * (uint32_t) dataWidth;
         uint32_t scratchRows   = DSPLIB_ceilingDiv(currPrm.numIndices, eleCount) * eleCount;
         uint32_t scratchSize   = (currPrm.isOutputInterleave == 1U) ? (scratchStride * scratchRows) : 0U;
         void    *pScratch      = NULL;
         if (scratchSize > 0) {
            pScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, scratchSize);
         }

         void *pOut, *pOutCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, outSizeBytes);
            pOutCn = (void *) malloc(outSizeBytes);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, outSizeBytes);
#else
            pOut   = (void *) msmcBuffer;
#endif

#if defined(DSPLIB_TEST_CN_OUTPUT_L2_HEAP) && defined(__C7504__)
            pOutCn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, outSizeBytes);
#else
            pOutCn = (void *) ddrBuffer;
#endif
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, outSizeBytes);
#else
            pOut   = (void *) msmcBuffer;
#endif
            pOutCn = (void *) ddrBuffer;
         }
         DSPLIB_DEBUGPRINTFN(0, "pIn0: %p pIn1: %p pOut: %p pOutCn: %p\n", pIn0, pIn1, pOut, pOutCn);

         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn0 && pIn1 && pOut && pOutCn && (pScratch || (scratchSize == 0))) {
            /* Fill input data; random indices span [1, numSel + 1] to also exercise the upper clamp */
            if (prm[tpi].testPattern == STATIC) {
               TI_fillBuffer(STATIC, 0, pIn0, prm[tpi].staticIn0, bufParamsIn0.dim_x, 1, 0, sizeof(int32_t),
                             testPatternString);
            }
            else {
               uint8_t maxIdx = (numSel + 2 > 255) ? 255 : (uint8_t) (numSel + 2);
               TI_fillBuffer(RANDOM_SMALL, maxIdx, pIn0, NULL, bufParamsIn0.dim_x, 1, 0, sizeof(int32_t),
                             testPatternString);
            }

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn1, prm[tpi].staticIn1, bufParamsIn1.dim_x,
                                   bufParamsIn1.dim_y, bufParamsIn1.stride_y, dataWidth, testPatternString);
            }
            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pIn1, prm[tpi].staticIn1, bufParamsIn1.dim_x,
                             bufParamsIn1.dim_y, bufParamsIn1.stride_y, dataWidth, testPatternString);
            }

            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                            &bufParamsOut, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init = DSPLIB_tableSourceRowColSelector_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut,
                                                                   &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }

            status_opt = DSPLIB_tableSourceRowColSelector_exec_checkParams(handle, pIn0, pIn1, pOut, pScratch);

            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_tableSourceRowColSelector_exec(handle, pIn0, pIn1, pOut, pScratch);
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
               status_opt = DSPLIB_tableSourceRowColSelector_exec(handle, pIn0, pIn1, pOut, pScratch);
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
            status_opt = DSPLIB_tableSourceRowColSelector_exec(handle, pIn0, pIn1, pOut, pScratch);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)

            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
            status_init = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                            &bufParamsOut, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 8");
               status_init = DSPLIB_tableSourceRowColSelector_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut,
                                                                   &kerInitArgs);
               DSPLIB_asm(" MARK 9");
               TI_profile_stop();
            }
            // initialize the kernel to use the natural C variant

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            status_nat = DSPLIB_tableSourceRowColSelector_exec(handle, pIn0, pIn1, pOutCn, pScratch);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();

            if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
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
               if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
                  status_ref_vs_opt =
                      TI_compare_mem_2D_float((void *) currPrm.staticOut, (void *) pOut, 0.001, (double) powf(2, -10),
                                              bufParamsOut.dim_x, bufParamsOut.dim_y, bufParamsOut.stride_y, dataWidth);
               }
               else {
                  status_ref_vs_opt =
                      TI_compare_mem_2D((void *) currPrm.staticOut, (void *) pOut, 0, 0, bufParamsOut.dim_x,
                                        bufParamsOut.dim_y, bufParamsOut.stride_y, dataWidth);
               }

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

            sprintf(desc, "%s generated input | numRows = %d, numCols = %d, numIndices = %d, Data Type %d",
                    testPatternString, currPrm.numRows, currPrm.numCols, currPrm.numIndices, currPrm.dataType);

            uint64_t hwCycles = 0;
            float    per_vector_cycles;
            uint64_t num_se_fetches =
                ((selLen * currPrm.numIndices * dataWidth) + (__C7X_VEC_SIZE_BYTES__ - 1)) / __C7X_VEC_SIZE_BYTES__;
            hwCycles          = pProfile[3 * tpi + 1];
            per_vector_cycles = (float_t) ((double_t) hwCycles / (double_t) num_se_fetches);
            // write to CSV, must happen prior to write to screen because TI_profile_formula clears values in counters
            DSPLIB_tableSourceRowColSelector_perfEst(handle, &archCycles, &estCycles);
            fprintf(fpOutputCSV, "tableSourceRowColSelector, %d, %d, %d, %d, %d, %ld, %d, %d, %3.2f\n", testNum,
                    dataWidth * 8, currPrm.numRows, currPrm.numCols, currPrm.numIndices, estCycles, (int) hwCycles,
                    !currentTestFail, per_vector_cycles);
            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum++, (selLen * currPrm.numIndices), archCycles, estCycles, currentTestFail, desc);
         }
         else {
            sprintf(desc, "numRows=%d, numCols=%d, numIndices=%d", currPrm.numRows, currPrm.numCols,
                    currPrm.numIndices);

            DSPLIB_DEBUGPRINTFN(0, "%s\n", "Skipping Test");

            TI_profile_skip_test(desc);
            // clear the counters between runs
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

#if defined(DSPLIB_TEST_CN_OUTPUT_L2_HEAP) && defined(__C7504__)
            TI_align_free(pOutCn);
#endif
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pOut);
#endif
         }
         if (pScratch != NULL) {
            TI_align_free(pScratch);
         }
         TI_align_free(pIn1);
         TI_align_free(pIn0);

      } // end repetitions
   }    // end idat test cases

   free(handle);
   /* Close results CSV */
   fclose(fpOutputCSV);
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return fail: %d\n", fail);

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_tableSourceRowColSelector");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_tableSourceRowColSelector_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t                                   testNum         = 1000;
   int32_t                                   currentTestFail = 0;
   DSPLIB_STATUS                             status_nat;
   DSPLIB_STATUS                             status_opt;
   DSPLIB_bufParams1D_t                      bufParamsIn0;
   DSPLIB_bufParams2D_t                      bufParamsIn1;
   DSPLIB_bufParams2D_t                      bufParamsOut;
   DSPLIB_tableSourceRowColSelector_InitArgs kerInitArgs;
   int32_t                                   handleSize = DSPLIB_tableSourceRowColSelector_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle                       handle     = malloc(handleSize);
   int                                       fail       = 0;
   void                                     *pIn0       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                                     *pIn1       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                                     *pOut       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                                     *pScratch   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);

   if (handle == NULL) {
      printf("Failed to allocate kernel handle\n");
      return 1;
   }

   bufParamsIn0.data_type = DSPLIB_INT32;
   bufParamsIn0.dim_x     = 2;

   bufParamsIn1.dim_x    = 4;
   bufParamsIn1.dim_y    = 4;
   bufParamsIn1.stride_y = 16;

   bufParamsOut.dim_x    = 2;
   bufParamsOut.dim_y    = 4;
   bufParamsOut.stride_y = 16;

   while (testNum <= 1023) {
      bufParamsIn0.data_type = DSPLIB_INT32;
      bufParamsIn0.dim_x     = 2;
      bufParamsIn1.data_type = DSPLIB_INT32;
      bufParamsIn1.dim_x     = 4;
      bufParamsIn1.dim_y     = 4;
      bufParamsIn1.stride_y  = 16;
      bufParamsOut.data_type = DSPLIB_INT32;
      bufParamsOut.dim_x     = 2;
      bufParamsOut.dim_y     = 4;
      bufParamsOut.stride_y  = 16;

      kerInitArgs.isColSelect        = 1;
      kerInitArgs.isOutputInterleave = 1;

      switch (testNum) {
      case 1000:
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_tableSourceRowColSelector_init_checkParams(NULL, &bufParamsIn0, &bufParamsIn1,
                                                                                   &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_tableSourceRowColSelector_init_checkParams(NULL, &bufParamsIn0, &bufParamsIn1,
                                                                                   &bufParamsOut, &kerInitArgs);
         currentTestFail       = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         bufParamsIn0.data_type = DSPLIB_FLOAT32;
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                             &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_nat != DSPLIB_ERR_INVALID_TYPE);
         break;
      case 1002:
         bufParamsIn1.data_type = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT128;
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                             &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_nat != DSPLIB_ERR_INVALID_TYPE);
         break;
      case 1003:
         bufParamsOut.data_type = DSPLIB_FLOAT32;
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                             &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_nat != DSPLIB_ERR_INVALID_TYPE);
         break;
      case 1004:
         bufParamsOut.dim_x = 3;
         status_nat         = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                                &bufParamsOut, &kerInitArgs);
         currentTestFail    = (status_nat != DSPLIB_ERR_INVALID_DIMENSION);
         break;
      case 1005:
         bufParamsIn0.dim_x = 3;
         status_nat         = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                                &bufParamsOut, &kerInitArgs);
         currentTestFail    = (status_nat != DSPLIB_ERR_INVALID_DIMENSION);
         break;
      case 1006:
         bufParamsIn1.stride_y = 32;
         status_nat            = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                                   &bufParamsOut, &kerInitArgs);
         currentTestFail       = (status_nat != DSPLIB_ERR_NOT_EQUAL_WIDTH_STRIDE);
         break;
      case 1007:
         status_nat      = DSPLIB_tableSourceRowColSelector_exec_checkParams(handle, NULL, pIn1, pOut, pScratch);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1008:
         status_nat      = DSPLIB_tableSourceRowColSelector_exec_checkParams(handle, pIn0, NULL, pOut, pScratch);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1009:
         status_nat      = DSPLIB_tableSourceRowColSelector_exec_checkParams(handle, pIn0, pIn1, NULL, pScratch);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1010:
         bufParamsIn1.data_type = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT128;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat =
             DSPLIB_tableSourceRowColSelector_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt =
             DSPLIB_tableSourceRowColSelector_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1011:
         bufParamsOut.stride_y = 4;
         status_nat            = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                                   &bufParamsOut, &kerInitArgs);
         currentTestFail       = (status_nat != DSPLIB_ERR_INVALID_DIMENSION);
         break;
      case 1012:
         bufParamsIn0.dim_x = 0;
         status_nat         = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                                &bufParamsOut, &kerInitArgs);
         currentTestFail    = (status_nat != DSPLIB_ERR_INVALID_DIMENSION);
         break;
      case 1013:
         bufParamsIn1.dim_x = 0;
         status_nat         = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                                &bufParamsOut, &kerInitArgs);
         currentTestFail    = (status_nat != DSPLIB_ERR_INVALID_DIMENSION);
         break;
      case 1014:
         bufParamsIn1.dim_y = 0;
         status_nat         = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                                &bufParamsOut, &kerInitArgs);
         currentTestFail    = (status_nat != DSPLIB_ERR_INVALID_DIMENSION);
         break;
      case 1015:
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, NULL, &bufParamsOut,
                                                                             &kerInitArgs);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1016:
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                             &bufParamsOut, NULL);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1017:
         kerInitArgs.isColSelect = 2;
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                             &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_nat != DSPLIB_ERR_INVALID_VALUE);
         break;
      case 1018:
         bufParamsOut.stride_y = 18;
         status_nat            = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                                   &bufParamsOut, &kerInitArgs);
         currentTestFail       = (status_nat != DSPLIB_ERR_INVALID_DIMENSION);
         break;
      case 1019:
         kerInitArgs.isOutputInterleave = 2;
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                             &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_nat != DSPLIB_ERR_INVALID_VALUE);
         break;
      case 1020:
         /* Interleaved output dimensions with a non-interleaved output */
         kerInitArgs.isOutputInterleave = 0;
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                             &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_nat != DSPLIB_ERR_INVALID_DIMENSION);
         break;
      case 1021:
         /* Row select, non-interleaved: output is numIndices rows of numCols */
         kerInitArgs.isColSelect        = 0;
         kerInitArgs.isOutputInterleave = 0;
         bufParamsOut.dim_x             = 4;
         bufParamsOut.dim_y             = 2;
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                             &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_nat != DSPLIB_SUCCESS);
         break;
      case 1022:
         /* The interleaved output needs the scratch buffer, the non-interleaved output accepts NULL */
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_nat =
             DSPLIB_tableSourceRowColSelector_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
         status_nat                     = (status_nat == DSPLIB_SUCCESS)
                                              ? DSPLIB_tableSourceRowColSelector_exec_checkParams(handle, pIn0, pIn1, pOut, NULL)
                                              : status_nat;
         kerInitArgs.isOutputInterleave = 0;
         bufParamsOut.dim_x             = 4;
         bufParamsOut.dim_y             = 2;
         status_opt =
             DSPLIB_tableSourceRowColSelector_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
         status_opt      = (status_opt == DSPLIB_SUCCESS)
                               ? DSPLIB_tableSourceRowColSelector_exec_checkParams(handle, pIn0, pIn1, pOut, NULL)
                               : status_opt;
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_SUCCESS));
         break;
      case 1023:
         /* Complex tables are not supported */
         bufParamsIn1.data_type = DSPLIB_CINT32;
         bufParamsOut.data_type = DSPLIB_CINT32;
         status_nat      = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1,
                                                                             &bufParamsOut, &kerInitArgs);
         currentTestFail = (status_nat != DSPLIB_ERR_INVALID_TYPE);
         break;
      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   TI_align_free(pIn0);
   TI_align_free(pIn1);
   TI_align_free(pOut);
   TI_align_free(pScratch);
   free(handle);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_tableSourceRowColSelector_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_tableSourceRowColSelector_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_tableSourceRowColSelector_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for individual test projects */
#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
int main()
{
   int test_fail = 1, cov_fail = 1;

   uint32_t profile[512 * 3];

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

   return test_fail || cov_fail;
}
#endif
