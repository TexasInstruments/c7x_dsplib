// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bexp_priv.h"

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define LAST_VEC_AND_MASK_OFFSET (SE_PARAM_BASE)

/**
 * Init function of the bexp kernel to setup the SE
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_bexp_init_ci(DSPLIB_kernelHandle         handle,
                                  const DSPLIB_bufParams1D_t *bufParamsIn,
                                  const DSPLIB_bufParams1D_t *bufParamsOut,
                                  const DSPLIB_bexp_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   uint32_t blockSize = pKerPrivArgs->blockSize;

   typedef typename c7x::make_full_vector<dataType>::type Vec;
   uint32_t                                               eleCount = (int32_t) c7x::element_count_of<Vec>::value;
   uint32_t                                               numVecs  = (blockSize + eleCount - 1) / eleCount;
   if (numVecs < DSPLIB_BEXP_IXX_IXX_OXX_MIN_VECS_TO_ENABLE_SE_READS) {
      pKerPrivArgs->isSEReadEnabled = false;
      pKerPrivArgs->mainLoopCount   = numVecs;

      dataType *pAndMask    = (dataType *) ((uint8_t *) pBlock + LAST_VEC_AND_MASK_OFFSET);
      uint32_t  remElements = numVecs * eleCount - blockSize;
      for (int32_t i = 0; i < (int32_t) eleCount; i++) {
         pAndMask[i] = (dataType) 0;
      }
      for (int32_t i = 0; i < (int32_t) (eleCount - remElements); i++) {
         pAndMask[i] = (dataType) 0xFFFFFFFFFFFFFFFFU;
      }
   }
   else {
      pKerPrivArgs->isSEReadEnabled           = true;
      pKerPrivArgs->mainLoopCount             = (numVecs + 1) / 2;
      pKerPrivArgs->se1StartAddressByteOffset = (numVecs / 2) * eleCount * sizeof(dataType);

      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      __SE_ELETYPE     SE_ELETYPE;
      __SE_VECLEN      SE_VECLEN;

      SE_VECLEN  = c7x::se_veclen<Vec>::value;
      SE_ELETYPE = c7x::se_eletype<Vec>::value;

      se0Params = __gen_SE_TEMPLATE_v1();

      se0Params.ICNT0   = (numVecs - pKerPrivArgs->mainLoopCount) * eleCount;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DIMFMT  = __SE_DIMFMT_1D;

      se1Params = __gen_SE_TEMPLATE_v1();

      se1Params.ICNT0   = blockSize - se0Params.ICNT0;
      se1Params.ELETYPE = SE_ELETYPE;
      se1Params.VECLEN  = SE_VECLEN;
      se1Params.DIMFMT  = __SE_DIMFMT_1D;

      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

template DSPLIB_STATUS DSPLIB_bexp_init_ci<int8_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams1D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsOut,
                                                   const DSPLIB_bexp_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bexp_init_ci<uint8_t>(DSPLIB_kernelHandle         handle,
                                                    const DSPLIB_bufParams1D_t *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t *bufParamsOut,
                                                    const DSPLIB_bexp_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bexp_init_ci<int16_t>(DSPLIB_kernelHandle         handle,
                                                    const DSPLIB_bufParams1D_t *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t *bufParamsOut,
                                                    const DSPLIB_bexp_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bexp_init_ci<uint16_t>(DSPLIB_kernelHandle         handle,
                                                     const DSPLIB_bufParams1D_t *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t *bufParamsOut,
                                                     const DSPLIB_bexp_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bexp_init_ci<int32_t>(DSPLIB_kernelHandle         handle,
                                                    const DSPLIB_bufParams1D_t *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t *bufParamsOut,
                                                    const DSPLIB_bexp_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bexp_init_ci<uint32_t>(DSPLIB_kernelHandle         handle,
                                                     const DSPLIB_bufParams1D_t *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t *bufParamsOut,
                                                     const DSPLIB_bexp_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bexp_init_ci<int64_t>(DSPLIB_kernelHandle         handle,
                                                    const DSPLIB_bufParams1D_t *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t *bufParamsOut,
                                                    const DSPLIB_bexp_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bexp_init_ci<uint64_t>(DSPLIB_kernelHandle         handle,
                                                     const DSPLIB_bufParams1D_t *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t *bufParamsOut,
                                                     const DSPLIB_bexp_InitArgs *pKerInitArgs);

/**
 * Templated implementation of the bexp kernel to handle
 * unsigned and signed data types differently
 */
