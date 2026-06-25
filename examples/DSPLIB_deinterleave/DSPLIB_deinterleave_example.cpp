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
uint16_t inMatUint16[16][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143},
    {256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271},
    {384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399},
    {512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527},
    {640, 641, 642, 643, 644, 645, 646, 647, 648, 649, 650, 651, 652, 653, 654, 655},
    {768, 769, 770, 771, 772, 773, 774, 775, 776, 777, 778, 779, 780, 781, 782, 783},
    {896, 897, 898, 899, 900, 901, 902, 903, 904, 905, 906, 907, 908, 909, 910, 911},
    {1024, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037, 1038, 103},
    {1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167},
    {1280, 1281, 1282, 1283, 1284, 1285, 1286, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295},
    {1408, 1409, 1410, 1411, 1412, 1413, 1414, 1415, 1416, 1417, 1418, 1419, 1420, 1421, 1422, 1423},
    {1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543, 1544, 1545, 1546, 1547, 1548, 1549, 1550, 1551},
    {1664, 1665, 1666, 1667, 1668, 1669, 1670, 1671, 1672, 1673, 1674, 1675, 1676, 1677, 1678, 1679},
    {1792, 1793, 1794, 1795, 1796, 1797, 1798, 1799, 1800, 1801, 1802, 1803, 1804, 1805, 1806, 1807},
    {1920, 1921, 1922, 1923, 1924, 1925, 1926, 1927, 1928, 1929, 1930, 1931, 1932, 1933, 1934, 1935}};
uint16_t outMatUint16[16][16] = {0};

int32_t inMatInt32[16][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143},
    {256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271},
    {384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399},
    {512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527},
    {640, 641, 642, 643, 644, 645, 646, 647, 648, 649, 650, 651, 652, 653, 654, 655},
    {768, 769, 770, 771, 772, 773, 774, 775, 776, 777, 778, 779, 780, 781, 782, 783},
    {896, 897, 898, 899, 900, 901, 902, 903, 904, 905, 906, 907, 908, 909, 910, 911},
    {1024, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037, 1038, 1039},
    {1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167},
    {1280, 1281, 1282, 1283, 1284, 1285, 1286, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295},
    {1408, 1409, 1410, 1411, 1412, 1413, 1414, 1415, 1416, 1417, 1418, 1419, 1420, 1421, 1422, 1423},
    {1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543, 1544, 1545, 1546, 1547, 1548, 1549, 1550, 1551},
    {1664, 1665, 1666, 1667, 1668, 1669, 1670, 1671, 1672, 1673, 1674, 1675, 1676, 1677, 1678, 1679},
    {1792, 1793, 1794, 1795, 1796, 1797, 1798, 1799, 1800, 1801, 1802, 1803, 1804, 1805, 1806, 1807},
    {1920, 1921, 1922, 1923, 1924, 1925, 1926, 1927, 1928, 1929, 1930, 1931, 1932, 1933, 1934, 1935},
};
int32_t outMatInt32[16][16] = {0};

double inMatDp[16][16]  = {{0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7},
                           {128, 129, 130, 131, 132, 133, 134, 135, 256, 257, 258, 259, 260, 261, 262, 263},
                           {256, 257, 258, 259, 260, 261, 262, 263, 128, 129, 130, 131, 132, 133, 134, 135},
                           {384, 385, 386, 387, 388, 389, 390, 391, 512, 513, 514, 515, 516, 517, 518, 519},
                           {512, 513, 514, 515, 516, 517, 518, 519, 384, 385, 386, 387, 388, 389, 390, 391},
                           {640, 641, 642, 643, 644, 645, 646, 647, 768, 769, 770, 771, 772, 773, 774, 775},
                           {768, 769, 770, 771, 772, 773, 774, 775, 640, 641, 642, 643, 644, 645, 646, 647},
                           {896, 897, 898, 899, 900, 901, 902, 901, 512, 513, 514, 515, 516, 517, 518, 519},
                           {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7},
                           {128, 129, 130, 131, 132, 133, 134, 135, 256, 257, 258, 259, 260, 261, 262, 263},
                           {256, 257, 258, 259, 260, 261, 262, 263, 128, 129, 130, 131, 132, 133, 134, 135},
                           {384, 385, 386, 387, 388, 389, 390, 391, 512, 513, 514, 515, 516, 517, 518, 519},
                           {512, 513, 514, 515, 516, 517, 518, 519, 384, 385, 386, 387, 388, 389, 390, 391},
                           {640, 641, 642, 643, 644, 645, 646, 647, 768, 769, 770, 771, 772, 773, 774, 775},
                           {768, 769, 770, 771, 772, 773, 774, 775, 640, 641, 642, 643, 644, 645, 646, 647},
                           {896, 897, 898, 899, 900, 901, 902, 901, 512, 513, 514, 515, 516, 517, 518, 519}};
