// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
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

   // Setup input and output buffers for single-precision datatype

   float pIn0[] = {1.1, 2.4, 0.8, 9.7, 3.7, 8.1, 4.7, 0.7, 5.0, 6.3, 7.2, 1.5, 0.3, 2.6, 9.1, 8.6};

   float pIn1[] = {1.1};

   uint32_t pOut[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   // handles and struct for call to kernel
   DSPLIB_STATUS                     status;
   DSPLIB_thresholdDetector_InitArgs kerInitArgs;
   DSPLIB_thresholdDetector_SetArgs  kerSetArgs;
   int32_t                           handleSize = DSPLIB_thresholdDetector_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle               handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn0, bufParamsOut;
   DSPLIB_bufParams1D_t bufParamsIn1;

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_FLOAT32;
   bufParamsIn0.dim_x     = M;
   bufParamsIn0.dim_y     = N;                 // Value
   bufParamsIn0.stride_y  = M * sizeof(float); // Value

   bufParamsIn1.data_type = DSPLIB_FLOAT32;
   bufParamsIn1.dim_x     = 1;

   bufParamsOut.data_type = DSPLIB_UINT32;
   bufParamsOut.dim_x     = M;
   bufParamsOut.dim_y     = N;                    // Value
   bufParamsOut.stride_y  = M * sizeof(uint32_t); // Value

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
   kerSetArgs.opType     = DSPLIB_THRESHOLDDETECTOR_EQ;

   status = DSPLIB_SUCCESS;

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_thresholdDetector_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // set
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_thresholdDetector_set(handle, &kerSetArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_thresholdDetector_exec_checkParams(handle, pIn0, pIn1, pOut);

   // exec
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_thresholdDetector_exec(handle, pIn0, pIn1, pOut);
   }

   // print results
   printf("---------------------------------\n");
   printf("|         Input Vector 0        |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%10g,  ", pIn0[i * M + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|         Input Vector 1         |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < 1; i++) {
      printf("\n\n");
      printf("%10g,  ", pIn1[i]);
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|           Results              |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%d  ", pOut[i * M + j]);
      }
   }
   printf("\n\n");

   free(handle);
   return 0;
}
