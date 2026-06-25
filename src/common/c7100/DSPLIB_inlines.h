// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef C7100_MMA_INLINES_H
#define C7100_MMA_INLINES_H

/*******************************************************************************
 *
 * Inlined functions
 *
 ******************************************************************************/
#ifdef __cplusplus
#include <c7x_scalable.h>

/* Horizontal maximum value implementation */
template <typename V> inline uint64_t c7x_horizontal_max(V vin);

template <> inline uint64_t c7x_horizontal_max(c7x::uchar_vec vin)
{
   uchar32  m1     = __max(vin.even(), vin.odd());
   uchar16  m2     = __max(m1.even(), m1.odd());
   uchar8   m3     = __max(m2.even(), m2.odd());
   uchar4   m4     = __max(m3.even(), m3.odd());
   uchar2   m5     = __max(m4.even(), m4.odd());
   uint64_t retVal = (uint64_t) __max(m5.even(), m5.odd());
   return retVal;
}
template <> inline uint64_t c7x_horizontal_max(c7x::ushort_vec vin)
{
   ushort16 m1     = __max(vin.even(), vin.odd());
   ushort8  m2     = __max(m1.even(), m1.odd());
   ushort4  m3     = __max(m2.even(), m2.odd());
   ushort2  m4     = __max(m3.even(), m3.odd());
   uint64_t retVal = (uint64_t) __max(m4.even(), m4.odd());
   return retVal;
}
template <> inline uint64_t c7x_horizontal_max(c7x::uint_vec vin)
{
   uint64_t retVal = (uint64_t) (__sort_asc((uint16) vin).sf());
   return retVal;
}
template <> inline uint64_t c7x_horizontal_max(c7x::ulong_vec vin)
{
   ulong4   m1     = __max(vin.even(), vin.odd());
   ulong2   m2     = __max(m1.even(), m1.odd());
   uint64_t retVal = (uint64_t) __max(m2.even(), m2.odd());
   return retVal;
}

// Horizontal max computation with index

template <typename V, typename W>
inline void c7x_horizontal_max_with_index(V maxValVec, V vIdx, W *maxVal, int *maxIdx);

template <>
inline void c7x_horizontal_max_with_index(c7x::float_vec maxValVec, c7x::float_vec vIdx, float *maxVal, int *maxIdx)
{
   __vpred vpMask;
   vpMask            = __cmp_lt_pred(maxValVec.even(), maxValVec.odd());
   float8 maxValVec1 = (float8) __select(vpMask, maxValVec.odd(), maxValVec.even());
   float8 vIdx1      = (float8) __select(vpMask, vIdx.odd(), vIdx.even());

   vpMask            = __cmp_lt_pred(maxValVec1.even(), maxValVec1.odd());
   float4 maxValVec2 = (float4) __select(vpMask, maxValVec1.odd(), maxValVec1.even());
   float4 vIdx2      = (float4) __select(vpMask, vIdx1.odd(), vIdx1.even());

   vpMask            = __cmp_lt_pred(maxValVec2.even(), maxValVec2.odd());
   float2 maxValVec3 = (float2) __select(vpMask, maxValVec2.odd(), maxValVec2.even());
   float2 vIdx3      = (float2) __select(vpMask, vIdx2.odd(), vIdx2.even());

   vpMask  = __cmp_lt_pred(maxValVec3.even(), maxValVec3.odd());
   *maxVal = (float) __select(vpMask, maxValVec3.odd(), maxValVec3.even());
   *maxIdx = ((int) __select(vpMask, vIdx3.odd(), vIdx3.even()));
}

