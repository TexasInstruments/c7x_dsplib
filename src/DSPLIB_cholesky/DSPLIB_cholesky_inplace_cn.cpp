// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cholesky_inplace_priv.h"

// #define ENABLE_ORIG_IN_PLACE

/*Natural C implementation*/
template <typename dataType>
DSPLIB_STATUS
DSPLIB_cholesky_in_place_core_cn(dataType *A, const int32_t order, const int32_t colAStride, const int32_t enable_test)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   short         i, j, k;
   dataType      sum    = 0.0;
   dataType      sum1   = 0.0;
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (enable_test) {
      /* test A for positive definite matrix:    */
      /* z_transpose*A*z>0 where z=1,2,...order  */
      sum1 = 0;
      for (i = 0; i < order; i++) {
         sum = 0;
         for (j = 0; j < order; j++) {
            sum += A[i * colAStride + j] * (dataType) (j + 1);
         }
         sum1 += (dataType) (i + 1) * sum;
      }
      if (sum1 <= 0) {
         status = DSPLIB_ERR_FAILURE;
         DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
      }
   }

#ifdef ENABLE_ORIG_IN_PLACE

   /* cholesky outer product version from p 145 Matrix Computations by Golub and Loan */
   for (k = 0; k < order; k++) {
      A[k * colAStride + k] = (dataType) sqrt(A[k * colAStride + k]);
      for (i = k + 1; i < order; i++) {
         A[i * colAStride + k] = A[i * colAStride + k] / A[k * colAStride + k];
      }
      for (j = k + 1; j < order; j++) {
         for (i = j; i < order; i++) {
            A[i * colAStride + j] -= A[i * colAStride + k] * A[j * colAStride + k];
         }
      }
   }

#else

   if (status == DSPLIB_SUCCESS) {
      /* slight mod from Ran Katzur */
      /* The result is stored in upper triangular matrix */
      for (k = 0; k < order; k++) {
         A[k + k * colAStride] = (dataType) sqrt(A[k + k * colAStride]);
         for (i = k + 1; i < order; i++) {
            A[i + k * colAStride] /= A[k + k * colAStride];
            for (j = k + 1; j <= i; j++) {
               A[i + j * colAStride] -= A[i + k * colAStride] * A[j + k * colAStride];
            }
         }
      }
   }
#endif
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_in_place_core_cn<float>(float        *A,
                                                               const int32_t order,
                                                               const int32_t colAStride,
                                                               const int32_t enable_test);
template DSPLIB_STATUS DSPLIB_cholesky_in_place_core_cn<double>(double       *A,
                                                                const int32_t order,
                                                                const int32_t colAStride,
                                                                const int32_t enable_test);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_inplace_exec_cn(DSPLIB_kernelHandle handle, void *restrict pA, void *restrict pMul)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_cholesky_inplace_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_inplace_PrivArgs *) handle;

   dataType *pLocalA = (dataType *) pA;

   int32_t order      = pKerPrivArgs->order;
   int32_t strideA    = pKerPrivArgs->stride;
   int32_t colAStride = strideA / sizeof(dataType);

   int32_t enable_test = pKerPrivArgs->enableTest;

   status = DSPLIB_cholesky_in_place_core_cn<dataType>(pLocalA, order, colAStride, enable_test);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_cholesky_inplace_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pA, void *restrict pMul);

template DSPLIB_STATUS
DSPLIB_cholesky_inplace_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pInA, void *restrict pMul);
