// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_minIndex_priv.h"
#include "DSPLIB_types.h"
#include "c7x_scalable.h"
#include <algorithm> // std::min
#include <cmath>     // floor
#include <float.h>
#include <limits>
#include <numeric> // std::iota
#include <vector>  // std::vector

#define INDEX_UNROLL_FACTOR 2

#if defined(__C7504__) || defined(__C7524__)
// vector containing indices of minimum indices (starts with 0, ..., SIMD width but changes as we loop through)
const c7x::uint_vec   jumpFactor          = c7x::uint_vec(16);
const c7x::uint_vec   lastRunOffsets      = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
const c7x::ushort_vec jumpFactorShort     = c7x::ushort_vec(32);
const c7x::ushort_vec lastRunOffsetsShort = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
const c7x::ulong_vec  jumpFactorDp        = c7x::ulong_vec(8);
const c7x::ulong_vec  lastRunOffsetsDp    = c7x::ulong_vec(0, 1, 2, 3);
const c7x::uchar_vec  jumpFactorChar      = c7x::uchar_vec(64);

// clang-format off
const c7x::uchar_vec lastRunOffsetsChar = c7x::uchar_vec(0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,    
                                                         16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
// clang-format on

#else
const c7x::uint_vec   jumpFactor      = c7x::uint_vec(32);
const c7x::uint_vec   lastRunOffsets  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
const c7x::ushort_vec jumpFactorShort = c7x::ushort_vec(64);

// clang-format off
const c7x::ushort_vec lastRunOffsetsShort = c7x::ushort_vec(0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
                                                            16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
// clang-format on

const c7x::ulong_vec jumpFactorDp     = c7x::ulong_vec(16);
const c7x::ulong_vec lastRunOffsetsDp = c7x::ulong_vec(0, 1, 2, 3, 4, 5, 6, 7);
const c7x::uchar_vec jumpFactorChar   = c7x::uchar_vec(128);

// clang-format off
const c7x::uchar_vec lastRunOffsetsChar = c7x::uchar_vec(0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
                                                         17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
                                                         34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                                                         51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
// clang-format on
#endif

template <typename dataType>
DSPLIB_STATUS DSPLIB_minIndex_init_ci(DSPLIB_kernelHandle             handle,
                                      const DSPLIB_bufParams1D_t     *bufParamsIn,
                                      const DSPLIB_bufParams1D_t     *bufParamsOut,
                                      const DSPLIB_minIndex_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params, se1Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;

   DSPLIB_minIndex_PrivArgs *pKerPrivArgs = (DSPLIB_minIndex_PrivArgs *) handle;

   uint8_t *pBlock     = pKerPrivArgs->bufPblock;
   uint32_t blocksSize = pKerPrivArgs->blockSize;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;
   SE_VECLEN                                                       = c7x::se_veclen<vec>::value;
   SE_ELETYPE                                                      = c7x::se_eletype<vec>::value;
   uint32_t length                                                 = blocksSize;
   uint32_t width                                                  = eleCount;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   /**********************************************************************/
   /* Prepare streaming engine 0,1 to fetch the input                    */
   /**********************************************************************/
   se0Params = __gen_SE_TEMPLATE_v1();

   // default SE0 parameters
   se0Params.ICNT0   = width;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.DIMFMT  = __SE_DIMFMT_1D;

   se1Params = __gen_SE_TEMPLATE_v1();

   // default SE1 parameters
   se1Params.ICNT0   = width;
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN  = SE_VECLEN;
   se1Params.DIMFMT  = __SE_DIMFMT_1D;

   // variables to calculate and store compute loop's iteration counter
   uint32_t numBlocks     = length / width;
   uint32_t remBlocksSize = length % width;
   if (remBlocksSize) {
      numBlocks++;
   }
   else {
      /* Nothing to do here */
   }
   // case: length of input <= width
   // one SE fetch is length elements, rest of vec filled with '0'
   if (length <= width) {

      // SE0 fetch length
      se0Params.ICNT0 = length;
      // SE1 not used
   }

   // case: length of input is > width but < 2*width
   // SE0 fetch is one full width, SE1 fetch is partial fetch, rest of vec filled with '0'
   else if (length < 2 * width) {

      // SE0 full fetch
      se0Params.ICNT0 = width;
      // SE1 partial fetch
      se1Params.ICNT0 = remBlocksSize;
   }

   // case: len >= 2*width
   // SE0 and SE1 fetches are full widths only
   else {

      // SE0 Dim is 2D
      se0Params.DIMFMT = __SE_DIMFMT_2D;
      // SE0 jump length each get_adv is 2 widths
      se0Params.DIM1 = 2 * width;
      // SE only performs full fetches in multiples of INDEX_UNROLL_FACTOR, i.e. 2
      se0Params.ICNT1 = length / (INDEX_UNROLL_FACTOR * width);
      // SE0 fetches full widths
      se0Params.ICNT0 = width;

      // SE1 fetches in same manner as SE0, but starts 1 width ahead
      se1Params = se0Params;
   }

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_minIndex_init_ci<int16_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_minIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minIndex_init_ci<uint16_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                         const DSPLIB_minIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minIndex_init_ci<int32_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_minIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minIndex_init_ci<uint32_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                         const DSPLIB_minIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minIndex_init_ci<int8_t>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                       const DSPLIB_minIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minIndex_init_ci<uint8_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_minIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minIndex_init_ci<float>(DSPLIB_kernelHandle             handle,
                                                      const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                      const DSPLIB_minIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_minIndex_init_ci<double>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                       const DSPLIB_minIndex_InitArgs *pKerInitArgs);

template <typename T, typename TIndex> metadata<T, TIndex> DSPLIB_minIndex_loopLogic(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of minimum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::uint_vec minIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec minIndices0 = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec minIndices1 = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uint_vec minIndicesA       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec minIndicesB       = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec firstHalfIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec secondHalfIndices = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
#else
   c7x::uint_vec minIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec minIndices0 = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec minIndices1 = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uint_vec  minIndicesA       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec  minIndicesB       = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uint_vec  firstHalfIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec  secondHalfIndices = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#endif
   // derive c7x vector type from value template and index template
   typedef typename c7x::make_full_vector<T>::type      vec;
   typedef typename c7x::make_full_vector<TIndex>::type index_vec;

   // holds all of the minimum values that have previously been read
   vec     minVals0; // = T(std::numeric_limits<T>::max());
   vec     minVals1; // = minVals0;
   __vpred maskOfmins;

   // holds the overall min vals
   vec    minVals;
   size_t width = c7x::element_count_of<vec>::value;
   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      minVals = c7x::strm_eng<0, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         minVals.s[i] = std::numeric_limits<T>::max();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      minVals0 = c7x::strm_eng<0, vec>::get_adv();
      minVals1 = c7x::strm_eng<1, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;
      for (size_t i = remElements; i < width; i++) {
         minVals1.s[i] = std::numeric_limits<T>::max();
      }
      maskOfmins = __cmp_gt_pred(minVals0, minVals1);
      minVals    = __select(maskOfmins, minVals1, minVals0);
      minIndices = __select(maskOfmins, minIndices1, minIndices0);

      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfmins                             = __cmp_ge_pred(minVals0, minVals1);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndices1, minIndices0);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndices0, minIndices1);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minVals0, minVals1);
      minVals                                = __select(maskOfmins, minVals1, minVals0);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfmins, minIndices1, minIndices0);
      index_vec zeroVec                      = c7x::uint_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      minIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      vec     inVec0, inVec1;
      __vpred mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      vec minValsA = T(std::numeric_limits<T>::max());
      vec minValsB = minValsA;

      // holds the overall min vals
      vec minValsLarge; // = T(std::numeric_limits<T>::max());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, vec>::get_adv();
         mask0  = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA =
             __select(mask0, minIndicesA, firstHalfIndices); // set the min indices to be the indices whose values have
                                                             // changed from the previous min values

         inVec1      = c7x::strm_eng<1, vec>::get_adv();
         mask1       = __cmp_ge_pred(inVec1, minValsB);
         minValsB    = __select(mask1, minValsB, inVec1);
         minIndicesB = __select(mask1, minIndicesB, secondHalfIndices);

         // update the new locations of the indices to be set for the next iteration
         firstHalfIndices += jumpFactor;
         secondHalfIndices += jumpFactor;
      }

      int32_t remBlockSize = length - (INDEX_UNROLL_FACTOR * numIterations * width);

      // if no remainder block, go to end

      int32_t remVecLen = DSPLIB_ceilingDiv(remBlockSize, width);
      T      *remStart  = (T *) pSrc + length - width;

      if (remBlockSize != 0 && remVecLen == 1) {
         inVec0           = *(vec *) remStart;
         firstHalfIndices = c7x::uint_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsets;
         mask0            = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values
      }

      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0           = *(vec *) (remStart - width);
         firstHalfIndices = c7x::uint_vec(length - (2 * c7x::element_count_of<index_vec>::value)) + lastRunOffsets;
         mask0            = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values

         inVec1            = *(vec *) remStart;
         secondHalfIndices = firstHalfIndices + jumpFactor;
         mask1             = __cmp_ge_pred(inVec1, minValsB);
         minValsB          = __select(mask1, minValsB, inVec1);
         minIndicesB       = __select(mask1, minIndicesB, secondHalfIndices);
      }
      else {
         /* Nothing to do here */
      }
      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      __vpred   maskOfminValues              = __cmp_ge_pred(minValsA, minValsB);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndicesB, minIndicesA);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndicesA, minIndicesB);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minValsA, minValsB);
      minValsLarge                           = __select(maskOfminValues, minValsB, minValsA);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfminValues, minIndicesB, minIndicesA);
      index_vec zeroVec                      = c7x::uint_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      index_vec minIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      minVals    = minValsLarge;
      minIndices = minIndicesLarge;
   }
   metadata<T, TIndex> output;
   output.minVals    = minVals;
   output.minIndices = minIndices;
   return output;
}

