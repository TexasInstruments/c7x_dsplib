// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bexp_priv.h"

int32_t DSPLIB_bexp_getHandleSize(DSPLIB_bexp_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_bexp_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_bexp_init_checkParams(DSPLIB_kernelHandle         handle,
                                           const DSPLIB_bufParams1D_t *bufParamsIn,
                                           const DSPLIB_bufParams1D_t *bufParamsOut,
                                           const DSPLIB_bexp_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "%s", "Enter DSPLIB_bexp_init_checkParams\n");

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn->data_type != DSPLIB_INT8) && (bufParamsIn->data_type != DSPLIB_INT16) &&
          (bufParamsIn->data_type != DSPLIB_INT32) && (bufParamsIn->data_type != DSPLIB_INT64) &&
          (bufParamsIn->data_type != DSPLIB_UINT8) && (bufParamsIn->data_type != DSPLIB_UINT16) &&
          (bufParamsIn->data_type != DSPLIB_UINT32) && (bufParamsIn->data_type != DSPLIB_UINT64)) {
         DSPLIB_DEBUGPRINTFN(0, "%s", "bufParamsIn->data_type is not int type\n");
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if ((bufParamsOut->data_type != DSPLIB_INT32)) {
         DSPLIB_DEBUGPRINTFN(0, "%s", "bufParamsOut->data_type is not DSPLIB_INT32\n");
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

DSPLIB_STATUS
DSPLIB_bexp_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "%s", "Enter DSPLIB_bexp_exec_checkParams\n");
   if ((pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_bexp_init(DSPLIB_kernelHandle         handle,
                               DSPLIB_bufParams1D_t       *bufParamsIn,
                               DSPLIB_bufParams1D_t       *bufParamsOut,
                               const DSPLIB_bexp_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   pKerPrivArgs->blockSize = pKerInitArgs->dataSize;

   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->blockSize %d bufParamsIn->data_type %d\n", pKerPrivArgs->blockSize,
                       bufParamsIn->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_cn<int8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_cn<uint8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_cn<int16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_cn<uint16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_cn<int32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_cn<uint32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_cn<int64_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_cn<uint64_t>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT  CP 2 status %d\n", status);
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_ci<int8_t>;
         status                = DSPLIB_bexp_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_ci<uint8_t>;
         status                = DSPLIB_bexp_init_ci<uint8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_ci<int16_t>;
         status                = DSPLIB_bexp_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_ci<uint16_t>;
         status                = DSPLIB_bexp_init_ci<uint16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_ci<int32_t>;
         status                = DSPLIB_bexp_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_ci<uint32_t>;
         status                = DSPLIB_bexp_init_ci<uint32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_ci<int64_t>;
         status                = DSPLIB_bexp_init_ci<int64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_bexp_exec_ci<uint64_t>;
         status                = DSPLIB_bexp_init_ci<uint64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_bexp_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS status;

   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->blockSize %d\n", pKerPrivArgs->blockSize);
   status = pKerPrivArgs->execute(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}
