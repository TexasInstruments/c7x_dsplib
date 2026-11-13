// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_tableSourceRowColSelector_priv.h"

int32_t DSPLIB_tableSourceRowColSelector_getHandleSize(DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_tableSourceRowColSelector_PrivArgs);

   DSPLIB_DEBUGPRINTFN(0, "privBufSize: %d\n", privBufSize);

   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_init_checkParams(DSPLIB_kernelHandle                              handle,
                                                  const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                                  const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                                  const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                                  const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if ((handle == NULL) || (bufParamsIn0 == NULL) || (bufParamsIn1 == NULL) || (bufParamsOut == NULL) ||
       (pKerInitArgs == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      /* Length of the table row or column selected by an index */
      uint32_t selLen = (pKerInitArgs->isColSelect == 0U) ? bufParamsIn1->dim_x : bufParamsIn1->dim_y;

      if (bufParamsIn0->data_type != DSPLIB_INT32) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if ((bufParamsIn1->data_type != DSPLIB_INT32) && (bufParamsIn1->data_type != DSPLIB_INT64) &&
               (bufParamsIn1->data_type != DSPLIB_UINT32) && (bufParamsIn1->data_type != DSPLIB_UINT64) &&
               (bufParamsIn1->data_type != DSPLIB_FLOAT32) && (bufParamsIn1->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn1->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (pKerInitArgs->isColSelect > 1U) {
         status = DSPLIB_ERR_INVALID_VALUE;
      }
      else if (pKerInitArgs->isOutputInterleave > 1U) {
         status = DSPLIB_ERR_INVALID_VALUE;
      }
      else if ((bufParamsIn0->dim_x == 0) || (bufParamsIn1->dim_x == 0) || (bufParamsIn1->dim_y == 0)) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else if (((pKerInitArgs->isOutputInterleave == 1U) &&
                ((bufParamsOut->dim_x != bufParamsIn0->dim_x) || (bufParamsOut->dim_y != selLen))) ||
               ((pKerInitArgs->isOutputInterleave == 0U) &&
                ((bufParamsOut->dim_x != selLen) || (bufParamsOut->dim_y != bufParamsIn0->dim_x)))) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else if (bufParamsIn1->stride_y != (int32_t) (bufParamsIn1->dim_x * DSPLIB_sizeof(bufParamsIn1->data_type))) {
         status = DSPLIB_ERR_NOT_EQUAL_WIDTH_STRIDE;
      }
      else if ((bufParamsOut->stride_y < (int32_t) (bufParamsOut->dim_x * DSPLIB_sizeof(bufParamsOut->data_type))) ||
               (((uint32_t) bufParamsOut->stride_y % DSPLIB_sizeof(bufParamsOut->data_type)) != 0U)) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else {
         /* Nothing to do here */
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_checkParams(DSPLIB_kernelHandle handle,
                                                                const void *restrict pIn0,
                                                                const void *restrict pIn1,
                                                                const void *restrict pOut,
                                                                const void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   if ((handle == NULL) || (pIn0 == NULL) || (pIn1 == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else if ((((DSPLIB_tableSourceRowColSelector_PrivArgs *) handle)->isOutputInterleave == 1U) && (pScratch == NULL)) {
      /* Only the interleaved output uses the scratch buffer */
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_init(DSPLIB_kernelHandle                              handle,
                                                    DSPLIB_bufParams1D_t                            *bufParamsIn0,
                                                    DSPLIB_bufParams2D_t                            *bufParamsIn1,
                                                    DSPLIB_bufParams2D_t                            *bufParamsOut,
                                                    const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS                              status       = DSPLIB_SUCCESS;
   DSPLIB_tableSourceRowColSelector_PrivArgs *pKerPrivArgs = (DSPLIB_tableSourceRowColSelector_PrivArgs *) handle;

   pKerPrivArgs->numRows            = (int32_t) bufParamsIn1->dim_y;
   pKerPrivArgs->numCols            = (int32_t) bufParamsIn1->dim_x;
   pKerPrivArgs->numIndices         = (int32_t) bufParamsIn0->dim_x;
   pKerPrivArgs->strideOut          = (uint32_t) bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsOut->data_type);
   pKerPrivArgs->isColSelect        = pKerInitArgs->isColSelect;
   pKerPrivArgs->isOutputInterleave = pKerInitArgs->isOutputInterleave;

   DSPLIB_DEBUGPRINTFN(0, "pKerInitArgs->funcStyle: %d bufParamsOut->data_type: %d\n", pKerInitArgs->funcStyle,
                       bufParamsOut->data_type);

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn1->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_tableSourceRowColSelector_exec_cn<float>;
      }
      else if (bufParamsIn1->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_tableSourceRowColSelector_exec_cn<double>;
      }
      else if (bufParamsIn1->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_tableSourceRowColSelector_exec_cn<int32_t>;
      }
      else if (bufParamsIn1->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_tableSourceRowColSelector_exec_cn<int64_t>;
      }
      else if (bufParamsIn1->data_type == DSPLIB_UINT32) {
         pKerPrivArgs->execute = DSPLIB_tableSourceRowColSelector_exec_cn<uint32_t>;
      }
      else if (bufParamsIn1->data_type == DSPLIB_UINT64) {
         pKerPrivArgs->execute = DSPLIB_tableSourceRowColSelector_exec_cn<uint64_t>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      /* Variant index: [isOutputInterleave][pairTail][singleTail] */
      const pFxnDSPLIB_tableSourceRowColSelector_exec(*execVariants)[2][2] = NULL;

      if (bufParamsIn1->data_type == DSPLIB_FLOAT32) {
         static const pFxnDSPLIB_tableSourceRowColSelector_exec execTable[2][2][2] = {
             /* non-interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<float, 0, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<float, 0, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<float, 0, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<float, 0, 1, 1>}},
             /* interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<float, 1, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<float, 1, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<float, 1, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<float, 1, 1, 1>}}};
         execVariants = execTable;
         status = DSPLIB_tableSourceRowColSelector_init_ci<float>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                  pKerInitArgs);
      }
      else if (bufParamsIn1->data_type == DSPLIB_FLOAT64) {
         static const pFxnDSPLIB_tableSourceRowColSelector_exec execTable[2][2][2] = {
             /* non-interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<double, 0, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<double, 0, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<double, 0, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<double, 0, 1, 1>}},
             /* interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<double, 1, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<double, 1, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<double, 1, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<double, 1, 1, 1>}}};
         execVariants = execTable;
         status = DSPLIB_tableSourceRowColSelector_init_ci<double>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                   pKerInitArgs);
      }
      else if (bufParamsIn1->data_type == DSPLIB_INT32) {
         static const pFxnDSPLIB_tableSourceRowColSelector_exec execTable[2][2][2] = {
             /* non-interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 0, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 0, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 0, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 0, 1, 1>}},
             /* interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 1, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 1, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 1, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 1, 1, 1>}}};
         execVariants = execTable;
         status = DSPLIB_tableSourceRowColSelector_init_ci<int32_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                    pKerInitArgs);
      }
      else if (bufParamsIn1->data_type == DSPLIB_INT64) {
         static const pFxnDSPLIB_tableSourceRowColSelector_exec execTable[2][2][2] = {
             /* non-interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 0, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 0, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 0, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 0, 1, 1>}},
             /* interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 1, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 1, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 1, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 1, 1, 1>}}};
         execVariants = execTable;
         status = DSPLIB_tableSourceRowColSelector_init_ci<int64_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                    pKerInitArgs);
      }
      else if (bufParamsIn1->data_type == DSPLIB_UINT32) {
         static const pFxnDSPLIB_tableSourceRowColSelector_exec execTable[2][2][2] = {
             /* non-interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 0, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 0, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 0, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 0, 1, 1>}},
             /* interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 1, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 1, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 1, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 1, 1, 1>}}};
         execVariants = execTable;
         status = DSPLIB_tableSourceRowColSelector_init_ci<uint32_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                     pKerInitArgs);
      }
      else if (bufParamsIn1->data_type == DSPLIB_UINT64) {
         static const pFxnDSPLIB_tableSourceRowColSelector_exec execTable[2][2][2] = {
             /* non-interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 0, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 0, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 0, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 0, 1, 1>}},
             /* interleaved */
             {{DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 1, 0, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 1, 0, 1>},
              {DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 1, 1, 0>,
               DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 1, 1, 1>}}};
         execVariants = execTable;
         status = DSPLIB_tableSourceRowColSelector_init_ci<uint64_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                     pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }

      if (status == DSPLIB_SUCCESS) {
         pKerPrivArgs->execute = execVariants[pKerPrivArgs->isOutputInterleave][(pKerPrivArgs->numIndices >> 1) & 1]
                                             [pKerPrivArgs->numIndices & 1];
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_exec(DSPLIB_kernelHandle handle,
                                      void *restrict pIn0,
                                      void *restrict pIn1,
                                      void *restrict pOut,
                                      void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status;

   DSPLIB_tableSourceRowColSelector_PrivArgs *pKerPrivArgs = (DSPLIB_tableSourceRowColSelector_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn0, pIn1, pOut, pScratch);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
