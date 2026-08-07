// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bufParams.h"
#include "DSPLIB_thresholdDetector_priv.h"
#include <string.h>

int32_t DSPLIB_thresholdDetector_getHandleSize(DSPLIB_thresholdDetector_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_thresholdDetector_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_thresholdDetector_init_checkParams(DSPLIB_kernelHandle                      handle,
                                          const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                          const DSPLIB_bufParams1D_t              *bufParamsIn1,
                                          const DSPLIB_bufParams2D_t              *bufParamsOut,
                                          const DSPLIB_thresholdDetector_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_thresholdDetector_init_checkParams \n");

   if ((handle == NULL) || (bufParamsIn0 == NULL) || (bufParamsIn1 == NULL) || (bufParamsOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn0->data_type != DSPLIB_FLOAT32 || bufParamsIn1->data_type != DSPLIB_FLOAT32) &&
          (bufParamsIn0->data_type != DSPLIB_FLOAT64 || bufParamsIn1->data_type != DSPLIB_FLOAT64) &&
          (bufParamsIn0->data_type != DSPLIB_INT32 || bufParamsIn1->data_type != DSPLIB_INT32) &&
          (bufParamsIn0->data_type != DSPLIB_UINT32 || bufParamsIn1->data_type != DSPLIB_UINT32) &&
          (bufParamsIn0->data_type != DSPLIB_INT16 || bufParamsIn1->data_type != DSPLIB_INT16) &&
          (bufParamsIn0->data_type != DSPLIB_UINT16 || bufParamsIn1->data_type != DSPLIB_UINT16) &&
          (bufParamsIn0->data_type != DSPLIB_INT64 || bufParamsIn1->data_type != DSPLIB_INT64) &&
          (bufParamsIn0->data_type != DSPLIB_UINT64 || bufParamsIn1->data_type != DSPLIB_UINT64) &&
          (bufParamsIn0->data_type != DSPLIB_INT8 || bufParamsIn1->data_type != DSPLIB_INT8) &&
          (bufParamsIn0->data_type != DSPLIB_UINT8 || bufParamsIn1->data_type != DSPLIB_UINT8)) {
         {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
   }
   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn0->data_type != DSPLIB_FLOAT32 || bufParamsOut->data_type != DSPLIB_UINT32) &&
          (bufParamsIn0->data_type != DSPLIB_FLOAT64 || bufParamsOut->data_type != DSPLIB_UINT64) &&
          (bufParamsIn0->data_type != DSPLIB_INT32 || bufParamsOut->data_type != DSPLIB_UINT32) &&
          (bufParamsIn0->data_type != DSPLIB_UINT32 || bufParamsOut->data_type != DSPLIB_UINT32) &&
          (bufParamsIn0->data_type != DSPLIB_INT16 || bufParamsOut->data_type != DSPLIB_UINT16) &&
          (bufParamsIn0->data_type != DSPLIB_UINT16 || bufParamsOut->data_type != DSPLIB_UINT16) &&
          (bufParamsIn0->data_type != DSPLIB_INT64 || bufParamsOut->data_type != DSPLIB_UINT64) &&
          (bufParamsIn0->data_type != DSPLIB_UINT64 || bufParamsOut->data_type != DSPLIB_UINT64) &&
          (bufParamsIn0->data_type != DSPLIB_INT8 || bufParamsOut->data_type != DSPLIB_UINT8) &&
          (bufParamsIn0->data_type != DSPLIB_UINT8 || bufParamsOut->data_type != DSPLIB_UINT8)) {
         {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
   }
   return status;
}

DSPLIB_STATUS DSPLIB_thresholdDetector_exec_checkParams(DSPLIB_kernelHandle handle,
                                                        const void *restrict pIn0,
                                                        const void *restrict pIn1,
                                                        const void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_thresholdDetector_exec_checkParams \n");

   if ((pIn0 == NULL) || (pIn1 == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_thresholdDetector_init(DSPLIB_kernelHandle                      handle,
                                            DSPLIB_bufParams2D_t                    *bufParamsIn0,
                                            DSPLIB_bufParams1D_t                    *bufParamsIn1,
                                            DSPLIB_bufParams2D_t                    *bufParamsOut,
                                            const DSPLIB_thresholdDetector_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_thresholdDetector_PrivArgs *pKerPrivArgs = (DSPLIB_thresholdDetector_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_thresholdDetector_init \n");

   pKerPrivArgs->dim_x          = bufParamsIn0->dim_x;
   pKerPrivArgs->dim_y          = bufParamsIn0->dim_y;
   pKerPrivArgs->strideIn       = bufParamsIn0->stride_y / DSPLIB_sizeof(bufParamsIn0->data_type);
   pKerPrivArgs->strideOut      = bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsOut->data_type);
   pKerPrivArgs->funcStyle      = pKerInitArgs->funcStyle;
   pKerPrivArgs->dataType       = bufParamsIn0->data_type;
   pKerPrivArgs->execute        = NULL;
   pKerPrivArgs->setArgs.opType = DSPLIB_THRESHOLDDETECTOR_EQ;

   if (pKerInitArgs->funcStyle != DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn0->data_type == DSPLIB_FLOAT32) {
         status = DSPLIB_thresholdDetector_init_ci<float, uint32_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                    pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_FLOAT64) {
         status = DSPLIB_thresholdDetector_init_ci<double, uint64_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                     pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT8) {
         status = DSPLIB_thresholdDetector_init_ci<int8_t, uint8_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                    pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT8) {
         status = DSPLIB_thresholdDetector_init_ci<uint8_t, uint8_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                     pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT16) {
         status = DSPLIB_thresholdDetector_init_ci<int16_t, uint16_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                      pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT16) {
         status = DSPLIB_thresholdDetector_init_ci<uint16_t, uint16_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                       pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT32) {
         status = DSPLIB_thresholdDetector_init_ci<int32_t, uint32_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                      pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT32) {
         status = DSPLIB_thresholdDetector_init_ci<uint32_t, uint32_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                       pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_INT64) {
         status = DSPLIB_thresholdDetector_init_ci<int64_t, uint64_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                      pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT64) {
         status = DSPLIB_thresholdDetector_init_ci<uint64_t, uint64_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                       pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_thresholdDetector_set(DSPLIB_kernelHandle                     handle,
                                           const DSPLIB_thresholdDetector_SetArgs *pKerSetArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_thresholdDetector_set \n");

   if ((handle == NULL) || (pKerSetArgs == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else if (pKerSetArgs->opType > DSPLIB_THRESHOLDDETECTOR_GE) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }

   if (status == DSPLIB_SUCCESS) {
      DSPLIB_thresholdDetector_PrivArgs *pKerPrivArgs = (DSPLIB_thresholdDetector_PrivArgs *) handle;

      if (pKerPrivArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
         if (pKerPrivArgs->dataType == DSPLIB_FLOAT32) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_FLOAT64) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_INT8) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT8) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_INT16) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT16) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_INT32) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT32) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_INT64) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_cn<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT64) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_cn<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
      else {
         if (pKerPrivArgs->dataType == DSPLIB_FLOAT32) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<float, uint32_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_FLOAT64) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<double, uint64_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_INT8) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT8) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<uint8_t, uint8_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_INT16) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT16) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint16_t, uint16_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_INT32) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT32) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint32_t, uint32_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_INT64) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute = DSPLIB_thresholdDetector_exec_ci<int64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT64) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_THRESHOLDDETECTOR_EQ:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_EQ>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_NE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_NE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_LE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_LE>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GT:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GT>;
               break;
            case DSPLIB_THRESHOLDDETECTOR_GE:
               pKerPrivArgs->execute =
                   DSPLIB_thresholdDetector_exec_ci<uint64_t, uint64_t, DSPLIB_THRESHOLDDETECTOR_GE>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }

      if (status == DSPLIB_SUCCESS) {
         memcpy(&pKerPrivArgs->setArgs, pKerSetArgs, sizeof(DSPLIB_thresholdDetector_SetArgs));
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_thresholdDetector_get(DSPLIB_kernelHandle handle, DSPLIB_thresholdDetector_SetArgs *pKerSetArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if ((handle == NULL) || (pKerSetArgs == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      DSPLIB_thresholdDetector_PrivArgs *pKerPrivArgs = (DSPLIB_thresholdDetector_PrivArgs *) handle;
      memcpy(pKerSetArgs, &pKerPrivArgs->setArgs, sizeof(DSPLIB_thresholdDetector_SetArgs));
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_thresholdDetector_exec(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_thresholdDetector_exec \n");

   DSPLIB_thresholdDetector_PrivArgs *pKerPrivArgs = (DSPLIB_thresholdDetector_PrivArgs *) handle;

   if (pKerPrivArgs->execute == NULL) {
      /* DSPLIB_thresholdDetector_set has not been called */
      status = DSPLIB_ERR_FAILURE;
   }
   else {
      status = pKerPrivArgs->execute(handle, pIn0, pIn1, pOut);
   }

   return status;
}
