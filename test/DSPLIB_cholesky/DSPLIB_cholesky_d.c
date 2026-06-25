// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_cholesky_check.h"
#include "DSPLIB_cholesky_idat.h"

#define DSPLIB_ROW_STRIDE(x, y) (x + y) //(((x+y)/y)*y)

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
int DSPLIB_cholesky_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsA, bufParamsU, bufParamsUT;
   DSPLIB_bufParams1D_t bufParamsY, bufParamsB, bufParamsX, bufParamsDiv, bufParamsMul;

#if !defined(EVM_TEST)
   uint64_t lokiCycles;
#endif // #if !defined(EVM_TEST)

   uint32_t                      testNum;
   DSPLIB_cholesky_testParams_t *prm;
   DSPLIB_cholesky_testParams_t  currPrm;
   DSPLIB_cholesky_getTestParams(&prm, &test_cases);
   DSPLIB_cholesky_InitArgs kerInitArgs;

   uint64_t            archCycles = 0;
   int32_t             handleSize = DSPLIB_cholesky_getHandleSize(&kerInitArgs); // bufpblock size...
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_cholesky_solver_InitArgs kerInitArgsSolver;
   int32_t                         handleSolverSize = DSPLIB_cholesky_solver_getHandleSize(&kerInitArgsSolver);
   DSPLIB_kernelHandle             handleSolver     = malloc(handleSolverSize);

   DSPLIB_cholesky_inplace_InitArgs kerInitArgsInplace;
   int32_t                          handleInplaceSize = DSPLIB_cholesky_inplace_getHandleSize(&kerInitArgsInplace);
   DSPLIB_kernelHandle              handleInplace     = malloc(handleInplaceSize);

   TI_profile_init("DSPLIB_cholesky");

