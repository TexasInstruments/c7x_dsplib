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
uint8_t inUint8[]  = {42,  113, 70, 103, 136, 106, 121, 52, 85, 169, 150, 58,  223, 128, 49,  178,
                      135, 133, 21, 211, 219, 93,  163, 57, 70, 160, 80,  113, 195, 229, 206, 238};
uint8_t outUint8[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int32_t inInt32[]  = {17768,  -56377, 39018,  18548,  56402, -41728, -27573, 22765,  -57558, 31950,  -42821,
                      55212,  -48653, 7932,   43492,  57671, 125,    25283,  -63403, 10233,  -56548, -5655,
                      -12824, 17294,  -61577, -55973, -1745, -36252, 49716,  -10336, 50378,  1947};
int32_t outInt32[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

double inDp[]  = {-0.5253458023, -0.8233585358, -1.0709538460, 0.3379540443,  -1.0223491192, -1.3904408216,
                  0.9285941124,  -1.4981004000, 1.1738815308,  -1.3435922861, 0.9802856445,  -1.7018808126,
                  1.8004162312,  -1.7898828983, 0.0862534046,  -1.2951574326, -1.0397505760, 1.1911921501,
                  0.9306175709,  0.6262545586,  1.8696205616,  0.5578334332,  1.0389394760,  -1.6260781288,
                  -1.4603903294, 0.0808403492,  -1.6870714426, -1.7203743458, -1.1813796759, -0.1543180943,
                  1.2787091732,  0.2932744026};
double outDp[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

int32_t size = 32;

int main(void)
{
   DSPLIB_blk_move_InitArgs kerInitArgs;
   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t             handleSize = DSPLIB_blk_move_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   /* Example to bulk move uint8_t data type*/
   bufParamsIn.data_type = DSPLIB_UINT8;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_UINT8;
   bufParamsOut.dim_x     = size;

   int32_t status = DSPLIB_blk_move_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_blk_move_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_blk_move_exec_checkParams(handle, inUint8, outUint8);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_blk_move_exec(handle, inUint8, outUint8);
         }
      }
   }
   printf("Unsigned integer 8bit data copy\n");
   printf("%10s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%d -> %d\n", inUint8[i], outUint8[i]);
   }
   printf("\n");

   /* Example to bulk move int32_t data type*/
   bufParamsIn.data_type = DSPLIB_INT32;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_INT32;
   bufParamsOut.dim_x     = size;

   status = DSPLIB_blk_move_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_blk_move_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_blk_move_exec_checkParams(handle, inInt32, outInt32);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_blk_move_exec(handle, inInt32, outInt32);
         }
      }
   }
   printf("Integer 32bit data copy\n");
   printf("%10s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%d -> %d\n", inInt32[i], outInt32[i]);
   }
   printf("\n");

   /* Example to bulk move "double" data type*/
   bufParamsIn.data_type = DSPLIB_FLOAT64;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_FLOAT64;
   bufParamsOut.dim_x     = size;

   status = DSPLIB_blk_move_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_blk_move_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_blk_move_exec_checkParams(handle, inDp, outDp);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_blk_move_exec(handle, inDp, outDp);
         }
      }
   }
   printf("Double precision floating point data copy\n");
   printf("%10s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%.10f -> %.10f\n", inDp[i], outDp[i]);
   }
   printf("\n");

   return 0;
}
