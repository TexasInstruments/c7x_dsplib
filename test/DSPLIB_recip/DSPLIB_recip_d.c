// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_recip_idat.h"

#if defined(__C7504__) || defined(__C7524__)
__attribute__((section(".l2sramaux"), aligned(64))) int8_t l2auxBuffer[DSPLIB_L2_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t   ddrBuffer[2048 * 1024];

#else
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];
#endif

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int DSPLIB_recip_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;

   uint32_t k = 0;

   uint32_t testNum;
   uint64_t archCycles = 0;
   uint64_t estCycles  = 0;

   recip_testParams_t *prm;
   recip_testParams_t  currPrm;
   recip_getTestParams(&prm, &test_cases);

   DSPLIB_recip_InitArgs kerInitArgs;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 0\n");
#endif

   int32_t             handleSize = DSPLIB_recip_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 1\n");
#endif

   TI_profile_init("DSPLIB_recip");

   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_recip.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Bit Width, Width, Height, Stride In, Stride Out, StoreStyle, archCycles, "
                        "estCycles, Loki Cycles\n");

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         int32_t       status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t       status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t       status_ref_vs_nat = TI_TEST_KERNEL_FAIL;
         DSPLIB_STATUS status_init       = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt        = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat        = DSPLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;

         currentTestFail = 0;
         archCycles      = 0;
         estCycles       = 0;

         kerInitArgs.dataSize = currPrm.width;

         // store style for fixed point (0 if floating)
         kerInitArgs.storeStyle = currPrm.storeStyle;
         bufParamsIn.data_type  = currPrm.dataType;
         bufParamsIn.dim_x      = currPrm.width;
         bufParamsIn.dim_y      = currPrm.height;
         bufParamsIn.stride_y   = currPrm.strideIn;

         bufParamsOut.data_type = currPrm.dataType;
         bufParamsOut.dim_y     = currPrm.height;
         bufParamsOut.stride_y  = currPrm.strideOut;

         // output size may differ based on floating vs fixed point computation
         if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
            bufParamsOut.dim_x = currPrm.width;
         }

         else {
            bufParamsOut.dim_x = 2 * currPrm.width;
         }

         int32_t dataWidth;

         if (currPrm.dataType == DSPLIB_FLOAT32) {
            dataWidth = sizeof(DSPLIB_F32);
         }
         else if (currPrm.dataType == DSPLIB_FLOAT64) {
            dataWidth = sizeof(DSPLIB_D64);
         }
         else {
            dataWidth = DSPLIB_sizeof(currPrm.dataType);
         }

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
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d data_type %d width %d height %d\n", bufParamsIn.data_type,
                bufParamsIn.dim_x, bufParamsIn.dim_y);
         printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d pIn  %p\n", pIn);
#endif
         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn && pOut && pOutCn) {

            /* No need to fill anything here, we just used patter 0xAB
             * for testing purposes.                                    */

            if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x,
                                   bufParamsIn.dim_y, bufParamsIn.stride_y, dataWidth, testPatternString);
            }

            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pIn, prm[tpi].staticIn, bufParamsIn.dim_x,
                             bufParamsIn.dim_y, bufParamsIn.stride_y, dataWidth, testPatternString);
            }

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 0\n");
#endif

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 1 status_init %d\n", status_init);
#endif
            status_init = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init           = DSPLIB_recip_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 2 status_init %d\n", status_init);
#endif
            status_opt = DSPLIB_recip_exec_checkParams(handle, pIn, pOut);
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 3 status_opt %d\n", status_opt);
#endif
            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_recip_exec(handle, pIn, pOut);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
            }

            /* The following for loop is to call kernel repeatedly so as to
             * train the branch predictor                                   */
            for (k = 0; k < (DSPLIB_NUM_WARM_REPS); k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               DSPLIB_asm(" MARK 4");
               status_opt = DSPLIB_recip_exec(handle, pIn, pOut);
               DSPLIB_asm(" MARK 5");
               TI_profile_stop();
            }

            // get output to L1D
            int16_t outSum   = 0;
            int8_t *pOutTemp = (int8_t *) pOut; // treat output as bytes to be data type agnostic
            for (k = 0; k < bufParamsOut.dim_y * bufParamsOut.stride_y; k++) {
               outSum += *pOutTemp;
               pOutTemp++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum = outSum;

            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            DSPLIB_asm(" MARK 6");
            status_opt = DSPLIB_recip_exec(handle, pIn, pOut);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();

            // estimate cycles while the handle still holds the optimized configuration
            DSPLIB_recip_perfEst(handle, &archCycles, &estCycles);

            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
            status_init           = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 4 status_opt %d\n", status_opt);
#endif
            // initialize the kernel to use the natural C variant
            DSPLIB_recip_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 5\n");
#endif
            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            status_nat = DSPLIB_recip_exec(handle, pIn, pOutCn);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 6 status_nat %d\n", status_nat);
#endif
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
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 7 comparisonDone %d status_nat_vs_opt %d\n", comparisonDone,
                   status_nat_vs_opt);
