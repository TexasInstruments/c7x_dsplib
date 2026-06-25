// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <stdint.h>

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

#define SVD_MAXDIM 16
#define SVD_MINDIM 8

/*  TEST CASE FOR Nrows > Ncol */
float A[SVD_MAXDIM][SVD_MINDIM] = {
    {-0.712, -0.359, 0.435, 0.610, 0.794, -0.043, 0.704, -0.875},
    {0.068, 0.528, 0.348, 0.221, -0.650, -0.475, -0.611, -0.587},
    {-0.613, -0.063, -0.312, 0.336, -0.641, 0.152, -0.757, 0.971},
    {-0.194, -0.855, 0.081, 0.873, -0.343, -0.050, -0.860, -0.055},
    {0.591, 0.576, -0.446, 0.385, -0.467, -0.742, 0.509, 0.601},
    {0.786, -0.143, -0.179, -0.864, 0.382, 0.210, -0.451, 0.769},
    {-0.853, 0.238, 0.105, -0.494, -0.610, 0.348, -0.522, 0.196},
    {0.493, 0.559, 0.069, -0.850, -0.492, 0.210, 0.095, -0.901},
    {-0.214, 0.649, 0.484, 0.319, 0.907, -0.007, -0.081, 0.693},
    {0.850, 0.741, 0.830, 0.232, -0.049, -0.621, 0.001, 0.098},
    {0.617, -0.894, 0.604, -0.993, 0.454, -0.918, 0.203, -0.053},
    {0.641, -0.727, 0.097, -0.851, 0.482, -0.808, -0.752, -0.732},
    {0.841, 0.732, 0.587, 0.749, -0.274, -0.494, 0.442, -0.424},
    {-0.753, 0.272, 0.809, 0.197, 0.651, -0.190, -0.705, 0.268},
    {-0.084, 0.900, 0.275, -0.630, 0.982, -0.522, 0.317, 0.622},
    {-0.250, -0.586, 0.771, -0.767, -0.394, -0.980, -0.499, -0.552},
};
float U_A[SVD_MAXDIM][SVD_MAXDIM];
float U1_A[SVD_MAXDIM][SVD_MAXDIM];
float V_A[SVD_MINDIM][SVD_MINDIM];
float V1_A[SVD_MINDIM][SVD_MINDIM];
float diag_A[SVD_MINDIM];
float superDiag_A[SVD_MINDIM];
float scratch_A[4][SVD_MAXDIM];

/*  TEST CASE FOR Nrows < Ncol */
float B[SVD_MINDIM][SVD_MAXDIM] = {
    {-0.149, -0.878, 0.779, -0.048, -0.323, 0.205, -0.403, -0.137, 0.003, 0.227, -0.711, -0.762, -0.979, -0.219, 0.848,
     -0.414},
    {-0.750, -0.036, 0.242, 0.323, -0.403, -0.358, 0.892, 0.659, 0.316, 0.212, 0.474, 0.474, 0.418, -0.514, 0.642,
     -0.730},
    {-0.392, 0.421, 0.222, 0.285, -0.374, 0.819, -0.852, 0.629, 0.046, -0.563, 0.867, 0.066, 0.218, 0.715, 0.653,
     0.468},
    {-0.320, -0.106, -0.208, 0.277, 0.536, -0.317, -0.064, -0.148, 0.895, -0.590, -0.675, 0.313, -0.104, 0.968, 0.583,
     0.504},
    {0.389, -0.195, -0.211, -0.985, -0.376, -0.062, 0.644, 0.670, 0.375, 0.511, -0.264, -0.407, 0.226, -0.611, -0.939,
     0.906},
    {0.283, -0.147, 0.183, -0.181, 0.536, -0.882, 0.671, 0.431, -0.472, 0.997, -0.255, 0.424, 0.964, -0.672, -0.071,
     0.353},
    {0.133, 0.718, 0.368, 0.756, -0.344, 0.011, 0.426, -0.969, -0.478, -0.838, -0.376, 0.748, -0.450, -0.314, 0.654,
     0.833},
    {0.538, -0.164, -0.348, 0.075, -0.046, -0.676, -0.494, 0.483, -0.679, 0.251, -0.093, -0.715, 0.578, 0.836, 0.638,
     -0.289},
};
float U_B[SVD_MAXDIM][SVD_MAXDIM];
float U1_B[SVD_MAXDIM][SVD_MAXDIM];
float V_B[SVD_MAXDIM][SVD_MAXDIM];
float V1_B[SVD_MAXDIM][SVD_MAXDIM];
float diag_B[SVD_MINDIM];
float superDiag_B[SVD_MINDIM];
float scratch_B[4][SVD_MAXDIM];

