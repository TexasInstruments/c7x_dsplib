// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_mat_submat_copy_priv.h"

/*******************************************************************************
 *
 * INITIALIZATION
 *
 *******************************************************************************/

DSPLIB_STATUS DSPLIB_mat_submat_copy_init_cn(DSPLIB_kernelHandle                   handle,
                                             DSPLIB_bufParams2D_t                 *bufParamsX,
                                             DSPLIB_bufParams2D_t                 *bufParamsY,
                                             const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return status;
}

/*******************************************************************************/
/* NATURAL C IMPLEMENTATION                                                    */
/*******************************************************************************/
template <typename dataType>
DSPLIB_STATUS DSPLIB_mat_submat_copy_exec_cn(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS                    status       = DSPLIB_SUCCESS;
   DSPLIB_mat_submat_copy_PrivArgs *pKerPrivArgs = (DSPLIB_mat_submat_copy_PrivArgs *) handle;
   int32_t                          strideX      = pKerPrivArgs->strideX;
   int32_t                          strideY      = pKerPrivArgs->strideY;
   int32_t                          dir          = pKerPrivArgs->dir;
   int32_t                          stRow        = pKerPrivArgs->stRow;
   int32_t                          stCol        = pKerPrivArgs->stCol;
   int32_t                          nRows        = pKerPrivArgs->nRows;
   int32_t                          nCols        = pKerPrivArgs->nCols;
   int32_t                          colStrideX   = strideX / sizeof(dataType);
   int32_t                          colStrideY   = strideY / sizeof(dataType);

   /* Typecast void pointers to respective data type */
   dataType *yLocal = (dataType *) pY;
   dataType *xLocal = (dataType *) pX;

   DSPLIB_DEBUGPRINTFN(0, "yLocal: %p xLocal: %p nRows : %d nCols: %d stRow: %d stCol:%d dir: %d \n", yLocal, xLocal,
                       nRows, nCols, stRow, stCol, dir);
   uint32_t i, j;
   /* ------------------------------------------------------------------- */
   /*  Write each column of 'pXLocal' to a row of 'pYLocal'.           */
   /* ------------------------------------------------------------------- */
   for (i = stRow; i < (uint32_t) (stRow + nRows); i++) {
      for (j = stCol; j < (uint32_t) (stCol + nCols); j++) {
         if (dir == 0) {
            yLocal[(i - stRow) * colStrideY + (j - stCol)] = xLocal[i * colStrideX + j];
         }
         else {
            xLocal[i * colStrideX + j] = yLocal[(i - stRow) * colStrideY + (j - stCol)];
         }
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
template DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);
