// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_qrd_check.h"
#include "DSPLIB_qrd_idat.h"
char print_txt[16];

// Additional rows to avoid valgrind warnings
#define DSPLIB_ROW_STRIDE(x, y) (x + y)

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

int DSPLIB_qrd_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsA, bufParamsQ, bufParamsR, bufParamsInvA, bufParamsInvAFinal;
   DSPLIB_bufParams1D_t bufParamsU, bufParamsX, bufParamsB;

#if !defined(EVM_TEST)
   uint64_t lokiCycles;
#endif // #if !defined(EVM_TEST)
   uint32_t testNum;

   DSPLIB_qrd_testParams_t *prm;
   DSPLIB_qrd_testParams_t  currPrm;
   DSPLIB_qrd_getTestParams(&prm, &test_cases);

   DSPLIB_qrdInitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_qrd_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_qrdSolverInitArgs kerInitArgsSolv;

   int32_t             handleSolvSize = DSPLIB_qrd_solver_getHandleSize(&kerInitArgsSolv);
   DSPLIB_kernelHandle handleSolv     = malloc(handleSolvSize);

   DSPLIB_qrdInvInitArgs kerInitArgsInv;
   int32_t               handleInvSize = DSPLIB_qrd_inverse_getHandleSize(&kerInitArgsInv);
   DSPLIB_kernelHandle   handleInv     = malloc(handleInvSize);

   TI_profile_init("DSPLIB_qrd");
#if (!defined(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_qrd.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Data size, Loki cycles, Pass/Fail\n");
#endif // #if (!defined(EVM_TEST))

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         DSPLIB_DEBUGPRINTFN(0, "Current TestID: %d Current Repetition: %d\n", currPrm.testID, repCount + 1);

         DSPLIB_STATUS status_init = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt  = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat  = DSPLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;

         currentTestFail = 0;

         bufParamsA.data_type = currPrm.dataType;
         bufParamsA.dim_x     = currPrm.widthIn;
         bufParamsA.dim_y     = currPrm.heightIn;
         bufParamsA.stride_y  = currPrm.strideIn;
         if (bufParamsA.stride_y % 512 == 0) {
            bufParamsA.stride_y += 128;
         }

         int32_t nRows  = bufParamsA.dim_y;
         int32_t nCols  = bufParamsA.dim_x;
         int32_t maxDim = (nRows >= nCols) ? nRows : nCols;

         bufParamsR.data_type = currPrm.dataType;
         bufParamsR.dim_x     = currPrm.widthIn;
         bufParamsR.dim_y     = currPrm.heightIn;
         bufParamsR.stride_y  = currPrm.strideIn;
         if (bufParamsR.stride_y % 512 == 0) {
            bufParamsR.stride_y += 128;
         }
         bufParamsQ.data_type = currPrm.dataType;
         bufParamsQ.dim_x     = currPrm.heightIn;
         bufParamsQ.dim_y     = currPrm.heightIn;
         bufParamsQ.stride_y =
             DSPLIB_CALC_STRIDE(bufParamsQ.dim_x * DSPLIB_sizeof(bufParamsQ.data_type), DSPLIB_ALIGN_SHIFT_64BYTES);
         if (bufParamsQ.stride_y % 512 == 0) {
            bufParamsQ.stride_y += 128;
         }

         bufParamsInvA.data_type = currPrm.dataType;
         bufParamsInvA.dim_x     = currPrm.widthIn;
         bufParamsInvA.dim_y     = currPrm.heightIn;
         bufParamsInvA.stride_y  = currPrm.strideIn;
         if (bufParamsInvA.stride_y % 512 == 0) {
            bufParamsInvA.stride_y += 128;
         }

         bufParamsInvAFinal.data_type = currPrm.dataType;
         bufParamsInvAFinal.dim_x     = currPrm.widthIn;
         bufParamsInvAFinal.dim_y     = currPrm.heightIn;
         bufParamsInvAFinal.stride_y  = DSPLIB_CALC_STRIDE(
              bufParamsInvAFinal.dim_x * DSPLIB_sizeof(bufParamsInvAFinal.data_type), DSPLIB_ALIGN_SHIFT_128BYTES);
         if (bufParamsInvAFinal.stride_y % 512 == 0) {
            bufParamsInvAFinal.stride_y += 128;
         }

         bufParamsU.data_type = currPrm.dataType;
         bufParamsU.dim_x     = maxDim;

         bufParamsB.data_type = currPrm.dataType;
         bufParamsB.dim_x     = currPrm.heightIn;

         bufParamsX.data_type = currPrm.dataType;
         bufParamsX.dim_x     = currPrm.widthIn;

         int32_t  eleCount = 2 * __C7X_VEC_SIZE_BYTES__ / DSPLIB_sizeof(currPrm.dataType);
         uint64_t aSize    = bufParamsA.stride_y * DSPLIB_ROW_STRIDE(bufParamsA.dim_y, eleCount);
         uint64_t qSize    = bufParamsQ.stride_y * DSPLIB_ROW_STRIDE(bufParamsQ.dim_y, eleCount);
         uint64_t rSize    = bufParamsR.stride_y * DSPLIB_ROW_STRIDE(bufParamsR.dim_y, eleCount);
         uint64_t invASize = bufParamsInvA.stride_y * DSPLIB_ROW_STRIDE(bufParamsInvA.dim_y, eleCount);
         uint64_t uSize =
             DSPLIB_CALC_STRIDE(bufParamsU.dim_x * DSPLIB_sizeof(bufParamsU.data_type), DSPLIB_ALIGN_SHIFT_64BYTES);
         uint64_t bSize =
             DSPLIB_CALC_STRIDE(bufParamsB.dim_x * DSPLIB_sizeof(bufParamsB.data_type), DSPLIB_ALIGN_SHIFT_64BYTES);
         uint64_t xSize =
             DSPLIB_CALC_STRIDE(bufParamsX.dim_x * DSPLIB_sizeof(bufParamsX.data_type), DSPLIB_ALIGN_SHIFT_64BYTES);

         DSPLIB_DEBUGPRINTFN(0,
                             "aSize: %d qSize: %d rSize: %d invASize: %d uSize: %d "
                             "bSize: %d xSize: %d\n",
                             aSize, qSize, rSize, invASize, uSize, bSize, xSize);

         DSPLIB_DEBUGPRINTFN(0, "I/P data_type: %d O/P data_type: %d\n", bufParamsA.data_type, bufParamsA.data_type);
         DSPLIB_DEBUGPRINTFN(0, "widthIn: %d heightIn: %d strideIn: %d\n", bufParamsA.dim_x, bufParamsA.dim_y,
                             bufParamsA.stride_y);
         DSPLIB_DEBUGPRINTFN(0, "widthQ: %d heightQ: %d strideQ: %d\n", bufParamsQ.dim_x, bufParamsQ.dim_y,
                             bufParamsQ.stride_y);
         DSPLIB_DEBUGPRINTFN(0, "widthR: %d heightR: %d strideR: %d\n", bufParamsR.dim_x, bufParamsR.dim_y,
                             bufParamsR.stride_y);

         void *pA = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, aSize);
         void *pB = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bSize);

         void *pQ, *pR, *pInvA, *pU, *pX, *pScratch, *pInvAScratch;
         void *pQCn, *pRCn, *pInvACn, *pUCn, *pXCn, *pScratchCn;

         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pQ           = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, qSize);
            pR           = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, rSize);
            pInvA        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, invASize);
            pU           = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            pX           = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            pScratch     = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            pInvAScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, invASize);

            pQCn       = (void *) malloc(qSize);
            pRCn       = (void *) malloc(rSize);
            pInvACn    = (void *) malloc(invASize);
            pUCn       = (void *) malloc(uSize);
            pXCn       = (void *) malloc(xSize);
            pScratchCn = (void *) malloc(uSize);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
            int32_t usedBytes = 0;