// explicit templatization for int32, uint32_t
template <int32_t, uint32_t> metadata<int32_t, uint32_t> DSPLIB_minIndex_loopLogic(size_t length, void *pSrc);

template <uint32_t, uint32_t> metadata<uint32_t, uint32_t> DSPLIB_minIndex_loopLogic(size_t length, void *pSrc);

// explicit templatization for int8_t type
template <> metadata<int8_t, uint8_t> DSPLIB_minIndex_loopLogic<int8_t, uint8_t>(size_t length, void *pSrc)
{

   /* printf("\ninside int 8 loop logic with length: %d\n", length); */
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of minimum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::uchar_vec minIndices  = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec minIndices0 = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec minIndices1 = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                               50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uchar_vec minIndicesA = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec minIndicesB = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                               50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec firstHalfIndices  = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                                                     19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec secondHalfIndices = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                                                     49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#else
   c7x::uchar_vec minIndices =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec minIndices0 =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec minIndices1 =
       c7x::uchar_vec(64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
                      88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                      110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uchar_vec minIndicesA =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec minIndicesB =
       c7x::uchar_vec(64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
                      88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                      110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127);
   c7x::uchar_vec firstHalfIndices =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec secondHalfIndices =
       c7x::uchar_vec(64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
                      88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                      110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127);
#endif

   typedef typename c7x::make_full_vector<int8_t>::type  vec;
   typedef typename c7x::make_full_vector<uint8_t>::type index_vec;

   // holds all of the minimum values that have previously been read
   vec     minVals0; // = int8_t(std::numeric_limits<int8_t>::max());
   vec     minVals1; // = minVals0;
   __vpred maskOfmins;

   // holds the overall min vals
   vec    minVals;
   size_t width = c7x::element_count_of<vec>::value;

   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      minVals = c7x::strm_eng<0, vec>::get_adv();

      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         minVals.s[i] = std::numeric_limits<int8_t>::max();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      minVals0 = c7x::strm_eng<0, vec>::get_adv();
      minVals1 = c7x::strm_eng<1, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;

      for (size_t i = remElements; i < width; i++) {
         minVals1.s[i] = std::numeric_limits<int8_t>::max();
      }
      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfmins                             = __cmp_ge_pred(minVals0, minVals1);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndices1, minIndices0);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndices0, minIndices1);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minVals0, minVals1);
      minVals                                = __select(maskOfmins, minVals1, minVals0);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfmins, minIndices1, minIndices0);
      index_vec zeroVec                      = c7x::uchar_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      minIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      vec     inVec0, inVec1;
      __vpred mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      vec minValsA = int8_t(std::numeric_limits<int8_t>::max());
      vec minValsB = minValsA;

      // holds the overall min vals
      vec minValsLarge; // = int8_t(std::numeric_limits<int8_t>::max());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, vec>::get_adv();
         mask0  = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA =
             __select(mask0, minIndicesA, firstHalfIndices); // set the min indices to be the indices whose values have
                                                             // changed from the previous min values

         inVec1      = c7x::strm_eng<1, vec>::get_adv();
         mask1       = __cmp_ge_pred(inVec1, minValsB);
         minValsB    = __select(mask1, minValsB, inVec1);
         minIndicesB = __select(mask1, minIndicesB, secondHalfIndices);

         // update the new locations of the indices to be set for the next iteration
         firstHalfIndices += jumpFactorChar;
         secondHalfIndices += jumpFactorChar;
      }

      int32_t remBlockSize = length - (INDEX_UNROLL_FACTOR * numIterations * width);

      // if no remainder block, go to end

      int32_t remVecLen = DSPLIB_ceilingDiv(remBlockSize, width);
      int8_t *remStart  = (int8_t *) pSrc + length - width;

      if (remBlockSize != 0 && remVecLen == 1) {

         inVec0           = *(vec *) remStart;
         firstHalfIndices = c7x::uchar_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsetsChar;
         mask0            = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values
      }

      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0           = *(vec *) (remStart - width);
         firstHalfIndices = c7x::uchar_vec(length - (2 * c7x::element_count_of<index_vec>::value)) + lastRunOffsetsChar;
         mask0            = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values

         inVec1            = *(vec *) remStart;
         secondHalfIndices = c7x::uchar_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsetsChar;
         mask1             = __cmp_ge_pred(inVec1, minValsB);
         minValsB          = __select(mask1, minValsB, inVec1);
         minIndicesB       = __select(mask1, minIndicesB, secondHalfIndices);
      }
      else {
         /* Nothing to do here */
      }
      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      __vpred   maskOfminValues              = __cmp_ge_pred(minValsA, minValsB);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndicesB, minIndicesA);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndicesA, minIndicesB);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minValsA, minValsB);
      minValsLarge                           = __select(maskOfminValues, minValsB, minValsA);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfminValues, minIndicesB, minIndicesA);
      index_vec zeroVec                      = c7x::uchar_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      index_vec minIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      minVals    = minValsLarge;
      minIndices = minIndicesLarge;
   }

   metadata<int8_t, uint8_t> output;
   output.minVals    = minVals;
   output.minIndices = minIndices;
   return output;
}

