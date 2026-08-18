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
   DSPLIB_STATUS               status;
   DSPLIB_mulConstant_InitArgs kerInitArgs;
   DSPLIB_bufParams1D_t        bufParamsIn, bufParamsOut;
   int32_t                     handleSize;
   DSPLIB_kernelHandle         handle;

   /*-------------------------------------------------------------------------*/
   /* Example 1: float32 - scale a vector by 1.5                              */
   /*-------------------------------------------------------------------------*/

   float    f32_in0[]  = {1.5f};
   float    f32_in1[]  = {2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f};
   float    f32_out[8] = {0};
   uint32_t f32_size   = 8;

   handleSize = DSPLIB_mulConstant_getHandleSize(&kerInitArgs);
   handle     = malloc(handleSize);

   bufParamsIn.data_type  = DSPLIB_FLOAT32;
   bufParamsIn.dim_x      = f32_size;
   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = f32_size;
   kerInitArgs.dataSize   = f32_size;
   kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_mulConstant_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_mulConstant_exec(handle, f32_in0, f32_in1, f32_out);

   printf("float32 scale by %.4g:\n", f32_in0[0]);
   for (uint32_t i = 0; i < f32_size; i++)
      printf("  %.4f * %.4f = %.4f\n", f32_in0[0], f32_in1[i], f32_out[i]);

   free(handle);

   /*-------------------------------------------------------------------------*/
   /* Example 2: cfloat32 - multiply a complex vector by a complex constant   */
   /*                                                                          */
   /* Data layout: interleaved float pairs [re0, im0, re1, im1, ...]          */
   /* Constant (pIn1): one complex pair [re, im]                               */
   /*-------------------------------------------------------------------------*/

   /* constant: 0.5 + 0.25j */
   float    cf32_in0[]  = {0.5f, 0.25f};
   /* vector: 4 complex samples as interleaved re/im float pairs */
   float    cf32_in1[]  = {2.0f, 4.0f,  3.0f, 6.0f,  5.0f, 2.0f,  4.0f, 8.0f};
   float    cf32_out[8] = {0};
   uint32_t cf32_size   = 4;

   handleSize = DSPLIB_mulConstant_getHandleSize(&kerInitArgs);
   handle     = malloc(handleSize);

   bufParamsIn.data_type  = DSPLIB_CFLOAT32;
   bufParamsIn.dim_x      = cf32_size;
   bufParamsOut.data_type = DSPLIB_CFLOAT32;
   bufParamsOut.dim_x     = cf32_size;
   kerInitArgs.dataSize   = cf32_size;
   kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_mulConstant_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_mulConstant_exec(handle, cf32_in0, cf32_in1, cf32_out);

   printf("\ncfloat32 scale by (%.4g + %.4gj):\n", cf32_in0[0], cf32_in0[1]);
   for (uint32_t i = 0; i < cf32_size; i++)
      printf("  (%.2f + %.2fj) * (%.2f + %.2fj) = (%.4f + %.4fj)\n",
             cf32_in0[0], cf32_in0[1], cf32_in1[2*i], cf32_in1[2*i+1],
             cf32_out[2*i], cf32_out[2*i+1]);

   free(handle);

   /*-------------------------------------------------------------------------*/
   /* Example 3: cint32 (Q31) - multiply a complex Q31 vector by a constant   */
   /*                                                                          */
   /* Data layout: interleaved int32_t pairs [re0, im0, re1, im1, ...]        */
   /* Q31 encoding: real_value = integer / 2^31                                */
   /*   0.25 * 2^31 =  536870912                                               */
   /*   0.50 * 2^31 = 1073741824                                               */
   /*   0.75 * 2^31 = 1610612736                                               */
   /*-------------------------------------------------------------------------*/

   /* constant: 0.5 + 0.25j */
   int32_t  ci32_in0[]  = {1073741824, 536870912};
   /* vector: 4 complex Q31 samples */
   int32_t  ci32_in1[]  = {1073741824,  536870912,
                            536870912, 1073741824,
                           1610612736,  536870912,
                            536870912, 1610612736};
   int32_t  ci32_out[8] = {0};
   uint32_t ci32_size   = 4;

   handleSize = DSPLIB_mulConstant_getHandleSize(&kerInitArgs);
   handle     = malloc(handleSize);

   bufParamsIn.data_type  = DSPLIB_CINT32;
   bufParamsIn.dim_x      = ci32_size;
   bufParamsOut.data_type = DSPLIB_CINT32;
   bufParamsOut.dim_x     = ci32_size;
   kerInitArgs.dataSize   = ci32_size;
   kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_mulConstant_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_mulConstant_exec(handle, ci32_in0, ci32_in1, ci32_out);

   printf("\ncint32 (Q31) scale by (%d, %d):\n", ci32_in0[0], ci32_in0[1]);
   for (uint32_t i = 0; i < ci32_size; i++)
      printf("  (%d, %d) -> (%d, %d)\n",
             ci32_in1[2*i], ci32_in1[2*i+1], ci32_out[2*i], ci32_out[2*i+1]);

   free(handle);

   return 0;
}
