// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/DSPLIB_inlines.h"
#include "DSPLIB_peakAbsoluteValue_priv.h"

/* Absolute value of every lane. Integers use the saturating __abs. float and double clear the sign bit (bit-identical
 * to VABSSP/VABSDP, but it also runs on the .M unit, which gives ii = 2 instead of 3 in the multi-iteration loop). */
template <typename vec> static inline vec DSPLIB_peakAbsoluteValue_absLanes(vec inVec) { return __abs(inVec); }

static inline c7x::float_vec DSPLIB_peakAbsoluteValue_absLanes(c7x::float_vec inVec)
{
   return c7x::reinterpret<c7x::float_vec>(c7x::reinterpret<c7x::uint_vec>(inVec) & (c7x::uint_vec) 0x7FFFFFFFu);
}

static inline c7x::double_vec DSPLIB_peakAbsoluteValue_absLanes(c7x::double_vec inVec)
{
   return c7x::reinterpret<c7x::double_vec>(c7x::reinterpret<c7x::ulong_vec>(inVec) &
                                            (c7x::ulong_vec) 0x7FFFFFFFFFFFFFFFull);
}

/* Horizontal maximum of unsigned lanes. For 8-bit and 16-bit lanes the bytes (halfwords) inside each 32-bit lane are
 * folded first, so only 32-bit lanes are sorted (the host-emulation sort of 16/32 lanes reads uninitialised values). */
template <typename unsignedVec> static inline uint64_t DSPLIB_peakAbsoluteValue_horizontalMax(unsignedVec inVec)
{
   return c7x_horizontal_max(inVec);
}

static inline uint64_t DSPLIB_peakAbsoluteValue_horizontalMax(c7x::uchar_vec inVec)
{
   c7x::uint_vec words = c7x::reinterpret<c7x::uint_vec>(inVec);
   words               = c7x::reinterpret<c7x::uint_vec>(
       __max(c7x::reinterpret<c7x::uchar_vec>(words), c7x::reinterpret<c7x::uchar_vec>(words >> 8)));
   words = c7x::reinterpret<c7x::uint_vec>(
       __max(c7x::reinterpret<c7x::uchar_vec>(words), c7x::reinterpret<c7x::uchar_vec>(words >> 16)));
   return c7x_horizontal_max(words & (c7x::uint_vec) 0xFFu);
}

