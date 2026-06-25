// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <stdint.h>
#include <string.h>

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

#define ORDER 16

float A[ORDER][ORDER] = {
    {0.597, 0.235, 0.316, -0.163, 0.229, 0.332, -0.903, 0.263, 0.110, 0.047, -0.444, -0.096, -0.627, -0.096, 0.305,
     -0.651},
    {0.235, 0.702, 0.379, -0.215, 0.207, 0.339, -1.035, 0.338, 0.109, 0.006, -0.533, -0.062, -0.804, -0.141, 0.407,
     -0.783},
    {0.316, 0.379, 1.294, -0.358, 0.303, 0.598, -1.953, 0.530, 0.263, 0.142, -0.898, -0.083, -1.459, -0.182, 0.488,
     -1.311},
    {-0.163, -0.215, -0.358, 0.547, -0.201, -0.283, 1.040, -0.380, -0.238, -0.066, 0.479, -0.009, 0.788, 0.107, -0.338,
     0.597},
    {0.229, 0.207, 0.303, -0.201, 0.816, 0.338, -1.214, 0.388, 0.137, 0.028, -0.525, 0.005, -0.871, -0.133, 0.338,
     -0.889},
    {0.332, 0.339, 0.598, -0.283, 0.338, 1.068, -1.878, 0.548, 0.259, -0.001, -0.913, -0.064, -1.390, -0.197, 0.519,
     -1.390},
    {-0.903, -1.035, -1.953, 1.040, -1.214, -1.878, 5.947, -1.832, -0.747, -0.023, 2.520, 0.148, 4.078, 0.471, -1.561,
     3.708},
    {0.263, 0.338, 0.530, -0.380, 0.388, 0.548, -1.832, 1.044, 0.355, -0.045, -0.920, -0.108, -1.289, -0.135, 0.569,
     -1.253},
    {0.110, 0.109, 0.263, -0.238, 0.137, 0.259, -0.747, 0.355, 0.533, 0.018, -0.425, -0.022, -0.557, -0.139, 0.186,
     -0.519},
    {0.047, 0.006, 0.142, -0.066, 0.028, -0.001, -0.023, -0.045, 0.018, 0.452, -0.048, -0.027, -0.152, -0.017, -0.036,
     -0.063},
    {-0.444, -0.533, -0.898, 0.479, -0.525, -0.913, 2.520, -0.920, -0.425, -0.048, 1.580, 0.079, 1.998, 0.196, -0.731,
     1.899},
    {-0.096, -0.062, -0.083, -0.009, 0.005, -0.064, 0.148, -0.108, -0.022, -0.027, 0.079, 0.445, 0.054, -0.064, -0.026,
     0.089},
    {-0.627, -0.804, -1.459, 0.788, -0.871, -1.390, 4.078, -1.289, -0.557, -0.152, 1.998, 0.054, 3.466, 0.370, -1.260,
     2.913},
    {-0.096, -0.141, -0.182, 0.107, -0.133, -0.197, 0.471, -0.135, -0.139, -0.017, 0.196, -0.064, 0.370, 0.407, -0.163,
     0.459},
    {0.305, 0.407, 0.488, -0.338, 0.338, 0.519, -1.561, 0.569, 0.186, -0.036, -0.731, -0.026, -1.260, -0.163, 0.855,
     -1.261},
    {-0.651, -0.783, -1.311, 0.597, -0.889, -1.390, 3.708, -1.253, -0.519, -0.063, 1.899, 0.089, 2.913, 0.459, -1.261,
     3.220},
};

float B[ORDER] = {ORDER, ORDER, ORDER, ORDER, ORDER, ORDER, ORDER, ORDER,
                  ORDER, ORDER, ORDER, ORDER, ORDER, ORDER, ORDER, ORDER};

float L[ORDER][ORDER];
float X[ORDER];
float Y[ORDER];

float scratch1[ORDER];
float scratch2[ORDER][ORDER];
float scratch3[ORDER];
float buffA[ORDER][ORDER];

int32_t Ncols;
int32_t Nrows;

