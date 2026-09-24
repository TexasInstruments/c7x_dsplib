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
   DSPLIB_STATUS        status;
   DSPLIB_diag_InitArgs kerInitArgs;
   DSPLIB_bufParams2D_t bufParamsIn;
   DSPLIB_bufParams1D_t bufParamsOut;
   int32_t              handleSize;
   DSPLIB_kernelHandle  handle;

   /*-------------------------------------------------------------------------*/
   /* Example 1: float32 - extract the diagonal of a 5x5 matrix               */
   /*                                                                          */
   /* Element (r, c) holds 10*r + c, so the diagonal is 0, 11, 22, 33, 44.    */
   /*-------------------------------------------------------------------------*/

   float f32_in[5 * 5] = {0.0f,  1.0f,  2.0f,  3.0f,  4.0f,  10.0f, 11.0f, 12.0f, 13.0f,
                          14.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 30.0f, 31.0f, 32.0f,
                          33.0f, 34.0f, 40.0f, 41.0f, 42.0f, 43.0f, 44.0f};
   float    f32_out[5] = {0};
   uint32_t f32_order  = 5;

   bufParamsIn.data_type  = DSPLIB_FLOAT32;
   bufParamsIn.dim_x      = f32_order;
   bufParamsIn.dim_y      = f32_order;
   bufParamsIn.stride_y   = f32_order * sizeof(float);
   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = f32_order;
   kerInitArgs.dataSize   = f32_order;
   kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;

   handleSize = DSPLIB_diag_getHandleSize(&kerInitArgs);
   handle     = malloc(handleSize);

   status = DSPLIB_diag_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_diag_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_diag_exec(handle, f32_in, f32_out);

   printf("float32 5x5 input:\n");
   for (uint32_t r = 0; r < f32_order; r++) {
      printf(" ");
      for (uint32_t c = 0; c < f32_order; c++)
         printf(" %5.1f", f32_in[r * f32_order + c]);
      printf("\n");
   }
   printf("diagonal (status %d):\n ", status);
   for (uint32_t i = 0; i < f32_order; i++)
      printf(" %5.1f", f32_out[i]);
   printf("\n");

   free(handle);

   /*-------------------------------------------------------------------------*/
   /* Example 2: cint16 - extract the diagonal of a 3x3 complex matrix        */
   /*                                                                          */
   /* Data layout: interleaved int16_t pairs [re, im] per element.            */
   /* Element (r, c) holds (10*r + c) + (-(10*r + c))j, so the diagonal is    */
   /* (0, 0), (11, -11), (22, -22).                                           */
   /*-------------------------------------------------------------------------*/

   int16_t ci16_in[3 * 3 * 2] = {0,   0,   1,   -1,  2,   -2,  10,  -10, 11,
                                 -11, 12,  -12, 20,  -20, 21,  -21, 22,  -22};
   int16_t  ci16_out[3 * 2] = {0};
   uint32_t ci16_order      = 3;

   bufParamsIn.data_type  = DSPLIB_CINT16;
   bufParamsIn.dim_x      = ci16_order;
   bufParamsIn.dim_y      = ci16_order;
   bufParamsIn.stride_y   = ci16_order * 2 * sizeof(int16_t);
   bufParamsOut.data_type = DSPLIB_CINT16;
   bufParamsOut.dim_x     = ci16_order;
   kerInitArgs.dataSize   = ci16_order;
   kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;

   handleSize = DSPLIB_diag_getHandleSize(&kerInitArgs);
   handle     = malloc(handleSize);

   status = DSPLIB_diag_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_diag_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_diag_exec(handle, ci16_in, ci16_out);

   printf("\ncint16 3x3 diagonal (status %d):\n", status);
   for (uint32_t i = 0; i < ci16_order; i++)
      printf("  (%d, %d)\n", ci16_out[2 * i], ci16_out[2 * i + 1]);

   free(handle);

   return 0;
}
