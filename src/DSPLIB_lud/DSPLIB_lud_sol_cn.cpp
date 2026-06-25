// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_lud_sol_priv.h"
#include <cstdint>

/**********************************************************************/
/* INITIALIZATION                                                     */
/**********************************************************************/

DSPLIB_STATUS DSPLIB_lud_sol_init_cn(DSPLIB_kernelHandle           handle,
                                     const DSPLIB_bufParams2D_t   *bufParamsP,
                                     const DSPLIB_bufParams2D_t   *bufParamsL,
                                     const DSPLIB_bufParams2D_t   *bufParamsU,
                                     const DSPLIB_bufParams1D_t   *bufParamsB,
                                     const DSPLIB_bufParams1D_t   *bufParamsX,
                                     const DSPLIB_bufParams2D_t   *bufParamsVecScratch,
                                     const DSPLIB_bufParams2D_t   *bufParamsScratchTrans,
                                     const DSPLIB_lud_solInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*  NATURAL C IMPLEMENTATION                                          */
/**********************************************************************/

template <typename dataType>
int DSPLIB_lud_sol_cn(int32_t         order,
                      unsigned short *P,
                      dataType       *L,
                      dataType       *U,
                      dataType       *B,
                      dataType       *B_Mod,
                      dataType       *Y,
                      dataType       *X,
                      const int32_t   strideOrder,
                      const int32_t   strideP)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering nat C c7x implementation");
   int      row, col;
   dataType sum;

   int32_t dataSize  = sizeof(dataType);
   int32_t dataSizeP = sizeof(unsigned short);

   int32_t orderStride  = strideOrder / dataSize;
   int32_t orderPStride = strideP / dataSizeP;

   /* modify b based on permutation matrix P */
   for (row = 0; row < order; row++) {
      sum = 0;
      for (col = 0; col < order; col++) {
         sum += P[col + row * orderPStride] * B[col];
      }
      B_Mod[row] = sum;
   }

   /* solve L*y=b for y using forward substitution */
   for (row = 0; row < order; row++) {
      if (row == 0) {
         Y[row] = B_Mod[row] / L[0];
      }
      else {
         sum = 0.0;
         for (col = 0; col <= row - 1; col++) {
            sum += L[row * orderStride + col] * Y[col];
         }
         Y[row] = (B_Mod[row] - sum) / L[row * orderStride + row];
      }
   }

   /* solve U*x=y for x using backward substitution */
   for (row = order - 1; row >= 0; row--) {
      if (row == order - 1) {
         X[row] = Y[row] / U[row * orderStride + row];
      }
      else {
         sum = 0.0;
         for (col = order - 1; col >= row + 1; col--) {
            sum += U[row * orderStride + col] * X[col];
         }
         X[row] = (Y[row] - sum) / U[row * orderStride + row];
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting nat C c7x implementation");
   return 0;
}

template int DSPLIB_lud_sol_cn<float>(int32_t         order,
                                      unsigned short *P,
                                      float          *L,
                                      float          *U,
                                      float          *B,
                                      float          *B_Mod,
                                      float          *Y,
                                      float          *X,
                                      const int32_t   strideOrder,
                                      const int32_t   strideP);
template int DSPLIB_lud_sol_cn<double>(int32_t         order,
                                       unsigned short *P,
                                       double         *L,
                                       double         *U,
                                       double         *B,
                                       double         *B_Mod,
                                       double         *Y,
                                       double         *X,
                                       const int32_t   strideOrder,
                                       const int32_t   strideP);

template <typename dataType>
DSPLIB_STATUS DSPLIB_lud_sol_exec_cn(DSPLIB_kernelHandle handle,
                                     void *restrict pP,
                                     void *restrict pL,
                                     void *restrict pU,
                                     void *restrict pB,
                                     void *restrict pX,
                                     void *restrict pVecScratch,
                                     void *restrict pScratchTrans)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering exec_cn function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_lud_sol_PrivArgs *pKerPrivArgs = (DSPLIB_lud_sol_PrivArgs *) handle;

   int32_t order       = pKerPrivArgs->order;
   int32_t strideOrder = pKerPrivArgs->strideOrder;
   int32_t strideP     = pKerPrivArgs->strideP;
   int32_t strideVec   = pKerPrivArgs->strideVec;

   /* Typecast void pointers to respective data type */
   unsigned short *pPLocal     = (unsigned short *) pP;
   dataType       *pLLocal     = (dataType *) pL;
   dataType       *pULocal     = (dataType *) pU;
   dataType       *pBLocal     = (dataType *) pB;
   dataType       *pB_ModLocal = (dataType *) pVecScratch;
   dataType       *pYLocal     = (dataType *) (pVecScratch) + (strideVec / sizeof(dataType));
   dataType       *pXLocal     = (dataType *) pX;

   DSPLIB_DEBUGPRINTFN(0,
                       "pPLocal: %p pLLocal: %p pULocal: %p pBLocal: %p  pB_ModLocal: %p  pYLocal: %p  pXLocal: %p\n",
                       pPLocal, pLLocal, pULocal, pBLocal, pB_ModLocal, pYLocal, pXLocal);

   DSPLIB_lud_sol_cn<dataType>(order, pPLocal, pLLocal, pULocal, pBLocal, pB_ModLocal, pYLocal, pXLocal, strideOrder,
                               strideP);

   DSPLIB_DEBUGPRINTFN(0, "Exiting exec_cn function with return status: %d\n", status);

   return (status);
}

template DSPLIB_STATUS DSPLIB_lud_sol_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                     void *restrict pP,
                                                     void *restrict pL,
                                                     void *restrict pU,
                                                     void *restrict pB,
                                                     void *restrict pX,
                                                     void *restrict pVecScratch,
                                                     void *restrict pScratchTrans);

template DSPLIB_STATUS DSPLIB_lud_sol_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                      void *restrict pP,
                                                      void *restrict pL,
                                                      void *restrict pU,
                                                      void *restrict pB,
                                                      void *restrict pX,
                                                      void *restrict pVecScratch,
                                                      void *restrict pScratchTrans);
