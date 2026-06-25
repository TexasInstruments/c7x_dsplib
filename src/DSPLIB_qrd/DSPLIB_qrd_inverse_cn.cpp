// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_inverse_priv.h"
#include <cstring>

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_inverse_init_cn(DSPLIB_kernelHandle          handle,
                                         DSPLIB_bufParams2D_t        *bufParamsQ,
                                         DSPLIB_bufParams2D_t        *bufParamsR,
                                         DSPLIB_bufParams2D_t        *bufParamsInvA,
                                         DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                         const DSPLIB_qrdInvInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}
template DSPLIB_STATUS DSPLIB_qrd_inverse_init_cn<float>(DSPLIB_kernelHandle          handle,
                                                         DSPLIB_bufParams2D_t        *bufParamsQ,
                                                         DSPLIB_bufParams2D_t        *bufParamsR,
                                                         DSPLIB_bufParams2D_t        *bufParamsInvA,
                                                         DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                                         const DSPLIB_qrdInvInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_qrd_inverse_init_cn<double>(DSPLIB_kernelHandle          handle,
                                                          DSPLIB_bufParams2D_t        *bufParamsQ,
                                                          DSPLIB_bufParams2D_t        *bufParamsR,
                                                          DSPLIB_bufParams2D_t        *bufParamsInvA,
                                                          DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                                          const DSPLIB_qrdInvInitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/
template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_inverse_exec_cn(DSPLIB_kernelHandle handle,
                                         void *restrict pQ,
                                         void *restrict pR,
                                         void *restrict pInvA,
                                         void *restrict pLocalInvAScracth,
                                         void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_qrd_inverse_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_inverse_PrivArgs *) handle;
   int32_t                      strideQ      = pKerPrivArgs->strideQ;
   int32_t                      strideR      = pKerPrivArgs->strideR;
   int32_t                      heightR      = pKerPrivArgs->heightR;
   int32_t                      widthR       = pKerPrivArgs->widthR;
   int32_t                      strideInvA   = pKerPrivArgs->strideInvA;
   int32_t                      dataSize     = sizeof(dataType);

   /* Typecast void pointers to respective data type */
   dataType *pLocalQ    = (dataType *) pQ;
   dataType *pLocalR    = (dataType *) pR;
   dataType *pLocalInvA = (dataType *) pInvA;

   int32_t colRstride    = strideR / dataSize;
   int32_t colQstride    = strideQ / dataSize;
   int32_t colInvAStride = strideInvA / dataSize;

   int32_t row;
   int32_t col;
   int32_t k;

   dataType sum;
   dataType factor;

   DSPLIB_DEBUGPRINTFN(0, "pLocalQ: %p pLocalR: %p pLocalInvA: %p  widthR: %d heightR: %d\n", pLocalQ, pLocalR,
                       pLocalInvA, widthR, heightR);

   /* ------------------------------------------------------------------- */
   /*  Write each column of 'pLocal' to a row of 'pLocalInvA'.           */
   /* ------------------------------------------------------------------- */

   /* set pLocalInvA matrix to identity */
   for (row = 0; row < heightR; row++) {
      for (col = 0; col < widthR; col++) {
         if (row == col) {
            pLocalInvA[col + row * colInvAStride] = 1.0;
         }
         else {
            pLocalInvA[col + row * colInvAStride] = 0.0;
         }
      }
   }

   /* use Gauss Jordan algorithm to invert R whose result will be in pLocalInvA */
   for (col = widthR - 1; col >= 1; col--) {
      for (row = col - 1; row >= 0; row--) {
         factor = pLocalR[col + row * colRstride] / pLocalR[col + col * colRstride];
         for (k = 0; k < widthR; k++) {
            pLocalInvA[k + row * colInvAStride] -= factor * pLocalInvA[k + col * colInvAStride];
            pLocalR[k + row * colRstride] -= factor * pLocalR[k + col * colRstride];
         }
      }
   }

   /* scale R and pLocalInvA to get identity matrix in R */
   for (row = heightR - 1; row >= 0; row--) {
      factor = pLocalR[row + row * colRstride];
      for (col = 0; col < widthR; col++) {
         pLocalInvA[col + row * colInvAStride] /= factor;
         pLocalR[col + row * colRstride] /= factor;
      }
   }

   /* pLocalInvA = inv_R * Q' */
   for (row = 0; row < heightR; row++) {
      for (col = 0; col < widthR; col++) {
         sum = 0.0;
         for (k = 0; k < widthR; k++) {
            sum += pLocalInvA[k + row * colInvAStride] * pLocalQ[k + col * colQstride];
         }
         pLocalR[col] = sum;
      }
      for (col = 0; col < widthR; col++) {
         pLocalInvA[col + row * colInvAStride] = pLocalR[col];
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_qrd_inverse_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                         void *restrict pQ,
                                                         void *restrict pR,
                                                         void *restrict pInvA,
                                                         void *restrict pLocalInvAScracth,
                                                         void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_qrd_inverse_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                          void *restrict pQ,
                                                          void *restrict pR,
                                                          void *restrict pInvA,
                                                          void *restrict pLocalInvAScracth,
                                                          void *restrict pScratch);
