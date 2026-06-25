// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_fir_priv.h"

int32_t DSPLIB_fir_getHandleSize(DSPLIB_fir_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_fir_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_fir_init_checkParams(DSPLIB_kernelHandle         handle,
                            const DSPLIB_bufParams2D_t *bufParamsIn,
                            const DSPLIB_bufParams2D_t *bufParamsFilter,
                            const DSPLIB_bufParams2D_t *bufParamsOut,
                            const DSPLIB_fir_InitArgs  *pKerInitArgs)
{
   DSPLIB_STATUS        status = DSPLIB_SUCCESS;
   DSPLIB_bufParams2D_t bufParamsFilterLocal;
   DSPLIB_bufParams2D_t bufParamsInLocal, bufParamsOutLocal;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_fir_init_checkParams\n");
#endif
   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_INT16) {
         status =
             DSPLIB_fir_getSizes<int16_t>(pKerInitArgs, &bufParamsInLocal, &bufParamsFilterLocal, &bufParamsOutLocal);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         status =
             DSPLIB_fir_getSizes<int32_t>(pKerInitArgs, &bufParamsInLocal, &bufParamsFilterLocal, &bufParamsOutLocal);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {

         status =
             DSPLIB_fir_getSizes<int8_t>(pKerInitArgs, &bufParamsInLocal, &bufParamsFilterLocal, &bufParamsOutLocal);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         status =
             DSPLIB_fir_getSizes<float>(pKerInitArgs, &bufParamsInLocal, &bufParamsFilterLocal, &bufParamsOutLocal);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   if (status == DSPLIB_SUCCESS) {
      if (bufParamsIn->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsFilter->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != DSPLIB_FLOAT32) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_fir_getSizes(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                  DSPLIB_bufParams2D_t      *bufParamsIn,
                                  DSPLIB_bufParams2D_t      *bufParamsFilter,
                                  DSPLIB_bufParams2D_t      *bufParamsOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_fir_getSizes\n");
#endif
   if (bufParamsIn->data_type == DSPLIB_INT16) {
      status = DSPLIB_fir_getSizes<int16_t>(pKerInitArgs, bufParamsIn, bufParamsFilter, bufParamsOut);
   }
   else if (bufParamsIn->data_type == DSPLIB_INT32) {
      status = DSPLIB_fir_getSizes<int32_t>(pKerInitArgs, bufParamsIn, bufParamsFilter, bufParamsOut);
   }
   else if (bufParamsIn->data_type == DSPLIB_INT8) {
      status = DSPLIB_fir_getSizes<int8_t>(pKerInitArgs, bufParamsIn, bufParamsFilter, bufParamsOut);
   }
   else if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
      status = DSPLIB_fir_getSizes<float>(pKerInitArgs, bufParamsIn, bufParamsFilter, bufParamsOut);
   }
   else {
      status = DSPLIB_ERR_NOT_IMPLEMENTED;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_fir_exec_checkParams(DSPLIB_kernelHandle handle,
                                          const void *restrict pIn,
                                          const void *restrict pFilter,
                                          const void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_fir_exec_checkParams\n");
#endif
   if ((pIn == NULL) || (pFilter == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_fir_init(DSPLIB_kernelHandle   handle,
                              DSPLIB_bufParams2D_t *bufParamsIn,
                              DSPLIB_bufParams2D_t *bufParamsFilter,
                              DSPLIB_bufParams2D_t *bufParamsOut,
                              DSPLIB_fir_InitArgs  *pKerInitArgs)
{
   DSPLIB_STATUS        status;
   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_fir_init\n");
#endif
   pKerPrivArgs->initArgs = *pKerInitArgs;

   int32_t ele_size = 0;

   if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
      ele_size = 4;
   }
   else if (bufParamsIn->data_type == DSPLIB_INT32 || bufParamsIn->data_type == DSPLIB_UINT32) {
      ele_size = 4;
   }
   else if (bufParamsIn->data_type == DSPLIB_INT16 || bufParamsIn->data_type == DSPLIB_UINT16) {
      ele_size = 2;
   }
   else if (bufParamsIn->data_type == DSPLIB_INT8 || bufParamsIn->data_type == DSPLIB_UINT8) {
      ele_size = 1;
   }
   else {
      status = DSPLIB_ERR_INVALID_TYPE;
   }

   if (ele_size != 0) {
      pKerPrivArgs->dataBufferInPitch  = bufParamsIn->stride_y / ele_size;
      pKerPrivArgs->dataBufferOutPitch = bufParamsOut->stride_y / ele_size;
      pKerPrivArgs->outputSize         = bufParamsOut->dim_x;
      pKerPrivArgs->filterPitch        = bufParamsFilter->stride_y / ele_size;
   }

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      status = DSPLIB_fir_init_cn(handle, bufParamsIn, bufParamsFilter, bufParamsOut, pKerInitArgs);
      if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_fir_exec_cn<int16_t, int64_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_fir_exec_cn<int32_t, DSPLIB_int128_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_fir_exec_cn<int8_t, int32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_fir_exec_cn<float, int32_t>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_INT16) {
         status = DSPLIB_fir_init_ci<int16_t>(handle, bufParamsIn, bufParamsFilter, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         status = DSPLIB_fir_init_ci<int32_t>(handle, bufParamsIn, bufParamsFilter, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         status = DSPLIB_fir_init_ci<int8_t>(handle, bufParamsIn, bufParamsFilter, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT bufParamsIn->data_type == DSPLIB_FLOAT32\n");
#endif
         status = DSPLIB_fir_init_ci<float>(handle, bufParamsIn, bufParamsFilter, bufParamsOut, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_fir_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_fir_exec\n");
#endif

   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pFilter, pOut);

   return status;
}
