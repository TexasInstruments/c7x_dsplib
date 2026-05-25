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

   // Setup input and output buffers for single-precision data; the output is a single value
   float in[] = {0.71649936, -0.13543484, 0.50923542, 0.54119591, -0.19242506, 0.38308575, 0.56363197,
                 0.24567145, -0.05629663, 0.99152828, 0.4799542,  -1.97309674, 0.79839982, 0.06691247};

   float out[] = {0.};

   uint32_t size = 14;

   // handles and struct for call to kernel
   DSPLIB_STATUS                     status;
   DSPLIB_peakAbsoluteValue_InitArgs kerInitArgs;
   int32_t                           handleSize = DSPLIB_peakAbsoluteValue_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle               handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = 1;

   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   // check the parameters, then init
   status = DSPLIB_peakAbsoluteValue_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_peakAbsoluteValue_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_peakAbsoluteValue_exec(handle, in, out);

   // print results
   printf("peakAbsoluteValue status = %d\n", (int) status);
   printf("peakAbsoluteValue (");
   for (size_t c = 0; c < size; c++) {
      printf("%10g,", in[c]);
   }
   printf(") = %10g\n", out[0]);

   free(handle);

   return 0;
}
