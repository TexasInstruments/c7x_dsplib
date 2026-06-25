// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cholesky_priv.h"

/*Natural C implementation*/

// Functions to perform cholesky decomposition
template <typename dataType>
DSPLIB_STATUS DSPF_sp_cholesky_cn(int enable_test, DSPLIB_cholesky_PrivArgs *pKerPrivArgs, dataType *A, dataType *U)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   int32_t       i, j, k;
   double        sum, sum1;
   int32_t       order     = pKerPrivArgs->order;
   int32_t       yStrideCn = pKerPrivArgs->strideCn / sizeof(dataType);
   int32_t       yStride   = pKerPrivArgs->stride / sizeof(dataType);

   // Seems to be wrong
   if (enable_test) {
      /* test A for positive definite matrix:    */
      /* z_transpose*A*z>0 where z=1,2,...order  */
      sum1 = 0;
      for (i = 0; i < order; i++) {
         sum = 0;
         for (j = 0; j < order; j++) {
            sum += (dataType) A[i * yStride + j] * (dataType) (j + 1);
         }
         sum1 += (dataType) (i + 1) * sum;
      }
      if (sum1 <= 0) {
         status = DSPLIB_ERR_FAILURE;
      }
   }

   if (status == DSPLIB_SUCCESS) {
      /* generate upper diagonal matrix L */
      for (j = 0; j < order; j++) // j represents col
      {
         /* diagonal entry */
         sum = 0.0;
         for (k = 0; k <= j - 1; k++) {
            sum += U[k * yStrideCn + j] * U[k * yStrideCn + j];
         }

         U[j * yStrideCn + j] = (dataType) sqrt((dataType) A[j * yStride + j] - sum);

         /* lower triangular entries */
         for (i = j + 1; i < order; i++) {
            sum = 0.0;
            for (k = 0; k <= j - 1; k++) {
               sum += U[k * yStrideCn + i] * U[k * yStrideCn + j];
            }
            U[j * yStrideCn + i] = ((dataType) A[j * yStride + i] - sum) / U[j * yStrideCn + j];
         }
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_exec_cn(DSPLIB_kernelHandle handle,
                                      void *restrict pInA,
                                      void *restrict pOutU,
                                      void *restrict multiplierPtr)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;
   dataType                 *pInALocal    = (dataType *) pInA;
   dataType                 *pOutULocal   = (dataType *) pOutU;
   DSPLIB_STATUS             status       = DSPLIB_ERR_FAILURE;
   int32_t                   enable_test  = pKerPrivArgs->enableTest;
   status                                 = DSPF_sp_cholesky_cn(enable_test, pKerPrivArgs, pInALocal, pOutULocal);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

// explicit insaddtiation for the different data type versions
template DSPLIB_STATUS DSPLIB_cholesky_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                      void *restrict pInA,
                                                      void *restrict pOutU,
                                                      void *restrict multiplierPtr);

template DSPLIB_STATUS DSPLIB_cholesky_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                       void *restrict pInA,
                                                       void *restrict pOutU,
                                                       void *restrict multiplierPtr);
