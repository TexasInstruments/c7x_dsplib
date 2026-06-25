// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_maxIndex_priv.h"
#include "DSPLIB_types.h"
#include "c7x_scalable.h"
#include <algorithm> // std::min
#include <cmath>     // floor
#include <cstdint>
#include <float.h>
#include <limits>
#include <numeric> // std::iota
#include <vector>  // std::vector

#define INDEX_UNROLL_FACTOR 2

#if defined(__C7504__) || defined(__C7524__)
// vector containing indices of maximum indices (starts with 0, ..., SIMD width but changes as we loop through)
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
DSPLIB_STATUS DSPLIB_maxIndex_init_ci(DSPLIB_kernelHandle             handle,
                                      const DSPLIB_bufParams1D_t     *bufParamsIn,
                                      const DSPLIB_bufParams1D_t     *bufParamsOut,
                                      const DSPLIB_maxIndex_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params, se1Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;

   DSPLIB_maxIndex_PrivArgs *pKerPrivArgs = (DSPLIB_maxIndex_PrivArgs *) handle;

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

template DSPLIB_STATUS DSPLIB_maxIndex_init_ci<int16_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_maxIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_maxIndex_init_ci<uint16_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                         const DSPLIB_maxIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_maxIndex_init_ci<int32_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_maxIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_maxIndex_init_ci<uint32_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                         const DSPLIB_maxIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_maxIndex_init_ci<int8_t>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                       const DSPLIB_maxIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_maxIndex_init_ci<uint8_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_maxIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_maxIndex_init_ci<float>(DSPLIB_kernelHandle             handle,
                                                      const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                      const DSPLIB_maxIndex_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_maxIndex_init_ci<double>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                       const DSPLIB_maxIndex_InitArgs *pKerInitArgs);

template <typename T, typename TIndex>
DSPLIB_STATUS DSPLIB_maxIndex_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_maxIndex_PrivArgs *pKerPrivArgs = (DSPLIB_maxIndex_PrivArgs *) handle;
   uint32_t                  blockSize    = pKerPrivArgs->blockSize;
   uint32_t                  length       = blockSize;
   DSPLIB_STATUS             status       = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 se0Params, se1Params;

   T *restrict pInLocal         = (T *) pIn;
   uint32_t *restrict pOutLocal = (uint32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_maxIndex_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<T>::type vec;
   uint32_t                                        eleCount = c7x::element_count_of<vec>::value;
   uint32_t                                        width    = eleCount;
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

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   size_t bitsInType = sizeof(T) * 8; // sizeof(T) is measured in bytes
   bitsInType        = (bitsInType > 32) ? 32 : bitsInType;

   size_t   maxSingleBufferSize = pow(2, bitsInType);
   uint32_t numBufferIterations = DSPLIB_ceilingDiv(length, maxSingleBufferSize);

   std::vector<T>        maxVals(numBufferIterations);
   std::vector<uint32_t> maxIndices(numBufferIterations);

   T *currentValuePtr;
   T  currentValue; // using this value so we don't dereference the pointer three different times per iteration
   T  largest;
   metadata<T, TIndex> loopOutput;
   size_t              currentIterationSize;
   TIndex              maxIndex;
   TIndex             *currentIndexPtr;
   TIndex              currentIndex;
   size_t              i = 1;
   for (uint32_t buffer = 0; buffer < numBufferIterations; buffer++) {

      currentIterationSize = std::min((size_t) maxSingleBufferSize, (size_t) (length - (maxSingleBufferSize * buffer)));

      loopOutput = DSPLIB_maxIndex_loopLogic<T, TIndex>(currentIterationSize, pInLocal);
      // find the maximum index by looping through the min vector and getting the corresponding min index
      // use pointer since .s[i] is problematic
      currentValuePtr = (T *) &loopOutput.maxVals;
      largest         = *currentValuePtr++;
      currentIndexPtr = (TIndex *) &loopOutput.maxIndices;
      maxIndex        = *currentIndexPtr++;
      for (i = 1; i < c7x::element_count_of<vec>::value; i++) {
         currentValue = *currentValuePtr;
         currentIndex = *currentIndexPtr;
         if (currentValue > largest) {
            largest  = currentValue;
            maxIndex = currentIndex;
         }
         // need the first instance of the maximum value, so set the maximum index to the lower index if current value
         // is same as current maximum value
         else if (currentValue == largest) {
            if (currentIndex < maxIndex) {
               maxIndex = currentIndex;
            }
         }
         else {
            /* Nothing to do here */
         }
         currentValuePtr++;
         currentIndexPtr++;
      }
      maxVals[buffer]    = largest;
      maxIndices[buffer] = ((uint32_t) maxIndex) + (buffer * maxSingleBufferSize);
   }

   T        largestVal   = maxVals[0];
   uint32_t largestIndex = maxIndices[0];
   for (i = 1; i < maxVals.size(); i++) {
      if (maxVals[i] > largestVal) {
         largestVal   = maxVals[i];
         largestIndex = maxIndices[i];
      }
   }

   *pOutLocal = largestIndex;
   // close SE0 and SE1
   __SE0_CLOSE();
   if (length > width) {
      __SE1_CLOSE();
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_maxIndex_exec_ci<int8_t, uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_maxIndex_exec_ci<uint8_t, uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_maxIndex_exec_ci<int16_t, uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_maxIndex_exec_ci<uint16_t, uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_maxIndex_exec_ci<int32_t, uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_maxIndex_exec_ci<uint32_t, uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_maxIndex_exec_ci<float, uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_maxIndex_exec_ci<double, uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template <typename T, typename TIndex> metadata<T, TIndex> DSPLIB_maxIndex_loopLogic(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of maximum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::uint_vec maxIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec maxIndices0 = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec maxIndices1 = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uint_vec maxIndicesA       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec maxIndicesB       = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec firstHalfIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec secondHalfIndices = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
#else
   c7x::uint_vec maxIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec maxIndices0 = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec maxIndices1 = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uint_vec  maxIndicesA       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec  maxIndicesB       = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uint_vec  firstHalfIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec  secondHalfIndices = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#endif
   // derive c7x vector type from value template and index template
   typedef typename c7x::make_full_vector<T>::type      vec;
   typedef typename c7x::make_full_vector<TIndex>::type index_vec;

   // holds all of the maximum values that have previously been read
   vec     maxVals0; // = T(std::numeric_limits<T>::lowest());
   vec     maxVals1; // = maxVals0;
   __vpred maskOfMaxs;

   // holds the overall max vals
   vec    maxVals;
   size_t width = c7x::element_count_of<vec>::value;
   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      maxVals = c7x::strm_eng<0, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         maxVals.s[i] = std::numeric_limits<T>::lowest();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      maxVals0 = c7x::strm_eng<0, vec>::get_adv();
      maxVals1 = c7x::strm_eng<1, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;
      for (size_t i = remElements; i < width; i++) {
         maxVals1.s[i] = std::numeric_limits<T>::lowest();
      }
      maskOfMaxs = __cmp_gt_pred(maxVals0, maxVals1);
      maxVals    = __select(maskOfMaxs, maxVals0, maxVals1);
      maxIndices = __select(maskOfMaxs, maxIndices0, maxIndices1);

      // get the maximum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfMaxs                             = __cmp_gt_pred(maxVals0, maxVals1);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(maxIndices1, maxIndices0);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndices0, maxIndices1);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(maxVals0, maxVals1);
      maxVals                                = __select(maskOfMaxs, maxVals0, maxVals1);
      index_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxs, maxIndices0, maxIndices1);
      index_vec zeroVec                      = c7x::uint_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      maxIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      vec     inVec0, inVec1;
      __vpred mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      vec maxValsA = T(std::numeric_limits<T>::lowest());
      vec maxValsB = maxValsA;

      // holds the overall max vals
      vec maxValsLarge = T(std::numeric_limits<T>::lowest());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, vec>::get_adv();
         mask0  = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA =
             __select(mask0, firstHalfIndices, maxIndicesA); // set the max indices to be the indices whose values have
                                                             // changed from the previous max values

         inVec1      = c7x::strm_eng<1, vec>::get_adv();
         mask1       = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB    = __select(mask1, inVec1, maxValsB);
         maxIndicesB = __select(mask1, secondHalfIndices, maxIndicesB);

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
         mask0            = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values
      }

      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0           = *(vec *) (remStart - width);
         firstHalfIndices = c7x::uint_vec(length - (2 * c7x::element_count_of<index_vec>::value)) + lastRunOffsets;
         mask0            = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values

         inVec1            = *(vec *) remStart;
         secondHalfIndices = c7x::uint_vec(length - (c7x::element_count_of<index_vec>::value)) + lastRunOffsets;
         mask1             = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB          = __select(mask1, inVec1, maxValsB);
         maxIndicesB       = __select(mask1, secondHalfIndices, maxIndicesB);
      }
      else {
         /* Nothing to do here */
      }

      // get the maximum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      __vpred   maskOfMaxValues              = __cmp_gt_pred(maxValsA, maxValsB);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(maxIndicesB, maxIndicesA);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndicesA, maxIndicesB);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(maxValsA, maxValsB);
      maxValsLarge                           = __select(maskOfMaxValues, maxValsA, maxValsB);
      index_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxValues, maxIndicesA, maxIndicesB);
      index_vec zeroVec                      = c7x::uint_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      index_vec maxIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      maxVals    = maxValsLarge;
      maxIndices = maxIndicesLarge;
   }
   metadata<T, TIndex> output;
   output.maxVals    = maxVals;
   output.maxIndices = maxIndices;
   return output;
}

