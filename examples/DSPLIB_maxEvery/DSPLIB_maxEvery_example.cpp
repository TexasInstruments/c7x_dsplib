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
   float in0[] = {0.71649936, 0.13543484, 0.50923542, 0.54119591, 0.19242506, 0.38308575, 0.56363197,
                  0.24567145, 0.05629663, 0.99152828, 0.4799542,  0.97309674, 0.79839982, 0.06691247};

   float in1[] = {0.77968865, 0.7689088,  0.71902326, 0.9556115,  0.56870971, 0.19660892, 0.13702307,
                  0.04597967, 0.90127774, 0.70177184, 0.77441556, 0.11305819, 0.61073371, 0.17159725};

   float out[] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

   uint32_t size = 14;

   // handles and struct for call to kernel
   DSPLIB_STATUS            status;
   DSPLIB_maxEvery_InitArgs kerInitArgs;
   int32_t                  handleSize = DSPLIB_maxEvery_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle      handle     = malloc(handleSize);

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
      status = DSPLIB_maxEvery_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_maxEvery_exec(handle, in0, in1, out);

   // print results
   for (size_t c = 0; c < size; c++) {
      printf("max(%10g , %10g) = %10g\n", in0[c], in1[c], out[c]);
   }

   return 0;
}
