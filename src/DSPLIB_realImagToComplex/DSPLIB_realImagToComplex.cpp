// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bufParams.h"
#include "DSPLIB_realImagToComplex_priv.h"

int32_t DSPLIB_realImagToComplex_getHandleSize(DSPLIB_realImagToComplex_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_realImagToComplex_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_realImagToComplex_init_checkParams(DSPLIB_kernelHandle                      handle,
                                          const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                          const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                          const DSPLIB_bufParams2D_t              *bufParamsOut,
                                          const DSPLIB_realImagToComplex_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_realImagToComplex_init_checkParams \n");

   if ((handle == NULL) || (bufParamsIn0 == NULL) || (bufParamsIn1 == NULL) || (bufParamsOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn0->data_type != DSPLIB_FLOAT32 || bufParamsIn1->data_type != DSPLIB_FLOAT32) &&
          (bufParamsIn0->data_type != DSPLIB_FLOAT64 || bufParamsIn1->data_type != DSPLIB_FLOAT64) &&
          (bufParamsIn0->data_type != DSPLIB_INT32 || bufParamsIn1->data_type != DSPLIB_INT32) &&
          (bufParamsIn0->data_type != DSPLIB_UINT32 || bufParamsIn1->data_type != DSPLIB_UINT32) &&
          (bufParamsIn0->data_type != DSPLIB_INT16 || bufParamsIn1->data_type != DSPLIB_INT16) &&
          (bufParamsIn0->data_type != DSPLIB_UINT16 || bufParamsIn1->data_type != DSPLIB_UINT16)) {
         {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
      else {
         /* Nothing to do here */
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_realImagToComplex_exec_checkParams(DSPLIB_kernelHandle handle,
                                                        const void *restrict pIn1,
                                                        const void *restrict pIn0,
                                                        const void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_realImagToComplex_exec_checkParams \n");

   if ((pIn1 == NULL) || (pIn0 == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_realImagToComplex_init(DSPLIB_kernelHandle                      handle,
                                            DSPLIB_bufParams2D_t                    *bufParamsIn0,
                                            DSPLIB_bufParams2D_t                    *bufParamsIn1,
                                            DSPLIB_bufParams2D_t                    *bufParamsOut,
                                            const DSPLIB_realImagToComplex_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_realImagToComplex_PrivArgs *pKerPrivArgs = (DSPLIB_realImagToComplex_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_realImagToComplex_init \n");

   pKerPrivArgs->dim_x     = bufParamsIn0->dim_x;
   pKerPrivArgs->dim_y     = bufParamsIn0->dim_y;
   pKerPrivArgs->strideIn  = bufParamsIn0->stride_y / DSPLIB_sizeof(bufParamsIn0->data_type);
   pKerPrivArgs->strideOut = bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsIn0->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn0->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_cn<float>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_cn<double>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_cn<int32_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_cn<uint32_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_cn<int16_t>;
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_cn<uint16_t>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsIn0->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_ci<float>;
         status =
             DSPLIB_realImagToComplex_init_ci<float>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_ci<double>;
         status =
             DSPLIB_realImagToComplex_init_ci<double>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_ci<int32_t>;
         status =
             DSPLIB_realImagToComplex_init_ci<int32_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_ci<uint32_t>;
         status =
             DSPLIB_realImagToComplex_init_ci<uint32_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_ci<int16_t>;
         status =
             DSPLIB_realImagToComplex_init_ci<int16_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_realImagToComplex_exec_ci<uint16_t>;
         status =
             DSPLIB_realImagToComplex_init_ci<uint16_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }

      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_realImagToComplex_exec(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_realImagToComplex_exec \n");

   DSPLIB_realImagToComplex_PrivArgs *pKerPrivArgs = (DSPLIB_realImagToComplex_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn1, pIn0, pOut);

   return status;
}
