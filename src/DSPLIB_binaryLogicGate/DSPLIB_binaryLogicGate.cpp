// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_binaryLogicGate_priv.h"
#include "DSPLIB_bufParams.h"
#include <string.h>

int32_t DSPLIB_binaryLogicGate_getHandleSize(DSPLIB_binaryLogicGate_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_binaryLogicGate_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS
DSPLIB_binaryLogicGate_init_checkParams(DSPLIB_kernelHandle                    handle,
                                        const DSPLIB_bufParams2D_t            *bufParamsIn0,
                                        const DSPLIB_bufParams2D_t            *bufParamsIn1,
                                        const DSPLIB_bufParams2D_t            *bufParamsOut,
                                        const DSPLIB_binaryLogicGate_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_binaryLogicGate_init_checkParams \n");

   if ((handle == NULL) || (bufParamsIn0 == NULL) || (bufParamsIn1 == NULL) || (bufParamsOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn0->data_type != DSPLIB_UINT32 || bufParamsIn1->data_type != DSPLIB_UINT32) &&
          (bufParamsIn0->data_type != DSPLIB_UINT16 || bufParamsIn1->data_type != DSPLIB_UINT16) &&
          (bufParamsIn0->data_type != DSPLIB_UINT64 || bufParamsIn1->data_type != DSPLIB_UINT64) &&
          (bufParamsIn0->data_type != DSPLIB_UINT8 || bufParamsIn1->data_type != DSPLIB_UINT8)) {
         {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
   }
   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn0->data_type != DSPLIB_UINT32 || bufParamsOut->data_type != DSPLIB_UINT32) &&
          (bufParamsIn0->data_type != DSPLIB_UINT16 || bufParamsOut->data_type != DSPLIB_UINT16) &&
          (bufParamsIn0->data_type != DSPLIB_UINT64 || bufParamsOut->data_type != DSPLIB_UINT64) &&
          (bufParamsIn0->data_type != DSPLIB_UINT8 || bufParamsOut->data_type != DSPLIB_UINT8)) {
         {
            status = DSPLIB_ERR_INVALID_TYPE;
         }
      }
   }
   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn0->dim_x != bufParamsIn1->dim_x) || (bufParamsIn0->dim_y != bufParamsIn1->dim_y) ||
          (bufParamsIn0->dim_x != bufParamsOut->dim_x) || (bufParamsIn0->dim_y != bufParamsOut->dim_y)) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
   }
   if (status == DSPLIB_SUCCESS) {
      /* exec uses a single input stride for both inputs, so In1 must have the same stride as In0 */
      if (bufParamsIn0->stride_y != bufParamsIn1->stride_y) {
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
   }
   return status;
}

DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_checkParams(DSPLIB_kernelHandle handle,
                                                      const void *restrict pIn0,
                                                      const void *restrict pIn1,
                                                      const void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_binaryLogicGate_exec_checkParams \n");

   if ((handle == NULL) || (pIn0 == NULL) || (pIn1 == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_binaryLogicGate_init(DSPLIB_kernelHandle                    handle,
                                          DSPLIB_bufParams2D_t                  *bufParamsIn0,
                                          DSPLIB_bufParams2D_t                  *bufParamsIn1,
                                          DSPLIB_bufParams2D_t                  *bufParamsOut,
                                          const DSPLIB_binaryLogicGate_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                    status       = DSPLIB_SUCCESS;
   DSPLIB_binaryLogicGate_PrivArgs *pKerPrivArgs = (DSPLIB_binaryLogicGate_PrivArgs *) handle;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_binaryLogicGate_init \n");

   pKerPrivArgs->dim_x          = bufParamsIn0->dim_x;
   pKerPrivArgs->dim_y          = bufParamsIn0->dim_y;
   pKerPrivArgs->strideIn       = bufParamsIn0->stride_y / DSPLIB_sizeof(bufParamsIn0->data_type);
   pKerPrivArgs->strideOut      = bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsOut->data_type);
   pKerPrivArgs->funcStyle      = pKerInitArgs->funcStyle;
   pKerPrivArgs->dataType       = bufParamsIn0->data_type;
   pKerPrivArgs->execute        = NULL;
   pKerPrivArgs->setArgs.opType = DSPLIB_BINARYLOGICGATE_AND;

   if (pKerInitArgs->funcStyle != DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn0->data_type == DSPLIB_UINT8) {
         status =
             DSPLIB_binaryLogicGate_init_ci<uint8_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT16) {
         status =
             DSPLIB_binaryLogicGate_init_ci<uint16_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT32) {
         status =
             DSPLIB_binaryLogicGate_init_ci<uint32_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == DSPLIB_UINT64) {
         status =
             DSPLIB_binaryLogicGate_init_ci<uint64_t>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_binaryLogicGate_set(DSPLIB_kernelHandle handle, const DSPLIB_binaryLogicGate_SetArgs *pKerSetArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_binaryLogicGate_set \n");

   if ((handle == NULL) || (pKerSetArgs == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else if (pKerSetArgs->opType > DSPLIB_BINARYLOGICGATE_XOR) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }

   if (status == DSPLIB_SUCCESS) {
      DSPLIB_binaryLogicGate_PrivArgs *pKerPrivArgs = (DSPLIB_binaryLogicGate_PrivArgs *) handle;

      if (pKerPrivArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
         if (pKerPrivArgs->dataType == DSPLIB_UINT8) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_BINARYLOGICGATE_AND:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint8_t, DSPLIB_BINARYLOGICGATE_AND>;
               break;
            case DSPLIB_BINARYLOGICGATE_OR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint8_t, DSPLIB_BINARYLOGICGATE_OR>;
               break;
            case DSPLIB_BINARYLOGICGATE_XOR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint8_t, DSPLIB_BINARYLOGICGATE_XOR>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT16) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_BINARYLOGICGATE_AND:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint16_t, DSPLIB_BINARYLOGICGATE_AND>;
               break;
            case DSPLIB_BINARYLOGICGATE_OR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint16_t, DSPLIB_BINARYLOGICGATE_OR>;
               break;
            case DSPLIB_BINARYLOGICGATE_XOR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint16_t, DSPLIB_BINARYLOGICGATE_XOR>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT32) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_BINARYLOGICGATE_AND:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint32_t, DSPLIB_BINARYLOGICGATE_AND>;
               break;
            case DSPLIB_BINARYLOGICGATE_OR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint32_t, DSPLIB_BINARYLOGICGATE_OR>;
               break;
            case DSPLIB_BINARYLOGICGATE_XOR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint32_t, DSPLIB_BINARYLOGICGATE_XOR>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT64) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_BINARYLOGICGATE_AND:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint64_t, DSPLIB_BINARYLOGICGATE_AND>;
               break;
            case DSPLIB_BINARYLOGICGATE_OR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint64_t, DSPLIB_BINARYLOGICGATE_OR>;
               break;
            case DSPLIB_BINARYLOGICGATE_XOR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_cn<uint64_t, DSPLIB_BINARYLOGICGATE_XOR>;
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
         if (pKerPrivArgs->dataType == DSPLIB_UINT8) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_BINARYLOGICGATE_AND:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint8_t, DSPLIB_BINARYLOGICGATE_AND>;
               break;
            case DSPLIB_BINARYLOGICGATE_OR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint8_t, DSPLIB_BINARYLOGICGATE_OR>;
               break;
            case DSPLIB_BINARYLOGICGATE_XOR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint8_t, DSPLIB_BINARYLOGICGATE_XOR>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT16) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_BINARYLOGICGATE_AND:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint16_t, DSPLIB_BINARYLOGICGATE_AND>;
               break;
            case DSPLIB_BINARYLOGICGATE_OR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint16_t, DSPLIB_BINARYLOGICGATE_OR>;
               break;
            case DSPLIB_BINARYLOGICGATE_XOR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint16_t, DSPLIB_BINARYLOGICGATE_XOR>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT32) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_BINARYLOGICGATE_AND:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint32_t, DSPLIB_BINARYLOGICGATE_AND>;
               break;
            case DSPLIB_BINARYLOGICGATE_OR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint32_t, DSPLIB_BINARYLOGICGATE_OR>;
               break;
            case DSPLIB_BINARYLOGICGATE_XOR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint32_t, DSPLIB_BINARYLOGICGATE_XOR>;
               break;
            default:
               status = DSPLIB_ERR_INVALID_VALUE;
               break;
            }
         }
         else if (pKerPrivArgs->dataType == DSPLIB_UINT64) {
            switch (pKerSetArgs->opType) {
            case DSPLIB_BINARYLOGICGATE_AND:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint64_t, DSPLIB_BINARYLOGICGATE_AND>;
               break;
            case DSPLIB_BINARYLOGICGATE_OR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint64_t, DSPLIB_BINARYLOGICGATE_OR>;
               break;
            case DSPLIB_BINARYLOGICGATE_XOR:
               pKerPrivArgs->execute = DSPLIB_binaryLogicGate_exec_ci<uint64_t, DSPLIB_BINARYLOGICGATE_XOR>;
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
         memcpy(&pKerPrivArgs->setArgs, pKerSetArgs, sizeof(DSPLIB_binaryLogicGate_SetArgs));
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_binaryLogicGate_get(DSPLIB_kernelHandle handle, DSPLIB_binaryLogicGate_SetArgs *pKerSetArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if ((handle == NULL) || (pKerSetArgs == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      DSPLIB_binaryLogicGate_PrivArgs *pKerPrivArgs = (DSPLIB_binaryLogicGate_PrivArgs *) handle;
      memcpy(pKerSetArgs, &pKerPrivArgs->setArgs, sizeof(DSPLIB_binaryLogicGate_SetArgs));
   }

   return status;
}

DSPLIB_STATUS
DSPLIB_binaryLogicGate_exec(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_binaryLogicGate_exec \n");

   DSPLIB_binaryLogicGate_PrivArgs *pKerPrivArgs = (DSPLIB_binaryLogicGate_PrivArgs *) handle;

   if (pKerPrivArgs->execute == NULL) {
      /* DSPLIB_binaryLogicGate_set has not been called */
      status = DSPLIB_ERR_FAILURE;
   }
   else {
      status = pKerPrivArgs->execute(handle, pIn0, pIn1, pOut);
   }

   return status;
}