// explicit templatization for uint8_t type
template <> metadata<uint8_t, uint8_t> DSPLIB_minIndex_loopLogic<uint8_t, uint8_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of minimum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::uchar_vec minIndices  = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec minIndices0 = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec minIndices1 = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                               50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uchar_vec minIndicesA = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec minIndicesB = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                               50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec firstHalfIndices  = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                                                     19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec secondHalfIndices = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                                                     49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#else
   c7x::uchar_vec minIndices =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec minIndices0 =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec minIndices1 =
       c7x::uchar_vec(64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
                      88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                      110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uchar_vec minIndicesA =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec minIndicesB =
       c7x::uchar_vec(64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
                      88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                      110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127);
   c7x::uchar_vec firstHalfIndices =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec secondHalfIndices =
       c7x::uchar_vec(64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
                      88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                      110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127);
#endif

   typedef typename c7x::make_full_vector<uint8_t>::type vec;
   typedef typename c7x::make_full_vector<uint8_t>::type index_vec;

   // holds all of the minimum values that have previously been read
   vec     minVals0; // = uint8_t(std::numeric_limits<uint8_t>::max());
   vec     minVals1; // = minVals0;
   __vpred maskOfmins;

   // holds the overall min vals
   vec    minVals;
   size_t width = c7x::element_count_of<vec>::value;

   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      minVals = c7x::strm_eng<0, vec>::get_adv();

      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         minVals.s[i] = std::numeric_limits<uint8_t>::max();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      minVals0 = c7x::strm_eng<0, vec>::get_adv();
      minVals1 = c7x::strm_eng<1, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;

      for (size_t i = remElements; i < width; i++) {
         minVals1.s[i] = std::numeric_limits<uint8_t>::max();
      }
      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfmins                             = __cmp_ge_pred(minVals0, minVals1);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndices1, minIndices0);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndices0, minIndices1);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minVals0, minVals1);
      minVals                                = __select(maskOfmins, minVals1, minVals0);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfmins, minIndices1, minIndices0);
      index_vec zeroVec                      = c7x::uchar_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      minIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      vec     inVec0, inVec1;
      __vpred mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      vec minValsA = uint8_t(std::numeric_limits<uint8_t>::max());
      vec minValsB = minValsA;

      // holds the overall min vals
      vec minValsLarge; // = uint8_t(std::numeric_limits<uint8_t>::max());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, vec>::get_adv();
         mask0  = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA =
             __select(mask0, minIndicesA, firstHalfIndices); // set the min indices to be the indices whose values have
                                                             // changed from the previous min values

         inVec1      = c7x::strm_eng<1, vec>::get_adv();
         mask1       = __cmp_ge_pred(inVec1, minValsB);
         minValsB    = __select(mask1, minValsB, inVec1);
         minIndicesB = __select(mask1, minIndicesB, secondHalfIndices);

         // update the new locations of the indices to be set for the next iteration
         firstHalfIndices += jumpFactorChar;
         secondHalfIndices += jumpFactorChar;
      }

      int32_t remBlockSize = length - (INDEX_UNROLL_FACTOR * numIterations * width);

      // if no remainder block, go to end

      int32_t  remVecLen = DSPLIB_ceilingDiv(remBlockSize, width);
      uint8_t *remStart  = (uint8_t *) pSrc + length - width;

      if (remBlockSize != 0 && remVecLen == 1) {

         inVec0           = *(vec *) remStart;
         firstHalfIndices = c7x::uchar_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsetsChar;
         mask0            = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values
      }

      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0           = *(vec *) (remStart - width);
         firstHalfIndices = c7x::uchar_vec(length - (2 * c7x::element_count_of<index_vec>::value)) + lastRunOffsetsChar;
         mask0            = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values

         inVec1            = *(vec *) remStart;
         secondHalfIndices = c7x::uchar_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsetsChar;
         mask1             = __cmp_ge_pred(inVec1, minValsB);
         minValsB          = __select(mask1, minValsB, inVec1);
         minIndicesB       = __select(mask1, minIndicesB, secondHalfIndices);
      }
      else {
         /* Nothing to do here */
      }
      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      __vpred   maskOfminValues              = __cmp_ge_pred(minValsA, minValsB);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndicesB, minIndicesA);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndicesA, minIndicesB);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minValsA, minValsB);
      minValsLarge                           = __select(maskOfminValues, minValsB, minValsA);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfminValues, minIndicesB, minIndicesA);
      index_vec zeroVec                      = c7x::uchar_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      index_vec minIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      minVals    = minValsLarge;
      minIndices = minIndicesLarge;
   }

   metadata<uint8_t, uint8_t> output;
   output.minVals    = minVals;
   output.minIndices = minIndices;
   return output;
}

