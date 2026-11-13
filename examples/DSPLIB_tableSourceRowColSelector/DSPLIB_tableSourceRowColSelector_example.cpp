// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ROWS (4)
#define NUM_COLS (3)
#define NUM_INDICES (3)

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // Setup input buffers for single-precision datatype; an index outside the table is clamped to its first or last
   // row (row select) or column (column select)

   int32_t pIn0[] = {2, -5, 99};

   float pIn1[] = {1.0, 0.1, -1.0, 2.0, 0.2, -2.0, 3.0, 0.3, -3.0, 4.0, 0.4, -4.0};

   // handles and struct for call to kernel
   DSPLIB_STATUS                             status;
   DSPLIB_tableSourceRowColSelector_InitArgs kerInitArgs;
   int32_t                                   handleSize = DSPLIB_tableSourceRowColSelector_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle                       handle     = malloc(handleSize);

   if (handle == NULL) {
      printf("Failed to allocate kernel handle\n");
      return -1;
   }

   DSPLIB_bufParams1D_t bufParamsIn0;
   DSPLIB_bufParams2D_t bufParamsIn1, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_INT32;
   bufParamsIn0.dim_x     = NUM_INDICES;

   bufParamsIn1.data_type = DSPLIB_FLOAT32;
   bufParamsIn1.dim_x     = NUM_COLS;
   bufParamsIn1.dim_y     = NUM_ROWS;
   bufParamsIn1.stride_y  = NUM_COLS * sizeof(float);

   // every index selects one table column; the interleaved output writes it as one output column (the AudioWeaver
   // multichannel wire, one channel per index)
   kerInitArgs.funcStyle          = DSPLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.isColSelect        = 1;
   kerInitArgs.isOutputInterleave = 1;

   // length of the table row or column selected by an index
   uint32_t selLen = (kerInitArgs.isColSelect == 1U) ? NUM_ROWS : NUM_COLS;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   if (kerInitArgs.isOutputInterleave == 1U) {
      bufParamsOut.dim_x = NUM_INDICES;
      bufParamsOut.dim_y = selLen;
   }
   else {
      bufParamsOut.dim_x = selLen;
      bufParamsOut.dim_y = NUM_INDICES;
   }
   bufParamsOut.stride_y = bufParamsOut.dim_x * sizeof(float);

   float *pOut = (float *) malloc(bufParamsOut.dim_y * bufParamsOut.stride_y);
   memset(pOut, 0, bufParamsOut.dim_y * bufParamsOut.stride_y);

   // only the interleaved output uses the scratch buffer: roundup(numIndices, lanes) x roundup(selLen, lanes) elements
   void *pScratch = NULL;
   if (kerInitArgs.isOutputInterleave == 1U) {
      uint32_t lanes        = __C7X_VEC_SIZE_BYTES__ / sizeof(float);
      uint32_t scratchRows  = ((NUM_INDICES + lanes - 1U) / lanes) * lanes;
      uint32_t scratchWidth = ((selLen + lanes - 1U) / lanes) * lanes;
      pScratch              = malloc(scratchRows * scratchWidth * sizeof(float));
   }

   if ((pOut == NULL) || ((kerInitArgs.isOutputInterleave == 1U) && (pScratch == NULL))) {
      printf("Failed to allocate the output or scratch buffer\n");
      free(handle);
      free(pOut);
      return -1;
   }

   // init checkparams
   status = DSPLIB_tableSourceRowColSelector_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut,
                                                              &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_tableSourceRowColSelector_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_tableSourceRowColSelector_exec_checkParams(handle, pIn0, pIn1, pOut, pScratch);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_tableSourceRowColSelector_exec(handle, pIn0, pIn1, pOut, pScratch);

   // print results
   printf("---------------------------------\n");
   printf("|            Table              |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < NUM_ROWS; i++) {
      printf("\n\n");
      for (size_t j = 0; j < NUM_COLS; j++) {
         printf("%f,  ", pIn1[i * NUM_COLS + j]);
      }
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|           Indices             |\n");
   printf("---------------------------------\n");
   printf("\n\n");
   for (size_t i = 0; i < NUM_INDICES; i++) {
      printf("%d,  ", pIn0[i]);
   }

   printf("\n\n");
   printf("---------------------------------\n");
   printf("|           Results             |\n");
   printf("---------------------------------\n");
   for (size_t i = 0; i < bufParamsOut.dim_y; i++) {
      printf("\n\n");
      for (size_t j = 0; j < bufParamsOut.dim_x; j++) {
         printf("%f  ", pOut[i * bufParamsOut.dim_x + j]);
      }
   }
   printf("\n\n");

   free(handle);
   free(pOut);
   free(pScratch);

   return 0;
}
