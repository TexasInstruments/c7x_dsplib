// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_lud_decomp.h"
#include "DSPLIB_lud_idat.h"
#include "DSPLIB_lud_inv_checks.h"
#include "DSPLIB_lud_sol_checks.h"

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

int DSPLIB_lud_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsP, bufParamsL, bufParamsU, bufParamsA, bufParamsInvA;
   DSPLIB_bufParams2D_t bufParamsScratchTrans, bufParamsVecScratch;
   DSPLIB_bufParams1D_t bufParamsB, bufParamsB_Mod, bufParamsY, bufParamsX, bufParamsDiv;

#if !defined(EVM_TEST)
   uint64_t lokiCycles;
#endif // #if !defined(EVM_TEST)
   uint32_t testNum;

   DSPLIB_lud_testParams_t *prm;
   DSPLIB_lud_testParams_t  currPrm;
   DSPLIB_lud_getTestParams(&prm, &test_cases);

   DSPLIB_ludInitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_lud_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_lud_solInitArgs kerInitArgsSol;

   int32_t             handleSolSize = DSPLIB_lud_sol_getHandleSize(&kerInitArgsSol);
   DSPLIB_kernelHandle handleSol     = malloc(handleSolSize);

   DSPLIB_lud_invInitArgs kerInitArgsInv;

   int32_t             handleInvSize = DSPLIB_lud_inv_getHandleSize(&kerInitArgsInv);
   DSPLIB_kernelHandle handleInv     = malloc(handleInvSize);

   TI_profile_init("DSPLIB_lud");

#if (!defined(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_lud.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Data size, Loki cycles, Pass/Fail\n");
#endif //   #if defined(EVM_TEST)

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
         currentTestFail        = 0;

         bufParamsA.data_type = currPrm.dataType;
         bufParamsA.dim_x     = currPrm.order;
         bufParamsA.dim_y     = currPrm.order;
         bufParamsA.stride_y  = currPrm.strideOrder;

         if (bufParamsA.stride_y % 512 == 0) {
            bufParamsA.stride_y += 128;
         }

         bufParamsP.data_type = DSPLIB_UINT16;
         bufParamsP.dim_x     = currPrm.order;
         bufParamsP.dim_y     = currPrm.order;
         bufParamsP.stride_y  = currPrm.strideP;

         if (bufParamsP.stride_y % 512 == 0) {
            bufParamsP.stride_y += 128;
         }

         bufParamsL.data_type = currPrm.dataType;
         bufParamsL.dim_x     = currPrm.order;
         bufParamsL.dim_y     = currPrm.order;
         bufParamsL.stride_y  = currPrm.strideOrder;

         if (bufParamsL.stride_y % 512 == 0) {
            bufParamsL.stride_y += 128;
         }

         bufParamsU.data_type = currPrm.dataType;
         bufParamsU.dim_x     = currPrm.order;
         bufParamsU.dim_y     = currPrm.order;
         bufParamsU.stride_y  = currPrm.strideOrder;
         if (bufParamsU.stride_y % 512 == 0) {
            bufParamsU.stride_y += 128;
         }

         bufParamsB.data_type = currPrm.dataType;
         bufParamsB.dim_x     = currPrm.order;

         bufParamsB_Mod.data_type = currPrm.dataType;
         bufParamsB_Mod.dim_x     = currPrm.order;

         bufParamsY.data_type = currPrm.dataType;
         bufParamsY.dim_x     = currPrm.order;

         bufParamsVecScratch.data_type = currPrm.dataType;
         bufParamsVecScratch.dim_x     = currPrm.order;
         bufParamsVecScratch.dim_y     = 3;
         bufParamsVecScratch.stride_y  = currPrm.order * DSPLIB_sizeof(bufParamsVecScratch.data_type);

         bufParamsX.data_type = currPrm.dataType;
         bufParamsX.dim_x     = currPrm.order;

         bufParamsInvA.data_type = currPrm.dataType;
         bufParamsInvA.dim_x     = currPrm.order;
         bufParamsInvA.dim_y     = currPrm.order;
         bufParamsInvA.stride_y  = currPrm.strideOrder;

         if (bufParamsInvA.stride_y % 512 == 0) {
            bufParamsInvA.stride_y += 128;
         }

         bufParamsScratchTrans.data_type = currPrm.dataType;
         bufParamsScratchTrans.dim_x     = currPrm.order;
         bufParamsScratchTrans.dim_y     = currPrm.order;
         bufParamsScratchTrans.stride_y  = currPrm.strideOrder;

         if (bufParamsScratchTrans.stride_y % 512 == 0) {
            bufParamsScratchTrans.stride_y += 128;
         }

         bufParamsDiv.data_type = currPrm.dataType;
         bufParamsDiv.dim_x     = currPrm.order;

         int32_t order       = bufParamsB.dim_x;
         int32_t strideOrder = bufParamsA.stride_y;
         int32_t strideP     = bufParamsP.stride_y;
         int32_t eleCount    = 2 * __C7X_VEC_SIZE_BYTES__ / DSPLIB_sizeof(currPrm.dataType);

         uint64_t aSize = bufParamsA.stride_y * DSPLIB_ROW_STRIDE(bufParamsA.dim_y, eleCount);
         uint64_t pSize = bufParamsP.stride_y * DSPLIB_ROW_STRIDE(bufParamsP.dim_y, eleCount);
         uint64_t lSize = bufParamsL.stride_y * DSPLIB_ROW_STRIDE(bufParamsL.dim_y, eleCount);
         uint64_t uSize = bufParamsU.stride_y * DSPLIB_ROW_STRIDE(bufParamsU.dim_y, eleCount);
         uint64_t bSize =
             DSPLIB_CALC_STRIDE(bufParamsB.dim_x * DSPLIB_sizeof(bufParamsB.data_type), DSPLIB_ALIGN_SHIFT_64BYTES);

         uint64_t b_modSize = DSPLIB_CALC_STRIDE(bufParamsB_Mod.dim_x * DSPLIB_sizeof(bufParamsB_Mod.data_type),
                                                 DSPLIB_ALIGN_SHIFT_64BYTES);
         uint64_t ySize =
             DSPLIB_CALC_STRIDE(bufParamsY.dim_x * DSPLIB_sizeof(bufParamsY.data_type), DSPLIB_ALIGN_SHIFT_64BYTES);
         uint64_t xSize =
             DSPLIB_CALC_STRIDE(bufParamsX.dim_x * DSPLIB_sizeof(bufParamsX.data_type), DSPLIB_ALIGN_SHIFT_64BYTES);

         uint64_t invASize = bufParamsInvA.stride_y * DSPLIB_ROW_STRIDE(bufParamsInvA.dim_y, eleCount);

         uint64_t vecScratchSize = bufParamsVecScratch.stride_y * bufParamsVecScratch.dim_y;

         DSPLIB_DEBUGPRINTFN(0,
                             "aSize: %d pSize: %d lSize: %d uSize: %d bSize: %d b_modSize: %d ySize: %d xSize: %d  "
                             "invASize: %d pScratch: %d\n",
                             aSize, pSize, lSize, uSize, bSize, b_modSize, ySize, xSize, invASize, xSize);
         DSPLIB_DEBUGPRINTFN(0, "order: %d strideOrder: %d strideP: %d\n", bufParamsA.dim_x, bufParamsA.stride_y,
                             bufParamsP.stride_y);

         void *pA = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, aSize);
         void *pB = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bSize);

         void *pL, *pU, *pP, *pB_Mod, *pY, *pX, *pInvA, *pScratch;
         void *pLCn, *pUCn, *pPCn, *pB_ModCn, *pYCn, *pXCn, *pInvACn, *pScratchCn;
         void *pVecScratch, *pVecScratchCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pL = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, lSize);
            pU = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            pP = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, pSize);

            pX          = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            pInvA       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, invASize);
            pScratch    = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            pVecScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, vecScratchSize);

            pLCn = (void *) malloc(lSize);
            pUCn = (void *) malloc(uSize);
            pPCn = (void *) malloc(pSize);

            pXCn          = (void *) malloc(xSize);
            pInvACn       = (void *) malloc(invASize);
            pScratchCn    = (void *) malloc(xSize);
            pVecScratchCn = (void *) malloc(vecScratchSize);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
            int32_t usedBytes = 0;