// explicit templatization for int16_t type
template <> metadata<int16_t, uint16_t> DSPLIB_minIndex_loopLogic<int16_t, uint16_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of minimum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::ushort_vec minIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec minIndices0 = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec minIndices1 = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ushort_vec minIndicesA       = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec minIndicesB       = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec firstHalfIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec secondHalfIndices = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#else
   c7x::ushort_vec minIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec minIndices0 = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec minIndices1 = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                                 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ushort_vec minIndicesA = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec minIndicesB = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                                 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::ushort_vec firstHalfIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                                                       19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec secondHalfIndices = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                                                       48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#endif

   typedef typename c7x::make_full_vector<int16_t>::type  vec;
   typedef typename c7x::make_full_vector<uint16_t>::type index_vec;

   // holds all of the minimum values that have previously been read
   vec     minVals0; // = int16_t(std::numeric_limits<int16_t>::max());
   vec     minVals1; // = minVals0;
   __vpred maskOfmins;

   // holds the overall min vals
   vec    minVals;
   size_t width = c7x::element_count_of<vec>::value;

   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      minVals = c7x::strm_eng<0, vec>::get_adv();

      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         minVals.s[i] = std::numeric_limits<int16_t>::max();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      minVals0 = c7x::strm_eng<0, vec>::get_adv();
      minVals1 = c7x::strm_eng<1, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;

      for (size_t i = remElements; i < width; i++) {
         minVals1.s[i] = std::numeric_limits<int16_t>::max();
      }
      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfmins                             = __cmp_ge_pred(minVals0, minVals1);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndices1, minIndices0);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndices0, minIndices1);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minVals0, minVals1);
      minVals                                = __select(maskOfmins, minVals1, minVals0);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfmins, minIndices1, minIndices0);
      index_vec zeroVec                      = c7x::ushort_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      minIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      vec     inVec0, inVec1;
      __vpred mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      vec minValsA = int16_t(std::numeric_limits<int16_t>::max());
      vec minValsB = minValsA;

      // holds the overall min vals
      vec minValsLarge; // = int16_t(std::numeric_limits<int16_t>::max());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, vec>::get_adv();
         mask0  = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA =
             __select(mask0, minIndicesA, firstHalfIndices); // set the min indices to be the indices whose values have
                                                             // changed from the previous min values

         inVec1      = c7x::strm_eng<1, vec>::get_adv();
         mask1       = __cmp_ge_pred(inVec1, minValsB);
         minValsB    = __select(mask1, minValsB, inVec1);
         minIndicesB = __select(mask1, minIndicesB, secondHalfIndices);

         // update the new locations of the indices to be set for the next iteration
         firstHalfIndices += jumpFactorShort;
         secondHalfIndices += jumpFactorShort;
      }

      int32_t remBlockSize = length - (INDEX_UNROLL_FACTOR * numIterations * width);

      // if no remainder block, go to end

      int32_t  remVecLen = DSPLIB_ceilingDiv(remBlockSize, width);
      int16_t *remStart  = (int16_t *) pSrc + length - width;

      if (remBlockSize != 0 && remVecLen == 1) {

         inVec0           = *(vec *) remStart;
         firstHalfIndices = c7x::ushort_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsetsShort;
         mask0            = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values
      }

      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0 = *(vec *) (remStart - width);
         firstHalfIndices =
             c7x::ushort_vec(length - (2 * c7x::element_count_of<index_vec>::value)) + lastRunOffsetsShort;
         mask0 = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values

         inVec1            = *(vec *) remStart;
         secondHalfIndices = c7x::ushort_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsetsShort;
         mask1             = __cmp_ge_pred(inVec1, minValsB);
         minValsB          = __select(mask1, minValsB, inVec1);
         minIndicesB       = __select(mask1, minIndicesB, secondHalfIndices);
      }
      else {
         /* Nothing to do here */
      }
      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      __vpred   maskOfminValues              = __cmp_ge_pred(minValsA, minValsB);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndicesB, minIndicesA);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndicesA, minIndicesB);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minValsA, minValsB);
      minValsLarge                           = __select(maskOfminValues, minValsB, minValsA);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfminValues, minIndicesB, minIndicesA);
      index_vec zeroVec                      = c7x::ushort_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      index_vec minIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      minVals    = minValsLarge;
      minIndices = minIndicesLarge;
   }

   metadata<int16_t, uint16_t> output;
   output.minVals    = minVals;
   output.minIndices = minIndices;
   return output;
}

