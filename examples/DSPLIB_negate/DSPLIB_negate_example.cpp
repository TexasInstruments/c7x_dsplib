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

int main(void)
{

   // Setup input and output buffers for single- and double-precision datatypes

   int32_t in[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

   int32_t out[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   uint32_t size = 14;

   // handles and struct for call to kernel
   DSPLIB_STATUS          status;
   DSPLIB_negate_InitArgs kerInitArgs;
   int32_t                handleSize = DSPLIB_negate_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle    handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_INT32;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_INT32;
   bufParamsOut.dim_x     = size;

   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_negate_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_negate_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      DSPLIB_negate_exec_checkParams(handle, in, out);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_negate_exec(handle, in, out);

   // print results
   for (size_t c = 0; c < size; c++) {
      printf("-(%d) =  %d\n", in[c], out[c]);
   }

   return 0;
}
