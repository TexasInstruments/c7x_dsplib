// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_tableSourceRowColSelector_priv.h"

void DSPLIB_tableSourceRowColSelector_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   uint64_t tableSourceRowColSelectorStartupCycles   = 0;
   uint64_t tableSourceRowColSelectorTeardownCycles  = 0;
   uint64_t tableSourceRowColSelectorOperationCycles = 0;
   uint64_t tableSourceRowColSelectorOverheadCycles  = 0;

   DSPLIB_tableSourceRowColSelector_PrivArgs *pKerPrivArgs = (DSPLIB_tableSourceRowColSelector_PrivArgs *) handle;

   bool     isOutputInterleave = (pKerPrivArgs->isOutputInterleave == 1U);
   uint64_t numIndices         = (uint64_t) pKerPrivArgs->numIndices;

   /* Groups of four indices, then the leftover pair and single index */
   uint64_t numGroups  = numIndices / 4U;
   uint64_t pairTail   = (numIndices >> 1U) & 1U;
   uint64_t singleTail = numIndices & 1U;

   /* One vector stored per cycle */
   uint64_t numVecs = numIndices * (uint64_t) pKerPrivArgs->nVecs;
   uint64_t numTransposedVecs =
       isOutputInterleave ? ((uint64_t) pKerPrivArgs->selLen * (uint64_t) pKerPrivArgs->numBlocks) : 0U;

   tableSourceRowColSelectorStartupCycles = isOutputInterleave
                                                ? DSPLIB_TABLESOURCEROWCOLSELECTOR_INTERLEAVED_STARTUP_CYCLES
                                                : DSPLIB_TABLESOURCEROWCOLSELECTOR_NONINTERLEAVED_STARTUP_CYCLES;

   tableSourceRowColSelectorOperationCycles =
       (numGroups * DSPLIB_TABLESOURCEROWCOLSELECTOR_GROUP_CYCLES) +
       ((pairTail | singleTail) * DSPLIB_TABLESOURCEROWCOLSELECTOR_TAIL_CYCLES) +
       ((pairTail & singleTail) * DSPLIB_TABLESOURCEROWCOLSELECTOR_TAIL_PAIR_AND_SINGLE_CYCLES) + numVecs +
       numTransposedVecs;
   tableSourceRowColSelectorOverheadCycles =
       tableSourceRowColSelectorStartupCycles + tableSourceRowColSelectorTeardownCycles;
   *estCycles  = tableSourceRowColSelectorOperationCycles + tableSourceRowColSelectorOverheadCycles;
   *archCycles = tableSourceRowColSelectorOperationCycles;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_init_ci(DSPLIB_kernelHandle                              handle,
                                                       const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                                       const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                                       const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                                       const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_tableSourceRowColSelector_init_ci function");

   DSPLIB_STATUS                              status       = DSPLIB_SUCCESS;
   DSPLIB_tableSourceRowColSelector_PrivArgs *pKerPrivArgs = (DSPLIB_tableSourceRowColSelector_PrivArgs *) handle;
   uint8_t                                   *pBlock       = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   int32_t numRows    = pKerPrivArgs->numRows;
   int32_t numCols    = pKerPrivArgs->numCols;
   int32_t numIndices = pKerPrivArgs->numIndices;
   bool    rowSelect  = (pKerPrivArgs->isColSelect == 0U);
   int32_t selLen     = rowSelect ? numCols : numRows;

   /* Interleaved output: one scratch row per index */
   int32_t scratchStride = DSPLIB_ceilingDiv(selLen, eleCount) * eleCount;
   int32_t dstStride     = (pKerPrivArgs->isOutputInterleave == 1U) ? scratchStride : (int32_t) pKerPrivArgs->strideOut;

   pKerPrivArgs->selLen    = selLen;
   pKerPrivArgs->selMax    = rowSelect ? (numRows - 1) : (numCols - 1);
   pKerPrivArgs->selStride = rowSelect ? numCols : 1;
   pKerPrivArgs->numBlocks = DSPLIB_ceilingDiv(numIndices, eleCount);

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   if (rowSelect) {
      /* Row select: read the row linearly */
      se0Params.ICNT0 = selLen;
      se0Params.ICNT1 = 1;
      se0Params.ICNT2 = 1;
      se0Params.ICNT3 = 1;

      /* SA0 writes one output (or scratch) row per index */
      sa0Params.ICNT0  = selLen;
      sa0Params.ICNT1  = numIndices;
      sa0Params.DIM1   = dstStride;
      sa0Params.DIMFMT = __SA_DIMFMT_2D;

      pKerPrivArgs->nVecs = DSPLIB_ceilingDiv(selLen, eleCount);
   }
   else {
      /* Column select: read the column transposed. When numRows is not a multiple of eleCount, the column is read
       * as two sets, one from row 0 and one ending at the last row. */
      int32_t rowsPerVec   = (selLen < eleCount) ? selLen : eleCount;
      int32_t numSets      = ((selLen > eleCount) && ((selLen % eleCount) != 0)) ? 2 : 1;
      int32_t blocksPerSet = DSPLIB_ceilingDiv(DSPLIB_ceilingDiv(selLen, eleCount), numSets);
      int32_t setOffset    = selLen - (blocksPerSet * rowsPerVec);

      se0Params.TRANSPOSE = (sizeof(dataType) == 4U) ? __SE_TRANSPOSE_32BIT : __SE_TRANSPOSE_64BIT;
      se0Params.ICNT0     = 1;
      se0Params.ICNT1     = rowsPerVec;
      se0Params.DIM1      = numCols;
      se0Params.ICNT2     = blocksPerSet;
      se0Params.DIM2      = eleCount * numCols;
      se0Params.ICNT3     = numSets;
      se0Params.DIM3      = setOffset * numCols;

      /* SA0 writes one output (or scratch) row per index */
      sa0Params.ICNT0  = rowsPerVec;
      sa0Params.ICNT1  = blocksPerSet;
      sa0Params.DIM1   = eleCount;
      sa0Params.ICNT2  = numSets;
      sa0Params.DIM2   = setOffset;
      sa0Params.ICNT3  = numIndices;
      sa0Params.DIM3   = dstStride;
      sa0Params.DIMFMT = __SA_DIMFMT_4D;

      pKerPrivArgs->nVecs = blocksPerSet * numSets;
   }
   /* Each SE reads two indices; DIM4 is the jump between them, set per pair in exec */
   se0Params.ICNT4   = 2;
   se0Params.ELETYPE = c7x::se_eletype<vec>::value;
   se0Params.VECLEN  = c7x::se_veclen<vec>::value;
   se0Params.DIMFMT  = __SE_DIMFMT_5D;

   sa0Params.VECLEN = c7x::sa_veclen<vec>::value;

   if (pKerPrivArgs->isOutputInterleave == 1U) {
      int32_t numBlocks = pKerPrivArgs->numBlocks;

      __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
      __SA_TEMPLATE_v1 sa1Params = __gen_SA_TEMPLATE_v1();

      /* Read the scratch transposed, eleCount indices per vector */
      se1Params.TRANSPOSE = (sizeof(dataType) == 4U) ? __SE_TRANSPOSE_32BIT : __SE_TRANSPOSE_64BIT;
      se1Params.ICNT0     = selLen;
      se1Params.ICNT1     = (numIndices < eleCount) ? numIndices : eleCount;
      se1Params.DIM1      = scratchStride;
      se1Params.ICNT2     = numBlocks;
      se1Params.DIM2      = eleCount * scratchStride;
      se1Params.ELETYPE   = c7x::se_eletype<vec>::value;
      se1Params.VECLEN    = c7x::se_veclen<vec>::value;
      se1Params.DIMFMT    = __SE_DIMFMT_3D;

      /* Write eleCount indices per vector; DECDIM1 masks indices past numIndices */
      sa1Params.ICNT0         = eleCount;
      sa1Params.ICNT1         = selLen;
      sa1Params.DIM1          = (int32_t) pKerPrivArgs->strideOut;
      sa1Params.ICNT2         = numBlocks;
      sa1Params.DIM2          = eleCount;
      sa1Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa1Params.DECDIM1_WIDTH = (uint32_t) numIndices;
      sa1Params.VECLEN        = c7x::sa_veclen<vec>::value;
      sa1Params.DIMFMT        = __SA_DIMFMT_3D;

      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
   }

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

template DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_init_ci<float>(DSPLIB_kernelHandle                              handle,
                                                const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                                const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                                const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                                const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_init_ci<double>(DSPLIB_kernelHandle                              handle,
                                                 const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                                 const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                                 const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                                 const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_init_ci<int32_t>(DSPLIB_kernelHandle                              handle,
                                                  const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                                  const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                                  const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                                  const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_init_ci<int64_t>(DSPLIB_kernelHandle                              handle,
                                                  const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                                  const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                                  const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                                  const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_init_ci<uint32_t>(DSPLIB_kernelHandle                              handle,
                                                   const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                                   const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                                   const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                                   const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_init_ci<uint64_t>(DSPLIB_kernelHandle                              handle,
                                                   const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                                   const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                                   const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                                   const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

template <typename dataType, int32_t isOutputInterleave, int32_t pairTail, int32_t singleTail>
DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn0,
                                                       void *restrict pIn1,
                                                       void *restrict pOut,
                                                       void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_tableSourceRowColSelector_exec_ci function");

   DSPLIB_tableSourceRowColSelector_PrivArgs *pKerPrivArgs = (DSPLIB_tableSourceRowColSelector_PrivArgs *) handle;

   int32_t *restrict pInLocalIdx    = (int32_t *) pIn0;
   dataType *restrict pInLocalTable = (dataType *) pIn1;
   dataType *restrict pOutLocal     = (dataType *) pOut;
   dataType *restrict pScratchLocal = (dataType *) pScratch;

   DSPLIB_DEBUGPRINTFN(0, "pInLocalIdx: %p pInLocalTable: %p pOutLocal: %p\n", pInLocalIdx, pInLocalTable, pOutLocal);

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   /* Interleaved output goes through the scratch buffer first */
   dataType *restrict pDst = (isOutputInterleave == 0) ? pOutLocal : pScratchLocal;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   /* Each group of four indices: SE0 reads two and SE1 reads two.
    * Row and column select differ only in the stream template and selStride. */
   int32_t numGroups = pKerPrivArgs->numIndices >> 2;
   int32_t lastIdx   = pKerPrivArgs->numIndices - 1;
   int32_t nVecs     = pKerPrivArgs->nVecs;
   int32_t selMax    = pKerPrivArgs->selMax;
   int32_t selStride = pKerPrivArgs->selStride;

   __SE_TEMPLATE_v1 se1Params = se0Params;

   /* Copy the selected vector of each index to its output (or scratch) row */
   __SA0_OPEN(sa0Params);

   /* Offsets are computed one group ahead to hide the index loads. Positions past the end reuse the last index,
    * so after the groups offset0 to offset2 hold the leftover pair and single index. */
   int32_t offset0 = __min(__max(pInLocalIdx[0], 0), selMax) * selStride;
   int32_t offset1 = __min(__max(pInLocalIdx[__min(1, lastIdx)], 0), selMax) * selStride;
   int32_t offset2 = __min(__max(pInLocalIdx[__min(2, lastIdx)], 0), selMax) * selStride;
   int32_t offset3 = __min(__max(pInLocalIdx[__min(3, lastIdx)], 0), selMax) * selStride;

   for (int32_t groupCount = 0; groupCount < numGroups; groupCount++) {
      se0Params.DIM4 = offset1 - offset0;
      se1Params.DIM4 = offset3 - offset2;

      __SE0_OPEN(pInLocalTable + offset0, se0Params);
      __SE1_OPEN(pInLocalTable + offset2, se1Params);

      int32_t nextIdx = 4 * groupCount + 4;

      offset0 = __min(__max(pInLocalIdx[__min(nextIdx, lastIdx)], 0), selMax) * selStride;
      offset1 = __min(__max(pInLocalIdx[__min(nextIdx + 1, lastIdx)], 0), selMax) * selStride;
      offset2 = __min(__max(pInLocalIdx[__min(nextIdx + 2, lastIdx)], 0), selMax) * selStride;
      offset3 = __min(__max(pInLocalIdx[__min(nextIdx + 3, lastIdx)], 0), selMax) * selStride;

      /* ii = 1 (one store per cycle) for all types, here and in the loops below */
#pragma MUST_ITERATE(2, , 2)
      for (int32_t vecCount = 0; vecCount < 2 * nVecs; vecCount++) {
         vec data = c7x::strm_eng<0, vec>::get_adv();

         __vpred opWr    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *opWrPtr = c7x::strm_agen<0, vec>::get_adv(pDst);
         __vstore_pred(opWr, opWrPtr, data);
      }
      __SE0_CLOSE();

#pragma MUST_ITERATE(2, , 2)
      for (int32_t vecCount = 0; vecCount < 2 * nVecs; vecCount++) {
         vec data = c7x::strm_eng<1, vec>::get_adv();

         __vpred opWr    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *opWrPtr = c7x::strm_agen<0, vec>::get_adv(pDst);
         __vstore_pred(opWr, opWrPtr, data);
      }
      __SE1_CLOSE();
   }

   /* Leftover pair on SE0, leftover single index on SE1. Each loop below runs once or not at all. */
   se0Params.DIM4  = offset1 - offset0;
   se1Params.ICNT4 = 1;

   for (int32_t pairCount = 0; pairCount < pairTail; pairCount++) {
      __SE0_OPEN(pInLocalTable + offset0, se0Params);
   }
   for (int32_t singleCount = 0; singleCount < singleTail; singleCount++) {
      __SE1_OPEN(pInLocalTable + ((pairTail == 0) ? offset0 : offset2), se1Params);
   }

   for (int32_t pairCount = 0; pairCount < pairTail; pairCount++) {
#pragma MUST_ITERATE(2, , 2)
      for (int32_t vecCount = 0; vecCount < 2 * nVecs; vecCount++) {
         vec data = c7x::strm_eng<0, vec>::get_adv();

         __vpred opWr    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *opWrPtr = c7x::strm_agen<0, vec>::get_adv(pDst);
         __vstore_pred(opWr, opWrPtr, data);
      }
      __SE0_CLOSE();
   }
   for (int32_t singleCount = 0; singleCount < singleTail; singleCount++) {
#pragma MUST_ITERATE(1, , )
      for (int32_t vecCount = 0; vecCount < nVecs; vecCount++) {
         vec data = c7x::strm_eng<1, vec>::get_adv();

         __vpred opWr    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *opWrPtr = c7x::strm_agen<0, vec>::get_adv(pDst);
         __vstore_pred(opWr, opWrPtr, data);
      }
      __SE1_CLOSE();
   }

   __SA0_CLOSE();

   /* Interleaved output: read the scratch transposed into the output */
   for (int32_t passCount = 0; passCount < isOutputInterleave; passCount++) {
      int32_t numTransposedVecs = pKerPrivArgs->numBlocks * pKerPrivArgs->selLen;

      __SE_TEMPLATE_v1 seTransposeParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SA_TEMPLATE_v1 saTransposeParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

      __SE0_OPEN(pScratchLocal, seTransposeParams);
      __SA0_OPEN(saTransposeParams);

      /* ii = 1 (one store per cycle) */
#pragma MUST_ITERATE(1, , )
      for (int32_t vecCount = 0; vecCount < numTransposedVecs; vecCount++) {
         vec data = c7x::strm_eng<0, vec>::get_adv();

         __vpred opWr    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *opWrPtr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(opWr, opWrPtr, data);
      }

      __SA0_CLOSE();
      __SE0_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<float, 0, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut,
                                                                                void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<float, 0, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut,
                                                                                void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<float, 0, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut,
                                                                                void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<float, 0, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut,
                                                                                void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<float, 1, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut,
                                                                                void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<float, 1, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut,
                                                                                void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<float, 1, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut,
                                                                                void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<float, 1, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut,
                                                                                void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<double, 0, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut,
                                                                                 void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<double, 0, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut,
                                                                                 void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<double, 0, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut,
                                                                                 void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<double, 0, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut,
                                                                                 void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<double, 1, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut,
                                                                                 void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<double, 1, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut,
                                                                                 void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<double, 1, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut,
                                                                                 void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<double, 1, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut,
                                                                                 void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 0, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 0, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 0, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 0, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 1, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 1, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 1, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int32_t, 1, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 0, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 0, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 0, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 0, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 1, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 1, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 1, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<int64_t, 1, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut,
                                                                                  void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 0, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 0, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 0, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 0, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 1, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 1, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 1, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint32_t, 1, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 0, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 0, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 0, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 0, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 1, 0, 0>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 1, 0, 1>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 1, 1, 0>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci<uint64_t, 1, 1, 1>(DSPLIB_kernelHandle handle,
                                                                                   void *restrict pIn0,
                                                                                   void *restrict pIn1,
                                                                                   void *restrict pOut,
                                                                                   void *restrict pScratch);
