// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_check.h"
#include "../common/DSPLIB_test.h"

template <typename dataType>
int32_t svd_check_decomposition_core(int       Nrows,
                                     int       Ncols,
                                     dataType *A,
                                     dataType *U,
                                     dataType *diag,
                                     dataType *V,
                                     dataType *U1,
                                     char     *label,
                                     int32_t   colAStride,
                                     int32_t   colUStride,
                                     int32_t   colVStride,
                                     uint32_t  enableReducedForm)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int pass, row, col, k;

   dataType total_error, sum, error, max_err;
   dataType avg_error_SVD;
   dataType tolerance   = 0.05; // tolerance = 0.001;
   dataType maxError    = tolerance;
   dataType maxErrorA   = 0;
   dataType maxErrorSum = 0;

   pass        = 1;
   total_error = 0;
   max_err     = 0;

   if (!enableReducedForm) {
      for (row = 0; row < Nrows; row++) {
         for (col = 0; col < Ncols; col++) {
            if (col < Nrows) {
               U1[col + row * colUStride] = U[col + row * colUStride] * diag[col];
            }
            else {
               U1[col + row * colUStride] = 0;
            }
         }
      }

      for (row = 0; row < Nrows; row++) {
         for (col = 0; col < Ncols; col++) {
            sum = 0;
            for (k = 0; k < Ncols; k++) {
               sum += U1[k + row * colUStride] * V[k + col * colVStride];
            }
            error = fabs(A[col + row * colAStride] - sum);

            if (error > max_err) {
               max_err = error;
            }
            if ((error > tolerance) || (isnan(error))) {
               pass = 0;
               if (maxError < error) {
                  maxError    = error;
                  maxErrorA   = A[col + row * colAStride];
                  maxErrorSum = sum;
               }
               break;
            }
            else {
               total_error += error;
            }
         }
         if (pass == 0) {
            break;
         }
      }
   }
   else {
      if (Nrows >= Ncols) {
         for (row = 0; row < Nrows; row++) {
            for (col = 0; col < Ncols; col++) {
               U1[col + row * colUStride] = U[col + row * colUStride] * diag[col];
            }
         }
         for (row = 0; row < Nrows; row++) {
            for (col = 0; col < Ncols; col++) {
               sum = 0;
               for (k = 0; k < Ncols; k++) {
                  sum += U1[k + row * colUStride] * V[k + col * colVStride];
               }
               error = fabs(A[col + row * colAStride] - sum);
               if (error > max_err) {
                  max_err = error;
               }
               if ((error > tolerance) || (isnan(error))) {
                  pass = 0;
                  if (maxError < error) {
                     maxError    = error;
                     maxErrorA   = A[col + row * colAStride];
                     maxErrorSum = sum;
                  }
                  break;
               }
               else {
                  total_error += error;
               }
            }
            if (pass == 0) {
               break;
            }
         }
      }
      else { /* Nrows<Ncols */
         for (row = 0; row < Nrows; row++) {
            for (col = 0; col < Nrows; col++) {
               U1[col + row * colUStride] = U[col + row * colUStride] * diag[col];
            }
         }
         for (row = 0; row < Nrows; row++) {
            for (col = 0; col < Ncols; col++) {
               sum = 0;
               for (k = 0; k < Nrows; k++) {
                  sum += U1[k + row * colUStride] * V[k + col * colVStride];
               }
               error = fabs(A[col + row * colAStride] - sum);
               if (error > max_err) {
                  max_err = error;
               }
               if ((error > tolerance) || (isnan(error))) {
                  pass = 0;
                  if (maxError < error) {
                     maxError    = error;
                     maxErrorA   = A[col + row * colAStride];
                     maxErrorSum = sum;
                  }
                  break;
               }
               else {
                  total_error += error;
               }
            }
            if (pass == 0) {
               break;
            }
         }
      } /* if (Nrows>=Ncols) */
   }

   avg_error_SVD = total_error / ((dataType) Nrows * (dataType) Ncols);
   DSPLIB_DEBUGPRINTFN(0, "%s Decomposition check, avg_error_SVD: %f max_err: %f PASS/FAIL: %d\n", label, avg_error_SVD,
                       max_err, (int) pass);

   if (pass == 1) {
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_PASS %d\n", TI_TEST_KERNEL_PASS);
      return TI_TEST_KERNEL_PASS;
   }
   else {

      printf("%s Warning Decomposition check, orig=%e  calc=%e  error=%e\n", label, maxErrorA, maxErrorSum, maxError);

      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d\n", TI_TEST_KERNEL_FAIL);
      return TI_TEST_KERNEL_FAIL;
   }
}

