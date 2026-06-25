// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_decomp.h"

template <typename dataType>
int32_t check_decomposition(int             testID,
                            int             order,
                            dataType       *A,
                            dataType       *L,
                            dataType       *U,
                            unsigned short *P,
                            dataType       *pScratchU,
                            const int32_t   strideOrder,
                            const int32_t   strideP,
                            char           *print_str)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int      pass, row, col, k;
   dataType sum, error;
   dataType tolerance = 0.01;
   int32_t  dataSize  = sizeof(dataType);
   int32_t  dataSizeP = sizeof(unsigned short);
   dataType maxSum = 0, maxError = 0, maxA = 0;
   int32_t  orderStride  = strideOrder / dataSize;
   int32_t  orderPStride = strideP / dataSizeP;

   pass = 1;
   for (row = 0; row < order; row++) {
      for (col = 0; col < order; col++) {
         sum = 0;
         for (k = 0; k < order; k++) {
            sum += L[k + row * orderStride] * U[col + k * orderStride];
         }
         pScratchU[col + row * orderStride] = sum;
      }
   }
   for (row = 0; row < order; row++) {
      for (col = 0; col < order; col++) {
         sum = 0;
         for (k = 0; k < order; k++) {
            sum += P[row + k * orderPStride] * pScratchU[col + k * orderStride];
         }
         error = fabs(A[col + row * orderStride] - sum);
         if (error > tolerance) {
            pass = 0;
            if (error > maxError) {
               maxError = error;
               maxSum   = sum;
               maxA     = A[col + row * orderStride];
            }
         }
      }
   }
   if (pass == 1) {
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_PASS %d\n", TI_TEST_KERNEL_PASS);
      return TI_TEST_KERNEL_PASS;
   }
   else {
      printf("Warning at: [%d] %s LUD check (A = P * L * U), Maximum difference : %e (PLU : %e != A : %e)\n", testID,
             print_str, maxError, maxSum, maxA);
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d\n", TI_TEST_KERNEL_FAIL);
      return TI_TEST_KERNEL_FAIL;
   }
}

template int32_t check_decomposition<float>(int             testID,
                                            int             order,
                                            float          *A,
                                            float          *L,
                                            float          *U,
                                            unsigned short *P,
                                            float          *pScratchU,
                                            const int32_t   strideOrder,
                                            const int32_t   strideP,
                                            char           *print_str);

template int32_t check_decomposition<double>(int             testID,
                                             int             order,
                                             double         *A,
                                             double         *L,
                                             double         *U,
                                             unsigned short *P,
                                             double         *pScratchU,
                                             const int32_t   strideOrder,
                                             const int32_t   strideP,
                                             char           *print_str);

int32_t DSPLIB_lud_decomposition_check(int           testID,
                                       int           order,
                                       void         *pA,
                                       void         *pL,
                                       void         *pU,
                                       void         *pP,
                                       void         *pScratchU,
                                       const int32_t strideOrder,
                                       const int32_t strideP,
                                       int           data_type,
                                       char         *print_str)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   int32_t status = TI_TEST_KERNEL_PASS;
   if (data_type == DSPLIB_FLOAT32) {

      status = check_decomposition<float>(testID, order, (float *) pA, (float *) pL, (float *) pU,
                                          (unsigned short *) pP, (float *) pScratchU, strideOrder, strideP, print_str);
   }
   else if (data_type == DSPLIB_FLOAT64) {

      status =
          check_decomposition<double>(testID, order, (double *) pA, (double *) pL, (double *) pU, (unsigned short *) pP,
                                      (double *) pScratchU, strideOrder, strideP, print_str);
   }
   else {
      status = TI_TEST_KERNEL_FAIL;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with status: %d\n", status);
   return status;
}
