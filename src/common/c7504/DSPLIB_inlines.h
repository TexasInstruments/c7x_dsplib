// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef C7504_DSPLIB_INLINES_H
#define C7504_DSPLIB_INLINES_H 1

/*******************************************************************************
 *
 * Inlined functions
 *
 ******************************************************************************/

/******************************************************************************/
#ifdef __cplusplus
#include <c7x_scalable.h>

// Convert long to int
template <typename X, typename Y> inline typename c7x::make_full_vector<X>::type convert_long_to_int(Y vec);

template <> inline c7x::make_full_vector<int32_t>::type convert_long_to_int<int8, long4>(long4 vec)
{
   typedef typename c7x::make_full_vector<int32_t>::type vecRet;
   vecRet                                                v8bits = __as_int8(vec);
   return v8bits;
}

template <> inline c7x::make_full_vector<uint32_t>::type convert_long_to_int<uint8, ulong4>(ulong4 vec)
{
   typedef typename c7x::make_full_vector<uint32_t>::type vecRet;
   vecRet                                                 v8bits = __as_uint8(vec);
   return v8bits;
}

// Convert char to short
template <typename X, typename Y>
inline typename c7x::make_full_vector<X>::type convert_char_to_short(Y vecIn, bool typeIndex);

template <>
inline c7x::make_full_vector<int16_t>::type convert_char_to_short<short16, char16>(char16 vecIn, bool typeIndex)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecRet;
   vecRet                                                vecOut;
   if (typeIndex) {
      uchar16 vecInConv = __as_uchar16(vecIn);
      vecOut            = __convert_short16(vecInConv);
   }
   else {
      vecOut = __convert_short16(vecIn);
   }
   return vecOut;
}

template <>
inline c7x::make_full_vector<uint16_t>::type convert_char_to_short<ushort16, uchar16>(uchar16 vecIn, bool typeIndex)
{
   typedef typename c7x::make_full_vector<uint16_t>::type vecRet;
   vecRet                                                 vecOut = __convert_ushort16(vecIn);
   return vecOut;
}

// Multiply 2 char and result in short
template <typename X, typename Y> inline void mul_char_to_short(Y vecIn1, Y vecIn2, X vecOut1, X vecOut2);

template <>
inline void mul_char_to_short<short16 &, char32>(c7x::char_vec   vecIn1,
                                                 c7x::char_vec   vecIn2,
                                                 c7x::short_vec &vecOut1Short,
                                                 c7x::short_vec &vecOut2Short)
{
   short16 vecInShort1    = __as_short16(vecIn1);
   short16 vecInShort2    = __as_short16(vecIn2);
   vecInShort2            = __shift_right(vecInShort2, c7x::short_vec(8));
   short16 vecInShortOdd  = __shift_right(vecInShort1, c7x::short_vec(8));
   short16 vecInShortEven = __shift_right(__shift_left(vecInShort1, c7x::short_vec(8)), c7x::short_vec(8));

   vecOut1Short = vecInShortEven * vecInShort2;
   vecOut2Short = vecInShortOdd * vecInShort2;
}

template <>
inline void mul_char_to_short<ushort16 &, uchar32>(c7x::uchar_vec   vecIn1,
                                                   c7x::uchar_vec   vecIn2,
                                                   c7x::ushort_vec &vecOut1uShort,
                                                   c7x::ushort_vec &vecOut2uShort)
{
   __vmpyubh_vvw(vecIn1, vecIn2, vecOut1uShort, vecOut2uShort);
}

// Horizontal max computation with index
template <typename V, typename W>
inline void c7x_horizontal_max_with_index(V maxValVec, V vIdx, W *maxVal, int *maxIdx);