#if (!defined(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_cholesky.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Data size, Loki cycles, Pass/Fail\n");
#endif // #if (!defined(EVM_TEST))   #if (EVM_TEST==1)

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];
      for (repCount = 0; repCount < numReps; repCount++) {
         DSPLIB_STATUS status_init = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt  = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat  = DSPLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;
         currentTestFail        = 0;

         bufParamsA.data_type = currPrm.dataType;
         bufParamsA.dim_x     = currPrm.order;
         bufParamsA.dim_y     = currPrm.order;
         bufParamsA.stride_y  = currPrm.strideIn;

         bufParamsU.data_type = currPrm.dataType;
         bufParamsU.dim_x     = currPrm.order;
         bufParamsU.dim_y     = currPrm.order;
         bufParamsU.stride_y  = currPrm.strideOut;

         int32_t dataWidth      = DSPLIB_sizeof(currPrm.dataType);
         kerInitArgs.order      = currPrm.order;
         kerInitArgs.stride     = currPrm.strideOut;
         kerInitArgs.strideCn   = currPrm.order * dataWidth;
         kerInitArgs.enableTest = currPrm.enableTest;

         int32_t memPad = 128;

         if ((bufParamsA.stride_y % 512) == 0) {
            bufParamsA.stride_y  = bufParamsA.stride_y + memPad;
            bufParamsU.stride_y  = bufParamsU.stride_y + memPad;
            kerInitArgs.stride   = kerInitArgs.stride + memPad;
            kerInitArgs.strideCn = kerInitArgs.strideCn + memPad;
         }

         int32_t  eleCount = 2 * __C7X_VEC_SIZE_BYTES__ / DSPLIB_sizeof(currPrm.dataType);
         uint64_t uSize    = bufParamsU.stride_y * DSPLIB_ROW_STRIDE(bufParamsU.dim_y, eleCount);
         uint64_t aSize    = bufParamsA.stride_y * DSPLIB_ROW_STRIDE(bufParamsA.dim_y, eleCount);
         DSPLIB_DEBUGPRINTFN(0, "uSize: %lu aSize: %lu\n", uSize, aSize);

         void *pA = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, aSize);

         void    *pMultiplier = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, (bufParamsA.dim_y * dataWidth));
         uint64_t usedSizeL2  = aSize;
         usedSizeL2 += bufParamsA.dim_y * dataWidth;
         DSPLIB_DEBUGPRINTFN(0, "pA: %p pTemp: %p pMultiplier: %p\n", pA, pTemp, pMultiplier);

         kerInitArgsSolver.order    = kerInitArgs.order;
         kerInitArgsSolver.stride   = kerInitArgs.stride;
         kerInitArgsSolver.strideCn = kerInitArgs.strideCn;

         bufParamsUT.data_type = currPrm.dataType;
         bufParamsUT.dim_x     = currPrm.order;
         bufParamsUT.dim_y     = currPrm.order;
         bufParamsUT.stride_y  = kerInitArgsSolver.stride;

         bufParamsY.data_type = currPrm.dataType;
         bufParamsY.dim_x     = currPrm.order;

         bufParamsB.data_type = currPrm.dataType;
         bufParamsB.dim_x     = currPrm.order;

         bufParamsX.dim_x     = currPrm.order;
         bufParamsX.data_type = currPrm.dataType;

         bufParamsDiv.dim_x     = currPrm.order;
         bufParamsDiv.data_type = currPrm.dataType;

         void *pUT  = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
         void *pY   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsX.dim_x * dataWidth);
         void *pB   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsX.dim_x * dataWidth);
         void *pX   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsX.dim_x * dataWidth);
         void *pDiv = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsX.dim_x * dataWidth);

         usedSizeL2 += uSize;
         usedSizeL2 += bufParamsX.dim_x * dataWidth;
         usedSizeL2 += bufParamsX.dim_x * dataWidth;
         usedSizeL2 += bufParamsX.dim_x * dataWidth;
         usedSizeL2 += bufParamsX.dim_x * dataWidth;

         DSPLIB_DEBUGPRINTFN(0, "pUT: %p pY: %p pB: %p pX: %p pDiv: %p\n", pUT, pY, pB, pX, pDiv);

         uint64_t sizeL   = bufParamsU.dim_y * bufParamsU.stride_y;
         uint64_t sizeXCn = bufParamsX.dim_x * dataWidth;
         DSPLIB_DEBUGPRINTFN(0, "sizeL: %lu sizeXCn: %lu\n", sizeL, sizeXCn);

         kerInitArgsInplace.order      = kerInitArgs.order;
         kerInitArgsInplace.stride     = kerInitArgs.stride;
         kerInitArgsInplace.enableTest = currPrm.enableTest;

         bufParamsMul.dim_x     = currPrm.order;
         bufParamsMul.data_type = currPrm.dataType;

         void    *pACn;
         uint32_t sizePInACn = bufParamsA.dim_y * kerInitArgsInplace.stride;
         void    *pU, *pUCn;
         void    *pXCn;
         void    *pTemp;

         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pU = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            usedSizeL2 += uSize;

            pTemp = (void *) malloc(aSize);
            pUCn  = (void *) malloc(bufParamsU.dim_y * kerInitArgs.strideCn);
            pXCn  = (void *) malloc(sizeXCn);
            pACn  = (void *) malloc(aSize);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
            pU = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            usedSizeL2 += uSize;

            pUCn  = (void *) ddrBuffer;
            pTemp = (void *) (ddrBuffer + sizeL);
            pXCn  = (void *) (ddrBuffer + sizeL + aSize);
            pACn  = (void *) (ddrBuffer + sizeL + +aSize + sizeXCn);
         }
         else {
            pU = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            usedSizeL2 += uSize;

            pUCn  = (void *) ddrBuffer;
            pTemp = (void *) (ddrBuffer + sizeL);
            pXCn  = (void *) (ddrBuffer + sizeL + aSize);
            pACn  = (void *) (ddrBuffer + sizeL + +aSize + sizeXCn);
         }
         DSPLIB_DEBUGPRINTFN(0, "usedSizeL2: %ld bytes\n", usedSizeL2);
         DSPLIB_DEBUGPRINTFN(0, "data_type = %d order %d\n", bufParamsA.data_type, kerInitArgs.order);
         DSPLIB_DEBUGPRINTFN(0, "pL: %p pUCn: %p pXCn: %p pACn: %p\n", pU, pUCn, pXCn, pACn);

         /* Only run the test if the buffer allocations fit in the heap */

         if (pA && pACn && pB && pU && pUCn && pX && pXCn && pY && pUT && pTemp && pMultiplier && pDiv) {

            if (currPrm.testPatternA == STATIC) {
               TI_fillBuffer_float(prm[tpi].testPatternA, 0, pA, prm[tpi].staticIn, bufParamsA.dim_x, bufParamsA.dim_y,
                                   bufParamsA.stride_y, dataWidth, testPatternString);

               TI_fillBuffer_float(prm[tpi].testPatternA, 0, pB, prm[tpi].staticInB, bufParamsB.dim_x, 1, 0, dataWidth,
                                   testPatternString);
            }
            else {
               TI_fillBuffer_float(prm[tpi].testPatternA, 0, pA, pTemp, bufParamsA.dim_x, bufParamsA.dim_y,
                                   bufParamsA.stride_y, dataWidth, testPatternString);
               int32_t ii;

               /* Filling the B vector with constant values = order */
               if (dataWidth == 4) {
                  float *pBFill = (float *) pB;
                  for (ii = 0; ii < (int32_t) currPrm.order; ii++) {
                     pBFill[ii] = (float) currPrm.order;
                  }
               }
               else {
                  double *pBFill = (double *) pB;
                  for (ii = 0; ii < (int32_t) currPrm.order; ii++) {
                     pBFill[ii] = (double) currPrm.order;
                  }
               }
            }

            memcpy(pTemp, pA, bufParamsA.dim_y * bufParamsA.stride_y);

            /***********************************************************************
             *                   Cholesky Decomposition
             **********************************************************************/
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init           = DSPLIB_cholesky_init_checkParams(handle, &bufParamsA, &bufParamsU, &kerInitArgs);

            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               status_init = DSPLIB_cholesky_init(handle, &bufParamsA, &bufParamsU, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();

               if (status_init == DSPLIB_SUCCESS) {
                  status_opt = DSPLIB_cholesky_exec_checkParams(handle, pA, pU, pMultiplier);
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
               status_opt = DSPLIB_cholesky_exec(handle, pA, pU, pMultiplier); // calling c7x
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
#if !defined(__C7X_HOSTEM__)
               /* The following for loop is to call kernel repeatedly so as to
                * train the branch predictor
                */
               uint32_t k;
               for (k = 0; k < 4; k++) {
                  // run warm instruction cache test
                  TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
                  DSPLIB_asm(" MARK 4");

                  status_opt = DSPLIB_cholesky_exec(handle, pA, pU, pMultiplier);

                  DSPLIB_asm(" MARK 5");
                  TI_profile_stop();
               }

               // get output to L1D
               int16_t outSumL   = 0;
               int8_t *pOutTempL = (int8_t *) pU; // treat output as bytes to be data type agnostic

               for (k = 0; k < bufParamsU.dim_x * bufParamsU.dim_y; k++) {
                  outSumL += *pOutTempL;
                  pOutTempL++;
               }

               // dummy store of outSum to insure that the compiler does not remove it.
               volatileSum = outSumL;

               // run warm instruction cache test
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

               DSPLIB_asm(" MARK 6");
               status_opt = DSPLIB_cholesky_exec(handle, pA, pU, pMultiplier);
               DSPLIB_asm(" MARK 7");
               TI_profile_stop();
#endif // #if (!defined(EVM_TEST))
            }

            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC; // Natural C implementation
            status_init           = DSPLIB_cholesky_init_checkParams(handle, &bufParamsA, &bufParamsU, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 0");
               status_init = DSPLIB_cholesky_init(handle, &bufParamsA, &bufParamsU, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();

               if (status_init == DSPLIB_SUCCESS) {
                  status_nat = DSPLIB_cholesky_exec_checkParams(handle, pA, pUCn, pMultiplier);
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
               DSPLIB_asm(" MARK 2");
               status_nat = DSPLIB_cholesky_exec(handle, pA, pUCn, pMultiplier); // calling c7x
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
            }

            int32_t statusCompareL    = TI_TEST_KERNEL_PASS;
            int32_t statusCompareLRef = TI_TEST_KERNEL_PASS;
            comparisonDone            = 0;

            if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {
               if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
                  statusCompareL = TI_compare_mem_2D_upper_triangle(
                      (void *) pU, (void *) pUCn, 0.001, (double) powf(2, -10), bufParamsU.dim_x, bufParamsU.stride_y,
                      kerInitArgs.strideCn, dataWidth, dataWidth);

                  comparisonDone = 1;
               }

               if (currPrm.staticOut != NULL) {
                  statusCompareLRef = TI_compare_mem_2D_upper_triangle(
                      (void *) pU, (void *) currPrm.staticOut, 0.001, (double) powf(2, -10), bufParamsU.dim_x,
                      bufParamsU.stride_y, bufParamsU.dim_x * DSPLIB_sizeof(currPrm.dataType), dataWidth, dataWidth);
               }
            }

            /* Set the 'fail' flag based on test vector comparison results */
            DSPLIB_DEBUGPRINTFN(0, "statusCompareL: %d statusCompareLRef: %d\n", statusCompareL, statusCompareLRef);
            DSPLIB_DEBUGPRINTFN(0, "status_init: %d status_opt: %d status_nat: %d\n", status_init, status_opt,
                                status_nat);
            DSPLIB_DEBUGPRINTFN(0, "comparisonDone: %d\n", comparisonDone);

            currentTestFail = ((statusCompareL == TI_TEST_KERNEL_FAIL) || (statusCompareLRef == TI_TEST_KERNEL_FAIL) ||
                               (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) ||
                               (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0))
                                  ? 1
                                  : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
            DSPLIB_DEBUGPRINTFN(0, "fail: %d, currentTestFail: %d\n", fail, currentTestFail);

            pProfile[12 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[12 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[12 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "Cholesky: %s input | enableTest = %d Order = %d DataType = %d", testPatternString,
                    currPrm.enableTest, currPrm.order, currPrm.dataType);

            archCycles = pProfile[12 * tpi + 1];
#if (!defined(EVM_TEST))
            lokiCycles = archCycles;
            // write to CSV, must happen prior to write to screen because
            fprintf(fpOutputCSV, "CHOLESKY, %d, %d, %d, %d, %d\n", testNum, currPrm.testPatternA,
                    currPrm.order * currPrm.order, (int) lokiCycles, !currentTestFail);
#endif // #if (!defined(EVM_TEST))

            TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_x, archCycles, 0, currentTestFail, desc);

            /**************************************************************************
             *                   Cholesky Solver
             ***************************************************************************/
            /* TEST OPTIMIZED KERNEL */
            kerInitArgsSolver.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init =
                DSPLIB_cholesky_solver_init_checkParams(handleSolver, &bufParamsU, &bufParamsUT, &bufParamsY,
                                                        &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);

            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 12");

               status_init = DSPLIB_cholesky_solver_init(handleSolver, &bufParamsU, &bufParamsUT, &bufParamsY,
                                                         &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);
               DSPLIB_asm(" MARK 13");
               TI_profile_stop();

               if (status_init == DSPLIB_SUCCESS) {
                  status_opt = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, pU, pUT, pY, pB, pX, pDiv);
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
               status_opt = DSPLIB_cholesky_solver_exec(handleSolver, pU, pUT, pY, pB, pX, pDiv);

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
                  status_opt = DSPLIB_cholesky_solver_exec(handleSolver, pU, pUT, pY, pB, pX, pDiv);
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
               status_opt = DSPLIB_cholesky_solver_exec(handleSolver, pU, pUT, pY, pB, pX, pDiv);
               DSPLIB_asm(" MARK 19");
               TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)
            }

            /* Test _cn kernel */
            kerInitArgsSolver.funcStyle = DSPLIB_FUNCTION_NATC;

            status_init =
                DSPLIB_cholesky_solver_init_checkParams(handleSolver, &bufParamsU, &bufParamsUT, &bufParamsY,
                                                        &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 20");

               status_init = DSPLIB_cholesky_solver_init(handleSolver, &bufParamsU, &bufParamsUT, &bufParamsY,
                                                         &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);
               DSPLIB_asm(" MARK 21");
               TI_profile_stop();

               if (status_init == DSPLIB_SUCCESS) {
                  status_nat = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, pUCn, pUT, pY, pB, pXCn, pDiv);
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
               DSPLIB_asm(" MARK 22");
               status_nat = DSPLIB_cholesky_solver_exec(handleSolver, pUCn, pUT, pY, pB, pXCn, pDiv);
               DSPLIB_asm(" MARK 23");
               TI_profile_stop();
            }

            int32_t statusSolvOpt     = TI_TEST_KERNEL_PASS;
            int32_t statusSolvCn      = TI_TEST_KERNEL_PASS;
            int32_t statusCompareX    = TI_TEST_KERNEL_PASS;
            int32_t statusCompareXRef = TI_TEST_KERNEL_PASS;
            comparisonDone            = 0;

            if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {

               statusSolvOpt = cholesky_check_solution(currPrm.testID, currPrm.dataType, currPrm.order, pTemp, pX, pB,
                                                       "OPT: ", bufParamsA.stride_y / dataWidth);

               statusSolvCn = cholesky_check_solution(currPrm.testID, currPrm.dataType, currPrm.order, pTemp, pXCn, pB,
                                                      "NATC: ", bufParamsA.stride_y / dataWidth);
               DSPLIB_DEBUGPRINTFN(0, "statusSolvOpt: %d statusSolvCn: %d\n", statusSolvOpt, statusSolvCn);

               if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
                  statusCompareX = TI_compare_mem_2D_float((void *) pX, (void *) pXCn, 0.01, (double) powf(2, -10),
                                                           currPrm.order, 1, 0, DSPLIB_sizeof(currPrm.dataType));
                  comparisonDone = 1;
               }

               if (currPrm.staticOut != NULL) {
                  statusCompareXRef =
                      TI_compare_mem_2D_float((void *) pX, currPrm.staticOutX, 0.01, (double) powf(2, -10),
                                              currPrm.order, 1, 0, DSPLIB_sizeof(currPrm.dataType));
               }
            }

            DSPLIB_DEBUGPRINTFN(0, "statusCompareX: %d statusCompareXRef: %d\n", statusCompareX, statusCompareXRef);
            DSPLIB_DEBUGPRINTFN(0, "status_init: %d status_opt: %d status_nat: %d\n", status_init, status_opt,
                                status_nat);
            DSPLIB_DEBUGPRINTFN(0, "comparisonDone: %d\n", comparisonDone);

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail = ((statusSolvCn == TI_TEST_KERNEL_FAIL) || (statusSolvOpt == TI_TEST_KERNEL_FAIL) ||
                               (statusCompareX == TI_TEST_KERNEL_FAIL) || (statusCompareXRef == TI_TEST_KERNEL_FAIL) ||
                               (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) ||
                               (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0))
                                  ? 1
                                  : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
            DSPLIB_DEBUGPRINTFN(0, "fail: %d, currentTestFail: %d\n", fail, currentTestFail);

            pProfile[12 * tpi + 3] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[12 * tpi + 4] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[12 * tpi + 5] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "Cholesky Solver: %s input | enableTest = %d Order = %d DataType = %d", testPatternString,
                    currPrm.enableTest, currPrm.order, currPrm.dataType);
            archCycles = pProfile[12 * tpi + 4];

