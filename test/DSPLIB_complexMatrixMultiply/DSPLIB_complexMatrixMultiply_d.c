// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "../common/DSPLIB_test.h"
#include "DSPLIB_bufParams.h"
#include "DSPLIB_complexMatrixMultiply.h"
#include "DSPLIB_complexMatrixMultiply_idat.h"
#include "DSPLIB_types.h"
#include "TI_memory.h"
#include "TI_test.h"
#include <dsplib.h>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Global variables                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/

#ifdef WIN32
#if defined(__C7524__) || defined(__C7504__)
int8_t l2auxBuffer[DSPLIB_L2_BUFFER_SIZE];
int8_t ddrBuffer[512 * 256 * 16];
#else
int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
int8_t ddrBuffer[512 * 256 * 16];
#endif
#else
#if defined(__C7524__) || defined(__C7504__)
__attribute__((section(".l2sramaux"), aligned(64))) int8_t l2auxBuffer[DSPLIB_L2_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t   ddrBuffer[512 * 256 * 16];

#else
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[512 * 256 * 16];

#endif
#endif // WIN32

int16_t volatile volatileSum0 = 0; // use volatile to keep compiler from removing this operation

char test_name[] = "DSPLIB_complexMatrixMultiply";

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DSPLIB_complexMatrixMultiply_d */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs regerssion testing for the DSPLIB_complexMatrixMultiply kernel
int DSPLIB_complexMatrixMultiply_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi;                 // iteration variable for test input
   int32_t  currentTestFail = 0; // status of current test case
   uint32_t repCount;            // iteration variable for repetition count
   uint32_t testNum;             // test number

   int32_t fail = 0;

   // input and output buffer parameter variables
   DSPLIB_bufParams3D_t bufParamsInA;
   DSPLIB_bufParams3D_t bufParamsInB;
   DSPLIB_bufParams3D_t bufParamsOut;

   // data structure to hold test cases parameters
   DSPLIB_complexMatrixMultiply_testParams_t *prm;
   DSPLIB_complexMatrixMultiply_testParams_t  currPrm;

   // get total number of test cases
   DSPLIB_complexMatrixMultiply_getTestParams(&prm, &test_cases);

   // initialization args for kernel
   DSPLIB_complexMatrixMultiply_InitArgs kerInitArgs;

   // create kernel's handle
   int32_t             handleSize = DSPLIB_complexMatrixMultiply_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   TI_profile_init("DSPLIB_complexMatrixMultiply");

   // file IO for benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_complexMatrixMultiply.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, dTypeIn, NUmber of batch, M, N, K, Loki cycles, pass/fail\n");
   // iterate through test cases
   for (tpi = 0; tpi < test_cases; tpi++) {
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < prm[tpi].numReps; repCount++) {
         currentTestFail       = 0;
         DSPLIB_STATUS ret_val = DSPLIB_SUCCESS;

         /*****************************************/
         /* Assign input output buffer parameters */
         /*****************************************/
         bufParamsInA.data_type = currPrm.dType;
         bufParamsInB.data_type = currPrm.dType;
         bufParamsOut.data_type = currPrm.dType;

         bufParamsInA.dim_x    = currPrm.K;
         bufParamsInA.dim_y    = currPrm.M;
         bufParamsInA.dim_z    = currPrm.nBatch;
         bufParamsInA.stride_y = bufParamsInA.dim_x * DSPLIB_sizeof(currPrm.dType) * 2; // complex
         bufParamsInA.stride_z = bufParamsInA.dim_y * bufParamsInA.stride_y;

         bufParamsInB.dim_x    = currPrm.N;
         bufParamsInB.dim_y    = currPrm.K;
         bufParamsInB.dim_z    = currPrm.nBatch;
         bufParamsInB.stride_y = bufParamsInB.dim_x * DSPLIB_sizeof(currPrm.dType) * 2; // complex
         bufParamsInB.stride_z = bufParamsInB.dim_y * bufParamsInB.stride_y;

         bufParamsOut.dim_x    = currPrm.N;
         bufParamsOut.dim_y    = currPrm.M;
         bufParamsOut.dim_z    = currPrm.nBatch;
         bufParamsOut.stride_y = bufParamsOut.dim_x * DSPLIB_sizeof(currPrm.dType) * 2; // complex
         bufParamsOut.stride_z = bufParamsOut.dim_y * bufParamsOut.stride_y;

         size_t inpSizeA    = bufParamsInA.dim_z * bufParamsInA.stride_z;
         size_t inpSizeB    = bufParamsInB.dim_z * bufParamsInB.stride_z;
         size_t outSize     = bufParamsOut.dim_z * bufParamsOut.stride_z;
         kerInitArgs.params = *(DSPLIB_complexMatrixMultiply_Params *) currPrm.params;

         /*****************************************/
         /* Create buffers for input and outputs  */
         /*****************************************/

         // input
         void *pInA = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, inpSizeA);
         void *pInB = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, inpSizeB);

         // output
         void *pOut     = NULL;
         void *pOutCn   = NULL;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pOut = TI_memalign(DSPLIB_L2DATA_ALIGNMENT, outSize);
            pOutCn = malloc(outSize);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7524__) || defined(__C7504__)
            pOut = TI_memalign(DSPLIB_L2DATA_ALIGNMENT, outSize);
