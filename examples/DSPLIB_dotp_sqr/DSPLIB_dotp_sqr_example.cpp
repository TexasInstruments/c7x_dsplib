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

   // Setup input and output buffers for single-precision datatypes
   float in0[] = {347151321.4895425,  -1508965912.7120323, 315476603.5976496,  -2006259649.5821486,
                  -1986449688.703281, -1849293011.7608643, -143628852.8778286, -1087024509.198186,
                  1709583718.5658712, -1316763647.455204,  718943339.9298277,  98548501.02118254,
                  -810278836.9877863, 513476644.30877876,  -1363130012.586555, -506771959.6431799};

   float in1[] = {-1221823816.406968,  -723063524.3307643,  -1206879942.3232794, 1181882002.966154,
                  -220585387.26494932, 738710549.0486851,   1990868463.1102962,  -781941101.8854094,
                  -495348338.08435726, -1879199371.4832664, 946852834.2490883,   201518919.34937048,
                  -121520689.39983368, 1904112500.906951,   1807522028.0387173,  1898638530.4437623};

   float expected_out[] = {-2.47051805505871e+18, 2.5264695138850492e+19};

   float out[] = {0., 0.};

   uint32_t size_in  = 16;
   uint32_t size_out = 2;

   // handles and struct for call to kernel
   DSPLIB_STATUS            status;
   DSPLIB_dotp_sqr_InitArgs kerInitArgs;
   int32_t                  handleSize = DSPLIB_dotp_sqr_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle      handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = size_in;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = size_out;

   kerInitArgs.dataSize  = size_in;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_dotp_sqr_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_dotp_sqr_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      DSPLIB_dotp_sqr_exec_checkParams(handle, in0, in1, out);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_dotp_sqr_exec(handle, in0, in1, out);

   // print results
   printf("\n");
   printf("---------------------------------\n");
   printf("| Input Vec0    | Input Vec1    |\n");
   printf("---------------------------------\n");
   for (size_t c = 0; c < size_in; c++) {
      printf("| %10g \t| %10g \t|\n", in0[c], in1[c]);
   }

   printf("-------------------------------------------------\n");
   printf("|                   Results                     |\n");
   printf("-------------------------------------------------\n");
   printf("| Operation     | Expected      | Actual        |\n");
   printf("-------------------------------------------------\n");
   printf("| Dotproduct \t| %10g \t| %10g \t|\n", expected_out[0], out[0]);
   printf("| Square Add \t| %10g \t| %10g \t|\n", expected_out[1], out[1]);

   return 0;
}