// explicit templatization for int8_t type
template <> metadata<int8_t, uint8_t> DSPLIB_maxIndex_loopLogic<int8_t, uint8_t>(size_t length, void *pSrc)
{

#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of maximum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::uchar_vec maxIndices  = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec maxIndices0 = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec maxIndices1 = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                               50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uchar_vec maxIndicesA = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec maxIndicesB = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                               50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec firstHalfIndices  = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                                                     19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec secondHalfIndices = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                                                     49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#else
   c7x::uchar_vec maxIndices =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec maxIndices0 =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec maxIndices1 =
       c7x::uchar_vec(64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
                      88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                      110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uchar_vec maxIndicesA =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec maxIndicesB =
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

   // holds all of the maximum values that have previously been read
   vec     maxVals0; // = int8_t(std::numeric_limits<int8_t>::lowest());
   vec     maxVals1; // = maxVals0;
   __vpred maskOfMaxs;

   // holds the overall max vals
   vec    maxVals;
   size_t width = c7x::element_count_of<vec>::value;

   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      maxVals = c7x::strm_eng<0, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         maxVals.s[i] = std::numeric_limits<int8_t>::lowest();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      maxVals0 = c7x::strm_eng<0, vec>::get_adv();
      maxVals1 = c7x::strm_eng<1, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;
      for (size_t i = remElements; i < width; i++) {
         maxVals1.s[i] = std::numeric_limits<int8_t>::lowest();
      }
      // get the maximum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfMaxs                             = __cmp_gt_pred(maxVals0, maxVals1);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(maxIndices1, maxIndices0);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndices0, maxIndices1);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(maxVals0, maxVals1);
      maxVals                                = __select(maskOfMaxs, maxVals0, maxVals1);
      index_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxs, maxIndices0, maxIndices1);
      index_vec zeroVec                      = c7x::uchar_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      maxIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      vec     inVec0, inVec1;
      __vpred mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      vec maxValsA = int8_t(std::numeric_limits<int8_t>::lowest());
      vec maxValsB = maxValsA;

      // holds the overall max vals
      vec maxValsLarge = int8_t(std::numeric_limits<int8_t>::lowest());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, vec>::get_adv();
         mask0  = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA =
             __select(mask0, firstHalfIndices, maxIndicesA); // set the max indices to be the indices whose values have
                                                             // changed from the previous max values

         inVec1      = c7x::strm_eng<1, vec>::get_adv();
         mask1       = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB    = __select(mask1, inVec1, maxValsB);
         maxIndicesB = __select(mask1, secondHalfIndices, maxIndicesB);

         // update the new locations of the indices to be set for the next iteration
         firstHalfIndices += jumpFactorChar;
         secondHalfIndices += jumpFactorChar;
      }

      int32_t remBlockSize = length - (INDEX_UNROLL_FACTOR * numIterations * width);

      int32_t remVecLen = DSPLIB_ceilingDiv(remBlockSize, width);
      int8_t *remStart  = (int8_t *) pSrc + length - width;

      if (remBlockSize != 0 && remVecLen == 1) {
         inVec0 = *(vec *) remStart;

         firstHalfIndices = c7x::uchar_vec(length - width) + lastRunOffsetsChar;
         mask0            = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values
      }

      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0           = *(vec *) (remStart - width);
         firstHalfIndices = c7x::uchar_vec(length - 2 * width) + lastRunOffsetsChar;
         mask0            = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values

         inVec1            = *(vec *) remStart;
         secondHalfIndices = c7x::uchar_vec(length - width) + lastRunOffsetsChar;
         mask1             = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB          = __select(mask1, inVec1, maxValsB);
         maxIndicesB       = __select(mask1, secondHalfIndices, maxIndicesB);
      }
      else {
         /* Nothing to do here */
      }

      // get the maximum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      __vpred   maskOfMaxValues              = __cmp_gt_pred(maxValsA, maxValsB);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(maxIndicesB, maxIndicesA);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndicesA, maxIndicesB);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(maxValsA, maxValsB);
      maxValsLarge                           = __select(maskOfMaxValues, maxValsA, maxValsB);
      index_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxValues, maxIndicesA, maxIndicesB);
      index_vec zeroVec                      = c7x::uchar_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      index_vec maxIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      maxVals    = maxValsLarge;
      maxIndices = maxIndicesLarge;
   }
   metadata<int8_t, uint8_t> output;
   output.maxVals    = maxVals;
   output.maxIndices = maxIndices;
   return output;
}