// explicit templatization for uint16_t type
template <> metadata<uint16_t, uint16_t> DSPLIB_minIndex_loopLogic<uint16_t, uint16_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of minimum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::ushort_vec minIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec minIndices0 = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec minIndices1 = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ushort_vec minIndicesA       = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec minIndicesB       = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec firstHalfIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec secondHalfIndices = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#else
   c7x::ushort_vec minIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec minIndices0 = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec minIndices1 = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                                 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ushort_vec minIndicesA = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec minIndicesB = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                                 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::ushort_vec firstHalfIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                                                       19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec secondHalfIndices = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                                                       48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#endif

   typedef typename c7x::make_full_vector<uint16_t>::type vec;
   typedef typename c7x::make_full_vector<uint16_t>::type index_vec;

   // holds all of the minimum values that have previously been read
   vec     minVals0; // = uint16_t(std::numeric_limits<uint16_t>::max());
   vec     minVals1; // = minVals0;
   __vpred maskOfmins;

   // holds the overall min vals
   vec    minVals;
   size_t width = c7x::element_count_of<vec>::value;

   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      minVals = c7x::strm_eng<0, vec>::get_adv();

      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         minVals.s[i] = std::numeric_limits<int16_t>::max();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      minVals0 = c7x::strm_eng<0, vec>::get_adv();
      minVals1 = c7x::strm_eng<1, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;

      for (size_t i = remElements; i < width; i++) {
         minVals1.s[i] = std::numeric_limits<int16_t>::max();
      }
      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfmins                             = __cmp_ge_pred(minVals0, minVals1);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndices1, minIndices0);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndices0, minIndices1);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minVals0, minVals1);
      minVals                                = __select(maskOfmins, minVals1, minVals0);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfmins, minIndices1, minIndices0);
      index_vec zeroVec                      = c7x::ushort_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      minIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      vec     inVec0, inVec1;
      __vpred mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      vec minValsA = uint16_t(std::numeric_limits<uint16_t>::max());
      vec minValsB = minValsA;

      // holds the overall min vals
      vec minValsLarge; // = uint16_t(std::numeric_limits<uint16_t>::max());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, vec>::get_adv();
         mask0  = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA =
             __select(mask0, minIndicesA, firstHalfIndices); // set the min indices to be the indices whose values have
                                                             // changed from the previous min values

         inVec1      = c7x::strm_eng<1, vec>::get_adv();
         mask1       = __cmp_ge_pred(inVec1, minValsB);
         minValsB    = __select(mask1, minValsB, inVec1);
         minIndicesB = __select(mask1, minIndicesB, secondHalfIndices);

         // update the new locations of the indices to be set for the next iteration
         firstHalfIndices += jumpFactorShort;
         secondHalfIndices += jumpFactorShort;
      }

      int32_t remBlockSize = length - (INDEX_UNROLL_FACTOR * numIterations * width);

      // if no remainder block, go to end

      int32_t   remVecLen = DSPLIB_ceilingDiv(remBlockSize, width);
      uint16_t *remStart  = (uint16_t *) pSrc + length - width;

      if (remBlockSize != 0 && remVecLen == 1) {

         inVec0           = *(vec *) remStart;
         firstHalfIndices = c7x::ushort_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsetsShort;
         mask0            = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values
      }

      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0 = *(vec *) (remStart - width);
         firstHalfIndices =
             c7x::ushort_vec(length - (2 * c7x::element_count_of<index_vec>::value)) + lastRunOffsetsShort;
         mask0 = __cmp_ge_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, minValsA, inVec0); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, minIndicesA,
                                firstHalfIndices); // set the min indices to be the indices whose values have changed
                                                   // from the previous min values

         inVec1            = *(vec *) remStart;
         secondHalfIndices = c7x::ushort_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsetsShort;
         mask1             = __cmp_ge_pred(inVec1, minValsB);
         minValsB          = __select(mask1, minValsB, inVec1);
         minIndicesB       = __select(mask1, minIndicesB, secondHalfIndices);
      }
      else {
         /* Nothing to do here */
      }
      // get the minimum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      __vpred   maskOfminValues              = __cmp_ge_pred(minValsA, minValsB);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(minIndicesB, minIndicesA);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, minIndicesA, minIndicesB);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(minValsA, minValsB);
      minValsLarge                           = __select(maskOfminValues, minValsB, minValsA);
      index_vec minIndicesIgnoringTiebreaker = __select(maskOfminValues, minIndicesB, minIndicesA);
      index_vec zeroVec                      = c7x::ushort_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, minIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      index_vec minIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      minVals    = minValsLarge;
      minIndices = minIndicesLarge;
   }

   metadata<uint16_t, uint16_t> output;
   output.minVals    = minVals;
   output.minIndices = minIndices;
   return output;
}