#endif

            if (currPrm.staticOut != NULL) {
               if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
                  status_ref_vs_opt = TI_compare_mem_2DDualStride_float(
                      (void *) pOut, (void *) currPrm.staticOut, 0.001, (double) powf(2, -10), bufParamsOut.dim_x,
                      bufParamsOut.dim_y, bufParamsOut.stride_y, bufParamsOut.dim_x * dataWidth, dataWidth);

                  status_ref_vs_nat = TI_compare_mem_2DDualStride_float(
                      (void *) pOutCn, (void *) currPrm.staticOut, 0.001, (double) powf(2, -10), bufParamsOut.dim_x,
                      bufParamsOut.dim_y, bufParamsOut.stride_y, bufParamsOut.dim_x * dataWidth, dataWidth);
               }
               else {
                  status_ref_vs_opt = TI_compare_mem_2DDualStride(
                      (void *) pOut, (void *) currPrm.staticOut, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y,
                      bufParamsOut.stride_y, bufParamsOut.dim_x * dataWidth, dataWidth);

                  status_ref_vs_nat = TI_compare_mem_2DDualStride(
                      (void *) pOutCn, (void *) currPrm.staticOut, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y,
                      bufParamsOut.stride_y, bufParamsOut.dim_x * dataWidth, dataWidth);
               }

               comparisonDone = 1;
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
               status_ref_vs_nat = TI_TEST_KERNEL_PASS;
            }

#if DSPLIB_DEBUGPRINT
            printf(
                "DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 8 status_nat_vs_opt %d status_ref_vs_opt %d currentTestFail %d\n",
                status_nat_vs_opt, status_ref_vs_opt, currentTestFail);
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 8 status_init %d status_opt %d status_nat %d\n", status_init,
                   status_opt, status_nat);
#endif
            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_nat == TI_TEST_KERNEL_FAIL) || (status_init != DSPLIB_SUCCESS) ||
                 (status_opt != DSPLIB_SUCCESS) || (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0) ||
                 (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_recip_d CP 8 fail %d\n", fail);
#endif

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | width = %d, height = %d, Data Type %d", testPatternString,
                    currPrm.width, currPrm.height, currPrm.dataType);

            // write to CSV, must happen prior to write to screen because TI_profile_forsqra_recip clears values in
            // counters
            fprintf(fpOutputCSV, "recip, %d, %d, %d, %d, %d, %d, %d, %lu, %lu, %lu\n", testNum,
                    DSPLIB_sizeof(bufParamsIn.data_type) * 8, currPrm.width, currPrm.height, currPrm.strideIn,
                    currPrm.strideOut, currPrm.storeStyle, archCycles, estCycles, cycles[TI_PROFILE_KERNEL_OPT_WARM]);

            TI_profile_add_test(testNum++, currPrm.width * currPrm.height, archCycles, estCycles, currentTestFail,
                                desc);
         }
         else {
            sprintf(desc, "width=%d, height=%d", currPrm.width, currPrm.height);
            TI_profile_skip_test(desc);
            // clear the counters between runs; normally handled by TI_profile_recip_test
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
         TI_align_free(pIn);

      } // end repetitions
   }    // end idat test cases

   free(handle);

