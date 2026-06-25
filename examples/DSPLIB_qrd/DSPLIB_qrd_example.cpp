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

#define QRD_MAXDIM 16

float A[QRD_MAXDIM][QRD_MAXDIM] = {
    {-0.161, -0.155, 0.692, -0.818, 0.372, 0.908, -0.585, -0.186, -0.224, 0.299, 0.906, -0.051, -0.564, 0.725, 0.050,
     0.189},
    {0.867, -0.611, 0.465, 0.150, 0.780, 0.698, 0.885, 0.477, -0.152, -0.410, 0.751, 0.963, -0.999, 0.622, 0.258,
     -0.159},
    {-0.533, -0.049, 0.022, 0.839, -0.142, 0.437, -0.347, 0.634, -0.264, -0.440, -0.416, 0.172, -0.715, 0.633, -0.639,
     -0.848},
    {-0.977, 0.827, 0.303, 0.803, 0.525, 0.188, 0.279, -0.628, 0.778, 0.030, -0.665, 0.779, -0.348, 0.593, -0.381,
     0.119},
    {-0.456, 0.642, -0.042, 0.402, 0.079, 0.612, 0.036, 0.815, -0.829, 0.620, -0.013, -0.544, 0.253, 0.349, -0.391,
     0.276},
    {0.175, 0.911, 0.079, -0.300, 0.099, -0.641, 0.072, -0.123, 0.389, 0.407, -0.344, -0.959, 0.000, 0.275, 0.160,
     0.544},
    {-0.083, -0.881, -0.054, 0.996, 0.730, 0.982, 0.811, 0.902, 0.602, -0.202, -0.642, -0.144, -0.853, -0.033, -0.868,
     0.322},
    {-0.122, 0.211, -0.978, 0.978, 0.569, 0.094, -0.146, -0.042, -0.499, 0.510, -0.001, 0.502, -0.215, -0.841, 0.046,
     0.702},
    {-0.722, 0.992, 0.697, -0.991, 0.974, 0.508, 0.910, 0.576, -0.694, -0.731, -0.568, -0.547, 0.236, -0.436, 0.775,
     -0.886},
    {0.775, 0.797, -0.908, 0.344, -0.109, -0.054, -0.698, 0.392, -0.544, 0.302, -0.106, 0.241, 0.461, 0.940, -0.057,
     0.739},
    {0.932, -0.360, 0.748, 0.906, -0.852, 0.658, 0.482, -0.546, 0.927, 0.914, -0.093, 0.163, -0.522, -0.318, 0.277,
     -0.747},
    {-0.521, 0.368, 0.597, 0.370, -0.686, 0.900, -0.237, -0.230, 0.201, 0.657, -0.989, -0.338, 0.597, -0.046, -0.599,
     0.528},
    {0.594, -0.851, 0.434, 0.742, 0.807, -0.084, -0.804, 0.734, -0.170, 0.104, -0.104, 0.308, 0.786, -0.827, 0.561,
     -0.735},
    {0.541, 0.158, 0.636, 0.855, 0.058, -0.602, -0.375, -0.741, -0.945, -0.364, -0.079, 0.652, 0.590, 0.323, 0.180,
     0.184},
    {0.472, -0.386, -0.074, 0.279, 0.529, 0.123, 0.012, -0.641, -0.774, 0.909, 0.667, -0.987, -0.918, 0.228, -0.722,
     0.623},
    {-0.614, 0.914, 0.478, 0.444, -0.688, -0.897, 0.703, -0.633, -0.260, -0.376, -0.981, -0.670, 0.947, 0.199, 0.514,
     0.419},
};

float B[QRD_MAXDIM] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};

float Q[QRD_MAXDIM][QRD_MAXDIM];
float R[QRD_MAXDIM][QRD_MAXDIM];
float invA[QRD_MAXDIM][QRD_MAXDIM];
float X[QRD_MAXDIM];

float scratch1[QRD_MAXDIM];
float scratch2[QRD_MAXDIM];
float scratch3[QRD_MAXDIM][QRD_MAXDIM];

