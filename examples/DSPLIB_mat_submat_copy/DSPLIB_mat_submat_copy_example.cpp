// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"

#include <stdint.h>
#include <typeinfo>
using namespace std;

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/
uint8_t uint8X[16][16] = {0};
uint8_t uint8Y[4][4]   = {0};

int8_t int8X[16][16] = {0};
int8_t int8Y[4][4]   = {0};

uint16_t uint16X[16][16] = {0};
uint16_t uint16Y[4][4]   = {0};

int16_t int16X[16][16] = {0};
int16_t int16Y[4][4]   = {0};

uint32_t uint32X[16][16] = {0};
uint32_t uint32Y[4][4]   = {0};

int32_t int32X[16][16] = {0};
int32_t int32Y[4][4]   = {0};

uint64_t uint64X[16][16] = {0};
uint64_t uint64Y[4][4]   = {0};

int64_t int64X[16][16] = {0};
int64_t int64Y[4][4]   = {0};

float floatX[16][16] = {0};
float floatY[4][4]   = {0};

double doubleX[16][16] = {0};
double doubleY[4][4]   = {0};

int32_t widthX  = 16;
int32_t heightX = 16;

int32_t nCols = 4;
int32_t nRows = 4;

int32_t                           stCol = 2;
int32_t                           stRow = 3;
template <typename dataType> void fillMat(int32_t type, dataType *pMat, int32_t row, int32_t col)
{
   if (type) {
      for (int i = 0; i < row; i++) {
         for (int j = 0; j < col; j++) {
            *pMat++ = (dataType) (i * row + j);
         }
      }
   }
   else {
      for (int i = 0; i < row; i++) {
         for (int j = 0; j < col; j++) {
            *pMat++ = (dataType) 0;
         }
      }
   }
}

template <> void fillMat(int32_t type, float *pMat, int32_t row, int32_t col)
{
   if (type) {
      for (int i = 0; i < row; i++) {
         for (int j = 0; j < col; j++) {
            *pMat++ = (float) (i * row + j) / 63;
         }
      }
   }
   else {
      for (int i = 0; i < row; i++) {
         for (int j = 0; j < col; j++) {
            *pMat++ = (float) 0;
         }
      }
   }
}

template <> void fillMat(int32_t type, double *pMat, int32_t row, int32_t col)
{
   if (type) {
      for (int i = 0; i < row; i++) {
         for (int j = 0; j < col; j++) {
            *pMat++ = (double) (i * row + j) / 63;
         }
      }
   }
   else {
      for (int i = 0; i < row; i++) {
         for (int j = 0; j < col; j++) {
            *pMat++ = (double) 0;
         }
      }
   }
}

