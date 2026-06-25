// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <stdint.h>

#define M (4)
#define N (4)
/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // Setup input and output buffers for single-precision datatype

   float pIn0[] = {-1254389271.6729355, 846329740.2856314,  -2034675893.1205897, 1152736492.5378802,
                   432897125.1184939,   -1637409821.695421, 2048793845.2273169,  -573829153.9910835,
                   -1482604712.8025167, 973461827.3405719,  -845293201.9937484,  2073649103.584723,
                   -1395278345.171962,  1810954602.3482516, -621834275.2083961,  1532975018.9154205};

   float pIn1[] = {-1726394018.5432198, 1248693027.1154785, -936420591.7423073, 1583476012.4729936,
                   -453726189.20847654, 2017345693.885102,  -1378491205.514679, 639478215.9043806,
                   -1983045721.3675592, 943217864.2764503,  1150843602.4392853, -1532769841.7853904,
                   1893746021.5721803,  -1048263892.456779, 774198263.9941026,  -1329475805.6279135};

   float pOut[] = {0.};

   // handles and struct for call to kernel
   DSPLIB_STATUS                     status;
   DSPLIB_realImagToComplex_InitArgs kerInitArgs;
   int32_t                           handleSize = DSPLIB_realImagToComplex_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle               handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn0, bufParamsIn1, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_FLOAT32;
   bufParamsIn0.dim_x     = M;
   bufParamsIn0.dim_y     = N;                 // Value
   bufParamsIn0.stride_y  = M * sizeof(float); // Value

   bufParamsIn1.data_type = DSPLIB_FLOAT32;
   bufParamsIn1.dim_x     = M;
   bufParamsIn1.dim_y     = N;                 // Value
   bufParamsIn1.stride_y  = M * sizeof(float); // Value

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = 2 * M;
   bufParamsOut.dim_y     = N;                     // Value
   bufParamsOut.stride_y  = 2 * M * sizeof(float); // Value

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_realImagToComplex_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      DSPLIB_realImagToComplex_exec_checkParams(handle, pIn0, pIn1, pOut);

   // exec
   if (status == DSPLIB_SUCCESS) {

      // to correct status = DSPLIB_realImagToComplex_exec(handle, pIn0, pOut);
      DSPLIB_realImagToComplex_exec(handle, pIn0, pIn1, pOut);
   }

   // print results
   printf("---------------------------------\n");
   printf("|         Input Vector 0        |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%10g,  ", pIn0[i * M + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|         Input Vector 1         |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%10g,  ", pIn1[i * M + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|           Results              |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < 2 * N; j++) {
         printf("%10g  ", pOut[i * 2 * N + j]);
      }
   }

   free(handle);
   return 0;
}