#if (!(EVM_TEST))
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_recip");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_recip_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t               testNum         = 1000;
   int32_t               currentTestFail = 0;
   DSPLIB_STATUS         status_nat;
   DSPLIB_STATUS         status_opt;
   DSPLIB_bufParams2D_t  bufParamsIn, bufParamsOut;
   DSPLIB_recip_InitArgs kerInitArgs;
   int32_t               handleSize = DSPLIB_recip_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle   handle     = malloc(handleSize);
   int                   fail       = 0;
   void                 *pIn        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                 *pOut       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);

   bufParamsIn.dim_x     = 16;
   bufParamsIn.dim_y     = 1;
   bufParamsIn.stride_y  = 16 * sizeof(int16_t);
   bufParamsOut.dim_x    = 16;
   bufParamsOut.dim_y    = 1;
   bufParamsOut.stride_y = 32 * sizeof(int16_t);
   while (testNum <= 1013) {

      switch (testNum) {
      case 1000:
         bufParamsIn.data_type  = DSPLIB_INT32;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(NULL, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         bufParamsIn.data_type  = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsIn.data_type  = DSPLIB_FLOAT32;
         bufParamsOut.data_type = DSPLIB_INT16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         status_nat      = DSPLIB_recip_exec_checkParams(handle, NULL, pOut);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1004:
         status_nat      = DSPLIB_recip_exec_checkParams(handle, pIn, NULL);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1005:
         bufParamsIn.data_type  = DSPLIB_INT128;
         bufParamsOut.data_type = DSPLIB_INT32;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1006:
         bufParamsIn.data_type  = DSPLIB_INT16;
         bufParamsOut.data_type = DSPLIB_INT16;
         bufParamsIn.dim_x      = 16;
         bufParamsOut.dim_x     = 16 * 2;
         kerInitArgs.storeStyle = DSPLIB_LINEAR_ST;
         kerInitArgs.dataSize   = 16;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         status_nat            = DSPLIB_recip_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         status_nat            = DSPLIB_recip_exec_checkParams(handle, pIn, pOut);
         status_nat            = DSPLIB_recip_exec(handle, pIn, pOut);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         status_opt            = DSPLIB_recip_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         status_opt            = DSPLIB_recip_exec_checkParams(handle, pIn, pOut);
         status_opt            = DSPLIB_recip_exec(handle, pIn, pOut);

         currentTestFail = ((status_nat != DSPLIB_ERR_NOT_IMPLEMENTED) || (status_opt != DSPLIB_ERR_NOT_IMPLEMENTED));
         break;
      case 1007:
         bufParamsIn.data_type  = DSPLIB_INT16;
         bufParamsOut.data_type = DSPLIB_INT16;
         bufParamsIn.dim_x      = 0;
         bufParamsIn.dim_y      = 1;
         bufParamsIn.stride_y   = 16 * sizeof(int16_t);
         bufParamsOut.dim_x     = 16 * 2;
         bufParamsOut.dim_y     = 1;
         bufParamsOut.stride_y  = 32 * sizeof(int16_t);
         kerInitArgs.storeStyle = DSPLIB_INTERLEAVE_ST;
         kerInitArgs.dataSize   = 16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1008:
         bufParamsIn.data_type  = DSPLIB_INT16;
         bufParamsOut.data_type = DSPLIB_INT16;
         bufParamsIn.dim_x      = 16;
         bufParamsIn.dim_y      = 0;
         bufParamsIn.stride_y   = 16 * sizeof(int16_t);
         bufParamsOut.dim_x     = 16 * 2;
         bufParamsOut.dim_y     = 1;
         bufParamsOut.stride_y  = 32 * sizeof(int16_t);
         kerInitArgs.storeStyle = DSPLIB_INTERLEAVE_ST;
         kerInitArgs.dataSize   = 16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1009:
         bufParamsIn.data_type  = DSPLIB_INT16;
         bufParamsOut.data_type = DSPLIB_INT16;
         bufParamsIn.dim_x      = 16;
         bufParamsIn.dim_y      = 1;
         bufParamsIn.stride_y   = 16 * sizeof(int16_t);
         bufParamsOut.dim_x     = 16 * 2;
         bufParamsOut.dim_y     = 1;
         bufParamsOut.stride_y  = 32 * sizeof(int16_t);
         kerInitArgs.storeStyle = DSPLIB_INTERLEAVE_ST;
         kerInitArgs.dataSize   = 8;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1010:
         bufParamsIn.data_type  = DSPLIB_INT16;
         bufParamsOut.data_type = DSPLIB_INT16;
         bufParamsIn.dim_x      = 16;
         bufParamsIn.dim_y      = 1;
         bufParamsIn.stride_y   = 8 * sizeof(int16_t);
         bufParamsOut.dim_x     = 16 * 2;
         bufParamsOut.dim_y     = 1;
         bufParamsOut.stride_y  = 32 * sizeof(int16_t);
         kerInitArgs.storeStyle = DSPLIB_INTERLEAVE_ST;
         kerInitArgs.dataSize   = 16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1011:
         bufParamsIn.data_type  = DSPLIB_INT16;
         bufParamsOut.data_type = DSPLIB_INT16;
         bufParamsIn.dim_x      = 16;
         bufParamsIn.dim_y      = 1;
         bufParamsIn.stride_y   = 16 * sizeof(int16_t);
         bufParamsOut.dim_x     = 16 * 2;
         bufParamsOut.dim_y     = 1;
         bufParamsOut.stride_y  = 16 * sizeof(int16_t);
         kerInitArgs.storeStyle = DSPLIB_INTERLEAVE_ST;
         kerInitArgs.dataSize   = 16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1012:
         bufParamsIn.data_type  = DSPLIB_INT16;
         bufParamsOut.data_type = DSPLIB_INT16;
         bufParamsIn.dim_x      = 16;
         bufParamsIn.dim_y      = 1;
         bufParamsIn.stride_y   = 16 * sizeof(int16_t);
         bufParamsOut.dim_x     = 16 * 2;
         bufParamsOut.dim_y     = 2;
         bufParamsOut.stride_y  = 32 * sizeof(int16_t);
         kerInitArgs.storeStyle = DSPLIB_INTERLEAVE_ST;
         kerInitArgs.dataSize   = 16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1013:
         bufParamsIn.data_type  = DSPLIB_INT16;
         bufParamsOut.data_type = DSPLIB_INT16;
         bufParamsIn.dim_x      = 16;
         bufParamsIn.dim_y      = 1;
         bufParamsIn.stride_y   = 16 * sizeof(int16_t);
         bufParamsOut.dim_x     = 16;
         bufParamsOut.dim_y     = 1;
         bufParamsOut.stride_y  = 32 * sizeof(int16_t);
         kerInitArgs.storeStyle = DSPLIB_INTERLEAVE_ST;
         kerInitArgs.dataSize   = 16;
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
         status_nat             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt             = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_recip_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_recip_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_recip_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   TI_align_free(pIn);
   TI_align_free(pOut);
   free(handle);

   return fail;
}

/* Main call for insqridual test projects */
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
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");
   return fail;
}
#endif