template <typename UnsignedDataType>
inline void bexp_exec_ci_unsigned_small_inputs(void *restrict pIn,
                                               void *restrict pOut,
                                               uint8_t *restrict pBlock,
                                               int32_t mainLoopCount)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");

   uint32_t *restrict pOutLocal = (uint32_t *) pOut;
   uint32_t                                                       out_val;
   typedef typename c7x::make_full_vector<UnsignedDataType>::type Vec;

   Vec  mask     = Vec(0);
   Vec *pInLocal = (Vec *) pIn;
   for (int32_t counter = 0; counter < (mainLoopCount - 1); counter++) {
      Vec b = *pInLocal++;
      mask |= b;
   }
   Vec lastVec        = *pInLocal;
   Vec lastVecAndMask = *(Vec *) ((uint8_t *) pBlock + LAST_VEC_AND_MASK_OFFSET);
   lastVec &= lastVecAndMask;
   mask |= lastVec;

   UnsignedDataType mask_final = (UnsignedDataType) c7x_horizontal_max(mask);
   /**
    * Check the last bit and if it is 1, return 0
    * else, return norm + 1
    */
   int num_bits = sizeof(UnsignedDataType) * 8;
   int shift;
   using SignedDataType = std::make_signed_t<UnsignedDataType>;
   shift                = __norm((SignedDataType) mask_final);
   out_val              = (uint32_t) (shift + 1);
   if (((uint64_t) 1 << (uint64_t) (num_bits - 1)) & mask_final) {
      out_val = 0;
   }
   *pOutLocal = out_val;

   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}
template <typename SignedDataType>
inline void bexp_exec_ci_signed_small_inputs(void *restrict pIn,
                                             void *restrict pOut,
                                             uint8_t *restrict pBlock,
                                             int32_t mainLoopCount)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");

   uint32_t *restrict pOutLocal = (uint32_t *) pOut;
   uint32_t                                                     out_val;
   typedef typename c7x::make_full_vector<SignedDataType>::type Vec;

   Vec  mask     = Vec(0);
   Vec *pInLocal = (Vec *) pIn;
   DSPLIB_DEBUGPRINTFN(0, "CInt: mainLoopCount = %d\n", mainLoopCount);
   for (int32_t counter = 0; counter < (mainLoopCount - 1); counter++) {
      Vec b = *pInLocal++;
      mask |= __abs(b);
   }
   Vec lastVec        = *pInLocal;
   Vec lastVecAndMask = *(Vec *) ((uint8_t *) pBlock + LAST_VEC_AND_MASK_OFFSET);
   mask |= __abs(lastVec & lastVecAndMask);

   using UnsignedDataType = std::make_unsigned_t<SignedDataType>;
   typedef typename c7x::make_full_vector<UnsignedDataType>::type UVec;
   SignedDataType mask_final = (SignedDataType) c7x_horizontal_max(c7x::reinterpret<UVec>(mask));
   out_val                   = (uint32_t) __norm(mask_final);
   *pOutLocal                = out_val;

   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}
template <typename UnsignedDataType> inline void bexp_exec_ci_unsigned(void *restrict pOut, uint32_t mainLoopCount)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");

   uint32_t *restrict pOutLocal = (uint32_t *) pOut;
   uint32_t                                                       out_val;
   typedef typename c7x::make_full_vector<UnsignedDataType>::type Vec;

   Vec mask0 = Vec(0);
   Vec mask1 = Vec(0);
