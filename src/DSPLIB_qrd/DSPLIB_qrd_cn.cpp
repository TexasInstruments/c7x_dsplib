// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_priv.h"
#include <cstring>

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_init_cn(DSPLIB_kernelHandle       handle,
                                 DSPLIB_bufParams2D_t     *bufParamsA,
                                 DSPLIB_bufParams2D_t     *bufParamsQ,
                                 DSPLIB_bufParams2D_t     *bufParamsR,
                                 DSPLIB_bufParams1D_t     *bufParamsU,
                                 const DSPLIB_qrdInitArgs *pKerInitArgs)
{

   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}
template DSPLIB_STATUS DSPLIB_qrd_init_cn<float>(DSPLIB_kernelHandle       handle,
                                                 DSPLIB_bufParams2D_t     *bufParamsA,
                                                 DSPLIB_bufParams2D_t     *bufParamsQ,
                                                 DSPLIB_bufParams2D_t     *bufParamsR,
                                                 DSPLIB_bufParams1D_t     *bufParamsU,
                                                 const DSPLIB_qrdInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_qrd_init_cn<double>(DSPLIB_kernelHandle       handle,
                                                  DSPLIB_bufParams2D_t     *bufParamsA,
                                                  DSPLIB_bufParams2D_t     *bufParamsQ,
                                                  DSPLIB_bufParams2D_t     *bufParamsR,
                                                  DSPLIB_bufParams1D_t     *bufParamsU,
                                                  const DSPLIB_qrdInitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_exec_cn(DSPLIB_kernelHandle handle,
                                 const void *restrict pA,
                                 const void *restrict pQ,
                                 const void *restrict pR,
                                 const void *restrict pU,
                                 const void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_qrd_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_PrivArgs *) handle;
   int32_t              nRows        = pKerPrivArgs->heightA;
   int32_t              nCols        = pKerPrivArgs->widthA;
   int32_t              strideA      = pKerPrivArgs->strideA;
   int32_t              strideQ      = pKerPrivArgs->strideQ;
   int32_t              strideR      = pKerPrivArgs->strideR;
   int32_t              dataSize     = sizeof(dataType);

   int32_t colStrideQ = strideQ / dataSize;
   int32_t colStrideR = strideR / dataSize;
   int32_t colStrideA = strideA / dataSize;

   dataType alpha;
   dataType scale;
   dataType sum;
   dataType norm_sqr;

   int32_t row;
   int32_t col;
   int32_t i;
   int32_t k;
   int32_t loopCount;

   /* Typecast void pointers to respective data type */
   dataType *pLocalA = (dataType *) pA;
   dataType *pLocalQ = (dataType *) pQ;
   dataType *pLocalR = (dataType *) pR;
   dataType *pLocalU = (dataType *) pU;

   DSPLIB_DEBUGPRINTFN(0, "pALocal: %p pLocalQ: %p pLocalR: %p pLocalU: %p nCols: %d nRows: %d\n", pLocalA, pLocalQ,
                       pLocalR, pLocalU, nCols, nRows);

   /* ------------------------------------------------------------------- */
   /*  Write each column of 'pALocal' to a row of 'pLocal'.           */
   /* ------------------------------------------------------------------- */

   memcpy(pLocalR, pLocalA, sizeof(dataType) * nRows * colStrideA);
   /* ------------------------------------------------------------------- */
   /* generate identify matrix and copy A to R                            */
   /* ------------------------------------------------------------------- */
   memset(pLocalQ, 0.0, sizeof(dataType) * nRows * colStrideQ);
   for (row = 0; row < nRows; row++) {
      pLocalQ[row + row * colStrideQ] = 1.0;
   }

   if (nRows <= nCols) {
      loopCount = nRows - 2;
   }
   else {
      loopCount = nCols - 1;
   }

   for (col = 0; col <= loopCount; col++) {
      sum = 0;
      for (row = col; row < nRows; row++) {
         sum += pLocalR[col + (row * colStrideR)] * pLocalR[col + (row * colStrideR)];
      }
      if (sum != 0) {
         alpha = sqrt(sum);
         if (pLocalR[col + (col * colStrideR)] >= 0) {
            alpha = -alpha;
         }
         pLocalU[col]                      = pLocalR[col + (col * colStrideR)] + alpha;
         pLocalR[col + (col * colStrideR)] = -alpha;
         norm_sqr                          = pLocalU[col] * pLocalU[col];
         for (row = col + 1; row < nRows; row++) {
            pLocalU[row]                      = pLocalR[col + (row * colStrideR)];
            pLocalR[col + (row * colStrideR)] = 0;
            norm_sqr += pLocalU[row] * pLocalU[row];
         }
         if (alpha * pLocalU[col] != 0.00) {
            scale = 1 / (alpha * pLocalU[col]);
            /* R=Q1*R */
            for (i = col + 1; i < nCols; i++) {
               sum = 0;
               for (k = col; k < nRows; k++) {
                  sum += pLocalU[k] * pLocalR[i + (k * colStrideR)];
               }
               sum *= scale;
               for (k = col; k < nRows; k++) {
                  pLocalR[i + (k * colStrideR)] -= pLocalU[k] * sum;
               }
            }
            /* Q=A*Q1 */
            for (i = 0; i < nRows; i++) {
               sum = 0;
               for (k = col; k < nRows; k++) {
                  sum += pLocalU[k] * pLocalQ[k + (i * colStrideQ)];
               }
               sum *= scale;
               for (k = col; k < nRows; k++) {
                  pLocalQ[k + i * colStrideQ] -= pLocalU[k] * sum;
               }
            }
         } /* if (norm_sqr!=0) */
      }    /* if (sum!=0) */
   }       /* for (col=0;col<=loopCount;col++) */

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}

// explicnt instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_qrd_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                 const void *restrict pA,
                                                 const void *restrict pQ,
                                                 const void *restrict pR,
                                                 const void *restrict pU,
                                                 const void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_qrd_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                  const void *restrict pA,
                                                  const void *restrict pQ,
                                                  const void *restrict pR,
                                                  const void *restrict pU,
                                                  const void *restrict pScratch);
