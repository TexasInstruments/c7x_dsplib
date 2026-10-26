// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

   uint32_t pIn0[] = {1, 3, 2, 9, 3, 8, 4, 0, 5, 6, 7, 8, 0, 2, 1, 6};

   // handles and struct for call to kernel
   DSPLIB_STATUS                status;
   DSPLIB_nonZeroCheck_InitArgs kerInitArgs;
   int32_t                      handleSize = DSPLIB_nonZeroCheck_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle          handle     = malloc(handleSize);

   if (handle == NULL) {
      printf("Failed to allocate kernel handle\n");
      return -1;
   }

   DSPLIB_bufParams2D_t bufParamsIn0;
   DSPLIB_bufParams1D_t bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_UINT32;
   bufParamsIn0.dim_x     = M;
   bufParamsIn0.dim_y     = N;                    // Value
   bufParamsIn0.stride_y  = M * sizeof(uint32_t); // Value

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.opType    = DSPLIB_NONZEROCHECK_ALL;
   kerInitArgs.axisType  = DSPLIB_NONZEROCHECK_COLUMN;

   if (kerInitArgs.axisType == DSPLIB_NONZEROCHECK_ROW) {
      bufParamsOut.data_type = DSPLIB_UINT32;
      bufParamsOut.dim_x     = M;
   }
   else {
      bufParamsOut.data_type = DSPLIB_UINT32;
      bufParamsOut.dim_x     = N;
   }

   uint32_t *pOut = (uint32_t *) malloc(bufParamsOut.dim_x * sizeof(uint32_t));
   memset(pOut, 0, bufParamsOut.dim_x * sizeof(uint32_t));

   status = DSPLIB_SUCCESS;

   // init checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_nonZeroCheck_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_nonZeroCheck_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_nonZeroCheck_exec_checkParams(handle, pIn0, pOut);

   // exec
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_nonZeroCheck_exec(handle, pIn0, pOut);
   }

   // print results
   printf("---------------------------------\n");
   printf("|         Input Vector          |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%d,  ", pIn0[i * M + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|           Results              |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < bufParamsOut.dim_x; i++) {
      printf("%d, ", pOut[i]);
   }
   printf("\n\n");

   free(handle);
   free(pOut);
   return 0;
}
