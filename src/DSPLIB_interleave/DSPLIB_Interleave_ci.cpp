// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../DSPLIB_matTrans/DSPLIB_matTrans.h"
#include "DSPLIB_Interleave_priv.h"

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define MASK_OFFSET (SE_SA0_PARAM_OFFSET + 64)

/**********************************************************************/
/* INITIALIZATION                                                     */
/**********************************************************************/

template <typename T> struct c7x_vec_type;
template <> struct c7x_vec_type<double> {
   using type = c7x::double_vec;
};

template <> struct c7x_vec_type<float> {
   using type = c7x::float_vec;
};
// uint32_t
template <> struct c7x_vec_type<uint32_t> {
   using type = c7x::uint_vec;
};

// int32_t
template <> struct c7x_vec_type<int32_t> {
   using type = c7x::int_vec;
};

// int8_t
template <> struct c7x_vec_type<int8_t> {
   using type = c7x::char_vec;
};

// uint8_t
template <> struct c7x_vec_type<uint8_t> {
   using type = c7x::uchar_vec;
};

// int16_t
template <> struct c7x_vec_type<int16_t> {
   using type = c7x::short_vec;
};

// uint16_t
template <> struct c7x_vec_type<uint16_t> {
   using type = c7x::ushort_vec;
};

// int16_t
template <> struct c7x_vec_type<int64_t> {
   using type = c7x::long_vec;
};

// uint16_t
template <> struct c7x_vec_type<uint64_t> {
   using type = c7x::ulong_vec;
};

template <typename VecT> struct c7x_vec_cast;

template <> struct c7x_vec_cast<c7x::float_vec> {
   template <typename SrcT> static c7x::float_vec convert(const SrcT &val) { return c7x::as_float_vec(val); }
};

template <> struct c7x_vec_cast<c7x::double_vec> {
   template <typename SrcT> static c7x::double_vec convert(const SrcT &val) { return c7x::as_double_vec(val); }
};
// uint32_vec
template <> struct c7x_vec_cast<c7x::uint_vec> {
   template <typename SrcT> static c7x::uint_vec convert(const SrcT &val) { return c7x::as_uint_vec(val); }
};

// int32_vec
template <> struct c7x_vec_cast<c7x::int_vec> {
   template <typename SrcT> static c7x::int_vec convert(const SrcT &val) { return c7x::as_int_vec(val); }
};

// int8_vec
template <> struct c7x_vec_cast<c7x::char_vec> {
   template <typename SrcT> static c7x::char_vec convert(const SrcT &val) { return c7x::as_char_vec(val); }
};

// uint8_vec
template <> struct c7x_vec_cast<c7x::uchar_vec> {
   template <typename SrcT> static c7x::uchar_vec convert(const SrcT &val) { return c7x::as_uchar_vec(val); }
};

// int16_vec
template <> struct c7x_vec_cast<c7x::short_vec> {
   template <typename SrcT> static c7x::short_vec convert(const SrcT &val) { return c7x::as_short_vec(val); }
};

// uint16_vec
template <> struct c7x_vec_cast<c7x::ushort_vec> {
   template <typename SrcT> static c7x::ushort_vec convert(const SrcT &val) { return c7x::as_ushort_vec(val); }
};
// int16_vec
template <> struct c7x_vec_cast<c7x::ulong_vec> {
   template <typename SrcT> static c7x::ulong_vec convert(const SrcT &val) { return c7x::as_ulong_vec(val); }
};

// uint16_vec
template <> struct c7x_vec_cast<c7x::long_vec> {
   template <typename SrcT> static c7x::long_vec convert(const SrcT &val) { return c7x::as_long_vec(val); }
};

// Default template (for double or other types)
template <typename T> constexpr DSPLIB_data_type_e getDataTypeEnum() { return DSPLIB_FLOAT64; }

// Specialization for float
template <> constexpr DSPLIB_data_type_e getDataTypeEnum<float>() { return DSPLIB_FLOAT32; }