// explicit templatization for float type
template <> metadata<float, uint32_t> DSPLIB_minIndex_loopLogic<float, uint32_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of minimum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::uint_vec minIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec minIndices0 = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec minIndices1 = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uint_vec minIndicesA       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec minIndicesB       = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec firstHalfIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec secondHalfIndices = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
#else
   c7x::uint_vec   minIndices        = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec   minIndices0       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec   minIndices1       = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uint_vec  minIndicesA       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec  minIndicesB       = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uint_vec  firstHalfIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec  secondHalfIndices = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#endif
   // holds all of the minimum values that have previously been read
   c7x::float_vec minVals0; // = std::numeric_limits<float>::max();
   c7x::float_vec minVals1; // = minVals0;
   __vpred        maskOfmins;

   size_t width = c7x::element_count_of<c7x::float_vec>::value;

   // holds the overall min vals
   c7x::float_vec minVals;
   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      minVals = c7x::strm_eng<0, c7x::float_vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         minVals.s[i] = std::numeric_limits<float>::max();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      minVals0 = c7x::strm_eng<0, c7x::float_vec>::get_adv();
      minVals1 = c7x::strm_eng<1, c7x::float_vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;
      for (size_t i = remElements; i < width; i++) {
         minVals1.s[i] = std::numeric_limits<float>::max();
      }
      maskOfmins = __cmp_lt_pred(minVals0, minVals1);
      minVals    = __select(maskOfmins, minVals0, minVals1);
      minIndices = __select(maskOfmins, minIndices0, minIndices1);
   }
   else {
      // input vectors
      c7x::float_vec inVec0, inVec1;
      __vpred        mask0, mask1, maskOfminsLarge;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      c7x::float_vec minValsA = std::numeric_limits<float>::max();
      c7x::float_vec minValsB = minValsA;

      // holds the overall min vals
      c7x::float_vec minValsLarge; //  = std::numeric_limits<float>::max();
      size_t         numIterations = length / (INDEX_UNROLL_FACTOR * width);
      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, c7x::float_vec>::get_adv();
         mask0  = __cmp_lt_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, inVec0, minValsA); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA =
             __select(mask0, firstHalfIndices, minIndicesA); // set the min indices to be the indices whose values have
                                                             // changed from the previous min values

         inVec1      = c7x::strm_eng<1, c7x::float_vec>::get_adv();
         mask1       = __cmp_lt_pred(inVec1, minValsB);
         minValsB    = __select(mask1, inVec1, minValsB);
         minIndicesB = __select(mask1, secondHalfIndices, minIndicesB);

         // update the new locations of the indices to be set for the next iteration
         firstHalfIndices += jumpFactor;
         secondHalfIndices += jumpFactor;
      }

      int32_t remBlockSize = length - (numIterations * INDEX_UNROLL_FACTOR * width);

      // if no remainder block, go to end

      int32_t remVecLen = DSPLIB_ceilingDiv(remBlockSize, width);
      float  *remStart  = (float *) pSrc + length - width;

      // if remainder 1
      if (remBlockSize != 0 && remVecLen == 1) {

         inVec0           = *(c7x::float_vec *) remStart;
         firstHalfIndices = c7x::uint_vec(length - (width)) + lastRunOffsets;
         mask0            = __cmp_lt_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, inVec0, minValsA); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, firstHalfIndices,
                                minIndicesA); // set the min indices to be the indices whose values have changed from
                                              // the previous min values
      }

      // if remainder 2
      else if (remBlockSize != 0 && remVecLen == 2) {

         inVec0           = *(c7x::float_vec *) (remStart - width);
         firstHalfIndices = c7x::uint_vec(length - (2 * width)) + lastRunOffsets;
         mask0            = __cmp_lt_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, inVec0, minValsA); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, firstHalfIndices,
                                minIndicesA); // set the min indices to be the indices whose values have changed from
                                              // the previous min values

         inVec1            = *(c7x::float_vec *) remStart;
         secondHalfIndices = c7x::uint_vec(length - (width)) + lastRunOffsets;
         mask1             = __cmp_lt_pred(inVec1, minValsB);
         minValsB          = __select(mask1, inVec1, minValsB);
         minIndicesB       = __select(mask1, secondHalfIndices, minIndicesB);
      }
      else {
         /* Nothing to do here */
      }
      maskOfminsLarge               = __cmp_lt_pred(minValsA, minValsB);
      minValsLarge                  = __select(maskOfminsLarge, minValsA, minValsB);
      c7x::uint_vec minIndicesLarge = __select(maskOfminsLarge, minIndicesA, minIndicesB);

      minVals    = minValsLarge;
      minIndices = minIndicesLarge;
   }
   metadata<float, uint32_t> output;
   output.minVals    = minVals;
   output.minIndices = minIndices;
   return output;
}

