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
float in0[] = {0.43816237, 0.92750481, 0.13278964, 0.75629345, 0.28504617, 0.61937450, 0.89412736, 0.07148259,
               0.50316842, 0.34190275, 0.98253760, 0.21947803, 0.66725184, 0.43109529, 0.7896401,  0.27491836,
               0.86143072, 0.05724931, 0.69258409, 0.11903755, 0.94521863, 0.38307641, 0.52791480, 0.76309547,
               0.40672893, 0.98712054, 0.24289361, 0.65573824, 0.31840275, 0.80145938};

float out[] = {0.};

int main(void)
{
   DSPLIB_STATUS          status;
   DSPLIB_addNCh_InitArgs kerInitArgs;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t             handleSize = DSPLIB_addNCh_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_bufParams3D_t bufParamsIn;
   DSPLIB_bufParams2D_t bufParamsOut;

   /* Example to bulk move uint8_t data type*/
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = 5;
   bufParamsIn.dim_y     = 3;
   bufParamsIn.dim_z     = 2;
   bufParamsIn.stride_y  = 5 * sizeof(float);
   bufParamsIn.stride_z  = 5 * sizeof(float) * 3;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = 5;
   bufParamsOut.dim_y     = 2;
   bufParamsOut.stride_y  = 5 * sizeof(float);

   status = DSPLIB_SUCCESS;

   // init checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_addNCh_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_addNCh_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_addNCh_exec_checkParams(handle, in0, out);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_addNCh_exec(handle, in0, out);

   printf("\n FLOAT \n");

   // print results
   for (size_t i = 0; i < 2; i++) {
      printf("\n\n");
      for (size_t j = 0; j < 5; j++) {
         printf("%f, ", out[i * 5 + j]);
      }
   }

   printf("\n\n");
   free(handle);

   return 0;
}
