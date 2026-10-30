// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_peakAbsoluteValue_priv.h"

int32_t DSPLIB_peakAbsoluteValue_getHandleSize(DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_peakAbsoluteValue_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_checkParams(DSPLIB_kernelHandle                      handle,
                                                        const DSPLIB_bufParams2D_t              *bufParamsIn,
                                                        const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                        const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_peakAbsoluteValue_init_checkParams\n");
#endif
   if ((handle == NULL) || (bufParamsIn == NULL) || (bufParamsOut == NULL) || (pKerInitArgs == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {

      if ((bufParamsIn->data_type != DSPLIB_INT16) && (bufParamsIn->data_type != DSPLIB_INT32) &&
          (bufParamsIn->data_type != DSPLIB_INT8) && (bufParamsIn->data_type != DSPLIB_INT64) &&
          (bufParamsIn->data_type != DSPLIB_FLOAT32) && (bufParamsIn->data_type != DSPLIB_FLOAT64)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }

      else if (bufParamsIn->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if ((bufParamsIn->dim_x < 1) || (bufParamsIn->dim_y < 1)) {
         /* The exec loop runs at least once */
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else if ((bufParamsOut->dim_x != bufParamsIn->dim_y) || (bufParamsOut->dim_y != 1)) {
         /* One output value per input row */
         status = DSPLIB_ERR_INVALID_DIMENSION;
      }
      else if (((uint32_t) bufParamsIn->stride_y < bufParamsIn->dim_x * DSPLIB_sizeof(bufParamsIn->data_type)) ||
               (((uint32_t) bufParamsIn->stride_y % DSPLIB_sizeof(bufParamsIn->data_type)) != 0U)) {
         /* A row must fit inside its stride, and the stride is a whole number of elements */
         status = DSPLIB_ERR_INVALID_DIMENSION;
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
DSPLIB_peakAbsoluteValue_exec_checkParams(DSPLIB_kernelHandle handle,
                                          const void *restrict pIn,
                                          const void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_peakAbsoluteValue_exec_checkParams\n");
#endif
   if ((pIn == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init(DSPLIB_kernelHandle                      handle,
                                            DSPLIB_bufParams2D_t                    *bufParamsIn,
                                            DSPLIB_bufParams2D_t                    *bufParamsOut,
                                            const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_peakAbsoluteValue_PrivArgs *pKerPrivArgs = (DSPLIB_peakAbsoluteValue_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_peakAbsoluteValue_init\n");
#endif
   pKerPrivArgs->width  = (int32_t) bufParamsIn->dim_x;
   pKerPrivArgs->height = (int32_t) bufParamsIn->dim_y;
   pKerPrivArgs->strideInElements =
       (int32_t) ((uint32_t) bufParamsIn->stride_y / DSPLIB_sizeof(bufParamsIn->data_type));

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_peakAbsoluteValue_init width %d height %d bufParamsIn->data_type %d\n",
          pKerPrivArgs->width, pKerPrivArgs->height, bufParamsIn->data_type);
#endif

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      if (bufParamsIn->data_type == DSPLIB_INT8) {
         pKerPrivArgs->execute = DSPLIB_peakAbsoluteValue_exec_cn<int8_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         pKerPrivArgs->execute = DSPLIB_peakAbsoluteValue_exec_cn<int16_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         pKerPrivArgs->execute = DSPLIB_peakAbsoluteValue_exec_cn<int32_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         pKerPrivArgs->execute = DSPLIB_peakAbsoluteValue_exec_cn<int64_t>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_peakAbsoluteValue_exec_cn<float>;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         pKerPrivArgs->execute = DSPLIB_peakAbsoluteValue_exec_cn<double>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT  CP 2 status %d\n", status);
#endif
      }
   }
   else {
      /* Row variants: rows of up to 2 vectors and of 3-4 vectors run two rows per loop iteration (the first row alone
       * for an odd number of rows), longer rows use an inner loop. init_ci then decides whether the transposed variant
       * is cheaper, and the variant is picked from the table below */
      int32_t eleCount   = (int32_t) (__C7X_VEC_SIZE_BYTES__ / DSPLIB_sizeof(bufParamsIn->data_type));
      int32_t numVecs    = (pKerPrivArgs->width + eleCount - 1) / eleCount;
      int32_t variantIdx = (numVecs <= 2) ? 0 : ((numVecs <= 4) ? 1 : 2);

      const pFxnDSPLIB_peakAbsoluteValue_exec(*execVariants)[3] = NULL;

      if (bufParamsIn->data_type == DSPLIB_INT8) {
         static const pFxnDSPLIB_peakAbsoluteValue_exec execTable[3][3] = {
             /* even number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int8_t, uint8_t, 1, 0>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int8_t, uint8_t, 2, 0>,
              DSPLIB_peakAbsoluteValue_exec_ci<int8_t, uint8_t>},
             /* odd number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int8_t, uint8_t, 1, 1>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int8_t, uint8_t, 2, 1>,
              DSPLIB_peakAbsoluteValue_exec_ci<int8_t, uint8_t>},
             /* transposed */
             {DSPLIB_peakAbsoluteValue_transposed_exec_ci<int8_t>, NULL, NULL}};
         execVariants = execTable;
         status       = DSPLIB_peakAbsoluteValue_init_ci<int8_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         static const pFxnDSPLIB_peakAbsoluteValue_exec execTable[3][3] = {
             /* even number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int16_t, uint16_t, 1, 0>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int16_t, uint16_t, 2, 0>,
              DSPLIB_peakAbsoluteValue_exec_ci<int16_t, uint16_t>},
             /* odd number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int16_t, uint16_t, 1, 1>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int16_t, uint16_t, 2, 1>,
              DSPLIB_peakAbsoluteValue_exec_ci<int16_t, uint16_t>},
             /* transposed */
             {DSPLIB_peakAbsoluteValue_transposed_exec_ci<int16_t>, NULL, NULL}};
         execVariants = execTable;
         status       = DSPLIB_peakAbsoluteValue_init_ci<int16_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT32) {
         static const pFxnDSPLIB_peakAbsoluteValue_exec execTable[3][3] = {
             /* even number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int32_t, uint32_t, 1, 0>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int32_t, uint32_t, 2, 0>,
              DSPLIB_peakAbsoluteValue_exec_ci<int32_t, uint32_t>},
             /* odd number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int32_t, uint32_t, 1, 1>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int32_t, uint32_t, 2, 1>,
              DSPLIB_peakAbsoluteValue_exec_ci<int32_t, uint32_t>},
             /* transposed */
             {DSPLIB_peakAbsoluteValue_transposed_exec_ci<int32_t>, NULL, NULL}};
         execVariants = execTable;
         status       = DSPLIB_peakAbsoluteValue_init_ci<int32_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_INT64) {
         static const pFxnDSPLIB_peakAbsoluteValue_exec execTable[3][3] = {
             /* even number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int64_t, uint64_t, 1, 0>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int64_t, uint64_t, 2, 0>,
              DSPLIB_peakAbsoluteValue_exec_ci<int64_t, uint64_t>},
             /* odd number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int64_t, uint64_t, 1, 1>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int64_t, uint64_t, 2, 1>,
              DSPLIB_peakAbsoluteValue_exec_ci<int64_t, uint64_t>},
             /* transposed */
             {DSPLIB_peakAbsoluteValue_transposed_exec_ci<int64_t>, NULL, NULL}};
         execVariants = execTable;
         status       = DSPLIB_peakAbsoluteValue_init_ci<int64_t>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         static const pFxnDSPLIB_peakAbsoluteValue_exec execTable[3][3] = {
             /* even number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<float, uint32_t, 1, 0>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<float, uint32_t, 2, 0>,
              DSPLIB_peakAbsoluteValue_exec_ci<float, uint32_t>},
             /* odd number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<float, uint32_t, 1, 1>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<float, uint32_t, 2, 1>,
              DSPLIB_peakAbsoluteValue_exec_ci<float, uint32_t>},
             /* transposed */
             {DSPLIB_peakAbsoluteValue_transposed_exec_ci<float>, NULL, NULL}};
         execVariants = execTable;
         status       = DSPLIB_peakAbsoluteValue_init_ci<float>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT64) {
         static const pFxnDSPLIB_peakAbsoluteValue_exec execTable[3][3] = {
             /* even number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<double, uint64_t, 1, 0>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<double, uint64_t, 2, 0>,
              DSPLIB_peakAbsoluteValue_exec_ci<double, uint64_t>},
             /* odd number of rows */
             {DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<double, uint64_t, 1, 1>,
              DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<double, uint64_t, 2, 1>,
              DSPLIB_peakAbsoluteValue_exec_ci<double, uint64_t>},
             /* transposed */
             {DSPLIB_peakAbsoluteValue_transposed_exec_ci<double>, NULL, NULL}};
         execVariants = execTable;
         status       = DSPLIB_peakAbsoluteValue_init_ci<double>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }

      if (status == DSPLIB_SUCCESS) {
         int32_t tableRow = (pKerPrivArgs->transposed == 1) ? 2 : (pKerPrivArgs->height & 1);
         int32_t tableCol = (pKerPrivArgs->transposed == 1) ? 0 : variantIdx;

         pKerPrivArgs->execute = execVariants[tableRow][tableCol];
      }
   }
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  CP 3 status %d\n", status);
#endif
   return status;
}

DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_peakAbsoluteValue_exec\n");
#endif

   DSPLIB_peakAbsoluteValue_PrivArgs *pKerPrivArgs = (DSPLIB_peakAbsoluteValue_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT pKerPrivArgs->width %d height %d\n", pKerPrivArgs->width, pKerPrivArgs->height);
#endif
   status = pKerPrivArgs->execute(handle, pIn, pOut);

   return status;
}