// explicit templatization for uint8_t type
template <> metadata<uint8_t, uint8_t> DSPLIB_maxIndex_loopLogic<uint8_t, uint8_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of maximum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::uchar_vec maxIndices  = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec maxIndices0 = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec maxIndices1 = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                               50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uchar_vec maxIndicesA = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec maxIndicesB = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                               50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec firstHalfIndices  = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                                                     19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uchar_vec secondHalfIndices = c7x::uchar_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                                                     49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#else
   c7x::uchar_vec maxIndices =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec maxIndices0 =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec maxIndices1 =
       c7x::uchar_vec(64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
                      88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                      110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uchar_vec maxIndicesA =
       c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                      51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::uchar_vec maxIndicesB =
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

   // holds all of the maximum values that have previously been read
   vec     maxVals0; // = uint8_t(std::numeric_limits<uint8_t>::lowest());
   vec     maxVals1; // = maxVals0;
   __vpred maskOfMaxs;

   // holds the overall max vals
   vec    maxVals;
   size_t width = c7x::element_count_of<vec>::value;

   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      maxVals = c7x::strm_eng<0, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         maxVals.s[i] = std::numeric_limits<uint8_t>::lowest();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      maxVals0 = c7x::strm_eng<0, vec>::get_adv();
      maxVals1 = c7x::strm_eng<1, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;
      for (size_t i = remElements; i < width; i++) {
         maxVals1.s[i] = std::numeric_limits<uint8_t>::lowest();
      }
      // get the maximum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfMaxs                             = __cmp_gt_pred(maxVals0, maxVals1);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(maxIndices1, maxIndices0);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndices0, maxIndices1);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(maxVals0, maxVals1);
      maxVals                                = __select(maskOfMaxs, maxVals0, maxVals1);
      index_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxs, maxIndices0, maxIndices1);
      index_vec zeroVec                      = c7x::uchar_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      maxIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      vec     inVec0, inVec1;
      __vpred mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      vec maxValsA = uint8_t(std::numeric_limits<uint8_t>::lowest());
      vec maxValsB = maxValsA;

      // holds the overall max vals
      vec maxValsLarge = uint8_t(std::numeric_limits<uint8_t>::lowest());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, vec>::get_adv();
         mask0  = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA =
             __select(mask0, firstHalfIndices, maxIndicesA); // set the max indices to be the indices whose values have
                                                             // changed from the previous max values

         inVec1      = c7x::strm_eng<1, vec>::get_adv();
         mask1       = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB    = __select(mask1, inVec1, maxValsB);
         maxIndicesB = __select(mask1, secondHalfIndices, maxIndicesB);

         // update the new locations of the indices to be set for the next iteration
         firstHalfIndices += jumpFactorChar;
         secondHalfIndices += jumpFactorChar;
      }

      int32_t remBlockSize = length - (INDEX_UNROLL_FACTOR * numIterations * width);

      int32_t remVecLen = DSPLIB_ceilingDiv(remBlockSize, width);
      int8_t *remStart  = (int8_t *) pSrc + length - width;

      if (remBlockSize != 0 && remVecLen == 1) {
         inVec0 = *(vec *) remStart;

         firstHalfIndices = c7x::uchar_vec(length - width) + lastRunOffsetsChar;
         mask0            = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values
      }

      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0           = *(vec *) (remStart - width);
         firstHalfIndices = c7x::uchar_vec(length - 2 * width) + lastRunOffsetsChar;
         mask0            = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values

         inVec1            = *(vec *) remStart;
         secondHalfIndices = c7x::uchar_vec(length - width) + lastRunOffsetsChar;
         mask1             = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB          = __select(mask1, inVec1, maxValsB);
         maxIndicesB       = __select(mask1, secondHalfIndices, maxIndicesB);
      }
      else {
         /* Nothing to do here */
      }
      // get the maximum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      __vpred   maskOfMaxValues              = __cmp_gt_pred(maxValsA, maxValsB);
      __vpred   maskOfSmallerIndices         = __cmp_ge_pred(maxIndicesB, maxIndicesA);
      index_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndicesA, maxIndicesB);
      __vpred   maskOfTiebreakerValues       = __cmp_eq_pred(maxValsA, maxValsB);
      maxValsLarge                           = __select(maskOfMaxValues, maxValsA, maxValsB);
      index_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxValues, maxIndicesA, maxIndicesB);
      index_vec zeroVec                      = c7x::uchar_vec(0);
      index_vec nonTiebreakerVec             = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      index_vec tiebreakerVec                = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      index_vec maxIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      maxVals    = maxValsLarge;
      maxIndices = maxIndicesLarge;
   }

   metadata<uint8_t, uint8_t> output;
   output.maxVals    = maxVals;
   output.maxIndices = maxIndices;
   return output;
}

