// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blk_eswap_priv.h"

#if __C7X_VEC_SIZE_BITS__ == 256
uchar32 vMask16 = uchar32(1,
                          0,
                          3,
                          2,
                          5,
                          4,
                          7,
                          6,
                          9,
                          8,
                          11,
                          10,
                          13,
                          12,
                          15,
                          14,
                          17,
                          16,
                          19,
                          18,
                          21,
                          20,
                          23,
                          22,
                          25,
                          24,
                          27,
                          26,
                          29,
                          28,
                          31,
                          30);
uchar32 vMask32 = uchar32(3,
                          2,
                          1,
                          0,
                          7,
                          6,
                          5,
                          4,
                          11,
                          10,
                          9,
                          8,
                          15,
                          14,
                          13,
                          12,
                          19,
                          18,
                          17,
                          16,
                          23,
                          22,
                          21,
                          20,
                          27,
                          26,
                          25,
                          24,
                          31,
                          30,
                          29,
                          28);
uchar32 vMask64 = uchar32(7,
                          6,
                          5,
                          4,
                          3,
                          2,
                          1,
                          0,
                          15,
                          14,
                          13,
                          12,
                          11,
                          10,
                          9,
                          8,
                          23,
                          22,
                          21,
                          20,
                          19,
                          18,
                          17,
                          16,
                          31,
                          30,
                          29,
                          28,
                          27,
                          26,
                          25,
                          24);
#elif __C7X_VEC_SIZE_BITS__ == 512
uchar64 vMask16 = uchar64(1,
                          0,
                          3,
                          2,
                          5,
                          4,
                          7,
                          6,
                          9,
                          8,
                          11,
                          10,
                          13,
                          12,
                          15,
                          14,
                          17,
                          16,
                          19,
                          18,
                          21,
                          20,
                          23,
                          22,
                          25,
                          24,
                          27,
                          26,
                          29,
                          28,
                          31,
                          30,
                          33,
                          32,
                          35,
                          34,
                          37,
                          36,
                          39,
                          38,
                          41,
                          40,
                          43,
                          42,
                          45,
                          44,
                          47,
                          46,
                          49,
                          48,
                          51,
                          50,
                          53,
                          52,
                          55,
                          54,
                          57,
                          56,
                          59,
                          58,
                          61,
                          60,
                          63,
                          62);
uchar64 vMask32 = uchar64(3,
                          2,
                          1,
                          0,
                          7,
                          6,
                          5,
                          4,
                          11,
                          10,
                          9,
                          8,
                          15,
                          14,
                          13,
                          12,
                          19,
                          18,
                          17,
                          16,
                          23,
                          22,
                          21,
                          20,
                          27,
                          26,
                          25,
                          24,
                          31,
                          30,
                          29,
                          28,
                          35,
                          34,
                          33,
                          32,
                          39,
                          38,
                          37,
                          36,
                          43,
                          42,
                          41,
                          40,
                          47,
                          46,
                          45,
                          44,
                          51,
                          50,
                          49,
                          48,
                          55,
                          54,
                          53,
                          52,
                          59,
                          58,
                          57,
                          56,
                          63,
                          62,
                          61,
                          60);

uchar64 vMask64 = uchar64(7,
                          6,
                          5,
                          4,
                          3,
                          2,
                          1,
                          0,
                          15,
                          14,
                          13,
                          12,
                          11,
                          10,
                          9,
                          8,
                          23,
                          22,
                          21,
                          20,
                          19,
                          18,
                          17,
                          16,
                          31,
                          30,
                          29,
                          28,
                          27,
                          26,
                          25,
                          24,
                          39,
                          38,
                          37,
                          36,
                          35,
                          34,
                          33,
                          32,
                          47,
                          46,
                          45,
                          44,
                          43,
                          42,
                          41,
                          40,
                          55,
                          54,
                          53,
                          52,
                          51,
                          50,
                          49,
                          48,
                          63,
                          62,
                          61,
                          60,
                          59,
                          58,
                          57,
                          56);
#endif

// Define the mask template
template <uint32_t maskType> inline c7x::uchar_vec blk_eswap_mask();

template <> inline c7x::uchar_vec blk_eswap_mask<DSPLIB_UINT16>() { return vMask16; }

template <> inline c7x::uchar_vec blk_eswap_mask<DSPLIB_UINT32>() { return vMask32; }

template <> inline c7x::uchar_vec blk_eswap_mask<DSPLIB_UINT64>() { return vMask64; }

