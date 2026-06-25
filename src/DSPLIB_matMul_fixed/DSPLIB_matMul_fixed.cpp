// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bufParams.h"
#include "DSPLIB_matMul_fixed_priv.h"

int32_t DSPLIB_matMul_fixed_getHandleSize(DSPLIB_matMul_fixed_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_matMul_fixed_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_matMul_fixed_init_checkParams(DSPLIB_kernelHandle                 handle,
                                     const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                     const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                     const DSPLIB_bufParams2D_t         *bufParamsOut,
                                     const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_matMul_fixed_init_checkParams\n");
#endif
   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn0->data_type != DSPLIB_INT8) && (bufParamsIn0->data_type != DSPLIB_INT16)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }

      else if (bufParamsIn0->data_type != bufParamsOut->data_type ||
               bufParamsIn1->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }

   if (status == DSPLIB_SUCCESS) {

      if ((bufParamsIn0->dim_x != bufParamsIn1->dim_y) || // A is m x k, B is k x n, C is m x n
          (bufParamsIn0->dim_y != bufParamsOut->dim_y) || (bufParamsIn1->dim_x != bufParamsOut->dim_x)) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_matMul_fixed_exec_checkParams(DSPLIB_kernelHandle handle,
                                                   const void *restrict pIn0,
                                                   const void *restrict pIn1,
                                                   const void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_matMul_fixed_exec_checkParams\n");
#endif
   if ((pIn0 == NULL) || (pIn1 == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_matMul_fixed_init(DSPLIB_kernelHandle                 handle,
                                       DSPLIB_bufParams2D_t               *bufParamsIn0,
                                       DSPLIB_bufParams2D_t               *bufParamsIn1,
                                       DSPLIB_bufParams2D_t               *bufParamsOut,
                                       const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;

   uint32_t M = bufParamsOut->dim_y;
   uint32_t N = bufParamsOut->dim_x;
   uint32_t K = bufParamsIn0->dim_x;

   pKerPrivArgs->M  = M;
   pKerPrivArgs->N  = N;
   pKerPrivArgs->K  = K;
   pKerPrivArgs->qs = pKerInitArgs->qs;

   uint32_t datatype          = bufParamsIn0->data_type;
   uint32_t strideIn0Elements = bufParamsIn0->stride_y / DSPLIB_sizeof(datatype);
   uint32_t strideIn1Elements = bufParamsIn1->stride_y / DSPLIB_sizeof(datatype);
   uint32_t strideOutElements = bufParamsOut->stride_y / DSPLIB_sizeof(datatype);

   pKerPrivArgs->strideIn0Elements = strideIn0Elements;
   pKerPrivArgs->strideIn1Elements = strideIn1Elements;
   pKerPrivArgs->strideOutElements = strideOutElements;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_matMul_fixed_init\n");
#endif

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn0->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_matMul_fixed_exec_cn<int8_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_matMul_fixed_exec_cn<int16_t>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT  CP 2 status %d\n", status);
#endif
      }
   }
   else {
      if (bufParamsIn0->data_type == DSPLIB_INT8) {
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT bufParamsIn->data_type == DSPLIB_INT8_T\n");
#endif
         pKerPrivArgs->execute = DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>;
         status = DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(handle, bufParamsIn0, bufParamsIn1,
                                                                          bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT16) {
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT bufParamsIn->data_type == DSPLIB_INT16_T\n");
#endif
         pKerPrivArgs->execute = DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>;
         status = DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(handle, bufParamsIn0, bufParamsIn1,
                                                                            bufParamsOut, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  CP 3 status %d\n", status);
#endif
   return status;
}

DSPLIB_STATUS
DSPLIB_matMul_fixed_exec(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_matMul_fixed_exec\n");
#endif

   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn0, pIn1, pOut);

   return status;
}