template <>
inline void c7x_horizontal_max_with_index(c7x::double_vec maxValVec, c7x::double_vec vIdx, double *maxVal, int *maxIdx)
{
   __vpred vpMask;
   vpMask             = __cmp_lt_pred(maxValVec.even(), maxValVec.odd());
   double4 maxValVec1 = (double4) __select(vpMask, maxValVec.odd(), maxValVec.even());
   double4 vIdx1      = (double4) __select(vpMask, vIdx.odd(), vIdx.even());

   vpMask             = __cmp_lt_pred(maxValVec1.even(), maxValVec1.odd());
   double2 maxValVec2 = (double2) __select(vpMask, maxValVec1.odd(), maxValVec1.even());
   double2 vIdx2      = (double2) __select(vpMask, vIdx1.odd(), vIdx1.even());

   vpMask  = __cmp_lt_pred(maxValVec2.even(), maxValVec2.odd());
   *maxVal = (double) __select(vpMask, maxValVec2.odd(), maxValVec2.even());
   *maxIdx = ((int) __select(vpMask, vIdx2.odd(), vIdx2.even()));
}

template <>
inline void c7x_horizontal_max_with_index(c7x::char_vec maxValVec, c7x::char_vec vIdx, int8_t *maxVal, int *maxIdx)
{
   __vpred                                              vpMask;
   typedef typename c7x::make_full_vector<int8_t>::type vec;

   char32 maxValVec1 = maxValVec.even();
   maxValVec1        = __max(maxValVec.odd(), maxValVec1);

   char16 maxValVec2 = maxValVec1.even();
   maxValVec2        = __max(maxValVec1.odd(), maxValVec2);

   char8 maxValVec3 = maxValVec2.even();
   maxValVec3       = __max(maxValVec2.odd(), maxValVec3);

   char4 maxValVec4 = maxValVec3.even();
   maxValVec4       = __max(maxValVec3.odd(), maxValVec4);

   char2 maxValVec5 = maxValVec4.even();
   maxValVec5       = __max(maxValVec4.odd(), maxValVec5);

   int8_t maxValVec6   = maxValVec5.even();
   maxValVec6          = __max(maxValVec5.odd(), maxValVec6);
   *maxVal             = (int8_t) maxValVec6;
   int8_t maxValScaler = (int8_t) maxValVec6;

   char64 zero_vec         = vec(0);
   char64 diff_vector      = vec(maxValScaler) - maxValVec;
   vpMask                  = __cmp_eq_pred(diff_vector, zero_vec);
   char64  masked_indices  = __select(vpMask, vIdx, vec(-1));
   uchar64 umasked_indices = __as_uchar64(masked_indices);

   uchar32 vIdx1 = (uchar32) __min(umasked_indices.even(), umasked_indices.odd());
   uchar16 vIdx2 = (uchar16) __min(vIdx1.even(), vIdx1.odd());
   uchar8  vIdx3 = (uchar8) __min(vIdx2.even(), vIdx2.odd());
   uchar4  vIdx4 = (uchar4) __min(vIdx3.even(), vIdx3.odd());
   uchar2  vIdx5 = (uchar2) __min(vIdx4.even(), vIdx4.odd());
   *maxIdx       = (uint8_t) __min(vIdx5.even(), vIdx5.odd());
}

template <>
inline void c7x_horizontal_max_with_index(c7x::short_vec maxValVec, c7x::short_vec vIdx, int16_t *maxVal, int *maxIdx)
{
   __vpred                                               vpMask;
   typedef typename c7x::make_full_vector<int16_t>::type vec;

   vec sortIn = __vdsortdd16h_vv(maxValVec);
   *maxVal    = (short) (sortIn.lo().s[0] > sortIn.hi().s[0] ? sortIn.lo().s[0] : sortIn.hi().s[0]);

   short maxValScaler = (short) (sortIn.lo().s[0] > sortIn.hi().s[0] ? sortIn.lo().s[0] : sortIn.hi().s[0]);

   short32 zero_vec       = vec(0);
   short32 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask                 = __cmp_eq_pred(diff_vector, zero_vec);
   short32 masked_indices = __select(vpMask, vIdx, vec(255));
   short32 sorted_indices = __vdsortii16h_vv(masked_indices);

   *maxIdx = (sorted_indices.s[0] < sorted_indices.s[16]) ? sorted_indices.s[0] : sorted_indices.s[16];
}

