// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cholesky_solver_priv.h"

template <typename dataType>
DSPLIB_STATUS
DSPLIB_cholesky_solver_cn(const int order, dataType *U, dataType *y, dataType *b, dataType *x, int32_t colLStride)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   short    i   = 0;
   short    j   = 0;
   dataType sum = 0;

   /* FORWARD SUBSTITUTION */
   for (i = 0; i < order; i++) {
      sum = 0.0;
      for (j = 0; j < i; j++) {
         sum += ((dataType) U[i + j * colLStride]) * y[j];
      }
      y[i] = (b[i] - sum) / ((dataType) U[i + i * colLStride]);
   }

   /* BACKWARD SUBSTITUTION */
   for (i = order - 1; i >= 0; i--) {
      sum = 0.0;
      for (j = i + 1; j < order; j++) {
         sum += ((dataType) U[j + i * colLStride]) * x[j];
      }
      x[i] = (y[i] - sum) / ((dataType) U[i + i * colLStride]);
   }

   return DSPLIB_SUCCESS;
}
template DSPLIB_STATUS
DSPLIB_cholesky_solver_cn<float>(const int order, float *L, float *y, float *b, float *x, int32_t colLStride);
template DSPLIB_STATUS
DSPLIB_cholesky_solver_cn<double>(const int order, double *L, double *y, double *b, double *x, int32_t colLStride);

/*Natural C implementation*/
template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_solver_exec_cn(DSPLIB_kernelHandle handle,
                                             void *restrict pU,
                                             void *restrict pScratch,
                                             void *restrict pY,
                                             void *restrict pB,
                                             void *restrict pX,
                                             void *restrict pDiv)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_cholesky_solver_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_solver_PrivArgs *) handle;
   int32_t                          order        = pKerPrivArgs->order;
   int32_t                          strideL      = pKerPrivArgs->strideCn;
   int32_t                          colLStride   = strideL / sizeof(dataType);

   dataType *pLocalU = (dataType *) pU;
   dataType *pLocalY = (dataType *) pY;
   dataType *pLocalB = (dataType *) pB;
   dataType *pLocalX = (dataType *) pX;

   DSPLIB_STATUS status = DSPLIB_cholesky_solver_cn<dataType>(order, pLocalU, pLocalY, pLocalB, pLocalX, colLStride);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_cholesky_solver_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                             void *restrict pU,
                                                             void *restrict pScratch,
                                                             void *restrict pY,
                                                             void *restrict pB,
                                                             void *restrict pX,
                                                             void *restrict pDiv);

template DSPLIB_STATUS DSPLIB_cholesky_solver_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                              void *restrict pU,
                                                              void *restrict pScratch,
                                                              void *restrict pY,
                                                              void *restrict pB,
                                                              void *restrict pX,
                                                              void *restrict pDiv);