// explicit templatization for int16_t type
template <> metadata<int16_t, uint16_t> DSPLIB_maxIndex_loopLogic<int16_t, uint16_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of maximum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::ushort_vec maxIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec maxIndices0 = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec maxIndices1 = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ushort_vec maxIndicesA       = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec maxIndicesB       = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec firstHalfIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec secondHalfIndices = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#else
   c7x::ushort_vec maxIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec maxIndices0 = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec maxIndices1 = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                                 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ushort_vec maxIndicesA = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec maxIndicesB = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                                 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::ushort_vec firstHalfIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                                                       19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec secondHalfIndices = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                                                       48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#endif

   typedef typename c7x::make_full_vector<int16_t>::type  vec;
   typedef typename c7x::make_full_vector<uint16_t>::type index_vec;

   // holds all of the maximum values that have previously been read
   vec     maxVals0; // = int16_t(std::numeric_limits<int16_t>::lowest());
   vec     maxVals1; // = maxVals0;
   __vpred maskOfMaxs;

   // holds the overall max vals
   vec    maxVals;
   size_t width = c7x::element_count_of<vec>::value;

   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      maxVals = c7x::strm_eng<0, vec>::get_adv();

      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         maxVals.s[i] = std::numeric_limits<int16_t>::lowest();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      maxVals0 = c7x::strm_eng<0, vec>::get_adv();
      maxVals1 = c7x::strm_eng<1, vec>::get_adv();

      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;

      for (size_t i = remElements; i < width; i++) {
         maxVals1.s[i] = std::numeric_limits<int16_t>::lowest();
      }
      // get the maximum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfMaxs                                   = __cmp_gt_pred(maxVals0, maxVals1);
      __vpred         maskOfSmallerIndices         = __cmp_ge_pred(maxIndices1, maxIndices0);
      c7x::ushort_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndices0, maxIndices1);
      __vpred         maskOfTiebreakerValues       = __cmp_eq_pred(maxVals0, maxVals1);
      maxVals                                      = __select(maskOfMaxs, maxVals0, maxVals1);
      c7x::ushort_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxs, maxIndices0, maxIndices1);
      c7x::ushort_vec zeroVec                      = c7x::ushort_vec(0);
      c7x::ushort_vec nonTiebreakerVec = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      c7x::ushort_vec tiebreakerVec    = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      maxIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      c7x::short_vec inVec0, inVec1;
      __vpred        mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      c7x::short_vec maxValsA = int16_t(std::numeric_limits<int16_t>::lowest());
      c7x::short_vec maxValsB = maxValsA;

      // holds the overall max vals
      c7x::short_vec maxValsLarge = int16_t(std::numeric_limits<int16_t>::lowest());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, vec>::get_adv();
         mask0  = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA =
             __select(mask0, firstHalfIndices, maxIndicesA); // set the max indices to be the indices whose values have
                                                             // changed from the previous max values

         inVec1      = c7x::strm_eng<1, vec>::get_adv();
         mask1       = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB    = __select(mask1, inVec1, maxValsB);
         maxIndicesB = __select(mask1, secondHalfIndices, maxIndicesB);

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
         firstHalfIndices = c7x::ushort_vec(length - width) + lastRunOffsetsShort;

         mask0 = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values
      }

      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0           = *(vec *) (remStart - width);
         firstHalfIndices = c7x::ushort_vec(length - 2 * width) + lastRunOffsetsShort;
         mask0            = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values
         inVec1            = *(vec *) remStart;
         secondHalfIndices = c7x::ushort_vec(length - width) + lastRunOffsetsShort;
         mask1             = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB          = __select(mask1, inVec1, maxValsB);
         maxIndicesB       = __select(mask1, secondHalfIndices, maxIndicesB);
      }
      else {
         /* Nothing to do here */
      }
      __vpred         maskOfMaxValues              = __cmp_gt_pred(maxValsA, maxValsB);
      __vpred         maskOfSmallerIndices         = __cmp_ge_pred(maxIndicesB, maxIndicesA);
      c7x::ushort_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndicesA, maxIndicesB);
      __vpred         maskOfTiebreakerValues       = __cmp_eq_pred(maxValsA, maxValsB);
      maxValsLarge                                 = __select(maskOfMaxValues, maxValsA, maxValsB);
      c7x::ushort_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxValues, maxIndicesA, maxIndicesB);
      c7x::ushort_vec zeroVec                      = c7x::ushort_vec(0);
      c7x::ushort_vec nonTiebreakerVec = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      c7x::ushort_vec tiebreakerVec    = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      c7x::ushort_vec maxIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      maxVals    = maxValsLarge;
      maxIndices = maxIndicesLarge;
   }

   metadata<int16_t, uint16_t> output;
   output.maxVals    = maxVals;
   output.maxIndices = maxIndices;
   return output;
}

