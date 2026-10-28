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

   // Setup input and output buffers: a 2 x 7 matrix of single-precision values
   float in[] = {-0.71649936, -0.13543484, 0.50923542, 0.54119591, 0.19242506,  0.38308575, 0.56363197,
                 0.24567145,  0.05629663,  0.99152828, 0.4799542,  -0.97309674, 0.79839982, 0.06691247};

   float out[] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

   uint32_t width  = 7;
   uint32_t height = 2;

   // handles and struct for call to kernel
   DSPLIB_STATUS                status;
   DSPLIB_absoluteValueInitArgs kerInitArgs;
   int32_t                      handleSize = DSPLIB_absoluteValue_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle          handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters (stride_y is in bytes)
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(float);

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(float);

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   // check the parameters, then init
   status = DSPLIB_absoluteValue_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_absoluteValue_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_absoluteValue_exec(handle, in, out);

   // print results
   printf("absoluteValue status = %d\n", (int) status);
   for (size_t c = 0; c < width * height; c++) {
      printf("absoluteValue(%10g) = %10g\n", in[c], out[c]);
   }

   free(handle);

   return 0;
}