int main(void)
{

   /*****************************************************************************************
    *                          Example code for Cholesky Decomposition
    * ****************************************************************************************/
   printf("Example for Cholesky Decomposition:\n");
   int32_t              order = ORDER;
   DSPLIB_bufParams2D_t bufParamsA, bufParamsL;

   bufParamsA.dim_x     = order;
   bufParamsA.dim_y     = order;
   bufParamsA.stride_y  = bufParamsA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
   bufParamsA.data_type = DSPLIB_FLOAT32;

   bufParamsL.dim_x     = order;
   bufParamsL.dim_y     = order;
   bufParamsL.stride_y  = bufParamsA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32);
   bufParamsL.data_type = DSPLIB_FLOAT32;

   DSPLIB_cholesky_InitArgs kerInitArgs;
   int32_t                  handleSize = DSPLIB_cholesky_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle      handle     = malloc(handleSize);

   kerInitArgs.order     = order;
   kerInitArgs.stride    = bufParamsL.stride_y;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t status = DSPLIB_ERR_FAILURE;
   status         = DSPLIB_cholesky_init_checkParams(handle, &bufParamsA, &bufParamsL, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_cholesky_init(handle, &bufParamsA, &bufParamsL, &kerInitArgs);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_cholesky_exec_checkParams(handle, A, L, scratch1);
         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_cholesky_exec(handle, A, L, scratch1);
         }
      }
   }

   printf("Input Matrix A:\n");
   for (int32_t i = 0; i < order; i++) {
      for (int32_t j = 0; j < order; j++) {
         printf("%6.3f ", A[i][j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("Output Matrix L (upper triangular):\n");
   for (int32_t i = 0; i < order; i++) {
      for (int32_t j = 0; j < order; j++) {
         printf("%6.3f ", L[i][j]);
      }
      printf("\n");
   }
   printf("\n");

   /*****************************************************************************************
    *                          Example code for Cholesky Solver
    * ****************************************************************************************/
   DSPLIB_bufParams2D_t bufParamsScratch2;
   DSPLIB_bufParams1D_t bufParamsY, bufParamsB, bufParamsX, bufParamsScratch3;

   bufParamsScratch2.dim_x     = order;
   bufParamsScratch2.dim_y     = order;
   bufParamsScratch2.stride_y  = bufParamsL.stride_y;
   bufParamsScratch2.data_type = DSPLIB_FLOAT32;

   bufParamsY.dim_x     = order;
   bufParamsY.data_type = DSPLIB_FLOAT32;

   bufParamsB.dim_x     = order;
   bufParamsB.data_type = DSPLIB_FLOAT32;

   bufParamsX.dim_x     = order;
   bufParamsX.data_type = DSPLIB_FLOAT32;

   bufParamsScratch3.dim_x     = order;
   bufParamsScratch3.data_type = DSPLIB_FLOAT32;

   DSPLIB_cholesky_solver_InitArgs kerInitArgsSolver;
   int32_t                         handleSolverSize = DSPLIB_cholesky_solver_getHandleSize(&kerInitArgsSolver);
   DSPLIB_kernelHandle             handleSolver     = malloc(handleSolverSize);

   kerInitArgsSolver.order     = order;
   kerInitArgsSolver.stride    = kerInitArgs.stride;
   kerInitArgsSolver.strideCn  = kerInitArgs.strideCn;
   kerInitArgsSolver.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_ERR_FAILURE;
   status = DSPLIB_cholesky_solver_init_checkParams(handleSolver, &bufParamsL, &bufParamsScratch2, &bufParamsY,
                                                    &bufParamsB, &bufParamsX, &bufParamsScratch3, &kerInitArgsSolver);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_cholesky_solver_init(handleSolver, &bufParamsL, &bufParamsScratch2, &bufParamsY, &bufParamsB,
                                           &bufParamsX, &bufParamsScratch3, &kerInitArgsSolver);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, L, scratch2, Y, B, X, scratch3);
         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_cholesky_solver_exec(handleSolver, L, scratch2, Y, B, X, scratch3);
         }
      }
   }

   printf("Solution Vector X:\n");
   for (int32_t i = 0; i < order; i++) {
      printf("%7.3f\n", X[i]);
   }
   printf("\n");

   /*****************************************************************************************
    *                          Example code for Cholesky Inplace Decomposition
    * ****************************************************************************************/

   DSPLIB_cholesky_inplace_InitArgs kerInitArgsInplace;
   int32_t                          handleInplaceSize = DSPLIB_cholesky_inplace_getHandleSize(&kerInitArgsInplace);
   DSPLIB_kernelHandle              handleInplace     = malloc(handleInplaceSize);

   kerInitArgsInplace.order      = order;
   kerInitArgsInplace.stride     = kerInitArgs.stride;
   kerInitArgsInplace.enableTest = 1; /* To check for positive definiteness of input matrix A */
   kerInitArgsInplace.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_ERR_FAILURE;
   status =
       DSPLIB_cholesky_inplace_init_checkParams(handleInplace, &bufParamsA, &bufParamsScratch3, &kerInitArgsInplace);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_cholesky_inplace_init(handleInplace, &bufParamsA, &bufParamsScratch3, &kerInitArgsInplace);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_cholesky_inplace_exec_checkParams(handleInplace, A, scratch3);
         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_cholesky_inplace_exec(handleInplace, A, scratch3);
         }
      }
   }

   printf("Output Matrix A after Inplace (only upper triangular entries to be considered):\n");
   for (int32_t i = 0; i < order; i++) {
      for (int32_t j = 0; j < order; j++) {
         printf("%6.3f ", A[i][j]);
      }
      printf("\n");
   }
   printf("\n");

   /*****************************************************************************************
    *               Example code for Cholesky Solver After Inplace Decomposition
    * ****************************************************************************************/
   memset((void *) X, 0, bufParamsX.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT32));

   status = DSPLIB_ERR_FAILURE;
   status = DSPLIB_cholesky_solver_init_checkParams(handleSolver, &bufParamsA, &bufParamsScratch2, &bufParamsY,
                                                    &bufParamsB, &bufParamsX, &bufParamsScratch3, &kerInitArgsSolver);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_cholesky_solver_init(handleSolver, &bufParamsA, &bufParamsScratch2, &bufParamsY, &bufParamsB,
                                           &bufParamsX, &bufParamsScratch3, &kerInitArgsSolver);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_cholesky_solver_exec_checkParams(handleSolver, A, scratch2, Y, B, X, scratch3);
         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_cholesky_solver_exec(handleSolver, A, scratch2, Y, B, X, scratch3);
         }
      }
   }

   printf("Solution Vector X after Inplace decompostion:\n");
   for (int32_t i = 0; i < order; i++) {
      printf("%7.3f\n", X[i]);
   }
   printf("\n");
   printf("End of Cholesky Example\n");

   free(handle);
   free(handleSolver);
   free(handleInplace);

   return 0;
}