template <>
inline void c7x_horizontal_max_with_index(c7x::int_vec maxValVec, c7x::int_vec vIdx, int32_t *maxVal, int *maxIdx)
{
   __vpred                                               vpMask;
   typedef typename c7x::make_full_vector<int32_t>::type vec;

   vec sortIn = __sort_desc(maxValVec);
   *maxVal    = (int) sortIn.s[0];

   int maxValScaler = (int) sortIn.s[0];

   int16 zero_vec       = vec(0);
   int16 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask               = __cmp_eq_pred(diff_vector, zero_vec);
   int16 masked_indices = __select(vpMask, vIdx, vec(255));
   int16 sorted_indices = __sort_asc(masked_indices);

   *maxIdx = sorted_indices.s[0];
}

template <>
inline void c7x_horizontal_max_with_index(c7x::long_vec maxValVec, c7x::long_vec vIdx, int64_t *maxVal, int *maxIdx)
{
   __vpred                                               vpMask;
   typedef typename c7x::make_full_vector<int64_t>::type vec;

   long4 maxValVec1 = maxValVec.even();
   maxValVec1       = __max(maxValVec.odd(), maxValVec1);

   long2 maxValVec2 = maxValVec1.even();
   maxValVec2       = __max(maxValVec1.odd(), maxValVec2);

   long maxValVec3 = maxValVec2.even();
   maxValVec3      = __max(maxValVec2.odd(), maxValVec3);
   *maxVal         = (long) maxValVec3;

   long maxValScaler = (long) maxValVec3;

   long8 zero_vec       = vec(0);
   long8 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask               = __cmp_eq_pred(diff_vector, zero_vec);
   long8 maxIdxVec      = vec(255);
   long8 masked_indices = __select(vpMask, vIdx, maxIdxVec);

   long4 vIdx1 = (long4) __min(masked_indices.even(), masked_indices.odd());
   long2 vIdx2 = (long2) __min(vIdx1.even(), vIdx1.odd());
   *maxIdx     = (long) __min(vIdx2.even(), vIdx2.odd());
}

template <>
inline void c7x_horizontal_max_with_index(c7x::uchar_vec maxValVec, c7x::uchar_vec vIdx, uint8_t *maxVal, int *maxIdx)
{
   __vpred                                               vpMask;
   typedef typename c7x::make_full_vector<uint8_t>::type vec;

   uchar32 maxValVec1 = maxValVec.even();
   maxValVec1         = __max(maxValVec.odd(), maxValVec1);

   uchar16 maxValVec2 = maxValVec1.even();
   maxValVec2         = __max(maxValVec1.odd(), maxValVec2);

   uchar8 maxValVec3 = maxValVec2.even();
   maxValVec3        = __max(maxValVec2.odd(), maxValVec3);

   uchar4 maxValVec4 = maxValVec3.even();
   maxValVec4        = __max(maxValVec3.odd(), maxValVec4);

   uchar2 maxValVec5 = maxValVec4.even();
   maxValVec5        = __max(maxValVec4.odd(), maxValVec5);

   uint8_t maxValVec6   = maxValVec5.even();
   maxValVec6           = __max(maxValVec5.odd(), maxValVec6);
   *maxVal              = (uint8_t) maxValVec6;
   uint8_t maxValScaler = (uint8_t) maxValVec6;

   uchar64 zero_vec       = vec(0);
   uchar64 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask                 = __cmp_eq_pred(diff_vector, zero_vec);
   uchar64 maxIdxVec      = vec(255);
   uchar64 masked_indices = __select(vpMask, vIdx, maxIdxVec);

   uchar32 vIdx1 = (uchar32) __min(masked_indices.even(), masked_indices.odd());
   uchar16 vIdx2 = (uchar16) __min(vIdx1.even(), vIdx1.odd());
   uchar8  vIdx3 = (uchar8) __min(vIdx2.even(), vIdx2.odd());
   uchar4  vIdx4 = (uchar4) __min(vIdx3.even(), vIdx3.odd());
   uchar2  vIdx5 = (uchar2) __min(vIdx4.even(), vIdx4.odd());
   *maxIdx       = (int) __min(vIdx5.even(), vIdx5.odd());
}

