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
   float in[] = {0.0075269,  0.42490765, 0.5737673,  0.2208442,  0.09775102, 0.24138574, 0.04012135, 0.74615402,
                 0.08575411, 0.02825585, 0.0394516,  0.53011919, 0.8855483,  0.64490047, 0.43757865, 0.9008144,
                 0.88742952, 0.5273014,  0.15618296, 0.2486165,  0.73751811, 0.80145564, 0.00925736, 0.61880944,
                 0.01848634, 0.78157248, 0.29912587, 0.34630224, 0.28394529, 0.16470226, 0.03790734, 0.05151005,
                 0.93717938, 0.93691852, 0.85173587, 0.99901312, 0.72999805, 0.07310897, 0.68994449, 0.80322511,
                 0.59070248, 0.52609875, 0.8837833,  0.43461102, 0.29789329, 0.8795983,  0.08831519, 0.25615793,
                 0.57545443, 0.76529679, 0.7559087,  0.60065098, 0.93648813, 0.9509784,  0.33040872, 0.07305676,
                 0.79920768, 0.03319901, 0.90727184, 0.59441656, 0.42762858, 0.92020711, 0.80031864, 0.01848634};

   float filter[] = {0.68517633, 0.62932418, 0.40943144, 0.83713068, 0.32060292, 0.92259115, 0.96543234, 0.63864105,
                     0.82545659, 0.20080124, 0.85765419, 0.21333606, 0.04969507, 0.87302331, 0.13975056, 0.94914013,
                     0.97427525, 0.18113375, 0.65753913, 0.64872713, 0.65282617, 0.81546733, 0.22026294, 0.85940409,
                     0.21061344, 0.31353881, 0.27924398, 0.53946946, 0.86391542, 0.94781037, 0.73137234, 0.01975425};

   float out[] = {
       0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
       0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
       0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
   };

   uint32_t dataSize = 64;
   // in bytes
   uint32_t dataPitchInSize = 64 * 4;
   // in bytes
   uint32_t dataPitchOutSize = 64 * 4;
   uint32_t batchSize        = 1;
   uint32_t filterSize       = 32;
   // float data type
   uint32_t shift = 1;

   // handles and struct for call to kernel
   DSPLIB_STATUS       status;
   DSPLIB_fir_InitArgs kerInitArgs;
   int32_t             handleSize = DSPLIB_fir_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;
   DSPLIB_bufParams1D_t bufParamsFilter;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = dataSize;
   bufParamsIn.stride_y  = dataPitchInSize;
   bufParamsIn.dim_y     = batchSize;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = size;
   bufParamsOut.stride_y  = dataPitchOutSize;
   bufParamsOut.dim_y     = batchSize;

   bufParamsFilter.data_type = DSPLIB_FLOAT32;
   bufParamsFilter.dim_x     = filterSize;

   kerInitArgs.dataSize   = size;
   kerInitArgs.batchSize  = batchSize;
   kerInitArgs.filterSize = filterSize;
   kerInitArgs.shift      = shift;
   kerInitArgs.funcStyle  = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_fir_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

   // exec checkparams

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_fir_exec(handle, in, filter, out);

   // print results
   for (size_t c = 0; c < ; c++) {
      printf("%10g (*) %10g = %10g\n", in[c], filter[c], out[c]);
   }

   return 0;
}
