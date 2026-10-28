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

   /* Floating Point Recip Computation Example */

   float inSp[] = {0.01,       0.39384615, 0.77769231, 1.16153846, 1.54538462, 1.92923077, 2.31307692,
                   2.69692308, 3.08076923, 3.46461538, 3.84846154, 4.23230769, 4.61615385, 5.};

   float outSp[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   // 2 rows of 7 values
   size_t width  = 7;
   size_t height = 2;

   // handles and struct for call to kernel
   DSPLIB_STATUS         status;
   DSPLIB_recip_InitArgs kerInitArgs;
   int32_t               handleSize = DSPLIB_recip_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle   handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(float);

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(float);

   // set floating point store pattern to linear
   kerInitArgs.storeStyle = DSPLIB_LINEAR_ST;

   kerInitArgs.dataSize  = width;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_recip_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_recip_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      DSPLIB_recip_exec_checkParams(handle, inSp, outSp);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_recip_exec(handle, inSp, outSp);

   // print results
   printf("\nFloating Point Example Outputs:\n");

   for (size_t row = 0; row < height; row++) {
      for (size_t i = 0; i < width; i++) {
         printf("row %zu: (1 / %10g) = %10g\n", row, inSp[row * width + i], outSp[row * width + i]);
      }
   }

   /* Fixed Point Recip Computation Example */

   // input must be Q15 format (0x8000 to 0x7FFF)
   int16_t inFixed[] = {-32768, -27726, -22685, -17644, -12603, -7562, -2521,
                        2520,   7561,   12602,  17643,  22684,  27725, 32767};

   int16_t outFixed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   // 2 rows of 7 values; each output row holds 2 * 7 values (exp and frac)
   size_t inWidth     = 7;
   size_t outWidth    = 14;
   size_t heightFixed = 2;

   // handles and struct for call to kernel
   DSPLIB_STATUS         statusFixed;
   DSPLIB_recip_InitArgs kerInitArgsFixed;
   int32_t               handleSizeFixed = DSPLIB_recip_getHandleSize(&kerInitArgsFixed);
   DSPLIB_kernelHandle   handleFixed     = malloc(handleSizeFixed);

   DSPLIB_bufParams2D_t bufParamsInFixed, bufParamsOutFixed;

   // fill in input and output buffer parameters
   bufParamsInFixed.data_type = DSPLIB_INT16;
   bufParamsInFixed.dim_x     = inWidth;
   bufParamsInFixed.dim_y     = heightFixed;
   bufParamsInFixed.stride_y  = inWidth * sizeof(int16_t);

   bufParamsOutFixed.data_type = DSPLIB_INT16;
   // output row must be 2x input row width for fixed point computation
   bufParamsOutFixed.dim_x    = outWidth;
   bufParamsOutFixed.dim_y    = heightFixed;
   bufParamsOutFixed.stride_y = outWidth * sizeof(int16_t);

   // fixed point store pattern: horizontal stack or interleaved store
   kerInitArgsFixed.storeStyle = DSPLIB_HSTACK_ST;
   /* kerInitArgsFixed.storeStyle = DSPLIB_INTERLEAVE_ST; */

   kerInitArgsFixed.dataSize  = inWidth;
   kerInitArgsFixed.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   statusFixed = DSPLIB_SUCCESS;

   // init checkparams
   if (statusFixed == DSPLIB_SUCCESS)
      statusFixed =
          DSPLIB_recip_init_checkParams(handleFixed, &bufParamsInFixed, &bufParamsOutFixed, &kerInitArgsFixed);

   // init
   if (statusFixed == DSPLIB_SUCCESS)
      statusFixed = DSPLIB_recip_init(handleFixed, &bufParamsInFixed, &bufParamsOutFixed, &kerInitArgsFixed);

   // exec checkparams
   if (statusFixed == DSPLIB_SUCCESS)
      DSPLIB_recip_exec_checkParams(handleFixed, inFixed, outFixed);

   // exec
   if (statusFixed == DSPLIB_SUCCESS)
      statusFixed = DSPLIB_recip_exec(handleFixed, inFixed, outFixed);

   // print results
   printf("\nFixed Point Example Outputs:\n");

   for (size_t row = 0; row < heightFixed; row++) {
      for (size_t i = 0; i < inWidth; i++) {
         printf("row %zu: Q15_recip_hstack(%d): Exp = %d, Frac = %d\n", row, inFixed[row * inWidth + i],
                outFixed[row * outWidth + i], outFixed[row * outWidth + inWidth + i]);
      }
   }

   /* for (size_t row = 0; row < heightFixed; row++) { */
   /*    for (size_t i = 0; i < inWidth; i++) { */
   /*       printf("row %zu: Q15_recip_interleave(%d): Exp = %d, Frac = %d\n", row, inFixed[row * inWidth + i], */
   /*              outFixed[row * outWidth + 2 * i], outFixed[row * outWidth + 2 * i + 1]); */
   /*    } */
   /* } */

   return 0;
}