double outMatDp[16][16] = {0};

int32_t width  = 16;
int32_t height = 16;

int main(void)
{

   DSPLIB_deinterleaveInitArgs kerInitArgs;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t             handleSize = DSPLIB_deinterleave_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;

   /* Example for transpose of uint16_t data type */
   bufParamsIn.data_type = DSPLIB_UINT16;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * DSPLIB_sizeof(DSPLIB_UINT16);

   bufParamsOut.data_type = DSPLIB_UINT16;
   bufParamsOut.dim_x     = height;
   bufParamsOut.dim_y     = width;
   bufParamsOut.stride_y  = height * DSPLIB_sizeof(DSPLIB_UINT16);

   int32_t status = DSPLIB_deinterleave_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_deinterleave_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_deinterleave_exec_checkParams(handle, inMatUint16, outMatUint16);
         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_deinterleave_exec(handle, inMatUint16, outMatUint16);
         }
      }
   }

   printf("\n\n16bit Deinterleave\n");
   uint16_t *pInMat0 = &inMatUint16[0][0];
   printf("Input Matrix:\n");
   for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
         printf("%2d, ", *pInMat0++);
      }
      printf("\n");
   }
   uint16_t *pOutMat0 = &outMatUint16[0][0];
   printf("\nOutput Matrix:\n");
   for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
         printf("%2d, ", *pOutMat0++);
      }
      printf("\n");
   }

   /* Example for deinterleave of int32_t data type */
   bufParamsIn.data_type = DSPLIB_INT32;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * DSPLIB_sizeof(DSPLIB_INT32);

   bufParamsOut.data_type = DSPLIB_INT32;
   bufParamsOut.dim_x     = height;
   bufParamsOut.dim_y     = width;
   bufParamsOut.stride_y  = height * DSPLIB_sizeof(DSPLIB_INT32);

   status = DSPLIB_deinterleave_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_deinterleave_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_deinterleave_exec_checkParams(handle, inMatInt32, outMatInt32);
         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_deinterleave_exec(handle, inMatInt32, outMatInt32);
         }
      }
   }

   int32_t *pInMat1 = &inMatInt32[0][0];
   printf("\n\n32bit  deinterleavee\n");
   printf("\nInput Matrix:\n");
   for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
         printf("%2d, ", *pInMat1++);
      }
      printf("\n");
   }
   int32_t *pOutMat1 = &outMatInt32[0][0];
   printf("\nOutput Matrix:\n");
   for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
         printf("%2d, ", *pOutMat1++);
      }
      printf("\n");
   }

   /* Example for deinterleave of double data type */
   bufParamsIn.data_type = DSPLIB_FLOAT64;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * DSPLIB_sizeof(DSPLIB_FLOAT64);

   bufParamsOut.data_type = DSPLIB_FLOAT64;
   bufParamsOut.dim_x     = height;
   bufParamsOut.dim_y     = width;
   bufParamsOut.stride_y  = height * DSPLIB_sizeof(DSPLIB_FLOAT64);

   status = DSPLIB_deinterleave_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_deinterleave_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_deinterleave_exec_checkParams(handle, inMatDp, outMatDp);
         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_interleave_exec(handle, inMatDp, outMatDp);
         }
      }
   }

   double *pInMat2 = &inMatDp[0][0];
   printf("\n\n64bit deinterleave\n");
   printf("\nInput Matrix:\n");
   for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
         printf("%.2lf, ", *pInMat2++);
      }
      printf("\n");
   }

   double *pOutMat2 = &outMatDp[0][0];
   printf("\nOutput Matrix:\n");
   for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
         printf("%.2lf, ", *pOutMat2++);
      }
      printf("\n");
   }

   free(handle);

   return 0;
}