#if defined(__C7504__) || defined(__C7524__)
            usedBytes = aSize + bSize;
            pQ        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, qSize);
            usedBytes += qSize;
            pR = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, rSize);
            usedBytes += rSize;
            pInvA = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, invASize);
            usedBytes += invASize;
            pU = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            usedBytes += uSize;
            pX = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            usedBytes += xSize;
            pScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            usedBytes += uSize;
            pInvAScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, invASize);
            usedBytes += invASize;
#else
            usedBytes = aSize + bSize;
            pQ        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, qSize);
            usedBytes += qSize;
            pR = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, rSize);
            usedBytes += rSize;
            pU = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            usedBytes += uSize;
            pX = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            usedBytes += xSize;
            pScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            usedBytes += uSize;
            pInvAScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, invASize);
            usedBytes += invASize;
            pInvA = msmcBuffer;
            usedBytes += invASize;
#endif
            usedBytes = 0;

            pQCn = (void *) ddrBuffer;
            usedBytes += qSize;

            pRCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += rSize;

            pInvACn = (void *) (ddrBuffer + usedBytes);
            usedBytes += invASize;

            pUCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += uSize;

            pXCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += uSize;

            pScratchCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += uSize;
         }
         if (pA && pB && pQ && pR && pU && pX && pInvA && pInvAScratch && pScratch) {
            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pA, prm[tpi].staticInA, bufParamsA.dim_x, bufParamsA.dim_y,
                                   bufParamsA.stride_y, DSPLIB_sizeof(currPrm.dataType), testPatternString);
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pB, prm[tpi].staticInB, bufParamsB.dim_x, 1, 0,
                                   DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }

            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pA, prm[tpi].staticInA, bufParamsA.dim_x,
                             bufParamsA.dim_y, bufParamsA.stride_y, DSPLIB_sizeof(currPrm.dataType), testPatternString);
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pB, prm[tpi].staticInB, bufParamsA.dim_x, 0, 0,
                             DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }
            /****************************************************************************************************
             *                               QRD
             ****************************************************************************************************/
            /* TEST OPTIMIZED KERNEL */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init =
                DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               status_init = DSPLIB_qrd_init(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
               if (status_init == DSPLIB_SUCCESS) {
                  status_opt = DSPLIB_qrd_exec_checkParams(handle, pA, pQ, pR, pU, pScratch);
               }
               else {
                  status_opt = status_init;
               }
            }
            else {
               status_opt = status_init;
            }

            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_qrd_exec(handle, pA, pQ, pR, pU, pScratch);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();