// explicit templatization for double type
template <> metadata<double, uint64_t> DSPLIB_minIndex_loopLogic<double, uint64_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of minimum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::ulong_vec minIndices  = c7x::ulong_vec(0, 1, 2, 3);
   c7x::ulong_vec minIndices0 = c7x::ulong_vec(0, 1, 2, 3);
   c7x::ulong_vec minIndices1 = c7x::ulong_vec(4, 5, 6, 7);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ulong_vec minIndicesA       = c7x::ulong_vec(0, 1, 2, 3);
   c7x::ulong_vec minIndicesB       = c7x::ulong_vec(4, 5, 6, 7);
   c7x::ulong_vec firstHalfIndices  = c7x::ulong_vec(0, 1, 2, 3);
   c7x::ulong_vec secondHalfIndices = c7x::ulong_vec(4, 5, 6, 7);
#else
   c7x::ulong_vec minIndices        = c7x::ulong_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::ulong_vec minIndices0       = c7x::ulong_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::ulong_vec minIndices1       = c7x::ulong_vec(8, 9, 10, 11, 12, 13, 14, 15);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ulong_vec minIndicesA       = c7x::ulong_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::ulong_vec minIndicesB       = c7x::ulong_vec(8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ulong_vec firstHalfIndices  = c7x::ulong_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::ulong_vec secondHalfIndices = c7x::ulong_vec(8, 9, 10, 11, 12, 13, 14, 15);
#endif
   // holds all of the minimum values that have previously been read
   c7x::double_vec minVals0; // = std::numeric_limits<double>::max();
   c7x::double_vec minVals1; // = minVals0;
   __vpred         maskOfmins;

   size_t width = c7x::element_count_of<c7x::double_vec>::value;

   // holds the overall min vals
   c7x::double_vec minVals;
   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      minVals = c7x::strm_eng<0, c7x::double_vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         minVals.s[i] = std::numeric_limits<double>::max();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      minVals0 = c7x::strm_eng<0, c7x::double_vec>::get_adv();
      minVals1 = c7x::strm_eng<1, c7x::double_vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remainingElement = length % width;
      for (size_t i = remainingElement; i < width; i++) {
         minVals1.s[i] = std::numeric_limits<double>::max();
      }
      maskOfmins = __cmp_lt_pred(minVals0, minVals1);
      minVals    = __select(maskOfmins, minVals0, minVals1);
      minIndices = __select(maskOfmins, minIndices0, minIndices1);
   }
   else {
      // input vectors
      c7x::double_vec inVec0, inVec1;
      __vpred         mask0, mask1, maskOfminsLarge;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      c7x::double_vec minValsA = std::numeric_limits<double>::max();
      c7x::double_vec minValsB = minValsA;

      // holds the overall min vals
      c7x::double_vec minValsLarge; //  = std::numeric_limits<double>::max();
      size_t          numIterations = length / (INDEX_UNROLL_FACTOR * width);
      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, c7x::double_vec>::get_adv();
         mask0  = __cmp_lt_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, inVec0, minValsA); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA =
             __select(mask0, firstHalfIndices, minIndicesA); // set the min indices to be the indices whose values have
                                                             // changed from the previous min values

         inVec1      = c7x::strm_eng<1, c7x::double_vec>::get_adv();
         mask1       = __cmp_lt_pred(inVec1, minValsB);
         minValsB    = __select(mask1, inVec1, minValsB);
         minIndicesB = __select(mask1, secondHalfIndices, minIndicesB);

         // update the new locations of the indices to be set for the next iteration
         firstHalfIndices += jumpFactorDp;
         secondHalfIndices += jumpFactorDp;
      }

      int32_t remBlockSize = length - (numIterations * INDEX_UNROLL_FACTOR * width);

      // if no remainder block, go to end

      int32_t remVecLen = DSPLIB_ceilingDiv(remBlockSize, width);
      double *remStart  = (double *) pSrc + length - width;

      // if remainder 1
      if (remBlockSize != 0 && remVecLen == 1) {

         inVec0           = *(c7x::double_vec *) remStart;
         firstHalfIndices = c7x::ulong_vec(length - (width)) + lastRunOffsetsDp;
         mask0            = __cmp_lt_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, inVec0, minValsA); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, firstHalfIndices,
                                minIndicesA); // set the min indices to be the indices whose values have changed from
                                              // the previous min values
      }

      // if remainder 2
      else if (remBlockSize != 0 && remVecLen == 2) {

         inVec0           = *(c7x::double_vec *) (remStart - width);
         firstHalfIndices = c7x::ulong_vec(length - (2 * width)) + lastRunOffsetsDp;
         mask0            = __cmp_lt_pred(inVec0, minValsA);
         minValsA =
             __select(mask0, inVec0, minValsA); // change the values in the min val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         minIndicesA = __select(mask0, firstHalfIndices,
                                minIndicesA); // set the min indices to be the indices whose values have changed from
                                              // the previous min values

         inVec1            = *(c7x::double_vec *) remStart;
         secondHalfIndices = c7x::ulong_vec(length - (width)) + lastRunOffsetsDp;
         mask1             = __cmp_lt_pred(inVec1, minValsB);
         minValsB          = __select(mask1, inVec1, minValsB);
         minIndicesB       = __select(mask1, secondHalfIndices, minIndicesB);
      }
      else {
         /* Do nothing */
      }

      maskOfminsLarge                = __cmp_lt_pred(minValsA, minValsB);
      minValsLarge                   = __select(maskOfminsLarge, minValsA, minValsB);
      c7x::ulong_vec minIndicesLarge = __select(maskOfminsLarge, minIndicesA, minIndicesB);

      minVals    = minValsLarge;
      minIndices = minIndicesLarge;
   }

   metadata<double, uint64_t> output;
   output.minVals    = minVals;
   output.minIndices = minIndices;
   return output;
}