template <typename dataType>
inline void interleave_init(DSPLIB_kernelHandle         handle,
                            const DSPLIB_bufParams2D_t *bufParamsIn,
                            const DSPLIB_bufParams2D_t *bufParamsOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function to init SE for 32-bit data");

   DSPLIB_interleave_PrivArgs                            *pKerPrivArgs = (DSPLIB_interleave_PrivArgs *) handle;
   uint8_t                                               *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                                               widthIn      = pKerPrivArgs->widthIn;
   uint32_t                                               heightIn     = pKerPrivArgs->heightIn;
   int32_t                                                strideIn     = bufParamsIn->stride_y;
   int32_t                                                strideOut    = bufParamsOut->stride_y;
   int32_t                                                dataSize     = sizeof(dataType);
   __SE_TEMPLATE_v1                                       se0Params    = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1                                       sa0Params    = __gen_SA_TEMPLATE_v1();
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;
   if (heightIn == 1) {
      se0Params.ICNT0   = eleCount;
      se0Params.DIM1    = eleCount;
      se0Params.ICNT1   = DSPLIB_ceilingDiv(widthIn, eleCount);
      se0Params.DIMFMT  = __SE_DIMFMT_2D;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;

      sa0Params.ICNT0 = 1;
      sa0Params.ICNT1 = DSPLIB_ceilingDiv((heightIn * widthIn), 2);
      sa0Params.DIM1  = 2 * (strideOut / dataSize);

      sa0Params.VECLEN        = SA_VECLEN;
      sa0Params.DIMFMT        = __SA_DIMFMT_2D;
      pKerPrivArgs->loopCount = DSPLIB_ceilingDiv(widthIn, eleCount);
   }
   else {
      se0Params.ICNT0 = eleCount;
      se0Params.ICNT1 = heightIn;
      se0Params.DIM1  = strideIn / dataSize;
      se0Params.ICNT2 = DSPLIB_ceilingDiv(widthIn, eleCount);
      se0Params.DIM2  = eleCount;

      se0Params.DIMFMT  = __SE_DIMFMT_3D;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;

      sa0Params.ICNT0         = widthIn * heightIn;
      sa0Params.VECLEN        = SA_VECLEN;
      sa0Params.DIMFMT        = __SA_DIMFMT_1D;
      pKerPrivArgs->loopCount = DSPLIB_ceilingDiv(DSPLIB_ceilingDiv(widthIn * heightIn, eleCount), 2);
   }
   if (pKerPrivArgs->dataType == DSPLIB_FLOAT32) {
#if __C7X_VEC_SIZE_BITS__ == 256

      uint8_t mask[64] = {
          0,  1,  2,  3,  // 0
          16, 17, 18, 19, // 4
          4,  5,  6,  7,  // 1
          20, 21, 22, 23, // 5
          8,  9,  10, 11, // 2
          24, 25, 26, 27, // 6
          12, 13, 14, 15, // 3
          28, 29, 30, 31, // 7
          32, 33, 34, 35, // 8
          48, 49, 50, 51, // 12
          36, 37, 38, 39, // 9
          52, 53, 54, 55, // 13
          40, 41, 42, 43, // 10
          56, 57, 58, 59, // 14
          44, 45, 46, 47, // 11
          60, 61, 62, 63  // 15
      };
#else
      uint8_t mask[64] = {
          0,  1,  2,  3,  // Row 0
          32, 33, 34, 35, // Row 8
          4,  5,  6,  7,  // Row 1
          36, 37, 38, 39, // Row 9
          8,  9,  10, 11, // Row 2
          40, 41, 42, 43, // Row 10
          12, 13, 14, 15, // Row 3
          44, 45, 46, 47, // Row 11
          16, 17, 18, 19, // Row 4
          48, 49, 50, 51, // Row 12
          20, 21, 22, 23, // Row 5
          52, 53, 54, 55, // Row 13
          24, 25, 26, 27, // Row 6
          56, 57, 58, 59, // Row 14
          28, 29, 30, 31, // Row 7
          60, 61, 62, 63  // Row 15
      };
#endif
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      memcpy(((uint8_t *) pBlock + MASK_OFFSET), mask, 64);
   }
   else if (pKerPrivArgs->dataType == DSPLIB_FLOAT64) {
#if __C7X_VEC_SIZE_BITS__ == 256

      uint8_t mask[64] = {
          0,  1,  2,  3,  // 0
          4,  5,  6,  7,  // 1
          16, 17, 18, 19, // 4
          20, 21, 22, 23, // 5
          8,  9,  10, 11, // 2
          12, 13, 14, 15, // 3
          24, 25, 26, 27, // 6
          28, 29, 30, 31, // 7
          32, 33, 34, 35, // 8
          36, 37, 38, 39, // 9
          48, 49, 50, 51, // 12
          52, 53, 54, 55, // 13
          40, 41, 42, 43, // 10
          44, 45, 46, 47, // 11
          56, 57, 58, 59, // 14
          60, 61, 62, 63  // 15
      };
#else
      uint8_t mask[64] = {
          0,  1,  2,  3,  // 0
          4,  5,  6,  7,  // 1
          32, 33, 34, 35, // 8
          36, 37, 38, 39, // 9
          8,  9,  10, 11, // 2
          12, 13, 14, 15, // 3
          40, 41, 42, 43, // 10
          44, 45, 46, 47, // 11
          16, 17, 18, 19, // 4
          20, 21, 22, 23, // 5
          48, 49, 50, 51, // 12
          52, 53, 54, 55, // 13
          24, 25, 26, 27, // 6
          28, 29, 30, 31, // 7
          56, 57, 58, 59, // 14
          60, 61, 62, 63  // 15
      };
#endif
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      memcpy(((uint8_t *) pBlock + MASK_OFFSET), mask, 64);
   }
   else {
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function to init SE for 32-bit data");
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_interleave_init_ci(DSPLIB_kernelHandle              handle,
                                        const DSPLIB_bufParams2D_t      *bufParamsIn,
                                        const DSPLIB_bufParams2D_t      *bufParamsOut,
                                        const DSPLIB_interleaveInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   interleave_init<dataType>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_interleave_init_ci<float>(DSPLIB_kernelHandle              handle,
                                                        const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                        const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                        const DSPLIB_interleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_interleave_init_ci<double>(DSPLIB_kernelHandle              handle,
                                                         const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                         const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                         const DSPLIB_interleaveInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_interleave_init_ci<int32_t>(DSPLIB_kernelHandle              handle,
                                                          const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                          const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                          const DSPLIB_interleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_interleave_init_ci<uint32_t>(DSPLIB_kernelHandle              handle,
                                                           const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                           const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                           const DSPLIB_interleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_interleave_init_ci<int64_t>(DSPLIB_kernelHandle              handle,
                                                          const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                          const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                          const DSPLIB_interleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_interleave_init_ci<uint64_t>(DSPLIB_kernelHandle              handle,
                                                           const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                           const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                           const DSPLIB_interleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_interleave_init_ci<int16_t>(DSPLIB_kernelHandle              handle,
                                                          const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                          const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                          const DSPLIB_interleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_interleave_init_ci<uint16_t>(DSPLIB_kernelHandle              handle,
                                                           const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                           const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                           const DSPLIB_interleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_interleave_init_ci<int8_t>(DSPLIB_kernelHandle              handle,
                                                         const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                         const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                         const DSPLIB_interleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_interleave_init_ci<uint8_t>(DSPLIB_kernelHandle              handle,
                                                          const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                          const DSPLIB_bufParams2D_t      *bufParamsOut,
                                                          const DSPLIB_interleaveInitArgs *pKerInitArgs);

/**********************************************************************/
/* IMPLEMENTATION                                                     */
/**********************************************************************/
template <typename dataType>
inline void interleave_compute(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for Transpose 64/32-bit data");

   DSPLIB_interleave_PrivArgs *pKerPrivArgs = (DSPLIB_interleave_PrivArgs *) handle;
   uint32_t                    heightIn     = pKerPrivArgs->heightIn;
   int32_t                     strideIn     = pKerPrivArgs->strideIn;
   uint32_t                    dataSize     = sizeof(dataType);
   uint32_t                    widthIn      = pKerPrivArgs->widthIn;
   __SE_TEMPLATE_v1            se0Params;
   __SA_TEMPLATE_v1            sa0Params;
   uint8_t                    *pBlock = pKerPrivArgs->bufPblock;
   se0Params                          = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params                          = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   dataType *restrict pInLocal        = (dataType *) pIn;
   dataType *restrict pOutLocal       = (dataType *) pOut;
   dataType *restrict pOutLocal1      = (dataType *) pOut;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                loopCount = pKerPrivArgs->loopCount;
   using vecType                                                    = typename c7x_vec_type<dataType>::type;
   strideIn                                                         = strideIn / dataSize;
   int32_t eleCount                                                 = c7x::element_count_of<vec>::value;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p loopCount: %d\n", pInLocal, pOutLocal, loopCount);
   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa0Params);

   if (heightIn == 1) {
      pOutLocal1 += 1;
      loopCount = DSPLIB_ceilingDiv(widthIn, eleCount);
      for (int32_t i = 0; i < loopCount; i++) {
         vec vec1 = c7x::strm_eng<0, vec>::get_adv();

         for (int32_t cnt = 0; cnt < eleCount; cnt += 4) {

            dataType data = vec1.s[0 + cnt];

            dataType data3 = vec1.s[1 + cnt];

            __vpred dstWr = c7x::strm_agen<0, dataType>::get_vpred();

            dataType *dstWrPtr = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);

            __vstore_pred(dstWr, dstWrPtr, data);

            __vpred dstWr2 = c7x::strm_agen<1, dataType>::get_vpred();

            dataType *dstWrPtr2 = c7x::strm_agen<1, dataType>::get_adv(pOutLocal1);

            __vstore_pred(dstWr2, dstWrPtr2, data3);

            data  = vec1.s[2 + cnt];
            data3 = vec1.s[3 + cnt];

            dstWr = c7x::strm_agen<0, dataType>::get_vpred();

            dstWrPtr = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);

            __vstore_pred(dstWr, dstWrPtr, data);

            dstWr2 = c7x::strm_agen<1, dataType>::get_vpred();

            dstWrPtr2 = c7x::strm_agen<1, dataType>::get_adv(pOutLocal1);

            __vstore_pred(dstWr2, dstWrPtr2, data3);
         }
      }
   }
   else {

      for (int32_t i = 0; i < loopCount; i++) {
         vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
         vec loadVec2 = c7x::strm_eng<0, vec>::get_adv();

         loadVec1 = c7x_vec_cast<vecType>::convert(loadVec1);
         loadVec2 = c7x_vec_cast<vecType>::convert(loadVec2);

         __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred_interleave_low_low(predTemp, pStoreVec, (loadVec1), (loadVec2));

         __vpred predTemp1  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred_interleave_high_high(predTemp1, pStoreVec1, loadVec1, loadVec2);
      }
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for interleave compute");
}

template <typename dataType>
inline void interleave_compute_fp(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for Transpose 16-bit data");

   DSPLIB_interleave_PrivArgs *pKerPrivArgs = (DSPLIB_interleave_PrivArgs *) handle;
   uint32_t                    heightIn     = pKerPrivArgs->heightIn;
   __SE_TEMPLATE_v1            se0Params;
   __SA_TEMPLATE_v1            sa0Params;
   uint32_t                    widthIn = pKerPrivArgs->widthIn;

   uint8_t *pBlock               = pKerPrivArgs->bufPblock;
   se0Params                     = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params                     = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   dataType *restrict pInLocal   = (dataType *) pIn;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOutLocal1 = (dataType *) pOut;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                loopCount = pKerPrivArgs->loopCount;
   using vecType                                                    = typename c7x_vec_type<dataType>::type;
   int32_t eleCount                                                 = c7x::element_count_of<vec>::value;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p loopCount: %d\n", pInLocal, pOutLocal, loopCount);
   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa0Params);

   uint8_t *mask = (uint8_t *) pBlock + MASK_OFFSET;

   if (heightIn == 1) {

      loopCount = DSPLIB_ceilingDiv(widthIn, eleCount);
      pOutLocal1 += 1;

      for (int32_t i = 0; i < loopCount; i++) {
         vec vec1 = c7x::strm_eng<0, vec>::get_adv();
         for (int32_t cnt = 0; cnt < eleCount; cnt += 4) {

            dataType data = vec1.s[0 + cnt];

            dataType data3 = vec1.s[1 + cnt];

            __vpred dstWr = c7x::strm_agen<0, dataType>::get_vpred();

            dataType *dstWrPtr = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);

            __vstore_pred(dstWr, dstWrPtr, data);

            __vpred dstWr2 = c7x::strm_agen<1, dataType>::get_vpred();

            dataType *dstWrPtr2 = c7x::strm_agen<1, dataType>::get_adv(pOutLocal1);

            __vstore_pred(dstWr2, dstWrPtr2, data3);

            data  = vec1.s[2 + cnt];
            data3 = vec1.s[3 + cnt];

            dstWr = c7x::strm_agen<0, dataType>::get_vpred();

            dstWrPtr = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);

            __vstore_pred(dstWr, dstWrPtr, data);

            dstWr2 = c7x::strm_agen<1, dataType>::get_vpred();

            dstWrPtr2 = c7x::strm_agen<1, dataType>::get_adv(pOutLocal1);

            __vstore_pred(dstWr2, dstWrPtr2, data3);
         }
      }
   }
   else {

      c7x::uchar_vec vMask = *stov_ptr(c7x::uchar_vec, (uint8_t *) mask);

      for (int32_t i = 0; i < loopCount; i++) {
         vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
         vec loadVec2 = c7x::strm_eng<0, vec>::get_adv();

         vec storeVec1 = c7x_vec_cast<vecType>::convert(
             __permute_low_low(vMask, c7x::as_uchar_vec(loadVec2), c7x::as_uchar_vec(loadVec1)));
         vec storeVec2 = c7x_vec_cast<vecType>::convert(
             __permute_high_high(vMask, c7x::as_uchar_vec(loadVec2), c7x::as_uchar_vec(loadVec1)));

         loadVec1 = c7x_vec_cast<vecType>::convert(storeVec1);
         loadVec2 = c7x_vec_cast<vecType>::convert(storeVec2);

         __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(predTemp, pStoreVec, (loadVec1));

         __vpred predTemp1  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(predTemp1, pStoreVec1, (loadVec2));
      }
   }

   __SE1_CLOSE();
   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for interleave compute for float and double");
}

void DSPLIB_interleave_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   DSPLIB_interleave_PrivArgs *pKerPrivArgs = (DSPLIB_interleave_PrivArgs *) handle;

   uint64_t interleaveStartupCycles   = 0;
   uint64_t interleaveTeardownCycles  = 0;
   uint64_t interleaveOperationCycles = 0;
   uint64_t interleaveOverheadCycles  = 0;
   uint32_t heightIn                  = pKerPrivArgs->heightIn;
   uint32_t widthIn                   = pKerPrivArgs->widthIn;
   uint32_t strideOut                 = pKerPrivArgs->strideOut;
   uint32_t dataType                  = pKerPrivArgs->dataType;

   uint32_t strideElements = strideOut / DSPLIB_sizeof(dataType);

   if (((heightIn == 1u) || (heightIn == 2u)) && (strideElements == heightIn)) {
      if (dataType == (uint32_t) DSPLIB_FLOAT32 || dataType == (uint32_t) DSPLIB_FLOAT64) {
         if (heightIn == 1) {
            uint32_t loopCount        = pKerPrivArgs->loopCount;
            interleaveOverheadCycles  = 25;
            interleaveStartupCycles   = 4;
            interleaveOperationCycles = 8 + loopCount * 4;
            interleaveTeardownCycles  = 2;
         }
         else {
            uint32_t loopCount        = pKerPrivArgs->loopCount;
            interleaveOverheadCycles  = 25;
            interleaveStartupCycles   = 4;
            interleaveOperationCycles = 6 + loopCount * 2;
            interleaveTeardownCycles  = 1;
         }
      }
      else {
         if (heightIn == 1) {
            uint32_t loopCount        = pKerPrivArgs->loopCount;
            interleaveOverheadCycles  = 25;
            interleaveStartupCycles   = 4;
            interleaveOperationCycles = 8 + loopCount * 4;
            interleaveTeardownCycles  = 1;
         }
         else {
            uint32_t loopCount        = pKerPrivArgs->loopCount;
            interleaveOverheadCycles  = 25;
            interleaveStartupCycles   = 4;
            interleaveOperationCycles = 3 + loopCount * 2;
            interleaveTeardownCycles  = 1;
         }
      }
      interleaveOverheadCycles += interleaveStartupCycles + interleaveTeardownCycles;
      *estCycles  = interleaveOperationCycles + interleaveOverheadCycles;
      *archCycles = interleaveOperationCycles;
   }
   else {
      DSPLIB_matTrans_perfEst(handle, archCycles, estCycles, dataType, widthIn);
   }
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_interleave_exec_ci_fp(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   interleave_compute_fp<dataType>(handle, pIn, pOut);
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_interleave_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   interleave_compute<dataType>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

// DSPLIB_interleave.cpp
template DSPLIB_STATUS
DSPLIB_interleave_exec_ci_fp<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_interleave_exec_ci_fp<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_interleave_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_interleave_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_interleave_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_interleave_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_interleave_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_interleave_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_interleave_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_interleave_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
