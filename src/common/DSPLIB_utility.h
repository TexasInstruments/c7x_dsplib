// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_DSPLIB_UTILITY_H_
#define COMMON_DSPLIB_UTILITY_H_ 1

#include <float.h>  // for max float, double values
#include <limits.h> // for min, max integer values
#include <math.h>

#include "DSPLIB_bufParams.h"
#include "DSPLIB_types.h"

/*******************************************************************************
 *
 * EXTERNAL VARIABLES
 *
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif                     /* __cplusplus */
extern uint64_t beg_count; /* Begin cycle count for profiling */
extern uint64_t end_count; /* End cycle count for profiling */
extern uint64_t overhead;  /* Cycle profiling overhead */
#ifdef __cplusplus
}
#endif /* __cplusplus */

/*******************************************************************************
 *
 * Definition and arithmetic for DSPLIB_int128_t class
 *
 ******************************************************************************/

// Define a 128-bit integer class to allow natural-c implementations of DSPLIB
// 32-bit input/output functions to be templated.  The class is implemented in
// a header file for easy sharing.  All member functions, including constructors
// are declared inline for two reasons: (1) performance and (2) necessary for
// implementing the class in a multiple-inclusion header file.

class DSPLIB_int128_t {
 public:
   int64_t hi;
   int64_t lo;
   DSPLIB_int128_t(int64_t h,
                   int64_t l); // constructor for both high and low specified
   DSPLIB_int128_t(int64_t l); // constructor for just low specified (sign extends to high)
   DSPLIB_int128_t();          // constructor for neither field specified
   DSPLIB_int128_t operator+(const DSPLIB_int128_t &) const; // operator +
   DSPLIB_int128_t operator>>(const int8_t &) const;         // operator >>
};

// define constructor
inline DSPLIB_int128_t::DSPLIB_int128_t(int64_t h, int64_t l)
{
   hi = h;
   lo = l;
}

// define constructor
inline DSPLIB_int128_t::DSPLIB_int128_t(int64_t l)
{
   // sign extend l
   hi = (((uint64_t) l & 0x8000000000000000ULL) != 0LL) ? (int64_t) 0xffffffffffffffffULL
                                                        : (int64_t) 0x0000000000000000ULL;
   lo = l;
}

// define constructor
inline DSPLIB_int128_t::DSPLIB_int128_t()
{
   hi = 0x0000000000000000LL;
   lo = 0x0000000000000000LL;
}

static inline void DSPLIB_UTIL_shiftRight128(uint64_t *rh, // result
                                             uint64_t *rl,
                                             uint64_t  ah, // operand
                                             uint64_t  al,
                                             int32_t   sh, // shift amount
                                             int32_t   s)    // signed
{
   uint64_t h;
   uint64_t l;
   int32_t  i;

   h = ah;
   l = al;
   for (i = 0; i < sh; i++) {
      l = (uint64_t) __shift_right(l, (uint32_t) 1) | (uint64_t) __shift_left(h, (uint32_t) 63);
      h = (uint64_t) __shift_right(h, (uint32_t) 1) | ((s != 0) ? (h & 0x8000000000000000ULL) : 0ULL);
   }

   *rh = h;
   *rl = l;
}

static inline void DSPLIB_UTIL_Add128(uint64_t *rh, // result
                                      uint64_t *rl,
                                      uint64_t  ah, // left operand
                                      uint64_t  al,
                                      uint64_t  bh, // right operand
                                      uint64_t  bl)
{
   // break up the operands into 4 32b chunks packed into 64b uints
   uint64_t all;
   uint64_t alh;
   uint64_t ahl;
   uint64_t ahh;
   uint64_t bll;
   uint64_t blh;
   uint64_t bhl;
   uint64_t bhh;
   uint64_t s0;
   uint64_t s1;
   uint64_t s2;
   uint64_t s3;
   uint64_t sh;
   uint64_t sl;

   all = (uint64_t) __shift_right(al, (uint32_t) 0) & 0x0ffffffffULL;
   alh = (uint64_t) __shift_right(al, (uint32_t) 32) & 0x0ffffffffULL;
   ahl = (uint64_t) __shift_right(ah, (uint32_t) 0) & 0x0ffffffffULL;
   ahh = (uint64_t) __shift_right(ah, (uint32_t) 32) & 0x0ffffffffULL;

   bll = (uint64_t) __shift_right(bl, (uint32_t) 0) & 0x0ffffffffULL;
   blh = (uint64_t) __shift_right(bl, (uint32_t) 32) & 0x0ffffffffULL;
   bhl = (uint64_t) __shift_right(bh, (uint32_t) 0) & 0x0ffffffffULL;
   bhh = (uint64_t) __shift_right(bh, (uint32_t) 32) & 0x0ffffffffULL;

   // the adds
   s0 = all + bll;
   s1 = alh + blh + __shift_right(s0, (uint32_t) 32);
   s2 = ahl + bhl + __shift_right(s1, (uint32_t) 32);
   s3 = ahh + bhh + __shift_right(s2, (uint32_t) 32);

   // pack the results
   sl = (uint64_t) __shift_left(s1, (uint32_t) 32) | (s0 & 0x0ffffffffULL);
   sh = (uint64_t) __shift_left(s3, (uint32_t) 32) | (s2 & 0x0ffffffffULL);

   *rl = sl;
   *rh = sh;
}

// define overloaded + (plus) operator
inline DSPLIB_int128_t DSPLIB_int128_t::operator+(const DSPLIB_int128_t &b) const
{
   DSPLIB_int128_t result;

   DSPLIB_UTIL_Add128((uint64_t *) &(result.hi), (uint64_t *) &(result.lo), this->hi, this->lo, b.hi, b.lo);

   return result;
}

// define overloaded >> (bit shift right) operator
inline DSPLIB_int128_t DSPLIB_int128_t::operator>>(const int8_t &shift) const
{
   DSPLIB_int128_t result;

   DSPLIB_UTIL_shiftRight128((uint64_t *) &result.hi, (uint64_t *) &result.lo, this->hi, this->lo, (int32_t) shift, 1);
   return result;
}

static inline DSPLIB_int128_t DSPLIB_UTIL_mult(int64_t a, int64_t b)
{
   DSPLIB_int128_t result(0, 0);

   result.lo = (int64_t) a * (int64_t) b;
   // sign extend the product
   result.hi =
       (((uint64_t) result.lo & 0x8000000000000000ULL) != 0LL) ? (int64_t) 0xffffffffffffffffULL : (int64_t) 0ULL;

   return result;
}

#endif
