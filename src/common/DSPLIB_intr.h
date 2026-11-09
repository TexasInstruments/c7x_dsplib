// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_DSPLIB_INTR_H_
#define COMMON_DSPLIB_INTR_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "DSPLIB_utility.h"

static inline c7x::long_vec __vdotp(c7x::short_vec a, c7x::short_vec b)
{
    return __vdotp4hd_vvv(a, b);
}

static inline c7x::long_vec __vdotp(c7x::int_vec a, c7x::int_vec b)
{
    return __vdotp2wd_vvv(a, b);
}

static inline c7x::ushort_vec __as_unsigned(c7x::short_vec a)
{
    return c7x::as_ushort_vec(a);
}

static inline c7x::uint_vec __as_unsigned(c7x::int_vec a)
{
    return c7x::as_uint_vec(a);
}

#if defined(__C7524__)
static inline c7x::char_vec __ilut_read(c7x::char_vec a)
{
    #if __C7X_VEC_SIZE_BITS__ == 512
    return __ilut_read_char(__as_uchar64(a));
    #else
    return __ilut_read_char(__as_uchar32(a));
    #endif
}

static inline c7x::uchar_vec __ilut_read(c7x::uchar_vec a)
{
    return __ilut_read_uchar(a);
}

static inline c7x::short_vec __ilut_read(c7x::short_vec a)
{
    #if __C7X_VEC_SIZE_BITS__ == 512
    return __ilut_read_char(__as_ushort32(a));
    #else
    return __ilut_read_short(__as_ushort16(a));
    #endif
}

static inline c7x::ushort_vec __ilut_read(c7x::ushort_vec a)
{
    return __ilut_read_ushort(a);
}

static inline c7x::int_vec __ilut_read(c7x::int_vec a)
{
    #if __C7X_VEC_SIZE_BITS__ == 512
    return __ilut_read_int(__as_uint16(a));
    #else
    return __ilut_read_int(__as_uint8(a));
    #endif
}

static inline c7x::uint_vec __ilut_read(c7x::uint_vec a)
{
    return __ilut_read_uint(a);
}
#endif /* __C7524__ */

static inline void __vmpysu_vvw(c7x::int_vec sign, c7x::uint_vec uns, c7x::long_vec& lo, c7x::long_vec& hi)
{
    __vmpysuwd_vvw(sign, uns, lo, hi);
}

static inline void __vmpysu_vvw(c7x::short_vec sign, c7x::ushort_vec uns, c7x::int_vec& lo, c7x::int_vec& hi)
{
    __vmpysuhw_vvw(sign, uns, lo, hi);
}

static inline c7x::short_vec gather_elem(__vpred p, c7x::short_vec x)
{
    return c7x::reinterpret<c7x::short_vec>(
        __gather_set_bits(p, c7x::reinterpret<c7x::char_vec>(x)));
}

static inline c7x::ushort_vec gather_elem(__vpred p, c7x::ushort_vec x)
{
    return c7x::reinterpret<c7x::ushort_vec>(
        __gather_set_bits(p, c7x::reinterpret<c7x::char_vec>(x)));
}

static inline c7x::uint_vec gather_elem(__vpred p, c7x::uint_vec x)
{
    return c7x::reinterpret<c7x::uint_vec>(
        __gather_set_bits(p, c7x::reinterpret<c7x::char_vec>(x)));
}

static inline c7x::ushort_vec permute_even_odd(uint8_t *permute_mask, c7x::ushort_vec x0, c7x::ushort_vec x1)
{
    using uchar_fv = c7x::make_full_vector<uint8_t>::type;
    return c7x::reinterpret<c7x::ushort_vec>(__permute_even_odd_short(*((uchar_fv*)&permute_mask[0]), c7x::as_uchar_vec(x0), c7x::as_uchar_vec(x1)));
}

static inline c7x::uint_vec permute_even_odd(uint8_t *permute_mask, c7x::uint_vec x0, c7x::uint_vec x1)
{
    using uchar_fv = c7x::make_full_vector<uint8_t>::type;
    return c7x::reinterpret<c7x::uint_vec>(__permute_even_odd_int(*((uchar_fv*)&permute_mask[0]), c7x::as_uchar_vec(x0), c7x::as_uchar_vec(x1)));
}

template <typename T>
inline T get_ramp(void);

template <>
inline uint8 get_ramp(void)
{
    return uint8(0, 1, 2, 3, 4, 5, 6, 7);
}

template <>
inline ushort16 get_ramp(void)
{
    return ushort16(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
}

template <>
inline ushort8 get_ramp(void)
{
    return ushort8(0, 1, 2, 3, 4, 5, 6, 7);
}

template <typename T>
inline uint32_t popcount(__vpred x);

template <>
inline uint32_t popcount<c7x::ushort_vec>(__vpred x)
{
    return __popcount_short(x);
}

template <>
inline uint32_t popcount<c7x::uint_vec>(__vpred x)
{
    return __popcount_int(x);
}

template <typename T>
inline __vpred make_mask(uint32_t cnt);

template <>
inline __vpred make_mask<c7x::ushort_vec>(uint32_t cnt)
{
    return __mask_short(cnt);
}

template <>
inline __vpred make_mask<c7x::uint_vec>(uint32_t cnt)
{
    return __mask_int(cnt);
}

static inline void vmpy(c7x::ushort_vec a, c7x::ushort_vec b, c7x::uint_vec &c, c7x::uint_vec &d)
{
    __vmpyuhw_vvw(a, b, c, d);
}

static inline void vmpy(c7x::uint_vec a, c7x::uint_vec b, c7x::ulong_vec &c, c7x::ulong_vec &d)
{
    __vmpyuwd_vvw(a, b, c, d);
}

// Multiply -> round -> shift
static inline c7x::short_vec __vmultroundshift(c7x::short_vec a, c7x::short_vec b)
{
    return __vsmpyr1hh_vvv(a, b);
}

static inline c7x::int_vec __vmultroundshift(c7x::int_vec a, c7x::int_vec b)
{
    return __vsmpyr1ww_vvv(a, b);
}

// Multiply signed vector
static inline void __vmpys(c7x::short_vec a, c7x::short_vec b, c7x::int_vec &y_h, c7x::int_vec &y_l)
{
    __vmpyhw_vvw(a, b, y_h, y_l);
}
static inline void __vmpys(c7x::int_vec a, c7x::int_vec b, c7x::long_vec &y_h, c7x::long_vec &y_l)
{
    __vmpywd_vvw(a, b, y_h, y_l);
}

// Right-shfit
static inline c7x::short_vec __vrsh(c7x::short_vec vector, int32_t right_shift)
{
    c7x::int_vec shift = c7x::int_vec(right_shift);
    return __vshrh_vvv(vector, shift);
}
static inline c7x::int_vec __vrsh(c7x::int_vec vector, int32_t right_shift)
{
    c7x::int_vec shift = c7x::int_vec(right_shift);
    return __vshvrw_vvv(vector, shift);
}
static inline c7x::long_vec __vrsh(c7x::long_vec vector, int64_t right_shift)
{
    c7x::long_vec shift = c7x::long_vec(right_shift);
    return __vshrd_vvv(vector, shift);
}

static inline c7x::short_vec deal_vector(c7x::short_vec a)
{
    return __vdeal2h_vv(a);
}

static inline c7x::int_vec deal_vector(c7x::int_vec a)
{
    return __vdeal2w_vv(a);
}

#endif //COMMON_DSPLIB_INTR_H_