template <>
inline void c7x_horizontal_max_with_index(c7x::float_vec maxValVec, c7x::float_vec vIdx, float *maxVal, int *maxIdx)
{
   __vpred vpMask;
   vpMask            = __cmp_lt_pred(maxValVec.even(), maxValVec.odd());
   float4 maxValVec1 = (float4) __select(vpMask, maxValVec.odd(), maxValVec.even());
   float4 vIdx1      = (float4) __select(vpMask, vIdx.odd(), vIdx.even());

   vpMask            = __cmp_lt_pred(maxValVec1.even(), maxValVec1.odd());
   float2 maxValVec2 = (float2) __select(vpMask, maxValVec1.odd(), maxValVec1.even());
   float2 vIdx2      = (float2) __select(vpMask, vIdx1.odd(), vIdx1.even());

   vpMask  = __cmp_lt_pred(maxValVec2.even(), maxValVec2.odd());
   *maxVal = (float) __select(vpMask, maxValVec2.odd(), maxValVec2.even());
   *maxIdx = ((int) __select(vpMask, vIdx2.odd(), vIdx2.even()));
}

template <>
inline void c7x_horizontal_max_with_index(c7x::double_vec maxValVec, c7x::double_vec vIdx, double *maxVal, int *maxIdx)
{
   __vpred vpMask;
   vpMask             = __cmp_lt_pred(maxValVec.even(), maxValVec.odd());
   double2 maxValVec1 = (double2) __select(vpMask, maxValVec.odd(), maxValVec.even());
   double2 vIdx1      = (double2) __select(vpMask, vIdx.odd(), vIdx.even());

   vpMask  = __cmp_lt_pred(maxValVec1.even(), maxValVec1.odd());
   *maxVal = (double) __select(vpMask, maxValVec1.odd(), maxValVec1.even());
   *maxIdx = (int) (__select(vpMask, vIdx1.odd(), vIdx1.even()));
}

template <>
inline void c7x_horizontal_max_with_index(c7x::char_vec maxValVec, c7x::char_vec vIdx, int8_t *maxVal, int *maxIdx)
{
   typedef typename c7x::make_full_vector<int8_t>::type vec;
   __vpred                                              vpMask;

   char16 maxValVec1 = maxValVec.even();
   maxValVec1        = __max(maxValVec.odd(), maxValVec1);

   char8 maxValVec2 = maxValVec1.even();
   maxValVec2       = __max(maxValVec1.odd(), maxValVec2);

   char4 maxValVec3 = maxValVec2.even();
   maxValVec3       = __max(maxValVec2.odd(), maxValVec3);

   char2 maxValVec4 = maxValVec3.even();
   maxValVec4       = __max(maxValVec3.odd(), maxValVec4);

   int8_t maxValVec5 = maxValVec4.even();
   maxValVec5        = __max(maxValVec4.odd(), maxValVec5);
   *maxVal           = (int8_t) maxValVec5;

   int8_t maxValScaler = (int8_t) maxValVec5;

   char32 zero_vec         = vec(0);
   char32 diff_vector      = vec(maxValScaler) - maxValVec;
   vpMask                  = __cmp_eq_pred(diff_vector, zero_vec);
   char32  masked_indices  = __select(vpMask, vIdx, vec(-1));
   uchar32 umasked_indices = __as_uchar32(masked_indices);

   uchar16 vIdx1 = (uchar16) __min(umasked_indices.even(), umasked_indices.odd());
   uchar8  vIdx2 = (uchar8) __min(vIdx1.even(), vIdx1.odd());
   uchar4  vIdx3 = (uchar4) __min(vIdx2.even(), vIdx2.odd());
   uchar2  vIdx4 = (uchar2) __min(vIdx3.even(), vIdx3.odd());
   *maxIdx       = (uint8_t) __min(vIdx4.even(), vIdx4.odd());
}

template <>
inline void c7x_horizontal_max_with_index(c7x::short_vec maxValVec, c7x::short_vec vIdx, short *maxVal, int *maxIdx)
{
   __vpred                                               vpMask;
   typedef typename c7x::make_full_vector<int16_t>::type vec;

   vec sortIn         = __sort_desc(maxValVec);
   *maxVal            = (short) sortIn.s[0];
   short maxValScaler = (short) sortIn.s[0];

   short16 zero_vec       = vec(0);
   short16 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask                 = __cmp_eq_pred(diff_vector, zero_vec);
   short16 masked_indices = __select(vpMask, vIdx, vec(255));

   short8 vIdx1 = (short8) __min(masked_indices.even(), masked_indices.odd());
   short4 vIdx2 = (short4) __min(vIdx1.even(), vIdx1.odd());
   short2 vIdx3 = (short2) __min(vIdx2.even(), vIdx2.odd());
   *maxIdx      = (uint16_t) __min(vIdx3.even(), vIdx3.odd());
}

