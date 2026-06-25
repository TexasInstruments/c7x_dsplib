// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_mat_submat_copy_idat.h"

// #define DSPLIB_TEST_CN_OUTPUT_L2_HEAP

#define DSPLIB_ROW_STRIDE(x, y) (((x + y) / y) * y)

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

int DSPLIB_mat_submat_copy_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsX, bufParamsY;
   uint64_t             hwCycles;
#if !defined(EVM_TEST)

#endif // #if !defined(EVM_TEST)
   uint32_t testNum;

   DSPLIB_mat_submat_copy_testParams_t *prm;
   DSPLIB_mat_submat_copy_testParams_t  currPrm;
   DSPLIB_mat_submat_copy_getTestParams(&prm, &test_cases);

   DSPLIB_mat_submat_copyInitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_mat_submat_copy_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   TI_profile_init("DSPLIB_mat_submat_copy");
#if !defined(EVM_TEST)
   // file IO for Loki benchmarking
   FILE *fpYputCSV = fopen("DSPLIB_mat_submat_copy.csv", "w+");
   fprintf(fpYputCSV, "Kernel, Test ID, Test type, Bit Width, Parameters, Data size, Batch size, Filter size, HW "
                      "cycles, Pass/Fail, Per vector cycle\n");
#endif //  #if !defined(EVM_TEST)

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         DSPLIB_DEBUGPRINTFN(0, "Current TestID: %d Current Repetition: %d\n", currPrm.testID, repCount + 1);

         int32_t       status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t       status_ref_vs_opt = TI_TEST_KERNEL_PASS;
         DSPLIB_STATUS status_init       = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt        = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat        = DSPLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;
         int32_t eleCount       = 0;

         currentTestFail = 0;

         bufParamsX.data_type = currPrm.dataType;
         bufParamsX.dim_x     = currPrm.widthX;
         bufParamsX.dim_y     = currPrm.heightX;
         bufParamsX.stride_y  = currPrm.strideX;
         if (bufParamsX.stride_y % 512 == 0) {
            bufParamsX.stride_y += 128;
         }

         kerInitArgs.dir   = currPrm.dir;
         kerInitArgs.stRow = currPrm.stRow;
         kerInitArgs.stCol = currPrm.stCol;
         kerInitArgs.nRows = currPrm.nRows;
         kerInitArgs.nCols = currPrm.nCols;

         bufParamsY.data_type = currPrm.dataType;
         bufParamsY.dim_x     = currPrm.nCols;
         bufParamsY.dim_y     = currPrm.nRows;
         bufParamsY.stride_y  = currPrm.strideY;

         if (bufParamsY.stride_y % 512 == 0) {
            bufParamsY.stride_y += 128;
         }

         uint64_t xSize = bufParamsX.stride_y * bufParamsX.dim_y;
         uint64_t ySize = bufParamsY.stride_y * bufParamsY.dim_y;

         DSPLIB_DEBUGPRINTFN(0, "I/P data_type: %d O/P data_type: %d\n", bufParamsX.data_type, bufParamsY.data_type);
         DSPLIB_DEBUGPRINTFN(0, "widthX: %d heightX: %d strideX:  %d\n", bufParamsX.dim_x, bufParamsX.dim_y,
                             bufParamsX.stride_y);
         DSPLIB_DEBUGPRINTFN(0, "widthOut: %d heightOut: %d strideY: %d\n", bufParamsY.dim_x, bufParamsY.dim_y,
                             bufParamsY.stride_y);
         void *pY, *pYCn, *pX, *pXCn;

         int32_t usedBytes = 0;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pX   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            pY   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, ySize);
            pYCn = (void *) malloc(ySize);
            pXCn = (void *) malloc(xSize);
            usedBytes += xSize;
            usedBytes += ySize;
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            pX = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            pY = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, ySize);

#else
            if (!currPrm.dir) {
                 pX = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
                 pY = (void *) msmcBuffer;
            }
            else {
                 pX = (void *) msmcBuffer;
                 pY = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, ySize);
            }

