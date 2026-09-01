// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../DSPLIB_matTrans/DSPLIB_matTrans.h"
#include "DSPLIB_deinterleave_priv.h"

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define MASK_OFFFSET (SE_SA0_PARAM_OFFSET + 128)

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
inline void deinterleave_init(DSPLIB_kernelHandle         handle,
                              const DSPLIB_bufParams2D_t *bufParamsIn,
                              const DSPLIB_bufParams2D_t *bufParamsOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function to init SE for 32-bit data");

   DSPLIB_deinterleave_PrivArgs                          *pKerPrivArgs = (DSPLIB_deinterleave_PrivArgs *) handle;
   uint8_t                                               *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                                               widthIn      = pKerPrivArgs->widthIn;
   uint32_t                                               heightIn     = pKerPrivArgs->heightIn;
   int32_t                                                strideOut    = bufParamsOut->stride_y;
   int32_t                                                strideIn     = bufParamsIn->stride_y;
   int32_t                                                dataSize     = sizeof(dataType);
   __SE_TEMPLATE_v1                                       se0Params    = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1                                       sa0Params    = __gen_SA_TEMPLATE_v1();
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;

   if (widthIn == 1) {

      if ((bufParamsIn->data_type == DSPLIB_FLOAT64) || ((bufParamsIn->data_type == DSPLIB_INT64)) ||
          ((bufParamsIn->data_type == DSPLIB_UINT64))) {
         int32_t iter = (heightIn + (eleCount * 2) - 1) / (eleCount * 2);

         se0Params.ICNT0     = widthIn;
         se0Params.ICNT1     = (heightIn > (uint32_t) eleCount) ? eleCount : heightIn;
         se0Params.DIM1      = strideIn / dataSize;
         se0Params.ICNT2     = DSPLIB_ceilingDiv(heightIn, eleCount);
         se0Params.DIM2      = (strideIn / dataSize) * eleCount;
         se0Params.DIMFMT    = __SE_DIMFMT_3D;
         se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
         se0Params.ELETYPE   = SE_ELETYPE;
         se0Params.VECLEN    = SE_VECLEN;

         sa0Params.ICNT0         = heightIn;
         sa0Params.VECLEN        = SA_VECLEN;
         sa0Params.DIMFMT        = __SA_DIMFMT_1D;
         pKerPrivArgs->loopCount = DSPLIB_ceilingDiv(heightIn, eleCount);

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
         *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET))    = iter;
      }
      else if ((bufParamsIn->data_type == DSPLIB_FLOAT32) || ((bufParamsIn->data_type == DSPLIB_INT32)) ||
               ((bufParamsIn->data_type == DSPLIB_UINT32))) {
         int32_t iter        = (heightIn + (eleCount * 2) - 1) / (eleCount * 2);
         se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
         se0Params.ICNT0     = widthIn;
         se0Params.ICNT1     = (heightIn > (uint32_t) eleCount) ? eleCount : heightIn;
         se0Params.DIM1      = strideIn / dataSize;
         se0Params.ICNT2     = DSPLIB_ceilingDiv(heightIn, eleCount);
         se0Params.DIM2      = (strideIn / dataSize) * eleCount;
         se0Params.DIMFMT    = __SE_DIMFMT_3D;
         se0Params.ELETYPE   = SE_ELETYPE;
         se0Params.VECLEN    = SE_VECLEN;

         sa0Params.ICNT0         = heightIn;
         sa0Params.VECLEN        = SA_VECLEN;
         sa0Params.DIMFMT        = __SA_DIMFMT_1D;
         pKerPrivArgs->loopCount = DSPLIB_ceilingDiv(heightIn, eleCount);

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
         *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET))    = iter;
      }
      else if (((bufParamsIn->data_type == DSPLIB_INT16)) || ((bufParamsIn->data_type == DSPLIB_UINT16))) {
         se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
         se0Params.ICNT0     = widthIn;
         se0Params.ICNT1     = (heightIn > (uint32_t) (eleCount / 2)) ? (eleCount / 2) : heightIn;
         se0Params.ICNT2     = DSPLIB_ceilingDiv(heightIn, eleCount / 2);
         se0Params.DIM2      = (strideIn / dataSize) * (eleCount / 2);
         se0Params.ELETYPE   = SE_ELETYPE;
         se0Params.VECLEN    = SE_VECLEN;
         se0Params.DIM1      = strideIn / dataSize;
         se0Params.DIMFMT    = __SE_DIMFMT_3D;

         sa0Params.ICNT0  = heightIn;
         sa0Params.DIMFMT = __SA_DIMFMT_1D;
         sa0Params.VECLEN = SA_VECLEN;

         pKerPrivArgs->loopCount = DSPLIB_ceilingDiv(heightIn, eleCount);

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      }
      else if ((bufParamsIn->data_type == DSPLIB_INT8) || (bufParamsIn->data_type == DSPLIB_UINT8)) {
         uint32_t outEleCount = eleCount / 4;

         se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
         se0Params.ELETYPE   = SE_ELETYPE;
         se0Params.VECLEN    = SE_VECLEN;
         se0Params.ICNT0     = widthIn;
         se0Params.ICNT1     = outEleCount;
         se0Params.DIM1      = strideIn;
         se0Params.DIMFMT    = __SE_DIMFMT_3D;
         se0Params.ICNT2     = DSPLIB_ceilingDiv(heightIn, outEleCount);
         se0Params.DIM2      = strideIn * outEleCount;

         sa0Params.VECLEN = SA_VECLEN;
         sa0Params.ICNT0  = outEleCount;
         sa0Params.ICNT1  = DSPLIB_ceilingDiv(heightIn, outEleCount);
         sa0Params.DIM1   = outEleCount;
         sa0Params.DIMFMT = __SA_DIMFMT_2D;

         uint8_t mask[64] = {
             0,  4,  8,  12, 16, 20, 24, 28, // 0
             2,  9,  10, 11, 3,  13, 14, 15, // 1
             1,  17, 18, 19, 5,  21, 22, 23, // 2
             6,  25, 26, 27, 7,  29, 30, 31, // 3
             32, 33, 34, 35, 36, 37, 38, 39, // 4
             40, 41, 42, 43, 44, 45, 46, 47, // 5
             48, 49, 50, 51, 52, 53, 54, 55, // 6
             56, 57, 58, 59, 60, 61, 62, 63  // 7
         };
         pKerPrivArgs->loopCount = DSPLIB_ceilingDiv(heightIn, (eleCount / 4));

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
         memcpy((uint8_t *) pBlock + MASK_OFFFSET, mask, 64);
      }
      else {
         se0Params.ICNT0   = 1;
         se0Params.DIM1    = 1;
         se0Params.ICNT1   = heightIn * widthIn;
         se0Params.DIMFMT  = __SE_DIMFMT_2D;
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;

         sa0Params.ICNT0 = 1;
         sa0Params.ICNT1 = heightIn * widthIn;
         sa0Params.DIM1  = 1;

         sa0Params.VECLEN = SA_VECLEN;
         sa0Params.DIMFMT = __SA_DIMFMT_2D;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      }
   }
   else {
      se0Params.ICNT0   = widthIn * heightIn;
      se0Params.DIMFMT  = __SE_DIMFMT_1D;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DECIM   = __SE_DECIM_2;

      sa0Params.ICNT0  = eleCount;
      sa0Params.ICNT1  = widthIn;
      sa0Params.ICNT2  = DSPLIB_ceilingDiv(heightIn, eleCount);
      sa0Params.DIM1   = strideOut / dataSize;
      sa0Params.DIM2   = eleCount;
      sa0Params.VECLEN = SA_VECLEN;
      sa0Params.DIMFMT = __SA_DIMFMT_3D;
      if (bufParamsIn->data_type == DSPLIB_UINT8 || bufParamsIn->data_type == DSPLIB_INT8) {

         pKerPrivArgs->loopCount = DSPLIB_ceilingDiv(DSPLIB_ceilingDiv(widthIn * heightIn, eleCount), 2);
      }
      else {
         pKerPrivArgs->loopCount = DSPLIB_ceilingDiv(DSPLIB_ceilingDiv(widthIn * heightIn, eleCount), 2);
      }

      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function to init SE ");
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_deinterleave_init_ci(DSPLIB_kernelHandle                handle,
                                          const DSPLIB_bufParams2D_t        *bufParamsIn,
                                          const DSPLIB_bufParams2D_t        *bufParamsOut,
                                          const DSPLIB_deinterleaveInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   deinterleave_init<dataType>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<float>(DSPLIB_kernelHandle                handle,
                                                          const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                          const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                          const DSPLIB_deinterleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<double>(DSPLIB_kernelHandle                handle,
                                                           const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                           const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                           const DSPLIB_deinterleaveInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<int32_t>(DSPLIB_kernelHandle                handle,
                                                            const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                            const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                            const DSPLIB_deinterleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<uint32_t>(DSPLIB_kernelHandle                handle,
                                                             const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                             const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                             const DSPLIB_deinterleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<int64_t>(DSPLIB_kernelHandle                handle,
                                                            const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                            const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                            const DSPLIB_deinterleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<uint64_t>(DSPLIB_kernelHandle                handle,
                                                             const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                             const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                             const DSPLIB_deinterleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<int16_t>(DSPLIB_kernelHandle                handle,
                                                            const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                            const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                            const DSPLIB_deinterleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<uint16_t>(DSPLIB_kernelHandle                handle,
                                                             const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                             const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                             const DSPLIB_deinterleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<int8_t>(DSPLIB_kernelHandle                handle,
                                                           const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                           const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                           const DSPLIB_deinterleaveInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_deinterleave_init_ci<uint8_t>(DSPLIB_kernelHandle                handle,
                                                            const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                            const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                            const DSPLIB_deinterleaveInitArgs *pKerInitArgs);

/**********************************************************************/
/* IMPLEMENTATION                                                     */
/**********************************************************************/
template <typename dataType>
inline void deinterleave_compute_64_32bit(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for Transpose 64/32-bit data");

   DSPLIB_deinterleave_PrivArgs *pKerPrivArgs = (DSPLIB_deinterleave_PrivArgs *) handle;
   uint32_t                      widthIn      = pKerPrivArgs->widthIn;
   __SE_TEMPLATE_v1              se0Params;
   __SA_TEMPLATE_v1              sa0Params;
   uint8_t                      *pBlock = pKerPrivArgs->bufPblock;
   se0Params                            = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params                            = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   dataType *restrict pInLocal          = (dataType *) pIn;
   dataType *restrict pOutLocal         = (dataType *) pOut;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                loopCount = pKerPrivArgs->loopCount;
   typedef typename c7x::make_full_vector<dataType>::type vec;

   using vecType = typename c7x_vec_type<dataType>::type;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p loopCount: %d\n", pInLocal, pOutLocal, loopCount);
   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   if (widthIn == 1) {

      for (int32_t i = 0; i < loopCount; i++) {
         vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();

         __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(predTemp, pStoreVec, loadVec1);
      }
      __SE0_CLOSE();
      __SA0_CLOSE();
   }
   else {
      __SE1_OPEN(pInLocal + 1, se0Params);

      for (int32_t i = 0; i < loopCount; i++) {
         vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
         vec loadVec2 = c7x::strm_eng<1, vec>::get_adv();

         loadVec1 = c7x_vec_cast<vecType>::convert(loadVec1);
         loadVec2 = c7x_vec_cast<vecType>::convert(loadVec2);

         __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(predTemp, pStoreVec, loadVec1);

         __vpred predTemp1  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(predTemp1, pStoreVec1, loadVec2);
      }

      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for Transpose 64/32-bit data");
}

template <typename dataType>
inline void deinterleave_compute_16bit(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for Transpose 16-bit data");

   DSPLIB_deinterleave_PrivArgs *pKerPrivArgs = (DSPLIB_deinterleave_PrivArgs *) handle;
   uint32_t                      widthIn      = pKerPrivArgs->widthIn;
   int32_t                       strideIn     = pKerPrivArgs->strideIn;
   uint32_t                      dataSize     = sizeof(dataType);
   __SE_TEMPLATE_v1              se0Params;
   __SA_TEMPLATE_v1              sa0Params;
   uint8_t                      *pBlock = pKerPrivArgs->bufPblock;
   se0Params                            = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params                            = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   dataType *restrict pInLocal          = (dataType *) pIn;
   dataType *restrict pOutLocal         = (dataType *) pOut;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount  = c7x::element_count_of<vec>::value;
   int32_t                                                loopCount = pKerPrivArgs->loopCount;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   using vecType = typename c7x_vec_type<dataType>::type;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p loopCount: %d\n", pInLocal, pOutLocal, loopCount);

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   if (widthIn == 1) {
      int32_t offset = dataSize > 0? ((strideIn / dataSize) * (eleCount / 2)) : 0;
      __SE1_OPEN(pInLocal + offset, se0Params);
      for (int i = 0; i < loopCount; i++) {
         vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
         vec loadVec2 = c7x::strm_eng<0, vec>::get_adv();

         vec checkEven = __pack_consec_low(loadVec2, loadVec1);

         __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(predTemp, pStoreVec, checkEven);
      }
      __SE0_CLOSE();
      __SA0_CLOSE();
   }
   else {
      {
         __SE1_OPEN(pInLocal + 1, se0Params);

         for (int32_t i = 0; i < loopCount; i++) {
            vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
            vec loadVec2 = c7x::strm_eng<1, vec>::get_adv();

            loadVec1 = c7x_vec_cast<vecType>::convert(loadVec1);
            loadVec2 = c7x_vec_cast<vecType>::convert(loadVec2);

            __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(predTemp, pStoreVec, loadVec1);

            __vpred predTemp1  = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(predTemp1, pStoreVec1, loadVec2);
         }
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for Transpose 16-bit data");
}

template <typename dataType>
inline void deinterleave_compute_8bit(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for Transpose 8-bit data");

   DSPLIB_deinterleave_PrivArgs *pKerPrivArgs = (DSPLIB_deinterleave_PrivArgs *) handle;
   uint32_t                      widthIn      = pKerPrivArgs->widthIn;
   __SE_TEMPLATE_v1              se0Params;
   __SA_TEMPLATE_v1              sa0Params;
   uint8_t                      *pBlock = pKerPrivArgs->bufPblock;
   se0Params                            = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params                            = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   uint8_t *mask                        = ((uint8_t *) pBlock + MASK_OFFFSET);
   dataType *restrict pInLocal          = (dataType *) pIn;
   dataType *restrict pOutLocal         = (dataType *) pOut;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                loopCount = pKerPrivArgs->loopCount;
   using vecType                                                    = typename c7x_vec_type<dataType>::type;

   using vecType = typename c7x_vec_type<dataType>::type;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p loopCount: %d\n", pInLocal, pOutLocal, loopCount);

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   uchar32 vMask = *stov_ptr(uchar32, (uint8_t *) mask);
   if (widthIn == 1) {
      for (int i = 0; i < loopCount; i++) {
         vec loadVec = c7x::strm_eng<0, vec>::get_adv();

         loadVec = c7x_vec_cast<vecType>::convert(__permute(vMask, c7x::as_uchar_vec(loadVec)));

         __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(predTemp, pStoreVec, loadVec);
      }
      __SE0_CLOSE();
      __SA0_CLOSE();
   }
   else {
      {
         __SE1_OPEN(pInLocal + 1, se0Params);

         for (int32_t i = 0; i < loopCount; i++) {
            vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
            vec loadVec2 = c7x::strm_eng<1, vec>::get_adv();

            loadVec1 = c7x_vec_cast<vecType>::convert(loadVec1);
            loadVec2 = c7x_vec_cast<vecType>::convert(loadVec2);

            __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(predTemp, pStoreVec, loadVec1);

            __vpred predTemp1  = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(predTemp1, pStoreVec1, loadVec2);
         }
         __SE0_CLOSE();
         __SE1_CLOSE();
         __SA0_CLOSE();
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for Transpose 8-bit data");
}

void DSPLIB_deinterleave_perfEst(DSPLIB_kernelHandle handle,
                                 uint64_t           *archCycles,
                                 uint64_t           *estCycles,
                                 uint64_t            dataType)
{
   DSPLIB_deinterleave_PrivArgs *pKerPrivArgs = (DSPLIB_deinterleave_PrivArgs *) handle;

   uint64_t deinterleaveStartupCycles   = 0;
   uint64_t deinterleaveTeardownCycles  = 0;
   uint64_t deinterleaveOperationCycles = 0;
   uint64_t deinterleaveOverheadCycles  = 0;
   uint32_t widthIn                     = pKerPrivArgs->widthIn;
   uint32_t strideIn                    = pKerPrivArgs->strideIn;

   if ((widthIn == 1u) || (widthIn == 2u && (uint32_t) widthIn == (uint32_t) strideIn / DSPLIB_sizeof(dataType))) {

      if ((dataType == DSPLIB_FLOAT32) || (dataType == DSPLIB_FLOAT64) || (dataType == DSPLIB_INT32) ||
          (dataType == DSPLIB_UINT32) || (dataType == DSPLIB_INT64) || (dataType == DSPLIB_UINT64)) {
         if (widthIn == 1) {
            uint32_t loopCount          = pKerPrivArgs->loopCount;
            deinterleaveOverheadCycles  = 25;
            deinterleaveStartupCycles   = 4;
            deinterleaveOperationCycles = 1 + loopCount * 1;
            deinterleaveTeardownCycles  = 1;
         }
         else {
            uint32_t loopCount          = pKerPrivArgs->loopCount;
            deinterleaveOverheadCycles  = 25 + 16;
            deinterleaveStartupCycles   = 4;
            deinterleaveOperationCycles = 1 + loopCount * 2;
            deinterleaveTeardownCycles  = 4;
         }
      }
      if ((dataType == DSPLIB_INT16) || (dataType == DSPLIB_UINT16)) {
         if (widthIn == 1) {
            uint32_t loopCount          = pKerPrivArgs->loopCount;
            deinterleaveOverheadCycles  = 16;
            deinterleaveStartupCycles   = 14;
            deinterleaveOperationCycles = 4 + loopCount * 1;
            deinterleaveTeardownCycles  = 3;
         }
         else {
            uint32_t loopCount          = pKerPrivArgs->loopCount;
            deinterleaveOverheadCycles  = 16;
            deinterleaveStartupCycles   = 16;
            deinterleaveOperationCycles = 1 + loopCount * 2;
            deinterleaveTeardownCycles  = 2;
         }
      }
      else {
         if (widthIn == 1) {
            uint32_t loopCount          = pKerPrivArgs->loopCount;
            deinterleaveOverheadCycles  = 23;
            deinterleaveStartupCycles   = 4;
            deinterleaveOperationCycles = 4 + loopCount * 1;
            deinterleaveTeardownCycles  = 2;
         }
         else {
            uint32_t loopCount          = pKerPrivArgs->loopCount;
            deinterleaveOverheadCycles  = 23;
            deinterleaveStartupCycles   = 15;
            deinterleaveOperationCycles = 1 + loopCount * 2;
            deinterleaveTeardownCycles  = 1;
         }
      }
      deinterleaveOverheadCycles += deinterleaveStartupCycles + deinterleaveTeardownCycles;
      *estCycles  = deinterleaveOperationCycles + deinterleaveOverheadCycles;
      *archCycles = deinterleaveOperationCycles;
   }
   else {
      DSPLIB_matTrans_perfEst(handle, archCycles, estCycles, dataType, widthIn);
   }
}

template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_64_32bit<float>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_64_32bit<double>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_64_32bit<uint64_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}
template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_64_32bit<int64_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_16bit<int16_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}
template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_64_32bit<int32_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_64_32bit<uint32_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_16bit<uint16_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}
template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_8bit<int8_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_deinterleave_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   deinterleave_compute_8bit<uint8_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}
