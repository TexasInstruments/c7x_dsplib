// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"
#include "DSPLIB_qrd_check.h"

template <typename dataType>
int32_t DSPLIB_qrd_check_decomposition_core(int32_t   testID,
                                            int32_t   NRows,
                                            int32_t   NCols,
                                            dataType *A,
                                            dataType *Q,
                                            dataType *R,
                                            char      print_txt[16],
                                            int32_t   strideA,
                                            int32_t   strideQ,
                                            int32_t   strideR)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t colAStride = strideA / sizeof(dataType);
   int32_t colQStride = strideQ / sizeof(dataType);
   int32_t colRStride = strideR / sizeof(dataType);
   int32_t pass, row, col, k;

   dataType sum, error, maxError = 0, maxA, maxSum = 0;

   dataType tolerance = 0.01;

   pass = 1;

   for (row = 0; row < NRows; row++) {
      for (col = 0; col < NCols; col++) {
         sum = 0;
         for (k = 0; k < NRows; k++) {
            sum += Q[k + row * colQStride] * R[col + k * colRStride];
         }
         error = fabs(A[col + row * colAStride] - sum);
         if (error > tolerance) {
            pass = 0;
            if (error > maxError) {
               maxError = error;
               maxA     = A[col + row * colAStride];
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
      printf("Warning at: [%d] : %s decomposition check (A = Q x R), max difference orig=%e calc=%e error=%e\n", testID,
             print_txt, maxA, maxSum, maxError);
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d\n", TI_TEST_KERNEL_FAIL);
      return TI_TEST_KERNEL_FAIL;
   }
}

template <typename dataType>
int32_t
DSPLIB_qrd_check_transformation_core(int32_t testID, int32_t NRows, dataType *Q, char print_txt[16], int32_t strideQ)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t  colQStride = strideQ / sizeof(dataType);
   int32_t  row, col, k, pass;
   dataType sum, error, tolerance, maxError = 0;

   pass      = 1;
   tolerance = 0.01;
   /* check Q*Q'=I */
   for (row = 0; row < NRows; row++) {
      for (col = 0; col < NRows; col++) {
         sum = 0;
         for (k = 0; k < NRows; k++) {
            sum += Q[k + row * colQStride] * Q[k + col * colQStride];
         }
         if (row == col) {
            error = fabs(sum - 1.0);
         }
         else {
            error = fabs(sum);
         }
         if (error > tolerance) {
            pass = 0;
            if (error > maxError) {
               maxError = error;
            }
         }
      }
   }

   if (pass == 1) {
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_PASS %d\n", TI_TEST_KERNEL_PASS);
      return TI_TEST_KERNEL_PASS;
   }
   else {
      printf("Warning at: [%d] : %s QRD Decomposition check Q*Q'=I, maximum difference : %e\n", testID, print_txt,
             maxError);
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d\n", TI_TEST_KERNEL_FAIL);
      return TI_TEST_KERNEL_FAIL;
   }
}

template <typename dataType>
int32_t DSPLIB_qrd_check_solution_core(int32_t   testID,
                                       int32_t   NRows,
                                       int32_t   NCols,
                                       dataType *A,
                                       dataType *x,
                                       dataType *b,
                                       char      print_txt[16],
                                       int32_t   strideQ)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t  row, col, pass;
   dataType sum, error, tolerance, maxError = 0, maxB = 0, maxSum = 0;
   pass               = 1;
   tolerance          = (sizeof(dataType) == 8) ? (0.01) : (0.01);
   int32_t colAStride = strideQ / sizeof(dataType);
   /* check A*x=b  */
   for (row = 0; row < NRows; row++) {
      sum = 0.0;
      for (col = 0; col < NCols; col++) {
         sum += A[col + row * colAStride] * x[col];
      }
      error = fabs(b[row] - sum);
      if (error > tolerance) {
         pass = 0;
         if (error > maxError) {
            maxError = error;
            maxB     = b[row];
            maxSum   = sum;
         }
      }
   }
   if (pass == 1) {
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_PASS %d\n", TI_TEST_KERNEL_PASS);
      return TI_TEST_KERNEL_PASS;
   }
   else {
      printf("Warning at: [%d] : %s A * x = B, maximum difference => orig=%e calc=%e error=%e\n", testID, print_txt,
             maxB, maxSum, maxError);
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d\n", TI_TEST_KERNEL_FAIL);
      return TI_TEST_KERNEL_FAIL;
   }
}

