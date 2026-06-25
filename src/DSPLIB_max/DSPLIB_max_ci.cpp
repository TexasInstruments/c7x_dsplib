// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_max_priv.h"
#include <float.h>
#include <limits>

#define UNROLL_FACTOR 4

template <typename dataType>
DSPLIB_STATUS DSPLIB_max_init_ci(DSPLIB_kernelHandle         handle,
                                 const DSPLIB_bufParams1D_t *bufParamsIn,
                                 const DSPLIB_bufParams1D_t *bufParamsOut,
                                 const DSPLIB_max_InitArgs  *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params, se1Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;

   DSPLIB_max_PrivArgs *pKerPrivArgs = (DSPLIB_max_PrivArgs *) handle;

   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   uint32_t blockSize = pKerPrivArgs->blockSize;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;
   SE_VECLEN                                                       = c7x::se_veclen<vec>::value;
   SE_ELETYPE                                                      = c7x::se_eletype<vec>::value;

   uint32_t length = blockSize;
   uint32_t width  = eleCount;

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

   uint32_t numBlocks     = length / width;
   uint32_t remBlocksSize = length % width;
   if (remBlocksSize) {
      numBlocks++;
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

   // case: len >= 2*width but < 3*width
   // SE0 and SE1 full fetch, remainder from direct memory
   else if (length < 3 * width) {
      // SE0 full fetch
      se0Params.ICNT0 = width;
      // SE1 full fetch
      se1Params.ICNT0 = width;
   }

   // case: len >= 3*width but < 4*width
   // 3 full fetches - SE0 2 times, SE1 once
   else if (length < 4 * width) {
      // SE0 2 full fetches
      se0Params.DIMFMT = __SE_DIMFMT_2D;
      se0Params.DIM1   = 2 * width;
      se0Params.ICNT1  = 2;
      se0Params.ICNT0  = width;
      // SE1 partial fetch
      se1Params.ICNT0 = width;
   }

   // case: len >= 4*width
   // SE0 and SE1 fetches are full widths only
   else {

      // SE0 Dim is 2D
      se0Params.DIMFMT = __SE_DIMFMT_2D;
      // SE0 jump length each get_adv is 2 widths
      se0Params.DIM1 = 2 * width;
      // SE only performs full fetches in multiples of UNROLL_FACTOR, i.e. 4
      se0Params.ICNT1 = length / (((uint32_t) UNROLL_FACTOR >> (uint32_t) 1) * width);
      // SE0 fetches full widths
      se0Params.ICNT0 = width;

      // SE1 fetches in same manner as SE0, but starts 1 width ahead
      se1Params = se0Params;
   }

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_max_init_ci<int16_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams1D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsOut,
                                                   const DSPLIB_max_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_max_init_ci<uint16_t>(DSPLIB_kernelHandle         handle,
                                                    const DSPLIB_bufParams1D_t *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t *bufParamsOut,
                                                    const DSPLIB_max_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_max_init_ci<int32_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams1D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsOut,
                                                   const DSPLIB_max_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_max_init_ci<uint32_t>(DSPLIB_kernelHandle         handle,
                                                    const DSPLIB_bufParams1D_t *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t *bufParamsOut,
                                                    const DSPLIB_max_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_max_init_ci<int8_t>(DSPLIB_kernelHandle         handle,
                                                  const DSPLIB_bufParams1D_t *bufParamsIn,
                                                  const DSPLIB_bufParams1D_t *bufParamsOut,
                                                  const DSPLIB_max_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_max_init_ci<uint8_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams1D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsOut,
                                                   const DSPLIB_max_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_max_init_ci<float>(DSPLIB_kernelHandle         handle,
                                                 const DSPLIB_bufParams1D_t *bufParamsIn,
                                                 const DSPLIB_bufParams1D_t *bufParamsOut,
                                                 const DSPLIB_max_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_max_init_ci<double>(DSPLIB_kernelHandle         handle,
                                                  const DSPLIB_bufParams1D_t *bufParamsIn,
                                                  const DSPLIB_bufParams1D_t *bufParamsOut,
                                                  const DSPLIB_max_InitArgs  *pKerInitArgs);

template <typename dataType, int32_t dataIn>
DSPLIB_STATUS DSPLIB_max_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_max_PrivArgs *pKerPrivArgs = (DSPLIB_max_PrivArgs *) handle;
   uint32_t             blockSize    = pKerPrivArgs->blockSize;

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 se0Params, se1Params;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_max_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   se0Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   uint32_t length = blockSize;
   uint32_t width  = eleCount;

   // Input samples
   __SE0_OPEN(pInLocal, se0Params);
   if (length > width) {
      __SE1_OPEN(pInLocal + eleCount, se1Params);
   }

   // // Output samples

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   // call loop logic method
   vec maxVec = DSPLIB_max_loopLogic<dataType, vec>(blockSize, eleCount, pInLocal);

   dataType *currentMax = (dataType *) &maxVec;
   dataType  largest    = *currentMax++;
   dataType  currentValue;

   for (size_t i = 1; i < c7x::element_count_of<vec>::value; i++) {
      currentValue = *currentMax;

      if (currentValue > largest) {
         largest = currentValue;
      }

      currentMax++;
   }

   *pOutLocal = largest;

   __SE0_CLOSE();
   if (length > width) {
      __SE1_CLOSE();
   }

   return status;
}

template <>
DSPLIB_STATUS
DSPLIB_max_exec_ci<int8_t, DSPLIB_INT8>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   DSPLIB_max_PrivArgs *pKerPrivArgs = (DSPLIB_max_PrivArgs *) handle;
   uint32_t             blockSize    = pKerPrivArgs->blockSize;

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 se0Params;

   int8_t *restrict pInLocal  = (int8_t *) pIn;
   int8_t *restrict pOutLocal = (int8_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_max_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<int8_t>::type vec;
   uint32_t                                             eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   se0Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + eleCount, se0Params);

   // call loop logic method
   vec maxVec = DSPLIB_max_loopLogic<int8_t, vec>(blockSize, eleCount, pInLocal);

   c7x::short_vec lowShorts    = __low_char_to_short(maxVec);
   c7x::short_vec highShorts   = __high_char_to_short(maxVec);
   c7x::int_vec   lowlowInts   = __low_short_to_int(lowShorts);
   c7x::int_vec   highlowInts  = __high_short_to_int(lowShorts);
   c7x::int_vec   lowhighInts  = __low_short_to_int(highShorts);
   c7x::int_vec   highhighInts = __high_short_to_int(highShorts);
   c7x::int_vec   lowmax       = __max(lowlowInts, highlowInts);
   c7x::int_vec   highmax      = __max(lowhighInts, highhighInts);
   c7x::int_vec   maxOfInts    = __max(lowmax, highmax);
   maxOfInts                   = __sort_desc(maxOfInts); // this intrinsic contains valgrind errors
   // convert back to char
   *pOutLocal = (int8_t) maxOfInts.s[0];
   // close SE0, SE1
   __SE0_CLOSE();
   __SE1_CLOSE();

   return status;
}

template <>
DSPLIB_STATUS
DSPLIB_max_exec_ci<uint8_t, DSPLIB_UINT8>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   DSPLIB_max_PrivArgs *pKerPrivArgs = (DSPLIB_max_PrivArgs *) handle;
   uint32_t             blockSize    = pKerPrivArgs->blockSize;

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 se0Params;

   uint8_t *restrict pInLocal  = (uint8_t *) pIn;
   uint8_t *restrict pOutLocal = (uint8_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_max_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<uint8_t>::type vec;
   uint32_t                                              eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   se0Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + eleCount, se0Params);

   // call loop logic method
   vec maxVec = DSPLIB_max_loopLogic<uint8_t, vec>(blockSize, eleCount, pInLocal);

   c7x::ushort_vec lowShorts  = __low_uchar_to_ushort(maxVec);
   c7x::ushort_vec highShorts = __high_uchar_to_ushort(maxVec);
   c7x::uint_vec   maxOfInts  = __max((__max((__low_ushort_to_uint(lowShorts)), (__high_ushort_to_uint(lowShorts)))),
                                      (__max((__low_ushort_to_uint(highShorts)), (__high_ushort_to_uint(highShorts)))));
   maxOfInts                  = __sort_desc(maxOfInts);
   // convert back to char
   *pOutLocal = (uint8_t) maxOfInts.s[0];
   // close SE0, SE1
   __SE0_CLOSE();
   __SE1_CLOSE();

   return status;
}

template <>
DSPLIB_STATUS
DSPLIB_max_exec_ci<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   DSPLIB_max_PrivArgs *pKerPrivArgs = (DSPLIB_max_PrivArgs *) handle;
   uint32_t             blockSize    = pKerPrivArgs->blockSize;

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 se0Params;

   int16_t *restrict pInLocal  = (int16_t *) pIn;
   int16_t *restrict pOutLocal = (int16_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_max_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<int16_t>::type vec;
   uint32_t                                              eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   se0Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + eleCount, se0Params);

   // call loop logic method
   vec maxVec = DSPLIB_max_loopLogic<int16_t, vec>(blockSize, eleCount, pInLocal);

   c7x::int_vec lowVec  = __low_short_to_int(maxVec);
   c7x::int_vec highVec = __high_short_to_int(maxVec);

   c7x::int_vec maxOfInts = __max(lowVec, highVec);
   maxOfInts              = __sort_desc(maxOfInts);
   // convert back to char
   *pOutLocal = (int16_t) maxOfInts.s[0];
   // close SE0, SE1
   __SE0_CLOSE();
   __SE1_CLOSE();

   return status;
}

template <>
DSPLIB_STATUS
DSPLIB_max_exec_ci<uint16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   DSPLIB_max_PrivArgs *pKerPrivArgs = (DSPLIB_max_PrivArgs *) handle;
   uint32_t             blockSize    = pKerPrivArgs->blockSize;

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 se0Params;

   uint16_t *restrict pInLocal  = (uint16_t *) pIn;
   uint16_t *restrict pOutLocal = (uint16_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_max_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<uint16_t>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   se0Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + eleCount, se0Params);

   // call loop logic method
   vec maxVec = DSPLIB_max_loopLogic<uint16_t, vec>(blockSize, eleCount, pInLocal);

   c7x::uint_vec lowVec    = __low_ushort_to_uint(maxVec);
   c7x::uint_vec highVec   = __high_ushort_to_uint(maxVec);
   c7x::uint_vec maxOfInts = __max(lowVec, highVec);
   maxOfInts               = __sort_desc(maxOfInts);
   // convert back to char
   *pOutLocal = (uint16_t) maxOfInts.s[0];
   // close SE0, SE1
   __SE0_CLOSE();
   __SE1_CLOSE();

   return status;
}

template DSPLIB_STATUS
DSPLIB_max_exec_ci<int32_t, DSPLIB_INT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_max_exec_ci<uint32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_max_exec_ci<float, DSPLIB_FLOAT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_max_exec_ci<double, DSPLIB_FLOAT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template <typename T, typename vec> vec DSPLIB_max_loopLogic(size_t length, size_t width, T *pSrc)
{
   vec maxVal0 = (vec) std::numeric_limits<T>::lowest();
   vec maxVal1 = maxVal0;
   vec maxVec  = maxVal1; // vector that holds the maximum values at the end of the loops

   // can only fill part of one width
   if (length <= width) {
      maxVec = c7x::strm_eng<0, vec>::get_adv();
      // maxVec.print();
      // fill the uninitialized values with MIN_VAL (different from ::min() because ::min() is 0 for float type)
      for (size_t i = length; i < width; i++) {
         maxVec.s[i] = std::numeric_limits<T>::lowest();
      }

      // debug
      // maxVec.print();
   }
   // can fill one with, but only part of a second
   else if (length < 2 * width) {

      vec inVec0 = c7x::strm_eng<0, vec>::get_adv();
      maxVal0    = __max(inVec0, maxVal0);
      vec inVec1 = c7x::strm_eng<1, vec>::get_adv();
      // fill the uninitialized values with MIN_VAL
      size_t remElements = length % width;
      for (size_t i = remElements; i < width; i++) {
         inVec1.s[i] = std::numeric_limits<T>::lowest();
      }
      maxVal1 = __max(inVec1, maxVal1);
      maxVec  = __max(maxVal0, maxVal1);
   }
   // can fill two widths, but only part of a third
   else if (length < 3 * width) {

      vec inVec0        = c7x::strm_eng<0, vec>::get_adv();
      vec inVec1        = c7x::strm_eng<1, vec>::get_adv();
      vec maxOfFirstTwo = __max(inVec0, inVec1);
      vec inVec2        = *(vec *) (pSrc + length - width);
      // fill the uninitialized values with MIN_VAL
      maxVec = __max(maxOfFirstTwo, inVec2);

      // debug
      // maxVec.print();
   }
   // can fill three widths, but only part of a fourth
   else if (length < 4 * width) {

      vec inVec0 = c7x::strm_eng<0, vec>::get_adv();
      vec inVec1 = c7x::strm_eng<1, vec>::get_adv();
      // inVec0.print();
      // inVec1.print();
      vec maxOfFirstTwo = __max(inVec0, inVec1);
      // maxOfFirstTwo.print();

      vec inVec2 = c7x::strm_eng<0, vec>::get_adv();
      vec inVec3 = *(vec *) (pSrc + length - width);
      // inVec2.print();
      // inVec3.print();

      // fill the uninitialized values with MIN_VAL
      vec maxOfLastTwo = __max(inVec2, inVec3);
      maxVec           = __max(maxOfFirstTwo, maxOfLastTwo);

      // debug
      // maxVec.print();
   }
   else {
      // redefine all of the max vectors for large loops since .s[i] calls make random calls to the stack even when not
      // being used, which will increase the ii
      vec maxValA = (vec) std::numeric_limits<T>::lowest();
      vec maxValB = maxValA;
      vec maxValC = maxValB;
      vec maxValD = maxValC;
      // loop through all the SIMD width blocks
      size_t numIterations = length / (width * UNROLL_FACTOR); // always read a multiple of SIMD width values

      for (size_t i = 0; i < numIterations; i += 1) {
         // put in parenthesis around the get advance call when it's being used in an argument to a c7x instrinsic
         // because the Ninja compiler will not compile due to "not enough arguments"
         maxValA = __max((c7x::strm_eng<0, vec>::get_adv()), maxValA);
         maxValB = __max((c7x::strm_eng<1, vec>::get_adv()), maxValB);
         maxValC = __max((c7x::strm_eng<0, vec>::get_adv()), maxValC);
         maxValD = __max((c7x::strm_eng<1, vec>::get_adv()), maxValD);
      }

      // number of remaining elements is less than 4 vector lengths
      // read remaining vector elements after full iterations
      // best case scenario: remaining is one vector length
      // worst case scenario: remaining is three vectors
      int32_t remBlockSize = length - (UNROLL_FACTOR * numIterations * width);
      int32_t remVecLen    = DSPLIB_ceilingDiv(remBlockSize, width);

      // vec remVec   = (vec) std::numeric_limits<T>::max();

      T  *remStart = pSrc + length - width;
      vec remVec   = *(vec *) remStart;
      // 1 vector remainder
      if (remBlockSize != 0 && remVecLen == 1) {

         remVec = *(vec *) remStart;
      }

      // 2 vector remainder
      else if (remBlockSize != 0 && remVecLen == 2) {

         // vec remVec0 = c7x::strm_eng<0, vec>::get_adv();
         vec remVec0 = *(vec *) (remStart - width);
         remVec      = __max(remVec0, remVec);
      }

      // 3 vector remainder
      else if (remBlockSize != 0 && remVecLen == 3) {

         // vec remVec0 = c7x::strm_eng<0, vec>::get_adv();
         // vec remVec1 = c7x::strm_eng<1, vec>::get_adv();
         vec remVec0 = *(vec *) (remStart - width);
         vec remVec1 = *(vec *) (remStart - 2 * width);

         vec remVec0_1 = __max(remVec0, remVec1);
         remVec        = __max(remVec0_1, remVec);
      }

      // 4 vector remainder
      else if (remBlockSize != 0 && remVecLen == 4) {

         // vec remVec0 = c7x::strm_eng<0, vec>::get_adv();
         // vec remVec1 = c7x::strm_eng<1, vec>::get_adv();
         // vec remVec2 = c7x::strm_eng<0, vec>::get_adv();

         vec remVec0   = *(vec *) (remStart - width);
         vec remVec1   = *(vec *) (remStart - 2 * width);
         vec remVec2   = *(vec *) (remStart - 3 * width);
         vec remVec0_1 = __max(remVec0, remVec1);
         vec remVec2_3 = __max(remVec, remVec2);
         remVec        = __max(remVec0_1, remVec2_3);
      }

      else {
         /* Nothing to do here */
      }

      // std::cout << "print maxvals" << std::endl;
      // maxValA.print();
      // maxValB.print();
      // maxValC.print();
      // maxValD.print();
      vec temp1     = __max(maxValA, maxValB);
      vec temp2     = __max(maxValC, maxValD);
      vec maxVecA   = __max(temp1, temp2);
      vec maxRemVec = __max(maxVecA, remVec);
      maxVec        = maxRemVec;
   }
   return maxVec;
}
