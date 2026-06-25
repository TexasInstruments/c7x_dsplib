// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_negate_priv.h"

int32_t DSPLIB_negate_getHandleSize(DSPLIB_negate_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_negate_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_negate_init_checkParams(DSPLIB_kernelHandle           handle,
                               const DSPLIB_bufParams1D_t   *bufParamsIn,
                               const DSPLIB_bufParams1D_t   *bufParamsOut,
                               const DSPLIB_negate_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_negate_init_checkParams\n");
#endif
   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn->data_type != DSPLIB_INT32) && (bufParamsIn->data_type != DSPLIB_INT16) &&
          (bufParamsIn->data_type != DSPLIB_INT8)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }
   else {
      /* Nothing to do here */
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_negate_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_negate_exec_checkParams\n");
#endif
   if ((pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_negate_init(DSPLIB_kernelHandle           handle,
                                 DSPLIB_bufParams1D_t         *bufParamsIn,
                                 DSPLIB_bufParams1D_t         *bufParamsOut,
                                 const DSPLIB_negate_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS           status       = DSPLIB_SUCCESS;
   DSPLIB_negate_PrivArgs *pKerPrivArgs = (DSPLIB_negate_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_negate_init\n");
#endif
   pKerPrivArgs->blockSize = pKerInitArgs->dataSize;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_negate_init pKerPrivArgs->blockSize %d "
          "bufParamsIn->data_type %d\n",
          pKerPrivArgs->blockSize, bufParamsIn->data_type);
#endif

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_negate_exec_cn<int32_t, DSPLIB_INT32>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_negate_exec_cn<int16_t, DSPLIB_INT16>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_negate_exec_cn<int8_t, DSPLIB_INT8>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT  CP 2 status %d\n", status);
#endif
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_INT32) {
         status                = DSPLIB_negate_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = DSPLIB_negate_exec_ci<int32_t, DSPLIB_INT32>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         status = DSPLIB_negate_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);

         pKerPrivArgs->execute = DSPLIB_negate_exec_ci<int16_t, DSPLIB_INT16>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         status = DSPLIB_negate_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);

         pKerPrivArgs->execute = DSPLIB_negate_exec_ci<int8_t, DSPLIB_INT8>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT  CP 2 status %d\n", status);
#endif
      }
   }
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  CP 3 status %d\n", status);
#endif
   return status;
}

DSPLIB_STATUS DSPLIB_negate_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_negate_exec\n");
#endif

   DSPLIB_negate_PrivArgs *pKerPrivArgs = (DSPLIB_negate_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT pKerPrivArgs->blockSize %d\n", pKerPrivArgs->blockSize);
#endif
   status = pKerPrivArgs->execute(handle, pIn, pOut);

   return status;
}
