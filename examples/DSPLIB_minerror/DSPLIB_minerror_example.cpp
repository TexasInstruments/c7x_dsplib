// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <malloc.h>
#include <stdint.h>

/******************************************************************************/
/*                                                                            */
/* helper functions                                                           */
/*                                                                            */
/******************************************************************************/
void fillRand(void *array, uint32_t width, uint32_t height, uint32_t stride, uint8_t elementSize)
{
   uint32_t row, col;
   if (elementSize == 1) {
      uint8_t *ptr = (uint8_t *) array;
      for (row = 0; row < height; row++) {
         for (col = 0; col < width; col++) {
            ptr[row * stride + col] = rand();
         }
      }
   }
   else if (elementSize == 2) {
      uint16_t *ptr = (uint16_t *) array;
      for (row = 0; row < height; row++) {
         for (col = 0; col < width; col++) {
            ptr[row * stride / 2 + col] = rand();
         }
      }
   }
   else if (elementSize == 4) {
      uint32_t *ptr = (uint32_t *) array;
      for (row = 0; row < height; row++) {
         for (col = 0; col < width; col++) {
            ptr[row * stride / 4 + col] = rand();
         }
      }
   }
   else if (elementSize == 8) {
      uint64_t *ptr = (uint64_t *) array;

      for (row = 0; row < height; row++) {
         for (col = 0; col < width; col++) {
            ptr[row * stride / 8 + col] = rand();
         }
      }
   }
   else {
      printf("Unsupported data type in fillRand\n");
   }
}

