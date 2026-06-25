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
int16_t in[] = {27108,  11760,  7774,   13463, 11003, 13887,  -15983, -6867,  -26565, -851,   -30096,
                -23807, 32616,  -16010, 4741,  -7007, -28014, 20478,  -25076, -20033, -20785, -15076,
                -25855, -15170, -15983, 30870, 9150,  -3309,  -28568, -23864, 574,    8023};

float  outSp[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
double outDp[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*
float refOutSp[] = {0.8272705078, 0.3588867188, 0.2372436523, 0.4108581543,
                    0.3357849121, 0.4237976074, -0.4877624512, -0.2095642090,
                    -0.8106994629, -0.0259704590, -0.9184570312, -0.7265319824,
                    0.9953613281, -0.4885864258, 0.1446838379, -0.2138366699,
                    -0.8549194336, 0.6249389648, -0.7652587891, -0.6113586426,
                    -0.6343078613, -0.4600830078, -0.7890319824, -0.4629516602,
                    -0.4877624512, 0.9420776367, 0.2792358398, -0.1009826660,
                    -0.8718261719, -0.7282714844, 0.0175170898, 0.2448425293};
 */

int32_t size = 32;

int main(void)
{
   DSPLIB_q15tofl_InitArgs kerInitArgs;
   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t             handleSize = DSPLIB_q15tofl_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;
   bufParamsIn.data_type = DSPLIB_INT16;
   bufParamsIn.dim_x     = size;

   /* Example for q15 to float conversion */
   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = size;

   int32_t status = DSPLIB_q15tofl_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_q15tofl_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_q15tofl_exec_checkParams(handle, in, outSp);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_q15tofl_exec(handle, in, outSp);
         }
      }
   }
   printf("q15 to single precision floating point conversion\n");
   printf("%10s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%10d -> %.10f\n", in[i], outSp[i]);
   }
   printf("\n");

   /* Example for q15 to double conversion */
   bufParamsOut.data_type = DSPLIB_FLOAT64;
   bufParamsOut.dim_x     = size;

   status = DSPLIB_q15tofl_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_q15tofl_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_q15tofl_exec_checkParams(handle, in, outDp);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_q15tofl_exec(handle, in, outDp);
         }
      }
   }
   printf("q15 to double precision floating point conversion\n");
   printf("%10s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%10d -> %.10lf\n", in[i], outDp[i]);
   }
   printf("\n");

   return 0;
}