#else
            pOut = msmcBuffer;
#endif
            pOutCn = ddrBuffer;
         }

         // only run the test if the buffer allocations fit in the heap
         if (!pInA || !pInB || !pOut || !pOutCn) {
            printf("ALLOCATION FAILED: pInA=%s pInB=%s pOut=%s pOutCn=%s\n", pInA ? "ok" : "NULL",
                   pInB ? "ok" : "NULL", pOut ? "ok" : "NULL", pOutCn ? "ok" : "NULL");
         }
         if (pInA && pInB && pOut && pOutCn) {
            // STB: If too big - generate your own test data inside of test driver
            // Set test type to "DSPLIB_TEST_OUTPUT_HEAP" so the CN is allocated on the heap
            // this should fix all of your problems
            TI_fillBuffer3D(prm[tpi].testPattern, 0, pInA, prm[tpi].staticInA, bufParamsInA.dim_x * 2,
                            bufParamsInA.dim_y, bufParamsInA.dim_z, bufParamsInA.stride_y, bufParamsInA.stride_z,
                            DSPLIB_sizeof(currPrm.dType), testPatternString);
            TI_fillBuffer3D(prm[tpi].testPattern, 0, pInB, prm[tpi].staticInB, bufParamsInB.dim_x * 2,
                            bufParamsInB.dim_y, bufParamsInB.dim_z, bufParamsInB.stride_y, bufParamsInB.stride_z,
                            DSPLIB_sizeof(currPrm.dType), testPatternString);
            TI_fillBuffer3D(CONSTANT, 0, pOut, prm[tpi].staticInB, bufParamsOut.dim_x, bufParamsOut.dim_y,
                            bufParamsOut.dim_z, bufParamsOut.stride_y, bufParamsOut.stride_z,
                            DSPLIB_sizeof(currPrm.dType), testPatternString);
            // check parameters for initializing the kernel and initialize kernel if successful
            ret_val = DSPLIB_complexMatrixMultiply_init_checkParams(handle, &bufParamsInA, &bufParamsInB, &bufParamsOut,
                                                                    &kerInitArgs);
            if (ret_val != DSPLIB_SUCCESS) {
               printf("DSPLIB_complexMatrixMultiply_init_checkParams failed: %d\n", ret_val);
            }
            currentTestFail += ret_val;
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            if (currentTestFail == 0) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               ret_val = DSPLIB_complexMatrixMultiply_init(handle, &bufParamsInA, &bufParamsInB, &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();

               if (ret_val != DSPLIB_SUCCESS) {
                  printf("DSPLIB_complexMatrixMultiply_init failed: %d\n", ret_val);
               }
               currentTestFail += ret_val;
            }
            // check execution parameters and call optimized kernel if sucessful
            ret_val = DSPLIB_complexMatrixMultiply_exec_checkParams(handle, pInA, pInB, pOut);
            if (ret_val != DSPLIB_SUCCESS) {
               printf("_exec_checkParams( failed: %d", ret_val);
            }
            currentTestFail += ret_val;
            if (currentTestFail == 0) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               ret_val = DSPLIB_complexMatrixMultiply_exec(handle, pInA, pInB, pOut);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
               currentTestFail += ret_val;
            }

            uint32_t k = 0;
            // the following for loop is to call kernel repeatedly so as to train the branch predictor
            for (k = 0; k < (DSPLIB_WARM_REPS); k++) {
               if (currentTestFail == 0) {
                  // run warm instruction cache test
                  TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
                  DSPLIB_asm(" MARK 4");
                  ret_val = DSPLIB_complexMatrixMultiply_exec(handle, pInA, pInB, pOut);
                  DSPLIB_asm(" MARK 5");
                  TI_profile_stop();
                  currentTestFail += ret_val;
               }
            }
            // get output to L1D
            int16_t outSum0   = 0;
            int8_t *pOutTemp0 = (int8_t *) pOut;
            for (k = 0; k < 1; k++) {
               outSum0 += *pOutTemp0;
               pOutTemp0++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum0 = outSum0;

            if (currentTestFail == 0) {
               // run warm instruction cache test
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);
               DSPLIB_asm(" MARK 6");
               ret_val = DSPLIB_complexMatrixMultiply_exec(handle, pInA, pInB, pOut);
               DSPLIB_asm(" MARK 7");
               TI_profile_stop();
               currentTestFail += ret_val;
            }
            /**************************************/
            /* Test natural C code for the kernel */
            /**************************************/

            // initialize the kernel function style to natural C
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;

            if (currentTestFail == 0) {
               // call init once again to use the natural C variant
               ret_val =
                   DSPLIB_complexMatrixMultiply_init(handle, &bufParamsInA, &bufParamsInB, &bufParamsOut, &kerInitArgs);
               currentTestFail += ret_val;
            }
            if (currentTestFail == 0) {
               TI_profile_start(TI_PROFILE_KERNEL_CN);
               DSPLIB_asm(" MARK 8");
               ret_val = DSPLIB_complexMatrixMultiply_exec(handle, pInA, pInB, pOutCn);
               // int16_t *temp = (int16_t *)currPrm.staticOut;
               DSPLIB_asm(" MARK 9");
               TI_profile_stop();
               currentTestFail += ret_val;
            }
            /*********************************************************************/
            /* Compare results with natural C output and static reference output */
            /*********************************************************************/
            int32_t comp_res  = 0;
            int64_t tolerance = 5;
            if (currentTestFail == 0) {
               comp_res = TI_compare_mem_3D((void *) pOut, (void *) pOutCn, tolerance, 1, bufParamsOut.dim_x * 2,
                                            bufParamsOut.dim_y, bufParamsOut.dim_z, bufParamsOut.stride_y,
                                            bufParamsOut.stride_z, DSPLIB_sizeof(currPrm.dType));
               // comp_res = TI_TEST_KERNEL_PASS;
               currentTestFail += (comp_res != TI_TEST_KERNEL_PASS);
               if (currentTestFail) {
                  printf("Failed comparing optimized with CN\n");
                  uint32_t m, n;
                  for (m = 0; m < currPrm.M; m++) {
                     for (n = 0; n < 2 * currPrm.N; n++) {
                        printf("%d ", ((int16_t *) pOut)[m * 2 * currPrm.N + n]);
                     }
                     printf("\n");
                  }
               }
               if (currPrm.staticOut != NULL) {
                  comp_res =
                      TI_compare_mem_3D((void *) pOutCn, (void *) currPrm.staticOut, tolerance, 1,
                                        bufParamsOut.dim_x * 2, bufParamsOut.dim_y, bufParamsOut.dim_z,
                                        bufParamsOut.stride_y, bufParamsOut.stride_z, DSPLIB_sizeof(currPrm.dType));
                  currentTestFail += (comp_res != TI_TEST_KERNEL_PASS);

                  if (currentTestFail)
                     printf("Failed comparing optimized with ref\n");
               }
            }

            // update profiling information
            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            uint8_t data_bits = (currPrm.dType == DSPLIB_INT16) ? 16 : 32;
            sprintf(desc, "%s generated | batch = %ld, M = %ld, K=%ld, N=%ld, %db", testPatternString, currPrm.nBatch,
                    currPrm.M, currPrm.K, currPrm.N, data_bits);

            // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            fprintf(fpOutputCSV, "complexMatrixMultiply, %d, %d, %ld, %ld, %ld, %ld, %lu, %s\n", testNum,
                    DSPLIB_sizeof(bufParamsInA.data_type) * 8, currPrm.nBatch, currPrm.M, currPrm.K, currPrm.N,
                    cycles[TI_PROFILE_KERNEL_OPT_WARM], currentTestFail == 0 ? "pass" : "fail");

            uint64_t archCycles = 0, estCycles = 0;
            DSPLIB_complexMatrixMultiply_perfEst(handle, &bufParamsInA, &bufParamsInB, &archCycles, &estCycles);

            size_t total_samples = currPrm.nBatch * currPrm.M * currPrm.K * currPrm.N;
            TI_profile_add_test(testNum++, total_samples, archCycles, estCycles, currentTestFail, desc);
         }
         else {
            sprintf(desc, "widthA =%ld, heightA = %ld, widthB = %ld, heightB = %ld", currPrm.K, currPrm.M, currPrm.N,
                    currPrm.K);
            TI_profile_skip_test(desc);
         } // end of memory allocation successful?

         /*************************************/
         /* Free buffers for each test vector */
         /*************************************/
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7524__) || defined(__C7504__)
            TI_align_free(pOut);