void fillRand_float(void *array, uint32_t width, uint32_t height, uint32_t stride, uint8_t elementSize)
{
   uint32_t row, col;
   if (elementSize == 4) {
      float *ptr = (float *) array;
      for (row = 0; row < height; row++) {
         for (col = 0; col < width; col++) {
            ptr[row * (stride / elementSize) + col] = (float) ((double) rand() / ((double) RAND_MAX / 2.0)) - 1.0F;
         }
      }
   }
   else if (elementSize == 8) {
      double *ptr = (double *) array;
      for (row = 0; row < height; row++) {
         for (col = 0; col < width; col++) {
            ptr[row * (stride / elementSize) + col] = ((double) rand() / ((double) RAND_MAX / 2.0)) - 1.0F;
         }
      }
   }
   else {
      printf("Unsupported data type in fillRand_float\n");
   }
}
/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // Setup input and output buffers for different datatypes
   uint32_t width  = 9;
   uint32_t height = 256;

   // Initialize the kernel handle
   DSPLIB_minerror_InitArgs kerInitArgs;
   int32_t                  handleSize = DSPLIB_minerror_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle      handle     = malloc(handleSize);
   DSPLIB_bufParams2D_t     bufParamsIn;
   DSPLIB_bufParams1D_t     bufParamsErrCoefs, bufParamsOutIndex, bufParamsOutVal;

   /*****************************************************************************************
    * Example 1: int8 inputs
    *****************************************************************************************/

   // Stride will change depending on data types
   uint32_t stride = DSPLIB_CALC_STRIDE(width * sizeof(int8_t), DSPLIB_ALIGN_SHIFT_64BYTES);

   // Int8 input matrix 256x9
   void *inMatInt8 = (void *) memalign(DSPLIB_L2DATA_ALIGNMENT, stride * height * sizeof(int8_t));
   if (inMatInt8 == NULL) {
      printf("inMatInt8 pointer is NULL\n");
      exit(1);
   }
   fillRand(inMatInt8, width, height, stride, sizeof(int8_t));

   // Int8 error coefficients vector 1x9
   void *inErrCoeffInt8 = (void *) memalign(DSPLIB_L2DATA_ALIGNMENT, width * sizeof(int8_t));
   if (inErrCoeffInt8 == NULL) {
      printf("inErrCoeffInt8 pointer is NULL\n");
      exit(1);
   }
   fillRand(inErrCoeffInt8, width, 1, 0, sizeof(int8_t));

   // Output values; Index is of data type int32_t always and outval will be 16 bit in this case
   int32_t outIndexInt8 = 0;
   int16_t outValInt8   = 0;

   // Status variables
   DSPLIB_STATUS status_init = DSPLIB_SUCCESS;
   DSPLIB_STATUS status_exec = DSPLIB_SUCCESS;

   // Initialize the kerInitArgs
   kerInitArgs.vecInSize    = height;
   kerInitArgs.errCoefsSize = width;
   kerInitArgs.strideIn     = stride;
   kerInitArgs.funcStyle    = DSPLIB_FUNCTION_OPTIMIZED;

   // Update initArgs and bufparams
   bufParamsIn.data_type = DSPLIB_INT8;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = stride;

   bufParamsErrCoefs.data_type = DSPLIB_INT8;
   bufParamsErrCoefs.dim_x     = width;

   bufParamsOutIndex.dim_x = 1;
   bufParamsOutVal.dim_x   = 1;

   printf("INT8 Example minerror\n");
   // Step 1: Init check params
   status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                  &bufParamsOutVal, &kerInitArgs);
   // Step 2: Init the kernel
   status_init = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex, &bufParamsOutVal,
                                      &kerInitArgs);
   // Step 3: Exec check params
   status_exec = DSPLIB_minerror_exec_checkParams(handle, inMatInt8, inErrCoeffInt8, &outIndexInt8, &outValInt8);
   // Step 4: Exec the kernel
   status_exec = DSPLIB_minerror_exec(handle, inMatInt8, inErrCoeffInt8, &outIndexInt8, &outValInt8);
   // Print the output
   if (status_init != DSPLIB_SUCCESS) {
      printf("INT8 Example minerror initialization failed!\n");
   }
   else if (status_exec != DSPLIB_SUCCESS) {
      printf("INT8 Example minerror execution failed!\n");
   }
   else {
      printf("INT8 Example minerror ran successfully!\nKernel output index = %d; output value = %d\n", outIndexInt8,
             outValInt8);
   }

   free(inMatInt8);
   free(inErrCoeffInt8);

   /*****************************************************************************************
    * Example 2: float32 inputs
    *****************************************************************************************/
   // Stride will change depending on data types
   stride = DSPLIB_CALC_STRIDE(width * sizeof(int8_t), DSPLIB_ALIGN_SHIFT_64BYTES);

   // Allocating aligned memory for inpu and error coefficients
   void *inMatFloat = (void *) memalign(DSPLIB_L2DATA_ALIGNMENT, stride * height * sizeof(float));
   if (inMatFloat == NULL) {
      printf("inMatFloat pointer is NULL\n");
      exit(1);
   }
   fillRand_float(inMatFloat, width, height, stride, sizeof(float));
   // float error coefficients vector 1x9
   void *inErrCoeffFloat = (void *) memalign(DSPLIB_L2DATA_ALIGNMENT, width * sizeof(float));
   if (inErrCoeffFloat == NULL) {
      printf("inErrCoeffFloat pointer is NULL\n");
      exit(1);
   }
   fillRand_float(inErrCoeffFloat, width, 1, 0, sizeof(float));

   // Output values; Index is of data type int32_t always and outval will be float 32 in this case
   int32_t outIndexFloat = 0;
   float   outValFloat   = 0;

   // Status variables
   status_init = DSPLIB_SUCCESS;
   status_exec = DSPLIB_SUCCESS;

   // Initialize the kerInitArgs
   kerInitArgs.vecInSize    = height;
   kerInitArgs.errCoefsSize = width;
   kerInitArgs.strideIn     = stride;
   kerInitArgs.funcStyle    = DSPLIB_FUNCTION_OPTIMIZED;

   // Update initArgs and bufparams
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = stride;

   bufParamsErrCoefs.data_type = DSPLIB_FLOAT32;
   bufParamsErrCoefs.dim_x     = width;

   bufParamsOutIndex.dim_x = 1;
   bufParamsOutVal.dim_x   = 1;

   printf("FLOAT32 Example minerror\n");

   // Step 1: Init check params
   status_init = DSPLIB_minerror_init_checkParams(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex,
                                                  &bufParamsOutVal, &kerInitArgs);
   // Step 2: Init the kernel
   status_init = DSPLIB_minerror_init(handle, &bufParamsIn, &bufParamsErrCoefs, &bufParamsOutIndex, &bufParamsOutVal,
                                      &kerInitArgs);
   // Step 3: Exec check params
   status_exec = DSPLIB_minerror_exec_checkParams(handle, inMatFloat, inErrCoeffFloat, &outIndexFloat, &outValFloat);
   // Step 4: Exec the kernel
   status_exec = DSPLIB_minerror_exec(handle, inMatFloat, inErrCoeffFloat, &outIndexFloat, &outValFloat);
   // Print the output
   if (status_init != DSPLIB_SUCCESS) {
      printf("FLOAT32 Example minerror initialization failed!\n");
   }
   else if (status_exec != DSPLIB_SUCCESS) {
      printf("FLOAT32 Example minerror execution failed!\n");
   }
   else {
      printf("FLOAT32 Example minerror ran successfully!\nKernel output index = %d; output value = %2.4f\n",
             outIndexFloat, outValFloat);
   }

   free(inMatFloat);
   free(inErrCoeffFloat);

   return 0;
}