#endif
            usedBytes = 0;
            pXCn      = (void *) (ddrBuffer);
            usedBytes += xSize;
            pYCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += ySize;
         }

         DSPLIB_DEBUGPRINTFN(0, "pX: %p pY: %p pYCn: %p\n", pX, pY, pYCn);

         /* Only run the test if the buffer allocations fit in the heap */
         if (pX && pY && pYCn && pXCn) {

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(currPrm.testPattern, 0, pX, currPrm.staticX, bufParamsX.dim_x, bufParamsX.dim_y,
                                   bufParamsX.stride_y, DSPLIB_sizeof(currPrm.dataType), testPatternString);
               TI_fillBuffer_float(currPrm.testPattern, 0, pY, currPrm.staticY, bufParamsY.dim_x, bufParamsY.dim_y,
                                   bufParamsY.stride_y, DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }
            else {
               TI_fillBuffer(currPrm.testPattern, (uint8_t) 255, pX, currPrm.staticX, bufParamsX.dim_x,
                             bufParamsX.dim_y, bufParamsX.stride_y, DSPLIB_sizeof(currPrm.dataType), testPatternString);
               TI_fillBuffer(currPrm.testPattern, (uint8_t) 255, pY, currPrm.staticY, bufParamsY.dim_x,
                             bufParamsY.dim_y, bufParamsY.stride_y, DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }
            memcpy(pXCn, pX, xSize);
            memcpy(pYCn, pY, ySize);

            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init = DSPLIB_mat_submat_copy_init_checkParams(handle, &bufParamsX, &bufParamsY, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               status_init = DSPLIB_mat_submat_copy_init(handle, &bufParamsX, &bufParamsY, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
               status_opt = DSPLIB_mat_submat_copy_exec_checkParams(handle, pX, pY);
            }
            else {
               status_opt = status_init;
            }

            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_mat_submat_copy_exec(handle, pX, pY);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();

#if !defined(__C7X_HOSTEM__)
               /* The following for loop is to call kernel repeatedly so as to
                * train the branch predictor                                   */
               uint32_t k;
               for (k = 0; k < 4; k++) {
                  // run warm instruction cache test
                  TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
                  DSPLIB_asm(" MARK 4");
                  status_opt = DSPLIB_mat_submat_copy_exec(handle, pX, pY);
                  DSPLIB_asm(" MARK 5");
                  TI_profile_stop();
               }

               // get output to L1D
               int16_t outSum = 0;
               int8_t *pYTemp = (int8_t *) pY; // treat output as bytes to be data type agnostic
               for (k = 0; k < bufParamsY.dim_x; k++) {
                  outSum += *pYTemp;
                  pYTemp++;
               }

               // dummy store of outSum to insure that the compiler does not remove it.
               volatileSum = outSum;

               // run warm instruction cache test
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

               DSPLIB_asm(" MARK 6");
               status_opt = DSPLIB_mat_submat_copy_exec(handle, pX, pY);
               DSPLIB_asm(" MARK 7");
               TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)
            }
            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
            status_init = DSPLIB_mat_submat_copy_init_checkParams(handle, &bufParamsX, &bufParamsY, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 8");
               status_init = DSPLIB_mat_submat_copy_init(handle, &bufParamsX, &bufParamsY, &kerInitArgs);
               DSPLIB_asm(" MARK 9");
               TI_profile_stop();

               status_opt = DSPLIB_mat_submat_copy_exec_checkParams(handle, pXCn, pYCn);
            }
            else {
               status_opt = status_init;
            }

            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CN);
               DSPLIB_asm(" MARK 10");
               status_opt = DSPLIB_mat_submat_copy_exec(handle, pXCn, pYCn);
               DSPLIB_asm(" MARK 11");
               TI_profile_stop();
            }

            if (status_opt == DSPLIB_SUCCESS) {
               if (!kerInitArgs.dir) {
                  if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                     status_nat_vs_opt = TI_compare_mem_2D_float(
                         (void *) pY, (void *) pYCn, 0.001, (double) powf(2, -10), bufParamsY.dim_x, bufParamsY.dim_y,
                         bufParamsY.stride_y, DSPLIB_sizeof(currPrm.dataType));
                  }
                  else {
                     status_nat_vs_opt =
                         TI_compare_mem_2D((void *) pY, (void *) pYCn, 0, 0, bufParamsY.dim_x, bufParamsY.dim_y,
                                           bufParamsY.stride_y, DSPLIB_sizeof(currPrm.dataType));
                  }

                  comparisonDone = 1;

                  if (currPrm.staticOut != NULL) {
                     if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                        status_ref_vs_opt = TI_compare_mem_2DDualStride_float(
                            (void *) pY, (void *) currPrm.staticOut, 0.001, (double) powf(2, -10), bufParamsY.dim_x,
                            bufParamsY.dim_y, bufParamsY.stride_y, bufParamsY.dim_x * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                     }
                     else {
                        status_ref_vs_opt = TI_compare_mem_2DDualStride(
                            (void *) pY, currPrm.staticOut, 0, 0, bufParamsY.dim_x, bufParamsY.dim_y,
                            bufParamsY.stride_y, bufParamsY.dim_x * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                     }
                     comparisonDone = 1;
                  }
                  else {
                     /* Set to pass since it wasn't supposed to run. */
                     status_ref_vs_opt = TI_TEST_KERNEL_PASS;
                  }
               }
               else {
                  if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {

                     status_nat_vs_opt = TI_compare_mem_2D_float(
                         (void *) pX, (void *) pXCn, 0.001, (double) powf(2, -10), bufParamsX.dim_x, bufParamsX.dim_y,
                         bufParamsX.stride_y, DSPLIB_sizeof(currPrm.dataType));
                  }
                  else {
                     status_nat_vs_opt =
                         TI_compare_mem_2D((void *) pX, (void *) pXCn, 0, 0, bufParamsX.dim_x, bufParamsX.dim_y,
                                           bufParamsX.stride_y, DSPLIB_sizeof(currPrm.dataType));
                  }

                  comparisonDone = 1;

                  if (currPrm.staticOut != NULL) {
                     if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                        status_ref_vs_opt = TI_compare_mem_2DDualStride_float(
                            (void *) pX, (void *) currPrm.staticOut, 0.001, (double) powf(2, -10), bufParamsX.dim_x,
                            bufParamsX.dim_y, bufParamsX.stride_y, bufParamsX.dim_x * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                     }
                     else {
                        status_ref_vs_opt = TI_compare_mem_2DDualStride(
                            (void *) pX, currPrm.staticOut, 0, 0, bufParamsX.dim_x, bufParamsX.dim_y,
                            bufParamsX.stride_y, bufParamsX.dim_x * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                     }
                     comparisonDone = 1;
                  }
                  else {
                     /* Set to pass since it wasn't supposed to run. */
                     status_ref_vs_opt = TI_TEST_KERNEL_PASS;
                  }
               }

               DSPLIB_DEBUGPRINTFN(0, "status_nat_vs_opt: %d status_ref_vs_opt: %d currentTestFail: %d\n",
                                   status_nat_vs_opt, status_ref_vs_opt, currentTestFail);

               /* Set the 'fail' flag based on test vector comparison results */
               currentTestFail =
                   ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                    (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) ||
                    (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0) || (currentTestFail == 1))
                       ? 1
                       : 0;

               fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

               pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
               pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
               pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);
            }
            else {
               DSPLIB_DEBUGPRINTFN(0, "Skipping Current Test as initialization failed with status %d\n", status_opt);
               pProfile[3 * tpi]     = 0;
               pProfile[3 * tpi + 1] = 0;
               pProfile[3 * tpi + 2] = 0;
            }

            sprintf(desc,
                    "%s Input|colX=%dr rowX=%d|nRows=%d nCols=%d|dir=%d|stRow=%d stCol=%d|"
                    "Data Type %d",
                    testPatternString, currPrm.widthX, currPrm.heightX, currPrm.nRows, currPrm.nCols, currPrm.dir,
                    currPrm.stRow, currPrm.stCol, currPrm.dataType);
            uint64_t hwCycles_local = 0;
            hwCycles_local          = pProfile[3 * tpi + 1];