template <>
inline void
c7x_horizontal_max_with_index(c7x::ushort_vec maxValVec, c7x::ushort_vec vIdx, uint16_t *maxVal, int *maxIdx)
{
   __vpred                                                vpMask;
   typedef typename c7x::make_full_vector<uint16_t>::type vec;

   vec sortIn = __vdsortddu16h_vv(maxValVec);
   *maxVal    = (ushort) (sortIn.lo().s[0] > sortIn.hi().s[0] ? sortIn.lo().s[0] : sortIn.hi().s[0]);

   ushort maxValScaler = (ushort) (sortIn.lo().s[0] > sortIn.hi().s[0] ? sortIn.lo().s[0] : sortIn.hi().s[0]);

   ushort32 zero_vec       = vec(0);
   ushort32 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask                  = __cmp_eq_pred(diff_vector, zero_vec);
   ushort32 masked_indices = __select(vpMask, vIdx, vec(255));
   ushort32 sorted_indices = __vdsortiiu16h_vv(masked_indices);

   *maxIdx = (sorted_indices.s[0] < sorted_indices.s[16]) ? sorted_indices.s[0] : sorted_indices.s[16];
}

template <>
inline void c7x_horizontal_max_with_index(c7x::uint_vec maxValVec, c7x::uint_vec vIdx, uint32_t *maxVal, int *maxIdx)
{
   __vpred                                                vpMask;
   typedef typename c7x::make_full_vector<uint32_t>::type vec;

   vec sortIn = __sort_desc(maxValVec);
   *maxVal    = (uint) sortIn.s[0];

   uint maxValScaler = (uint) sortIn.s[0];

   uint16 zero_vec       = vec(0);
   uint16 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask                = __cmp_eq_pred(diff_vector, zero_vec);
   uint16 masked_indices = __select(vpMask, vIdx, vec(255));
   uint16 sorted_indices = __sort_asc(masked_indices);

   *maxIdx = sorted_indices.s[0];
}

template <>
inline void c7x_horizontal_max_with_index(c7x::ulong_vec maxValVec, c7x::ulong_vec vIdx, uint64_t *maxVal, int *maxIdx)
{
   __vpred                                                vpMask;
   typedef typename c7x::make_full_vector<uint64_t>::type vec;
   ulong4                                                 maxValVec1 = maxValVec.even();
   maxValVec1                                                        = __max(maxValVec.odd(), maxValVec1);

   ulong2 maxValVec2 = maxValVec1.even();
   maxValVec2        = __max(maxValVec1.odd(), maxValVec2);

   ulong maxValVec3 = maxValVec2.even();
   maxValVec3       = __max(maxValVec2.odd(), maxValVec3);
   *maxVal          = (ulong) maxValVec3;

   ulong maxValScaler = (ulong) maxValVec3;

   ulong8 zero_vec       = vec(0);
   ulong8 diff_vector    = vec(maxValScaler) - maxValVec;
   vpMask                = __cmp_eq_pred(diff_vector, zero_vec);
   ulong8 maxIdxVec      = vec(255);
   ulong8 masked_indices = __select(vpMask, vIdx, maxIdxVec);

   ulong4 vIdx1 = (ulong4) __min(masked_indices.even(), masked_indices.odd());
   ulong2 vIdx2 = (ulong2) __min(vIdx1.even(), vIdx1.odd());
   *maxIdx      = (ulong) __min(vIdx2.even(), vIdx2.odd());
}