#if !defined(__C7X_HOSTEM__)
               uint32_t k;
               /* The following for loop is to call kernel repeatedly so as to
                * train the branch predictor                                   */
               for (k = 0; k < 4; k++) {
                  // run warm instruction cache test
                  TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
                  DSPLIB_asm(" MARK 4");
                  status_opt = DSPLIB_qrd_exec(handle, pA, pQ, pR, pU, pScratchCn);
                  DSPLIB_asm(" MARK 5");
                  TI_profile_stop();
               }

               // get output to L1D
               int16_t outSum   = 0;
               int8_t *pOutTemp = (int8_t *) pQ; // treat output as bytes to be data type agnostic
               for (k = 0; k < bufParamsQ.dim_x; k++) {
                  outSum += *pOutTemp;
                  pOutTemp++;
               }
               // dummy store of outSum to insure that the compiler does not remove
               // it.
               volatileSum = outSum;

               // run warm instruction cache test
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);
               DSPLIB_asm(" MARK 6");
               status_opt = DSPLIB_qrd_exec(handle, pA, pQ, pR, pU, pScratch);
               DSPLIB_asm(" MARK 7");
               TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)
            }
            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;

            status_init =
                DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 8");
               status_init = DSPLIB_qrd_init(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
               DSPLIB_asm(" MARK 9");
               TI_profile_stop();
               if (status_init == DSPLIB_SUCCESS) {
                  status_nat = DSPLIB_qrd_exec_checkParams(handle, pA, pQCn, pRCn, pUCn, pScratchCn);
               }
               else {
                  status_nat = status_init;
               }
            }
            else {
               status_nat = status_init;
            }

            if (status_nat == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CN);
               DSPLIB_asm(" MARK 10");
               status_nat = DSPLIB_qrd_exec(handle, pA, pQCn, pRCn, pUCn, pScratchCn);
               DSPLIB_asm(" MARK 11");
               TI_profile_stop();
            }

            int32_t statusDecompOpt, statusTransformOpt, statusDecompCn, statusTransformCn;
            int32_t statusCompareQ    = TI_TEST_KERNEL_FAIL;
            int32_t statusCompareR    = TI_TEST_KERNEL_FAIL;
            int32_t statusCompareQRef = TI_TEST_KERNEL_PASS;
            int32_t statusCompareRRef = TI_TEST_KERNEL_PASS;
            int32_t strideA           = bufParamsA.stride_y;
            int32_t strideQ           = bufParamsQ.stride_y;
            int32_t strideR           = bufParamsR.stride_y;
            int32_t status_nat_vs_opt = 0;
            int32_t status_opt_vs_ref = 0;
            int32_t status_check      = 0;
            if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {
               strcpy(print_txt, "optimized: ");
               statusDecompOpt = DSPLIB_qrd_check_decomposition(currPrm.testID, currPrm.dataType, nRows, nCols, pA, pQ,
                                                                pR, print_txt, strideA, strideQ, strideR);

               statusTransformOpt =
                   DSPLIB_qrd_check_transformation(currPrm.testID, currPrm.dataType, nRows, pQ, print_txt, strideQ);
               strcpy(print_txt, "natC: ");

               statusDecompCn = DSPLIB_qrd_check_decomposition(currPrm.testID, currPrm.dataType, nRows, nCols, pA, pQCn,
                                                               pRCn, print_txt, strideA, strideQ, strideR);
               statusTransformCn =
                   DSPLIB_qrd_check_transformation(currPrm.testID, currPrm.dataType, nRows, pQCn, print_txt, strideQ);

               /* ****************************************************************************/
               comparisonDone = 0;
               if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                  statusCompareQ =
                      TI_compare_mem_2D_float((void *) pQ, (void *) pQCn, 0.001, (double) powf(2, -8), nRows, nRows,
                                              bufParamsQ.stride_y, DSPLIB_sizeof(currPrm.dataType));
                  statusCompareR =
                      TI_compare_mem_2D_float((void *) pR, (void *) pRCn, 0.001, (double) powf(2, -8), nCols, nRows,
                                              bufParamsR.stride_y, DSPLIB_sizeof(currPrm.dataType));
                  comparisonDone = 1;
               }
               if (currPrm.staticOutQ != NULL && currPrm.staticOutR != NULL) {
                  comparisonDone  = 1;
                  int32_t widthQ  = 0;
                  int32_t heightQ = 0;
                  int32_t widthR  = 0;
                  int32_t heightR = 0;
                  if (currPrm.heightIn < currPrm.widthIn) {
                     widthQ  = currPrm.heightIn;
                     heightQ = currPrm.heightIn;
                     widthR  = currPrm.widthIn;
                     heightR = currPrm.heightIn;
                  }
                  else {
                     widthQ  = currPrm.widthIn;
                     heightQ = currPrm.heightIn;
                     widthR  = currPrm.widthIn;
                     heightR = currPrm.widthIn;
                  }
                  statusCompareQRef = TI_compare_mem_2DDualStride_float(
                      (void *) pQ, (void *) currPrm.staticOutQ, 0.001, (double) powf(2, -8), widthQ, heightQ,
                      bufParamsQ.stride_y, (widthQ * DSPLIB_sizeof(currPrm.dataType)), DSPLIB_sizeof(currPrm.dataType));
                  statusCompareRRef = TI_compare_mem_2DDualStride_float(
                      (void *) pR, (void *) currPrm.staticOutR, 0.001, (double) powf(2, -8), widthR, heightR,
                      bufParamsR.stride_y, (currPrm.widthIn * DSPLIB_sizeof(currPrm.dataType)),
                      DSPLIB_sizeof(currPrm.dataType));
               }
            }

            DSPLIB_DEBUGPRINTFN(0,
                                "statusCompareQ: %d, statusCompareR: %d, "
                                "statusCompareQRef: %d, statusCompareRRef: %d, "
                                "statusDecompOpt: %d, statusDecompCn: %d, "
                                "statusTransformOpt: %d, statusTransformCn: %d, "
                                "status_opt: %d, status_nat: %d, "
                                "comparisonDone: %d\n",
                                statusCompareQ, statusCompareR, statusCompareQRef, statusCompareRRef, statusDecompOpt,
                                statusDecompCn, statusTransformOpt, statusTransformCn, status_opt, status_nat,
                                comparisonDone);
            status_check = ((statusDecompOpt != statusDecompCn) || (statusTransformOpt != statusTransformCn)) ? 0 : 1;
            status_nat_vs_opt =
                ((statusCompareQ == TI_TEST_KERNEL_FAIL) || (statusCompareR == TI_TEST_KERNEL_FAIL)) ? 0 : 1;
            status_opt_vs_ref =
                ((statusCompareQRef == TI_TEST_KERNEL_FAIL) || (statusCompareRRef == TI_TEST_KERNEL_FAIL)) ? 0 : 1;

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                (((status_check + status_nat_vs_opt + status_opt_vs_ref) < 2) || (status_opt != DSPLIB_SUCCESS) ||
                 (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
            DSPLIB_DEBUGPRINTFN(0, "fail: %d, currentTestFail: %d\n", fail, currentTestFail);

            pProfile[9 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[9 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[9 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "QRD        : %s input | widthIn = %d heightIn = %d Data Type %d", testPatternString,
                    currPrm.widthIn, currPrm.heightIn, currPrm.dataType);
            uint64_t archCycles = pProfile[9 * tpi + 1];
#if (!defined(EVM_TEST))
            lokiCycles = archCycles;
            // write to CSV, must happen prior to write to screen because
            fprintf(fpOutputCSV, "QRD, %d, %d, %d, %d, %d\n", testNum, currPrm.testPattern,
                    currPrm.widthIn * currPrm.heightIn, (int) lokiCycles, !currentTestFail);

#endif // #if (!defined(EVM_TEST))

            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail, desc);
            /****************************************************************************************************
             *                               QRD SOLVER
             ****************************************************************************************************/
            /* Check invertability */
            int32_t invertible = 0, invertibleCn = 0;
            invertible   = DSPLIB_qrd_check_invertibility(currPrm.dataType, nRows, nCols, pR, strideR);
            invertibleCn = DSPLIB_qrd_check_invertibility(currPrm.dataType, nRows, nCols, pRCn, strideR);
            if (invertible & invertibleCn) {
               void *pYCn = pUCn;
               void *pY   = pU;
               /* TEST OPTIMIZED KERNEL */
               kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB,
                                                                &bufParamsU, &bufParamsX, &kerInitArgsSolv);
               if (status_init == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_INIT);
                  DSPLIB_asm(" MARK 12");
                  status_init = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                       &bufParamsX, &kerInitArgsSolv);
                  DSPLIB_asm(" MARK 13");
                  TI_profile_stop();
                  if (status_init == DSPLIB_SUCCESS) {
                     status_opt = DSPLIB_qrd_solver_exec_checkParams(handleSolv, pQ, pR, pB, pY, pX, pScratch, pInvA);
                  }
                  else {
                     status_opt = status_init;
                  }
               }
               else {
                  status_opt = status_init;
               }

               if (status_opt == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_OPT);
                  DSPLIB_asm(" MARK 14");
                  status_opt = DSPLIB_qrd_solver_exec(handleSolv, pQ, pR, pB, pY, pX, pScratch, pInvA);
                  DSPLIB_asm(" MARK 15");
                  TI_profile_stop();

#if !defined(__C7X_HOSTEM__)
                  /* The following for loop is to call kernel repeatedly so as to
                   * train the branch predictor                                   */
                  uint32_t k;
                  for (k = 0; k < 4; k++) {
                     // run warm instruction cache test
                     TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
                     TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
                     DSPLIB_asm(" MARK 16");
                     status_opt = DSPLIB_qrd_solver_exec(handleSolv, pQ, pR, pB, pY, pX, pScratch, pInvA);
                     DSPLIB_asm(" MARK 17");
                     TI_profile_stop();
                  }

                  // get output to L1D
                  int16_t outSum   = 0;
                  int8_t *pOutTemp = (int8_t *) pX; // treat output as bytes to be data type agnostic
                  for (k = 0; k < bufParamsX.dim_x; k++) {
                     outSum += *pOutTemp;
                     pOutTemp++;
                  }

                  // dummy store of outSum to insure that the compiler does not remove
                  // it.
                  volatileSum = outSum;

                  // run warm instruction cache test
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);
                  DSPLIB_asm(" MARK 18");
                  status_opt = DSPLIB_qrd_solver_exec(handleSolv, pQ, pR, pB, pY, pX, pScratch, pInvA);
                  DSPLIB_asm(" MARK 19");
                  TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)
               }
               /* Test _cn kernel */
               kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;

               status_init = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB,
                                                                &bufParamsU, &bufParamsX, &kerInitArgsSolv);
               if (status_init == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
                  DSPLIB_asm(" MARK 20");

                  status_init = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                       &bufParamsX, &kerInitArgsSolv);
                  DSPLIB_asm(" MARK 21");
                  TI_profile_stop();
                  if (status_init == DSPLIB_SUCCESS) {
                     status_nat =
                         DSPLIB_qrd_solver_exec_checkParams(handleSolv, pQCn, pRCn, pB, pYCn, pXCn, pScratch, pInvA);
                  }
               }
               else {
                  status_nat = status_init;
               }

               if (status_nat == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_CN);
                  DSPLIB_asm(" MARK 22");
                  status_nat = DSPLIB_qrd_solver_exec(handleSolv, pQCn, pRCn, pB, pYCn, pXCn, pScratch, pInvA);
                  DSPLIB_asm(" MARK 23");
                  TI_profile_stop();
               }
               /* ***********************************************************************************/

               int32_t statusSolvOpt, statusSolvCn;
               int     statusCompareX    = TI_TEST_KERNEL_FAIL;
               int     statusCompareXRef = TI_TEST_KERNEL_PASS;
               if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {
                  strcpy(print_txt, "optimized: ");
                  statusSolvOpt = DSPLIB_qrd_check_solution(currPrm.testID, currPrm.dataType, nRows, nCols, pA, pX, pB,
                                                            print_txt, strideA);

                  strcpy(print_txt, "natC: ");
                  statusSolvCn = DSPLIB_qrd_check_solution(currPrm.testID, currPrm.dataType, nRows, nCols, pA, pXCn, pB,
                                                           print_txt, strideA);

                  /* ****************************************************************************/
                  comparisonDone = 0;
                  if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                     statusCompareX = TI_compare_mem_2D_float((void *) pX, (void *) pXCn, 0.001, (double) powf(2, -8),
                                                              nCols, 1, 0, DSPLIB_sizeof(currPrm.dataType));
                     comparisonDone = 1;
                  }
                  if (currPrm.staticOutX != NULL) {
                     comparisonDone = 1;
                     statusCompareXRef =
                         TI_compare_mem_2D_float((void *) pX, (void *) currPrm.staticOutX, 0.001, (double) powf(2, -8),
                                                 nCols, 1, 0, DSPLIB_sizeof(currPrm.dataType));
                  }
               }
               DSPLIB_DEBUGPRINTFN(0,
                                   "statusCompareX: %d, statusCompareXRef: %d, "
                                   "statusSolvOpt: %d, statusSolvCn: %d, "
                                   "status_opt: %d, status_nat: %d, "
                                   "comparisonDone: %d\n",
                                   statusCompareX, statusCompareXRef, statusSolvOpt, statusSolvCn, status_opt,
                                   status_nat, comparisonDone);

               status_check      = (statusSolvOpt != statusSolvCn) ? 0 : 1;
               status_nat_vs_opt = ((statusCompareX == TI_TEST_KERNEL_FAIL)) ? 0 : 1;
               status_opt_vs_ref = ((statusCompareXRef == TI_TEST_KERNEL_FAIL)) ? 0 : 1;

               /* Set the 'fail' flag based on test vector comparison results */
               currentTestFail =
                   (((status_check + status_nat_vs_opt + status_opt_vs_ref) < 2) || (status_opt != DSPLIB_SUCCESS) ||
                    (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0))
                       ? 1
                       : 0;
               fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
               DSPLIB_DEBUGPRINTFN(0, "fail: %d, currentTestFail: %d\n", fail, currentTestFail);

               pProfile[9 * tpi + 3] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
               pProfile[9 * tpi + 4] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
               pProfile[9 * tpi + 5] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

               sprintf(desc, "QRD-solver :%s input | widthIn = %d heightIn = %d Data Type %d", testPatternString,
                       currPrm.widthIn, currPrm.heightIn, currPrm.dataType);
               archCycles = pProfile[9 * tpi + 4];