#if !defined(EVM_TEST)
            float    per_vector_cycles;
            uint64_t num_se_fetches =
                (((currPrm.heightX * DSPLIB_sizeof(currPrm.dataType)) + (__C7X_VEC_SIZE_BYTES__ - 1)) /
                 __C7X_VEC_SIZE_BYTES__) *
                currPrm.widthX;
            hwCycles_local    = hwCycles;
            per_vector_cycles = (float_t) ((double_t) hwCycles / (double_t) num_se_fetches);
            // write to CSV, must happen prior to write to screen because TI_profile_formula clears values in counters
            fprintf(fpYputCSV, "mat_submat_copy, %d, %d, %d, -, %d, 1, 1, %d, %d, %3.2f\n", testNum,
                    currPrm.testPattern, DSPLIB_sizeof(bufParamsX.data_type) * 8, currPrm.nCols * currPrm.nRows,
                    (int) hwCycles, !currentTestFail, per_vector_cycles);
#endif // #if !defined(EVM_TEST)

            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum++, currPrm.nCols * currPrm.nRows, hwCycles_local, 0, currentTestFail, desc);
         }
         else {
            sprintf(desc, "Data size=%d", DSPLIB_sizeof(bufParamsX.data_type));

            DSPLIB_DEBUGPRINTFN(0, "%s\n", "Skipping Test");

            TI_profile_skip_test(desc);
            // clear the counters between runs
            TI_profile_clear_run_stats();
         } // end of memory allocation successful?

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pXCn);
            free(pYCn);

            TI_align_free(pX);
            TI_align_free(pY);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)

            TI_align_free(pX);
            TI_align_free(pY);
