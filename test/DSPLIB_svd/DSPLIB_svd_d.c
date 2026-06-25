// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"
#include "DSPLIB_svd_check.h"

// include test data for this kernel
#include "DSPLIB_svd_idat.h"

char *labelNatC = "NAT: ";
char *labelOpt  = "OPT: ";

#define DSPLIB_ROW_STRIDE(x, y) (x + y) // (((x + y) / y) * y)

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

int DSPLIB_svd_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsA, bufParamsU, bufParamsV;
   DSPLIB_bufParams1D_t bufParamsDiag, bufParamsSuperDiag;

#if !defined(EVM_TEST)
   uint64_t lokiCycles;
#endif // #if !defined(EVM_TEST)
   uint32_t testNum;

   DSPLIB_svd_testParams_t *prm;
   DSPLIB_svd_testParams_t  currPrm;
   DSPLIB_svd_getTestParams(&prm, &test_cases);

   DSPLIB_svdInitArgs kerInitArgs;

   int32_t             handleSize = DSPLIB_svd_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   TI_profile_init("DSPLIB_svd");
#if (!defined(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_svd.csv", "w+");
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

         uint32_t enableReducedForm    = currPrm.enableReducedForm;
         kerInitArgs.enableReducedForm = enableReducedForm;

         bufParamsA.data_type = currPrm.dataType;
         bufParamsA.dim_x     = currPrm.widthIn;
         bufParamsA.dim_y     = currPrm.heightIn;
         bufParamsA.stride_y  = currPrm.strideIn;
         int32_t nRows        = bufParamsA.dim_y;
         int32_t nCols        = bufParamsA.dim_x;
         int32_t maxDim       = (nRows >= nCols) ? nRows : nCols;
         int32_t minDim       = (nRows >= nCols) ? nCols : nRows;
         int32_t usedBytes    = 0;

         bufParamsU.data_type = currPrm.dataType;
         bufParamsU.dim_x     = maxDim;
         bufParamsU.dim_y     = maxDim;
         bufParamsU.stride_y =
             DSPLIB_CALC_STRIDE(bufParamsU.dim_x * DSPLIB_sizeof(bufParamsU.data_type), OUTPUT_ALIGN_BYTES);
         int32_t memPad = 64;
         if ((bufParamsU.stride_y % 512) == 0) {
            bufParamsU.stride_y = bufParamsU.stride_y + memPad;
         }

         kerInitArgs.strideURows =
             DSPLIB_CALC_STRIDE(bufParamsU.dim_y * DSPLIB_sizeof(bufParamsU.data_type), OUTPUT_ALIGN_BYTES);
         if ((kerInitArgs.strideURows % 512) == 0) {
            kerInitArgs.strideURows = kerInitArgs.strideURows + memPad;
         }

         bufParamsV.data_type = currPrm.dataType;
         bufParamsV.dim_x     = nCols;
         bufParamsV.dim_y     = nCols;
         bufParamsV.stride_y =
             DSPLIB_CALC_STRIDE(bufParamsV.dim_x * DSPLIB_sizeof(bufParamsV.data_type), OUTPUT_ALIGN_BYTES);
         if ((bufParamsV.stride_y % 512) == 0) {
            bufParamsV.stride_y = bufParamsV.stride_y + memPad;
         }
         kerInitArgs.strideVRows =
             DSPLIB_CALC_STRIDE(bufParamsV.dim_y * DSPLIB_sizeof(bufParamsV.data_type), OUTPUT_ALIGN_BYTES);
         if ((kerInitArgs.strideVRows % 512) == 0) {
            kerInitArgs.strideVRows = kerInitArgs.strideVRows + memPad;
         }

         bufParamsDiag.data_type = currPrm.dataType;
         bufParamsDiag.dim_x     = minDim;

         bufParamsSuperDiag.data_type = currPrm.dataType;
         bufParamsSuperDiag.dim_x     = minDim;
         int32_t  eleCount            = 2 * __C7X_VEC_SIZE_BYTES__ / DSPLIB_sizeof(currPrm.dataType);
         uint32_t inpSize             = bufParamsA.stride_y * DSPLIB_ROW_STRIDE(bufParamsA.dim_y, eleCount);
         uint32_t uSize               = bufParamsU.stride_y * DSPLIB_ROW_STRIDE(bufParamsU.dim_y, eleCount);
         uint32_t vSize               = bufParamsV.stride_y * DSPLIB_ROW_STRIDE(bufParamsV.dim_y, eleCount);
         uint32_t u1Size              = uSize;
         uint32_t v1Size              = vSize;
         uint32_t diagSize            = bufParamsDiag.dim_x * DSPLIB_sizeof(bufParamsDiag.data_type);
         uint32_t superDiagSize       = bufParamsSuperDiag.dim_x * DSPLIB_sizeof(bufParamsSuperDiag.data_type);
         uint32_t scratchSize =
             4 * DSPLIB_CALC_STRIDE(maxDim * DSPLIB_sizeof(bufParamsV.data_type), OUTPUT_ALIGN_BYTES);

         DSPLIB_DEBUGPRINTFN(0, "inpSize: %d uSize: %d vSize: %d diagSize: %d superDiagSize: %d u1Size: %d\n", inpSize,
                             uSize, vSize, diagSize, superDiagSize, u1Size);

         DSPLIB_DEBUGPRINTFN(0, "I/P data_type: %d O/P data_type: %d\n", bufParamsA.data_type, bufParamsA.data_type);
         DSPLIB_DEBUGPRINTFN(0, "widthIn: %d heightIn: %d strideIn: %d\n", bufParamsA.dim_x, bufParamsA.dim_y,
                             bufParamsA.stride_y);
         DSPLIB_DEBUGPRINTFN(0, "widthU: %d heightU: %d strideU: %d\n", bufParamsU.dim_x, bufParamsU.dim_y,
                             bufParamsU.stride_y);
         DSPLIB_DEBUGPRINTFN(0, "widthV: %d heightV: %d strideV: %d\n", bufParamsV.dim_x, bufParamsV.dim_y,
                             bufParamsV.stride_y);

         void *pA = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, inpSize);
         void *pU, *pV, *pDiag, *pSuperDiag, *pU1, *pV1, *pScratch;
         void *pUCn, *pVCn, *pDiagCn, *pSuperDiagCn, *pU1Cn;

         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pU = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            memset(pU, 0, uSize);
            pV = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, vSize);

            pDiag      = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, diagSize);
            pSuperDiag = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, superDiagSize);
            pU1        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, u1Size);
            pV1        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, v1Size);
            pScratch   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, scratchSize);

            pUCn         = (void *) malloc(uSize);
            pVCn         = (void *) malloc(vSize);
            pDiagCn      = (void *) malloc(diagSize);
            pSuperDiagCn = (void *) malloc(superDiagSize);
            pU1Cn        = (void *) malloc(u1Size);
            usedBytes += uSize;
            usedBytes += vSize;
            usedBytes += u1Size;
            usedBytes += v1Size;
            usedBytes += DSPLIB_CALC_STRIDE(diagSize, OUTPUT_ALIGN_BYTES);
            usedBytes += DSPLIB_CALC_STRIDE(superDiagSize, OUTPUT_ALIGN_BYTES);
            usedBytes += scratchSize;
         }
         /* DebugComments- This condition needs to be checked */
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
            pU         = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, uSize);
            pV         = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, vSize);
            pDiag      = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, diagSize);
            pSuperDiag = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, superDiagSize);
            pU1        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, u1Size);
            pV1        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, v1Size);
            pScratch   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, scratchSize);

            usedBytes = 0;
            pUCn      = (void *) ddrBuffer;
            usedBytes += uSize;

            pVCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += vSize;

            pDiagCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += diagSize;

            pSuperDiagCn = (void *) (ddrBuffer + usedBytes);
            usedBytes += superDiagSize;

            pU1Cn = (void *) (ddrBuffer + usedBytes);
            usedBytes += u1Size;
            usedBytes += v1Size;
            usedBytes += scratchSize;
         }

         DSPLIB_DEBUGPRINTFN(0, "pA: %p pV: %p pU: %p\n", pA, pV, pU);

         /* Only run the test if the buffer allocations fit in the heap */
         if (pA && pV && pU && pU1 && pV1 && pScratch && pDiag && pSuperDiag) {

            if (currPrm.dataType == DSPLIB_FLOAT32 || currPrm.dataType == DSPLIB_FLOAT64) {
               TI_fillBuffer_float(prm[tpi].testPattern, 0, pA, prm[tpi].staticInA, bufParamsA.dim_x, bufParamsA.dim_y,
                                   bufParamsA.stride_y, DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }
            else {
               TI_fillBuffer(prm[tpi].testPattern, (uint8_t) 255, pA, prm[tpi].staticInA, bufParamsA.dim_x,
                             bufParamsA.dim_y, bufParamsA.stride_y, DSPLIB_sizeof(currPrm.dataType), testPatternString);
            }

            /* TEST OPTIMIZED KERNEL */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
            status_init = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                                      &bufParamsSuperDiag, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                             &bufParamsSuperDiag, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
               if (status_init == DSPLIB_SUCCESS) {
                  status_opt = DSPLIB_svd_exec_checkParams(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
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
               status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
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
                  status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
                  DSPLIB_asm(" MARK 5");
                  TI_profile_stop();
               }

               // get output to L1D
               int16_t outSum   = 0;
               int8_t *pOutTemp = (int8_t *) pU; // treat output as bytes to be data type agnostic
               for (k = 0; k < bufParamsU.dim_x; k++) {
                  outSum += *pOutTemp;
                  pOutTemp++;
               }

               // dummy store of outSum to insure that the compiler does not remove it.
               volatileSum = outSum;

               // run warm instruction cache test
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

               DSPLIB_asm(" MARK 6");
               status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
               DSPLIB_asm(" MARK 7");
               TI_profile_stop();
#endif
            }
            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;

            status_init = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                                      &bufParamsSuperDiag, &kerInitArgs);
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_CNINIT);
               DSPLIB_asm(" MARK 8");
               status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                             &bufParamsSuperDiag, &kerInitArgs);
               DSPLIB_asm(" MARK 9");
               TI_profile_stop();

               if (status_init == DSPLIB_SUCCESS) {
                  status_nat =
                      DSPLIB_svd_exec_checkParams(handle, pA, pUCn, pVCn, pDiagCn, pSuperDiagCn, pU1Cn, pV1, pScratch);
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
               status_nat = DSPLIB_svd_exec(handle, pA, pUCn, pVCn, pDiagCn, pSuperDiagCn, pU1Cn, pV1, pScratch);
               DSPLIB_asm(" MARK 11");
               TI_profile_stop();
            }
            /* ********************************************************************************** */
            int32_t colAStride = bufParamsA.stride_y / DSPLIB_sizeof(bufParamsA.data_type);
            int32_t colUStride = bufParamsU.stride_y / DSPLIB_sizeof(bufParamsU.data_type);
            int32_t colVStride = bufParamsV.stride_y / DSPLIB_sizeof(bufParamsV.data_type);

            /* *************************************************************************** */

            int32_t statusDecompOpt    = TI_TEST_KERNEL_FAIL;
            int32_t statusTransformOpt = TI_TEST_KERNEL_FAIL;
            int32_t statusDecompCn     = TI_TEST_KERNEL_FAIL;
            int32_t statusTransformCn  = TI_TEST_KERNEL_FAIL;

            int32_t statusCompareU       = TI_TEST_KERNEL_FAIL;
            int32_t statusCompareV       = TI_TEST_KERNEL_FAIL;
            int32_t statusCompareDiag    = TI_TEST_KERNEL_FAIL;
            int32_t statusCompareURef    = TI_TEST_KERNEL_PASS;
            int32_t statusCompareVRef    = TI_TEST_KERNEL_PASS;
            int32_t statusCompareDiagRef = TI_TEST_KERNEL_PASS;

            comparisonDone = 0;
            if ((status_opt == DSPLIB_SUCCESS) && (status_nat == DSPLIB_SUCCESS)) {
               statusDecompOpt =
                   svd_check_decomposition(currPrm.dataType, nRows, nCols, pA, pU, pDiag, pV, pU1, labelOpt, colAStride,
                                           colUStride, colVStride, enableReducedForm);

               statusTransformOpt = svd_check_transformation(currPrm.dataType, nRows, nCols, pU, pV, labelOpt,
                                                             colUStride, colVStride, enableReducedForm);

               statusDecompCn =
                   svd_check_decomposition(currPrm.dataType, nRows, nCols, pA, pUCn, pDiagCn, pVCn, pU1Cn, labelNatC,
                                           colAStride, colUStride, colVStride, enableReducedForm);

               statusTransformCn = svd_check_transformation(currPrm.dataType, nRows, nCols, pUCn, pVCn, labelNatC,
                                                            colUStride, colVStride, enableReducedForm);

               DSPLIB_DEBUGPRINTFN(
                   0, "statusDecompOpt: %d, statusTransformOpt: %d, statusDecompCn: %d, statusTransformCn: %d\n",
                   statusDecompOpt, statusTransformOpt, statusDecompCn, statusTransformCn);

               if ((currPrm.dataType == DSPLIB_FLOAT32) || (currPrm.dataType == DSPLIB_FLOAT64)) {
                  if (enableReducedForm) {
                     if (nRows >= nCols) {
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing U & UCn\n");
                        statusCompareU =
                            TI_compare_mem_2D_float((void *) pU, (void *) pUCn, 0.05, (double) powf(2, -6), nCols,
                                                    nRows, bufParamsU.stride_y, DSPLIB_sizeof(currPrm.dataType));
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing V & VCn\n");
                        statusCompareV =
                            TI_compare_mem_2D_float((void *) pV, (void *) pVCn, 0.05, (double) powf(2, -6), nCols,
                                                    nCols, bufParamsV.stride_y, DSPLIB_sizeof(currPrm.dataType));
                     }
                     else {
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing U & UCn\n");
                        statusCompareU =
                            TI_compare_mem_2D_float((void *) pU, (void *) pUCn, 0.05, (double) powf(2, -6), nRows,
                                                    nRows, bufParamsU.stride_y, DSPLIB_sizeof(currPrm.dataType));
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing V & VCn\n");
                        statusCompareV =
                            TI_compare_mem_2D_float((void *) pV, (void *) pVCn, 0.05, (double) powf(2, -6), nRows,
                                                    nCols, bufParamsV.stride_y, DSPLIB_sizeof(currPrm.dataType));
                     }
                  }
                  else {
                     DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing U & UCn\n");
                     statusCompareU =
                         TI_compare_mem_2D_float((void *) pU, (void *) pUCn, 0.05, (double) powf(2, -6), nRows, nRows,
                                                 bufParamsU.stride_y, DSPLIB_sizeof(currPrm.dataType));
                     DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing V & VCn\n");
                     statusCompareV =
                         TI_compare_mem_2D_float((void *) pV, (void *) pVCn, 0.05, (double) powf(2, -6), nCols, nCols,
                                                 bufParamsV.stride_y, DSPLIB_sizeof(currPrm.dataType));
                  }
                  DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing Diag & DiagCn\n");
                  statusCompareDiag =
                      TI_compare_mem_2D_float((void *) pDiag, (void *) pDiagCn, 0.05, (double) powf(2, -6),
                                              bufParamsDiag.dim_x, 1, 0, DSPLIB_sizeof(currPrm.dataType));

                  comparisonDone = 1;
               }

               if (currPrm.staticOutU != NULL) {
                  if (enableReducedForm) {
                     if (nRows >= nCols) /* vertical */
                     {
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing U & Static U\n");
                        statusCompareURef = TI_compare_mem_2DDualStride_float(
                            (void *) pU, (void *) currPrm.staticOutU, 0.05, (double) powf(2, -6), nCols, nRows,
                            bufParamsU.stride_y, nCols * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing V & Static V\n");
                        statusCompareVRef = TI_compare_mem_2DDualStride_float(
                            (void *) pV, (void *) currPrm.staticOutV, 0.05, (double) powf(2, -6), nCols, nCols,
                            bufParamsV.stride_y, nCols * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                     }
                     else {
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing U & Static U\n");
                        statusCompareURef = TI_compare_mem_2DDualStride_float(
                            (void *) pU, (void *) currPrm.staticOutU, 0.05, (double) powf(2, -6), nRows, nRows,
                            bufParamsU.stride_y, nRows * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing V & Static V\n");
                        statusCompareVRef = TI_compare_mem_2DDualStride_float(
                            (void *) pV, (void *) currPrm.staticOutV, 0.05, (double) powf(2, -6), nRows, nCols,
                            bufParamsV.stride_y, nRows * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                     }
                  }
                  else {
                     if (nRows >= nCols) {
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing U & Static U\n");
                        statusCompareURef = TI_compare_mem_2DDualStride_float(
                            (void *) pU, (void *) currPrm.staticOutU, 0.05, (double) powf(2, -6), nCols, nCols,
                            bufParamsU.stride_y, nRows * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                     }
                     else {
                        DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing U & Static U\n");
                        statusCompareURef = TI_compare_mem_2DDualStride_float(
                            (void *) pU, (void *) currPrm.staticOutU, 0.05, (double) powf(2, -6), nRows, nRows,
                            bufParamsU.stride_y, nRows * DSPLIB_sizeof(currPrm.dataType),
                            DSPLIB_sizeof(currPrm.dataType));
                     }
                     DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing V & Static V\n");
                     statusCompareVRef = TI_compare_mem_2DDualStride_float(
                         (void *) pV, (void *) currPrm.staticOutV, 0.05, (double) powf(2, -6), nCols, nCols,
                         bufParamsV.stride_y, nCols * DSPLIB_sizeof(currPrm.dataType), DSPLIB_sizeof(currPrm.dataType));
                  }

                  DSPLIB_DEBUGPRINTFN(0, "%s", "Comparing Diag & DiagCn\n");
                  statusCompareDiagRef = TI_compare_mem_2D_float((void *) pDiag, (void *) currPrm.staticOutDiag, 0.05,
                                                                 (double) powf(2, -6), bufParamsDiag.dim_x, 1, 0,
                                                                 DSPLIB_sizeof(currPrm.dataType));
               }
            }

            int32_t statusCheck =
                ((statusDecompOpt != statusDecompCn) || (statusTransformOpt != statusTransformCn)) ? 0 : 1;
            int32_t status_nat_vs_opt =
                ((statusCompareU == TI_TEST_KERNEL_FAIL) || (statusCompareV == TI_TEST_KERNEL_FAIL) ||
                 (statusCompareDiag == TI_TEST_KERNEL_FAIL))
                    ? 0
                    : 1;

            int32_t status_ref_vs_opt =
                ((statusCompareURef == TI_TEST_KERNEL_FAIL) || (statusCompareVRef == TI_TEST_KERNEL_FAIL) ||
                 (statusCompareDiagRef == TI_TEST_KERNEL_FAIL))
                    ? 0
                    : 1;

            currentTestFail =
                ((statusCheck + status_nat_vs_opt + status_ref_vs_opt < 2) || (status_init != DSPLIB_SUCCESS) ||
                 (status_opt != DSPLIB_SUCCESS) || (status_nat != DSPLIB_SUCCESS) || (comparisonDone == 0))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

            DSPLIB_DEBUGPRINTFN(0, "fail: %d, currentTestFail: %d\n", fail, currentTestFail);

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s input | widthIn= %d heightIn = %d reducedForm= %d Data Type %d", testPatternString,
                    currPrm.widthIn, currPrm.heightIn, enableReducedForm, currPrm.dataType);
            uint64_t archCycles = pProfile[3 * tpi + 1];
#if (!defined(EVM_TEST))
            lokiCycles = archCycles;
            // write to CSV, must happen prior to write to screen because
            fprintf(fpOutputCSV, "SVD, %d, %d, %d, %d, %d\n", testNum, currPrm.testPattern,
                    currPrm.widthIn * currPrm.heightIn, (int) lokiCycles, !currentTestFail);

#endif // #if (!defined(EVM_TEST))

            DSPLIB_DEBUGPRINTFN(0, "%s", "Current Test Complete\n");

            TI_profile_add_test(testNum++, bufParamsA.dim_x * bufParamsA.dim_y, archCycles, 0, currentTestFail, desc);
         }
         else {
            sprintf(desc, "Data size=%d", bufParamsA.dim_x * bufParamsA.dim_y);

            DSPLIB_DEBUGPRINTFN(0, "%s\n", "Skipping Test");

            TI_profile_skip_test(desc);
            // clear the counters between runs; normally handled by TI_profile_sqr_test
            TI_profile_clear_run_stats();

         } // end of memory allocation successful?

         /* Free buffers for each test vector */

         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pUCn);
            free(pVCn);
            free(pDiagCn);
            free(pSuperDiagCn);
            free(pU1Cn);

            TI_align_free(pU);
            TI_align_free(pV);
            TI_align_free(pDiag);
            TI_align_free(pSuperDiag);
            TI_align_free(pU1);
            TI_align_free(pV1);
            TI_align_free(pScratch);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pU);
            TI_align_free(pV);
            TI_align_free(pDiag);
            TI_align_free(pSuperDiag);
            TI_align_free(pU1);
            TI_align_free(pV1);
            TI_align_free(pScratch);
#else
            TI_align_free(pU);
            TI_align_free(pV);
            TI_align_free(pDiag);
            TI_align_free(pSuperDiag);
            TI_align_free(pU1);
            TI_align_free(pV1);
            TI_align_free(pScratch);
#endif
         }
         else {
#if defined(__C7504__) || defined(__C7524__)
            TI_align_free(pU);
            TI_align_free(pV);
            TI_align_free(pDiag);
            TI_align_free(pSuperDiag);
            TI_align_free(pU1);
            TI_align_free(pV1);
            TI_align_free(pScratch);
#endif
         }
         TI_align_free(pA);
      } // end repetitions
   }    // end idat test cases

   free(handle);

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
      TI_memError("DSPLIB_svd");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_svd_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t testNum         = 1001;
   int32_t currentTestFail = 0;
   int32_t fail            = 0;
   int32_t status_opt      = 0;
   int32_t status_init     = 0;

   DSPLIB_bufParams2D_t bufParamsA, bufParamsU, bufParamsV;
   DSPLIB_bufParams1D_t bufParamsDiag, bufParamsSuperdiag;
   DSPLIB_svdInitArgs   kerInitArgs;
   int32_t              handleSize = DSPLIB_svd_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle  handle     = malloc(handleSize);
   void                *pA         = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 4096);
   void                *pU         = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 4096);
   void                *pV         = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 4096);
   void                *pDiag      = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 1024);
   void                *pSuperDiag = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 1024);
   void                *pU1        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 4096);
   void                *pV1        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 4096);
   void                *pScratch   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, 4096);

   /* CASE 1001 - NULL HANDLE CASE*/
   status_init = DSPLIB_svd_init_checkParams(NULL, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                             &bufParamsSuperdiag, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1002 - Invalid Type case*/
   bufParamsA.data_type = DSPLIB_INT64;
   status_init          = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                                      &bufParamsSuperdiag, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1003 - Invalid Type case*/
   bufParamsA.data_type = DSPLIB_FLOAT32;
   bufParamsU.data_type = DSPLIB_FLOAT64;
   status_init          = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                                      &bufParamsSuperdiag, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1004 - Invalid Type case*/
   bufParamsA.data_type = DSPLIB_FLOAT32;
   bufParamsU.data_type = DSPLIB_FLOAT32;
   bufParamsV.data_type = DSPLIB_FLOAT64;
   status_init          = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                                      &bufParamsSuperdiag, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1005 - Invalid Type case*/
   bufParamsA.data_type    = DSPLIB_FLOAT32;
   bufParamsV.data_type    = DSPLIB_FLOAT32;
   bufParamsDiag.data_type = DSPLIB_FLOAT64;
   status_init             = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                                         &bufParamsSuperdiag, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1006 - Invalid Type case*/
   bufParamsA.data_type         = DSPLIB_FLOAT32;
   bufParamsDiag.data_type      = DSPLIB_FLOAT32;
   bufParamsSuperdiag.data_type = DSPLIB_FLOAT64;
   status_init = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                             &bufParamsSuperdiag, &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1007 - Null pointer case*/
   status_init     = DSPLIB_svd_exec_checkParams(handle, NULL, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1008 - Null pointer case*/
   status_init     = DSPLIB_svd_exec_checkParams(handle, pA, NULL, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1009 - Null pointer case*/
   status_init     = DSPLIB_svd_exec_checkParams(handle, pA, pU, NULL, pDiag, pSuperDiag, pU1, pV1, pScratch);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1010 - Null pointer case*/
   status_init     = DSPLIB_svd_exec_checkParams(handle, pA, pU, pV, NULL, pSuperDiag, pU1, pV1, pScratch);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1011 - Null pointer case*/
   status_init     = DSPLIB_svd_exec_checkParams(handle, pA, pU, pV, pDiag, NULL, pU1, pV1, pScratch);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1012 - Null pointer case*/
   status_init     = DSPLIB_svd_exec_checkParams(handle, pA, pU, pV, pDiag, pSuperDiag, NULL, pV1, pScratch);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1013 - Null pointer case*/
   status_init     = DSPLIB_svd_exec_checkParams(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, NULL, pScratch);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1014 - Null pointer case*/
   status_init     = DSPLIB_svd_exec_checkParams(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, NULL);
   currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1015 - Invalid Type case*/
   bufParamsA.data_type  = DSPLIB_INT64;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1016 - Invalid Type case*/
   bufParamsA.data_type  = DSPLIB_INT64;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1017 - Invalid Type case*/
   bufParamsA.data_type  = DSPLIB_INT64;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   currentTestFail = (status_init != DSPLIB_ERR_INVALID_TYPE);
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1018 - uFinal_ci.cpp*/
   bufParamsA.data_type = DSPLIB_FLOAT32;
   bufParamsA.dim_x     = 15;
   bufParamsA.dim_y     = 3;
   bufParamsA.stride_y  = 3 * sizeof(float);

   bufParamsU.dim_x    = 15;
   bufParamsU.dim_y    = 15;
   bufParamsU.stride_y = 15 * sizeof(float);

   bufParamsV.dim_x    = 15;
   bufParamsV.dim_y    = 15;
   bufParamsV.stride_y = 15 * sizeof(float);

   kerInitArgs.dimX              = bufParamsA.dim_x;
   kerInitArgs.dimY              = bufParamsA.dim_y;
   kerInitArgs.enableReducedForm = 0;
   kerInitArgs.funcStyle         = DSPLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.strideURows       = bufParamsA.stride_y;
   kerInitArgs.strideVRows       = bufParamsA.stride_y;

   memset(pA, 0, 1024);
   float *superDiagfill = (float *) pSuperDiag;
   superDiagfill[0]     = 1.414;

   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   if (status_init == DSPLIB_SUCCESS) {
      status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   }

   currentTestFail = ((status_init != DSPLIB_SUCCESS) && (status_opt != DSPLIB_SUCCESS));
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   if (status_init == DSPLIB_SUCCESS) {
      status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   }
   currentTestFail = ((status_init != DSPLIB_SUCCESS) && (status_opt != DSPLIB_SUCCESS));
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;

   kerInitArgs.enableReducedForm = 1;
   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   if (status_init == DSPLIB_SUCCESS) {
      status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   }

   currentTestFail = ((status_init != DSPLIB_SUCCESS) && (status_opt != DSPLIB_SUCCESS));
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;

   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1019 - Rotation check*/
   bufParamsA.data_type = bufParamsDiag.data_type = bufParamsDiag.data_type = bufParamsU.data_type =
       bufParamsV.data_type                                                 = DSPLIB_FLOAT64;
   uint32_t testDim                                                         = 10;
   bufParamsA.dim_x                                                         = testDim;
   bufParamsA.dim_y                                                         = testDim;
   bufParamsA.stride_y                                                      = testDim * sizeof(double);

   bufParamsU.dim_x    = testDim;
   bufParamsU.dim_y    = testDim;
   bufParamsU.stride_y = testDim * sizeof(double);

   bufParamsV.dim_x    = testDim;
   bufParamsV.dim_y    = testDim;
   bufParamsV.stride_y = testDim * sizeof(double);

   bufParamsDiag.dim_x      = testDim;
   bufParamsSuperdiag.dim_x = testDim;

   kerInitArgs.dimX        = bufParamsA.dim_x;
   kerInitArgs.dimY        = bufParamsA.dim_y;
   kerInitArgs.strideURows = bufParamsA.stride_y;
   kerInitArgs.strideVRows = bufParamsA.stride_y;

   double  *pAFill = (double *) pA;
   uint32_t ii = 0, jj = 0;
   for (ii = 0; ii < testDim; ii++) {
      for (jj = 0; jj < testDim; jj++) {
         pAFill[jj + ii * testDim] = (double) (jj + ii * testDim);
      }
   }

   kerInitArgs.funcStyle         = DSPLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.enableReducedForm = 1;

   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   if (status_init == DSPLIB_SUCCESS) {
      status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   }

   currentTestFail = ((status_init != DSPLIB_SUCCESS) && (status_opt != DSPLIB_SUCCESS));
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;

   kerInitArgs.funcStyle         = DSPLIB_FUNCTION_NATC;
   kerInitArgs.enableReducedForm = 1;

   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   if (status_init == DSPLIB_SUCCESS) {
      status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   }
   currentTestFail = ((status_init != DSPLIB_SUCCESS) && (status_opt != DSPLIB_SUCCESS));
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;

   kerInitArgs.enableReducedForm = 0;
   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   if (status_init == DSPLIB_SUCCESS) {
      status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   }

   currentTestFail = ((status_init != DSPLIB_SUCCESS) && (status_opt != DSPLIB_SUCCESS));
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;

   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   /* CASE 1020 - Rotation check*/
   bufParamsA.data_type = bufParamsDiag.data_type = bufParamsDiag.data_type = bufParamsU.data_type =
       bufParamsV.data_type                                                 = DSPLIB_FLOAT64;
   testDim                                                                  = 2;
   bufParamsA.dim_x                                                         = testDim;
   bufParamsA.dim_y                                                         = testDim;
   bufParamsA.stride_y                                                      = testDim * sizeof(double);

   bufParamsU.dim_x    = testDim;
   bufParamsU.dim_y    = testDim;
   bufParamsU.stride_y = testDim * sizeof(double);

   bufParamsV.dim_x    = testDim;
   bufParamsV.dim_y    = testDim;
   bufParamsV.stride_y = testDim * sizeof(double);

   bufParamsDiag.dim_x      = testDim;
   bufParamsSuperdiag.dim_x = testDim;

   kerInitArgs.dimX        = bufParamsA.dim_x;
   kerInitArgs.dimY        = bufParamsA.dim_y;
   kerInitArgs.strideURows = bufParamsA.stride_y;
   kerInitArgs.strideVRows = bufParamsA.stride_y;

   // double *pAFill = (double *)pA;
   for (ii = 0; ii < testDim; ii++) {
      for (jj = 0; jj < testDim; jj++) {
         pAFill[jj + ii * testDim] = (double) (jj + ii * testDim);
      }
   }

   double *superDiagfillDouble = (double *) pSuperDiag;
   superDiagfillDouble[0]      = 1.414f;

   kerInitArgs.funcStyle         = DSPLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.enableReducedForm = 1;

   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   if (status_init == DSPLIB_SUCCESS) {
      status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   }

   currentTestFail = ((status_init != DSPLIB_SUCCESS) && (status_opt != DSPLIB_SUCCESS));
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;

   kerInitArgs.funcStyle         = DSPLIB_FUNCTION_NATC;
   kerInitArgs.enableReducedForm = 1;

   superDiagfillDouble[0] = 1.414f;

   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   if (status_init == DSPLIB_SUCCESS) {
      status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   }
   currentTestFail = ((status_init != DSPLIB_SUCCESS) && (status_opt != DSPLIB_SUCCESS));
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;

   superDiagfillDouble[0] = 1.414f;

   kerInitArgs.enableReducedForm = 0;
   status_init = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperdiag,
                                 &kerInitArgs);

   if (status_init == DSPLIB_SUCCESS) {
      status_opt = DSPLIB_svd_exec(handle, pA, pU, pV, pDiag, pSuperDiag, pU1, pV1, pScratch);
   }

   currentTestFail = ((status_init != DSPLIB_SUCCESS) && (status_opt != DSPLIB_SUCCESS));
   fail            = ((fail == 1) || currentTestFail == 1) ? 1 : 0;

   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum, 0, 0, 0, currentTestFail, desc);
   testNum++;

   TI_align_free(pA);
   TI_align_free(pU);
   TI_align_free(pV);
   TI_align_free(pDiag);
   TI_align_free(pSuperDiag);
   TI_align_free(pU1);
   TI_align_free(pV1);
   TI_align_free(pScratch);
   free(handle);

#if defined(ENABLE_LDRA_COVERAGE)
   /* For every call of DSPLIB_svd_getHandleSize() the execution history is pushed
      as this function is the anchor point for LDRA in .cpp kernel files.
      Therefore calling DSPLIB_svd_getHandleSize() to push the execution history
      at the end of coverage test cases. */
   int32_t handleSize_LDRA = DSPLIB_svd_getHandleSize(&kerInitArgs);
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
   if (fail == 0) {
      printf("Coverage Test Pass!\n");
   }
   else {
      printf("Coverage Test Fail!\n");
   }

   return fail;
}
#endif