// explicit templatization for uint16_t type
template <> metadata<uint16_t, uint16_t> DSPLIB_maxIndex_loopLogic<uint16_t, uint16_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of maximum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::ushort_vec maxIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec maxIndices0 = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec maxIndices1 = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ushort_vec maxIndicesA       = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec maxIndicesB       = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec firstHalfIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ushort_vec secondHalfIndices = c7x::ushort_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#else
   c7x::ushort_vec maxIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec maxIndices0 = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec maxIndices1 = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                                 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ushort_vec maxIndicesA = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec maxIndicesB = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                                 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
   c7x::ushort_vec firstHalfIndices  = c7x::ushort_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                                                       19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::ushort_vec secondHalfIndices = c7x::ushort_vec(32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                                                       48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#endif

   typedef typename c7x::make_full_vector<uint16_t>::type vec;
   typedef typename c7x::make_full_vector<uint16_t>::type index_vec;

   // holds all of the maximum values that have previously been read
   vec     maxVals0; // = uint16_t(std::numeric_limits<uint16_t>::lowest());
   vec     maxVals1; // = maxVals0;
   __vpred maskOfMaxs;

   // holds the overall max vals
   vec    maxVals;
   size_t width = c7x::element_count_of<vec>::value;

   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      maxVals = c7x::strm_eng<0, vec>::get_adv();

      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         maxVals.s[i] = std::numeric_limits<uint16_t>::lowest();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      maxVals0 = c7x::strm_eng<0, vec>::get_adv();
      maxVals1 = c7x::strm_eng<1, vec>::get_adv();

      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;

      for (size_t i = remElements; i < width; i++) {
         maxVals1.s[i] = std::numeric_limits<uint16_t>::lowest();
      }
      // get the maximum values and their corresponding indices into single vectors while properly handling tiebreakers
      // as well
      maskOfMaxs                                   = __cmp_gt_pred(maxVals0, maxVals1);
      __vpred         maskOfSmallerIndices         = __cmp_ge_pred(maxIndices1, maxIndices0);
      c7x::ushort_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndices0, maxIndices1);
      __vpred         maskOfTiebreakerValues       = __cmp_eq_pred(maxVals0, maxVals1);
      maxVals                                      = __select(maskOfMaxs, maxVals0, maxVals1);
      c7x::ushort_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxs, maxIndices0, maxIndices1);
      c7x::ushort_vec zeroVec                      = c7x::ushort_vec(0);
      c7x::ushort_vec nonTiebreakerVec = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      c7x::ushort_vec tiebreakerVec    = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      maxIndices = nonTiebreakerVec + tiebreakerVec;
   }
   else {
      // input vectors
      c7x::ushort_vec inVec0, inVec1;
      __vpred         mask0, mask1;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      c7x::ushort_vec maxValsA = uint16_t(std::numeric_limits<uint16_t>::lowest());
      c7x::ushort_vec maxValsB = maxValsA;

      // holds the overall max vals
      c7x::ushort_vec maxValsLarge = uint16_t(std::numeric_limits<uint16_t>::lowest());

      size_t numIterations = length / (INDEX_UNROLL_FACTOR * width);

      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, c7x::ushort_vec>::get_adv();
         mask0  = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA =
             __select(mask0, firstHalfIndices, maxIndicesA); // set the max indices to be the indices whose values have
                                                             // changed from the previous max values

         inVec1      = c7x::strm_eng<1, c7x::ushort_vec>::get_adv();
         mask1       = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB    = __select(mask1, inVec1, maxValsB);
         maxIndicesB = __select(mask1, secondHalfIndices, maxIndicesB);

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
         firstHalfIndices = c7x::ushort_vec(length - width) + lastRunOffsetsShort;

         mask0 = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values
      }
      else if (remBlockSize != 0 && remVecLen == 2) {
         inVec0           = *(vec *) (remStart - width);
         firstHalfIndices = c7x::ushort_vec(length - 2 * width) + lastRunOffsetsShort;
         mask0            = __cmp_gt_pred(inVec0, maxValsA);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values
         inVec1            = *(vec *) remStart;
         secondHalfIndices = c7x::ushort_vec(length - width) + lastRunOffsetsShort;
         mask1             = __cmp_gt_pred(inVec1, maxValsB);
         maxValsB          = __select(mask1, inVec1, maxValsB);
         maxIndicesB       = __select(mask1, secondHalfIndices, maxIndicesB);
      }
      else {
         /* Nothing to do here */
      }
      __vpred         maskOfMaxValues              = __cmp_gt_pred(maxValsA, maxValsB);
      __vpred         maskOfSmallerIndices         = __cmp_ge_pred(maxIndicesB, maxIndicesA);
      c7x::ushort_vec smallestIndices              = __select(maskOfSmallerIndices, maxIndicesA, maxIndicesB);
      __vpred         maskOfTiebreakerValues       = __cmp_eq_pred(maxValsA, maxValsB);
      maxValsLarge                                 = __select(maskOfMaxValues, maxValsA, maxValsB);
      c7x::ushort_vec maxIndicesIgnoringTiebreaker = __select(maskOfMaxValues, maxIndicesA, maxIndicesB);
      c7x::ushort_vec zeroVec                      = c7x::ushort_vec(0);
      c7x::ushort_vec nonTiebreakerVec = __select(maskOfTiebreakerValues, zeroVec, maxIndicesIgnoringTiebreaker);
      c7x::ushort_vec tiebreakerVec    = __select(maskOfTiebreakerValues, smallestIndices, zeroVec);

      c7x::ushort_vec maxIndicesLarge = nonTiebreakerVec + tiebreakerVec;

      maxVals    = maxValsLarge;
      maxIndices = maxIndicesLarge;
   }

   metadata<uint16_t, uint16_t> output;
   output.maxVals    = maxVals;
   output.maxIndices = maxIndices;
   return output;
}