int32_t Ncols;
int32_t Nrows;

int main(void)
{

   DSPLIB_bufParams1D_t bufParamsDiag, bufParamsSuperDiag;
   DSPLIB_bufParams2D_t bufParamsA, bufParamsU, bufParamsV, bufParamsU1, bufParamsV1, bufParamsScratch;
   DSPLIB_svdInitArgs   kerInitArgs;
   DSPLIB_kernelHandle  handle;
   int32_t              handleSize, status;

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
   handleSize            = DSPLIB_svd_getHandleSize(&kerInitArgs);
   handle                = malloc(handleSize);

   bufParamsDiag.data_type      = DSPLIB_FLOAT32;
   bufParamsSuperDiag.data_type = DSPLIB_FLOAT32;
   bufParamsA.data_type         = DSPLIB_FLOAT32;
   bufParamsU.data_type = bufParamsU1.data_type = DSPLIB_FLOAT32;
   bufParamsV.data_type = bufParamsV1.data_type = DSPLIB_FLOAT32;
   bufParamsScratch.data_type                   = DSPLIB_FLOAT32;

   /* *****************************************************************************************
   *********************************** Example for Nrows < Ncols ******************************
   ******************************************************************************************** */

   Ncols = SVD_MINDIM;
   Nrows = SVD_MAXDIM;

   bufParamsDiag.dim_x = Ncols;

   bufParamsSuperDiag.dim_x = Ncols;

   bufParamsA.dim_x    = Ncols;
   bufParamsA.dim_y    = Nrows;
   bufParamsA.stride_y = bufParamsA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);

   bufParamsU.dim_x = bufParamsU1.dim_x = Nrows;
   bufParamsU.dim_y = bufParamsU1.dim_y = Nrows;
   bufParamsU.stride_y = bufParamsU1.stride_y = bufParamsU.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);

   bufParamsV.dim_x = bufParamsV1.dim_x = Ncols;
   bufParamsV.dim_y = bufParamsV1.dim_y = Ncols;
   bufParamsV.stride_y = bufParamsV1.stride_y = bufParamsV.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);

   bufParamsScratch.dim_x    = Nrows;
   bufParamsScratch.dim_y    = 4;
   bufParamsScratch.stride_y = bufParamsScratch.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);

   kerInitArgs.dimX        = Ncols;
   kerInitArgs.dimY        = Nrows;
   kerInitArgs.strideURows = bufParamsU.stride_y;
   kerInitArgs.strideVRows = bufParamsV.stride_y;

   /* Non-reduced form */
   kerInitArgs.enableReducedForm = 0;

   status = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                        &bufParamsSuperDiag, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperDiag,
                               &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_svd_exec_checkParams(handle, &A, &U_A, &V_A, &diag_A, &superDiag_A, &U1_A, &V1_A, &scratch_A);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_svd_exec(handle, &A, &U_A, &V_A, &diag_A, &superDiag_A, &U1_A, &V1_A, &scratch_A);
         }
      }
   }

   printf("\n\nExample for Nrows > Ncols\nInput Matrix A[Nrows X Ncols] => A[%d X %d]\n", Nrows, Ncols);
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f ", A[i][j]);
      }
      printf(";\n");
   }

   printf("\nOutput in non-reduced form:\n");
   printf("Matrix U: \n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Nrows; j++) {
         printf("%7.3f, ", U_A[i][j]);
      }
      printf("\n");
   }

   printf("\nSingular values:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      printf("%7.3f, ", diag_A[i]);
   }
   printf("\n");

   printf("\nMatrix V:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f, ", V_A[i][j]);
      }
      printf("\n");
   }

   /* Reduced form */
   kerInitArgs.enableReducedForm = 1;

   status = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                        &bufParamsSuperDiag, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperDiag,
                               &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_svd_exec_checkParams(handle, &A, &U_A, &V_A, &diag_A, &superDiag_A, &U1_A, &V1_A, &scratch_A);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_svd_exec(handle, &A, &U_A, &V_A, &diag_A, &superDiag_A, &U1_A, &V1_A, &scratch_A);
         }
      }
   }

   printf("\nOutput in reduced form:\n");
   printf("Matrix U: \n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f, ", U_A[i][j]);
      }
      printf("\n");
   }

   printf("\nSingular values:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      printf("%7.3f, ", diag_A[i]);
   }
   printf("\n");

   printf("\nMatrix V:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f, ", V_A[i][j]);
      }
      printf("\n");
   }

   /* *****************************************************************************************
   *********************************** Example for Nrows < Ncols ******************************
   ******************************************************************************************** */

   Ncols = SVD_MAXDIM;
   Nrows = SVD_MINDIM;

   bufParamsDiag.dim_x = Nrows;

   bufParamsSuperDiag.dim_x = Nrows;

   bufParamsA.dim_x    = Ncols;
   bufParamsA.dim_y    = Nrows;
   bufParamsA.stride_y = bufParamsA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);

   bufParamsU.dim_x = bufParamsU1.dim_x = Ncols;
   bufParamsU.dim_y = bufParamsU1.dim_y = Ncols;
   bufParamsU.stride_y = bufParamsU1.stride_y = bufParamsU.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);

   bufParamsV.dim_x = bufParamsV1.dim_x = Ncols;
   bufParamsV.dim_y = bufParamsV1.dim_y = Ncols;
   bufParamsV.stride_y = bufParamsV1.stride_y = bufParamsV.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);

   bufParamsScratch.dim_x    = Ncols;
   bufParamsScratch.dim_y    = 4;
   bufParamsScratch.stride_y = bufParamsScratch.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);

   kerInitArgs.dimX        = Ncols;
   kerInitArgs.dimY        = Nrows;
   kerInitArgs.strideURows = bufParamsU.stride_y;
   kerInitArgs.strideVRows = bufParamsV.stride_y;

   /* Non-reduced form */
   kerInitArgs.enableReducedForm = 0;

   status = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                        &bufParamsSuperDiag, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperDiag,
                               &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_svd_exec_checkParams(handle, &B, &U_B, &V_B, &diag_B, &superDiag_B, &U1_B, &V1_B, &scratch_B);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_svd_exec(handle, &B, &U_B, &V_B, &diag_B, &superDiag_B, &U1_B, &V1_B, &scratch_B);
         }
      }
   }

   printf("\n\nExample for Nrows > Ncols\nInput Matrix B[Nrows X Ncols] => B[%d X %d]\n", Nrows, Ncols);
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f ", B[i][j]);
      }
      printf(";\n");
   }

   printf("\nOutput in non-reduced form:\n");
   printf("Matrix U: \n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Nrows; j++) {
         printf("%7.3f, ", U_B[i][j]);
      }
      printf("\n");
   }

   printf("\nSingular values:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      printf("%7.3f, ", diag_B[i]);
   }
   printf("\n");

   printf("\nMatrix V:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f, ", V_B[i][j]);
      }
      printf("\n");
   }

   /* Reduced form */
   kerInitArgs.enableReducedForm = 1;

   status = DSPLIB_svd_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                        &bufParamsSuperDiag, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_svd_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperDiag,
                               &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_svd_exec_checkParams(handle, &B, &U_B, &V_B, &diag_B, &superDiag_B, &U1_B, &V1_B, &scratch_B);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_svd_exec(handle, &B, &U_B, &V_B, &diag_B, &superDiag_B, &U1_B, &V1_B, &scratch_B);
         }
      }
   }

   printf("\nOutput in reduced form:\n");
   printf("Matrix U: \n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Nrows; j++) {
         printf("%7.3f, ", U_B[i][j]);
      }
      printf("\n");
   }

   printf("\nSingular values:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      printf("%7.3f, ", diag_B[i]);
   }
   printf("\n");

   printf("\nMatrix V:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      for (int32_t j = 0; j < Nrows; j++) {
         printf("%7.3f, ", V_B[i][j]);
      }
      printf("\n");
   }

   return 0;
}
