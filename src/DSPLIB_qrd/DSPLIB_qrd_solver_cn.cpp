// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_solver_priv.h"
#include <cstring>
template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_solver_init_cn(DSPLIB_kernelHandle             handle,
                                        DSPLIB_bufParams2D_t           *bufParamsQ,
                                        DSPLIB_bufParams2D_t           *bufParamsR,
                                        DSPLIB_bufParams1D_t           *bufParamsB,
                                        DSPLIB_bufParams1D_t           *bufParamsY,
                                        DSPLIB_bufParams1D_t           *bufParamsX,
                                        const DSPLIB_qrdSolverInitArgs *pKerInitArgs)
{

   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}
template DSPLIB_STATUS DSPLIB_qrd_solver_init_cn<float>(DSPLIB_kernelHandle             handle,
                                                        DSPLIB_bufParams2D_t           *bufParamsQ,
                                                        DSPLIB_bufParams2D_t           *bufParamsR,
                                                        DSPLIB_bufParams1D_t           *bufParamsB,
                                                        DSPLIB_bufParams1D_t           *bufParamsY,
                                                        DSPLIB_bufParams1D_t           *bufParamsX,
                                                        const DSPLIB_qrdSolverInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_qrd_solver_init_cn<double>(DSPLIB_kernelHandle             handle,
                                                         DSPLIB_bufParams2D_t           *bufParamsQ,
                                                         DSPLIB_bufParams2D_t           *bufParamsR,
                                                         DSPLIB_bufParams1D_t           *bufParamsB,
                                                         DSPLIB_bufParams1D_t           *bufParamsY,
                                                         DSPLIB_bufParams1D_t           *bufParamsX,
                                                         const DSPLIB_qrdSolverInitArgs *pKerInitArgs);

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_solver_exec_cn(DSPLIB_kernelHandle handle,
                                        void *restrict pQ,
                                        void *restrict pR,
                                        void *restrict pB,
                                        void *restrict pY,
                                        void *restrict pX,
                                        void *restrict pD,
                                        void *restrict pR1)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_qrd_solver_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_solver_PrivArgs *) handle;
   int32_t                     heightR      = pKerPrivArgs->heightR;
   int32_t                     widthR       = pKerPrivArgs->widthR;
   int32_t                     strideQ      = pKerPrivArgs->strideQ;
   int32_t                     strideR      = pKerPrivArgs->strideR;
   int32_t                     dataSize     = sizeof(dataType);

   /* Typecast void pointers to respective data type */
   dataType *pLocalQ = (dataType *) pQ;
   dataType *pLocalR = (dataType *) pR;
   dataType *pLocalB = (dataType *) pB;
   dataType *pLocalY = (dataType *) pY;
   dataType *pLocalX = (dataType *) pX;

   int32_t colRstride = strideR / dataSize;
   int32_t colQstride = strideQ / dataSize;
   DSPLIB_DEBUGPRINTFN(0, "pLocalQ: %p pLocalR: %p pLocalB: %p pLocalY: %p pLocalX: %p  widthR: %d heightR: %d\n",
                       pLocalQ, pLocalR, pLocalB, pLocalY, pLocalX, widthR, heightR);

   /* ------------------------------------------------------------------- */
   /*  Write each column of 'pLocal' to a row of 'pLocalX'.           */
   /* ------------------------------------------------------------------- */

   int32_t  row;
   int32_t  col;
   int32_t  loopCnt;
   dataType sum;

   /* generate y=Q'*b */
   for (row = 0; row < heightR; row++) {
      sum = 0.0;
      for (col = 0; col < heightR; col++) {
         sum += pLocalQ[row + col * colQstride] * pLocalB[col];
      }
      pLocalY[row] = sum;
   }

   /* use backward substitution to solve x=inv(R)*y */
   if (heightR >= widthR) {
      loopCnt = widthR;
   }
   else {
      loopCnt = heightR;
   }
   memset(pLocalX, 0, sizeof(dataType) * widthR);
   for (row = loopCnt - 1; row >= 0; row--) {
      sum = 0.0;
      for (col = row + 1; col < loopCnt; col++) {
         sum += pLocalR[col + row * colRstride] * pLocalX[col];
      }
      pLocalX[row] = (pLocalY[row] - sum) / pLocalR[row + row * colRstride];
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_qrd_solver_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                        void *restrict pQ,
                                                        void *restrict pR,
                                                        void *restrict pB,
                                                        void *restrict pY,
                                                        void *restrict pX,
                                                        void *restrict pD,
                                                        void *restrict pR1);

template DSPLIB_STATUS DSPLIB_qrd_solver_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                         void *restrict pQ,
                                                         void *restrict pR,
                                                         void *restrict pB,
                                                         void *restrict pY,
                                                         void *restrict pX,
                                                         void *restrict pD,
                                                         void *restrict pR1);
