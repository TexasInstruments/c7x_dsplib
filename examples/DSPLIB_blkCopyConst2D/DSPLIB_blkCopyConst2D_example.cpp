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
float in0[] = {3.25f};

float out[] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

int main(void)
{
   DSPLIB_STATUS                 status;
   DSPLIB_blkCopyConst2DInitArgs kerInitArgs;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t             handleSize = DSPLIB_blkCopyConst2D_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn;
   DSPLIB_bufParams2D_t bufParamsOut;

   /* Example to bulk move uint8_t data type*/
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = 1;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = 6;
   bufParamsOut.dim_y     = 2;
   bufParamsOut.stride_y  = 6 * sizeof(float);

   status = DSPLIB_SUCCESS;

   // init checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_blkCopyConst2D_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_blkCopyConst2D_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_blkCopyConst2D_exec_checkParams(handle, in0, out);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_blkCopyConst2D_exec(handle, in0, out);

   printf("\n FLOAT \n");

   // print results
   for (size_t i = 0; i < 2; i++) {
      printf("\n\n");
      for (size_t j = 0; j < 6; j++) {
         printf("%f, ", out[i * 6 + j]);
      }
   }

   printf("\n\n");
   free(handle);

   return 0;
}