template <typename dataType> void printMat(dataType *pMat, int32_t row, int32_t col)
{
   for (int i = 0; i < row; i++) {
      for (int j = 0; j < col; j++) {
         printf("%5d, ", *pMat++);
      }
      printf("\n");
   }
}
template <> void printMat(float *pMat, int32_t row, int32_t col)
{
   for (int i = 0; i < row; i++) {
      for (int j = 0; j < col; j++) {
         printf("%6.2f, ", *pMat++);
      }
      printf("\n");
   }
}
template <> void printMat(double *pMat, int32_t row, int32_t col)
{
   for (int i = 0; i < row; i++) {
      for (int j = 0; j < col; j++) {
         printf("%6.2f, ", *pMat++);
      }
      printf("\n");
   }
}
template <typename dataType, int32_t DSPLIB_DATATYPE> void exampleMain(dataType *pXMat, dataType *pYMat)
{
   DSPLIB_mat_submat_copyInitArgs kerInitArgs;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t             handleSize = DSPLIB_mat_submat_copy_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsX, bufParamsY;

   int32_t widthIn = 0, heightIn = 0;
   int32_t widthOut = 0, heightOut = 0;
   bufParamsX.data_type = DSPLIB_DATATYPE;
   bufParamsX.dim_x     = widthX;
   bufParamsX.dim_y     = heightX;
   bufParamsX.stride_y  = widthX * DSPLIB_sizeof(DSPLIB_DATATYPE);

   bufParamsY.data_type = DSPLIB_DATATYPE;
   bufParamsY.dim_x     = nRows;
   bufParamsY.dim_y     = nCols;
   bufParamsY.stride_y  = nCols * DSPLIB_sizeof(DSPLIB_DATATYPE);

   kerInitArgs.dir   = 0;
   kerInitArgs.stRow = stRow;
   kerInitArgs.stCol = stCol;
   kerInitArgs.nRows = nRows;
   kerInitArgs.nCols = nCols;

   void *pX = (void *) pXMat;
   void *pY = (void *) pYMat;

   dataType *pIn;
   dataType *pOut;

   /* Filling the input buffer*/
   fillMat<dataType>(1, pXMat, heightX, widthX);
   fillMat<dataType>(0, pYMat, nRows, nCols);

   printf("\nFunction Parameters:-");
   printf("\nDirection of copy: %d | Start Row: %d  Start Col: %d | No.of Rows: %d No.of Cols: %d |\n", kerInitArgs.dir,
          kerInitArgs.stRow, kerInitArgs.nRows, kerInitArgs.stCol, kerInitArgs.nCols);

   printf("\nX Matrix before copy:\n");
   printMat((dataType *) pX, heightX, widthX);
   printf("\nY Matrix before copy:\n");
   printMat((dataType *) pY, nRows, nCols);
   int32_t status = DSPLIB_mat_submat_copy_init(handle, &bufParamsX, &bufParamsY, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_mat_submat_copy_init(handle, &bufParamsX, &bufParamsY, &kerInitArgs);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_mat_submat_copy_exec_checkParams(handle, pX, pY);
         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_mat_submat_copy_exec(handle, pX, pY);
         }
      }
   }

   printf("\nX Matrix after copy:\n");
   printMat((dataType *) pX, heightX, widthX);
   printf("\nY Matrix after copy:\n");
   printMat((dataType *) pY, nRows, nCols);

   kerInitArgs.dir   = 1;
   kerInitArgs.stRow = stRow;
   kerInitArgs.stCol = stCol;
   kerInitArgs.nRows = nRows;
   kerInitArgs.nCols = nCols;

   /* Filling the input buffer*/
   fillMat<dataType>(1, pXMat, heightX, widthX);
   fillMat<dataType>(1, pYMat, nRows, nCols);

   printf("\n\nReverse Direction\n");
   printf("\nFunction Parameters:-");
   printf("\nDirection of copy: %d | Start Row: %d  Start Col: %d | No.of Rows: %d No.of Cols: %d |\n", kerInitArgs.dir,
          kerInitArgs.stRow, kerInitArgs.nRows, kerInitArgs.stCol, kerInitArgs.nCols);

   printf("\nX Matrix before copy:\n");
   printMat((dataType *) pX, heightX, widthX);
   printf("\nY Matrix before copy:\n");
   printMat((dataType *) pY, nRows, nCols);
   status = DSPLIB_mat_submat_copy_init(handle, &bufParamsX, &bufParamsY, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_mat_submat_copy_init(handle, &bufParamsX, &bufParamsY, &kerInitArgs);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_mat_submat_copy_exec_checkParams(handle, pX, pY);
         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_mat_submat_copy_exec(handle, pX, pY);
         }
      }
   }

   printf("\nX Matrix after copy:\n");
   printMat((dataType *) pX, heightX, widthX);
   printf("\nY Matrix after copy:\n");
   printMat((dataType *) pY, nRows, nCols);
}

int main(void)
{
   printf("\n\nsigned 8bit submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<int8_t, DSPLIB_INT8>(&int8X[0][0], &int8Y[0][0]);
   printf("\n\nunsigned 8bit submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<uint8_t, DSPLIB_UINT8>(&uint8X[0][0], &uint8Y[0][0]);
   printf("\n\nsigned 16bit submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<int16_t, DSPLIB_INT16>(&int16X[0][0], &int16Y[0][0]);
   printf("\n\nunsigned 16bit submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<uint16_t, DSPLIB_UINT16>(&uint16X[0][0], &uint16Y[0][0]);
   printf("\n\nsigned 32bit submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<int32_t, DSPLIB_INT32>(&int32X[0][0], &int32Y[0][0]);
   printf("\n\nunsigned 32bit submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<uint32_t, DSPLIB_UINT32>(&uint32X[0][0], &uint32Y[0][0]);
   printf("\n\nsigned 64bit submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<int64_t, DSPLIB_INT64>(&int64X[0][0], &int64Y[0][0]);
   printf("\n\nunsigned 64bit submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<uint64_t, DSPLIB_UINT64>(&uint64X[0][0], &uint64Y[0][0]);
   printf("\n\nfloat submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<float, DSPLIB_FLOAT32>(&floatX[0][0], &floatY[0][0]);
   printf("\n\ndouble submat copy\n");
   printf("============================================================================================================"
          "=============================");
   exampleMain<double, DSPLIB_FLOAT64>(&doubleX[0][0], &doubleY[0][0]);

   return 0;
}