#if (!defined(EVM_TEST))
            lokiCycles = archCycles;
            // write to CSV, must happen prior to write to screen because
            fprintf(fpOutputCSV, "CHOLESKY SOLVER, %d, %d, %d, %d, %d\n", testNum, currPrm.testPatternA,
                    currPrm.order * currPrm.order, (int) lokiCycles, !currentTestFail);

#endif // #if (!defined(EVM_TEST))

            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail, desc);

            /******************************************************************************************
             *                   Cholesky Inplace Decomposition
             *******************************************************************************************/
            /* TEST OPTIMIZED KERNEL */
            kerInitArgsInplace.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init = DSPLIB_cholesky_inplace_init_checkParams(handleInplace, &bufParamsA, &bufParamsMul,
                                                                   &kerInitArgsInplace);

            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 12");

               status_init =
                   DSPLIB_cholesky_inplace_init(handleInplace, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);
               DSPLIB_asm(" MARK 13");
               TI_profile_stop();
               if (status_init == DSPLIB_SUCCESS) {
                  status_opt = DSPLIB_cholesky_inplace_exec_checkParams(handleInplace, pA, pMultiplier);
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
               status_opt = DSPLIB_cholesky_inplace_exec(handleInplace, pA, pMultiplier);

               DSPLIB_asm(" MARK 15");
               TI_profile_stop();
#if !defined(__C7X_HOSTEM__)
               /* The following for loop is to call kernel repeatedly so as to
                * train the branch predictor                                   */
               uint32_t k;
               for (k = 0; k < 4; k++) {
                  memcpy(pA, pTemp, bufParamsA.dim_y * bufParamsA.stride_y);
                  // run warm instruction cache test
                  TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
                  DSPLIB_asm(" MARK 16");
                  status_opt = DSPLIB_cholesky_inplace_exec(handleInplace, pA, pMultiplier);
                  DSPLIB_asm(" MARK 17");
                  TI_profile_stop();
               }

               // get output to L1D
               int16_t outSum   = 0;
               int8_t *pOutTemp = (int8_t *) pA; // treat output as bytes to be data type agnostic
               for (k = 0; k < bufParamsA.dim_x * bufParamsA.dim_y; k++) {
                  outSum += *pOutTemp;
                  pOutTemp++;
               }

               // dummy store of outSum to insure that the compiler does not remove
               // it.
               volatileSum = outSum;

               memcpy(pA, pTemp, bufParamsA.dim_y * bufParamsA.stride_y);
               // run warm instruction cache test
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);
               DSPLIB_asm(" MARK 18");
               status_opt = DSPLIB_cholesky_inplace_exec(handleInplace, pA, pMultiplier);
               DSPLIB_asm(" MARK 19");
               TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)
            }

            /* Test _cn kernel */
            memcpy(pACn, pTemp, aSize);
            kerInitArgsInplace.funcStyle = DSPLIB_FUNCTION_NATC;
            status_init = DSPLIB_cholesky_inplace_init_checkParams(handleInplace, &bufParamsA, &bufParamsMul,
                                                                   &kerInitArgsInplace);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 20");

               status_init =
                   DSPLIB_cholesky_inplace_init(handleInplace, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);
               DSPLIB_asm(" MARK 21");
               TI_profile_stop();

               if (status_init == DSPLIB_SUCCESS) {
                  status_nat = DSPLIB_cholesky_inplace_exec_checkParams(handleInplace, pACn, pMultiplier);
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
               DSPLIB_asm(" MARK 22");
               status_nat = DSPLIB_cholesky_inplace_exec(handleInplace, pACn, pMultiplier);
               DSPLIB_asm(" MARK 23");
               TI_profile_stop();
            }

            /* ***********************************************************************************/
            int32_t statusCompareA    = TI_TEST_KERNEL_FAIL;
            int32_t statusCompareARef = TI_TEST_KERNEL_PASS;
            comparisonDone            = 0;

            if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {
               if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
                  statusCompareA = TI_compare_mem_2D_upper_triangle(
                      (void *) pA, (void *) pACn, 0.001, (double) powf(2, -10), bufParamsA.dim_x, bufParamsA.stride_y,
                      bufParamsA.stride_y, dataWidth, dataWidth);
                  comparisonDone = 1;
               }

               if (currPrm.staticOut != NULL) {
                  statusCompareARef = TI_compare_mem_2D_upper_triangle(
                      (void *) pA, (void *) currPrm.staticOut, 0.001, (double) powf(2, -10), bufParamsA.dim_x,
                      bufParamsA.stride_y, bufParamsA.dim_x * DSPLIB_sizeof(currPrm.dataType), dataWidth, dataWidth);
               }
            }

            /* Set the 'fail' flag based on test vector comparison results */
            DSPLIB_DEBUGPRINTFN(0, "statusCompareA: %d statusCompareARef: %d\n", statusCompareA, statusCompareARef);
            DSPLIB_DEBUGPRINTFN(0, "status_init: %d status_opt: %d status_nat: %d\n", status_init, status_opt,
                                status_nat);
            DSPLIB_DEBUGPRINTFN(0, "comparisonDone: %d\n", comparisonDone);

            currentTestFail = ((statusCompareA == TI_TEST_KERNEL_FAIL) || (statusCompareARef == TI_TEST_KERNEL_FAIL) ||
                               (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) ||
                               (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0))
                                  ? 1
                                  : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
            DSPLIB_DEBUGPRINTFN(0, "fail: %d, currentTestFail: %d\n", fail, currentTestFail);

            pProfile[12 * tpi + 6] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[12 * tpi + 7] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[12 * tpi + 8] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "Cholesky Inplace: %s input | enableTest = %d Order = %d DataType = %d", testPatternString,
                    currPrm.enableTest, currPrm.order, currPrm.dataType);
            archCycles = pProfile[12 * tpi + 7];

