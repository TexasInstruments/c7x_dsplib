// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <math.h>
#include <stdint.h>

#define M (4)
#define N (4)
/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // Setup input and output buffers for 8-bit unsigned datatype

   uint8_t pIn[]  = {1, 2, 0, 9, 3, 8, 4, 0, 5, 6, 7, 1, 0, 2, 9, 8};
   uint8_t pOut[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   // handles and struct for call to kernel
   DSPLIB_STATUS              status;
   DSPLIB_logicalNOT_InitArgs kerInitArgs;
   int32_t                    handleSize = DSPLIB_logicalNOT_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle        handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_UINT8;
   bufParamsIn.dim_x     = M;
   bufParamsIn.dim_y     = N;                   // Value
   bufParamsIn.stride_y  = M * sizeof(uint8_t); // Value

   bufParamsOut.data_type = DSPLIB_UINT8;
   bufParamsOut.dim_x     = M;
   bufParamsOut.dim_y     = N;                   // Value
   bufParamsOut.stride_y  = M * sizeof(uint8_t); // Value

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_logicalNOT_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_logicalNOT_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_logicalNOT_exec_checkParams(handle, pIn, pOut);

   // exec
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_logicalNOT_exec(handle, pIn, pOut);
   }

   // print results
   printf("---------------------------------\n");
   printf("|         Input Vector          |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%u,  ", pIn[i * M + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|           Results             |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%u  ", pOut[i * M + j]);
      }
   }
   printf("\n\n");

   // Floating point: +0.0 and -0.0 give 1.0, any other value (including NaN) gives 0.0
   float pInF[]  = {0.0f, -0.0f, 1.5f, NAN, -2.25f, 0.0f, 3.0f, 0.0f, 7.0f, 0.5f, -0.0f, 8.0f, 1.0f, 0.0f, 9.0f, 4.0f};
   float pOutF[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   bufParamsIn.data_type  = DSPLIB_FLOAT32;
   bufParamsIn.stride_y   = M * sizeof(float);
   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.stride_y  = M * sizeof(float);

   status = DSPLIB_SUCCESS;

   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_logicalNOT_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_logicalNOT_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_logicalNOT_exec_checkParams(handle, pInF, pOutF);

   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_logicalNOT_exec(handle, pInF, pOutF);

   printf("---------------------------------\n");
   printf("|       Float Input Vector      |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%f,  ", pInF[i * M + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|        Float Results          |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%f  ", pOutF[i * M + j]);
      }
   }
   printf("\n\n");

   free(handle);
   return (status == DSPLIB_SUCCESS) ? 0 : 1;
}