template <>
inline void c7x_horizontal_max_with_index(c7x::int_vec maxValVec, c7x::int_vec vIdx, int *maxVal, int *maxIdx)
{
   __vpred                                               vpMask;
   typedef typename c7x::make_full_vector<int32_t>::type vec;

   vec sortIn = __sort_desc(maxValVec);
   ;
   *maxVal = (int) sortIn.s[0];

   int maxValScaler = (int) sortIn.s[0];

   int8 zero_vec       = vec(0);
   int8 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask              = __cmp_eq_pred(diff_vector, zero_vec);
   int8 masked_indices = __select(vpMask, vIdx, vec(255));
   int4 vIdx1          = (int4) __min(masked_indices.even(), masked_indices.odd());
   int2 vIdx2          = (int2) __min(vIdx1.even(), vIdx1.odd());
   *maxIdx             = (int) __min(vIdx2.even(), vIdx2.odd());
}

template <>
inline void c7x_horizontal_max_with_index(c7x::long_vec maxValVec, c7x::long_vec vIdx, long *maxVal, int *maxIdx)
{
   __vpred                                               vpMask;
   typedef typename c7x::make_full_vector<int64_t>::type vec;

   long2 maxValVec1 = maxValVec.even();
   maxValVec1       = __max(maxValVec.odd(), maxValVec1);

   long maxValVec2 = maxValVec1.even();
   maxValVec2      = __max(maxValVec1.odd(), maxValVec2);
   *maxVal         = (long) maxValVec2;

   long maxValScaler = (long) maxValVec2;

   long4 zero_vec       = vec(0);
   long4 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask               = __cmp_eq_pred(diff_vector, zero_vec);
   long4 maxIdxVec      = vec(255);
   long4 masked_indices = __select(vpMask, vIdx, maxIdxVec);

   long2 vIdx1 = (long2) __min(masked_indices.even(), masked_indices.odd());
   *maxIdx     = (long) __min(vIdx1.even(), vIdx1.odd());
}

template <>
inline void c7x_horizontal_max_with_index(c7x::uchar_vec maxValVec, c7x::uchar_vec vIdx, uchar *maxVal, int *maxIdx)
{
   __vpred                                               vpMask;
   typedef typename c7x::make_full_vector<uint8_t>::type vec;

   uchar16 maxValVec1 = maxValVec.even();
   maxValVec1         = __max(maxValVec.odd(), maxValVec1);

   uchar8 maxValVec2 = maxValVec1.even();
   maxValVec2        = __max(maxValVec1.odd(), maxValVec2);

   uchar4 maxValVec3 = maxValVec2.even();
   maxValVec3        = __max(maxValVec2.odd(), maxValVec3);

   uchar2 maxValVec4 = maxValVec3.even();
   maxValVec4        = __max(maxValVec3.odd(), maxValVec4);

   uchar maxValVec5 = maxValVec4.even();
   maxValVec5       = __max(maxValVec4.odd(), maxValVec5);
   *maxVal          = (uchar) maxValVec5;

   uchar   maxValScaler   = (uchar) maxValVec5;
   uchar32 zero_vec       = vec(0);
   uchar32 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask                 = __cmp_eq_pred(diff_vector, zero_vec);
   uchar32 masked_indices = __select(vpMask, vIdx, vec(255));

   uchar16 vIdx1 = (uchar16) __min(masked_indices.even(), masked_indices.odd());
   uchar8  vIdx2 = (uchar8) __min(vIdx1.even(), vIdx1.odd());
   uchar4  vIdx3 = (uchar4) __min(vIdx2.even(), vIdx2.odd());
   uchar2  vIdx4 = (uchar2) __min(vIdx3.even(), vIdx3.odd());
   *maxIdx       = (uint8_t) __min(vIdx4.even(), vIdx4.odd());
}

