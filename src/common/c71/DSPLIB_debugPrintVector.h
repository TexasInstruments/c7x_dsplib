// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_DSPLIB_UTIL_DEBUGPRINTVECTOR_H_
#define COMMON_DSPLIB_UTIL_DEBUGPRINTFECTOR_H_ 1

#include "../DSPLIB_types.h"

//----------------------------------------------------------------------------------
#ifdef __cplusplus
#include <c7x_scalable.h> // for device scalability

#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintVector(c7x::uchar_vec vector)
{
#if DSPLIB_DEBUGPRINT
   for (uint i = 0; i < c7x::element_count_of<c7x::uchar_vec>::value; i++) {
      DSPLIB_PRINTF("%04u", *((uchar *) (&vector) + i));
   }

   DSPLIB_PRINTF("%s", "\n");
#endif
}

#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintVector(c7x::char_vec vector)
{
#if DSPLIB_DEBUGPRINT
   for (uint i = 0; i < c7x::element_count_of<c7x::char_vec>::value; i++) {
      DSPLIB_PRINTF("%04d", *((int8_t *) (&vector) + i));
   }

   DSPLIB_PRINTF("%s", "\n");
#endif
}

#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintVector(c7x::ushort_vec vector)
{
#if DSPLIB_DEBUGPRINT
   for (uint i = 0; i < c7x::element_count_of<c7x::ushort_vec>::value; i++) {
      DSPLIB_PRINTF("%05u", *((ushort *) (&vector) + i));
   }

   DSPLIB_PRINTF("%s", "\n");
#endif
}

#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintVector(c7x::short_vec vector)
{
#if DSPLIB_DEBUGPRINT
   for (uint i = 0; i < c7x::element_count_of<c7x::short_vec>::value; i++) {
      DSPLIB_PRINTF("%05d", *((short *) (&vector) + i));
   }

   DSPLIB_PRINTF("%s", "\n");
#endif
}

#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintVector(c7x::uint_vec vector)
{
#if DSPLIB_DEBUGPRINT
   for (uint i = 0; i < c7x::element_count_of<c7x::uint_vec>::value; i++) {
      DSPLIB_PRINTF("%010u", *((uint *) (&vector) + i));
   }

   DSPLIB_PRINTF("%s", "\n");
#endif
}

#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintVector(c7x::float_vec vector)
{
#if DSPLIB_DEBUGPRINT
   for (uint i = 0; i < c7x::element_count_of<c7x::float_vec>::value; i++) {
      DSPLIB_PRINTF("%f ", *((float *) (&vector) + i));
   }

   DSPLIB_PRINTF("%s", "\n");
#endif
}

#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintVector(c7x::double_vec vector)
{
#if MATHLIB_DEBUGPRINT
   for (uint i = 0; i < c7x::element_count_of<c7x::double_vec>::value; i++) {
      MATHLIB_PRINTF("%lf", *((double *) (&vector) + i));
   }

   MATHLIB_PRINTF("%s", "\n");
#endif
}
#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintVector(c7x::int_vec vector)
{
#if DSPLIB_DEBUGPRINT
   for (uint i = 0; i < c7x::element_count_of<c7x::int_vec>::value; i++) {
      DSPLIB_PRINTF("%010d", *((int *) (&vector) + i));
   }

   DSPLIB_PRINTF("%s", "\n");
#endif
}

#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintVectorInHex(c7x::long_vec vector)
{
#if DSPLIB_DEBUGPRINT
   for (uint i = 0; i < c7x::element_count_of<c7x::long_vec>::value; i++) {
      DSPLIB_PRINTF("%016lx", *((long *) (&vector) + i));
   }

   DSPLIB_PRINTF("%s", "\n");
#endif
}

#if 0
#pragma FUNC_ALWAYS_INLINE
static inline void DSPLIB_debugPrintNonScalableVectorInHex(long8 vector)
{
#if DSPLIB_DEBUGPRINT
   DSPLIB_PRINTF("%010lx ", vector.s[7]);
   DSPLIB_PRINTF("%010lx ", vector.s[6]);
   DSPLIB_PRINTF("%010lx ", vector.s[5]);
   DSPLIB_PRINTF("%010lx ", vector.s[4]);
   DSPLIB_PRINTF("%010lx ", vector.s[3]);
   DSPLIB_PRINTF("%010lx ", vector.s[2]);
   DSPLIB_PRINTF("%010lx ", vector.s[1]);
   DSPLIB_PRINTF("%010lx ", vector.s[0]);

   DSPLIB_PRINTF("%s", "\n");
#endif
   return;
}
#endif

#endif //#ifdef __cplusplus
#endif /* COMMON_DSPLIB_UTIL_DEBUGPRINTVECTOR_H_ */