// Convert long to int

template <typename X, typename Y> inline typename c7x::make_full_vector<X>::type convert_long_to_int(Y vec);

template <> inline c7x::make_full_vector<int32_t>::type convert_long_to_int<int16, long8>(long8 vec)
{
   typedef typename c7x::make_full_vector<int32_t>::type vecRet;
   vecRet                                                v8bits = __as_int16(vec);
   return v8bits;
}

template <> inline c7x::make_full_vector<uint32_t>::type convert_long_to_int<uint16, ulong8>(ulong8 vec)
{
   typedef typename c7x::make_full_vector<uint32_t>::type vecRet;
   vecRet                                                 v8bits = __as_uint16(vec);
   return v8bits;
}

// Convert char to short

template <typename X, typename Y>
inline typename c7x::make_full_vector<X>::type convert_char_to_short(Y vecIn, bool typeIndex);

template <>
inline c7x::make_full_vector<int16_t>::type convert_char_to_short<short32, char32>(char32 vecIn, bool typeIndex)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecRet;
   vecRet                                                vecOut;
   if (typeIndex) {
      uchar32 vecInConv = __as_uchar32(vecIn);
      vecOut            = __convert_short32(vecInConv);
   }
   else {
      vecOut = __convert_short32(vecIn);
   }
   return vecOut;
}

template <>
inline c7x::make_full_vector<uint16_t>::type convert_char_to_short<ushort32, uchar32>(uchar32 vecIn, bool typeIndex)
{
   typedef typename c7x::make_full_vector<uint16_t>::type vecRet;
   vecRet                                                 vecOut = __convert_ushort32(vecIn);
   return vecOut;
}

// Multiply 2 char and result in short

template <typename X, typename Y> inline void mul_char_to_short(Y vecIn1, Y vecIn2, X vecOut1, X vecOut2);

template <>
inline void mul_char_to_short<short32 &, char32>(char32          vecIn1,
                                                 char32          vecIn2,
                                                 c7x::short_vec &vecOut1Short,
                                                 c7x::short_vec &vecOut2)
{
   short32 vecInShort1 = __convert_short32(vecIn1);
   short32 vecInShort2 = __convert_short32(vecIn2);
   vecOut1Short        = vecInShort1 * vecInShort2;
}

template <>
inline void mul_char_to_short<ushort32 &, uchar32>(uchar32          vecIn1,
                                                   uchar32          vecIn2,
                                                   c7x::ushort_vec &vecOut1uShort,
                                                   c7x::ushort_vec &vecOut2)
{
   vecOut1uShort = __mpy_ext(vecIn1, vecIn2);
}

template <typename dataType, typename V> inline dataType c7x_horizontal_max_fp(V vin);

template <> inline float c7x_horizontal_max_fp(c7x::float_vec vin)
{

   float8 vin1   = __max(vin.hi(), vin.lo());
   float4 vin2   = __max(vin1.hi(), vin1.lo());
   float2 vin3   = __max(vin2.hi(), vin2.lo());
   float  maxVal = __max(vin3.hi(), vin3.lo());
   return maxVal;
}

template <> inline double c7x_horizontal_max_fp(c7x::double_vec vin)
{
   double4 vin1   = __max(vin.hi(), vin.lo());
   double2 vin2   = __max(vin1.hi(), vin1.lo());
   double  maxVal = __max(vin2.hi(), vin2.lo());
   return maxVal;
}

template <typename dataType, typename V> inline dataType c7x_horizontal_min_fp(V vin);
template <> inline float                                 c7x_horizontal_min_fp(c7x::float_vec vin)
{

   float8 vin1   = __min(vin.hi(), vin.lo());
   float4 vin2   = __min(vin1.hi(), vin1.lo());
   float2 vin3   = __min(vin2.hi(), vin2.lo());
   float  minVal = __min(vin3.hi(), vin3.lo());
   return minVal;
}