#if defined(__C7504__) || defined(__C7524__)

            usedBytes = aSize + bSize;
            pL        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, lSize);
            usedBytes += lSize;
            pU = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            usedBytes += uSize;
            pP = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, pSize);
            usedBytes += pSize;

            pX = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            usedBytes += xSize;
            pInvA = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, invASize);
            usedBytes += invASize;
            pScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            usedBytes += xSize;
            pVecScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, vecScratchSize);
            usedBytes += vecScratchSize;

#else
            usedBytes = aSize + bSize;
            pL        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, lSize);
            usedBytes += lSize;
            pU = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            usedBytes += uSize;
            pP = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, pSize);
            usedBytes += pSize;

            pX = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            usedBytes += xSize;
            pInvA = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, invASize);
            usedBytes += invASize;
            pScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, xSize);
            usedBytes += xSize;
            pVecScratch = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, vecScratchSize);
            usedBytes += vecScratchSize;
#endif
            usedBytes = 0;
            pLCn      = (void *) ddrBuffer;
            usedBytes += lSize;

            pUCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += uSize;

            pPCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += pSize;

            pXCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += xSize;

            pInvACn = (void *) (ddrBuffer + usedBytes);
            usedBytes += invASize;

            pScratchCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += xSize;

            pVecScratchCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += vecScratchSize;
         }
         DSPLIB_DEBUGPRINTFN(0, "pA: %p pB: %p pL: %p pU: %p pP: %p pB_Mod: %p pY: %p pX: %p pInvA: %p pScratch: %p\n",
                             pA, pB, pL, pU, pP, pB_Mod, pY, pX, pInvA, pScratch);

         /* Only run the test if the buffer allocations fit in the heap */

         if (pA && pB && pL && pU && pP && pX && pInvA && pScratch && pVecScratch) {
            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pA, prm[tpi].staticInA, order, order, bufParamsA.stride_y,
                                   DSPLIB_sizeof(currPrm.dataType), testPatternString);

               TI_fillBuffer_float(prm[tpi].testPattern, 0, pB, prm[tpi].staticInB, order, 1, 0,
                                   DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }
            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pA, prm[tpi].staticInA, order, order,
                             bufParamsA.stride_y, DSPLIB_sizeof(currPrm.dataType), testPatternString);

               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pB, prm[tpi].staticInB, order, 0, 0,
                             DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }
            /****************************************************************************************************
             *                                          LUD
             ****************************************************************************************************/
            /* TEST OPTIMIZED KERNEL */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init =
                DSPLIB_lud_init_checkParams(handle, &bufParamsA, &bufParamsL, &bufParamsU, &bufParamsP, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               status_init = DSPLIB_lud_init(handle, &bufParamsA, &bufParamsL, &bufParamsU, &bufParamsP, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
               if (status_init == DSPLIB_SUCCESS) {
                  status_opt = DSPLIB_lud_exec_checkParams(handle, pA, pL, pU, pP);
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
               status_opt = DSPLIB_lud_exec(handle, pA, pL, pU, pP);
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
                  status_opt = DSPLIB_lud_exec(handle, pA, pL, pU, pP);
                  DSPLIB_asm(" MARK 5");
                  TI_profile_stop();
               }

               // get output to L1D
               int16_t outSum   = 0;
               int8_t *pOutTemp = (int8_t *) pL; // treat output as bytes to be data type agnostic
               for (k = 0; k < bufParamsL.dim_x; k++) {
                  outSum += *pOutTemp;
                  pOutTemp++;
               }

               // dummy store of outSum to insure that the compiler does not remove it.
               volatileSum = outSum;

               // run warm instruction cache test
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);
               DSPLIB_asm(" MARK 6");
               status_opt = DSPLIB_lud_exec(handle, pA, pL, pU, pP);
               DSPLIB_asm(" MARK 7");
               TI_profile_stop();
#endif // #if !defined(__C7X_HOSTEM__)
            }

            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
            status_init =
                DSPLIB_lud_init_checkParams(handle, &bufParamsA, &bufParamsL, &bufParamsU, &bufParamsP, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 8");
               status_init = DSPLIB_lud_init(handle, &bufParamsA, &bufParamsL, &bufParamsU, &bufParamsP, &kerInitArgs);
               DSPLIB_asm(" MARK 9");
               TI_profile_stop();
               if (status_init == DSPLIB_SUCCESS) {
                  status_nat = DSPLIB_lud_exec_checkParams(handle, pA, pLCn, pUCn, pPCn);
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
               status_nat = DSPLIB_lud_exec(handle, pA, pLCn, pUCn, pPCn);
               DSPLIB_asm(" MARK 11");
               TI_profile_stop();
            }

            int32_t statusDecompOpt, statusDecompCn;
            int32_t statusCompareL    = TI_TEST_KERNEL_FAIL;
            int32_t statusCompareU    = TI_TEST_KERNEL_FAIL;
            int32_t statusCompareP    = TI_TEST_KERNEL_FAIL;
            int32_t statusCompareLRef = TI_TEST_KERNEL_PASS;
            int32_t statusCompareURef = TI_TEST_KERNEL_PASS;
            int32_t statusComparePRef = TI_TEST_KERNEL_PASS;

            int32_t status_nat_vs_opt = 0;
            int32_t status_opt_vs_ref = 0;
            int32_t status_check      = 0;

            char print_txt[200];
            if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {

               strcpy(print_txt, "NAT C : ");
               statusDecompCn = DSPLIB_lud_decomposition_check(currPrm.testID, order, pA, pLCn, pUCn, pPCn, pInvACn,
                                                               strideOrder, strideP, currPrm.dataType, print_txt);
               strcpy(print_txt, "OPT: ");
               statusDecompOpt = DSPLIB_lud_decomposition_check(currPrm.testID, order, pA, pL, pU, pP, pInvACn,
                                                                strideOrder, strideP, currPrm.dataType, print_txt);

               /* ****************************************************************************/
               comparisonDone = 0;
               if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                  statusCompareL =
                      TI_compare_mem_2D_float((void *) pL, (void *) pLCn, 0.03, (double) powf(2, -8), order, order,
                                              bufParamsL.stride_y, DSPLIB_sizeof(currPrm.dataType));
                  statusCompareU =
                      TI_compare_mem_2D_float((void *) pU, (void *) pUCn, 0.03, (double) powf(2, -8), order, order,
                                              bufParamsU.stride_y, DSPLIB_sizeof(currPrm.dataType));
                  statusCompareP = TI_compare_mem_2D((void *) pP, (void *) pPCn, 0, 0, order, order,
                                                     bufParamsP.stride_y, DSPLIB_sizeof(bufParamsP.data_type));
                  comparisonDone = 1;
               }
               if (currPrm.staticOutP != NULL && currPrm.staticOutL != NULL && currPrm.staticOutU != NULL) {
                  comparisonDone    = 1;
                  statusCompareLRef = TI_compare_mem_2DDualStride_float(
                      (void *) pL, (void *) currPrm.staticOutL, 0.03, (double) powf(2, -8), order, order,
                      bufParamsL.stride_y, (order * DSPLIB_sizeof(currPrm.dataType)), DSPLIB_sizeof(currPrm.dataType));
                  statusCompareURef = TI_compare_mem_2DDualStride_float(
                      (void *) pU, (void *) currPrm.staticOutU, 0.03, (double) powf(2, -5), order, order,
                      bufParamsU.stride_y, (order * DSPLIB_sizeof(currPrm.dataType)), DSPLIB_sizeof(currPrm.dataType));
                  statusComparePRef = TI_compare_mem_2DDualStride(
                      (void *) pP, (void *) currPrm.staticOutP, 0, 0, order, order, bufParamsP.stride_y,
                      (order * DSPLIB_sizeof(bufParamsP.data_type)), DSPLIB_sizeof(bufParamsP.data_type));
               }
            }
            DSPLIB_DEBUGPRINTFN(0,
                                "statusCompareL: %d, statusCompareU: %d, statusCompareP: %d, "
                                "statusCompareLRef: %d, statusCompareURef: %d, statusComparePRef: %d, "
                                "statusDecompOpt: %d, statusDecompCn: %d,"
                                "status_opt: %d, status_nat: %d, "
                                "comparisonDone: %d\n",
                                statusCompareL, statusCompareU, statusCompareP, statusCompareLRef, statusCompareURef,
                                statusComparePRef, statusDecompOpt, statusDecompCn, status_opt, status_nat,
                                comparisonDone);

            status_check      = (statusDecompOpt != statusDecompCn) ? 0 : 1;
            status_nat_vs_opt = ((statusCompareL == TI_TEST_KERNEL_FAIL) || (statusCompareU == TI_TEST_KERNEL_FAIL) ||
                                 (statusCompareP == TI_TEST_KERNEL_FAIL))
                                    ? 0
                                    : 1;
            status_opt_vs_ref =
                ((statusCompareLRef == TI_TEST_KERNEL_FAIL) || (statusCompareURef == TI_TEST_KERNEL_FAIL) ||
                 (statusComparePRef == TI_TEST_KERNEL_FAIL))
                    ? 0
                    : 1;
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

            sprintf(desc, "LUD :%s input | order = %d Data Type %d", testPatternString, currPrm.order,
                    currPrm.dataType);

            uint64_t archCycles = pProfile[9 * tpi + 1];
