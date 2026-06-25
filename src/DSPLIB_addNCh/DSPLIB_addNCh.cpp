// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_addNCh_priv.h"

int32_t DSPLIB_addNCh_getHandleSize(DSPLIB_addNCh_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_addNCh_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_addNCh_init_checkParams(DSPLIB_kernelHandle           handle,
                               const DSPLIB_bufParams3D_t   *bufParamsIn,
                               const DSPLIB_bufParams2D_t   *bufParamsOut,
                               const DSPLIB_addNCh_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_addNCh_init_checkParams\n", status);

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {

      if ((bufParamsIn->data_type != DSPLIB_FLOAT32) && (bufParamsIn->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_addNCh_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_addNCh_exec_checkParams\n", status);

   if ((pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_addNCh_init(DSPLIB_kernelHandle           handle,
                                 DSPLIB_bufParams3D_t         *bufParamsIn,
                                 DSPLIB_bufParams2D_t         *bufParamsOut,
                                 const DSPLIB_addNCh_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS           status       = DSPLIB_SUCCESS;
   DSPLIB_addNCh_PrivArgs *pKerPrivArgs = (DSPLIB_addNCh_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT Enter DSPLIB_addNCh_init\n", status);

   pKerPrivArgs->samples           = bufParamsIn->dim_x;
   pKerPrivArgs->channels          = bufParamsIn->dim_y;
   pKerPrivArgs->inputs            = bufParamsIn->dim_z;
   pKerPrivArgs->strideInElementsY = bufParamsIn->stride_y / DSPLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideInElementsZ = bufParamsIn->stride_z / DSPLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsIn->data_type);

   DSPLIB_DEBUGPRINTFN(0,
                       "DSPLIB_DEBUGPRINT DSPLIB_addNCh_init pKerPrivArgs->samples %d "
                       "bufParamsIn->dim_x %d\n",
                       pKerPrivArgs->samples, bufParamsIn->dim_x);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_addNCh_exec_cn<float>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_addNCh_exec_cn<double>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         status = DSPLIB_addNCh_init_ci<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);

         if (pKerPrivArgs->inputs == 1) {
            pKerPrivArgs->execute = DSPLIB_addNCh_1Inputs_exec_ci<float>;
         }
         else if (pKerPrivArgs->inputs == 2) {
            pKerPrivArgs->execute = DSPLIB_addNCh_2Inputs_exec_ci<float>;
         }
         else if (pKerPrivArgs->inputs == 4) {
            pKerPrivArgs->execute = DSPLIB_addNCh_4Inputs_exec_ci<float>;
         }
         else {
            pKerPrivArgs->execute = DSPLIB_addNCh_exec_ci<float>;
         }
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         status = DSPLIB_addNCh_init_ci<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);

         if (pKerPrivArgs->inputs == 1) {
            pKerPrivArgs->execute = DSPLIB_addNCh_1Inputs_exec_ci<double>;
         }
         else if (pKerPrivArgs->inputs == 2) {
            pKerPrivArgs->execute = DSPLIB_addNCh_2Inputs_exec_ci<double>;
         }
         else if (pKerPrivArgs->inputs == 4) {
            pKerPrivArgs->execute = DSPLIB_addNCh_4Inputs_exec_ci<double>;
         }
         else {

            pKerPrivArgs->execute = DSPLIB_addNCh_exec_ci<double>;
         }
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_addNCh_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT Enter DSPLIB_addNCh_exec\n", status);

   DSPLIB_addNCh_PrivArgs *pKerPrivArgs = (DSPLIB_addNCh_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut);

   return status;
}
