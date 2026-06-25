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
   float in0[] = {1.5};

   float in1[] = {0.,         0.78539816, 1.57079633, 2.35619449, 3.14159265, 3.92699082, 4.71238898,
                  5.49778714, 0.,         0.78539816, 1.57079633, 2.35619449, 3.14159265, 3.92699082};

   float out[] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

   uint32_t size = 14;

   // handles and struct for call to kernel
   DSPLIB_STATUS               status;
   DSPLIB_subConstant_InitArgs kerInitArgs;
   int32_t                     handleSize = DSPLIB_subConstant_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle         handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = size;

   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_subConstant_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_subConstant_exec(handle, in0, in1, out);

   // print results
   for (size_t c = 0; c < size; c++) {
      printf("%10g - %10g = %10g\n", in0[0], in1[c], out[c]);
   }

   return 0;
}
