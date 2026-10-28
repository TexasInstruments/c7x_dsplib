// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bitwiseANDConst_priv.h"
#include "DSPLIB_bufParams.h"

int32_t DSPLIB_bitwiseANDConst_getHandleSize(DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_bitwiseANDConst_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_bitwiseANDConst_init_checkParams(DSPLIB_kernelHandle                    handle,
                                        const DSPLIB_bufParams2D_t            *bufParamsIn0,
                                        const DSPLIB_bufParams1D_t            *bufParamsIn1,
                                        const DSPLIB_bufParams2D_t            *bufParamsOut,
                                        const DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_bitwiseANDConst_init_checkParams \n");

   if ((handle == NULL) || (bufParamsIn0 == NULL) || (bufParamsIn1 == NULL) || (bufParamsOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn0->data_type != DSPLIB_UINT32 || bufParamsIn1->data_type != DSPLIB_UINT32) &&
          (bufParamsIn0->data_type != DSPLIB_UINT16 || bufParamsIn1->data_type != DSPLIB_UINT16) &&
          (bufParamsIn0->data_type != DSPLIB_UINT64 || bufParamsIn1->data_type != DSPLIB_UINT64) &&
          (bufParamsIn0->data_type != DSPLIB_UINT8 || bufParamsIn1->data_type != DSPLIB_UINT8)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn0->data_type != DSPLIB_UINT32 || bufParamsOut->data_type != DSPLIB_UINT32) &&
          (bufParamsIn0->data_type != DSPLIB_UINT16 || bufParamsOut->data_type != DSPLIB_UINT16) &&
          (bufParamsIn0->data_type != DSPLIB_UINT64 || bufParamsOut->data_type != DSPLIB_UINT64) &&
          (bufParamsIn0->data_type != DSPLIB_UINT8 || bufParamsOut->data_type != DSPLIB_UINT8)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   /* The constant is a single element */
   if (status == DSPLIB_SUCCESS) {
      if (bufParamsIn1->dim_x != 1u) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
   }

   /* The output buffer must match the input buffer dimensions */
   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsOut->dim_x != bufParamsIn0->dim_x) || (bufParamsOut->dim_y != bufParamsIn0->dim_y)) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_checkParams(DSPLIB_kernelHandle handle,
                                                      const void *restrict pIn0,
                                                      const void *restrict pIn1,
                                                      const void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_bitwiseANDConst_exec_checkParams \n");

   if ((handle == NULL) || (pIn0 == NULL) || (pIn1 == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_bitwiseANDConst_init(DSPLIB_kernelHandle                    handle,
                                          DSPLIB_bufParams2D_t                  *bufParamsIn0,
                                          DSPLIB_bufParams1D_t                  *bufParamsIn1,
                                          DSPLIB_bufParams2D_t                  *bufParamsOut,
                                          const DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                    status       = DSPLIB_SUCCESS;
   DSPLIB_bitwiseANDConst_PrivArgs *pKerPrivArgs = (DSPLIB_bitwiseANDConst_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_bitwiseANDConst_init \n");

   pKerPrivArgs->dim_x     = bufParamsIn0->dim_x;
   pKerPrivArgs->dim_y     = bufParamsIn0->dim_y;
   pKerPrivArgs->strideIn  = bufParamsIn0->stride_y / DSPLIB_sizeof(bufParamsIn0->data_type);
   pKerPrivArgs->strideOut = bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsOut->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn0->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_bitwiseANDConst_exec_cn<uint8_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_bitwiseANDConst_exec_cn<uint16_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_bitwiseANDConst_exec_cn<uint32_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_bitwiseANDConst_exec_cn<uint64_t>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsIn0->data_type == DSPLIB_UINT8) {
         status =
             DSPLIB_bitwiseANDConst_init_ci<uint8_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_bitwiseANDConst_exec_ci<uint8_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT16) {
         status =
             DSPLIB_bitwiseANDConst_init_ci<uint16_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_bitwiseANDConst_exec_ci<uint16_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT32) {
         status =
             DSPLIB_bitwiseANDConst_init_ci<uint32_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);

         pKerPrivArgs->execute = DSPLIB_bitwiseANDConst_exec_ci<uint32_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT64) {
         status =
             DSPLIB_bitwiseANDConst_init_ci<uint64_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_bitwiseANDConst_exec_ci<uint64_t>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_bitwiseANDConst_exec(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_bitwiseANDConst_exec \n");

   DSPLIB_bitwiseANDConst_PrivArgs *pKerPrivArgs = (DSPLIB_bitwiseANDConst_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn0, pIn1, pOut);

   return status;
}
