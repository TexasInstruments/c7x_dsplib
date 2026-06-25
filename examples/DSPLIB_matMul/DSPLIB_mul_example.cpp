// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bufParams.h"
#include "DSPLIB_types.h"
#include "dsplib.h"
#include <cstdio>
#include <stdint.h>

#define M (3)
#define N (3)
#define K (3)

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // clang-format off

  // Setup input and output buffers for single- and double-precision datatypes
   float in0[] = {0.71649936, 0.13543484, 0.50923542,
		  0.54119591, 0.19242506, 0.38308575,
                  0.24567145, 0.05629663, 0.99152828};

   float in1[] = {1.0, 0.0, 0.0,
		  0.0, 1.0, 0.0,
		  0.0, 0.0, 1.0};
   
   float out[] = {0., 0., 0.,
		  0., 0., 0.,
		  0., 0., 0.};

   double in0D[] = {0.71649936, 0.13543484, 0.50923542,
		  0.54119591, 0.19242506, 0.38308575,
                  0.24567145, 0.05629663, 0.99152828};

   double in1D[] = {1.0, 0.0, 0.0,
		  0.0, 1.0, 0.0,
		  0.0, 0.0, 1.0};
   
   double outD[] = {0., 0., 0.,
		  0., 0., 0.,
		  0., 0., 0.};

   // clang-format on

   // handles and struct for call to kernel
   DSPLIB_STATUS          status;
   DSPLIB_matMul_InitArgs kerInitArgs;
   int32_t                handleSize = DSPLIB_matMul_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle    handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn0, bufParamsIn1, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_FLOAT32;
   bufParamsIn0.dim_x     = K;
   bufParamsIn0.dim_y     = M;
   bufParamsIn0.stride_y  = K * sizeof(float);

   bufParamsIn1.data_type = DSPLIB_FLOAT32;
   bufParamsIn1.dim_x     = N;
   bufParamsIn1.dim_y     = K;
   bufParamsIn1.stride_y  = N * sizeof(float);

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = N;
   bufParamsOut.dim_y     = M;
   bufParamsOut.stride_y  = N * sizeof(float);

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_matMul_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // exec checkparams

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_matMul_exec(handle, in0, in1, out);

   printf("\n FLOAT \n");

   // print results
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%10g, ", out[i * N + j]);
      }
   }
   printf("\n\n");

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_FLOAT64;
   bufParamsIn0.dim_x     = K;
   bufParamsIn0.dim_y     = M;
   bufParamsIn0.stride_y  = K * sizeof(double);

   bufParamsIn1.data_type = DSPLIB_FLOAT64;
   bufParamsIn1.dim_x     = N;
   bufParamsIn1.dim_y     = K;
   bufParamsIn1.stride_y  = N * sizeof(double);

   bufParamsOut.data_type = DSPLIB_FLOAT64;
   bufParamsOut.dim_x     = N;
   bufParamsOut.dim_y     = M;
   bufParamsOut.stride_y  = N * sizeof(double);

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_matMul_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // exec checkparams

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_matMul_exec(handle, in0D, in1D, outD);

   printf("\n DOUBLE \n");

   // print results
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%10g, ", outD[i * N + j]);
      }
   }
   printf("\n\n");
   return 0;
}
