// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_minerror_priv.h"

int32_t DSPLIB_minerror_getHandleSize(DSPLIB_minerror_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   int32_t privBufSize = sizeof(DSPLIB_minerror_PrivArgs);
   DSPLIB_DEBUGPRINTFN(0, "Exit function with privBufSize: %d\n", privBufSize);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_minerror_init_checkParams(DSPLIB_kernelHandle             handle,
                                               const DSPLIB_bufParams2D_t     *bufParamsIn,
                                               const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                               const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                               const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                               const DSPLIB_minerror_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn->data_type != DSPLIB_INT8) && (bufParamsIn->data_type != DSPLIB_UINT8) &&
          (bufParamsIn->data_type != DSPLIB_INT16) && (bufParamsIn->data_type != DSPLIB_UINT16) &&
          (bufParamsIn->data_type != DSPLIB_INT32) && (bufParamsIn->data_type != DSPLIB_UINT32) &&
          (bufParamsIn->data_type != DSPLIB_INT64) && (bufParamsIn->data_type != DSPLIB_UINT64) &&
          (bufParamsIn->data_type != DSPLIB_FLOAT32) && (bufParamsIn->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsErrCoefs->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsOutVal->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_minerror_exec_checkParams(DSPLIB_kernelHandle handle,
                                               const void *restrict pIn,
                                               const void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status;

   if ((pIn == NULL) || (pErrCoefs == NULL) || (pMaxIndex == NULL) || (pMaxVal == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_minerror_init(DSPLIB_kernelHandle       handle,
                                   DSPLIB_bufParams2D_t     *bufParamsIn,
                                   DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                   DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                   DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                   DSPLIB_minerror_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS             status       = DSPLIB_SUCCESS;
   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   pKerPrivArgs->vecInSize    = pKerInitArgs->vecInSize;
   pKerPrivArgs->errCoefsSize = pKerInitArgs->errCoefsSize;
   pKerPrivArgs->strideIn     = pKerInitArgs->strideIn;

   DSPLIB_DEBUGPRINTFN(0, "pKerPrivArgs->vecInSize %d pKerPrivArgs->errCoefsSize %d bufParamsIn->data_type %d\n",
                       pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize, bufParamsIn->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<float>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<double>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<int8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<uint8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<int16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<uint16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<int32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<uint32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<int64_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_cn<uint64_t>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "Invalid data Type status: %d", status);
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<float>;
         status = DSPLIB_minerror_init_ci<float>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                 bufParamsOutVal, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<double>;
         status = DSPLIB_minerror_init_ci<double>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                  bufParamsOutVal, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<int8_t>;
         status = DSPLIB_minerror_init_ci<int8_t>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                  bufParamsOutVal, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT8) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<uint8_t>;
         status = DSPLIB_minerror_init_ci<uint8_t>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                   bufParamsOutVal, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<int16_t>;
         status = DSPLIB_minerror_init_ci<int16_t>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                   bufParamsOutVal, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT16) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<uint16_t>;
         status = DSPLIB_minerror_init_ci<uint16_t>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                    bufParamsOutVal, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<int32_t>;
         status = DSPLIB_minerror_init_ci<int32_t>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                   bufParamsOutVal, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<uint32_t>;
         status = DSPLIB_minerror_init_ci<uint32_t>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                    bufParamsOutVal, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<int64_t>;
         status = DSPLIB_minerror_init_ci<int64_t>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                   bufParamsOutVal, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_minerror_exec_ci<uint64_t>;
         status = DSPLIB_minerror_init_ci<uint64_t>(handle, bufParamsIn, bufParamsErrCoefs, bufParamsOutIndex,
                                                    bufParamsOutVal, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
         DSPLIB_DEBUGPRINTFN(0, "Invalid data Type status: %d", status);
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return status;
}

DSPLIB_STATUS DSPLIB_minerror_exec(DSPLIB_kernelHandle handle,
                                   void *restrict pIn,
                                   void *restrict pErrCoefs,
                                   int *restrict pMaxIndex,
                                   void *restrict pMaxVal)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   DSPLIB_STATUS status;

   DSPLIB_minerror_PrivArgs *pKerPrivArgs = (DSPLIB_minerror_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "DSPLIB_DEBUGPRINT pKerPrivArgs->vecInSize %d pKerPrivArgs->errCoefsSize %d\n",
                       pKerPrivArgs->vecInSize, pKerPrivArgs->errCoefsSize);

   status = pKerPrivArgs->execute(handle, pIn, pErrCoefs, pMaxIndex, pMaxVal);

   DSPLIB_DEBUGPRINTFN(0, "Exit function with status: %d\n", status);
   return status;
}
