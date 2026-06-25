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
   float in0[] = {418111121.4043479,   493975580.58166695,  -1327690694.7435732, 1609594951.4586601,
                  -658049711.6722021,  541939824.9155822,   -189110757.01818705, 1913006391.8430467,
                  -969557936.6827188,  -1024963586.8234506, -1056346069.3127146, -623394215.9058976,
                  -1401206938.2401671, 281881217.16167545,  1799136167.874629,   607695498.958703};

   float in1[] = {1761916077.6357298, 682121377.3916097,  1822035142.2268505, -544806636.1134768,
                  -196997566.6708808, 427459523.88139486, -949815065.1222329, -1184859293.4892488,
                  163272664.4674425,  1578533388.5449424, -516238511.4353714, 831302523.3456612,
                  1428561529.2142568, 197699700.19539976, 16053252.784401894, -1178531593.6398888};

   float out[] = {0.};

   uint32_t size_in  = 16;
   uint32_t size_out = 1;

   // handles and struct for call to kernel
   DSPLIB_STATUS           status;
   DSPLIB_dotprod_InitArgs kerInitArgs;
   int32_t                 handleSize = DSPLIB_dotprod_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle     handle     = malloc(handleSize);

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
      status = DSPLIB_dotprod_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_dotprod_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      DSPLIB_dotprod_exec_checkParams(handle, in0, in1, out);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_dotprod_exec(handle, in0, in1, out);

   // print results
   printf("---------------------------------\n");
   printf("| Input Vec0    | Input Vec1    |\n");
   printf("---------------------------------\n");
   for (size_t c = 0; c < size_in; c++) {
      printf("| %10g \t| %10g \t|\n", in0[c], in1[c]);
   }
   printf("---------------------------------\n");
   printf("Dotprod of Vec0.Vec1= %10g\n", out[0]);

   return 0;
}