#else
            if (!currPrm.dir) {
                 TI_align_free(pX);
            }
            else {
                 TI_align_free(pY);
            }
#endif
         }

      } // end repetitions
   }    // end idat test cases

   free(handle);

#if !defined(EVM_TEST)
   /* Close results CSV */
   fclose(fpYputCSV);
#endif // #if !defined(EVM_TEST)

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return fail: %d\n", fail);

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_mat_submat_copy");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_mat_submat_copy_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t                        testNum         = 1000;
   int32_t                        currentTestFail = 0;
   DSPLIB_STATUS                  status_nat;
   DSPLIB_STATUS                  status_opt;
   DSPLIB_bufParams2D_t           bufParamsIn, bufParamsOut;
   DSPLIB_mat_submat_copyInitArgs kerInitArgs;
   int32_t                        handleSize = DSPLIB_mat_submat_copy_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle            handle     = malloc(handleSize);
   int                            fail       = 0;
   void                          *pIn        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                          *pOut       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);

   bufParamsIn.dim_x  = 16;
   bufParamsOut.dim_x = 16;

   kerInitArgs.dir   = 0;
   kerInitArgs.stRow = 0;
   kerInitArgs.stCol = 0;
   kerInitArgs.nRows = 0;
   kerInitArgs.nCols = 0;
   while (testNum <= 1006) {

      switch (testNum) {
      case 1000:
         bufParamsIn.data_type  = DSPLIB_INT32;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_mat_submat_copy_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_mat_submat_copy_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         bufParamsIn.data_type  = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_mat_submat_copy_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_mat_submat_copy_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsIn.data_type  = DSPLIB_INT32;
         bufParamsOut.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_mat_submat_copy_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_mat_submat_copy_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         status_nat      = DSPLIB_mat_submat_copy_exec_checkParams(handle, NULL, pOut);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1004:
         status_nat      = DSPLIB_mat_submat_copy_exec_checkParams(handle, pIn, NULL);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1005:
         bufParamsIn.data_type  = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_mat_submat_copy_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_mat_submat_copy_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1006:

         bufParamsIn.dim_x  = 16;
         bufParamsOut.dim_x = 16;

         kerInitArgs.dir   = 0;
         kerInitArgs.stRow = 0;
         kerInitArgs.stCol = 0;
         kerInitArgs.nRows = 18;
         kerInitArgs.nCols = 0;

         bufParamsIn.data_type  = DSPLIB_INT32;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_mat_submat_copy_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

         bufParamsIn.dim_x  = 16;
         bufParamsOut.dim_x = 16;

         kerInitArgs.dir       = 0;
         kerInitArgs.stRow     = 0;
         kerInitArgs.stCol     = 0;
         kerInitArgs.nRows     = 0;
         kerInitArgs.nCols     = 19;
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_mat_submat_copy_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         printf("\n status_nat %d  status_opt %d \n", status_nat, status_opt);
         break;
      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_mat_submat_copy_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_mat_submat_copy_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_mat_submat_copy_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for insqridual test projects */
#if !defined(__ONESHOTTEST)
int main()
{
   int fail = 1;

   uint32_t profile[1024 * 3] = {0};

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