#endif
         }

         TI_align_free(pInA);
         TI_align_free(pInB);

         fail += currentTestFail;
      } // end repetitions
   } // end idat test cases

   free(handle);
   TI_profile_clear_run_stats();
   /* Close results CSV */
   fclose(fpOutputCSV);

   return currentTestFail;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* test_main                                                                                                          */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calls the testbench function
int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_complexMatrixMultiply");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_complexMatrixMultiply_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t              testNum         = 1000;
   int32_t              currentTestFail = 0;
   DSPLIB_STATUS        status_nat;
   DSPLIB_bufParams3D_t bufParamsA, bufParamsB;
   DSPLIB_bufParams3D_t bufParamsC;

   DSPLIB_complexMatrixMultiply_InitArgs kerInitArgs;
   int32_t                               handleSize = DSPLIB_complexMatrixMultiply_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle                   handle     = malloc(handleSize);

   int fail = 0;

   int16_t            DIMX   = 16;
   int16_t            DIMY   = 16;
   int16_t            NBATCH = 1;
   DSPLIB_data_type_e dType  = DSPLIB_INT16;

   bufParamsA.dim_x     = DIMX;
   bufParamsA.dim_y     = DIMY;
   bufParamsA.dim_z     = NBATCH;
   bufParamsA.stride_y  = DSPLIB_sizeof(DSPLIB_INT16) * bufParamsA.dim_x * 2; // complex
   bufParamsA.stride_z  = bufParamsA.stride_y * bufParamsA.dim_y;
   bufParamsA.data_type = dType;

   bufParamsB = bufParamsC = bufParamsA;

   size_t dataSize               = bufParamsA.stride_z * 1;
   kerInitArgs.funcStyle         = DSPLIB_FUNCTION_NATC;
   kerInitArgs.params.scaleValue = 1;
   kerInitArgs.params.shift      = 0;
   /*****************************************/
   /* Create buffers for input and outputs  */
   /*****************************************/

   // input
   void *pIn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, dataSize);
   // output
   void *pOut = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, dataSize);

   while (testNum < 1007) {
      switch (testNum) {
         /* Init Check Params  Coverage */
      case 1000:
         /* Null pointer*/
         status_nat =
             DSPLIB_complexMatrixMultiply_init_checkParams(NULL, &bufParamsA, &bufParamsB, &bufParamsC, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         bufParamsC.data_type = DSPLIB_INT32;
         status_nat =
             DSPLIB_complexMatrixMultiply_init_checkParams(handle, &bufParamsA, &bufParamsB, &bufParamsC, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE));

         bufParamsC.data_type = dType;
         break;
      case 1002:
         bufParamsA.data_type = bufParamsB.data_type = bufParamsC.data_type = DSPLIB_FLOAT32;
         status_nat =
             DSPLIB_complexMatrixMultiply_init_checkParams(handle, &bufParamsA, &bufParamsB, &bufParamsC, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE));

         bufParamsA.data_type = bufParamsB.data_type = bufParamsC.data_type = dType;
         break;
      case 1003:
         bufParamsA.dim_x = 8;
         status_nat =
             DSPLIB_complexMatrixMultiply_init_checkParams(handle, &bufParamsA, &bufParamsB, &bufParamsC, &kerInitArgs);
         currentTestFail  = ((status_nat != DSPLIB_ERR_INVALID_DIMENSION));
         bufParamsA.dim_x = DIMX;
         break;
      case 1004:
         bufParamsA.dim_z = 2;
         status_nat =
             DSPLIB_complexMatrixMultiply_init_checkParams(handle, &bufParamsA, &bufParamsB, &bufParamsC, &kerInitArgs);
         currentTestFail  = ((status_nat != DSPLIB_ERR_INVALID_DIMENSION));
         bufParamsA.dim_z = NBATCH;
         break;
      case 1005:
         bufParamsA.dim_x    = 16;
         bufParamsA.stride_y = 16;
         status_nat =
             DSPLIB_complexMatrixMultiply_init_checkParams(handle, &bufParamsA, &bufParamsB, &bufParamsC, &kerInitArgs);
         currentTestFail     = ((status_nat != DSPLIB_ERR_NOT_ALIGNED_PTRS_STRIDES));
         bufParamsA.stride_y = DSPLIB_sizeof(bufParamsA.data_type) * bufParamsA.dim_x * 2;
         break;
      case 1006:
         bufParamsA.dim_x     = DIMX;
         bufParamsA.data_type = DSPLIB_INT32;
         bufParamsA.stride_y  = DSPLIB_sizeof(bufParamsA.data_type) * bufParamsA.dim_x * 2;
         bufParamsB = bufParamsC       = bufParamsA;
         kerInitArgs.params.scaleValue = 4;
         status_nat =
             DSPLIB_complexMatrixMultiply_init_checkParams(handle, &bufParamsA, &bufParamsB, &bufParamsC, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_NOT_IMPLEMENTED));
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

   return fail;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* main                                                                                                               */
/*                                                                                                                    */
/**********************************************************************************************************************/

#if !defined(__ONESHOTTEST)
int main()
{
   int      fail = 1;
   uint32_t profile[256 * 3];

   DSPLIB_TEST_init();

   fail = test_main(&profile[0]);

#if !defined(NO_PRINTF)
   if (fail == 0) {
      printf("[DSPLIB_complexMatrixMultiply] Main Test Pass!\n");
   }
   else {
      printf("[DSPLIB_complexMatrixMultiply] Main Test Fail!\n");
      return fail;
   }
#ifdef DSPLIB_BENCHMARK
   int i;
   for (i = 0; i < test_cases; i++) {
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", i, profile[3 * i],
             profile[3 * i + 1] / (DSPLIB_WARM_REPS), profile[3 * i + 2]);
   }
#endif
#endif

   fail = coverage_test_main();
   if (fail == 0)
      printf("[DSPLIB_complexMatrixMultiply] Coverage Test Pass!\n");
   else
      printf("[DSPLIB_complexMatrixMultiply] Coverage Test Fail!\n");
   return fail;
}
#endif