template <typename T, typename TIndex>
DSPLIB_STATUS DSPLIB_minIndex_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_minIndex_PrivArgs *pKerPrivArgs = (DSPLIB_minIndex_PrivArgs *) handle;
   uint32_t                  blockSize    = pKerPrivArgs->blockSize;
   uint32_t                  length       = blockSize;
   DSPLIB_STATUS             status       = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 se0Params, se1Params;

   T *restrict pInLocal         = (T *) pIn;
   uint32_t *restrict pOutLocal = (uint32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_minIndex_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<T>::type vec;

   uint32_t eleCount = c7x::element_count_of<vec>::value;
   uint32_t width    = eleCount;
#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   se0Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal, se0Params);
   if (length > width) {
      __SE1_OPEN(pInLocal + eleCount, se1Params);
   }
   else {
      /* Nothing to do here */
   }

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   size_t bitsInType = sizeof(T) * 8; // sizeof(T) is measured in bytes
   bitsInType        = (bitsInType > 32) ? 32 : bitsInType;

   size_t   minSingleBufferSize = pow(2, bitsInType);
   uint32_t numBufferIterations = DSPLIB_ceilingDiv(length, minSingleBufferSize);

   std::vector<T>        minVals(numBufferIterations);
   std::vector<uint32_t> minIndices(numBufferIterations);
   T                    *currentValuePtr;
   T currentValue; // using this value so we don't dereference the pointer three different times per iteration
   T smallest;
   metadata<T, TIndex> loopOutput;
   size_t              currentIterationSize;
   TIndex              minIndex;
   TIndex             *currentIndexPtr;
   TIndex              currentIndex;
   size_t              i;
   for (uint32_t buffer = 0; buffer < numBufferIterations; buffer++) {
      currentIterationSize = std::min((size_t) minSingleBufferSize, (size_t) (length - (minSingleBufferSize * buffer)));
      loopOutput           = DSPLIB_minIndex_loopLogic<T, TIndex>(currentIterationSize, pInLocal);
      // find the minimum index by looping through the min vector and getting the corresponding min index
      // use pointer since .s[i] is problematic
      currentValuePtr = (T *) &loopOutput.minVals;
      smallest        = *currentValuePtr++;
      currentIndexPtr = (TIndex *) &loopOutput.minIndices;
      minIndex        = *currentIndexPtr++;
      for (i = 1; i < c7x::element_count_of<vec>::value; i++) {
         currentValue = *currentValuePtr;
         currentIndex = *currentIndexPtr;
         if (currentValue < smallest) {
            smallest = currentValue;
            minIndex = currentIndex;
         }
         // need the first instance of the minimum value, so set the minimum index to the lower index if current value
         // is same as current minimum value
         else if (currentValue == smallest) {
            if (currentIndex < minIndex) {
               minIndex = currentIndex;
            }
            else {
               /* Nothing to do here */
            }
         }
         else {
            /* Nothing to do here */
         }
         currentValuePtr++;
         currentIndexPtr++;
      }

      minVals[buffer]    = smallest;
      minIndices[buffer] = ((uint32_t) minIndex) + (buffer * minSingleBufferSize);
   }
   T        smallestVal   = minVals[0];
   uint32_t smallestIndex = minIndices[0];
   /* printf("\n%d, %d\n", minIndices[0], minVals[0]); */
   for (i = 1; i < minVals.size(); i++) {
      /* printf("\n%d, %d\n", minIndices[i], minVals[i]); */

      if (minVals[i] < smallestVal) {
         smallestVal   = minVals[i];
         smallestIndex = minIndices[i];
      }
      else {
         /* Nothing to do here */
      }
   }

   /* printf("\n%d, %d\n", smallestIndex, smallestVal); */
   *pOutLocal = smallestIndex;
   // close SE0 and SE1
   __SE0_CLOSE();
   if (length > width) {
      __SE1_CLOSE();
   }
   else {
      /* Nothing to do here */
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_minIndex_exec_ci<int8_t, uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_minIndex_exec_ci<uint8_t, uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_minIndex_exec_ci<int16_t, uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_minIndex_exec_ci<uint16_t, uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_minIndex_exec_ci<int32_t, uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_minIndex_exec_ci<uint32_t, uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_minIndex_exec_ci<float, uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_minIndex_exec_ci<double, uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
