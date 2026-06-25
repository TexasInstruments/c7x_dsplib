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

#define COL1 6
#define ROW1 6

#define COL2 7
#define ROW2 6

/*  TEST CASE FOR Nrows > Ncol */
double A_6X6[ROW1][COL1] = {
    {-9.847830124912404, 1.979392600613032, 7.016533023823769, -9.201005197765046, -3.1018801408921988,
     4.475792827977605},
    {-8.523550955779287, -7.186764665131948, -1.5480291073245134, 9.052286663579466, -2.287853355058809,
     2.707569996168516},
    {8.006169011551002, 1.0672434611005528, 6.4224538296900135, -4.499214674419864, -4.899934661344023,
     -8.465081600451507},
    {-8.296862209749111, -4.962243833152868, -6.262419011905176, 4.3561498253580595, 8.135535799702346,
     6.886973848548486},
    {3.4832240257197196, 5.320260655910364, -5.65036641017249, 3.3975206559555797, 8.168547876791365,
     7.6041399365338975},
    {9.76396526272125, 3.5423101425755927, 4.076085251430415, -9.805989929219162, 9.981225834353008, 8.677277465953562}

};
double U_6X6[ROW1][ROW1];
double U1_6X6[ROW1][ROW1];
double V_6X6[ROW1][ROW1];
double diag_6X6[ROW1];
double superDiag_6X6[ROW1];

/*  TEST CASE FOR Nrows < Ncol */
double A_6X7[ROW2][COL2] = {{4.674792492189724, -4.770404783566525, 7.903905058245247, 0.5895952397358855,
                             1.301206254340947, -4.998587886901196, -2.4594525834888348},
                            {-4.185743185305553, 7.210098885222543, 0.24032530121381157, -0.04525928023304715,
                             -4.516192347919201, -0.38821779925834754, 9.65002213379114},
                            {-4.534703858904823, 5.926291603878939, 7.47828253541374, 9.683248817581749,
                             -9.324096950123844, 5.215878346909118, 3.7505785512352574},
                            {-1.0140822088231793, -6.012453814971077, 6.130800001926563, -8.607174393254766,
                             6.574397099907372, 5.602638697913333, 2.500392156166386},
                            {0.4732793545014147, -7.66943476886955, 9.846355101605589, 0.915179217067589,
                             3.645832437466403, -8.313466039247686, -3.657916137498896},
                            {5.780451756481735, -2.0718564211336465, -5.134390117327577, 9.214168516880264,
                             9.321494375210975, 2.1357774491257047, 1.935069214283649}

};
double U_6X7[COL2][COL2];
double U1_6X7[COL2][COL2];
double V_6X7[COL2][COL2];
double diag_6X7[ROW2];
double superDiag_6X7[ROW2];

int32_t Ncols;
int32_t Nrows;

