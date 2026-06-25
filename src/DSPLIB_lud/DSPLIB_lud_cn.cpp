// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_priv.h"

/*********************************************************************
 *
 * INITIALIZATION
 *
 *********************************************************************/

DSPLIB_STATUS DSPLIB_lud_init_cn(DSPLIB_kernelHandle         handle,
                                 const DSPLIB_bufParams2D_t *bufParamsA,
                                 const DSPLIB_bufParams2D_t *bufParamsL,
                                 const DSPLIB_bufParams2D_t *bufParamsU,
                                 const DSPLIB_bufParams2D_t *bufParamsP,
                                 const DSPLIB_ludInitArgs   *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}
/**********************************************************************/
/* NATURAL C IMPLEMENTATION                                           */
/**********************************************************************/
template <typename dataType>
int DSPLIB_lud_cn(int             order,
                  dataType       *A,
                  dataType       *L,
                  dataType       *U,
                  unsigned short *P,
                  const int32_t   strideOrder,
                  const int32_t   strideP)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering nat C c7x implementation");
   int      row, col;
   int      min_row, max_row, k, temp;
   dataType min, max, tmp;
   max_row = 0;
   min_row = 0;

   int32_t dataSize  = sizeof(dataType);
   int32_t dataSizeP = sizeof(unsigned short);

   int32_t orderStride  = strideOrder / dataSize;
   int32_t orderPStride = strideP / dataSizeP;

   /* ------------------------------------------------------------------- */
   /* generate identify matrix                                  		   */
   /* ------------------------------------------------------------------- */
   for (row = 0; row < order; row++) {
      for (col = 0; col < order; col++) {
         if (row == col) {
            P[col + row * orderPStride] = 1;
         }
         else {
            P[col + row * orderPStride] = 0;
         }
      }
   }

   /* ------------------------------------------------------------------- */
   /* LU decomposition                                          */
   /* ------------------------------------------------------------------- */
   memcpy(U, A, sizeof(dataType) * order * orderStride);

   for (k = 0; k < order - 1; k++) {

      /* find min and max entries in column */
      max = 0.0;
      min = 3.4e+38;
      for (row = k; row < order; row++) {
         if (fabs(U[k + row * orderStride]) > max) {
            max     = fabs(U[k + row * orderStride]);
            max_row = row;
         }

         if (fabs(U[k + row * orderStride]) < min) {
            min     = fabs(U[k + row * orderStride]);
            min_row = row;
         }
      }

      /* swap rows if necessary */
      if (k != max_row) {
         for (col = 0; col < order; col++) {
            tmp                             = U[col + min_row * orderStride];
            U[col + min_row * orderStride]  = U[col + max_row * orderStride];
            U[col + max_row * orderStride]  = tmp;
            temp                            = P[col + min_row * orderPStride];
            P[col + min_row * orderPStride] = P[col + max_row * orderPStride];
            P[col + max_row * orderPStride] = temp;
         }
      }

      /* generate U matrix */

      dataType mulFactor = 1.0 / U[k + k * orderStride];
      for (row = k + 1; row < order; row++) {
         U[k + row * orderStride] *= mulFactor;
         // U[k + row * orderStride] /= U[k + k * orderStride];
      }

      for (row = k + 1; row < order; row++) {
         for (col = k + 1; col < order; col++) {
            U[col + row * orderStride] -= U[k + row * orderStride] * U[col + k * orderStride];
         }
      }
   }

   /* extract lower triangular entries from L into U and set L lower entries to zero */
   for (row = 0; row < order; row++) {
      for (col = 0; col < order; col++) {
         if (row < col) {
            L[col + row * orderStride] = 0;
         }
         else {
            if (row == col) {
               L[col + row * orderStride] = 1;
            }
            else {
               L[col + row * orderStride] = U[col + row * orderStride];
               U[col + row * orderStride] = 0;
            }
         }
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting nat C c7x implementation");
   return 0;
}

template int DSPLIB_lud_cn<float>(int             order,
                                  float          *A,
                                  float          *L,
                                  float          *U,
                                  unsigned short *P,
                                  const int32_t   strideOrder,
                                  const int32_t   strideP);
template int DSPLIB_lud_cn<double>(int             order,
                                   double         *A,
                                   double         *L,
                                   double         *U,
                                   unsigned short *P,
                                   const int32_t   strideOrder,
                                   const int32_t   strideP);

template <typename dataType>
DSPLIB_STATUS DSPLIB_lud_exec_cn(DSPLIB_kernelHandle handle,
                                 void *restrict pA,
                                 void *restrict pL,
                                 void *restrict pU,
                                 void *restrict pP)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering exec_cn function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_lud_PrivArgs *pKerPrivArgs = (DSPLIB_lud_PrivArgs *) handle;
   int32_t              order        = pKerPrivArgs->order;
   int32_t              strideOrder  = pKerPrivArgs->strideOrder;
   int32_t              strideP      = pKerPrivArgs->strideP;

   /* Typecast void pointers to respective data type */
   dataType       *pALocal = (dataType *) pA;
   dataType       *pLLocal = (dataType *) pL;
   dataType       *pULocal = (dataType *) pU;
   unsigned short *pPLocal = (unsigned short *) pP;

   DSPLIB_DEBUGPRINTFN(0, "pALocal: %p pLLocal: %p pULocal: %p pPLocal: %p\n", pALocal, pLLocal, pULocal, pPLocal);

   DSPLIB_lud_cn<dataType>(order, pALocal, pLLocal, pULocal, pPLocal, strideOrder, strideP);

   DSPLIB_DEBUGPRINTFN(0, "Exiting exec_cn function with return status: %d\n", status);

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_lud_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                 void *restrict pA,
                                                 void *restrict pL,
                                                 void *restrict pU,
                                                 void *restrict pP);

template DSPLIB_STATUS DSPLIB_lud_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                  void *restrict pA,
                                                  void *restrict pL,
                                                  void *restrict pU,
                                                  void *restrict pP);
