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

   uint8_t pIn1[] = {0};

   uint8_t pOut[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   // handles and struct for call to kernel
   DSPLIB_STATUS                   status;
   DSPLIB_bitwiseANDConst_InitArgs kerInitArgs;
   int32_t                         handleSize = DSPLIB_bitwiseANDConst_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle             handle     = malloc(handleSize);

   if (handle == NULL) {
      printf("Failed to allocate kernel handle\n");
      return -1;
   }

   DSPLIB_bufParams2D_t bufParamsIn0, bufParamsOut;
   DSPLIB_bufParams1D_t bufParamsIn1;

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_UINT8;
   bufParamsIn0.dim_x     = M;
   bufParamsIn0.dim_y     = N;                   // Value
   bufParamsIn0.stride_y  = M * sizeof(uint8_t); // Value

   bufParamsIn1.data_type = DSPLIB_UINT8;
   bufParamsIn1.dim_x     = 1;

   bufParamsOut.data_type = DSPLIB_UINT8;
   bufParamsOut.dim_x     = M;
   bufParamsOut.dim_y     = N;                   // Value
   bufParamsOut.stride_y  = M * sizeof(uint8_t); // Value

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_bitwiseANDConst_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_bitwiseANDConst_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_bitwiseANDConst_exec_checkParams(handle, pIn0, pIn1, pOut);

   // exec
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_bitwiseANDConst_exec(handle, pIn0, pIn1, pOut);
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
   for (size_t i = 0; i < 1; i++) {
      printf("\n\n");

      printf("%u,  ", pIn1[i]);
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
