// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_tableSourceRowColSelector_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_cn(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn0,
                                                       void *restrict pIn1,
                                                       void *restrict pOut,
                                                       void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS                              status       = DSPLIB_SUCCESS;
   DSPLIB_tableSourceRowColSelector_PrivArgs *pKerPrivArgs = (DSPLIB_tableSourceRowColSelector_PrivArgs *) handle;

   int32_t *restrict pInLocalIdx    = (int32_t *) pIn0;
   dataType *restrict pInLocalTable = (dataType *) pIn1;
   dataType *restrict pOutLocal     = (dataType *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "pInLocalIdx: %p pInLocalTable: %p pOutLocal: %p\n", pInLocalIdx, pInLocalTable, pOutLocal);

   int32_t  numRows            = pKerPrivArgs->numRows;
   int32_t  numCols            = pKerPrivArgs->numCols;
   int32_t  numIndices         = pKerPrivArgs->numIndices;
   uint32_t strideOut          = pKerPrivArgs->strideOut;
   uint8_t  isColSelect        = pKerPrivArgs->isColSelect;
   uint8_t  isOutputInterleave = pKerPrivArgs->isOutputInterleave;

   int32_t numSel = (isColSelect == 0U) ? numRows : numCols;
   int32_t selLen = (isColSelect == 0U) ? numCols : numRows;

   /* Table element: sel * selStride + selEleCount * eleStride */
   int32_t selStride = (isColSelect == 0U) ? numCols : 1;
   int32_t eleStride = (isColSelect == 0U) ? 1 : numCols;

   /* Output element: idxCount * outIdxStride + selEleCount * outEleStride */
   int32_t outIdxStride = (isOutputInterleave == 1U) ? 1 : (int32_t) strideOut;
   int32_t outEleStride = (isOutputInterleave == 1U) ? (int32_t) strideOut : 1;

   for (int32_t idxCount = 0; idxCount < numIndices; idxCount++) {
      int32_t sel = pInLocalIdx[idxCount];

      if (sel < 0) {
         sel = 0;
      }
      else if (sel > (numSel - 1)) {
         sel = numSel - 1;
      }
      else {
         /* Index within range */
      }

      for (int32_t selEleCount = 0; selEleCount < selLen; selEleCount++) {
         pOutLocal[idxCount * outIdxStride + selEleCount * outEleStride] =
             pInLocalTable[sel * selStride + selEleCount * eleStride];
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

// explicit instantiation for the different data type versions

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                                       void *restrict pIn0,
                                                                       void *restrict pIn1,
                                                                       void *restrict pOut,
                                                                       void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                                        void *restrict pIn0,
                                                                        void *restrict pIn1,
                                                                        void *restrict pOut,
                                                                        void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_cn<int32_t>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn0,
                                                                         void *restrict pIn1,
                                                                         void *restrict pOut,
                                                                         void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_cn<int64_t>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn0,
                                                                         void *restrict pIn1,
                                                                         void *restrict pOut,
                                                                         void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_cn<uint32_t>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn0,
                                                                          void *restrict pIn1,
                                                                          void *restrict pOut,
                                                                          void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_cn<uint64_t>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn0,
                                                                          void *restrict pIn1,
                                                                          void *restrict pOut,
                                                                          void *restrict pScratch);