template <>
inline void c7x_horizontal_max_with_index(c7x::ushort_vec maxValVec, c7x::ushort_vec vIdx, ushort *maxVal, int *maxIdx)
{
   __vpred                                                vpMask;
   typedef typename c7x::make_full_vector<uint16_t>::type vec;

   vec sortIn          = __sort_desc(maxValVec);
   *maxVal             = (ushort) sortIn.s[0];
   ushort maxValScaler = (ushort) sortIn.s[0];

   ushort16 zero_vec       = vec(0);
   ushort16 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask                  = __cmp_eq_pred(diff_vector, zero_vec);
   ushort16 masked_indices = __select(vpMask, vIdx, vec(255));

   ushort8 vIdx1 = (ushort8) __min(masked_indices.even(), masked_indices.odd());
   ushort4 vIdx2 = (ushort4) __min(vIdx1.even(), vIdx1.odd());
   ushort2 vIdx3 = (ushort2) __min(vIdx2.even(), vIdx2.odd());
   *maxIdx       = (uint16_t) __min(vIdx3.even(), vIdx3.odd());
}

template <>
inline void c7x_horizontal_max_with_index(c7x::uint_vec maxValVec, c7x::uint_vec vIdx, uint *maxVal, int *maxIdx)
{
   __vpred                                                vpMask;
   typedef typename c7x::make_full_vector<uint32_t>::type vec;

   vec sortIn = __sort_desc(maxValVec);
   ;
   *maxVal = (uint) sortIn.s[0];

   uint maxValScaler = (uint) sortIn.s[0];

   uint8 zero_vec       = vec(0);
   uint8 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask               = __cmp_eq_pred(diff_vector, zero_vec);
   uint8 masked_indices = __select(vpMask, vIdx, vec(255));
   uint4 vIdx1          = (uint4) __min(masked_indices.even(), masked_indices.odd());
   uint2 vIdx2          = (uint2) __min(vIdx1.even(), vIdx1.odd());
   *maxIdx              = (uint) __min(vIdx2.even(), vIdx2.odd());
}

template <>
inline void c7x_horizontal_max_with_index(c7x::ulong_vec maxValVec, c7x::ulong_vec vIdx, ulong *maxVal, int *maxIdx)
{
   __vpred                                                vpMask;
   typedef typename c7x::make_full_vector<uint64_t>::type vec;
   ulong2                                                 maxValVec1 = maxValVec.even();
   maxValVec1                                                        = __max(maxValVec.odd(), maxValVec1);

   ulong maxValVec2 = maxValVec1.even();
   maxValVec2       = __max(maxValVec1.odd(), maxValVec2);
   *maxVal          = (ulong) maxValVec2;

   ulong maxValScaler = (ulong) maxValVec2;

   ulong4 zero_vec       = vec(0);
   ulong4 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask                = __cmp_eq_pred(diff_vector, zero_vec);
   ulong4 maxIdxVec      = vec(255);
   ulong4 masked_indices = __select(vpMask, vIdx, maxIdxVec);

   ulong2 vIdx1 = (ulong2) __min(masked_indices.even(), masked_indices.odd());
   *maxIdx      = (ulong) __min(vIdx1.even(), vIdx1.odd());
}

// Horizontal max computation
template <typename V> inline uint64_t c7x_horizontal_max(V vin);

template <> inline uint64_t c7x_horizontal_max(c7x::uchar_vec vin)
{
   uint32_t retVal1 = (uint32_t) (__sort_desc(c7x::as_ushort_vec(vin)).s0());
   uint32_t retVal2 = (uint32_t) (__sort_desc(__shift_left(c7x::as_ushort_vec(vin), (ushort16) (8))).s0());
   return (uint64_t) (retVal1 > retVal2 ? ((retVal1 >> 8U) & 0xFFU) : ((retVal2 >> 8U) & 0xFFU));
}
template <> inline uint64_t c7x_horizontal_max(c7x::ushort_vec vin)
{
   uint64_t retVal = (uint64_t) (__sort_desc((ushort16) vin).s0());
   return retVal;
}
template <> inline uint64_t c7x_horizontal_max(c7x::uint_vec vin)
{
   uint64_t retVal = (uint64_t) (__sort_desc((uint8) vin).s0());
   return retVal;
}
template <> inline uint64_t c7x_horizontal_max(c7x::ulong_vec vin)
{
   ulong2   m1     = __max(vin.even(), vin.odd());
   uint64_t retVal = (uint64_t) __max(m1.even(), m1.odd());
   return retVal;
}

