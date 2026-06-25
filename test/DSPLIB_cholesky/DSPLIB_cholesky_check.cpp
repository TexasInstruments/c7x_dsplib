// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cholesky_check.h"
#include "../common/DSPLIB_test.h"

template <typename dataType>
int32_t cholesky_check_solution_core(int32_t   testId,
                                     int32_t   order,
                                     dataType *A,
                                     dataType *X,
                                     dataType *B,
                                     char     *label,
                                     int32_t   colAStride)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   /* check A*x_cn=b */
   int32_t  pass                = 1;
   dataType tolerance           = 0.01;
   dataType total_percent_error = 0.0;
   dataType error, percent_error;
   int32_t  i, j;
   dataType bMax = 0, bCalcMax = 0, percentErrorMax = 0;
   for (i = 0; i < order; i++) {
      dataType b_calc = 0.0;
      for (j = 0; j < order; j++) {
         b_calc += A[i * colAStride + j] * X[j];
      }
      error         = fabs(b_calc - B[i]);
      percent_error = 100.0 * error / fabs(B[i]);
      if (percent_error >= tolerance) {
         if (percent_error > percentErrorMax) {
            bMax            = B[i];
            bCalcMax        = b_calc;
            percentErrorMax = percent_error;
         }
         pass = 0;
         break;
      }
      total_percent_error += percent_error;
   }
   if (pass == 1) {
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_PASS %d\n", TI_TEST_KERNEL_PASS);
      return TI_TEST_KERNEL_PASS;
   }
   else {
      printf("[%d] %s A*x=b test failure at i=%d b_calc=%e B[%d]=%e  percent_error=%e\n", testId, label, i, bCalcMax, i,
             bMax, percentErrorMax);
      DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: TI_TEST_KERNEL_FAIL %d\n", TI_TEST_KERNEL_FAIL);

      return TI_TEST_KERNEL_FAIL;
   }
}

int32_t cholesky_check_solution(int32_t  testId,
                                uint32_t dataType,
                                int32_t  order,
                                void    *A,
                                void    *X,
                                void    *B,
                                char    *label,
                                int32_t  colAStride)
{
   if (dataType == DSPLIB_FLOAT32) {
      return cholesky_check_solution_core<float>(testId, order, (float *) A, (float *) X, (float *) B, label,
                                                 colAStride);
   }
   else {
      return cholesky_check_solution_core<double>(testId, order, (double *) A, (double *) X, (double *) B, label,
                                                  colAStride);
   }
}