int main(void)
{

   DSPLIB_bufParams1D_t      bufParamsDiag, bufParamsSuperDiag;
   DSPLIB_bufParams2D_t      bufParamsA, bufParamsU, bufParamsV, bufParamsU1;
   DSPLIB_svd_small_InitArgs kerInitArgs;
   DSPLIB_kernelHandle       handle;
   int32_t                   handleSize, status;

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
   handleSize            = DSPLIB_svd_small_getHandleSize(&kerInitArgs);
   handle                = malloc(handleSize);

   bufParamsDiag.data_type      = DSPLIB_FLOAT64;
   bufParamsSuperDiag.data_type = DSPLIB_FLOAT64;
   bufParamsA.data_type         = DSPLIB_FLOAT64;
   bufParamsU.data_type         = DSPLIB_FLOAT64;
   bufParamsU1.data_type        = DSPLIB_FLOAT64;
   bufParamsV.data_type         = DSPLIB_FLOAT64;

   /* *****************************************************************************************
   *********************************** Example for 6 X 6 input matrix *************************
   ******************************************************************************************** */

   Ncols = COL1;
   Nrows = ROW1;

   bufParamsDiag.dim_x = Ncols;

   bufParamsSuperDiag.dim_x = Ncols;

   bufParamsA.dim_x    = Ncols;
   bufParamsA.dim_y    = Nrows;
   bufParamsA.stride_y = bufParamsA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT64);

   bufParamsU.dim_x = bufParamsU1.dim_x = Nrows;
   bufParamsU.dim_y = bufParamsU1.dim_y = Nrows;
   bufParamsU.stride_y = bufParamsU1.stride_y = bufParamsU.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT64);

   bufParamsV.dim_x    = Ncols;
   bufParamsV.dim_y    = Ncols;
   bufParamsV.stride_y = bufParamsV.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT64);

   kerInitArgs.dimX        = Ncols;
   kerInitArgs.dimY        = Nrows;
   kerInitArgs.strideURows = bufParamsU.stride_y;
   kerInitArgs.strideVRows = bufParamsV.stride_y;

   /* Non-reduced form */
   kerInitArgs.enableReducedForm = 0;

   status = DSPLIB_svd_small_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                              &bufParamsSuperDiag, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_svd_small_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperDiag,
                                     &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_svd_small_exec_checkParams(handle, &A_6X6, &U_6X6, &V_6X6, &diag_6X6, &superDiag_6X6, &U1_6X6);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_svd_small_exec(handle, &A_6X6, &U_6X6, &V_6X6, &diag_6X6, &superDiag_6X6, &U1_6X6);
         }
      }
   }

   printf("\n\nExample for:\nInput Matrix A[Nrows X Ncols] => A[%d X %d]\n", Nrows, Ncols);
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f ", A_6X6[i][j]);
      }
      printf(";\n");
   }

   printf("\nOutput in non-reduced form:\n");
   printf("Matrix U: \n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Nrows; j++) {
         printf("%7.3f, ", U_6X6[i][j]);
      }
      printf("\n");
   }

   printf("\nSingular values:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      printf("%7.3f, ", diag_6X6[i]);
   }
   printf("\n");

   printf("\nMatrix V:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f, ", V_6X6[i][j]);
      }
      printf("\n");
   }

   /* Reduced form */
   kerInitArgs.enableReducedForm = 1;

   status = DSPLIB_svd_small_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                              &bufParamsSuperDiag, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_svd_small_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperDiag,
                                     &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_svd_small_exec_checkParams(handle, &A_6X6, &U_6X6, &V_6X6, &diag_6X6, &superDiag_6X6, &U1_6X6);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_svd_small_exec(handle, &A_6X6, &U_6X6, &V_6X6, &diag_6X6, &superDiag_6X6, &U1_6X6);
         }
      }
   }

   printf("\nOutput in reduced form:\n");
   printf("Matrix U: \n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f, ", U_6X6[i][j]);
      }
      printf("\n");
   }

   printf("\nSingular values:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      printf("%7.3f, ", diag_6X6[i]);
   }
   printf("\n");

   printf("\nMatrix V:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f, ", V_6X6[i][j]);
      }
      printf("\n");
   }

   /* *****************************************************************************************
   *********************************** Example for 6X7 input matr******************************
   ******************************************************************************************** */

   Ncols = COL2;
   Nrows = ROW2;

   bufParamsDiag.dim_x = Nrows;

   bufParamsSuperDiag.dim_x = Nrows;

   bufParamsA.dim_x    = Ncols;
   bufParamsA.dim_y    = Nrows;
   bufParamsA.stride_y = bufParamsA.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT64);

   bufParamsU.dim_x = bufParamsU1.dim_x = Ncols;
   bufParamsU.dim_y = bufParamsU1.dim_y = Ncols;
   bufParamsU.stride_y = bufParamsU1.stride_y = bufParamsU.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT64);

   bufParamsV.dim_x    = Ncols;
   bufParamsV.dim_y    = Ncols;
   bufParamsV.stride_y = bufParamsV.dim_x * DSPLIB_sizeof(DSPLIB_FLOAT64);

   kerInitArgs.dimX        = Ncols;
   kerInitArgs.dimY        = Nrows;
   kerInitArgs.strideURows = bufParamsU.stride_y;
   kerInitArgs.strideVRows = bufParamsV.stride_y;

   /* Non-reduced form */
   kerInitArgs.enableReducedForm = 0;

   status = DSPLIB_svd_small_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                              &bufParamsSuperDiag, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_svd_small_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperDiag,
                                     &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_svd_small_exec_checkParams(handle, &A_6X7, &U_6X7, &V_6X7, &diag_6X7, &superDiag_6X7, &U1_6X7);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_svd_small_exec(handle, &A_6X7, &U_6X7, &V_6X7, &diag_6X7, &superDiag_6X7, &U1_6X7);
         }
      }
   }

   printf("\n\nExample for \nInput Matrix[Nrows X Ncols] => A[%d X %d]\n", Nrows, Ncols);
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f ", A_6X7[i][j]);
      }
      printf(";\n");
   }

   printf("\nOutput in non-reduced form:\n");
   printf("Matrix U: \n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Nrows; j++) {
         printf("%7.3f, ", U_6X7[i][j]);
      }
      printf("\n");
   }

   printf("\nSingular values:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      printf("%7.3f, ", diag_6X7[i]);
   }
   printf("\n");

   printf("\nMatrix V:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      for (int32_t j = 0; j < Ncols; j++) {
         printf("%7.3f, ", V_6X7[i][j]);
      }
      printf("\n");
   }

   /* Reduced form */
   kerInitArgs.enableReducedForm = 1;

   status = DSPLIB_svd_small_init_checkParams(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag,
                                              &bufParamsSuperDiag, &kerInitArgs);
   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_svd_small_init(handle, &bufParamsA, &bufParamsU, &bufParamsV, &bufParamsDiag, &bufParamsSuperDiag,
                                     &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_svd_small_exec_checkParams(handle, &A_6X7, &U_6X7, &V_6X7, &diag_6X7, &superDiag_6X7, &U1_6X7);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_svd_small_exec(handle, &A_6X7, &U_6X7, &V_6X7, &diag_6X7, &superDiag_6X7, &U1_6X7);
         }
      }
   }

   printf("\nOutput in reduced form:\n");
   printf("Matrix U: \n");
   for (int32_t i = 0; i < Nrows; i++) {
      for (int32_t j = 0; j < Nrows; j++) {
         printf("%7.3f, ", U_6X7[i][j]);
      }
      printf("\n");
   }

   printf("\nSingular values:\n");
   for (int32_t i = 0; i < Nrows; i++) {
      printf("%7.3f, ", diag_6X7[i]);
   }
   printf("\n");

   printf("\nMatrix V:\n");
   for (int32_t i = 0; i < Ncols; i++) {
      for (int32_t j = 0; j < Nrows; j++) {
         printf("%7.3f, ", V_6X7[i][j]);
      }
      printf("\n");
   }

   free(handle);
   return 0;
}
