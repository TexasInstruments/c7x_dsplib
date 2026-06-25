// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_sol_checks.h"

template <typename dataType> int check_invertability(int order, dataType *U, const int32_t strideOrder)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int      row, invertible;
   dataType tolerance_invertible = 0.01;

   int32_t dataSize    = sizeof(dataType);
   int32_t orderStride = strideOrder / dataSize;

   invertible = 1;
   for (row = 0; row < order; row++) {
      if (fabs(U[row + row * orderStride]) < tolerance_invertible) {
         invertible = 0;
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with invertible: %d\n", invertible);
   return invertible;
}

template int check_invertability<float>(int order, float *U, const int32_t strideOrder);

template int check_invertability<double>(int order, double *U, const int32_t strideOrder);

template <typename dataType>
int32_t
check_solution(int testID, int order, dataType *A, dataType *x, dataType *b, const int32_t strideOrder, char *print_str)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int      row, col, pass;
   dataType error, sum;
   dataType tolerance_solver = 0.1;
   dataType maxSum = 0, maxError = 0, maxB = 0;

   int32_t dataSize    = sizeof(dataType);
   int32_t orderStride = strideOrder / dataSize;

   pass = 1;
   for (row = 0; row < order; row++) {
      sum = 0.0;
      for (col = 0; col < order; col++) {
         sum += A[col + row * orderStride] * x[col];
      }
      error = fabs(b[row] - sum);
      if (error > tolerance_solver) {
         pass = 0;
         if (error > maxError) {
            maxError = error;
            maxSum   = sum;
            maxB     = b[row];
         }
      }
   }

   if (pass == 1) {
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_PASS %d\n", TI_TEST_KERNEL_PASS);
      return TI_TEST_KERNEL_PASS;
   }
   else {
      printf("Warning at: [%d] %s LUD solver check (A * x = b), Maximum diffrence  : %e (Ax : %e != B : %e)\n", testID,
             print_str, maxError, maxSum, maxB);
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d\n", TI_TEST_KERNEL_FAIL);
      return TI_TEST_KERNEL_FAIL;
   }
}

template int32_t
check_solution<float>(int testID, int order, float *A, float *x, float *b, const int32_t strideOrder, char *print_str);

template int32_t check_solution<double>(int           testID,
                                        int           order,
                                        double       *A,
                                        double       *x,
                                        double       *b,
                                        const int32_t strideOrder,
                                        char         *print_str);

int DSPLIB_lud_invertability_check(int order, void *pU, const int32_t strideOrder, int data_type)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   int invertible = 0;
   if (data_type == DSPLIB_FLOAT32) {
      invertible = check_invertability<float>(order, (float *) pU, strideOrder);
   }
   else if (data_type == DSPLIB_FLOAT64) {
      invertible = check_invertability<double>(order, (double *) pU, strideOrder);
   }
   else {
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with invertible: %d\n", invertible);
   return invertible;
}

int32_t DSPLIB_lud_solution_check(int           testID,
                                  int           order,
                                  void         *pA,
                                  void         *px,
                                  void         *pb,
                                  const int32_t strideOrder,
                                  int           data_type,
                                  char         *print_str)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   int32_t status = TI_TEST_KERNEL_PASS;
   if (data_type == DSPLIB_FLOAT32) {
      status = check_solution<float>(testID, order, (float *) pA, (float *) px, (float *) pb, strideOrder, print_str);
   }
   else if (data_type == DSPLIB_FLOAT64) {
      status =
          check_solution<double>(testID, order, (double *) pA, (double *) px, (double *) pb, strideOrder, print_str);
   }
   else {
      status = TI_TEST_KERNEL_FAIL;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with status: %d\n", status);
   return status;
}