// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_nonZeroCheck_priv.h"

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA1_PARAM_OFFSET (SE_SA0_PARAM_OFFSET + SE_PARAM_SIZE)

void DSPLIB_nonZeroCheck_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   uint32_t dim_y    = pKerPrivArgs->dim_y;
   uint32_t eleCount = c7x::element_count_of<c7x::uchar_vec>::value / DSPLIB_sizeof(pKerPrivArgs->dataType);
   uint32_t numVecs  = DSPLIB_ceilingDiv(pKerPrivArgs->dim_x, eleCount);

   /* Modelled cycles of the nested exec loops with one and with two outputs
    * per stream: outer iterations x (inner count x ii + overhead). */
   uint64_t oneCycles;
   uint64_t twoCycles;

   if (pKerPrivArgs->axisType == DSPLIB_NONZEROCHECK_ROW) {
      /* Each outer iteration folds dim_y rows of 2 (or 4) column blocks. */
      oneCycles =
          (uint64_t) DSPLIB_ceilingDiv(numVecs, 2) *
          ((uint64_t) dim_y * DSPLIB_NONZEROCHECK_CYCLES_PER_ITERATION + DSPLIB_NONZEROCHECK_ROW_BLOCK_OVERHEAD_CYCLES);
      twoCycles = (uint64_t) DSPLIB_ceilingDiv(numVecs, 4) *
                  ((uint64_t) dim_y * DSPLIB_NONZEROCHECK_TWO_PER_STREAM_CYCLES_PER_ITERATION +
                   DSPLIB_NONZEROCHECK_ROW_TWO_BLOCKS_OVERHEAD_CYCLES);
   }
   else {
      /* Each outer iteration folds numVecs vectors of 2 (or 4) rows. */
      oneCycles =
          (uint64_t) DSPLIB_ceilingDiv(dim_y, 2) * ((uint64_t) numVecs * DSPLIB_NONZEROCHECK_CYCLES_PER_ITERATION +
                                                    DSPLIB_NONZEROCHECK_COLUMN_ROW_OVERHEAD_CYCLES);
      twoCycles = (uint64_t) DSPLIB_ceilingDiv(dim_y, 4) *
                  ((uint64_t) numVecs * DSPLIB_NONZEROCHECK_TWO_PER_STREAM_CYCLES_PER_ITERATION +
                   DSPLIB_NONZEROCHECK_COLUMN_TWO_ROWS_OVERHEAD_CYCLES);
   }

   /* The faster variant is the one DSPLIB_nonZeroCheck_init_ci sets up; two
    * rows per stream need at least two rows. */
   bool twoRowsFit            = (pKerPrivArgs->axisType == DSPLIB_NONZEROCHECK_ROW) || (dim_y >= 2u);
   pKerPrivArgs->twoPerStream = (twoRowsFit && (twoCycles < oneCycles)) ? 1u : 0u;

   *archCycles = (pKerPrivArgs->twoPerStream != 0u) ? twoCycles : oneCycles;
   *estCycles  = *archCycles + DSPLIB_NONZEROCHECK_CALL_OVERHEAD_CYCLES;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci(DSPLIB_kernelHandle                 handle,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                          const DSPLIB_bufParams1D_t         *bufParamsOut,
                                          const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                                          status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs                          *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint8_t                                               *pBlock   = pKerPrivArgs->bufPblock;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;

   uint32_t dim_x    = pKerPrivArgs->dim_x;
   uint32_t dim_y    = pKerPrivArgs->dim_y;
   int32_t  strideIn = pKerPrivArgs->strideIn;
   uint32_t numVecs  = DSPLIB_ceilingDiv(dim_x, eleCount);

   /* DSPLIB_nonZeroCheck_perfEst picks the faster exec variant from its cycle
    * model and records it in twoPerStream. */
   uint64_t archCycles;
   uint64_t estCycles;
   DSPLIB_nonZeroCheck_perfEst(handle, &archCycles, &estCycles);
   uint32_t twoPerStream = pKerPrivArgs->twoPerStream;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_nonZeroCheck_init_ci \n");
   DSPLIB_DEBUGPRINTFN(0, "dim_x %d dim_y %d strideIn %d eleCount %d twoPerStream %d\n", dim_x, dim_y, strideIn,
                       eleCount, twoPerStream);

   /**********************************************************************/
   /* Prepare SE/SA template                                             */
   /**********************************************************************/
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa1Params;

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;

   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   sa0Params.VECLEN  = SA_VECLEN;

   /* Two streams run in lockstep, each delivering one vector per cycle: SE0/SA0
    * produce the first half of the outputs and SE1/SA1 the second half. For an
    * odd output count the halves overlap by one output, which both streams
    * compute and store with the same value (the reduction is idempotent), so
    * both streams run the same number of iterations and no tail pass is
    * needed. With twoPerStream each stream produces two adjacent outputs per
    * outer iteration instead of one. */
   uint32_t numIterations;
   uint32_t perStream = twoPerStream + 1u;

   if (pKerPrivArgs->axisType == DSPLIB_NONZEROCHECK_ROW) {
      /* Each stream is a 3D stream over its column blocks: perStream blocks of a
       * row, then dim_y rows, then numIterations block groups. DECDIM1 deducts
       * DIM2 from DECDIM1_WIDTH each time the block group advances, so the SE
       * zero-pads the lanes past dim_x instead of reading them. Padded lanes
       * only reach output lanes that the SA predicate drops. */
      uint32_t numGroups = DSPLIB_ceilingDiv(numVecs, perStream);
      numIterations      = DSPLIB_ceilingDiv(numGroups, 2);

      pKerPrivArgs->secondHalfInOffset  = (numGroups - numIterations) * perStream * eleCount;
      pKerPrivArgs->secondHalfOutOffset = pKerPrivArgs->secondHalfInOffset;

      se0Params.ICNT0         = perStream * eleCount;
      se0Params.ICNT1         = dim_y;
      se0Params.DIM1          = strideIn;
      se0Params.ICNT2         = numIterations;
      se0Params.DIM2          = perStream * eleCount;
      se0Params.DIMFMT        = __SE_DIMFMT_3D;
      se0Params.DECDIM1       = __SE_DECDIM_DIM2;
      se0Params.DECDIM1_WIDTH = dim_x;

      se1Params               = se0Params;
      se1Params.DECDIM1_WIDTH = dim_x - pKerPrivArgs->secondHalfInOffset;

      sa0Params.ICNT0  = dim_x;
      sa0Params.DIMFMT = __SA_DIMFMT_1D;

      sa1Params       = sa0Params;
      sa1Params.ICNT0 = dim_x - pKerPrivArgs->secondHalfOutOffset;
   }
   else {
      /* Each stream is a 4D stream over its rows: one vector of perStream
       * adjacent rows, then numVecs vectors along the rows, then numIterations
       * row groups. DECDIM1 deducts DIM2 (= eleCount) from DECDIM1_WIDTH for
       * every vector along the rows and is reloaded for the next row group, so
       * the SE pads the lanes past dim_x of the last vector of every row. For
       * "all" a zero lane would read as a zero element, so pad with the maximum
       * value (never zero) instead; for "any" zero padding leaves the result
       * unchanged. */
      numIterations = DSPLIB_ceilingDiv(dim_y, 2 * perStream);

      pKerPrivArgs->secondHalfInOffset  = (dim_y - perStream * numIterations) * strideIn;
      pKerPrivArgs->secondHalfOutOffset = dim_y - perStream * numIterations;

      se0Params.ICNT0         = eleCount;
      se0Params.ICNT1         = perStream;
      se0Params.DIM1          = strideIn;
      se0Params.ICNT2         = numVecs;
      se0Params.DIM2          = eleCount;
      se0Params.ICNT3         = numIterations;
      se0Params.DIM3          = perStream * strideIn;
      se0Params.DIMFMT        = __SE_DIMFMT_4D;
      se0Params.DECDIM1       = __SE_DECDIM_DIM2;
      se0Params.DECDIM1_WIDTH = dim_x;

      if (pKerPrivArgs->opType == DSPLIB_NONZEROCHECK_ALL) {
         se0Params.FILLVAL = __SE_FILLVAL_MAX;
      }

      se1Params = se0Params;

      sa0Params.ICNT0  = perStream * numIterations;
      sa0Params.DIMFMT = __SA_DIMFMT_1D;
      sa0Params.VECLEN = __SA_VECLEN_1ELEM;

      sa1Params = sa0Params;
   }

   pKerPrivArgs->numVecs       = numVecs;
   pKerPrivArgs->numIterations = numIterations;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<uint8_t>(DSPLIB_kernelHandle                 handle,
                                                            const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                            const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                            const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<int8_t>(DSPLIB_kernelHandle                 handle,
                                                           const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                           const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                           const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<int16_t>(DSPLIB_kernelHandle                 handle,
                                                            const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                            const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                            const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<int32_t>(DSPLIB_kernelHandle                 handle,
                                                            const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                            const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                            const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<int64_t>(DSPLIB_kernelHandle                 handle,
                                                            const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                            const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                            const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<float>(DSPLIB_kernelHandle                 handle,
                                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                          const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                          const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<double>(DSPLIB_kernelHandle                 handle,
                                                           const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                           const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                           const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<uint16_t>(DSPLIB_kernelHandle                 handle,
                                                             const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                             const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                             const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<uint32_t>(DSPLIB_kernelHandle                 handle,
                                                             const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                             const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                             const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci<uint64_t>(DSPLIB_kernelHandle                 handle,
                                                             const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                             const DSPLIB_bufParams1D_t         *bufParamsOut,
                                                             const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

template <typename dataType>
DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   dataType *restrict pIn0Local  = (dataType *) pIn0;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOut1Local = pOutLocal + pKerPrivArgs->secondHalfOutOffset;

   uint32_t numIterations = pKerPrivArgs->numIterations;
   uint32_t dim_y         = pKerPrivArgs->dim_y;

   const vec zeroVec = (vec) 0;
   const vec oneVec  = (vec) 1;

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn0Local + pKerPrivArgs->secondHalfInOffset, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* Column reduction: per outer iteration each stream folds one block of
    * eleCount columns over all dim_y rows. A lane starts at one and is cleared
    * by the first zero element of its column, so it ends up holding the output
    * value. */
#pragma MUST_ITERATE(1, , )
   for (uint32_t blk = 0; blk < numIterations; blk++) {
      vec result0 = oneVec;
      vec result1 = oneVec;

      /* ii = 1 cycle per 2 vectors (one per stream) for every data type, C7504
       * and C7524, from the compiled .asm. Bound by the two streams (one
       * vector each per cycle); the compares use .L2/.S2, the selects
       * .M2/.C2. */
#pragma MUST_ITERATE(1, , )
      for (uint32_t i = 0; i < dim_y; i++) {
         vec in0 = c7x::strm_eng<0, vec>::get_adv();
         vec in1 = c7x::strm_eng<1, vec>::get_adv();
         result0 = __select(__cmp_eq_pred(in0, zeroVec), zeroVec, result0);
         result1 = __select(__cmp_eq_pred(in1, zeroVec), zeroVec, result1);
      }

      __vpred storePred0 = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec0 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(storePred0, pStoreVec0, result0);

      __vpred storePred1 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreVec1 = c7x::strm_agen<1, vec>::get_adv(pOut1Local);
      __vstore_pred(storePred1, pStoreVec1, result1);
   }

   __SA1_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SE0_CLOSE();

   return status;
}

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   dataType *restrict pIn0Local  = (dataType *) pIn0;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOut1Local = pOutLocal + pKerPrivArgs->secondHalfOutOffset;

   uint32_t numIterations = pKerPrivArgs->numIterations;
   uint32_t dim_y         = pKerPrivArgs->dim_y;

   const vec zeroVec = (vec) 0;
   const vec oneVec  = (vec) 1;

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn0Local + pKerPrivArgs->secondHalfInOffset, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* Column reduction: per outer iteration each stream folds two adjacent
    * blocks of eleCount columns over all dim_y rows, which halves the outer
    * iterations of DSPLIB_nonZeroCheck_all_row_exec_ci. Lanes as in that
    * variant. */
#pragma MUST_ITERATE(1, , )
   for (uint32_t blk = 0; blk < numIterations; blk++) {
      vec result00 = oneVec;
      vec result01 = oneVec;
      vec result10 = oneVec;
      vec result11 = oneVec;

      /* ii = 2 cycles per 4 vectors (two per stream) for every data type,
       * C7504 and C7524, from the compiled .asm. Bound by the two streams (two
       * vectors each) and by the four compares on .L2/.S2; the selects use
       * .M2/.C2. */
#pragma MUST_ITERATE(1, , )
      for (uint32_t i = 0; i < dim_y; i++) {
         vec in00 = c7x::strm_eng<0, vec>::get_adv();
         vec in01 = c7x::strm_eng<0, vec>::get_adv();
         vec in10 = c7x::strm_eng<1, vec>::get_adv();
         vec in11 = c7x::strm_eng<1, vec>::get_adv();
         result00 = __select(__cmp_eq_pred(in00, zeroVec), zeroVec, result00);
         result01 = __select(__cmp_eq_pred(in01, zeroVec), zeroVec, result01);
         result10 = __select(__cmp_eq_pred(in10, zeroVec), zeroVec, result10);
         result11 = __select(__cmp_eq_pred(in11, zeroVec), zeroVec, result11);
      }

      /* A block past dim_x gets an all-false SA predicate and is not stored. */
      __vpred storePred = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(storePred, pStoreVec, result00);

      storePred = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(storePred, pStoreVec, result01);

      storePred = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv(pOut1Local);
      __vstore_pred(storePred, pStoreVec, result10);

      storePred = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv(pOut1Local);
      __vstore_pred(storePred, pStoreVec, result11);
   }

   __SA1_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SE0_CLOSE();

   return status;
}

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<uint8_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<int64_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<uint16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<uint32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci<uint64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   dataType *restrict pIn0Local  = (dataType *) pIn0;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOut1Local = pOutLocal + pKerPrivArgs->secondHalfOutOffset;

   uint32_t numIterations = pKerPrivArgs->numIterations;
   uint32_t numVecs       = pKerPrivArgs->numVecs;

   const vec zeroVec = (vec) 0;
   const vec oneVec  = (vec) 1;

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn0Local + pKerPrivArgs->secondHalfInOffset, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* Row reduction: per outer iteration each stream folds one row across its
    * dim_x columns. A lane starts at one and is cleared by the first zero
    * element; the SE pads the partial last vector of a row with a non-zero
    * fill value, so the padding never clears a lane. The row is non-zero only
    * if no lane was cleared. */
#pragma MUST_ITERATE(1, , )
   for (uint32_t row = 0; row < numIterations; row++) {
      vec result0 = oneVec;
      vec result1 = oneVec;

      /* ii = 1 cycle per 2 vectors (one per stream) for every data type, C7504
       * and C7524, from the compiled .asm. Bound by the two streams (one
       * vector each per cycle); the compares use .L2/.S2, the selects
       * .M2/.C2. */
#pragma MUST_ITERATE(1, , )
      for (uint32_t i = 0; i < numVecs; i++) {
         vec in0 = c7x::strm_eng<0, vec>::get_adv();
         vec in1 = c7x::strm_eng<1, vec>::get_adv();
         result0 = __select(__cmp_eq_pred(in0, zeroVec), zeroVec, result0);
         result1 = __select(__cmp_eq_pred(in1, zeroVec), zeroVec, result1);
      }

      dataType out0 = (dataType) (__popcount_char(__cmp_eq_pred(result0, zeroVec)) == 0u);
      dataType out1 = (dataType) (__popcount_char(__cmp_eq_pred(result1, zeroVec)) == 0u);

      dataType *pStore = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);
      *pStore          = out0;
      pStore           = c7x::strm_agen<1, dataType>::get_adv(pOut1Local);
      *pStore          = out1;
   }

   __SA1_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SE0_CLOSE();

   return status;
}

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   dataType *restrict pIn0Local  = (dataType *) pIn0;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOut1Local = pOutLocal + pKerPrivArgs->secondHalfOutOffset;

   uint32_t numIterations = pKerPrivArgs->numIterations;
   uint32_t numVecs       = pKerPrivArgs->numVecs;

   const vec zeroVec = (vec) 0;
   const vec oneVec  = (vec) 1;

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn0Local + pKerPrivArgs->secondHalfInOffset, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* Row reduction: per outer iteration each stream folds two adjacent rows,
    * which halves the outer iterations of
    * DSPLIB_nonZeroCheck_all_column_exec_ci. The SE delivers vector i of the
    * first row, then vector i of the second row. Lanes and padding as in that
    * variant. */
#pragma MUST_ITERATE(1, , )
   for (uint32_t row = 0; row < numIterations; row++) {
      vec result00 = oneVec;
      vec result01 = oneVec;
      vec result10 = oneVec;
      vec result11 = oneVec;

      /* ii = 2 cycles per 4 vectors (two per stream) for every data type,
       * C7504 and C7524, from the compiled .asm. Bound by the two streams (two
       * vectors each) and by the four compares on .L2/.S2; the selects use
       * .M2/.C2. */
#pragma MUST_ITERATE(1, , )
      for (uint32_t i = 0; i < numVecs; i++) {
         vec in00 = c7x::strm_eng<0, vec>::get_adv();
         vec in01 = c7x::strm_eng<0, vec>::get_adv();
         vec in10 = c7x::strm_eng<1, vec>::get_adv();
         vec in11 = c7x::strm_eng<1, vec>::get_adv();
         result00 = __select(__cmp_eq_pred(in00, zeroVec), zeroVec, result00);
         result01 = __select(__cmp_eq_pred(in01, zeroVec), zeroVec, result01);
         result10 = __select(__cmp_eq_pred(in10, zeroVec), zeroVec, result10);
         result11 = __select(__cmp_eq_pred(in11, zeroVec), zeroVec, result11);
      }

      dataType out00 = (dataType) (__popcount_char(__cmp_eq_pred(result00, zeroVec)) == 0u);
      dataType out01 = (dataType) (__popcount_char(__cmp_eq_pred(result01, zeroVec)) == 0u);
      dataType out10 = (dataType) (__popcount_char(__cmp_eq_pred(result10, zeroVec)) == 0u);
      dataType out11 = (dataType) (__popcount_char(__cmp_eq_pred(result11, zeroVec)) == 0u);

      dataType *pStore = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);
      *pStore          = out00;
      pStore           = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);
      *pStore          = out01;
      pStore           = c7x::strm_agen<1, dataType>::get_adv(pOut1Local);
      *pStore          = out10;
      pStore           = c7x::strm_agen<1, dataType>::get_adv(pOut1Local);
      *pStore          = out11;
   }

   __SA1_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SE0_CLOSE();

   return status;
}

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<uint8_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<int64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<uint16_t>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<uint32_t>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci<uint64_t>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   dataType *restrict pIn0Local  = (dataType *) pIn0;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOut1Local = pOutLocal + pKerPrivArgs->secondHalfOutOffset;

   uint32_t numIterations = pKerPrivArgs->numIterations;
   uint32_t dim_y         = pKerPrivArgs->dim_y;

   const vec zeroVec = (vec) 0;
   const vec oneVec  = (vec) 1;

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn0Local + pKerPrivArgs->secondHalfInOffset, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* Column reduction: per outer iteration each stream folds one block of
    * eleCount columns over all dim_y rows. A lane starts at one and is cleared
    * by the first non-zero element of its column, so the output is its
    * complement. The loop selects against zero only, as "all" does: a 64-bit
    * one is not a VSEL immediate and would be rebuilt in every iteration. */
#pragma MUST_ITERATE(1, , )
   for (uint32_t blk = 0; blk < numIterations; blk++) {
      vec allZero0 = oneVec;
      vec allZero1 = oneVec;

      /* ii = 1 cycle per 2 vectors (one per stream) for every data type, C7504
       * and C7524, from the compiled .asm. Bound by the two streams (one
       * vector each per cycle); the compares use .L2/.S2, the selects
       * .M2/.C2. */
#pragma MUST_ITERATE(1, , )
      for (uint32_t i = 0; i < dim_y; i++) {
         vec in0  = c7x::strm_eng<0, vec>::get_adv();
         vec in1  = c7x::strm_eng<1, vec>::get_adv();
         allZero0 = __select(__cmp_eq_pred(in0, zeroVec), allZero0, zeroVec);
         allZero1 = __select(__cmp_eq_pred(in1, zeroVec), allZero1, zeroVec);
      }

      __vpred storePred0 = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec0 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(storePred0, pStoreVec0, oneVec - allZero0);

      __vpred storePred1 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreVec1 = c7x::strm_agen<1, vec>::get_adv(pOut1Local);
      __vstore_pred(storePred1, pStoreVec1, oneVec - allZero1);
   }

   __SA1_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SE0_CLOSE();

   return status;
}

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   dataType *restrict pIn0Local  = (dataType *) pIn0;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOut1Local = pOutLocal + pKerPrivArgs->secondHalfOutOffset;

   uint32_t numIterations = pKerPrivArgs->numIterations;
   uint32_t dim_y         = pKerPrivArgs->dim_y;

   const vec zeroVec = (vec) 0;
   const vec oneVec  = (vec) 1;

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn0Local + pKerPrivArgs->secondHalfInOffset, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* Column reduction: per outer iteration each stream folds two adjacent
    * blocks of eleCount columns over all dim_y rows, which halves the outer
    * iterations of DSPLIB_nonZeroCheck_any_row_exec_ci. Lanes as in that
    * variant. */
#pragma MUST_ITERATE(1, , )
   for (uint32_t blk = 0; blk < numIterations; blk++) {
      vec allZero00 = oneVec;
      vec allZero01 = oneVec;
      vec allZero10 = oneVec;
      vec allZero11 = oneVec;

      /* ii = 2 cycles per 4 vectors (two per stream) for every data type,
       * C7504 and C7524, from the compiled .asm. Bound by the two streams (two
       * vectors each) and by the four compares on .L2/.S2; the selects use
       * .M2/.C2. */
#pragma MUST_ITERATE(1, , )
      for (uint32_t i = 0; i < dim_y; i++) {
         vec in00  = c7x::strm_eng<0, vec>::get_adv();
         vec in01  = c7x::strm_eng<0, vec>::get_adv();
         vec in10  = c7x::strm_eng<1, vec>::get_adv();
         vec in11  = c7x::strm_eng<1, vec>::get_adv();
         allZero00 = __select(__cmp_eq_pred(in00, zeroVec), allZero00, zeroVec);
         allZero01 = __select(__cmp_eq_pred(in01, zeroVec), allZero01, zeroVec);
         allZero10 = __select(__cmp_eq_pred(in10, zeroVec), allZero10, zeroVec);
         allZero11 = __select(__cmp_eq_pred(in11, zeroVec), allZero11, zeroVec);
      }

      /* A block past dim_x gets an all-false SA predicate and is not stored. */
      __vpred storePred = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(storePred, pStoreVec, oneVec - allZero00);

      storePred = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(storePred, pStoreVec, oneVec - allZero01);

      storePred = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv(pOut1Local);
      __vstore_pred(storePred, pStoreVec, oneVec - allZero10);

      storePred = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv(pOut1Local);
      __vstore_pred(storePred, pStoreVec, oneVec - allZero11);
   }

   __SA1_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SE0_CLOSE();

   return status;
}

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<uint8_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<int64_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                            void *restrict pIn0,
                                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<uint16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<uint32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci<uint64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   dataType *restrict pIn0Local  = (dataType *) pIn0;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOut1Local = pOutLocal + pKerPrivArgs->secondHalfOutOffset;

   uint32_t numIterations = pKerPrivArgs->numIterations;
   uint32_t numVecs       = pKerPrivArgs->numVecs;

   const vec zeroVec = (vec) 0;
   const vec oneVec  = (vec) 1;

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn0Local + pKerPrivArgs->secondHalfInOffset, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* Row reduction: per outer iteration each stream folds one row across its
    * dim_x columns. A lane starts at one and is cleared by the first non-zero
    * element; the SE zero-pads the partial last vector of a row, so the
    * padding never clears a lane. The row is non-zero if any lane was
    * cleared. */
#pragma MUST_ITERATE(1, , )
   for (uint32_t row = 0; row < numIterations; row++) {
      vec allZero0 = oneVec;
      vec allZero1 = oneVec;

      /* ii = 1 cycle per 2 vectors (one per stream) for every data type, C7504
       * and C7524, from the compiled .asm. Bound by the two streams (one
       * vector each per cycle); the compares use .L2/.S2, the selects
       * .M2/.C2. */
#pragma MUST_ITERATE(1, , )
      for (uint32_t i = 0; i < numVecs; i++) {
         vec in0  = c7x::strm_eng<0, vec>::get_adv();
         vec in1  = c7x::strm_eng<1, vec>::get_adv();
         allZero0 = __select(__cmp_eq_pred(in0, zeroVec), allZero0, zeroVec);
         allZero1 = __select(__cmp_eq_pred(in1, zeroVec), allZero1, zeroVec);
      }

      dataType out0 = (dataType) (__popcount_char(__cmp_eq_pred(allZero0, zeroVec)) != 0u);
      dataType out1 = (dataType) (__popcount_char(__cmp_eq_pred(allZero1, zeroVec)) != 0u);

      dataType *pStore = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);
      *pStore          = out0;
      pStore           = c7x::strm_agen<1, dataType>::get_adv(pOut1Local);
      *pStore          = out1;
   }

   __SA1_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SE0_CLOSE();

   return status;
}

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_nonZeroCheck_PrivArgs *pKerPrivArgs = (DSPLIB_nonZeroCheck_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   dataType *restrict pIn0Local  = (dataType *) pIn0;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOut1Local = pOutLocal + pKerPrivArgs->secondHalfOutOffset;

   uint32_t numIterations = pKerPrivArgs->numIterations;
   uint32_t numVecs       = pKerPrivArgs->numVecs;

   const vec zeroVec = (vec) 0;
   const vec oneVec  = (vec) 1;

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn0Local + pKerPrivArgs->secondHalfInOffset, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* Row reduction: per outer iteration each stream folds two adjacent rows,
    * which halves the outer iterations of
    * DSPLIB_nonZeroCheck_any_column_exec_ci. The SE delivers vector i of the
    * first row, then vector i of the second row. Lanes and padding as in that
    * variant. */
#pragma MUST_ITERATE(1, , )
   for (uint32_t row = 0; row < numIterations; row++) {
      vec allZero00 = oneVec;
      vec allZero01 = oneVec;
      vec allZero10 = oneVec;
      vec allZero11 = oneVec;

      /* ii = 2 cycles per 4 vectors (two per stream) for every data type,
       * C7504 and C7524, from the compiled .asm. Bound by the two streams (two
       * vectors each) and by the four compares on .L2/.S2; the selects use
       * .M2/.C2. */
#pragma MUST_ITERATE(1, , )
      for (uint32_t i = 0; i < numVecs; i++) {
         vec in00  = c7x::strm_eng<0, vec>::get_adv();
         vec in01  = c7x::strm_eng<0, vec>::get_adv();
         vec in10  = c7x::strm_eng<1, vec>::get_adv();
         vec in11  = c7x::strm_eng<1, vec>::get_adv();
         allZero00 = __select(__cmp_eq_pred(in00, zeroVec), allZero00, zeroVec);
         allZero01 = __select(__cmp_eq_pred(in01, zeroVec), allZero01, zeroVec);
         allZero10 = __select(__cmp_eq_pred(in10, zeroVec), allZero10, zeroVec);
         allZero11 = __select(__cmp_eq_pred(in11, zeroVec), allZero11, zeroVec);
      }

      dataType out00 = (dataType) (__popcount_char(__cmp_eq_pred(allZero00, zeroVec)) != 0u);
      dataType out01 = (dataType) (__popcount_char(__cmp_eq_pred(allZero01, zeroVec)) != 0u);
      dataType out10 = (dataType) (__popcount_char(__cmp_eq_pred(allZero10, zeroVec)) != 0u);
      dataType out11 = (dataType) (__popcount_char(__cmp_eq_pred(allZero11, zeroVec)) != 0u);

      dataType *pStore = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);
      *pStore          = out00;
      pStore           = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);
      *pStore          = out01;
      pStore           = c7x::strm_agen<1, dataType>::get_adv(pOut1Local);
      *pStore          = out10;
      pStore           = c7x::strm_agen<1, dataType>::get_adv(pOut1Local);
      *pStore          = out11;
   }

   __SA1_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SE0_CLOSE();

   return status;
}

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<uint8_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<int64_t>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                             void *restrict pIn0,
                                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                                              void *restrict pIn0,
                                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<uint16_t>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<uint32_t>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pOut);

template DSPLIB_STATUS DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci<uint64_t>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pOut);
