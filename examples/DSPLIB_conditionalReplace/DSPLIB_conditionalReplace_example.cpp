// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <stdint.h>

#define NUM_CHAN (3)
#define NUM_SAMP (4)
/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // Each row is a channel with its own compare type, compare constant and replacement value
   int32_t pIn[NUM_CHAN * NUM_SAMP]  = {5, 12, 10, 40, 3, 7, 3, 1, -8, 0, 8, 100};
   int32_t pOut[NUM_CHAN * NUM_SAMP] = {0};

   // channel 0: >, channel 1: ==, channel 2: <=
   int32_t pCompareType[NUM_CHAN]  = {DSPLIB_CONDITIONALREPLACE_GT, DSPLIB_CONDITIONALREPLACE_EQ,
                                      DSPLIB_CONDITIONALREPLACE_LE};
   int32_t pConstValue[NUM_CHAN]   = {10, 3, 0};
   int32_t pReplaceValue[NUM_CHAN] = {-1, 99, 7};

   // handles and struct for call to kernel
   DSPLIB_STATUS                      status;
   DSPLIB_conditionalReplace_InitArgs kerInitArgs;
   DSPLIB_conditionalReplace_SetArgs  kerSetArgs;
   int32_t                            handleSize = DSPLIB_conditionalReplace_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle                handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_INT32;
   bufParamsIn.dim_x     = NUM_SAMP;
   bufParamsIn.dim_y     = NUM_CHAN;
   bufParamsIn.stride_y  = NUM_SAMP * sizeof(int32_t);

   bufParamsOut.data_type = DSPLIB_INT32;
   bufParamsOut.dim_x     = NUM_SAMP;
   bufParamsOut.dim_y     = NUM_CHAN;
   bufParamsOut.stride_y  = NUM_SAMP * sizeof(int32_t);

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   // init
   status = DSPLIB_conditionalReplace_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_conditionalReplace_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // set
   kerSetArgs.pCompareType  = pCompareType;
   kerSetArgs.pConstValue   = pConstValue;
   kerSetArgs.pReplaceValue = pReplaceValue;
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_conditionalReplace_set(handle, &kerSetArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_conditionalReplace_exec_checkParams(handle, pIn, pOut);

   // exec
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_conditionalReplace_exec(handle, pIn, pOut);
   }

   // print results
   printf("---------------------------------\n");
   printf("|            Input              |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < NUM_CHAN; i++) {
      printf("\nch %zu (compareType %d, const %d, replace %d): ", i, pCompareType[i], pConstValue[i], pReplaceValue[i]);
      for (size_t j = 0; j < NUM_SAMP; j++) {
         printf("%d,  ", pIn[i * NUM_SAMP + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|           Results             |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < NUM_CHAN; i++) {
      printf("\nch %zu: ", i);
      for (size_t j = 0; j < NUM_SAMP; j++) {
         printf("%d  ", pOut[i * NUM_SAMP + j]);
      }
   }
   printf("\n\n");

   free(handle);
   return (status == DSPLIB_SUCCESS) ? 0 : 1;
}