#if (!defined(EVM_TEST))
               lokiCycles = archCycles;
               // write to CSV, must happen prior to write to screen because
               fprintf(fpOutputCSV, "QRD-solver, %d, %d, %d, %d, %d\n", testNum, currPrm.testPattern,
                       currPrm.widthIn * currPrm.heightIn, (int) lokiCycles, !currentTestFail);
#endif // #if (!defined(EVM_TEST))

               DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

               TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail, desc);
               /****************************************************************************************************
                *                               QRD INVERSE
                ****************************************************************************************************/
               /* TEST OPTIMIZED KERNEL */
               kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init = DSPLIB_qrd_inverse_init_checkParams(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                                 &bufParamsInvAFinal, &kerInitArgsInv);

               if (status_init == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_INIT);
                  DSPLIB_asm(" MARK 24");
                  status_init = DSPLIB_qrd_inverse_init(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                        &bufParamsInvAFinal, &kerInitArgsInv);
                  DSPLIB_asm(" MARK 25");
                  TI_profile_stop();
                  if (status_init == DSPLIB_SUCCESS) {
                     status_opt = DSPLIB_qrd_inverse_exec_checkParams(handleInv, pQ, pR, pInvA, pInvAScratch, pScratch);
                  }
                  else {
                     status_opt = status_init;
                  }
               }
               else {
                  status_opt = status_init;
               }

               if (status_opt == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_OPT);
                  DSPLIB_asm(" MARK 26");
                  status_opt = DSPLIB_qrd_inverse_exec(handleInv, pQ, pR, pInvA, pInvAScratch, pScratch);
                  DSPLIB_asm(" MARK 27");
                  TI_profile_stop();