static inline uint64_t DSPLIB_peakAbsoluteValue_horizontalMax(c7x::ushort_vec inVec)
{
   c7x::uint_vec words = c7x::reinterpret<c7x::uint_vec>(inVec);
   words               = c7x::reinterpret<c7x::uint_vec>(
       __max(c7x::reinterpret<c7x::ushort_vec>(words), c7x::reinterpret<c7x::ushort_vec>(words >> 16)));
   return c7x_horizontal_max(words & (c7x::uint_vec) 0xFFFFu);
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_ci(DSPLIB_kernelHandle                      handle,
                                               const DSPLIB_bufParams2D_t              *bufParamsIn,
                                               const DSPLIB_bufParams2D_t              *bufParamsOut,
                                               const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_peakAbsoluteValue_PrivArgs *pKerPrivArgs = (DSPLIB_peakAbsoluteValue_PrivArgs *) handle;
   uint8_t                           *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                            width        = pKerPrivArgs->width;
   int32_t                            height       = pKerPrivArgs->height;
   int32_t                            strideIn     = pKerPrivArgs->strideInElements;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   /* Stream 0 reads the start of every row and stream 1 the end of it, vecsPerStream vectors each (1 for rows of up to
    * 2 vectors, otherwise a multiple of 2). Together they cover the row; the overlap is harmless for a maximum. */
   int32_t numVecs        = (width + eleCount - 1) / eleCount;
   int32_t vecsPerStream  = (numVecs <= 2) ? 1 : 2 * ((numVecs + 3) / 4);
   int32_t streamElements = (vecsPerStream * eleCount < width) ? vecsPerStream * eleCount : width;

   pKerPrivArgs->vecsPerStream = vecsPerStream;
   pKerPrivArgs->numIterations = (vecsPerStream + 1) / 2;
   pKerPrivArgs->stream1Offset = width - streamElements;

   /* Both streams share one 2D template: ICNT0 = elements per stream per row (the SE zero-fills the last vector of a
    * row, neutral for a maximum of absolute values), ICNT1 = rows, DIM1 = row stride. Stream 1 opens later in the row.
    */
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();

   se0Params.ICNT0   = streamElements;
   se0Params.ICNT1   = height;
   se0Params.DIM1    = strideIn;
   se0Params.ELETYPE = c7x::se_eletype<vec>::value;
   se0Params.VECLEN  = c7x::se_veclen<vec>::value;
   se0Params.DIMFMT  = __SE_DIMFMT_2D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se0Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_ci<int8_t>(DSPLIB_kernelHandle                      handle,
                                                                const DSPLIB_bufParams2D_t              *bufParamsIn,
                                                                const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                                const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_ci<int16_t>(DSPLIB_kernelHandle                      handle,
                                                                 const DSPLIB_bufParams2D_t              *bufParamsIn,
                                                                 const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                                 const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_ci<int64_t>(DSPLIB_kernelHandle                      handle,
                                                                 const DSPLIB_bufParams2D_t              *bufParamsIn,
                                                                 const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                                 const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_ci<int32_t>(DSPLIB_kernelHandle                      handle,
                                                                 const DSPLIB_bufParams2D_t              *bufParamsIn,
                                                                 const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                                 const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_ci<float>(DSPLIB_kernelHandle                      handle,
                                                               const DSPLIB_bufParams2D_t              *bufParamsIn,
                                                               const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                               const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_ci<double>(DSPLIB_kernelHandle                      handle,
                                                                const DSPLIB_bufParams2D_t              *bufParamsIn,
                                                                const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                                const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

/* Peak (vector maximum of the absolute values) of one row of at most 4 vectors: vecsPerStream vectors from each stream.
 * The first pair of vectors starts the maximum (no zero vector), the remaining pair is folded in. */
template <typename vec, int32_t vecsPerStream> static inline vec DSPLIB_peakAbsoluteValue_rowPeak()
{
   vec headVec    = c7x::strm_eng<0, vec>::get_adv();
   vec tailVec    = c7x::strm_eng<1, vec>::get_adv();
   vec peakAbsVec = __max(DSPLIB_peakAbsoluteValue_absLanes(headVec), DSPLIB_peakAbsoluteValue_absLanes(tailVec));

   for (int32_t vecIdx = 1; vecIdx < vecsPerStream; vecIdx++) {
      headVec    = c7x::strm_eng<0, vec>::get_adv();
      tailVec    = c7x::strm_eng<1, vec>::get_adv();
      peakAbsVec = __max(peakAbsVec,
                         __max(DSPLIB_peakAbsoluteValue_absLanes(headVec), DSPLIB_peakAbsoluteValue_absLanes(tailVec)));
   }

   return peakAbsVec;
}

/* Rows of at most 4 vectors (1 or 2 vectors per stream): each loop iteration processes two complete rows, so the
 * units left idle by one row are filled by the other and consecutive iterations overlap in the software pipeline. For
 * an odd number of rows (oddHeight = 1) the first row is processed on its own before the loop. */
template <typename dataType, typename unsignedType, int32_t vecsPerStream, int32_t oddHeight>
DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_peakAbsoluteValue_PrivArgs *pKerPrivArgs  = (DSPLIB_peakAbsoluteValue_PrivArgs *) handle;
   int32_t                            numRowPairs   = pKerPrivArgs->height / 2;
   int32_t                            stream1Offset = pKerPrivArgs->stream1Offset;

   dataType *restrict pInLocal      = (dataType *) pIn;
   unsignedType *restrict pOutLocal = (unsignedType *) pOut;

   typedef typename c7x::make_full_vector<dataType>::type     vec;
   typedef typename c7x::make_full_vector<unsignedType>::type unsignedVec;

   uint8_t         *pBlock    = pKerPrivArgs->bufPblock;
   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + stream1Offset, se1Params);

   /* oddHeight is 0 or 1: the loop runs once for an odd number of rows and not at all for an even one */
   for (int32_t row = 0; row < oddHeight; row++) {
      vec peakAbsVec = DSPLIB_peakAbsoluteValue_rowPeak<vec, vecsPerStream>();
      pOutLocal[row] = (unsignedType) DSPLIB_peakAbsoluteValue_horizontalMax(c7x::reinterpret<unsignedVec>(peakAbsVec));
   }

   /* ii = cycles per iteration (two rows), cycles per vector in brackets. From the compiled .asm; C7504 and C7524 are
    * identical and the odd and even height versions have the same ii.
    *
    * +--------------------+-----------+------------+------------+------------+---------+-----------+
    * | Vectors per stream | 8-bit int | 16-bit int | 32-bit int | 64-bit int | float   | double    |
    * +--------------------+-----------+------------+------------+------------+---------+-----------+
    * | 1 (4 vectors)      | 5 (1.25)  | 4 (1.0)    | 2 (0.5)    | 6 (1.5)    | 2 (0.5) | 5 (1.25)  |
    * | 2 (8 vectors)      | 7 (0.875) | 6 (0.75)   | 5 (0.625)  | 7 (0.875)  | 4 (0.5) | 7 (0.875) |
    * +--------------------+-----------+------------+------------+------------+---------+-----------+
    */
   for (int32_t rowPair = 0; rowPair < numRowPairs; rowPair++) {
      vec peakAbsVecA = DSPLIB_peakAbsoluteValue_rowPeak<vec, vecsPerStream>();
      vec peakAbsVecB = DSPLIB_peakAbsoluteValue_rowPeak<vec, vecsPerStream>();

      pOutLocal[oddHeight + 2 * rowPair] =
          (unsignedType) DSPLIB_peakAbsoluteValue_horizontalMax(c7x::reinterpret<unsignedVec>(peakAbsVecA));
      pOutLocal[oddHeight + 2 * rowPair + 1] =
          (unsignedType) DSPLIB_peakAbsoluteValue_horizontalMax(c7x::reinterpret<unsignedVec>(peakAbsVecB));
   }

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/* Rows of 5 or more vectors: an inner loop reads 2 vectors from each stream per iteration into four running maxima,
 * and the horizontal maximum runs once per row. unsignedType has the same size as dataType; the absolute values are
 * non-negative, so the unsigned horizontal maximum is also exact for float and double (bit patterns are ordered). */
template <typename dataType, typename unsignedType>
DSPLIB_STATUS DSPLIB_peakAbsoluteValue_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_peakAbsoluteValue_PrivArgs *pKerPrivArgs  = (DSPLIB_peakAbsoluteValue_PrivArgs *) handle;
   int32_t                            numIterations = pKerPrivArgs->numIterations;
   int32_t                            height        = pKerPrivArgs->height;
   int32_t                            stream1Offset = pKerPrivArgs->stream1Offset;

   dataType *restrict pInLocal      = (dataType *) pIn;
   unsignedType *restrict pOutLocal = (unsignedType *) pOut;

   typedef typename c7x::make_full_vector<dataType>::type     vec;
   typedef typename c7x::make_full_vector<unsignedType>::type unsignedVec;

   uint8_t         *pBlock    = pKerPrivArgs->bufPblock;
   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + stream1Offset, se1Params);

#pragma MUST_ITERATE(1, , 1)
   for (int32_t row = 0; row < height; row++) {
      vec peakAbsVecA = (vec) 0;
      vec peakAbsVecB = (vec) 0;
      vec peakAbsVecC = (vec) 0;
      vec peakAbsVecD = (vec) 0;

      /* 0.5 cycles per vector for all six types on C7504 and C7524 (from the compiled .asm): the two streaming
       * engines deliver one vector per cycle each, and four accumulators hide the max latency. */
#pragma MUST_ITERATE(2, , 1)
      for (int32_t iterIdx = 0; iterIdx < numIterations; iterIdx++) {
         vec headVec0 = c7x::strm_eng<0, vec>::get_adv();
         vec tailVec0 = c7x::strm_eng<1, vec>::get_adv();
         vec headVec1 = c7x::strm_eng<0, vec>::get_adv();
         vec tailVec1 = c7x::strm_eng<1, vec>::get_adv();
         peakAbsVecA  = __max(DSPLIB_peakAbsoluteValue_absLanes(headVec0), peakAbsVecA);
         peakAbsVecB  = __max(DSPLIB_peakAbsoluteValue_absLanes(tailVec0), peakAbsVecB);
         peakAbsVecC  = __max(DSPLIB_peakAbsoluteValue_absLanes(headVec1), peakAbsVecC);
         peakAbsVecD  = __max(DSPLIB_peakAbsoluteValue_absLanes(tailVec1), peakAbsVecD);
      }

      vec peakAbsVec = __max(__max(peakAbsVecA, peakAbsVecB), __max(peakAbsVecC, peakAbsVecD));

      pOutLocal[row] = (unsignedType) DSPLIB_peakAbsoluteValue_horizontalMax(c7x::reinterpret<unsignedVec>(peakAbsVec));
   }

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_ci<int8_t, uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS DSPLIB_peakAbsoluteValue_exec_ci<int16_t, uint16_t>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn,
                                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_peakAbsoluteValue_exec_ci<int32_t, uint32_t>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn,
                                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_peakAbsoluteValue_exec_ci<int64_t, uint64_t>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn,
                                                                           void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_ci<float, uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_ci<double, uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int8_t, uint8_t, 1, 0>(DSPLIB_kernelHandle handle,
                                                                        void *restrict pIn,
                                                                        void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int8_t, uint8_t, 1, 1>(DSPLIB_kernelHandle handle,
                                                                        void *restrict pIn,
                                                                        void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int8_t, uint8_t, 2, 0>(DSPLIB_kernelHandle handle,
                                                                        void *restrict pIn,
                                                                        void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int8_t, uint8_t, 2, 1>(DSPLIB_kernelHandle handle,
                                                                        void *restrict pIn,
                                                                        void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int16_t, uint16_t, 1, 0>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int16_t, uint16_t, 1, 1>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int16_t, uint16_t, 2, 0>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int16_t, uint16_t, 2, 1>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int32_t, uint32_t, 1, 0>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int32_t, uint32_t, 1, 1>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int32_t, uint32_t, 2, 0>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int32_t, uint32_t, 2, 1>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int64_t, uint64_t, 1, 0>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int64_t, uint64_t, 1, 1>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int64_t, uint64_t, 2, 0>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<int64_t, uint64_t, 2, 1>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<float, uint32_t, 1, 0>(DSPLIB_kernelHandle handle,
                                                                        void *restrict pIn,
                                                                        void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<float, uint32_t, 1, 1>(DSPLIB_kernelHandle handle,
                                                                        void *restrict pIn,
                                                                        void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<float, uint32_t, 2, 0>(DSPLIB_kernelHandle handle,
                                                                        void *restrict pIn,
                                                                        void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<float, uint32_t, 2, 1>(DSPLIB_kernelHandle handle,
                                                                        void *restrict pIn,
                                                                        void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<double, uint64_t, 1, 0>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<double, uint64_t, 1, 1>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<double, uint64_t, 2, 0>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci<double, uint64_t, 2, 1>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pOut);

void DSPLIB_peakAbsoluteValue_perfEst(DSPLIB_kernelHandle handle,
                                      uint64_t           *archCycles,
                                      uint64_t           *estCycles,
                                      uint32_t            dataType)
{
   DSPLIB_peakAbsoluteValue_PrivArgs *pKerPrivArgs = (DSPLIB_peakAbsoluteValue_PrivArgs *) handle;
   uint64_t                           height       = (uint64_t) pKerPrivArgs->height;

   static const uint32_t rowOverheadCycles[6]   = DSPLIB_PEAKABSOLUTEVALUE_ROW_OVERHEAD_CYCLES;
   static const uint32_t pairIi[2][6]           = {DSPLIB_PEAKABSOLUTEVALUE_PAIR_II_1_VECTOR,
                                                   DSPLIB_PEAKABSOLUTEVALUE_PAIR_II_2_VECTORS};
   static const uint32_t pairPipeUpCycles[2][6] = {DSPLIB_PEAKABSOLUTEVALUE_PAIR_PIPE_UP_1_VECTOR,
                                                   DSPLIB_PEAKABSOLUTEVALUE_PAIR_PIPE_UP_2_VECTORS};
   static const uint32_t oddRowCycles[6]        = DSPLIB_PEAKABSOLUTEVALUE_ODD_ROW_CYCLES;
   static const uint32_t callOverheadCycles[6]  = DSPLIB_PEAKABSOLUTEVALUE_CALL_OVERHEAD_CYCLES;

   /* Index into the per-type tables: 8-bit int, 16-bit int, 32-bit int, 64-bit int, float, double */
   uint32_t typeIdx;
   if (dataType == DSPLIB_INT8) {
      typeIdx = 0U;
   }
   else if (dataType == DSPLIB_INT16) {
      typeIdx = 1U;
   }
   else if (dataType == DSPLIB_INT32) {
      typeIdx = 2U;
   }
   else if (dataType == DSPLIB_INT64) {
      typeIdx = 3U;
   }
   else if (dataType == DSPLIB_FLOAT32) {
      typeIdx = 4U;
   }
   else {
      typeIdx = 5U;
   }

   /* Mirrors the variant choice of DSPLIB_peakAbsoluteValue_init: vecsPerStream 1 or 2 means two rows per iteration */
   if (pKerPrivArgs->vecsPerStream <= 2) {
      uint64_t numRowPairs = height / 2;
      uint32_t vecIdx      = (uint32_t) pKerPrivArgs->vecsPerStream - 1U;

      *archCycles =
          (numRowPairs == 0U) ? 0U : (pairPipeUpCycles[vecIdx][typeIdx] + numRowPairs * pairIi[vecIdx][typeIdx]);
      *archCycles += (height & 1U) * oddRowCycles[typeIdx];
   }
   else {
      *archCycles = height * (DSPLIB_PEAKABSOLUTEVALUE_PIPE_UP_CYCLES + rowOverheadCycles[typeIdx] +
                              (uint64_t) pKerPrivArgs->numIterations * DSPLIB_PEAKABSOLUTEVALUE_CYCLES_PER_ITERATION);
   }
   *estCycles = *archCycles + callOverheadCycles[typeIdx];
}
