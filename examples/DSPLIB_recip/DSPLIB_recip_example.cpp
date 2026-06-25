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

   size_t size = 14;

   // handles and struct for call to kernel
   DSPLIB_STATUS         status;
   DSPLIB_recip_InitArgs kerInitArgs;
   int32_t               handleSize = DSPLIB_recip_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle   handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = size;

   // set floating point store pattern to linear
   kerInitArgs.storeStyle = DSPLIB_LINEAR_ST;

   kerInitArgs.dataSize  = size;
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

   for (size_t i = 0; i < size; i++) {
      printf("(1 / %10g) = %10g\n", inSp[i], outSp[i]);
   }

   /* Fixed Point Recip Computation Example */

   // input must be Q15 format (0x8000 to 0x7FFF)
   int16_t inFixed[] = {-32768, -27726, -22685, -17644, -12603, -7562, -2521,
                        2520,   7561,   12602,  17643,  22684,  27725, 32767};

   int16_t outFixed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   size_t inSize  = 14;
   size_t outSize = 28;

   // handles and struct for call to kernel
   DSPLIB_STATUS         statusFixed;
   DSPLIB_recip_InitArgs kerInitArgsFixed;
   int32_t               handleSizeFixed = DSPLIB_recip_getHandleSize(&kerInitArgsFixed);
   DSPLIB_kernelHandle   handleFixed     = malloc(handleSizeFixed);

   DSPLIB_bufParams1D_t bufParamsInFixed, bufParamsOutFixed;

   // fill in input and output buffer parameters
   bufParamsInFixed.data_type = DSPLIB_INT16;
   bufParamsInFixed.dim_x     = inSize;

   bufParamsOutFixed.data_type = DSPLIB_INT16;
   // output must be 2x input size for fixed point computation
   bufParamsOutFixed.dim_x = outSize;

   // fixed point store pattern: horizontal stack or interleaved store
   kerInitArgsFixed.storeStyle = DSPLIB_HSTACK_ST;
   /* kerInitArgsFixed.storeStyle = DSPLIB_INTERLEAVE_ST; */

   kerInitArgsFixed.dataSize  = inSize;
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

   for (size_t i = 0; i < inSize; i++) {
      printf("Q15_recip_hstack(%d): Exp = %d, Frac = %d\n", inFixed[i], outFixed[i], outFixed[i + inSize]);
   }

   /* for (size_t i = 0; i < inSize; i++) { */
   /*    printf("Q15_recip_interleave(%d): Exp = %d, Frac = %d\n", inFixed[i], outFixed[2*i], outFixed[2*i + 1]);
    */
   /* } */

   return 0;
}
