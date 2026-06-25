// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bufParams.h"
#include "DSPLIB_types.h"
#include "dsplib.h"
#include <cstdio>
#include <stdint.h>

#define M (8)
#define N (8)
#define K (8)

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // clang-format off

  // Setup input and output buffers
  int16_t in0[] = {
                     56,  81,  52, 173, 110, 236, 224, 131,
                    127,   9, 233,  77, 162, 129, 191,  28,
                     48,  66,  97, 252,   3, 119,  16,  26,
                     99,  39, 140,   6, 132, 202,  16, 106,
                     98,  27, 235, 141, 223,  94,  44, 184,
                    231, 116, 100, 152, 197,   3, 241, 181,
                     53, 211,  67,  90,  64, 215, 246, 199,
                     40,  22, 157,  61, 247,  69, 181, 143,
                  };

  int16_t in1[] = {
                  4, 0, 0, 0, 0, 0, 0, 0,
                  0, 4, 0, 0, 0, 0, 0, 0,
                  0, 0, 4, 0, 0, 0, 0, 0,
                  0, 0, 0, 4, 0, 0, 0, 0,
                  0, 0, 0, 0, 4, 0, 0, 0,
                  0, 0, 0, 0, 0, 4, 0, 0,
                  0, 0, 0, 0, 0, 0, 4, 0,
                  0, 0, 0, 0, 0, 0, 0, 4,
                  };

  int16_t out[] = {
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                  };

   // clang-format on

   printf("***** Start of matMul_fixed example *****\n");
   // handles and struct for call to kernel
   DSPLIB_STATUS                status;
   DSPLIB_matMul_fixed_InitArgs kerInitArgs;
   int32_t                      handleSize = DSPLIB_matMul_fixed_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle          handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn0, bufParamsIn1, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_INT16;
   bufParamsIn0.dim_x     = K;
   bufParamsIn0.dim_y     = M;
   bufParamsIn0.stride_y  = K * sizeof(int16_t);

   bufParamsIn1.data_type = DSPLIB_INT16;
   bufParamsIn1.dim_x     = N;
   bufParamsIn1.dim_y     = K;
   bufParamsIn1.stride_y  = N * sizeof(int16_t);

   bufParamsOut.data_type = DSPLIB_INT16;
   bufParamsOut.dim_x     = N;
   bufParamsOut.dim_y     = M;
   bufParamsOut.stride_y  = N * sizeof(int16_t);

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.qs        = 1; // shift associated

   status = DSPLIB_SUCCESS;

   // init checkparams
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_matMul_fixed_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
   }

   // init
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_matMul_fixed_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
   }

   // exec checkparams
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_matMul_fixed_exec_checkParams(handle, in0, in1, out);
   }

   // exec kernel
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_matMul_fixed_exec(handle, in0, in1, out);
   }

   // print results
   printf("Input matrix in0:");
   for (size_t i = 0; i < M; i++) {
      printf("\n");
      for (size_t j = 0; j < K; j++) {
         printf("%5d ", in0[i * K + j]);
      }
   }
   printf("\n\n");

   printf("Input matrix in1:");
   for (size_t i = 0; i < K; i++) {
      printf("\n");
      for (size_t j = 0; j < N; j++) {
         printf("%5d ", in1[i * N + j]);
      }
   }
   printf("\n\n");

   printf("Resultant output matrix out:");
   for (size_t i = 0; i < M; i++) {
      printf("\n");
      for (size_t j = 0; j < N; j++) {
         printf("%5d ", out[i * N + j]);
      }
   }
   printf("\n\n");
   printf("***** End of matMul_fixed example *****\n");

   free(handle);
   return 0;
}