template <typename dataType, typename V = typename c7x::make_full_vector<dataType>::type>
inline dataType c7x_horizontal_max_fp(V vin);

template <> inline float c7x_horizontal_max_fp(c7x::float_vec vin)
{

   float4 vin1   = __max(vin.hi(), vin.lo());
   float2 vin2   = __max(vin1.hi(), vin1.lo());
   float  maxVal = __max(vin2.hi(), vin2.lo());
   return maxVal;
}

template <> inline double c7x_horizontal_max_fp(c7x::double_vec vin)
{
   double2 vin1   = __max(vin.hi(), vin.lo());
   double  maxVal = __max(vin1.hi(), vin1.lo());
   return maxVal;
}

template <typename dataType, typename V> inline dataType c7x_horizontal_min_fp(V vin);
template <> inline float                                 c7x_horizontal_min_fp(c7x::float_vec vin)
{

   float4 vin1   = __min(vin.hi(), vin.lo());
   float2 vin2   = __min(vin1.hi(), vin1.lo());
   float  minVal = __min(vin2.hi(), vin2.lo());
   return minVal;
}

template <> inline double c7x_horizontal_min_fp(c7x::double_vec vin)
{
   double2 vin1   = __min(vin.hi(), vin.lo());
   double  minVal = __min(vin1.hi(), vin1.lo());
   return minVal;
}

template <typename V, typename W> inline void c7x_horizontal_add(V inVec, W *horizontalSum);

template <> inline void c7x_horizontal_add(c7x::float_vec inVec, float *horizontalSum)
{
   float4 inVec1  = inVec.hi() + inVec.lo();
   float2 inVec2  = inVec1.hi() + inVec1.lo();
   *horizontalSum = inVec2.hi() + inVec2.lo();
}

template <> inline void c7x_horizontal_add(c7x::double_vec inVec, double *horizontalSum)
{
   double2 inVec1 = inVec.hi() + inVec.lo();
   *horizontalSum = inVec1.hi() + inVec1.lo();
}

template <typename V, typename W>
inline void c7x_horizontal_min_with_index(V minValVec, V vIdx, W *minVal, int *minIdx);

template <>
inline void c7x_horizontal_min_with_index(c7x::float_vec minValVec, c7x::float_vec vIdx, float *minVal, int *minIdx)
{
   __vpred vpMask;
   vpMask            = __cmp_lt_pred(minValVec.even(), minValVec.odd());
   float4 minValVec1 = (float4) __select(vpMask, minValVec.even(), minValVec.odd());
   float4 vIdx1      = (float4) __select(vpMask, vIdx.even(), vIdx.odd());

   vpMask            = __cmp_lt_pred(minValVec1.even(), minValVec1.odd());
   float2 minValVec2 = (float2) __select(vpMask, minValVec1.even(), minValVec1.odd());
   float2 vIdx2      = (float2) __select(vpMask, vIdx1.even(), vIdx1.odd());

   vpMask  = __cmp_lt_pred(minValVec2.even(), minValVec2.odd());
   *minVal = (float) __select(vpMask, minValVec2.even(), minValVec2.odd());
   *minIdx = ((int) __select(vpMask, vIdx2.even(), vIdx2.odd()));
}

template <>
inline void c7x_horizontal_min_with_index(c7x::double_vec minValVec, c7x::double_vec vIdx, double *minVal, int *minIdx)
{
   __vpred vpMask;
   vpMask             = __cmp_lt_pred(minValVec.even(), minValVec.odd());
   double2 minValVec1 = (double2) __select(vpMask, minValVec.even(), minValVec.odd());
   double2 vIdx1      = (double2) __select(vpMask, vIdx.even(), vIdx.odd());

   vpMask  = __cmp_lt_pred(minValVec1.even(), minValVec1.odd());
   *minVal = (double) __select(vpMask, minValVec1.even(), minValVec1.odd());
   *minIdx = (int) (__select(vpMask, vIdx1.even(), vIdx1.odd()));
}
#endif

#endif