#pragma MUST_ITERATE(4, , 1)
   for (uint32_t counter = 0; counter < mainLoopCount; counter++) {
      Vec b0 = c7x::strm_eng<0, Vec>::get_adv();
      Vec b1 = c7x::strm_eng<1, Vec>::get_adv();
      mask0 |= b0;
      mask1 |= b1;
   }
   mask0 |= mask1;

   UnsignedDataType mask_final = (UnsignedDataType) c7x_horizontal_max(mask0);
   /**
    * Check the last bit and if it is 1, return 0
    * else, return norm + 1
    */
   int num_bits = sizeof(UnsignedDataType) * 8;
   int shift;
   using SignedDataType = std::make_signed_t<UnsignedDataType>;
   shift                = __norm((SignedDataType) mask_final);
   out_val              = (uint32_t) (shift + 1);
   if (((uint64_t) 1 << (uint64_t) (num_bits - 1)) & mask_final) {
      out_val = 0;
   }
   *pOutLocal = out_val;

   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}

template <typename SignedDataType> inline void bexp_exec_ci_signed(void *restrict pOut, uint32_t mainLoopCount)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");

   uint32_t *restrict pOutLocal = (uint32_t *) pOut;
   uint32_t                                                     out_val;
   typedef typename c7x::make_full_vector<SignedDataType>::type Vec;

   Vec mask0 = Vec(0);
   Vec mask1 = Vec(0);
#pragma MUST_ITERATE(4, , 1)
   for (uint32_t counter = 0; counter < mainLoopCount; counter++) {
      Vec b0 = c7x::strm_eng<0, Vec>::get_adv();
      Vec b1 = c7x::strm_eng<1, Vec>::get_adv();
      mask0 |= __abs(b0);
      mask1 |= __abs(b1);
   }
   mask0 |= mask1;
   using UnsignedDataType = std::make_unsigned_t<SignedDataType>;
   typedef typename c7x::make_full_vector<UnsignedDataType>::type UVec;
   SignedDataType mask_final = (SignedDataType) c7x_horizontal_max(c7x::reinterpret<UVec>(mask0));
   out_val                   = (uint32_t) __norm(mask_final);
   *pOutLocal                = out_val;

   DSPLIB_DEBUGPRINTFN(0, "%s", "Exit function\n");
   return;
}

/**
 * Exec function of the bexp kernel
 */

