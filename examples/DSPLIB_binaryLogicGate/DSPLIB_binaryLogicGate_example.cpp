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

   uint8_t pIn0[] = {1, 2, 0, 9, 3, 8, 4, 0, 5, 6, 7, 1, 0, 2, 9, 8};

   uint8_t pIn1[] = {1, 3, 2, 9, 3, 8, 4, 0, 5, 6, 7, 8, 0, 2, 1, 6};

   uint8_t pOut[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   // handles and struct for call to kernel
   DSPLIB_STATUS                   status;
   DSPLIB_binaryLogicGate_InitArgs kerInitArgs;
   DSPLIB_binaryLogicGate_SetArgs  kerSetArgs;
   int32_t                         handleSize = DSPLIB_binaryLogicGate_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle             handle     = (DSPLIB_kernelHandle) malloc((size_t) handleSize);

   if (handle == NULL) {
      printf("Failed to allocate kernel handle\n");
      return -1;
   }

   DSPLIB_bufParams2D_t bufParamsIn0, bufParamsIn1, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_UINT8;
   bufParamsIn0.dim_x     = M;
   bufParamsIn0.dim_y     = N;                   // Value
   bufParamsIn0.stride_y  = M * sizeof(uint8_t); // Value

   bufParamsIn1.data_type = DSPLIB_UINT8;
   bufParamsIn1.dim_x     = M;
   bufParamsIn1.dim_y     = N;                   // Value
   bufParamsIn1.stride_y  = M * sizeof(uint8_t); // Value

   bufParamsOut.data_type = DSPLIB_UINT8;
   bufParamsOut.dim_x     = M;
   bufParamsOut.dim_y     = N;                   // Value
   bufParamsOut.stride_y  = M * sizeof(uint8_t); // Value

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
   kerSetArgs.opType     = DSPLIB_BINARYLOGICGATE_AND;

   status = DSPLIB_SUCCESS;

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_binaryLogicGate_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // set
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_binaryLogicGate_set(handle, &kerSetArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_binaryLogicGate_exec_checkParams(handle, pIn0, pIn1, pOut);

   // exec
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_binaryLogicGate_exec(handle, pIn0, pIn1, pOut);
   }

   // print results
   printf("---------------------------------\n");
   printf("|         Input Vector 0        |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%u,  ", pIn0[i * M + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|         Input Vector 1         |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%u,  ", pIn1[i * M + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|           Results              |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%u  ", pOut[i * M + j]);
      }
   }
   printf("\n\n");

   free(handle);
   return 0;
}