#if (!defined(EVM_TEST))
            lokiCycles = archCycles;
            //  write to CSV, must happen prior to write to screen because TI_profile_formula clears values in counters
            fprintf(fpOutputCSV, "LUD, %d, %d, %d, %d, %d\n", testNum, currPrm.testPattern,
                    currPrm.order * currPrm.order, (int) lokiCycles, !currentTestFail);
#endif // #if defined(EVM_TEST)

            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail, desc);
            /****************************************************************************************************
             *                                             LUD SOLVER
             ****************************************************************************************************/
            /* Check invertability */
            int32_t invertible = 0, invertibleCn = 0;
            invertible   = DSPLIB_lud_invertability_check(order, pU, strideOrder, currPrm.dataType);
            invertibleCn = DSPLIB_lud_invertability_check(order, (float *) pUCn, strideOrder, currPrm.dataType);
            if (invertible && invertibleCn) {

               /* TEST OPTIMIZED KERNEL */
               void *pScratchTrans   = (void *) pInvA;
               void *pScratchTransCn = (void *) pInvACn;
               //  void *pDiv = (void *)pScratch;
               //  void *pDivCn = (void *)pScratchCn;

               kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init = DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsP, &bufParamsL, &bufParamsU,
                                                             &bufParamsB, &bufParamsX, &bufParamsVecScratch,
                                                             &bufParamsScratchTrans, &kerInitArgsSol);
               if (status_init == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_INIT);
                  DSPLIB_asm(" MARK 12");
                  status_init =
                      DSPLIB_lud_sol_init(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB, &bufParamsX,
                                          &bufParamsVecScratch, &bufParamsScratchTrans, &kerInitArgsSol);
                  DSPLIB_asm(" MARK 13");
                  TI_profile_stop();
                  if (status_init == DSPLIB_SUCCESS) {
                     status_opt =
                         DSPLIB_lud_sol_exec_checkParams(handleSol, pP, pL, pU, pB, pX, pVecScratch, pScratchTrans);
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
                  status_opt = DSPLIB_lud_sol_exec(handleSol, pP, pL, pU, pB, pX, pVecScratch, pScratchTrans);
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
                     status_opt = DSPLIB_lud_sol_exec(handleSol, pP, pL, pU, pB, pX, pVecScratch, pScratchTrans);
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

                  // dummy store of outSum to insure that the compiler does not remove it.
                  volatileSum = outSum;

                  // run warm instruction cache test
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

                  DSPLIB_asm(" MARK 18");
                  status_opt = DSPLIB_lud_sol_exec(handleSol, pP, pL, pU, pB, pX, pVecScratch, pScratchTrans);
                  DSPLIB_asm(" MARK 19");
                  TI_profile_stop();
#endif
               }

               /* Test _cn kernel */
               kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_NATC;
               status_init = DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsP, &bufParamsL, &bufParamsU,
                                                             &bufParamsB, &bufParamsX, &bufParamsVecScratch,
                                                             &bufParamsScratchTrans, &kerInitArgsSol);
               if (status_init == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
                  DSPLIB_asm(" MARK 20");
                  status_init =
                      DSPLIB_lud_sol_init(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB, &bufParamsX,
                                          &bufParamsVecScratch, &bufParamsScratchTrans, &kerInitArgsSol);
                  DSPLIB_asm(" MARK 21");
                  TI_profile_stop();
                  if (status_init == DSPLIB_SUCCESS) {
                     status_nat = DSPLIB_lud_sol_exec_checkParams(handleSol, pPCn, pLCn, pUCn, pB, pXCn, pVecScratchCn,
                                                                  pScratchTransCn);
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
                  status_nat =
                      DSPLIB_lud_sol_exec(handleSol, pPCn, pLCn, pUCn, pB, pXCn, pVecScratchCn, pScratchTransCn);
                  DSPLIB_asm(" MARK 23");
                  TI_profile_stop();
               }

               int statusSolverOpt = 0, statusSolverCn = 0;
               int statusCompareX    = TI_TEST_KERNEL_FAIL;
               int statusCompareXRef = TI_TEST_KERNEL_PASS;
               if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {
                  strcpy(print_txt, "NAT_C  : ");
                  statusSolverCn = DSPLIB_lud_solution_check(currPrm.testID, order, pA, pXCn, pB, strideOrder,
                                                             currPrm.dataType, print_txt);
                  strcpy(print_txt, "OPT: ");
                  statusSolverOpt = DSPLIB_lud_solution_check(currPrm.testID, order, pA, pX, pB, strideOrder,
                                                              currPrm.dataType, print_txt);

                  /* ****************************************************************************/
                  comparisonDone = 0;
                  if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                     statusCompareX = TI_compare_mem_2D_float((void *) pX, (void *) pXCn, 0.03, (double) powf(2, -8),
                                                              order, 1, 0, DSPLIB_sizeof(currPrm.dataType));
                     comparisonDone = 1;
                  }

                  if (currPrm.staticOutX != NULL) {
                     comparisonDone = 1;
                     statusCompareXRef =
                         TI_compare_mem_2D_float((void *) pX, (void *) currPrm.staticOutX, 0.03, (double) powf(2, -8),
                                                 order, 1, 0, DSPLIB_sizeof(currPrm.dataType));
                  }
               }
               DSPLIB_DEBUGPRINTFN(0,
                                   "statusCompareX: %d, statusCompareXRef: %d, "
                                   "statusSolverOpt: %d, statusSolverCn: %d, "
                                   "status_opt: %d, status_nat: %d, "
                                   "comparisonDone: %d\n",
                                   statusCompareX, statusCompareXRef, statusSolverOpt, statusSolverCn, status_opt,
                                   status_nat, comparisonDone);
               status_check      = (statusSolverOpt != statusSolverCn) ? 0 : 1;
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

               sprintf(desc, "LUD-solver : %s input | order = %d Data Type %d", testPatternString, currPrm.order,
                       currPrm.dataType);
               archCycles = pProfile[9 * tpi + 4];