// explicit templatization for float type
template <> metadata<float, uint32_t> DSPLIB_maxIndex_loopLogic<float, uint32_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of maximum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::uint_vec maxIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec maxIndices0 = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec maxIndices1 = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uint_vec maxIndicesA       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec maxIndicesB       = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec firstHalfIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::uint_vec secondHalfIndices = c7x::uint_vec(8, 9, 10, 11, 12, 13, 14, 15);
#else
   c7x::uint_vec   maxIndices        = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec   maxIndices0       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec   maxIndices1       = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::uint_vec  maxIndicesA       = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec  maxIndicesB       = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
   c7x::uint_vec  firstHalfIndices  = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
   c7x::uint_vec  secondHalfIndices = c7x::uint_vec(16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#endif
   // holds all of the maximum values that have previously been read
   c7x::float_vec maxVals0; // = std::numeric_limits<float>::lowest();
   c7x::float_vec maxVals1; // = maxVals0;
   __vpred        maskOfMaxs;

   size_t width = c7x::element_count_of<c7x::float_vec>::value;

   // holds the overall max vals
   c7x::float_vec maxVals;
   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      maxVals = c7x::strm_eng<0, c7x::float_vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         maxVals.s[i] = std::numeric_limits<float>::lowest();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      maxVals0 = c7x::strm_eng<0, c7x::float_vec>::get_adv();
      maxVals1 = c7x::strm_eng<1, c7x::float_vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;
      for (size_t i = remElements; i < width; i++) {
         maxVals1.s[i] = std::numeric_limits<float>::lowest();
      }
      maskOfMaxs = __cmp_lt_pred(maxVals1, maxVals0);
      maxVals    = __select(maskOfMaxs, maxVals0, maxVals1);
      maxIndices = __select(maskOfMaxs, maxIndices0, maxIndices1);
   }
   else {
      // input vectors
      c7x::float_vec inVec0, inVec1;
      __vpred        mask0, mask1, maskOfMaxsLarge;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      c7x::float_vec maxValsA = std::numeric_limits<float>::lowest();
      c7x::float_vec maxValsB = maxValsA;

      // holds the overall max vals
      c7x::float_vec maxValsLarge  = std::numeric_limits<float>::lowest();
      size_t         numIterations = length / (INDEX_UNROLL_FACTOR * width);
      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, c7x::float_vec>::get_adv();
         mask0  = __cmp_lt_pred(maxValsA, inVec0);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA =
             __select(mask0, firstHalfIndices, maxIndicesA); // set the max indices to be the indices whose values have
                                                             // changed from the previous max values

         inVec1      = c7x::strm_eng<1, c7x::float_vec>::get_adv();
         mask1       = __cmp_lt_pred(maxValsB, inVec1);
         maxValsB    = __select(mask1, inVec1, maxValsB);
         maxIndicesB = __select(mask1, secondHalfIndices, maxIndicesB);

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
         mask0            = __cmp_lt_pred(maxValsA, inVec0);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values
      }

      // if remainder 2
      else if (remBlockSize != 0 && remVecLen == 2) {

         inVec0           = *(c7x::float_vec *) (remStart - width);
         firstHalfIndices = c7x::uint_vec(length - (2 * width)) + lastRunOffsets;
         mask0            = __cmp_lt_pred(maxValsA, inVec0);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values

         inVec1            = *(c7x::float_vec *) remStart;
         secondHalfIndices = c7x::uint_vec(length - (width)) + lastRunOffsets;
         mask1             = __cmp_lt_pred(maxValsB, inVec1);
         maxValsB          = __select(mask1, inVec1, maxValsB);
         maxIndicesB       = __select(mask1, secondHalfIndices, maxIndicesB);
      }
      else {
         /* Nothing to do here */
      }
      maskOfMaxsLarge               = __cmp_lt_pred(maxValsB, maxValsA);
      maxValsLarge                  = __select(maskOfMaxsLarge, maxValsA, maxValsB);
      c7x::uint_vec maxIndicesLarge = __select(maskOfMaxsLarge, maxIndicesA, maxIndicesB);

      // metadata<float, uint32_t> outputLarge;
      maxVals    = maxValsLarge;
      maxIndices = maxIndicesLarge;
   }
   metadata<float, uint32_t> output;
   output.maxVals    = maxVals;
   output.maxIndices = maxIndices;
   return output;
}