#if (!defined(EVM_TEST))
            lokiCycles = archCycles;
            // write to CSV, must happen prior to write to screen because
            fprintf(fpOutputCSV, "CHOLESKY INPLACE, %d, %d, %d, %d, %d\n", testNum, currPrm.testPatternA,
                    currPrm.order * currPrm.order, (int) lokiCycles, !currentTestFail);

#endif // #if (!defined(EVM_TEST))

            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail, desc);

            /**************************************************************************
             *                   Cholesky Solver for Inplace
             ***************************************************************************/
            /* TEST OPTIMIZED KERNEL */
            kerInitArgsSolver.strideCn  = kerInitArgsSolver.stride;
            kerInitArgsSolver.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init =
                DSPLIB_cholesky_solver_init_checkParams(handleSolver, &bufParamsA, &bufParamsUT, &bufParamsY,
                                                        &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);

            void *pAT = pUT;
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 12");

               status_init = DSPLIB_cholesky_solver_init(handleSolver, &bufParamsA, &bufParamsUT, &bufParamsY,
                                                         &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);
               DSPLIB_asm(" MARK 13");
               TI_profile_stop();

               if (status_init == DSPLIB_SUCCESS) {
                  status_opt = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, pA, pAT, pY, pB, pX, pDiv);
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
               status_opt = DSPLIB_cholesky_solver_exec(handleSolver, pA, pAT, pY, pB, pX, pDiv);

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
                  status_opt = DSPLIB_cholesky_solver_exec(handleSolver, pA, pAT, pY, pB, pX, pDiv);
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
               status_opt = DSPLIB_cholesky_solver_exec(handleSolver, pU, pAT, pY, pB, pX, pDiv);
               DSPLIB_asm(" MARK 19");
               TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)
            }

            /* Test _cn kernel */
            kerInitArgsSolver.funcStyle = DSPLIB_FUNCTION_NATC;

            status_init =
                DSPLIB_cholesky_solver_init_checkParams(handleSolver, &bufParamsA, &bufParamsUT, &bufParamsY,
                                                        &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 20");

               status_init = DSPLIB_cholesky_solver_init(handleSolver, &bufParamsA, &bufParamsUT, &bufParamsY,
                                                         &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);
               DSPLIB_asm(" MARK 21");
               TI_profile_stop();

               if (status_init == DSPLIB_SUCCESS) {
                  status_nat = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, pACn, pAT, pY, pB, pXCn, pDiv);
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
               DSPLIB_asm(" MARK 22");
               status_nat = DSPLIB_cholesky_solver_exec(handleSolver, pACn, pAT, pY, pB, pXCn, pDiv);
               DSPLIB_asm(" MARK 23");
               TI_profile_stop();
            }

            /* ***********************************************************************************/
            statusSolvOpt     = TI_TEST_KERNEL_PASS;
            statusSolvCn      = TI_TEST_KERNEL_PASS;
            statusCompareX    = TI_TEST_KERNEL_PASS;
            statusCompareXRef = TI_TEST_KERNEL_PASS;
            comparisonDone    = 0;

            if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {
               statusSolvOpt = cholesky_check_solution(currPrm.testID, currPrm.dataType, currPrm.order, pTemp, pX, pB,
                                                       "OPT: ", bufParamsA.stride_y / dataWidth);

               statusSolvCn = cholesky_check_solution(currPrm.testID, currPrm.dataType, currPrm.order, pTemp, pXCn, pB,
                                                      "NATC: ", bufParamsA.stride_y / dataWidth);
               DSPLIB_DEBUGPRINTFN(0, "statusSolvOpt: %d statusSolvCn: %d\n", statusSolvOpt, statusSolvCn);
               if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
                  statusCompareX = TI_compare_mem_2D_float((void *) pX, (void *) pXCn, 0.01, (double) powf(2, -10),
                                                           currPrm.order, 1, 0, DSPLIB_sizeof(currPrm.dataType));
                  comparisonDone = 1;
               }

               if (currPrm.staticOut != NULL) {
                  statusCompareXRef =
                      TI_compare_mem_2D_float((void *) pX, currPrm.staticOutX, 0.01, (double) powf(2, -10),
                                              currPrm.order, 1, 0, DSPLIB_sizeof(currPrm.dataType));
               }
            }

            DSPLIB_DEBUGPRINTFN(0, "statusCompareX: %d statusCompareXRef: %d\n", statusCompareX, statusCompareXRef);
            DSPLIB_DEBUGPRINTFN(0, "status_init: %d status_opt: %d status_nat: %d\n", status_init, status_opt,
                                status_nat);
            DSPLIB_DEBUGPRINTFN(0, "comparisonDone: %d\n", comparisonDone);

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail = ((statusSolvCn == TI_TEST_KERNEL_FAIL) || (statusSolvOpt == TI_TEST_KERNEL_FAIL) ||
                               (statusCompareX == TI_TEST_KERNEL_FAIL) || (statusCompareXRef == TI_TEST_KERNEL_FAIL) ||
                               (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) ||
                               (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0))
                                  ? 1
                                  : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
            DSPLIB_DEBUGPRINTFN(0, "fail: %d, currentTestFail: %d\n", fail, currentTestFail);

            pProfile[12 * tpi + 9]  = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[12 * tpi + 10] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[12 * tpi + 11] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "Cholesky Solver: %s input | enableTest = %d Order = %d DataType = %d", testPatternString,
                    currPrm.enableTest, currPrm.order, currPrm.dataType);
            archCycles = pProfile[12 * tpi + 10];