#if (!defined(EVM_TEST))
               lokiCycles = archCycles;
               //  write to CSV, must happen prior to write to screen because TI_profile_formula clears values in
               //  counters
               fprintf(fpOutputCSV, "LUD-solver, %d, %d, %d, %d, %d\n", testNum, currPrm.testPattern,
                       currPrm.order * currPrm.order, (int) lokiCycles, !currentTestFail);
#endif // #if (!defined(EVM_TEST))

               DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

               TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail, desc);
               /****************************************************************************************************
                *                                   LUD INVERSE
                ****************************************************************************************************/
               /* TEST OPTIMIZED KERNEL */
               kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsP, &bufParamsL, &bufParamsU,
                                                             &bufParamsInvA, &kerInitArgsInv);
               if (status_init == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_INIT);
                  DSPLIB_asm(" MARK 24");
                  status_init = DSPLIB_lud_inv_init(handleInv, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsInvA,
                                                    &kerInitArgsInv);
                  DSPLIB_asm(" MARK 25");
                  TI_profile_stop();
                  if (status_init == DSPLIB_SUCCESS) {
                     status_opt = DSPLIB_lud_inv_exec_checkParams(handleInv, pP, pL, pU, pInvA, pScratch);
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
                  status_opt = DSPLIB_lud_inv_exec(handleInv, pP, pL, pU, pInvA, pScratch);
                  DSPLIB_asm(" MARK 27");
                  TI_profile_stop();

#if !defined(__C7X_HOSTEM__)
                  /* The following for loop is to call kernel repeatedly so as to
                   * train the branch predictor                                   */
                  uint32_t k;
                  for (k = 0; k < 4; k++) {
                     memcpy(pL, pLCn, lSize);
                     memcpy(pU, pUCn, uSize);
                     // run warm instruction cache test
                     TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
                     TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
                     DSPLIB_asm(" MARK 28");
                     status_opt = DSPLIB_lud_inv_exec(handleInv, pP, pL, pU, pInvA, pScratch);
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

                  // dummy store of outSum to insure that the compiler does not remove it.
                  volatileSum = outSum;

                  // run warm instruction cache test
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

                  DSPLIB_asm(" MARK 30");
                  memcpy(pL, pLCn, lSize);
                  memcpy(pU, pUCn, uSize);
                  status_opt = DSPLIB_lud_inv_exec(handleInv, pP, pL, pU, pInvA, pScratch);
                  DSPLIB_asm(" MARK 31");
                  TI_profile_stop();
#endif
               }

               /* Test _cn kernel */
               kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
               status_init = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsP, &bufParamsL, &bufParamsU,
                                                             &bufParamsInvA, &kerInitArgsInv);
               if (status_init == DSPLIB_SUCCESS) {
                  TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
                  DSPLIB_asm(" MARK 1");

                  status_init = DSPLIB_lud_inv_init(handleInv, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsInvA,
                                                    &kerInitArgsInv);
                  DSPLIB_asm(" MARK 2");

                  TI_profile_stop();
                  if (status_init == DSPLIB_SUCCESS) {
                     status_nat = DSPLIB_lud_inv_exec_checkParams(handleInv, pPCn, pLCn, pUCn, pInvACn, pScratchCn);
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
                  status_nat = DSPLIB_lud_inv_exec(handleInv, pPCn, pLCn, pUCn, pInvACn, pScratchCn);
                  DSPLIB_asm(" MARK 4");
                  TI_profile_stop();
               }

               int32_t statusInvOpt = 0, statusInvCn = 0;
               int32_t statusCompareInvA    = TI_TEST_KERNEL_FAIL;
               int32_t statusCompareInvARef = TI_TEST_KERNEL_PASS;
               if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {
                  strcpy(print_txt, "NAT_C  : ");
                  statusInvCn = DSPLIB_lud_inverse_check(currPrm.testID, order, pA, pInvACn, strideOrder,
                                                         currPrm.dataType, print_txt);
                  strcpy(print_txt, "OPT: ");
                  statusInvOpt = DSPLIB_lud_inverse_check(currPrm.testID, order, pA, pInvA, strideOrder,
                                                          currPrm.dataType, print_txt);

                  /* ****************************************************************************/
                  comparisonDone = 0;
                  if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
                     statusCompareInvA =
                         TI_compare_mem_2D_float((void *) pInvA, (void *) pInvACn, 0.03, (double) powf(2, -8), order,
                                                 order, bufParamsInvA.stride_y, DSPLIB_sizeof(currPrm.dataType));
                     comparisonDone = 1;
                  }
                  if (currPrm.staticOutInvA != NULL) {
                     comparisonDone       = 1;
                     statusCompareInvARef = TI_compare_mem_2DDualStride_float(
                         (void *) pInvA, (void *) currPrm.staticOutInvA, 0.03, (double) powf(2, -8), order, order,
                         bufParamsL.stride_y, (order * DSPLIB_sizeof(currPrm.dataType)),
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

               status_check      = (statusInvOpt != statusInvCn) ? 0 : 1;
               status_nat_vs_opt = ((statusCompareInvA == TI_TEST_KERNEL_FAIL)) ? 0 : 1;
               status_opt_vs_ref = ((statusCompareInvARef == TI_TEST_KERNEL_FAIL)) ? 0 : 1;

               /* Set the 'fail' flag based on test vector comparison results */
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

               sprintf(desc, "LUD-inverse : %s input | order = %d Data Type %d", testPatternString, currPrm.order,
                       currPrm.dataType);
               archCycles = pProfile[9 * tpi + 7];
#if !defined(EVM_TEST)
               lokiCycles = archCycles;
               //  write to CSV, must happen prior to write to screen because TI_profile_formula clears values in
               //  counters
               fprintf(fpOutputCSV, "LUD-inverse, %d, %d, %d, %d, %d\n", testNum, currPrm.testPattern,
                       currPrm.order * currPrm.order, (int) lokiCycles, !currentTestFail);
#endif // #if defined(EVM_TEST)

               DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

               TI_profile_add_test(testNum++, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail,
                                   desc);
            }
            else {
               sprintf(desc, "Input Matrix is non-invertible: %s input | order = %d Data Type %d", testPatternString,
                       currPrm.order, currPrm.dataType);
               TI_profile_add_test(testNum, bufParamsA.dim_x * bufParamsA.dim_y, 0, 0, 0, desc);
               sprintf(desc, "Input Matrix is non-invertible: %s input | order = %d Data Type %d", testPatternString,
                       currPrm.order, currPrm.dataType);
               TI_profile_add_test(testNum++, bufParamsA.dim_x * bufParamsA.dim_y, 0, 0, 0, desc);
            }
         }
         else {
            sprintf(desc, "Data size=%d", DSPLIB_sizeof(bufParamsA.data_type));

            DSPLIB_DEBUGPRINTFN(0, "%s\n", "Skipping Test");

            TI_profile_skip_test(desc);
         } // end of memory allocation successful?
         // Solver and inverse

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            // printf("pA: %p pL: %p pU: %p pP: %p pB_Mod: %p pY: %p pX: %p pinvA: %p\n", pA, pL, pU, pP, pB_Mod, pY,
            // pX,
            //        pinvA);
            free(pPCn);
            free(pUCn);
            free(pLCn);

            free(pXCn);
            free(pInvACn);
            free(pScratchCn);
            free(pVecScratchCn);

            TI_align_free(pP);
            TI_align_free(pU);
            TI_align_free(pL);

            TI_align_free(pX);
            TI_align_free(pInvA);
            TI_align_free(pScratch);
            TI_align_free(pVecScratch);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pL);
            TI_align_free(pU);
            TI_align_free(pP);

            TI_align_free(pX);
            TI_align_free(pInvA);
            TI_align_free(pScratch);
            TI_align_free(pVecScratch);