#if !defined(__C7X_HOSTEM__)
                  /* The following for loop is to call kernel repeatedly so as to
                   * train the branch predictor                                   */
                  uint32_t k;
                  for (k = 0; k < 4; k++) {
                     // run warm instruction cache test
                     memcpy(pR, pRCn, rSize);
                     TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
                     TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
                     DSPLIB_asm(" MARK 28");
                     status_opt = DSPLIB_qrd_inverse_exec(handleInv, pQ, pR, pInvA, pInvAScratch, pScratch);
                     DSPLIB_asm(" MARK 29");
                     TI_profile_stop();
                  }

                  // get output to L1D
                  int16_t outSum   = 0;
                  int8_t *pOutTemp = (int8_t *) pInvA; // treat output as bytes to be data type agnostic
                  for (k = 0; k < bufParamsInvA.dim_x; k++) {
                     outSum += *pOutTemp;
                     pOutTemp++;
                  }

                  // dummy store of outSum to insure that the compiler does not remove
                  // it.
                  volatileSum = outSum;

                  // run warm instruction cache test
                  memcpy(pR, pRCn, rSize);
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);
                  DSPLIB_asm(" MARK 30");
                  status_opt = DSPLIB_qrd_inverse_exec(handleInv, pQ, pR, pInvA, pInvAScratch, pScratch);
                  DSPLIB_asm(" MARK 31");
                  TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)
               }
               /* Test _cn kernel */
               kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;

               status_init = DSPLIB_qrd_inverse_init_checkParams(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                                 &bufParamsInvAFinal, &kerInitArgsInv);
               if (status_init == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
                  DSPLIB_asm(" MARK 1");

                  status_init = DSPLIB_qrd_inverse_init(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                        &bufParamsInvAFinal, &kerInitArgsInv);
                  DSPLIB_asm(" MARK 2");
                  TI_profile_stop();
                  if (status_init == DSPLIB_SUCCESS) {
                     status_nat =
                         DSPLIB_qrd_inverse_exec_checkParams(handleInv, pQCn, pRCn, pInvACn, pInvAScratch, pScratch);
                  }
                  else {
                     status_nat = status_init;
                  }
               }
               else {
                  status_nat = status_init;
               }

               if (status_nat == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_CN);
                  DSPLIB_asm(" MARK 3");
                  status_nat = DSPLIB_qrd_inverse_exec(handleInv, pQCn, pRCn, pInvACn, pInvAScratch, pScratch);
                  DSPLIB_asm(" MARK 4");
                  TI_profile_stop();
               }
               /************************************************************************************/
               int32_t statusInvOpt, statusInvCn;
               int32_t strideInvA           = bufParamsInvA.stride_y;
               int32_t statusCompareInvA    = TI_TEST_KERNEL_FAIL;
               int32_t statusCompareInvARef = TI_TEST_KERNEL_PASS;
               if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {
                  strcpy(print_txt, "optimized: ");
                  statusInvOpt = DSPLIB_qrd_check_inverse(currPrm.testID, currPrm.dataType, nRows, nCols, pA, pInvA,
                                                          print_txt, strideA, strideInvA);

                  strcpy(print_txt, "natC: ");

                  statusInvCn = DSPLIB_qrd_check_inverse(currPrm.testID, currPrm.dataType, nRows, nCols, pA, pInvACn,
                                                         print_txt, strideA, strideInvA);

                  /* ****************************************************************************/
                  comparisonDone = 0;
                  if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                     statusCompareInvA =
                         TI_compare_mem_2D_float((void *) pInvA, (void *) pInvACn, 0.001, (double) powf(2, -8), nCols,
                                                 nRows, bufParamsInvA.stride_y, DSPLIB_sizeof(currPrm.dataType));
                     comparisonDone = 1;
                  }
                  if (currPrm.staticOutInvA != NULL) {
                     comparisonDone       = 1;
                     statusCompareInvARef = TI_compare_mem_2DDualStride_float(
                         (void *) pInvA, (void *) currPrm.staticOutInvA, 0.001, (double) powf(2, -8), nCols, nRows,
                         bufParamsInvA.stride_y, (nCols * DSPLIB_sizeof(currPrm.dataType)),
                         DSPLIB_sizeof(currPrm.dataType));
                  }
               }
               DSPLIB_DEBUGPRINTFN(0,
                                   "statusCompareInvA: %d, statusCompareInvARef: %d, "
                                   "statusInvOpt: %d, statusInvCn: %d, "
                                   "status_opt: %d, status_nat: %d, "
                                   "comparisonDone: %d\n",
                                   statusCompareInvA, statusCompareInvARef, statusInvOpt, statusInvCn, status_opt,
                                   status_nat, comparisonDone);
               /* Set the 'fail' flag based on test vector comparison results */
               status_check      = (statusInvCn != statusInvOpt) ? 0 : 1;
               status_nat_vs_opt = ((statusCompareInvA == TI_TEST_KERNEL_FAIL)) ? 0 : 1;
               status_opt_vs_ref = ((statusCompareInvARef == TI_TEST_KERNEL_FAIL)) ? 0 : 1;
               currentTestFail =
                   (((status_check + status_nat_vs_opt + status_opt_vs_ref) < 2) || (status_opt != DSPLIB_SUCCESS) ||
                    (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0))
                       ? 1
                       : 0;
               pProfile[9 * tpi + 6] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
               pProfile[9 * tpi + 7] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
               pProfile[9 * tpi + 8] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);
               fail                  = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
               DSPLIB_DEBUGPRINTFN(0, "fail: %d, currentTestFail: %d\n", fail, currentTestFail);

               sprintf(desc, "QRD-inverse: %s input | widthIn = %d heightIn = %d Data Type %d", testPatternString,
                       currPrm.widthIn, currPrm.heightIn, currPrm.dataType);
               archCycles = pProfile[9 * tpi + 7];
#if (!defined(EVM_TEST))
               lokiCycles = archCycles;
               // write to CSV, must happen prior to write to screen because
               fprintf(fpOutputCSV, "QRD-inverse, %d, %d, %d, %d, %d\n", testNum, currPrm.testPattern,
                       currPrm.widthIn * currPrm.heightIn, (int) lokiCycles, !currentTestFail);
#endif // #if (!defined(EVM_TEST))

               DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

               TI_profile_add_test(testNum++, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail,
                                   desc);
            }
            else {
               sprintf(desc, "Input matrix is non-invertible:%s input | widthIn = %d heightIn = %d Data Type %d",
                       testPatternString, currPrm.widthIn, currPrm.heightIn, currPrm.dataType);
               TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_y, 0, 0, 0, desc);
               sprintf(desc, "Input matrix is non-invertible: %s input | widthIn = %d heightIn = %d Data Type %d",
                       testPatternString, currPrm.widthIn, currPrm.heightIn, currPrm.dataType);
               TI_profile_add_test(testNum++, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, 0, desc);
            }
         }
         else {
            sprintf(desc, "Data size=%d ", DSPLIB_sizeof(bufParamsA.data_type));

            DSPLIB_DEBUGPRINTFN(0, "%s\n", "Skipping Test");

            TI_profile_skip_test(desc);
            TI_profile_clear_run_stats();
         } // end of memory allocation successful?
         // Solver and inverse

         /* Free buffers for each test vector */

         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pQCn);
            free(pRCn);
            free(pUCn);
            free(pInvACn);
            free(pXCn);
            free(pScratchCn);

            TI_align_free(pQ);
            TI_align_free(pR);
            TI_align_free(pU);
            TI_align_free(pInvA);
            TI_align_free(pInvAScratch);
            TI_align_free(pX);
            TI_align_free(pScratch);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pQ);
            TI_align_free(pR);
            TI_align_free(pU);
            TI_align_free(pInvA);
            TI_align_free(pX);
            TI_align_free(pScratch);
            TI_align_free(pInvAScratch);
#else
            TI_align_free(pQ);
            TI_align_free(pR);
            TI_align_free(pU);
            TI_align_free(pX);
            TI_align_free(pScratch);
            TI_align_free(pInvAScratch);
#endif
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pQ);
            TI_align_free(pR);
            TI_align_free(pU);
            TI_align_free(pInvA);
            TI_align_free(pX);
            TI_align_free(pScratch);
            TI_align_free(pInvAScratch);
#endif
         }
         TI_align_free(pA);
         TI_align_free(pB);
      } // end repetitions
   }    // end idat test cases

   free(handle);
   free(handleSolv);
   free(handleInv);

