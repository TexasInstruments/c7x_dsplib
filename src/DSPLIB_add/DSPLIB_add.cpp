// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_add_priv.h"

int32_t DSPLIB_add_getHandleSize(DSPLIB_add_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_add_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_add_init_checkParams(DSPLIB_kernelHandle         handle,
                            const DSPLIB_bufParams1D_t *bufParamsIn,
                            const DSPLIB_bufParams1D_t *bufParamsOut,
                            const DSPLIB_add_InitArgs  *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_add_init_checkParams\n");
#endif
   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn->data_type != DSPLIB_INT16) && (bufParamsIn->data_type != DSPLIB_INT32) &&
          (bufParamsIn->data_type != DSPLIB_INT8) && (bufParamsIn->data_type != DSPLIB_FLOAT32) &&
          (bufParamsIn->data_type != DSPLIB_UINT8) && (bufParamsIn->data_type != DSPLIB_UINT16) &&
          (bufParamsIn->data_type != DSPLIB_UINT32) && (bufParamsIn->data_type != DSPLIB_FLOAT64)) {
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

DSPLIB_STATUS DSPLIB_add_exec_checkParams(DSPLIB_kernelHandle handle,
                                          const void *restrict pIn1,
                                          const void *restrict pIn2,
                                          const void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_add_exec_checkParams\n");
#endif
   if ((pIn1 == NULL) || (pIn2 == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_add_init(DSPLIB_kernelHandle        handle,
                              DSPLIB_bufParams1D_t      *bufParamsIn,
                              DSPLIB_bufParams1D_t      *bufParamsOut,
                              const DSPLIB_add_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS        status       = DSPLIB_SUCCESS;
   DSPLIB_add_PrivArgs *pKerPrivArgs = (DSPLIB_add_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_add_init\n");
#endif
   pKerPrivArgs->blockSize = pKerInitArgs->dataSize;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_add_init pKerPrivArgs->blockSize %d "
          "bufParamsIn->data_type %d\n",
          pKerPrivArgs->blockSize, bufParamsIn->data_type);
#endif

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_add_exec_cn<float>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_add_exec_cn<double>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_add_exec_cn<int8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_add_exec_cn<uint8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_add_exec_cn<int16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_add_exec_cn<uint16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_add_exec_cn<int32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_add_exec_cn<uint32_t>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT  CP 2 status %d\n", status);
#endif
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT bufParamsIn->data_type == DSPLIB_FLOAT32\n");
#endif
         pKerPrivArgs->execute = DSPLIB_add_exec_ci<float>;
         status                = DSPLIB_add_init_ci<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_add_exec_ci<double>;
         status                = DSPLIB_add_init_ci<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_add_exec_ci<int8_t>;
         status                = DSPLIB_add_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_add_exec_ci<uint8_t>;
         status                = DSPLIB_add_init_ci<uint8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_add_exec_ci<int16_t>;
         status                = DSPLIB_add_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_add_exec_ci<uint16_t>;
         status                = DSPLIB_add_init_ci<uint16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_add_exec_ci<int32_t>;
         status                = DSPLIB_add_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_add_exec_ci<uint32_t>;
         status                = DSPLIB_add_init_ci<uint32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
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

DSPLIB_STATUS DSPLIB_add_exec(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_add_exec\n");
#endif

   DSPLIB_add_PrivArgs *pKerPrivArgs = (DSPLIB_add_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT pKerPrivArgs->blockSize %d\n", pKerPrivArgs->blockSize);
#endif

   status = pKerPrivArgs->execute(handle, pIn1, pIn2, pOut);

   return status;
}