template <>
DSPLIB_STATUS DSPLIB_bexp_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   uint8_t *pBlock        = pKerPrivArgs->bufPblock;
   uint32_t mainLoopCount = pKerPrivArgs->mainLoopCount;
   if (pKerPrivArgs->isSEReadEnabled) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SE0_OPEN((int8_t *) pIn, se0Params);
      __SE1_OPEN((int8_t *) (((uint8_t *) pIn) + pKerPrivArgs->se1StartAddressByteOffset), se1Params);

      bexp_exec_ci_signed<int8_t>(pOut, mainLoopCount);

      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   else {
      bexp_exec_ci_signed_small_inputs<int8_t>(pIn, pOut, pBlock, mainLoopCount);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

template <>
DSPLIB_STATUS DSPLIB_bexp_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   uint8_t *pBlock        = pKerPrivArgs->bufPblock;
   uint32_t mainLoopCount = pKerPrivArgs->mainLoopCount;
   if (pKerPrivArgs->isSEReadEnabled) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SE0_OPEN((uint8_t *) pIn, se0Params);
      __SE1_OPEN((uint8_t *) (((uint8_t *) pIn) + pKerPrivArgs->se1StartAddressByteOffset), se1Params);

      bexp_exec_ci_unsigned<uint8_t>(pOut, mainLoopCount);

      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   else {
      bexp_exec_ci_unsigned_small_inputs<uint8_t>(pIn, pOut, pBlock, mainLoopCount);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

template <>
DSPLIB_STATUS DSPLIB_bexp_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   uint8_t *pBlock        = pKerPrivArgs->bufPblock;
   uint32_t mainLoopCount = pKerPrivArgs->mainLoopCount;
   if (pKerPrivArgs->isSEReadEnabled) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SE0_OPEN((int16_t *) pIn, se0Params);
      __SE1_OPEN((int16_t *) (((uint8_t *) pIn) + pKerPrivArgs->se1StartAddressByteOffset), se1Params);

      bexp_exec_ci_signed<int16_t>(pOut, mainLoopCount);

      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   else {
      bexp_exec_ci_signed_small_inputs<int16_t>(pIn, pOut, pBlock, mainLoopCount);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

template <>
DSPLIB_STATUS DSPLIB_bexp_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   uint8_t *pBlock        = pKerPrivArgs->bufPblock;
   uint32_t mainLoopCount = pKerPrivArgs->mainLoopCount;
   if (pKerPrivArgs->isSEReadEnabled) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SE0_OPEN((uint16_t *) pIn, se0Params);
      __SE1_OPEN((uint16_t *) (((uint8_t *) pIn) + pKerPrivArgs->se1StartAddressByteOffset), se1Params);

      bexp_exec_ci_unsigned<uint16_t>(pOut, mainLoopCount);

      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   else {
      bexp_exec_ci_unsigned_small_inputs<uint16_t>(pIn, pOut, pBlock, mainLoopCount);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

template <>
DSPLIB_STATUS DSPLIB_bexp_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   uint8_t *pBlock        = pKerPrivArgs->bufPblock;
   uint32_t mainLoopCount = pKerPrivArgs->mainLoopCount;
   if (pKerPrivArgs->isSEReadEnabled) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SE0_OPEN((int32_t *) pIn, se0Params);
      __SE1_OPEN((int32_t *) (((uint8_t *) pIn) + pKerPrivArgs->se1StartAddressByteOffset), se1Params);

      bexp_exec_ci_signed<int32_t>(pOut, mainLoopCount);

      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   else {
      bexp_exec_ci_signed_small_inputs<int32_t>(pIn, pOut, pBlock, mainLoopCount);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

template <>
DSPLIB_STATUS DSPLIB_bexp_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   uint8_t *pBlock        = pKerPrivArgs->bufPblock;
   uint32_t mainLoopCount = pKerPrivArgs->mainLoopCount;
   if (pKerPrivArgs->isSEReadEnabled) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SE0_OPEN((uint32_t *) pIn, se0Params);
      __SE1_OPEN((uint32_t *) (((uint8_t *) pIn) + pKerPrivArgs->se1StartAddressByteOffset), se1Params);

      bexp_exec_ci_unsigned<uint32_t>(pOut, mainLoopCount);

      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   else {
      bexp_exec_ci_unsigned_small_inputs<uint32_t>(pIn, pOut, pBlock, mainLoopCount);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

template <>
DSPLIB_STATUS DSPLIB_bexp_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   uint8_t *pBlock        = pKerPrivArgs->bufPblock;
   uint32_t mainLoopCount = pKerPrivArgs->mainLoopCount;
   if (pKerPrivArgs->isSEReadEnabled) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SE0_OPEN((int32_t *) pIn, se0Params);
      __SE1_OPEN((int32_t *) (((uint8_t *) pIn) + pKerPrivArgs->se1StartAddressByteOffset), se1Params);

      bexp_exec_ci_signed<int64_t>(pOut, mainLoopCount);

      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   else {
      bexp_exec_ci_signed_small_inputs<int64_t>(pIn, pOut, pBlock, mainLoopCount);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}

template <>
DSPLIB_STATUS DSPLIB_bexp_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");
   DSPLIB_STATUS         status       = DSPLIB_SUCCESS;
   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;

   uint8_t *pBlock        = pKerPrivArgs->bufPblock;
   uint32_t mainLoopCount = pKerPrivArgs->mainLoopCount;
   if (pKerPrivArgs->isSEReadEnabled) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SE0_OPEN((uint32_t *) pIn, se0Params);
      __SE1_OPEN((uint32_t *) (((uint8_t *) pIn) + pKerPrivArgs->se1StartAddressByteOffset), se1Params);

      bexp_exec_ci_unsigned<uint64_t>(pOut, mainLoopCount);

      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   else {
      bexp_exec_ci_unsigned_small_inputs<uint64_t>(pIn, pOut, pBlock, mainLoopCount);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return status;
}
