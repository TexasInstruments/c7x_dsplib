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

   // Setup input and output buffers for single-precision datatype

   float in[] = {
       -93402767.98696518,  -635811590.2500186,  -2048438858.1264997, -943043884.078691,
       329773452.93652534,  -2010646387.0167627, -394264694.3655772,  1538797820.7693605,
       -253024826.9190998,  1761580513.2072868,  -1567489803.6343036, -836398874.9244952,
       -1052503241.4965706, 1457407716.1291046,  1070422402.7200341,  -2114310238.265214,
   };

   // 5.49778714, 0.,         0.78539816, 1.57079633, 2.35619449, 3.14159265, 3.92699082};

   float out[]          = {0.};
   float expected_out[] = {2.73448938427507e+19};

   uint32_t size_in  = 16;
   uint32_t size_out = 1;

   // handles and struct for call to kernel
   DSPLIB_STATUS          status;
   DSPLIB_sqrAdd_InitArgs kerInitArgs;
   int32_t                handleSize = DSPLIB_sqrAdd_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle    handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = size_in;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = size_out;

   kerInitArgs.dataSize  = size_in;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_sqrAdd_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_sqrAdd_exec(handle, in, out);

   // print results
   printf("---------------------------------\n");
   printf("|         Input Vector          |\n");
   printf("---------------------------------\n");
   for (size_t c = 0; c < size_in; c++) {
      printf("%10g \n", in[c]);
   }

   printf("---------------------------------\n");
   printf("|           Results              |\n");
   printf("---------------------------------\n");
   printf("Square Add of input vecotr: Expected = %10g\n", expected_out[0]);
   printf("Square Add of input vecotr: Actual   = %10g\n", out[0]);

   return 0;
}