template <> inline double c7x_horizontal_min_fp(c7x::double_vec vin)
{
   double4 vin1   = __min(vin.hi(), vin.lo());
   double2 vin2   = __min(vin1.hi(), vin1.lo());
   double  minVal = __min(vin2.hi(), vin2.lo());
   return minVal;
}

template <typename V, typename W> inline void c7x_horizontal_add(V inVec, W *horizontalSum);

template <> inline void c7x_horizontal_add(c7x::float_vec inVec, float *horizontalSum)
{
   float8 inVec1  = inVec.hi() + inVec.lo();
   float4 inVec2  = inVec1.hi() + inVec1.lo();
   float2 inVec3  = inVec2.hi() + inVec2.lo();
   *horizontalSum = inVec3.hi() + inVec3.lo();
}

template <> inline void c7x_horizontal_add(c7x::double_vec inVec, double *horizontalSum)
{
   double4 inVec1 = inVec.hi() + inVec.lo();
   double2 inVec2 = inVec1.hi() + inVec1.lo();
   *horizontalSum = inVec2.hi() + inVec2.lo();
}
template <typename V, typename W>
inline void c7x_horizontal_min_with_index(V minValVec, V vIdx, W *minVal, int *minIdx);
template <>
inline void c7x_horizontal_min_with_index(c7x::float_vec minValVec, c7x::float_vec vIdx, float *minVal, int *minIdx)
{
   __vpred vpMask;
   vpMask            = __cmp_lt_pred(minValVec.even(), minValVec.odd());
   float8 minValVec1 = (float8) __select(vpMask, minValVec.even(), minValVec.odd());
   float8 vIdx1      = (float8) __select(vpMask, vIdx.even(), vIdx.odd());

   vpMask            = __cmp_lt_pred(minValVec1.even(), minValVec1.odd());
   float4 minValVec2 = (float4) __select(vpMask, minValVec1.even(), minValVec1.odd());
   float4 vIdx2      = (float4) __select(vpMask, vIdx1.even(), vIdx1.odd());

   vpMask            = __cmp_lt_pred(minValVec2.even(), minValVec2.odd());
   float2 minValVec3 = (float2) __select(vpMask, minValVec2.even(), minValVec2.odd());
   float2 vIdx3      = (float2) __select(vpMask, vIdx2.even(), vIdx2.odd());

   vpMask  = __cmp_lt_pred(minValVec3.even(), minValVec3.odd());
   *minVal = (float) __select(vpMask, minValVec3.even(), minValVec3.odd());
   *minIdx = ((int) __select(vpMask, vIdx3.even(), vIdx3.odd()));
}

template <>
inline void c7x_horizontal_min_with_index(c7x::double_vec minValVec, c7x::double_vec vIdx, double *minVal, int *minIdx)
{
   __vpred vpMask;
   vpMask             = __cmp_lt_pred(minValVec.even(), minValVec.odd());
   double4 minValVec1 = (double4) __select(vpMask, minValVec.even(), minValVec.odd());
   double4 vIdx1      = (double4) __select(vpMask, vIdx.even(), vIdx.odd());

   vpMask             = __cmp_lt_pred(minValVec1.even(), minValVec1.odd());
   double2 minValVec2 = (double2) __select(vpMask, minValVec1.even(), minValVec1.odd());
   double2 vIdx2      = (double2) __select(vpMask, vIdx1.even(), vIdx1.odd());

   vpMask  = __cmp_lt_pred(minValVec2.even(), minValVec2.odd());
   *minVal = (double) __select(vpMask, minValVec2.even(), minValVec2.odd());
   *minIdx = ((int) __select(vpMask, vIdx2.even(), vIdx2.odd()));
}
#endif

/******************************************************************************/

#endif