// explicit templatization for double type
template <> metadata<double, uint64_t> DSPLIB_maxIndex_loopLogic<double, uint64_t>(size_t length, void *pSrc)
{
#if defined(__C7504__) || defined(__C7524__)
   // vector containing indices of maximum indices (starts with 0, ..., SIMD width but changes as we loop through)
   c7x::ulong_vec maxIndices  = c7x::ulong_vec(0, 1, 2, 3);
   c7x::ulong_vec maxIndices0 = c7x::ulong_vec(0, 1, 2, 3);
   c7x::ulong_vec maxIndices1 = c7x::ulong_vec(4, 5, 6, 7);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ulong_vec maxIndicesA       = c7x::ulong_vec(0, 1, 2, 3);
   c7x::ulong_vec maxIndicesB       = c7x::ulong_vec(4, 5, 6, 7);
   c7x::ulong_vec firstHalfIndices  = c7x::ulong_vec(0, 1, 2, 3);
   c7x::ulong_vec secondHalfIndices = c7x::ulong_vec(4, 5, 6, 7);
#else
   c7x::ulong_vec maxIndices        = c7x::ulong_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::ulong_vec maxIndices0       = c7x::ulong_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::ulong_vec maxIndices1       = c7x::ulong_vec(8, 9, 10, 11, 12, 13, 14, 15);
   // re-defined for large widths so that there's no common vectors used between small and large widths
   c7x::ulong_vec maxIndicesA       = c7x::ulong_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::ulong_vec maxIndicesB       = c7x::ulong_vec(8, 9, 10, 11, 12, 13, 14, 15);
   c7x::ulong_vec firstHalfIndices  = c7x::ulong_vec(0, 1, 2, 3, 4, 5, 6, 7);
   c7x::ulong_vec secondHalfIndices = c7x::ulong_vec(8, 9, 10, 11, 12, 13, 14, 15);
#endif

   c7x::double_vec maxVals0; // = std::numeric_limits<double>::lowest();
   c7x::double_vec maxVals1; // = maxVals0;
   __vpred         maskOfMaxs;

   size_t width = c7x::element_count_of<c7x::double_vec>::value;

   // holds the overall max vals
   c7x::double_vec maxVals;
   // can only fill part of one width - all we need to do is fill in the uninitialized values with MIN_VAL
   if (length <= width) {
      maxVals = c7x::strm_eng<0, c7x::double_vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      for (size_t i = length; i < width; i++) {
         maxVals.s[i] = std::numeric_limits<double>::lowest();
      }
   }
   // can fill one width but only part of a second
   else if (length < 2 * width) {
      maxVals0 = c7x::strm_eng<0, c7x::double_vec>::get_adv();
      maxVals1 = c7x::strm_eng<1, c7x::double_vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remainingElement = length % width;
      for (size_t i = remainingElement; i < width; i++) {
         maxVals1.s[i] = std::numeric_limits<double>::lowest();
      }
      maskOfMaxs = __cmp_lt_pred(maxVals1, maxVals0);
      maxVals    = __select(maskOfMaxs, maxVals0, maxVals1);
      maxIndices = __select(maskOfMaxs, maxIndices0, maxIndices1);
   }
   else {
      // input vectors
      c7x::double_vec inVec0, inVec1;
      __vpred         mask0, mask1, maskOfMaxsLarge;
      // redefine the vectors used in small loops for large loops since .s[i] calls make random calls to the stack even
      // when not being used, which will increase the ii
      c7x::double_vec maxValsA = std::numeric_limits<double>::lowest();
      c7x::double_vec maxValsB = maxValsA;

      // holds the overall max vals
      c7x::double_vec maxValsLarge  = std::numeric_limits<double>::lowest();
      size_t          numIterations = length / (INDEX_UNROLL_FACTOR * width);
      for (size_t i = 0; i < numIterations; i += 1) {
         inVec0 = c7x::strm_eng<0, c7x::double_vec>::get_adv();
         mask0  = __cmp_lt_pred(maxValsA, inVec0);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA =
             __select(mask0, firstHalfIndices, maxIndicesA); // set the max indices to be the indices whose values have
                                                             // changed from the previous max values

         inVec1      = c7x::strm_eng<1, c7x::double_vec>::get_adv();
         mask1       = __cmp_lt_pred(maxValsB, inVec1);
         maxValsB    = __select(mask1, inVec1, maxValsB);
         maxIndicesB = __select(mask1, secondHalfIndices, maxIndicesB);

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
         mask0            = __cmp_lt_pred(maxValsA, inVec0);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values
      }

      // if remainder 2
      else if (remBlockSize != 0 && remVecLen == 2) {

         inVec0           = *(c7x::double_vec *) (remStart - width);
         firstHalfIndices = c7x::ulong_vec(length - (2 * width)) + lastRunOffsetsDp;
         mask0            = __cmp_lt_pred(maxValsA, inVec0);
         maxValsA =
             __select(mask0, inVec0, maxValsA); // change the values in the max val vector depending on which positions
                                                // have been shown to have larger values (contained in mask)
         maxIndicesA = __select(mask0, firstHalfIndices,
                                maxIndicesA); // set the max indices to be the indices whose values have changed from
                                              // the previous max values

         inVec1            = *(c7x::double_vec *) remStart;
         secondHalfIndices = c7x::ulong_vec(length - (width)) + lastRunOffsetsDp;
         mask1             = __cmp_lt_pred(maxValsB, inVec1);
         maxValsB          = __select(mask1, inVec1, maxValsB);
         maxIndicesB       = __select(mask1, secondHalfIndices, maxIndicesB);
      }
      else {
         /* Do nothing */
      }

      maskOfMaxsLarge                = __cmp_lt_pred(maxValsB, maxValsA);
      maxValsLarge                   = __select(maskOfMaxsLarge, maxValsA, maxValsB);
      c7x::ulong_vec maxIndicesLarge = __select(maskOfMaxsLarge, maxIndicesA, maxIndicesB);

      maxVals    = maxValsLarge;
      maxIndices = maxIndicesLarge;
   }

   metadata<double, uint64_t> output;
   output.maxVals    = maxVals;
   output.maxIndices = maxIndices;
   return output;
}
