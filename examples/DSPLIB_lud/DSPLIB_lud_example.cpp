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

#define LUD_MAXDIM 16

float A[LUD_MAXDIM][LUD_MAXDIM] = {
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

float B[LUD_MAXDIM] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};

float          L[LUD_MAXDIM][LUD_MAXDIM];
float          U[LUD_MAXDIM][LUD_MAXDIM];
unsigned short P[LUD_MAXDIM][LUD_MAXDIM];

float invA[LUD_MAXDIM][LUD_MAXDIM];
float X[LUD_MAXDIM];

float scratch1[LUD_MAXDIM];
float scratch2[LUD_MAXDIM];
float scratch3[3 * LUD_MAXDIM];

int32_t Ncols;
int32_t Nrows;

template <typename dataType> int32_t DSPLIB_lud_isInvertible(int order, dataType *U, const int32_t strideOrder)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int      row, invertible;
   dataType tolerance_invertible = 0.004;

   int32_t dataSize    = sizeof(dataType);
   int32_t orderStride = strideOrder / dataSize;

   invertible = 1;
   for (row = 0; row < order; row++) {
      if (fabs(U[row + row * orderStride]) < tolerance_invertible) {
         invertible = 0;
      }
   }

   return invertible;
}

int main(void)
{

   /*****************************************************************************************
    *                          Example code for LU Decomposition
    * ****************************************************************************************/
   printf("Example for LU Decomposition:\n");
   Ncols = LUD_MAXDIM;
   Nrows = LUD_MAXDIM;

   DSPLIB_bufParams2D_t bufParamsP, bufParamsL, bufParamsU, bufParamsA;

   bufParamsA.dim_x     = Ncols;
   bufParamsA.dim_y     = Nrows;
   bufParamsA.stride_y  = bufParamsA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
   bufParamsA.data_type = DSPLIB_FLOAT32;

   bufParamsP.dim_x     = Ncols;
   bufParamsP.dim_y     = Nrows;
   bufParamsP.stride_y  = bufParamsP.dim_x * DSPLIB_sizeof(DSPLIB_UINT16);
   bufParamsP.data_type = DSPLIB_UINT16;

   bufParamsL.dim_x     = Nrows;
   bufParamsL.dim_y     = Nrows;
   bufParamsL.stride_y  = bufParamsL.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
   bufParamsL.data_type = DSPLIB_FLOAT32;

   bufParamsU.dim_x     = Ncols;
   bufParamsU.dim_y     = Nrows;
   bufParamsU.stride_y  = bufParamsU.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
   bufParamsU.data_type = DSPLIB_FLOAT32;

   DSPLIB_ludInitArgs  kerInitArgs;
   int32_t             handleSize = DSPLIB_lud_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   kerInitArgs.order     = Ncols;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t status = DSPLIB_ERR_FAILURE;
   status = DSPLIB_lud_init_checkParams(handle, &bufParamsA, &bufParamsL, &bufParamsU, &bufParamsP, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_lud_init(handle, &bufParamsA, &bufParamsL, &bufParamsU, &bufParamsP, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_lud_exec_checkParams(handle, &A, &L, &U, &P);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_lud_exec(handle, &A, &L, &U, &P);
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

   printf("Decompostion of P * A = L * U\n");

   printf("Output Matrix P:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f ", (float) P[i][j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("Output Matrix L:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f ", L[i][j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("Output Matrix U:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f ", U[i][j]);
      }
      printf("\n");
   }
   printf("\n");
   free(handle);

   /*****************************************************************************************
    *                          Example code for LUD Solver
    * ****************************************************************************************/

   int32_t invertible = DSPLIB_lud_isInvertible<float>(bufParamsU.dim_y, (float *) U, bufParamsU.stride_y);

   /* First check if Input matrix R is invertible */
   if (invertible) {

      DSPLIB_bufParams1D_t bufParamsB, bufParamsB_Mod, bufParamsY, bufParamsX;
      DSPLIB_bufParams2D_t bufParamsVecScratch, bufParamsScratchTrans;

      bufParamsB.dim_x     = Nrows;
      bufParamsB.data_type = DSPLIB_FLOAT32;

      bufParamsB_Mod.dim_x     = Nrows;
      bufParamsB_Mod.data_type = DSPLIB_FLOAT32;

      bufParamsY.dim_x     = Nrows;
      bufParamsY.data_type = DSPLIB_FLOAT32;

      bufParamsX.dim_x     = Nrows;
      bufParamsX.data_type = DSPLIB_FLOAT32;

      bufParamsVecScratch.dim_x     = Ncols;
      bufParamsVecScratch.dim_y     = Nrows;
      bufParamsVecScratch.stride_y  = bufParamsU.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
      bufParamsVecScratch.data_type = DSPLIB_FLOAT32;

      bufParamsScratchTrans.dim_x     = Ncols;
      bufParamsScratchTrans.dim_y     = Nrows;
      bufParamsScratchTrans.stride_y  = bufParamsU.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
      bufParamsScratchTrans.data_type = DSPLIB_FLOAT32;

      DSPLIB_lud_solInitArgs kerInitArgsSolv;
      int32_t                handleSolvSize = DSPLIB_lud_sol_getHandleSize(&kerInitArgsSolv);
      DSPLIB_kernelHandle    handleSolv     = malloc(handleSolvSize);

      kerInitArgsSolv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
      kerInitArgsSolv.order     = Ncols;

      status =
          DSPLIB_lud_sol_init_checkParams(handleSolv, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB, &bufParamsX,
                                          &bufParamsVecScratch, &bufParamsScratchTrans, &kerInitArgsSolv);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_lud_sol_init(handleSolv, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsB, &bufParamsX,
                                      &bufParamsVecScratch, &bufParamsScratchTrans, &kerInitArgsSolv);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_lud_sol_exec_checkParams(handleSolv, P, L, U, B, X, scratch3, invA);

            if (status == DSPLIB_SUCCESS) {
               status = DSPLIB_lud_sol_exec(handleSolv, P, L, U, B, X, scratch3, invA);
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
    *                          Example code for LUD Inverse
    * ****************************************************************************************/

   if (invertible) {
      DSPLIB_bufParams2D_t bufParamsInvA, bufParamsScratch;
      bufParamsInvA.dim_x     = Ncols;
      bufParamsInvA.dim_y     = Nrows;
      bufParamsInvA.stride_y  = bufParamsInvA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
      bufParamsInvA.data_type = DSPLIB_FLOAT32;

      bufParamsScratch.dim_x     = Ncols;
      bufParamsScratch.dim_y     = Nrows;
      bufParamsScratch.stride_y  = bufParamsScratch.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
      bufParamsScratch.data_type = DSPLIB_FLOAT32;

      DSPLIB_lud_invInitArgs kerInitArgsInv;
      int32_t                handleInvSize = DSPLIB_lud_inv_getHandleSize(&kerInitArgsInv);
      DSPLIB_kernelHandle    handleInv     = malloc(handleInvSize);

      kerInitArgsInv.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
      kerInitArgsInv.order     = Ncols;

      status = DSPLIB_lud_inv_init_checkParams(handleInv, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsInvA,
                                               &kerInitArgsInv);
      if (status == DSPLIB_SUCCESS) {
         status =
             DSPLIB_lud_inv_init(handleInv, &bufParamsP, &bufParamsL, &bufParamsU, &bufParamsInvA, &kerInitArgsInv);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_lud_inv_exec_checkParams(handleInv, P, L, U, invA, scratch2);

            if (status == DSPLIB_SUCCESS) {
               status = DSPLIB_lud_inv_exec(handleInv, P, L, U, invA, scratch2);
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
   printf("End of LUD Example \n");
   return 0;
}
