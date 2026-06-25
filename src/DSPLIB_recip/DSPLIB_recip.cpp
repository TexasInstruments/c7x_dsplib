// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_recip_priv.h"

int32_t DSPLIB_recip_getHandleSize(DSPLIB_recip_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_recip_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_recip_init_checkParams(DSPLIB_kernelHandle          handle,
                                            const DSPLIB_bufParams1D_t  *bufParamsIn,
                                            const DSPLIB_bufParams1D_t  *bufParamsOut,
                                            const DSPLIB_recip_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_recip_init_checkParams\n");
#endif
   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {

      // only float, double and short datatype support for now
      if ((bufParamsIn->data_type != DSPLIB_INT16) && (bufParamsIn->data_type != DSPLIB_FLOAT32) &&
          (bufParamsIn->data_type != DSPLIB_FLOAT64)) {
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
DSPLIB_recip_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_recip_exec_checkParams\n");
#endif
   if ((pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_recip_init(DSPLIB_kernelHandle          handle,
                                DSPLIB_bufParams1D_t        *bufParamsIn,
                                DSPLIB_bufParams1D_t        *bufParamsOut,
                                const DSPLIB_recip_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS          status       = DSPLIB_SUCCESS;
   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_recip_init\n");
#endif
   pKerPrivArgs->blockSize = pKerInitArgs->dataSize;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_recip_init pKerPrivArgs->blockSize %d bufParamsIn->data_type %d\n",
          pKerPrivArgs->blockSize, bufParamsIn->data_type);
#endif

   // fill private args with init args
   pKerPrivArgs->initArgs = *pKerInitArgs;

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_recip_exec_cn<float, DSPLIB_FLOAT32>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_recip_exec_cn<int16_t, DSPLIB_INT16>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_recip_exec_cn<double, DSPLIB_FLOAT64>;
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
         pKerPrivArgs->execute = DSPLIB_recip_exec_ci<float, DSPLIB_FLOAT32>;
         status                = DSPLIB_recip_init_ci<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_recip_exec_ci<int16_t, DSPLIB_INT16>;
         status                = DSPLIB_recip_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_recip_exec_ci<double, DSPLIB_FLOAT64>;
         status                = DSPLIB_recip_init_ci<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
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

DSPLIB_STATUS DSPLIB_recip_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_recip_exec\n");
#endif

   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT pKerPrivArgs->blockSize %d\n", pKerPrivArgs->blockSize);
#endif
   status = pKerPrivArgs->execute(handle, pIn, pOut);

   return status;
}
