// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_inv_checks.h"

template <typename dataType>
int32_t check_inverse(int testID, int order, dataType *A, dataType *invA, const int32_t strideOrder, char *print_str)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int      row, col, k, pass;
   dataType sum, error;
   dataType tolerance_inverse = 0.1;
   dataType maxSum = 0, maxError = 0;

   int32_t dataSize    = sizeof(dataType);
   int32_t orderStride = strideOrder / dataSize;

   pass = 1;
   for (row = 0; row < order; row++) {
      for (col = 0; col < order; col++) {
         sum = 0;
         for (k = 0; k < order; k++) {
            sum += (dataType) invA[k + row * orderStride] * (dataType) A[col + k * orderStride];
         }
         if (row == col)
            error = fabs(sum - 1.0);
         else
            error = fabs(sum);
         if (error > tolerance_inverse) {
            pass = 0;
            if (error > maxError) {
               maxError = error;
               maxSum   = sum;
            }
         }
      }
   }

   if (pass == 1) {
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_PASS %d\n", TI_TEST_KERNEL_PASS);
      return TI_TEST_KERNEL_PASS;
   }
   else {
      printf("Warning at: [%d] %s LUD inverse check (A * InvA = I), Maximum difference : %e, maxSum : %e\n", testID,
             print_str, maxError, maxSum);

      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d \n", TI_TEST_KERNEL_FAIL);
      return TI_TEST_KERNEL_FAIL;
   }
}

template int32_t
check_inverse<float>(int testID, int order, float *A, float *invA, const int32_t strideOrder, char *print_str);

template int32_t
check_inverse<double>(int testID, int order, double *A, double *invA, const int32_t strideOrder, char *print_str);

int32_t DSPLIB_lud_inverse_check(int           testID,
                                 int           order,
                                 void         *pA,
                                 void         *pinvA,
                                 const int32_t strideOrder,
                                 int           data_type,
                                 char         *print_str)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   int32_t status = TI_TEST_KERNEL_PASS;
   if (data_type == DSPLIB_FLOAT32) {
      status = check_inverse<float>(testID, order, (float *) pA, (float *) pinvA, strideOrder, print_str);
   }
   else if (data_type == DSPLIB_FLOAT64) {
      status = check_inverse<double>(testID, order, (double *) pA, (double *) pinvA, strideOrder, print_str);
   }
   else {
      status = TI_TEST_KERNEL_FAIL;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with status: %d\n", status);
   return status;
}
