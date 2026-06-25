// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_solver_priv.h"

int32_t DSPLIB_qrd_solver_getHandleSize(DSPLIB_qrdSolverInitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_qrd_solver_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS DSPLIB_qrd_solver_init_checkParams(DSPLIB_kernelHandle             handle,
                                                 DSPLIB_bufParams2D_t           *bufParamsQ,
                                                 DSPLIB_bufParams2D_t           *bufParamsR,
                                                 DSPLIB_bufParams1D_t           *bufParamsB,
                                                 DSPLIB_bufParams1D_t           *bufParamsY,
                                                 DSPLIB_bufParams1D_t           *bufParamsX,
                                                 const DSPLIB_qrdSolverInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {

      /* Condition change */
      if (((bufParamsQ->data_type != DSPLIB_FLOAT32) && (bufParamsQ->data_type != DSPLIB_FLOAT64)) ||
          ((bufParamsR->data_type != DSPLIB_FLOAT32) && (bufParamsR->data_type != DSPLIB_FLOAT64))) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsQ->data_type != bufParamsX->data_type || bufParamsR->data_type != bufParamsB->data_type ||
               bufParamsB->data_type != bufParamsX->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsQ->dim_x != bufParamsQ->dim_y || bufParamsR->dim_x != bufParamsR->dim_y) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }

      else {
         /* Do nothing */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_qrd_solver_exec_checkParams(DSPLIB_kernelHandle handle,
                                                 void *restrict pQ,
                                                 void *restrict pR,
                                                 void *restrict pB,
                                                 void *restrict pY,
                                                 void *restrict pX,
                                                 void *restrict pD,
                                                 void *restrict pR1)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   if ((pQ == NULL) || (pR == NULL) || (pB == NULL) || (pY == NULL) || (pX == NULL) || (pD == NULL) || (pR1 == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_qrd_solver_init(DSPLIB_kernelHandle             handle,
                                     DSPLIB_bufParams2D_t           *bufParamsQ,
                                     DSPLIB_bufParams2D_t           *bufParamsR,
                                     DSPLIB_bufParams1D_t           *bufParamsB,
                                     DSPLIB_bufParams1D_t           *bufParamsY,
                                     DSPLIB_bufParams1D_t           *bufParamsX,
                                     const DSPLIB_qrdSolverInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS               status       = DSPLIB_SUCCESS;
   DSPLIB_qrd_solver_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_solver_PrivArgs *) handle;

   pKerPrivArgs->widthR  = bufParamsR->dim_x;
   pKerPrivArgs->heightR = bufParamsR->dim_y;
   pKerPrivArgs->strideQ = bufParamsQ->stride_y;
   pKerPrivArgs->strideR = bufParamsR->stride_y;

   DSPLIB_DEBUGPRINTFN(0, "pKerInitArgs->funcStyle: %d bufParamsX->data_type: %d\n", pKerInitArgs->funcStyle,
                       bufParamsX->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if ((bufParamsQ->data_type == DSPLIB_FLOAT32) && (bufParamsR->data_type == DSPLIB_FLOAT32) &&
          (bufParamsX->data_type == DSPLIB_FLOAT32)) {
         pKerPrivArgs->execute = DSPLIB_qrd_solver_exec_cn<float>;
         status = DSPLIB_qrd_solver_init_cn<float>(handle, bufParamsQ, bufParamsR, bufParamsB, bufParamsY, bufParamsX,
                                                   pKerInitArgs);
      }
      else if ((bufParamsQ->data_type == DSPLIB_FLOAT64) && (bufParamsR->data_type == DSPLIB_FLOAT64) &&
               (bufParamsX->data_type == DSPLIB_FLOAT64)) {
         pKerPrivArgs->execute = DSPLIB_qrd_solver_exec_cn<double>;
         status = DSPLIB_qrd_solver_init_cn<double>(handle, bufParamsQ, bufParamsR, bufParamsB, bufParamsY, bufParamsX,
                                                    pKerInitArgs);
      }

      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if ((bufParamsQ->data_type == DSPLIB_FLOAT32) && (bufParamsR->data_type == DSPLIB_FLOAT32) &&
          (bufParamsX->data_type == DSPLIB_FLOAT32)) {
         pKerPrivArgs->execute = DSPLIB_qrd_solver_exec_ci<float>;
         status = DSPLIB_qrd_solver_init_ci<float>(handle, bufParamsQ, bufParamsR, bufParamsB, bufParamsY, bufParamsX,
                                                   pKerInitArgs);
      }
      else if ((bufParamsQ->data_type == DSPLIB_FLOAT64) && (bufParamsR->data_type == DSPLIB_FLOAT64) &&
               (bufParamsX->data_type == DSPLIB_FLOAT64)) {
         pKerPrivArgs->execute = DSPLIB_qrd_solver_exec_ci<double>;
         status = DSPLIB_qrd_solver_init_ci<double>(handle, bufParamsQ, bufParamsR, bufParamsB, bufParamsY, bufParamsX,
                                                    pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_qrd_solver_exec(DSPLIB_kernelHandle handle,
                                     void *restrict pQ,
                                     void *restrict pR,
                                     void *restrict pB,
                                     void *restrict pY,
                                     void *restrict pX,
                                     void *restrict pD,
                                     void *restrict pR1)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   DSPLIB_qrd_solver_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_solver_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "widthR: %d heightR: %d strideQ: %d strideR: %d\n", pKerPrivArgs->widthR,
                       pKerPrivArgs->heightR, pKerPrivArgs->strideQ, pKerPrivArgs->strideR);

   status = pKerPrivArgs->execute(handle, pQ, pR, pB, pY, pX, pD, pR1);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