#if (!defined(EVM_TEST))
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif // #if (!defined(EVM_TEST))

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return fail: %d\n", fail);

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_qrd");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_qrd_d(&pProfile[0], 0);
   }
}
int coverage_test_main()
{
   int32_t                  testNum         = 1000;
   int32_t                  currentTestFail = 0;
   DSPLIB_STATUS            status_nat;
   DSPLIB_STATUS            status_opt;
   DSPLIB_bufParams2D_t     bufParamsA, bufParamsQ, bufParamsR, bufParamsInvA, bufParamsInvAFinal;
   DSPLIB_bufParams1D_t     bufParamsU, bufParamsX, bufParamsB;
   DSPLIB_qrdInitArgs       kerInitArgs;
   int32_t                  handleSize = DSPLIB_qrd_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle      handle     = malloc(handleSize);
   DSPLIB_qrdSolverInitArgs kerInitArgsSolv;
   int32_t                  handleSolvSize = DSPLIB_qrd_solver_getHandleSize(&kerInitArgsSolv);
   DSPLIB_kernelHandle      handleSolv     = malloc(handleSolvSize);
   DSPLIB_qrdInvInitArgs    kerInitArgsInv;
   int32_t                  handleInvSize = DSPLIB_qrd_inverse_getHandleSize(&kerInitArgsInv);
   DSPLIB_kernelHandle      handleInv     = malloc(handleInvSize);
   int                      fail          = 0;
   void                    *pA            = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64 * 64);
   void                    *pQ            = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64 * 64);
   void                    *pR            = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64 * 64);
   void                    *pU            = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                    *pInvA         = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                    *pInvAScratch  = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                    *pScratch      = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                    *pB            = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                    *pX            = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                    *pD            = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);
   void                    *pR1           = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 64);

   bufParamsU.dim_x         = 32;
   bufParamsA.dim_x         = 32;
   bufParamsA.dim_y         = 32;
   bufParamsQ.dim_x         = 32;
   bufParamsQ.dim_y         = 32;
   bufParamsR.dim_x         = 32;
   bufParamsR.dim_y         = 32;
   bufParamsB.dim_x         = 32;
   bufParamsInvA.dim_x      = 32;
   bufParamsInvA.dim_y      = 32;
   bufParamsInvAFinal.dim_x = 32;
   bufParamsInvAFinal.dim_y = 32;
   bufParamsX.dim_x         = 32;
   while (testNum <= 1024) {

      switch (testNum) {
      case 1000:
         bufParamsA.data_type  = DSPLIB_INT32;
         bufParamsQ.data_type  = DSPLIB_INT32;
         bufParamsR.data_type  = DSPLIB_INT32;
         bufParamsU.data_type  = DSPLIB_INT32;
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat =
             DSPLIB_qrd_init_checkParams(NULL, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt =
             DSPLIB_qrd_init_checkParams(NULL, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         bufParamsA.data_type  = DSPLIB_INT32;
         bufParamsQ.data_type  = DSPLIB_INT32;
         bufParamsR.data_type  = DSPLIB_INT32;
         bufParamsU.data_type  = DSPLIB_INT32;
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat =
             DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt =
             DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsA.data_type  = DSPLIB_FLOAT32;
         bufParamsQ.data_type  = DSPLIB_INT16;
         bufParamsR.data_type  = DSPLIB_FLOAT32;
         bufParamsU.data_type  = DSPLIB_FLOAT32;
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat =
             DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsA.data_type  = DSPLIB_FLOAT32;
         bufParamsQ.data_type  = DSPLIB_FLOAT32;
         bufParamsR.data_type  = DSPLIB_INT16;
         bufParamsU.data_type  = DSPLIB_FLOAT32;
         status_opt =
             DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsA.data_type  = DSPLIB_FLOAT32;
         bufParamsQ.data_type  = DSPLIB_FLOAT32;
         bufParamsR.data_type  = DSPLIB_FLOAT32;
         bufParamsU.data_type  = DSPLIB_INT16;
         status_opt =
             DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         status_nat      = DSPLIB_qrd_exec_checkParams(handle, NULL, pQ, pR, pU, pScratch);
         status_nat      = DSPLIB_qrd_exec_checkParams(handle, pA, NULL, pR, pU, pScratch);
         status_nat      = DSPLIB_qrd_exec_checkParams(handle, pA, pQ, NULL, pU, pScratch);
         status_nat      = DSPLIB_qrd_exec_checkParams(handle, pA, pQ, pR, NULL, pScratch);
         status_nat      = DSPLIB_qrd_exec_checkParams(handle, pA, pQ, pR, pU, NULL);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1004:
         bufParamsA.data_type  = DSPLIB_INT32;
         bufParamsQ.data_type  = DSPLIB_INT32;
         bufParamsR.data_type  = DSPLIB_INT32;
         bufParamsU.data_type  = DSPLIB_INT32;
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_init(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_qrd_init(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1005:
         bufParamsInvA.data_type      = DSPLIB_INT32;
         bufParamsInvAFinal.data_type = DSPLIB_INT32;
         bufParamsR.data_type         = DSPLIB_INT32;
         bufParamsU.data_type         = DSPLIB_INT32;
         kerInitArgsInv.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_nat               = DSPLIB_qrd_inverse_init_checkParams(NULL, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                                        &bufParamsInvAFinal, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt               = DSPLIB_qrd_inverse_init_checkParams(NULL, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                                        &bufParamsInvAFinal, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1006:
         bufParamsInvA.data_type      = DSPLIB_INT32;
         bufParamsInvAFinal.data_type = DSPLIB_INT32;
         bufParamsR.data_type         = DSPLIB_INT32;
         bufParamsU.data_type         = DSPLIB_INT32;
         bufParamsQ.data_type         = DSPLIB_FLOAT32;
         kerInitArgsInv.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_inverse_init_checkParams(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                          &bufParamsInvAFinal, &kerInitArgsInv);
         kerInitArgsInv.funcStyle     = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsInvA.data_type      = DSPLIB_INT32;
         bufParamsInvAFinal.data_type = DSPLIB_INT32;
         bufParamsR.data_type         = DSPLIB_FLOAT32;
         bufParamsU.data_type         = DSPLIB_INT32;
         bufParamsQ.data_type         = DSPLIB_INT32;
         status_opt      = DSPLIB_qrd_inverse_init_checkParams(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                               &bufParamsInvAFinal, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1007:
         bufParamsInvA.data_type      = DSPLIB_FLOAT32;
         bufParamsA.data_type         = DSPLIB_FLOAT32;
         bufParamsInvAFinal.data_type = DSPLIB_FLOAT32;
         bufParamsQ.data_type         = DSPLIB_FLOAT64;
         bufParamsR.data_type         = DSPLIB_FLOAT32;
         kerInitArgsInv.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_inverse_init_checkParams(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                          &bufParamsInvAFinal, &kerInitArgsInv);
         kerInitArgsInv.funcStyle     = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsInvA.data_type      = DSPLIB_FLOAT32;
         bufParamsInvAFinal.data_type = DSPLIB_INT32;
         bufParamsQ.data_type         = DSPLIB_FLOAT32;
         bufParamsR.data_type         = DSPLIB_FLOAT64;
         status_opt      = DSPLIB_qrd_inverse_init_checkParams(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                               &bufParamsInvAFinal, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1008:
         status_nat = DSPLIB_qrd_inverse_exec_checkParams(handleInv, NULL, pR, pInvA, pInvAScratch, pScratch);
         status_nat = DSPLIB_qrd_inverse_exec_checkParams(handleInv, pQ, NULL, pInvA, pInvAScratch, pScratch);
         status_nat = DSPLIB_qrd_inverse_exec_checkParams(handleInv, pQ, pR, NULL, pInvAScratch, pScratch);
         status_nat = DSPLIB_qrd_inverse_exec_checkParams(handleInv, pQ, pR, pInvA, NULL, pScratch);
         status_nat = DSPLIB_qrd_inverse_exec_checkParams(handleInv, pQ, pR, pInvA, pInvAScratch, NULL);

         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1009:
         bufParamsInvA.data_type      = DSPLIB_INT32;
         bufParamsInvAFinal.data_type = DSPLIB_INT32;
         bufParamsR.data_type         = DSPLIB_INT32;
         bufParamsQ.data_type         = DSPLIB_INT32;
         bufParamsU.data_type         = DSPLIB_INT32;
         kerInitArgsInv.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_inverse_init(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA, &bufParamsInvAFinal,
                                              &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_qrd_inverse_init(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA, &bufParamsInvAFinal,
                                              &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1010:
         bufParamsB.data_type      = DSPLIB_INT32;
         bufParamsQ.data_type      = DSPLIB_INT32;
         bufParamsX.data_type      = DSPLIB_INT32;
         bufParamsR.data_type      = DSPLIB_INT32;
         bufParamsU.data_type      = DSPLIB_INT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init_checkParams(NULL, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_qrd_solver_init_checkParams(NULL, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                              &bufParamsX, &kerInitArgsSolv);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1011:
         bufParamsB.data_type      = DSPLIB_INT32;
         bufParamsQ.data_type      = DSPLIB_INT32;
         bufParamsX.data_type      = DSPLIB_INT32;
         bufParamsR.data_type      = DSPLIB_INT32;
         bufParamsU.data_type      = DSPLIB_INT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1012:
         bufParamsB.data_type      = DSPLIB_INT32;
         bufParamsQ.data_type      = DSPLIB_FLOAT32;
         bufParamsX.data_type      = DSPLIB_INT8;
         bufParamsR.data_type      = DSPLIB_FLOAT32;
         bufParamsU.data_type      = DSPLIB_FLOAT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1013:
         status_nat      = DSPLIB_qrd_solver_exec_checkParams(handleSolv, NULL, pR, pB, pU, pX, pD, pR1);
         status_nat      = DSPLIB_qrd_solver_exec_checkParams(handleSolv, pQ, NULL, pB, pU, pX, pD, pR1);
         status_nat      = DSPLIB_qrd_solver_exec_checkParams(handleSolv, pQ, pR, NULL, pU, pX, pD, pR1);
         status_nat      = DSPLIB_qrd_solver_exec_checkParams(handleSolv, pQ, pR, pB, NULL, pX, pD, pR1);
         status_nat      = DSPLIB_qrd_solver_exec_checkParams(handleSolv, pQ, pR, pB, pU, NULL, pD, pR1);
         status_nat      = DSPLIB_qrd_solver_exec_checkParams(handleSolv, pQ, pR, pB, pU, pX, NULL, pR1);
         status_nat      = DSPLIB_qrd_solver_exec_checkParams(handleSolv, pQ, pR, pB, pU, pX, pD, NULL);
         currentTestFail = (status_nat != DSPLIB_ERR_NULL_POINTER);
         break;
      case 1014:
         bufParamsB.data_type      = DSPLIB_INT32;
         bufParamsQ.data_type      = DSPLIB_INT32;
         bufParamsX.data_type      = DSPLIB_INT32;
         bufParamsR.data_type      = DSPLIB_INT32;
         bufParamsU.data_type      = DSPLIB_INT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                  &bufParamsX, &kerInitArgsSolv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1015:
         bufParamsInvA.data_type      = DSPLIB_FLOAT32;
         bufParamsInvAFinal.data_type = DSPLIB_FLOAT32;
         bufParamsR.data_type         = DSPLIB_FLOAT32;
         bufParamsQ.data_type         = DSPLIB_FLOAT32;
         bufParamsR.dim_x             = 16;
         bufParamsR.dim_y             = 23;
         bufParamsQ.dim_x             = 16;
         bufParamsQ.dim_y             = 16;
         kerInitArgsInv.funcStyle     = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_inverse_init_checkParams(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                          &bufParamsInvAFinal, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsR.dim_x         = 16;
         bufParamsR.dim_y         = 16;
         bufParamsQ.dim_x         = 23;
         bufParamsQ.dim_y         = 16;
         status_opt = DSPLIB_qrd_inverse_init_checkParams(handleInv, &bufParamsQ, &bufParamsR, &bufParamsInvA,
                                                          &bufParamsInvAFinal, &kerInitArgsInv);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1016:
         bufParamsB.data_type      = DSPLIB_INT32;
         bufParamsQ.data_type      = DSPLIB_FLOAT32;
         bufParamsX.data_type      = DSPLIB_INT32;
         bufParamsR.data_type      = DSPLIB_FLOAT32;
         bufParamsU.data_type      = DSPLIB_FLOAT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsB.data_type      = DSPLIB_INT32;
         bufParamsQ.data_type      = DSPLIB_FLOAT32;
         bufParamsX.data_type      = DSPLIB_FLOAT32;
         bufParamsR.data_type      = DSPLIB_FLOAT32;
         bufParamsU.data_type      = DSPLIB_FLOAT32;
         status_opt = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1017:
         bufParamsB.data_type      = DSPLIB_INT32;
         bufParamsQ.data_type      = DSPLIB_FLOAT32;
         bufParamsX.data_type      = DSPLIB_INT32;
         bufParamsR.data_type      = DSPLIB_FLOAT32;
         bufParamsU.data_type      = DSPLIB_FLOAT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsB.data_type      = DSPLIB_INT32;
         bufParamsQ.data_type      = DSPLIB_FLOAT32;
         bufParamsX.data_type      = DSPLIB_FLOAT32;
         bufParamsR.data_type      = DSPLIB_INT32;
         bufParamsU.data_type      = DSPLIB_FLOAT32;
         status_opt = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1018:
         bufParamsA.data_type = DSPLIB_FLOAT32;
         bufParamsQ.data_type = DSPLIB_FLOAT32;
         bufParamsR.data_type = DSPLIB_FLOAT32;
         bufParamsU.data_type = DSPLIB_FLOAT32;

         bufParamsA.dim_x    = 16;
         bufParamsA.dim_y    = 16;
         bufParamsA.stride_y = 64;

         bufParamsQ.dim_x    = 16;
         bufParamsQ.dim_y    = 16;
         bufParamsQ.stride_y = 64;

         bufParamsR.dim_x    = 16;
         bufParamsR.dim_y    = 16;
         bufParamsR.stride_y = 64;

         bufParamsU.dim_x = 16;

         memset(pA, 0, 16 * 64);
         memset(pR, 0, 16 * 64);
         memset(pQ, 0, 16 * 64);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat =
             DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         status_nat = DSPLIB_qrd_init(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         status_nat = DSPLIB_qrd_exec(handle, pA, pQ, pR, pU, pScratch);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt =
             DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         status_opt      = DSPLIB_qrd_init(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         status_opt      = DSPLIB_qrd_exec_checkParams(handle, pA, pQ, pR, pU, pScratch);
         status_opt      = DSPLIB_qrd_exec(handle, pA, pQ, pR, pU, pScratch);
         currentTestFail = ((status_nat != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS));
         break;
      case 1019:
         bufParamsB.data_type      = DSPLIB_FLOAT32;
         bufParamsQ.data_type      = DSPLIB_FLOAT32;
         bufParamsX.data_type      = DSPLIB_INT32;
         bufParamsR.data_type      = DSPLIB_INT32;
         bufParamsU.data_type      = DSPLIB_INT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                  &bufParamsX, &kerInitArgsSolv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1020:
         bufParamsB.data_type      = DSPLIB_FLOAT32;
         bufParamsQ.data_type      = DSPLIB_INT32;
         bufParamsX.data_type      = DSPLIB_FLOAT32;
         bufParamsR.data_type      = DSPLIB_INT32;
         bufParamsU.data_type      = DSPLIB_INT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                  &bufParamsX, &kerInitArgsSolv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1021:
         bufParamsB.data_type      = DSPLIB_FLOAT32;
         bufParamsQ.data_type      = DSPLIB_INT32;
         bufParamsX.data_type      = DSPLIB_INT32;
         bufParamsR.data_type      = DSPLIB_FLOAT32;
         bufParamsU.data_type      = DSPLIB_INT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                  &bufParamsX, &kerInitArgsSolv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         bufParamsQ.data_type      = DSPLIB_INT32;
         bufParamsX.data_type      = DSPLIB_INT32;
         bufParamsR.data_type      = DSPLIB_FLOAT64;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         currentTestFail =
             currentTestFail || ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         bufParamsQ.data_type      = DSPLIB_INT32;
         bufParamsX.data_type      = DSPLIB_FLOAT64;
         bufParamsR.data_type      = DSPLIB_INT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         currentTestFail =
             currentTestFail || ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         bufParamsQ.data_type      = DSPLIB_FLOAT64;
         bufParamsX.data_type      = DSPLIB_INT32;
         bufParamsR.data_type      = DSPLIB_INT32;
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         currentTestFail =
             currentTestFail || ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         break;
      case 1022:
         bufParamsB.data_type = DSPLIB_FLOAT32;
         bufParamsQ.data_type = DSPLIB_FLOAT32;
         bufParamsX.data_type = DSPLIB_FLOAT32;
         bufParamsR.data_type = DSPLIB_FLOAT32;
         bufParamsU.data_type = DSPLIB_FLOAT32;

         bufParamsQ.dim_x = 16;
         bufParamsQ.dim_y = 16;
         bufParamsR.dim_x = 17;
         bufParamsR.dim_y = 16;

         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsQ.dim_x          = 16;
         bufParamsQ.dim_y          = 17;
         bufParamsR.dim_x          = 16;
         bufParamsR.dim_y          = 16;
         status_opt = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                                         &bufParamsX, &kerInitArgsSolv);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1023: {

         bufParamsA.data_type = DSPLIB_FLOAT32;
         bufParamsQ.data_type = DSPLIB_FLOAT32;
         bufParamsR.data_type = DSPLIB_FLOAT32;
         bufParamsU.data_type = DSPLIB_FLOAT32;
         bufParamsB.data_type = DSPLIB_FLOAT32;
         bufParamsX.data_type = DSPLIB_FLOAT32;

         bufParamsA.dim_x    = 16;
         bufParamsA.dim_y    = 16;
         bufParamsA.stride_y = 64;

         bufParamsQ.dim_x    = 8;
         bufParamsQ.dim_y    = 8;
         bufParamsQ.stride_y = 64;

         bufParamsR.dim_x    = 16;
         bufParamsR.dim_y    = 8;
         bufParamsR.stride_y = 64;

         bufParamsU.dim_x = 16;
         bufParamsB.dim_x = 16;
         bufParamsX.dim_x = 16;

         kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsU,
                                             &bufParamsX, &kerInitArgsSolv);
         status_nat = DSPLIB_qrd_solver_exec(handleSolv, pQ, pR, pB, pU, pX, pD, pR1);

         currentTestFail = (status_nat != DSPLIB_SUCCESS);
      } break;

      case 1024: {

         bufParamsA.data_type = DSPLIB_FLOAT32;
         bufParamsQ.data_type = DSPLIB_FLOAT32;
         bufParamsR.data_type = DSPLIB_FLOAT32;
         bufParamsU.data_type = DSPLIB_FLOAT32;
         bufParamsB.data_type = DSPLIB_FLOAT32;
         bufParamsX.data_type = DSPLIB_FLOAT32;

         bufParamsA.dim_x    = 16;
         bufParamsA.dim_y    = 16;
         bufParamsA.stride_y = 16;

         bufParamsQ.dim_x    = 16;
         bufParamsQ.dim_y    = 16;
         bufParamsQ.stride_y = 16;

         bufParamsR.dim_x    = 16;
         bufParamsR.dim_y    = 16;
         bufParamsR.stride_y = 16;

         bufParamsU.dim_x = 16;
         bufParamsB.dim_x = 16;
         bufParamsX.dim_x = 16;
         kerInitArgs.dimX = 16;
         kerInitArgs.dimY = 16;

         float   *pFillA = (float *) pA;
         uint32_t ii = 0, jj = 0;
         for (ii = 0; ii < 16; ii++) {
            for (jj = 0; jj < 16; jj++) {
               pFillA[ii + jj * 16] = 0.0;
               if (ii == jj) {
                  pFillA[ii + jj * 16] = 1.0;
               }
            }
         }

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_qrd_init(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         if (status_nat == DSPLIB_SUCCESS) {
            status_nat = DSPLIB_qrd_exec(handle, pA, pQ, pR, pU, pScratch);
         }
         currentTestFail = (status_nat != DSPLIB_SUCCESS);

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_qrd_init(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsU, &kerInitArgs);
         if (status_opt == DSPLIB_SUCCESS) {
            status_opt = DSPLIB_qrd_exec(handle, pA, pQ, pR, pU, pScratch);
         }
         currentTestFail = currentTestFail || (status_opt != DSPLIB_SUCCESS);

      } break;

      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   TI_align_free(pA);
   TI_align_free(pQ);
   TI_align_free(pR);
   TI_align_free(pU);
   TI_align_free(pInvA);
   TI_align_free(pInvAScratch);
   TI_align_free(pScratch);
   TI_align_free(pB);
   TI_align_free(pX);
   TI_align_free(pD);
   TI_align_free(pR1);
   free(handle);
   free(handleSolv);
   free(handleInv);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_qrd_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_qrd_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_qrd_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}
/* Main call for insqridual test projects */
#if !defined(__ONESHOTTEST)
int main()
{
   int fail = 1;

   uint32_t profile[256 * 9] = {0};

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
             3 * i + 1, profile[9 * i + 0], profile[9 * i + 1], profile[9 * i + 2]);
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = "
             "%8d\n",
             3 * i + 2, profile[9 * i + 3], profile[9 * i + 4], profile[9 * i + 5]);
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = "
             "%8d\n",
             3 * i + 3, profile[9 * i + 6], profile[9 * i + 7], profile[9 * i + 8]);
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
