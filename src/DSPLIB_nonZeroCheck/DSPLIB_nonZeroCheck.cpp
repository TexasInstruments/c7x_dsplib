// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bufParams.h"
#include "DSPLIB_nonZeroCheck_priv.h"

int32_t DSPLIB_nonZeroCheck_getHandleSize(DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_nonZeroCheck_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_nonZeroCheck_init_checkParams(DSPLIB_kernelHandle                 handle,
                                     const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                     const DSPLIB_bufParams1D_t         *bufParamsOut,
                                     const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_nonZeroCheck_init_checkParams \n");

   if ((handle == NULL) || (bufParamsIn0 == NULL) || (bufParamsOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      bool typeSupported = false;
      switch (bufParamsIn0->data_type) {
      case DSPLIB_INT8:
      case DSPLIB_UINT8:
      case DSPLIB_INT16:
      case DSPLIB_UINT16:
      case DSPLIB_INT32:
      case DSPLIB_UINT32:
      case DSPLIB_INT64:
      case DSPLIB_UINT64:
      case DSPLIB_FLOAT32:
      case DSPLIB_FLOAT64:
         typeSupported = true;
         break;
      default:
         break;
      }

      /* Output data type must match the input data type */
      if ((!typeSupported) || (bufParamsOut->data_type != bufParamsIn0->data_type)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }
   if (status == DSPLIB_SUCCESS) {
      if (pKerInitArgs->opType > DSPLIB_NONZEROCHECK_ANY) {
         status = DSPLIB_ERR_INVALID_VALUE;
      }
      else {
         /* Nothing to do here */
      }
   }

   if (status == DSPLIB_SUCCESS) {
      if (pKerInitArgs->axisType > DSPLIB_NONZEROCHECK_COLUMN) {
         status = DSPLIB_ERR_INVALID_VALUE;
      }
      else {
         /* Nothing to do here */
      }
   }

   if (status == DSPLIB_SUCCESS) {
      uint32_t outLength =
          (pKerInitArgs->axisType == DSPLIB_NONZEROCHECK_ROW) ? bufParamsIn0->dim_x : bufParamsIn0->dim_y;

      if ((bufParamsIn0->dim_x < 1) || (bufParamsIn0->dim_y < 1)) {
         // The exec loops run at least once
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else if ((uint32_t) bufParamsIn0->stride_y < bufParamsIn0->dim_x * DSPLIB_sizeof(bufParamsIn0->data_type)) {
         // A row must fit inside its stride
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else if (bufParamsOut->dim_x != outLength) {
         // One output element per reduction
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else {
         /* Nothing to do here */
      }
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_nonZeroCheck_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn0, const void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_nonZeroCheck_exec_checkParams \n");

   if ((handle == NULL) || (pIn0 == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

/* Selects the natural C or optimized execution function for dataType and, for
 * the optimized variant, prepares the streaming engine configuration. */
template <typename dataType>
static DSPLIB_STATUS DSPLIB_nonZeroCheck_setExec(DSPLIB_kernelHandle                 handle,
                                                 const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                 const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                 const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;
   bool                          isRow        = (pKerInitArgs->axisType == DSPLIB_NONZEROCHECK_ROW);
   bool                          isTwo        = false;

   if (pKerInitArgs->funcStyle != DSPLIB_FUNCTION_NATC) {
      status = DSPLIB_nonZeroCheck_init_ci<dataType>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      isTwo  = (pKerPrivArgs->twoPerStream != 0u);
   }

   switch (pKerInitArgs->opType) {
   case DSPLIB_NONZEROCHECK_ALL:
      if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
         pKerPrivArgs->execute =
             isRow ? DSPLIB_nonZeroCheck_all_row_exec_cn<dataType> : DSPLIB_nonZeroCheck_all_column_exec_cn<dataType>;
      }
      else if (isRow) {
         pKerPrivArgs->execute = isTwo ? DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<dataType>
                                       : DSPLIB_nonZeroCheck_all_row_exec_ci<dataType>;
      }
      else {
         pKerPrivArgs->execute = isTwo ? DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<dataType>
                                       : DSPLIB_nonZeroCheck_all_column_exec_ci<dataType>;
      }
      break;
   case DSPLIB_NONZEROCHECK_ANY:
      if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
         pKerPrivArgs->execute =
             isRow ? DSPLIB_nonZeroCheck_any_row_exec_cn<dataType> : DSPLIB_nonZeroCheck_any_column_exec_cn<dataType>;
      }
      else if (isRow) {
         pKerPrivArgs->execute = isTwo ? DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<dataType>
                                       : DSPLIB_nonZeroCheck_any_row_exec_ci<dataType>;
      }
      else {
         pKerPrivArgs->execute = isTwo ? DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<dataType>
                                       : DSPLIB_nonZeroCheck_any_column_exec_ci<dataType>;
      }
      break;
   default:
      /* No exec function for this opType: fail instead of leaving execute unset */
      status = DSPLIB_ERR_INVALID_VALUE;
      break;
   }

   /* An axisType other than ROW would otherwise run the COLUMN variant */
   if (pKerInitArgs->axisType > DSPLIB_NONZEROCHECK_COLUMN) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_nonZeroCheck_init(DSPLIB_kernelHandle                 handle,
                                       DSPLIB_bufParams2D_t               *bufParamsIn0,
                                       DSPLIB_bufParams1D_t               *bufParamsOut,
                                       const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_nonZeroCheck_init \n");

   pKerPrivArgs->axisType = pKerInitArgs->axisType;
   pKerPrivArgs->dim_x    = bufParamsIn0->dim_x;
   pKerPrivArgs->dim_y    = bufParamsIn0->dim_y;
   pKerPrivArgs->strideIn = bufParamsIn0->stride_y / DSPLIB_sizeof(bufParamsIn0->data_type);
   pKerPrivArgs->opType   = pKerInitArgs->opType;
   pKerPrivArgs->dataType = bufParamsIn0->data_type;

   switch (bufParamsIn0->data_type) {
   case DSPLIB_INT8:
      status = DSPLIB_nonZeroCheck_setExec<int8_t>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   case DSPLIB_UINT8:
      status = DSPLIB_nonZeroCheck_setExec<uint8_t>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   case DSPLIB_INT16:
      status = DSPLIB_nonZeroCheck_setExec<int16_t>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   case DSPLIB_UINT16:
      status = DSPLIB_nonZeroCheck_setExec<uint16_t>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   case DSPLIB_INT32:
      status = DSPLIB_nonZeroCheck_setExec<int32_t>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   case DSPLIB_UINT32:
      status = DSPLIB_nonZeroCheck_setExec<uint32_t>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   case DSPLIB_INT64:
      status = DSPLIB_nonZeroCheck_setExec<int64_t>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   case DSPLIB_UINT64:
      status = DSPLIB_nonZeroCheck_setExec<uint64_t>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   case DSPLIB_FLOAT32:
      status = DSPLIB_nonZeroCheck_setExec<float>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   case DSPLIB_FLOAT64:
      status = DSPLIB_nonZeroCheck_setExec<double>(handle, bufParamsIn0, bufParamsOut, pKerInitArgs);
      break;
   default:
      status = DSPLIB_ERR_INVALID_TYPE;
      break;
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_nonZeroCheck_exec(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_nonZeroCheck_exec \n");

   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn0, pOut);

   return status;
}