int32_t Ncols;
int32_t Nrows;

template <typename dataType> int32_t DSPLIB_qrd_isInvertible(int32_t Nrows, int32_t Ncols, dataType *R, int32_t strideR)
{
   dataType tolerance  = (sizeof(dataType) == 8) ? (0.000001) : (0.00001);
   int32_t  invertible = 0;
   int32_t  colRStride = strideR / sizeof(dataType);
   if (Nrows == Ncols) {
      invertible = 1;
      for (int32_t row = 0; row < Nrows; row++) {
         if (fabs(R[row + row * colRStride]) < tolerance) {
            invertible = 0;
         }
      }
   }
   return invertible;
}

int main(void)
{

   /*****************************************************************************************
    *                          Example code for QR Decomposition
    * ****************************************************************************************/
   printf("Example for QR Decomposition:\n");
   Ncols          = QRD_MAXDIM;
   Nrows          = QRD_MAXDIM;
   int32_t maxDim = (Nrows >= Ncols) ? Nrows : Ncols;

   DSPLIB_bufParams1D_t bufParamsScratch1, bufParamsScratch2;
   DSPLIB_bufParams2D_t bufParamsA, bufParamsQ, bufParamsR;

   bufParamsA.dim_x     = Ncols;
   bufParamsA.dim_y     = Nrows;
   bufParamsA.stride_y  = bufParamsA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
   bufParamsA.data_type = DSPLIB_FLOAT32;

   bufParamsQ.dim_x     = Nrows;
   bufParamsQ.dim_y     = Nrows;
   bufParamsQ.stride_y  = bufParamsQ.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
   bufParamsQ.data_type = DSPLIB_FLOAT32;

   bufParamsR.dim_x     = Ncols;
   bufParamsR.dim_y     = Nrows;
   bufParamsR.stride_y  = bufParamsR.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
   bufParamsR.data_type = DSPLIB_FLOAT32;

   bufParamsScratch1.dim_x     = maxDim;
   bufParamsScratch1.data_type = DSPLIB_FLOAT32;

   bufParamsScratch2.dim_x     = maxDim;
   bufParamsScratch2.data_type = DSPLIB_FLOAT32;

   DSPLIB_qrdInitArgs  kerInitArgs;
   int32_t             handleSize = DSPLIB_qrd_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   kerInitArgs.dimX      = Ncols;
   kerInitArgs.dimY      = Nrows;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t status = DSPLIB_ERR_FAILURE;
   status =
       DSPLIB_qrd_init_checkParams(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsScratch1, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_qrd_init(handle, &bufParamsA, &bufParamsQ, &bufParamsR, &bufParamsScratch1, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_qrd_exec_checkParams(handle, &A, &Q, &R, &scratch1, &scratch2);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_qrd_exec(handle, &A, &Q, &R, &scratch1, &scratch2);
         }
      }
   }

   printf("Input Matrix A:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f ", A[i][j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("Decompostion of A = Q * R\n");

   printf("Output Matrix Q:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Nrows; j++) {
         printf("%7.3f ", Q[i][j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("Output Matrix R:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f ", R[i][j]);
      }
      printf("\n");
   }
   printf("\n");
   free(handle);

   /*****************************************************************************************
    *                          Example code for QRD Solver
    * ****************************************************************************************/

   int32_t invertible =
       DSPLIB_qrd_isInvertible<float>(bufParamsR.dim_y, bufParamsR.dim_x, (float *) R, bufParamsR.stride_y);

   /* First check if Input matrix R is invertible */
   if (invertible) {
      DSPLIB_bufParams1D_t bufParamsB, bufParamsX;
      DSPLIB_bufParams2D_t bufParamsScratch3;

      bufParamsB.dim_x     = Nrows;
      bufParamsB.data_type = DSPLIB_FLOAT32;

      bufParamsX.dim_x     = Nrows;
      bufParamsX.data_type = DSPLIB_FLOAT32;

      bufParamsScratch3.dim_x     = Ncols;
      bufParamsScratch3.dim_y     = Nrows;
      bufParamsScratch3.stride_y  = bufParamsScratch3.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
      bufParamsScratch3.data_type = DSPLIB_FLOAT32;

      DSPLIB_qrdSolverInitArgs kerInitArgsSolv;
      int32_t                  handleSolvSize = DSPLIB_qrd_solver_getHandleSize(&kerInitArgsSolv);
      DSPLIB_kernelHandle      handleSolv     = malloc(handleSolvSize);

      kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
      kerInitArgsSolv.dimX      = Ncols;
      kerInitArgsSolv.dimY      = Nrows;

      status = DSPLIB_qrd_solver_init_checkParams(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsScratch1,
                                                  &bufParamsX, &kerInitArgsSolv);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_qrd_solver_init(handleSolv, &bufParamsQ, &bufParamsR, &bufParamsB, &bufParamsScratch1,
                                         &bufParamsX, &kerInitArgsSolv);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_qrd_solver_exec_checkParams(handleSolv, &Q, &R, &B, &scratch1, &X, &scratch2, &scratch3);

            if (status == DSPLIB_SUCCESS) {
               status = DSPLIB_qrd_solver_exec(handleSolv, &Q, &R, &B, &scratch1, &X, &scratch2, &scratch3);
            }
         }
      }

      printf("Solution Vector X:\n");
      for (int32_t i = 0; i < bufParamsX.dim_x; i++) {
         printf("%7.3f\n", X[i]);
      }
      printf("\n");

      free(handleSolv);
   }
   else {
      printf("Input R matrix is not invertible\n");
   }

   /*****************************************************************************************
    *                          Example code for QRD Inverse
    * ****************************************************************************************/

   if (invertible) {
      DSPLIB_bufParams2D_t bufParamsInvA, bufParamsScratch3;
      bufParamsInvA.dim_x     = Ncols;
      bufParamsInvA.dim_y     = Nrows;
      bufParamsInvA.stride_y  = bufParamsInvA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
      bufParamsInvA.data_type = DSPLIB_FLOAT32;

      bufParamsScratch3.dim_x     = Ncols;
      bufParamsScratch3.dim_y     = Nrows;
      bufParamsScratch3.stride_y  = bufParamsScratch3.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
      bufParamsScratch3.data_type = DSPLIB_FLOAT32;

      DSPLIB_qrdInvInitArgs kerInitArgsInv;
      int32_t               handleInvSize = DSPLIB_qrd_inverse_getHandleSize(&kerInitArgsInv);
      DSPLIB_kernelHandle   handleInv     = malloc(handleInvSize);

      kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
      kerInitArgsInv.dimX      = Ncols;
      kerInitArgsInv.dimY      = Nrows;

      status = DSPLIB_qrd_inverse_init_checkParams(handleInv, &bufParamsQ, &bufParamsR, &bufParamsScratch3,
                                                   &bufParamsInvA, &kerInitArgsInv);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_qrd_inverse_init(handleInv, &bufParamsQ, &bufParamsR, &bufParamsScratch3, &bufParamsInvA,
                                          &kerInitArgsInv);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_qrd_inverse_exec_checkParams(handleInv, &Q, &R, &invA, &scratch3, scratch1);

            if (status == DSPLIB_SUCCESS) {
               status = DSPLIB_qrd_inverse_exec(handleInv, &Q, &R, &invA, &scratch3, scratch1);
            }
         }
      }

      printf("Inverse of Matrix A:\n");

      for (int32_t i = 0; i < Nrows; i++) {
         for (int32_t j = 0; j < Ncols; j++) {
            printf("%7.3f ", invA[i][j]);
         }
         printf("\n");
      }
      printf("\n");
      free(handleInv);
   }
   else {
      printf("Input R matrix is not invertible. Inverse of A cannot be computed\n");
   }

   printf("End of QRD Example \n");
   return 0;
}
