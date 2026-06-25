// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_inv_priv.h"

/*********************************************************************
 *
 * INITIALIZATION
 *
 *********************************************************************/

DSPLIB_STATUS DSPLIB_lud_inv_init_cn(DSPLIB_kernelHandle           handle,
                                     const DSPLIB_bufParams2D_t   *bufParamsP,
                                     const DSPLIB_bufParams2D_t   *bufParamsL,
                                     const DSPLIB_bufParams2D_t   *bufParamsU,
                                     const DSPLIB_bufParams2D_t   *bufParamsinvA,
                                     const DSPLIB_lud_invInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*  NATURAL C IMPLEMENTATION                                          */
/**********************************************************************/

template <typename dataType>
int DSPLIB_lud_inv_cn(const int       order,
                      unsigned short *P,
                      dataType       *L,
                      dataType       *U,
                      dataType       *invA,
                      const int32_t   strideOrder,
                      const int32_t   strideP)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering nat C c7x implementation");

   int       row, col, k;
   dataType  factor, sum;
   dataType *inv_L, *inv_U, *inv_U_x_inv_L;

   int32_t dataSize  = sizeof(dataType);
   int32_t dataSizeP = sizeof(unsigned short);

   int32_t orderStride  = strideOrder / dataSize;
   int32_t orderPStride = strideP / dataSizeP;

   /* set inv_A matrix to identity */
   inv_L = &invA[0];
   for (row = 0; row < order; row++) {
      for (col = 0; col < order; col++) {
         if (row == col) {
            inv_L[col + row * orderStride] = 1.0;
         }
         else {
            inv_L[col + row * orderStride] = 0.0;
         }
      }
   }

   /* use Gauss Jordan algorithm to invert L whose result is in inv_L */
   for (col = 0; col < order - 1; col++) {
      for (row = col + 1; row < order; row++) {
         dataType mulFact = 1 / L[col + col * orderStride];
         ;
         factor = L[col + row * orderStride] * mulFact;
         for (k = 0; k < order; k++) {
            inv_L[k + row * orderStride] -= factor * inv_L[k + col * orderStride];
            L[k + row * orderStride] -= factor * L[k + col * orderStride];
         }
      }
   }

   /* set inv_U matrix to identity */
   inv_U = &L[0];
   for (row = 0; row < order; row++) {
      for (col = 0; col < order; col++) {
         if (row == col) {
            inv_U[col + row * orderStride] = 1.0;
         }
         else {
            inv_U[col + row * orderStride] = 0.0;
         }
      }
   }

   /* use Gauss Jordan algorithm to invert U whose result is in L */
   for (col = order - 1; col >= 1; col--) {
      factor           = U[col + col * orderStride];
      dataType mulFact = 1 / factor;
      for (row = col - 1; row >= 0; row--) {
         factor = U[col + row * orderStride] * mulFact;
         for (k = 0; k < order; k++) {
            inv_U[k + row * orderStride] -= factor * inv_U[k + col * orderStride];
            U[k + row * orderStride] -= factor * U[k + col * orderStride];
         }
      }
   }

   /* scale U & L to get identity matrix in U */
   for (row = order - 1; row >= 0; row--) {
      factor           = U[row + row * orderStride];
      dataType mulFact = 1 / factor;
      for (col = 0; col < order; col++) {
         L[col + row * orderStride] *= mulFact;
         U[col + row * orderStride] *= mulFact;
      }
   }

   /* compute inv_U_x_inv_L=inv(U)*inv(L) */
   inv_U_x_inv_L = &L[0];
   for (row = 0; row < order; row++) {
      for (col = 0; col < order; col++) {
         sum = 0;
         for (k = 0; k < order; k++) {
            sum += inv_U[k + row * orderStride] * inv_L[col + k * orderStride];
         }
         inv_U_x_inv_L[col + row * orderStride] = sum;
      }
   }
   /* compute inv_A=inv(U)*inv(L)*P */
   for (row = 0; row < order; row++) {
      for (col = 0; col < order; col++) {
         sum = 0;
         for (k = 0; k < order; k++) {
            sum += inv_U_x_inv_L[k + row * orderStride] * P[col + k * orderPStride];
         }
         invA[col + row * orderStride] = sum;
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting nat C c7x implementation");
   return 0;
}

template int DSPLIB_lud_inv_cn<float>(const int       order,
                                      unsigned short *P,
                                      float          *L,
                                      float          *U,
                                      float          *invA,
                                      const int32_t   strideOrder,
                                      const int32_t   strideP);
template int DSPLIB_lud_inv_cn<double>(const int       order,
                                       unsigned short *P,
                                       double         *L,
                                       double         *U,
                                       double         *invA,
                                       const int32_t   strideOrder,
                                       const int32_t   strideP);

template <typename dataType>
DSPLIB_STATUS DSPLIB_lud_inv_exec_cn(DSPLIB_kernelHandle handle,
                                     void *restrict pP,
                                     void *restrict pL,
                                     void *restrict pU,
                                     void *restrict pinvA,
                                     void *restrict pStratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering exec_cn function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_lud_inv_PrivArgs *pKerPrivArgs = (DSPLIB_lud_inv_PrivArgs *) handle;

   int32_t order       = pKerPrivArgs->order;
   int32_t strideOrder = pKerPrivArgs->strideOrder;
   int32_t strideP     = pKerPrivArgs->strideP;

   /* Typecast void pointers to respective data type */
   unsigned short *pPLocal    = (unsigned short *) pP;
   dataType       *pLLocal    = (dataType *) pL;
   dataType       *pULocal    = (dataType *) pU;
   dataType       *pinvALocal = (dataType *) pinvA;

   DSPLIB_DEBUGPRINTFN(0, "pPLocal: %p pLLocal: %p pULocal: %p pinvALocal: %p order: %d\n", pPLocal, pLLocal, pULocal,
                       pinvALocal, order);

   DSPLIB_lud_inv_cn<dataType>(order, pPLocal, pLLocal, pULocal, pinvALocal, strideOrder, strideP);

   DSPLIB_DEBUGPRINTFN(0, "Exiting exec_cn function with return status: %d\n", status);

   return (status);
}

template DSPLIB_STATUS DSPLIB_lud_inv_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                     void *restrict pP,
                                                     void *restrict pL,
                                                     void *restrict pU,
                                                     void *restrict pinv_A,
                                                     void *restrict pStratch);

template DSPLIB_STATUS DSPLIB_lud_inv_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                      void *restrict pP,
                                                      void *restrict pL,
                                                      void *restrict pU,
                                                      void *restrict pinv_A,
                                                      void *restrict pStratch);