#else
            TI_align_free(pL);
            TI_align_free(pU);
            TI_align_free(pP);

            TI_align_free(pX);
            TI_align_free(pInvA);
            TI_align_free(pScratch);
            TI_align_free(pVecScratch);

#endif
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pL);
            TI_align_free(pP);
            TI_align_free(pU);

            TI_align_free(pX);
            TI_align_free(pInvA);
            TI_align_free(pScratch);
            TI_align_free(pVecScratch);

#endif
         }
         TI_align_free(pA);
         TI_align_free(pB);

      } // end repetitions
   }    // end idat test cases

   free(handle);
   free(handleSol);
   free(handleInv);

#if !defined(EVM_TEST)
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif // #if defined(EVM_TEST)

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return fail: %d\n", fail);

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_lud");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_lud_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t testNum         = 1000;
   int32_t currentTestFail = 0;

   DSPLIB_ludInitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_lud_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_lud_solInitArgs kerInitArgsSol;

   int32_t             handleSolSize = DSPLIB_lud_sol_getHandleSize(&kerInitArgsSol);
   DSPLIB_kernelHandle handleSol     = malloc(handleSolSize);

   DSPLIB_lud_invInitArgs kerInitArgsInv;

   int32_t             handleInvSize = DSPLIB_lud_inv_getHandleSize(&kerInitArgsInv);
   DSPLIB_kernelHandle handleInv     = malloc(handleInvSize);

   DSPLIB_STATUS status_nat;
   DSPLIB_STATUS status_opt;

   DSPLIB_bufParams2D_t bufParamsP, bufParamsL, bufParamsU, bufParamsA, bufParamsInvA;
   DSPLIB_bufParams1D_t bufParamsB, bufParamsB_Mod, bufParamsY, bufParamsX;

   int   fail = 0;
   void *pA, *pB, *pL, *pU, *pP, *pB_Mod, *pY, *pX, *pInvA, *pScratch;
   void *pLCn, *pUCn, *pPCn, *pB_ModCn, *pYCn, *pXCn, *pInvACn, *pScratchCn;

   void *pATemp, *pBTemp, *pLTemp, *pUTemp, *pPTemp, *pB_ModTemp, *pYTemp, *pXTemp, *pInvATemp, *pScratchTemp;
   void *pLCnTemp, *pUCnTemp, *pPCnTemp, *pB_ModCnTemp, *pYCnTemp, *pXCnTemp, *pInvACnTemp, *pScratchCnTemp;

   int32_t order        = 16;
   bufParamsA.data_type = DSPLIB_FLOAT32;
   bufParamsA.dim_x     = order;
   bufParamsA.dim_y     = order;
   bufParamsA.stride_y  = bufParamsA.dim_y * DSPLIB_sizeof(bufParamsA.data_type);

   bufParamsP.data_type = DSPLIB_UINT16;
   bufParamsP.dim_x     = order;
   bufParamsP.dim_y     = order;
   bufParamsP.stride_y  = bufParamsP.dim_y * DSPLIB_sizeof(bufParamsP.data_type);

   bufParamsL.data_type = bufParamsA.data_type;
   bufParamsL.dim_x     = order;
   bufParamsL.dim_y     = order;
   bufParamsL.stride_y  = bufParamsL.dim_y * DSPLIB_sizeof(bufParamsL.data_type);

   bufParamsU.data_type = bufParamsA.data_type;
   bufParamsU.dim_x     = order;
   bufParamsU.dim_y     = order;
   bufParamsU.stride_y  = bufParamsU.dim_y * DSPLIB_sizeof(bufParamsU.data_type);

   bufParamsInvA.data_type = bufParamsA.data_type;
   bufParamsInvA.dim_x     = order;
   bufParamsInvA.dim_y     = order;
   bufParamsInvA.stride_y  = bufParamsInvA.dim_y * DSPLIB_sizeof(bufParamsInvA.data_type);

   bufParamsX.data_type = bufParamsA.data_type;
   bufParamsX.dim_x     = order;

   uint64_t aSize    = bufParamsA.stride_y * bufParamsA.dim_y;
   uint64_t pSize    = bufParamsP.stride_y * bufParamsP.dim_y;
   uint64_t lSize    = bufParamsL.stride_y * bufParamsL.dim_y;
   uint64_t uSize    = bufParamsU.stride_y * bufParamsU.dim_y;
   uint64_t invASize = bufParamsInvA.stride_y * bufParamsInvA.dim_y;
   uint64_t xSize    = bufParamsX.dim_x * DSPLIB_sizeof(bufParamsX.data_type);

   DSPLIB_bufParams2D_t bufParamsPTemp, bufParamsLTemp, bufParamsUTemp, bufParamsATemp, bufParamsInvATemp;
   DSPLIB_bufParams1D_t bufParamsBTemp, bufParamsB_ModTemp, bufParamsYTemp, bufParamsXTemp;

   pA       = (void *) malloc(aSize);
   pB       = (void *) malloc(xSize);
   pL       = (void *) malloc(lSize);
   pP       = (void *) malloc(pSize);
   pU       = (void *) malloc(uSize);
   pInvA    = (void *) malloc(invASize);
   pScratch = (void *) malloc(xSize);
   pB_Mod   = (void *) malloc(xSize);
   pY       = (void *) malloc(xSize);
   pX       = (void *) malloc(xSize);

   pLCn    = (void *) malloc(lSize);
   pPCn    = (void *) malloc(pSize);
   pUCn    = (void *) malloc(uSize);
   pInvACn = (void *) malloc(invASize);

   TI_fillBuffer_float(RANDOM, 0, pA, NULL, order, order, bufParamsA.stride_y, DSPLIB_sizeof(bufParamsA.data_type),
                       testPatternString);

   while (testNum <= 1035) {

      switch (testNum) {
      case 1000:
         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;
         kerInitArgs.funcStyle    = DSPLIB_FUNCTION_NATC;
         status_nat               = DSPLIB_lud_init_checkParams(NULL, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                                &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle    = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt               = DSPLIB_lud_init_checkParams(NULL, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                                &bufParamsPTemp, &kerInitArgs);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1001:

         bufParamsATemp.data_type = DSPLIB_UINT16;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;
         kerInitArgs.funcStyle    = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         currentTestFail       = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;
         bufParamsPTemp.data_type = DSPLIB_UINT32;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         currentTestFail       = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1003:
         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = DSPLIB_FLOAT64;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         currentTestFail       = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1004:
         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = DSPLIB_FLOAT64;
         kerInitArgs.funcStyle    = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         currentTestFail       = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1005:
         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsATemp.dim_x = 16;
         bufParamsATemp.dim_y = 11;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1006:
         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsATemp.dim_x = bufParamsA.dim_x;
         bufParamsATemp.dim_y = bufParamsA.dim_y;

         bufParamsLTemp.dim_x = 16;
         bufParamsLTemp.dim_y = 11;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1007:
         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsATemp.dim_x = bufParamsA.dim_x;
         bufParamsATemp.dim_y = bufParamsA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = 16;
         bufParamsPTemp.dim_y = 11;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1008:
         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsATemp.dim_x = bufParamsA.dim_x;
         bufParamsATemp.dim_y = bufParamsA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = 16;
         bufParamsUTemp.dim_y = 11;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));
         break;
      case 1009:
         pATemp = NULL;
         pLTemp = NULL;
         pUTemp = NULL;
         pPTemp = NULL;

         pLCnTemp = NULL;
         pUCnTemp = NULL;
         pPCnTemp = NULL;

         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsATemp.dim_x = bufParamsA.dim_x;
         bufParamsATemp.dim_y = bufParamsA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         status_nat =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         status_nat            = DSPLIB_lud_exec_checkParams(handle, NULL, pL, pU, pP);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         status_opt =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         status_opt = DSPLIB_lud_exec_checkParams(handle, pA, NULL, pUTemp, pPTemp);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1010:
         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsATemp.dim_x = bufParamsA.dim_x;
         bufParamsATemp.dim_y = bufParamsA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         bufParamsATemp.data_type = DSPLIB_UINT16;
         status_nat =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

         bufParamsATemp.data_type = bufParamsA.data_type;
         status_opt = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                  &bufParamsPTemp, &kerInitArgs);
         bufParamsATemp.data_type = DSPLIB_UINT16;
         status_opt =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1011:
         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = bufParamsU.data_type;
         kerInitArgsInv.funcStyle    = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(NULL, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_lud_inv_init_checkParams(NULL, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                           &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));

         break;
      case 1012:

         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = DSPLIB_FLOAT64;
         bufParamsUTemp.data_type    = DSPLIB_FLOAT64;
         kerInitArgsInv.funcStyle    = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                           &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         break;
      case 1013:
         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = bufParamsU.data_type;
         bufParamsPTemp.data_type    = DSPLIB_UINT32;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                           &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         break;
      case 1014:
         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = DSPLIB_FLOAT64;
         bufParamsUTemp.data_type    = bufParamsU.data_type;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = DSPLIB_INT32;
         bufParamsUTemp.data_type    = bufParamsU.data_type;
         kerInitArgsInv.funcStyle    = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                           &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         break;
      case 1015:
         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = DSPLIB_FLOAT64;
         kerInitArgsInv.funcStyle    = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = DSPLIB_INT32;
         kerInitArgsInv.funcStyle    = DSPLIB_FUNCTION_NATC;
         kerInitArgsInv.funcStyle    = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                           &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         break;
      case 1016:
         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = bufParamsU.data_type;

         bufParamsInvATemp.dim_x = bufParamsInvA.dim_x;
         bufParamsInvATemp.dim_y = bufParamsInvA.dim_y;

         bufParamsLTemp.dim_x = 16;
         bufParamsLTemp.dim_y = 11;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));

         break;
      case 1017:
         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = bufParamsU.data_type;

         bufParamsInvATemp.dim_x = bufParamsInvA.dim_x;
         bufParamsInvATemp.dim_y = bufParamsInvA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = 16;
         bufParamsPTemp.dim_y = 11;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));

         break;
      case 1018:
         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = bufParamsU.data_type;

         bufParamsInvATemp.dim_x = bufParamsInvA.dim_x;
         bufParamsInvATemp.dim_y = bufParamsInvA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = 16;
         bufParamsUTemp.dim_y = 11;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));

         break;
      case 1019:
         pInvATemp   = NULL;
         pInvACnTemp = NULL;
         pLTemp      = NULL;
         pUTemp      = NULL;
         pPTemp      = NULL;

         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = bufParamsU.data_type;

         bufParamsInvATemp.dim_x = bufParamsInvA.dim_x;
         bufParamsInvATemp.dim_y = bufParamsInvA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         status_nat = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                          &bufParamsInvATemp, &kerInitArgsInv);

         status_nat = DSPLIB_lud_inv_exec_checkParams(handleInv, pPTemp, pLTemp, pUTemp, pInvATemp, pScratch);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         status_opt = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                          &bufParamsInvATemp, &kerInitArgsInv);

         status_opt      = DSPLIB_lud_inv_exec_checkParams(handleInv, pPTemp, pLTemp, pUTemp, pInvACnTemp, pScratch);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));

         break;
      case 1020:
         pInvATemp   = pInvA;
         pInvACnTemp = pInvACn;
         pLTemp      = pL;
         pUTemp      = pU;
         pPTemp      = pP;

         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = DSPLIB_FLOAT64;
         bufParamsUTemp.data_type    = DSPLIB_FLOAT64;

         bufParamsInvATemp.dim_x = 1;
         bufParamsInvATemp.dim_y = 1;

         bufParamsLTemp.dim_x = 1;
         bufParamsLTemp.dim_y = 1;

         bufParamsUTemp.dim_x = 1;
         bufParamsUTemp.dim_y = 1;

         bufParamsPTemp.dim_x = 1;
         bufParamsPTemp.dim_y = 1;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         status_nat = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                          &bufParamsInvATemp, &kerInitArgsInv);
         status_nat = DSPLIB_lud_inv_exec_checkParams(handleInv, pPTemp, pLTemp, pUTemp, pInvATemp, pScratch);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt      = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                           &bufParamsInvATemp, &kerInitArgsInv);
         status_opt      = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                               &bufParamsInvATemp, &kerInitArgsInv);
         status_opt      = DSPLIB_lud_inv_exec_checkParams(handleInv, pPTemp, pLTemp, pUTemp, pInvACnTemp, pScratch);
         currentTestFail = ((status_nat != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS));

         break;
      case 1021:

         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = bufParamsU.data_type;

         bufParamsInvATemp.dim_x = bufParamsInvA.dim_x;
         bufParamsInvATemp.dim_y = bufParamsInvA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         bufParamsLTemp.data_type = DSPLIB_UINT16;
         bufParamsUTemp.data_type = DSPLIB_UINT16;
         status_nat               = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                        &bufParamsInvATemp, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;
         status_opt = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         bufParamsLTemp.data_type = DSPLIB_UINT16;
         bufParamsUTemp.data_type = DSPLIB_UINT16;
         status_opt               = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                        &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1022:

         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = bufParamsU.data_type;

         bufParamsInvATemp.dim_x = bufParamsInvA.dim_x;
         bufParamsInvATemp.dim_y = bufParamsInvA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         bufParamsLTemp.data_type = DSPLIB_UINT16;
         bufParamsUTemp.data_type = DSPLIB_UINT16;
         status_nat               = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                        &bufParamsInvATemp, &kerInitArgsInv);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;
         status_opt = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         bufParamsLTemp.data_type = DSPLIB_UINT16;
         bufParamsUTemp.data_type = DSPLIB_UINT16;
         status_opt               = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                        &bufParamsInvATemp, &kerInitArgsInv);
         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1023:
         pATemp = NULL;
         pLTemp = NULL;
         pUTemp = NULL;
         pPTemp = NULL;

         pLCnTemp = NULL;
         pUCnTemp = NULL;
         pPCnTemp = NULL;

         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsATemp.dim_x = bufParamsA.dim_x;
         bufParamsATemp.dim_y = bufParamsA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         status_nat =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         status_nat            = DSPLIB_lud_exec_checkParams(handle, pA, pL, NULL, pP);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         status_opt =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         status_opt = DSPLIB_lud_exec_checkParams(handle, pA, NULL, pU, pP);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1024:
         pATemp = NULL;
         pLTemp = pL;
         pUTemp = pU;
         pPTemp = pP;

         pLCnTemp = pL;
         pUCnTemp = NULL;
         pPCnTemp = pP;

         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsATemp.dim_x = bufParamsA.dim_x;
         bufParamsATemp.dim_y = bufParamsA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         status_nat =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         status_nat            = DSPLIB_lud_exec_checkParams(handle, pA, pLCnTemp, pUCnTemp, pPCnTemp);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         status_opt =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         status_opt = DSPLIB_lud_exec_checkParams(handle, pATemp, pLTemp, pUTemp, pPTemp);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1025:
         pATemp = pA;
         pLTemp = NULL;
         pUTemp = pU;
         pPTemp = pP;

         pLCnTemp = pL;
         pUCnTemp = pU;
         pPCnTemp = NULL;

         bufParamsATemp.data_type = bufParamsA.data_type;
         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsATemp.dim_x = bufParamsA.dim_x;
         bufParamsATemp.dim_y = bufParamsA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         status_nat =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         status_nat            = DSPLIB_lud_exec_checkParams(handle, pATemp, pLCnTemp, pUCnTemp, pPCnTemp);
         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         status_opt =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         status_opt = DSPLIB_lud_exec_checkParams(handle, pATemp, pLTemp, pUTemp, pPTemp);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1026:
         pInvATemp   = pInvA;
         pInvACnTemp = pInvACn;
         pLTemp      = pL;
         pUTemp      = pU;
         pPTemp      = pP;

         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = DSPLIB_INT32;

         bufParamsInvATemp.dim_x = bufParamsInvA.dim_x;
         bufParamsInvATemp.dim_y = bufParamsInvA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         status_nat = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                          &bufParamsInvATemp, &kerInitArgsInv);

         status_nat = DSPLIB_lud_inv_exec_checkParams(handleInv, NULL, pLTemp, pUTemp, pInvATemp, pScratch);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         status_opt = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                          &bufParamsInvATemp, &kerInitArgsInv);

         status_opt      = DSPLIB_lud_inv_exec_checkParams(handleInv, pPTemp, NULL, pUTemp, pInvACnTemp, pScratch);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));

         break;
      case 1027:
         pInvATemp   = pInvA;
         pInvACnTemp = pInvACn;
         pLTemp      = pL;
         pUTemp      = pU;
         pPTemp      = pP;

         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = bufParamsP.data_type;
         bufParamsLTemp.data_type    = DSPLIB_INT32;
         bufParamsUTemp.data_type    = bufParamsU.data_type;

         bufParamsInvATemp.dim_x = bufParamsInvA.dim_x;
         bufParamsInvATemp.dim_y = bufParamsInvA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         status_nat = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                          &bufParamsInvATemp, &kerInitArgsInv);

         status_nat = DSPLIB_lud_inv_exec_checkParams(handleInv, pPTemp, pLTemp, NULL, pInvATemp, pScratch);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_inv_init_checkParams(handle, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         status_opt = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                          &bufParamsInvATemp, &kerInitArgsInv);

         status_opt      = DSPLIB_lud_inv_exec_checkParams(handleInv, pPTemp, pLTemp, pUTemp, NULL, pScratch);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));

         break;

      case 1028:
         pInvATemp   = pInvA;
         pInvACnTemp = pInvACn;
         pLTemp      = pL;
         pUTemp      = pU;
         pPTemp      = pP;

         bufParamsInvATemp.data_type = bufParamsInvA.data_type;
         bufParamsPTemp.data_type    = DSPLIB_FLOAT32;
         bufParamsLTemp.data_type    = bufParamsL.data_type;
         bufParamsUTemp.data_type    = bufParamsU.data_type;

         bufParamsInvATemp.dim_x = bufParamsInvA.dim_x;
         bufParamsInvATemp.dim_y = bufParamsInvA.dim_y;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         status_nat = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                          &bufParamsInvATemp, &kerInitArgsInv);

         status_nat = DSPLIB_lud_inv_exec_checkParams(handleInv, pPTemp, pLTemp, NULL, pInvATemp, pScratch);
         kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                                      &bufParamsInvATemp, &kerInitArgsInv);
         status_opt = DSPLIB_lud_inv_init(handleInv, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp,
                                          &bufParamsInvATemp, &kerInitArgsInv);

         status_opt      = DSPLIB_lud_inv_exec_checkParams(handleInv, pPTemp, pLTemp, pUTemp, NULL, pScratch);
         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));

         break;
      case 1029:

         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = DSPLIB_INT128;

         bufParamsB.data_type = bufParamsA.data_type;
         bufParamsB.dim_x     = order;

         bufParamsB_Mod.data_type = bufParamsA.data_type;
         bufParamsB_Mod.dim_x     = order;

         bufParamsY.data_type = bufParamsA.data_type;
         bufParamsY.dim_x     = order;

         bufParamsX.data_type = bufParamsA.data_type;
         bufParamsX.dim_x     = order;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         bufParamsInvA.data_type = bufParamsA.data_type;
         bufParamsInvA.dim_x     = order;
         bufParamsInvA.dim_y     = order;

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_NATC;

         status_nat =
             DSPLIB_lud_sol_init_checkParams(NULL, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsB,
                                             &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt =
             DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsB,
                                             &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         break;
      case 1030:

         bufParamsPTemp.data_type = DSPLIB_FLOAT64;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = DSPLIB_FLOAT64;

         bufParamsB.data_type = DSPLIB_FLOAT32;
         bufParamsB.dim_x     = order;

         bufParamsB_Mod.data_type = bufParamsA.data_type;
         bufParamsB_Mod.dim_x     = order;

         bufParamsY.data_type = bufParamsA.data_type;
         bufParamsY.dim_x     = order;

         bufParamsX.data_type = bufParamsA.data_type;
         bufParamsX.dim_x     = order;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         bufParamsInvA.data_type = bufParamsA.data_type;
         bufParamsInvA.dim_x     = order;
         bufParamsInvA.dim_y     = order;

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat =
             DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsB,
                                             &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         bufParamsPTemp.data_type = DSPLIB_UINT16;
         bufParamsLTemp.data_type = DSPLIB_FLOAT64;
         bufParamsUTemp.data_type = DSPLIB_FLOAT64;

         bufParamsB_Mod.data_type = bufParamsA.data_type;
         bufParamsB_Mod.dim_x     = order;

         bufParamsY.data_type = bufParamsA.data_type;
         bufParamsY.dim_x     = order;

         bufParamsX.data_type = DSPLIB_UINT16;
         bufParamsX.dim_x     = order;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         bufParamsInvA.data_type = bufParamsA.data_type;
         bufParamsInvA.dim_x     = order;
         bufParamsInvA.dim_y     = order;

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt =
             DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsB,
                                             &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));
         break;
      case 1031:

         bufParamsPTemp.data_type = bufParamsP.data_type;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsB.data_type = bufParamsA.data_type;
         bufParamsB.dim_x     = order;

         bufParamsB_Mod.data_type = bufParamsA.data_type;
         bufParamsB_Mod.dim_x     = order;

         bufParamsY.data_type = bufParamsA.data_type;
         bufParamsY.dim_x     = order;

         bufParamsX.data_type = bufParamsA.data_type;
         bufParamsX.dim_x     = order;

         bufParamsLTemp.dim_x = 256;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = 17;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         bufParamsInvA.data_type = bufParamsA.data_type;
         bufParamsInvA.dim_x     = order;
         bufParamsInvA.dim_y     = order;

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsP, &bufParamsLTemp, &bufParamsU, &bufParamsB,
                                                      &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);
         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = 11;

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsP, &bufParamsL, &bufParamsUTemp, &bufParamsB,
                                                      &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         currentTestFail =
             ((status_nat != DSPLIB_ERR_INVALID_DIMENSION) || (status_opt != DSPLIB_ERR_INVALID_DIMENSION));

         break;
      case 1032:

         bufParamsPTemp.data_type = DSPLIB_UINT16;
         bufParamsLTemp.data_type = DSPLIB_INT32;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsB.data_type = bufParamsA.data_type;
         bufParamsB.dim_x     = order;

         bufParamsB_Mod.data_type = bufParamsA.data_type;
         bufParamsB_Mod.dim_x     = order;

         bufParamsY.data_type = bufParamsA.data_type;
         bufParamsY.dim_x     = order;

         bufParamsX.data_type = bufParamsA.data_type;
         bufParamsX.dim_x     = order;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         bufParamsInvA.data_type = bufParamsA.data_type;
         bufParamsInvA.dim_x     = order;
         bufParamsInvA.dim_y     = order;

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat =
             DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsB,
                                             &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         bufParamsPTemp.data_type = DSPLIB_UINT16;
         bufParamsLTemp.data_type = DSPLIB_INT32;
         bufParamsUTemp.data_type = DSPLIB_UINT16;

         bufParamsB.data_type = DSPLIB_UINT16;
         status_nat = DSPLIB_lud_sol_init(handleSol, &bufParamsUTemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsB,
                                          &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         bufParamsPTemp.data_type = DSPLIB_UINT16;
         bufParamsLTemp.data_type = DSPLIB_INT32;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsB.data_type = bufParamsA.data_type;

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt =
             DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsB,
                                             &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         bufParamsPTemp.data_type = DSPLIB_UINT16;
         bufParamsLTemp.data_type = DSPLIB_INT32;
         bufParamsUTemp.data_type = DSPLIB_UINT16;

         bufParamsB.data_type = DSPLIB_UINT16;
         status_opt = DSPLIB_lud_sol_init(handleSol, &bufParamsPTemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsB,
                                          &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         currentTestFail = ((status_nat != DSPLIB_ERR_INVALID_TYPE) || (status_opt != DSPLIB_ERR_INVALID_TYPE));

         break;
      case 1033:

         bufParamsPTemp.data_type = DSPLIB_UINT16;
         bufParamsLTemp.data_type = DSPLIB_FLOAT64;
         bufParamsUTemp.data_type = DSPLIB_FLOAT64;

         bufParamsB.data_type = bufParamsA.data_type;
         bufParamsB.dim_x     = order;

         bufParamsB_Mod.data_type = bufParamsA.data_type;
         bufParamsB_Mod.dim_x     = order;

         bufParamsY.data_type = bufParamsA.data_type;
         bufParamsY.dim_x     = order;

         bufParamsX.data_type = bufParamsA.data_type;
         bufParamsX.dim_x     = order;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         bufParamsInvA.data_type = bufParamsA.data_type;
         bufParamsInvA.dim_x     = order;
         bufParamsInvA.dim_y     = order;

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB,
                                                      &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         status_nat = DSPLIB_lud_sol_init(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB,
                                          &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB,
                                                      &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         status_opt = DSPLIB_lud_sol_init(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB,
                                          &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         currentTestFail = ((status_nat != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS));
         break;
      case 1034:

         bufParamsPTemp.data_type = DSPLIB_UINT16;
         bufParamsLTemp.data_type = bufParamsL.data_type;
         bufParamsUTemp.data_type = bufParamsU.data_type;

         bufParamsB.data_type = bufParamsA.data_type;
         bufParamsB.dim_x     = order;

         bufParamsB_Mod.data_type = bufParamsA.data_type;
         bufParamsB_Mod.dim_x     = order;

         bufParamsY.data_type = bufParamsA.data_type;
         bufParamsY.dim_x     = order;

         bufParamsX.data_type = bufParamsA.data_type;
         bufParamsX.dim_x     = order;

         bufParamsLTemp.dim_x = bufParamsL.dim_x;
         bufParamsLTemp.dim_y = bufParamsL.dim_y;

         bufParamsUTemp.dim_x = bufParamsU.dim_x;
         bufParamsUTemp.dim_y = bufParamsU.dim_y;

         bufParamsPTemp.dim_x = bufParamsP.dim_x;
         bufParamsPTemp.dim_y = bufParamsP.dim_y;

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_NATC;
         status_nat = DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB,
                                                      &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         status_nat = DSPLIB_lud_sol_init(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB,
                                          &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         status_nat = DSPLIB_lud_sol_exec_checkParams(handleSol, pP, pL, pU, pB, pB_Mod, pY, NULL);
         status_nat = DSPLIB_lud_sol_exec_checkParams(handleSol, pP, pL, pU, pB, pB_Mod, NULL, pX);
         status_nat = DSPLIB_lud_sol_exec_checkParams(handleSol, pP, pL, pU, pB, NULL, pY, pX);
         status_nat = DSPLIB_lud_sol_exec_checkParams(handleSol, pP, pL, pU, NULL, pB_Mod, pY, pX);

         kerInitArgsSol.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt = DSPLIB_lud_sol_init_checkParams(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB,
                                                      &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);

         status_opt = DSPLIB_lud_sol_init(handleSol, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB,
                                          &bufParamsB_Mod, &bufParamsInvA, &bufParamsInvA, &kerInitArgsSol);
         status_opt = DSPLIB_lud_sol_exec_checkParams(handleSol, NULL, pL, pU, pB, pB_Mod, pY, pX);
         status_opt = DSPLIB_lud_sol_exec_checkParams(handleSol, pP, NULL, pU, pB, pB_Mod, pY, pX);
         status_opt = DSPLIB_lud_sol_exec_checkParams(handleSol, pP, pL, NULL, pB, pB_Mod, pY, pX);

         currentTestFail = ((status_nat != DSPLIB_ERR_NULL_POINTER) || (status_opt != DSPLIB_ERR_NULL_POINTER));
         break;
      case 1035:

         bufParamsATemp.data_type = DSPLIB_FLOAT32;
         bufParamsPTemp.data_type = DSPLIB_UINT16;
         bufParamsLTemp.data_type = DSPLIB_FLOAT32;
         bufParamsUTemp.data_type = DSPLIB_FLOAT32;

         bufParamsATemp.dim_x    = 1;
         bufParamsATemp.dim_y    = 1;
         bufParamsATemp.stride_y = 64;

         bufParamsLTemp.dim_x    = 1;
         bufParamsLTemp.dim_y    = 1;
         bufParamsLTemp.stride_y = 64;

         bufParamsUTemp.dim_x    = 1;
         bufParamsUTemp.dim_y    = 1;
         bufParamsUTemp.stride_y = 64;

         bufParamsPTemp.dim_x    = 1;
         bufParamsPTemp.dim_y    = 1;
         bufParamsPTemp.stride_y = 64;

         kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
         status_opt            = DSPLIB_lud_init_checkParams(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp,
                                                             &bufParamsPTemp, &kerInitArgs);
         status_opt =
             DSPLIB_lud_init(handle, &bufParamsATemp, &bufParamsLTemp, &bufParamsUTemp, &bufParamsPTemp, &kerInitArgs);
         status_opt = DSPLIB_lud_exec(handle, pA, pL, pUTemp, pPTemp);

         currentTestFail = ((status_opt != DSPLIB_SUCCESS));
         break;
      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   free(pA);
   free(pB);

   free(pL);
   free(pP);
   free(pU);
   free(pInvA);
   free(pScratch);
   free(pB_Mod);
   free(pY);
   free(pX);

   free(pLCn);
   free(pPCn);
   free(pUCn);
   free(pInvACn);

   free(handle);
   free(handleInv);
   free(handleSol);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_lud_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_lud_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_lud_getHandleSize(&kerInitArgs);
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