template <typename dataType>
DSPLIB_STATUS DSPLIB_blk_eswap_init_ci(DSPLIB_kernelHandle              handle,
                                       const DSPLIB_bufParams1D_t      *bufParamsIn,
                                       const DSPLIB_bufParams1D_t      *bufParamsOut,
                                       const DSPLIB_blk_eswap_InitArgs *pKerInitArgs)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS              status       = DSPLIB_SUCCESS;
   DSPLIB_blk_eswap_PrivArgs *pKerPrivArgs = (DSPLIB_blk_eswap_PrivArgs *) handle;
   uint32_t                   blockSize    = pKerPrivArgs->blockSize;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint8_t     *pBlock     = pKerPrivArgs->bufPblock;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   se0Params.DIMFMT           = __SE_DIMFMT_1D;
   se0Params.ELETYPE          = SE_ELETYPE;
   se0Params.VECLEN           = SE_VECLEN;
   se0Params.ICNT0            = blockSize;

   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   sa0Params.DIMFMT           = __SA_DIMFMT_1D;
   sa0Params.VECLEN           = SA_VECLEN;
   sa0Params.ICNT0            = blockSize;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

template DSPLIB_STATUS DSPLIB_blk_eswap_init_ci<uint16_t>(DSPLIB_kernelHandle              handle,
                                                          const DSPLIB_bufParams1D_t      *bufParamsIn,
                                                          const DSPLIB_bufParams1D_t      *bufParamsOut,
                                                          const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_eswap_init_ci<uint32_t>(DSPLIB_kernelHandle              handle,
                                                          const DSPLIB_bufParams1D_t      *bufParamsIn,
                                                          const DSPLIB_bufParams1D_t      *bufParamsOut,
                                                          const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_blk_eswap_init_ci<uint64_t>(DSPLIB_kernelHandle              handle,
                                                          const DSPLIB_bufParams1D_t      *bufParamsIn,
                                                          const DSPLIB_bufParams1D_t      *bufParamsOut,
                                                          const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_eswap_init_ci<int16_t>(DSPLIB_kernelHandle              handle,
                                                         const DSPLIB_bufParams1D_t      *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t      *bufParamsOut,
                                                         const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_eswap_init_ci<int32_t>(DSPLIB_kernelHandle              handle,
                                                         const DSPLIB_bufParams1D_t      *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t      *bufParamsOut,
                                                         const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_blk_eswap_init_ci<int64_t>(DSPLIB_kernelHandle              handle,
                                                         const DSPLIB_bufParams1D_t      *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t      *bufParamsOut,
                                                         const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_eswap_init_ci<float>(DSPLIB_kernelHandle              handle,
                                                       const DSPLIB_bufParams1D_t      *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t      *bufParamsOut,
                                                       const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_blk_eswap_init_ci<double>(DSPLIB_kernelHandle              handle,
                                                        const DSPLIB_bufParams1D_t      *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t      *bufParamsOut,
                                                        const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);

template <typename dataType, uint32_t maskType>
DSPLIB_STATUS DSPLIB_blk_eswap_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS                                          status       = DSPLIB_SUCCESS;
   DSPLIB_blk_eswap_PrivArgs                             *pKerPrivArgs = (DSPLIB_blk_eswap_PrivArgs *) handle;
   uint32_t                                               blockSize    = pKerPrivArgs->blockSize;
   uint32_t                                               cntr;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;
   __SE_TEMPLATE_v1                                       se0Params;
   __SA_TEMPLATE_v1                                       sa0Params;

   dataType *srcPtr = (dataType *) pIn;
   dataType *dstPtr = (dataType *) pOut;
   if (!dstPtr) {
      DSPLIB_DEBUGPRINTFN(0, "%s\n", "In-place swapping Enabled");
      dstPtr = srcPtr; /* In-place swaping */
   }

   DSPLIB_DEBUGPRINTFN(0, "srcPtr: %p dstPtr: %p blockSize: %d\n", srcPtr, dstPtr, blockSize);

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   __SE0_OPEN(srcPtr, se0Params);
   __SA0_OPEN(sa0Params);

   c7x::uchar_vec vMask = blk_eswap_mask<maskType>();
   for (cntr = 0; cntr < blockSize; cntr += eleCount) {
      vec     inpBlock     = c7x::strm_eng<0, vec>::get_adv();
      vec     swappedBlock = c7x::reinterpret<vec>(__permute(vMask, c7x::as_uchar_vec(inpBlock)));
      __vpred vpStore      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *outVecPtr    = c7x::strm_agen<0, vec>::get_adv(dstPtr);
      __vstore_pred(vpStore, outVecPtr, swappedBlock);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return (status);
}

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_ci<uint16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_ci<uint32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_ci<uint64_t, DSPLIB_UINT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_ci<int16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_ci<int32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_ci<int64_t, DSPLIB_UINT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_ci<float, DSPLIB_UINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_eswap_exec_ci<double, DSPLIB_UINT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