template <typename dataType>
int32_t svd_check_transformation_core(int       Nrows,
                                      int       Ncols,
                                      dataType *U,
                                      dataType *V,
                                      int32_t   colUStride,
                                      int32_t   colVStride,
                                      uint32_t  enableReducedForm,
                                      char     *label)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int      row, col, k, width, pass;
   dataType sum, error, maxErrorU, maxErrorV, tolerance;

   pass      = 1;
   tolerance = 0.05; // tolerance = 0.001;
   maxErrorU = tolerance;
   maxErrorV = tolerance;
   /* check U*U'=I */
   if (!enableReducedForm) {
      width = Nrows;
      for (row = 0; row < Nrows; row++) {
         for (col = 0; col < width; col++) {
            sum = 0;
            for (k = 0; k < width; k++) {
               sum += U[k + row * colUStride] * U[k + col * colUStride];
            }

            if (row == col) {
               error = fabs(sum - 1.0);
            }
            else {
               error = fabs(sum);
            }
            if (error > tolerance) {
               if (maxErrorU < error)
                  maxErrorU = error;
               pass = 0;
            }
         }
      }

      /* check V*V'=I */
      for (row = 0; row < Ncols; row++) {
         for (col = 0; col < Ncols; col++) {
            sum = 0;
            for (k = 0; k < Ncols; k++) {
               sum += V[k + row * colVStride] * V[k + col * colVStride];
            }

            if (row == col) {
               error = fabs(sum - 1.0);
            }
            else {
               error = fabs(sum);
            }
            if (error > tolerance) {
               if (maxErrorV < error)
                  maxErrorV = error;
               pass = 0;
            }
         }
      }
   }
   else {
      if (Nrows >= Ncols) { /* check V only */
         /* check V*V'=I */
         for (row = 0; row < Ncols; row++) {
            for (col = 0; col < Ncols; col++) {
               sum = 0;
               for (k = 0; k < Ncols; k++) {
                  sum += V[k + row * colVStride] * V[k + col * colVStride];
               }
               if (row == col) {
                  error = fabs(sum - 1.0);
               }
               else {
                  error = fabs(sum);
               }
               if (error > tolerance) {
                  pass = 0;
                  if (maxErrorU < error)
                     maxErrorU = error;
               }
            }
         }
      }
      else { /* check U only */
         /* check U*U'=I */
         width = Nrows;
         for (row = 0; row < Nrows; row++) {
            for (col = 0; col < width; col++) {
               sum = 0;
               for (k = 0; k < width; k++) {
                  sum += U[k + row * colUStride] * U[k + col * colUStride];
               }
               if (row == col) {
                  error = fabs(sum - 1.0);
               }
               else {
                  error = fabs(sum);
               }
               if (error > tolerance) {
                  pass = 0;
                  if (maxErrorV < error)
                     maxErrorV = error;
               }
            }
         }
      }
   }

   if (pass == 1) {
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_PASS %d\n", TI_TEST_KERNEL_PASS);
      return TI_TEST_KERNEL_PASS;
   }
   else {
      if (maxErrorU > tolerance)
         printf("%s Transformation check warning for U matrix, max error=%e\n", label, maxErrorU);
      if (maxErrorV > tolerance)
         printf("%s Transformation check warning for V matrix, max error=%e\n", label, maxErrorV);

      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d\n", TI_TEST_KERNEL_FAIL);

      return TI_TEST_KERNEL_FAIL;
   }
}

int32_t svd_check_decomposition(uint32_t dataType,
                                int32_t  Nrows,
                                int32_t  Ncols,
                                void    *A,
                                void    *U,
                                void    *diag,
                                void    *V,
                                void    *U1,
                                char    *label,
                                int32_t  colAStride,
                                int32_t  colUStride,
                                int32_t  colVStride,
                                uint32_t enableReducedForm)
{
   if (dataType == DSPLIB_FLOAT32) {
      return svd_check_decomposition_core<float>(Nrows, Ncols, (float *) A, (float *) U, (float *) diag, (float *) V,
                                                 (float *) U1, label, colAStride, colUStride, colVStride,
                                                 enableReducedForm);
   }
   else {
      return svd_check_decomposition_core<double>(Nrows, Ncols, (double *) A, (double *) U, (double *) diag,
                                                  (double *) V, (double *) U1, label, colAStride, colUStride,
                                                  colVStride, enableReducedForm);
   }
}

int32_t svd_check_transformation(uint32_t dataType,
                                 int32_t  Nrows,
                                 int32_t  Ncols,
                                 void    *U,
                                 void    *V,
                                 char    *label,
                                 int32_t  colUStride,
                                 int32_t  colVStride,
                                 uint32_t enableReducedForm)
{

   if (dataType == DSPLIB_FLOAT32) {
      return svd_check_transformation_core<float>(Nrows, Ncols, (float *) U, (float *) V, colUStride, colVStride,
                                                  enableReducedForm, label);
   }
   else {
      return svd_check_transformation_core<double>(Nrows, Ncols, (double *) U, (double *) V, colUStride, colVStride,
                                                   enableReducedForm, label);
   }
}