#if (!defined(EVM_TEST))
            lokiCycles = archCycles;
            // write to CSV, must happen prior to write to screen because
            fprintf(fpOutputCSV, "CHOLESKY SOLVER, %d, %d, %d, %d, %d\n", testNum, currPrm.testPatternA,
                    currPrm.order * currPrm.order, (int) lokiCycles, !currentTestFail);

#endif // #if (!defined(EVM_TEST))

            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail, desc);
         }
         else {
            sprintf(desc, "Data size=%d", currPrm.order * currPrm.order);
            TI_profile_skip_test(desc);
            TI_profile_clear_run_stats();
         } // end of memory allocation successful?

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pUCn);
            free(pXCn);
            TI_align_free(pU);
            free(pACn);
            free(pTemp);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
            TI_align_free(pU);
         }
         else {
            TI_align_free(pU);
         }

         TI_align_free(pMultiplier);
         TI_align_free(pA);
         TI_align_free(pUT);
         TI_align_free(pY);
         TI_align_free(pB);
         TI_align_free(pX);
         TI_align_free(pDiv);
      } // end repetitions
   }    // end idat test cases

   free(handle);
   free(handleInplace);
   free(handleSolver);

#if (!defined(EVM_TEST))
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif // #if (!defined(EVM_TEST))

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_cholesky");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_cholesky_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t testNum           = 1001;
   int32_t currentTestFail   = 0;
   int32_t fail              = 0;
   int32_t status_init       = 0;
   int32_t status_nat        = 0;
   int32_t status_opt        = 0;
   int32_t status_init_nat   = 0;
   int32_t status_init_opt   = 0;
   int32_t status_init_check = 0;
   int32_t status_exec_check = 0;

   DSPLIB_bufParams2D_t bufParamsA, bufParamsU, bufParamsUT;
   DSPLIB_bufParams1D_t bufParamsY, bufParamsB, bufParamsX, bufParamsDiv, bufParamsMul;

   DSPLIB_cholesky_InitArgs kerInitArgs;
   int32_t                  handleSize = DSPLIB_cholesky_getHandleSize(&kerInitArgs); // bufpblock size...
   DSPLIB_kernelHandle      handle     = malloc(handleSize);

   DSPLIB_cholesky_solver_InitArgs kerInitArgsSolver;
   int32_t                         handleSolverSize = DSPLIB_cholesky_solver_getHandleSize(&kerInitArgsSolver);
   DSPLIB_kernelHandle             handleSolver     = malloc(handleSolverSize);

   DSPLIB_cholesky_inplace_InitArgs kerInitArgsInplace;
   int32_t                          handleInplaceSize = DSPLIB_cholesky_inplace_getHandleSize(&kerInitArgsInplace);
   DSPLIB_kernelHandle              handleInplace     = malloc(handleInplaceSize);

   /* CASE 1001 - Covering Null pointers */
   {
      status_init_check = DSPLIB_cholesky_init_checkParams(NULL, &bufParamsA, &bufParamsU, &kerInitArgs);
      status_exec_check = DSPLIB_cholesky_exec_checkParams(handle, NULL, NULL, NULL);
      currentTestFail =
          ((status_init_check != DSPLIB_ERR_NULL_POINTER) || (status_exec_check != DSPLIB_ERR_NULL_POINTER)) ? 1 : 0;
      fail = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      status_init_check =
          DSPLIB_cholesky_inplace_init_checkParams(NULL, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);
      status_exec_check = DSPLIB_cholesky_inplace_exec_checkParams(handleInplace, NULL, NULL);
      currentTestFail =
          ((status_init_check != DSPLIB_ERR_NULL_POINTER) || (status_exec_check != DSPLIB_ERR_NULL_POINTER)) ? 1 : 0;
      fail = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      status_init_check = DSPLIB_cholesky_solver_init_checkParams(
          NULL, &bufParamsU, &bufParamsUT, &bufParamsY, &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);
      status_exec_check = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, NULL, NULL, NULL, NULL, NULL, NULL);

      currentTestFail =
          ((status_init_check != DSPLIB_ERR_NULL_POINTER) || (status_exec_check != DSPLIB_ERR_NULL_POINTER)) ? 1 : 0;
      fail = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      testNum++;
   }

   {
      void *pA   = malloc(4);
      void *pOut = malloc(4);
      void *pMul = malloc(4);
      /* CASE 1002 - Covering Null pointers */
      status_exec_check = DSPLIB_cholesky_exec_checkParams(handle, NULL, pOut, pMul);
      currentTestFail   = (status_exec_check != DSPLIB_ERR_NULL_POINTER) ? 1 : 0;
      fail              = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1003 - Covering Null pointers */
      status_exec_check = DSPLIB_cholesky_exec_checkParams(handle, pA, NULL, pMul);
      currentTestFail   = (status_exec_check != DSPLIB_ERR_NULL_POINTER) ? 1 : 0;
      fail              = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1004 - Covering Null pointers */
      status_exec_check = DSPLIB_cholesky_exec_checkParams(handle, pA, pOut, NULL);
      currentTestFail   = (status_exec_check != DSPLIB_ERR_NULL_POINTER) ? 1 : 0;
      fail              = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      free(pA);
      free(pOut);
      free(pMul);

      void *pU       = malloc(4);
      void *pScratch = malloc(4);
      void *pY       = malloc(4);
      void *pB       = malloc(4);
      void *pX       = malloc(4);
      void *pDiv     = malloc(4);
      /* CASE 1005 - Covering Null pointers */
      status_exec_check = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, NULL, pScratch, pY, pB, pX, pDiv);
      currentTestFail   = (status_exec_check != DSPLIB_ERR_NULL_POINTER) ? 1 : 0;
      fail              = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1006 - Covering Null pointers */
      status_exec_check = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, pU, NULL, pY, pB, pX, pDiv);
      currentTestFail   = (status_exec_check != DSPLIB_ERR_NULL_POINTER) ? 1 : 0;
      fail              = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1007 - Covering Null pointers */
      status_exec_check = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, pU, pScratch, NULL, pB, pX, pDiv);
      currentTestFail   = (status_exec_check != DSPLIB_ERR_NULL_POINTER) ? 1 : 0;
      fail              = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1008 - Covering Null pointers */
      status_exec_check = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, pU, pScratch, pY, NULL, pX, pDiv);
      currentTestFail   = (status_exec_check != DSPLIB_ERR_NULL_POINTER) ? 1 : 0;
      fail              = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1009 - Covering Null pointers */
      status_exec_check = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, pU, pScratch, pY, pB, NULL, pDiv);
      currentTestFail   = (status_exec_check != DSPLIB_ERR_NULL_POINTER) ? 1 : 0;
      fail              = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1010 - Covering Null pointers */
      status_exec_check = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, pU, pScratch, pY, pB, pX, NULL);
      currentTestFail   = (status_exec_check != DSPLIB_ERR_NULL_POINTER) ? 1 : 0;
      fail              = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      free(pU);
      free(pScratch);
      free(pY);
      free(pB);
      free(pX);
      free(pDiv);
   }

   /* CASE 1011 - Covering Input Invalid Data Type */
   {
      bufParamsA.data_type  = DSPLIB_INT32;
      bufParamsU.data_type  = DSPLIB_INT32;
      bufParamsUT.data_type = DSPLIB_INT32;

      status_init_check     = DSPLIB_cholesky_init_checkParams(handle, &bufParamsA, &bufParamsU, &kerInitArgs);
      kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
      status_init_nat       = DSPLIB_cholesky_init(handle, &bufParamsA, &bufParamsU, &kerInitArgs);
      kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
      status_init_opt       = DSPLIB_cholesky_init(handle, &bufParamsA, &bufParamsU, &kerInitArgs);

      currentTestFail = ((status_init_check != DSPLIB_ERR_INVALID_TYPE) ||
                         (status_init_nat != DSPLIB_ERR_INVALID_TYPE) || (status_init_opt != DSPLIB_ERR_INVALID_TYPE))
                            ? 1
                            : 0;
      fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      status_init_check =
          DSPLIB_cholesky_inplace_init_checkParams(handle, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);
      kerInitArgsInplace.funcStyle = DSPLIB_FUNCTION_NATC;
      status_init_nat = DSPLIB_cholesky_inplace_init(handle, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);
      kerInitArgsInplace.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
      status_init_opt = DSPLIB_cholesky_inplace_init(handle, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);

      currentTestFail = ((status_init_check != DSPLIB_ERR_INVALID_TYPE) ||
                         (status_init_nat != DSPLIB_ERR_INVALID_TYPE) || (status_init_opt != DSPLIB_ERR_INVALID_TYPE))
                            ? 1
                            : 0;
      fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      status_init_check = DSPLIB_cholesky_solver_init_checkParams(
          handle, &bufParamsU, &bufParamsUT, &bufParamsY, &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);
      kerInitArgsSolver.funcStyle = DSPLIB_FUNCTION_NATC;
      status_init_nat = DSPLIB_cholesky_solver_init(handle, &bufParamsU, &bufParamsUT, &bufParamsY, &bufParamsB,
                                                    &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);
      kerInitArgsSolver.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
      status_init_opt = DSPLIB_cholesky_solver_init(handle, &bufParamsU, &bufParamsUT, &bufParamsY, &bufParamsB,
                                                    &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);

      currentTestFail = ((status_init_check != DSPLIB_ERR_INVALID_TYPE) ||
                         (status_init_nat != DSPLIB_ERR_INVALID_TYPE) || (status_init_opt != DSPLIB_ERR_INVALID_TYPE))
                            ? 1
                            : 0;
      fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      testNum++;
   }

   {
      /* CASE 1012 - Covering Output Invalid Data Type */
      bufParamsA.data_type = DSPLIB_FLOAT32;
      bufParamsU.data_type = DSPLIB_INT32;
      status_init_check    = DSPLIB_cholesky_init_checkParams(handle, &bufParamsA, &bufParamsU, &kerInitArgs);
      currentTestFail      = (status_init_check != DSPLIB_ERR_INVALID_TYPE) ? 1 : 0;
      fail                 = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1013 - Covering Output Invalid Data Type */
      bufParamsU.data_type   = DSPLIB_FLOAT32;
      bufParamsUT.data_type  = DSPLIB_FLOAT32;
      bufParamsY.data_type   = DSPLIB_INT32;
      bufParamsB.data_type   = DSPLIB_FLOAT32;
      bufParamsX.data_type   = DSPLIB_FLOAT32;
      bufParamsDiv.data_type = DSPLIB_FLOAT32;

      status_init_check = DSPLIB_cholesky_solver_init_checkParams(
          handle, &bufParamsU, &bufParamsUT, &bufParamsY, &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);

      /* CASE 1014 - Covering Output Invalid Data Type */
      currentTestFail = (status_init_check != DSPLIB_ERR_INVALID_TYPE) ? 1 : 0;
      fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      bufParamsU.data_type   = DSPLIB_FLOAT32;
      bufParamsUT.data_type  = DSPLIB_FLOAT32;
      bufParamsY.data_type   = DSPLIB_FLOAT32;
      bufParamsB.data_type   = DSPLIB_INT32;
      bufParamsX.data_type   = DSPLIB_FLOAT32;
      bufParamsDiv.data_type = DSPLIB_FLOAT32;

      status_init_check = DSPLIB_cholesky_solver_init_checkParams(
          handle, &bufParamsU, &bufParamsUT, &bufParamsY, &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);

      currentTestFail = (status_init_check != DSPLIB_ERR_INVALID_TYPE) ? 1 : 0;
      fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1015 - Covering Output Invalid Data Type */
      bufParamsU.data_type   = DSPLIB_FLOAT32;
      bufParamsUT.data_type  = DSPLIB_FLOAT32;
      bufParamsY.data_type   = DSPLIB_FLOAT32;
      bufParamsB.data_type   = DSPLIB_FLOAT32;
      bufParamsX.data_type   = DSPLIB_INT32;
      bufParamsDiv.data_type = DSPLIB_FLOAT32;

      status_init_check = DSPLIB_cholesky_solver_init_checkParams(
          handle, &bufParamsU, &bufParamsUT, &bufParamsY, &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);

      currentTestFail = (status_init_check != DSPLIB_ERR_INVALID_TYPE) ? 1 : 0;
      fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;

      /* CASE 1016 - Covering Output Invalid Data Type */
      bufParamsU.data_type   = DSPLIB_FLOAT32;
      bufParamsUT.data_type  = DSPLIB_FLOAT32;
      bufParamsY.data_type   = DSPLIB_FLOAT32;
      bufParamsB.data_type   = DSPLIB_FLOAT32;
      bufParamsX.data_type   = DSPLIB_FLOAT32;
      bufParamsDiv.data_type = DSPLIB_INT32;

      status_init_check = DSPLIB_cholesky_solver_init_checkParams(
          handle, &bufParamsU, &bufParamsUT, &bufParamsY, &bufParamsB, &bufParamsX, &bufParamsDiv, &kerInitArgsSolver);

      currentTestFail = (status_init_check != DSPLIB_ERR_INVALID_TYPE) ? 1 : 0;
      fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
      testNum++;
   }

   /* CASE 1017 - Covering Dimension Test */
   {
      bufParamsA.data_type = DSPLIB_FLOAT32;
      bufParamsA.dim_x     = 1;
      bufParamsA.dim_y     = 2;

      status_init_check =
          DSPLIB_cholesky_inplace_init_checkParams(handle, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);
      currentTestFail = (status_init_check != DSPLIB_ERR_INVALID_DIMENSION) ? 1 : 0;
      fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      bufParamsA.dim_x   = 2;
      bufParamsA.dim_y   = 2;
      bufParamsMul.dim_x = 1;

      status_init_check =
          DSPLIB_cholesky_inplace_init_checkParams(handle, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);
      currentTestFail = (status_init_check != DSPLIB_ERR_INVALID_DIMENSION) ? 1 : 0;
      fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      testNum++;
   }

   /* CASE 1018 - Covering Non positive definite condition Test */
   {
      bufParamsA.data_type   = DSPLIB_FLOAT32;
      bufParamsU.data_type   = DSPLIB_FLOAT32;
      bufParamsMul.data_type = DSPLIB_FLOAT32;

      bufParamsA.dim_x = bufParamsU.dim_x = bufParamsMul.dim_x = 2;
      bufParamsA.dim_y = bufParamsU.dim_y = 2;

      void *pA   = (void *) malloc(bufParamsA.dim_x * bufParamsA.dim_y * DSPLIB_sizeof(bufParamsA.data_type));
      void *pU   = (void *) malloc(bufParamsU.dim_x * bufParamsU.dim_y * DSPLIB_sizeof(bufParamsU.data_type));
      void *pMul = (void *) malloc(bufParamsMul.dim_x * DSPLIB_sizeof(bufParamsMul.data_type));

      float *pAFill = (float *) pA;
      pAFill[0]     = -1;
      pAFill[1]     = -1;
      pAFill[2]     = -1;
      pAFill[3]     = -1;

      kerInitArgs.funcStyle  = DSPLIB_FUNCTION_NATC;
      kerInitArgs.order      = 2;
      kerInitArgs.stride     = 2;
      kerInitArgs.strideCn   = 2;
      kerInitArgs.enableTest = 1;

      status_init = DSPLIB_cholesky_init_checkParams(handle, &bufParamsA, &bufParamsU, &kerInitArgs);

      if (status_init == DSPLIB_SUCCESS) {
         status_init = DSPLIB_cholesky_init(handle, &bufParamsA, &bufParamsU, &kerInitArgs);
         if (status_init == DSPLIB_SUCCESS) {
            status_nat = DSPLIB_cholesky_exec_checkParams(handle, pA, pU, pMul);
         }
         else {
            status_nat = status_init;
         }
      }
      else {
         status_nat = status_init;
      }

      if (status_nat == DSPLIB_SUCCESS) {
         status_nat = DSPLIB_cholesky_exec(handle, pA, pU, pMul);
      }

      kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
      status_init           = DSPLIB_cholesky_init_checkParams(handle, &bufParamsA, &bufParamsU, &kerInitArgs);

      if (status_init == DSPLIB_SUCCESS) {
         status_init = DSPLIB_cholesky_init(handle, &bufParamsA, &bufParamsU, &kerInitArgs);
         if (status_init == DSPLIB_SUCCESS) {
            status_opt = DSPLIB_cholesky_exec_checkParams(handle, pA, pU, pMul);
         }
         else {
            status_opt = status_init;
         }
      }
      else {
         status_opt = status_init;
      }

      if (status_opt == DSPLIB_SUCCESS) {
         status_opt = DSPLIB_cholesky_exec(handle, pA, pU, pMul);
      }

      currentTestFail =
          ((status_init != DSPLIB_SUCCESS) || (status_nat != DSPLIB_ERR_FAILURE) || (status_opt != DSPLIB_ERR_FAILURE))
              ? 1
              : 0;
      fail = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      kerInitArgsInplace.funcStyle  = DSPLIB_FUNCTION_NATC;
      kerInitArgsInplace.order      = 2;
      kerInitArgsInplace.stride     = 2;
      kerInitArgsInplace.enableTest = 1;

      status_init = DSPLIB_cholesky_inplace_init_checkParams(handle, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);

      if (status_init == DSPLIB_SUCCESS) {
         status_init = DSPLIB_cholesky_inplace_init(handle, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);
         if (status_init == DSPLIB_SUCCESS) {
            status_nat = DSPLIB_cholesky_inplace_exec_checkParams(handle, pA, pMul);
         }
         else {
            status_nat = status_init;
         }
      }
      else {
         status_nat = status_init;
      }

      if (status_nat == DSPLIB_SUCCESS) {
         status_nat = DSPLIB_cholesky_inplace_exec(handle, pA, pMul);
      }

      kerInitArgsInplace.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
      status_init = DSPLIB_cholesky_inplace_init_checkParams(handle, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);

      if (status_init == DSPLIB_SUCCESS) {
         status_init = DSPLIB_cholesky_inplace_init(handle, &bufParamsA, &bufParamsMul, &kerInitArgsInplace);
         if (status_init == DSPLIB_SUCCESS) {
            status_opt = DSPLIB_cholesky_inplace_exec_checkParams(handle, pA, pMul);
         }
         else {
            status_opt = status_init;
         }
      }
      else {
         status_opt = status_init;
      }

      if (status_opt == DSPLIB_SUCCESS) {
         status_opt = DSPLIB_cholesky_inplace_exec(handle, pA, pMul);
      }

      currentTestFail =
          ((status_init != DSPLIB_SUCCESS) || (status_nat != DSPLIB_ERR_FAILURE) || (status_opt != DSPLIB_ERR_FAILURE))
              ? 1
              : 0;
      fail = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);

      free(pA);
      free(pU);
      free(pMul);
      testNum++;
   }

   free(handle);
   free(handleInplace);
   free(handleSolver);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_cholesky_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_cholesky_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_cholesky_getHandleSize(&kerInitArgs);
   printf("!!! Pushing final execution history. handleSize_LDRA: %d\n", handleSize_LDRA);
#endif

   return fail;
}

/* Main call for inaddidual test projects */
#if !defined(__ONESHOTTEST)
int main()
{
   int      fail              = 1;
   uint32_t profile[256 * 12] = {0};
   DSPLIB_TEST_init();

   fail = test_main(&profile[0]);

#if !defined(NO_PRINTF)
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail! %d Tests Failed!\n", fail);

   int i;
   for (i = 0; i < test_cases; i++) {
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", 4 * i, profile[12 * i + 0],
             profile[12 * i + 1], profile[12 * i + 2]);
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", 4 * i + 1, profile[12 * i + 3],
             profile[12 * i + 4], profile[12 * i + 5]);
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", 4 * i + 2, profile[12 * i + 6],
             profile[12 * i + 7], profile[12 * i + 8]);
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", 4 * i + 3, profile[12 * i + 9],
             profile[12 * i + 10], profile[12 * i + 11]);
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