template <typename dataType>
int32_t DSPLIB_qrd_check_inverse_core(int32_t   testID,
                                      int32_t   NRows,
                                      int32_t   NCols,
                                      dataType *A,
                                      dataType *InvA,
                                      char      print_txt[16],
                                      int32_t   strideA,
                                      int32_t   strideInvA)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t  row, col, k, pass;
   dataType sum, error, tolerance, maxInvA, maxSum, maxError = 0;
   int32_t  colAStride    = strideA / sizeof(dataType);
   int32_t  colInvAStride = strideInvA / sizeof(dataType);
   pass                   = 1;
   tolerance              = (sizeof(dataType) == 8) ? (0.01) : (0.01);
   for (row = 0; row < NRows; row++) {
      for (col = 0; col < NCols; col++) {
         sum = 0;
         for (k = 0; k < NCols; k++) {
            sum += InvA[k + row * colInvAStride] * A[col + k * colAStride];
         }
         if (row == col) {
            error = fabs(sum - 1.0);
         }
         else {
            error = fabs(sum);
         }

         if (error > tolerance && row == col) {
            pass = 0;
            if (error > maxError) {
               maxError = error;
               maxInvA  = 1.0;
               maxSum   = sum;
            }
         }
         else if (error > tolerance) {
            pass = 0;
            if (error > maxError) {
               maxError = error;
               maxInvA  = 0.0;
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
      printf("Warning at: [%d] : %s invA * A = I, max difference orig=%e calc=%e error=%e\n", testID, print_txt,
             maxInvA, maxSum, maxError);
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d\n", TI_TEST_KERNEL_FAIL);
      return TI_TEST_KERNEL_FAIL;
   }
}

template <typename dataType>
int32_t DSPLIB_qrd_check_invertibility_core(int32_t NRows, int32_t NCols, dataType *R, int32_t strideR)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   dataType tolerance;
   int32_t  invertible;
   int32_t  row;
   int32_t  colRStride = strideR / sizeof(dataType);
   tolerance           = (sizeof(dataType) == 8) ? (0.001) : (0.001);
   if (NRows == NCols) {
      invertible = 1;
      for (row = 0; row < NRows; row++) {
         if (fabs(R[row + row * colRStride]) < tolerance) {
            invertible = 0;
         }
      }
   }
   else {
      invertible = 0;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with invertible: %d\n", invertible);
   return invertible;
}

int32_t DSPLIB_qrd_check_decomposition(int32_t  testID,
                                       uint32_t dataType,
                                       int32_t  NRows,
                                       int32_t  NCols,
                                       void    *A,
                                       void    *Q,
                                       void    *R,
                                       char     print_txt[16],
                                       int32_t  strideA,
                                       int32_t  strideQ,
                                       int32_t  strideR)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t statusDecomp;
   if (dataType == DSPLIB_FLOAT32) {

      statusDecomp = DSPLIB_qrd_check_decomposition_core<float>(testID, NRows, NCols, (float *) A, (float *) Q,
                                                                (float *) R, print_txt, strideA, strideQ, strideR);
   }
   else {

      statusDecomp = DSPLIB_qrd_check_decomposition_core<double>(testID, NRows, NCols, (double *) A, (double *) Q,
                                                                 (double *) R, print_txt, strideA, strideQ, strideR);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with statusDecomp: %d\n", statusDecomp);
   return statusDecomp;
}

int32_t DSPLIB_qrd_check_transformation(int32_t  testID,
                                        uint32_t dataType,
                                        int32_t  NRows,
                                        void    *Q,
                                        char     print_txt[16],
                                        int32_t  strideQ)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t statusTransform;
   if (dataType == DSPLIB_FLOAT32) {

      statusTransform = DSPLIB_qrd_check_transformation_core<float>(testID, NRows, (float *) Q, print_txt, strideQ);
   }
   else {

      statusTransform = DSPLIB_qrd_check_transformation_core<double>(testID, NRows, (double *) Q, print_txt, strideQ);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with statusTransform: %d\n", statusTransform);
   return statusTransform;
}

int32_t DSPLIB_qrd_check_invertibility(uint32_t dataType, int32_t NRows, int32_t NCols, void *R, int32_t strideR)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t invertible;
   if (dataType == DSPLIB_FLOAT32) {

      invertible = DSPLIB_qrd_check_invertibility_core<float>(NRows, NCols, (float *) R, strideR);
   }
   else {

      invertible = DSPLIB_qrd_check_invertibility_core<double>(NRows, NCols, (double *) R, strideR);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with invertible: %d\n", invertible);
   return invertible;
}

int32_t DSPLIB_qrd_check_solution(int32_t  testID,
                                  uint32_t dataType,
                                  int32_t  NRows,
                                  int32_t  NCols,
                                  void    *A,
                                  void    *x,
                                  void    *b,
                                  char     print_txt[16],
                                  int32_t  strideA)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t statusSolv;
   if (dataType == DSPLIB_FLOAT32) {

      statusSolv = DSPLIB_qrd_check_solution_core<float>(testID, NRows, NCols, (float *) A, (float *) x, (float *) b,
                                                         print_txt, strideA);
   }
   else {

      statusSolv = DSPLIB_qrd_check_solution_core<double>(testID, NRows, NCols, (double *) A, (double *) x,
                                                          (double *) b, print_txt, strideA);
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with statusSolv: %d\n", statusSolv);
   return statusSolv;
}

int32_t DSPLIB_qrd_check_inverse(int32_t  testID,
                                 uint32_t dataType,
                                 int32_t  NRows,
                                 int32_t  NCols,
                                 void    *A,
                                 void    *InvA,
                                 char     print_txt[16],
                                 int32_t  strideA,
                                 int32_t  strideInvA)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t statusInv;
   if (dataType == DSPLIB_FLOAT32) {

      statusInv = DSPLIB_qrd_check_inverse_core<float>(testID, NRows, NCols, (float *) A, (float *) InvA, print_txt,
                                                       strideA, strideInvA);
   }
   else {

      statusInv = DSPLIB_qrd_check_inverse_core<double>(testID, NRows, NCols, (double *) A, (double *) InvA, print_txt,
                                                        strideA, strideInvA);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with statusInv: %d\n", statusInv);
   return statusInv;
}
