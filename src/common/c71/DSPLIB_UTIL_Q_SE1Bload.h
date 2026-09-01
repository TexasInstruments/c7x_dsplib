// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_DSPLIB_UTIL_Q_SE1BLOAD_H_
#define COMMON_DSPLIB_UTIL_Q_SE1BLOAD_H_ 1

/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup dsplib_util_q
 * \brief   load numRows of B into the Bload buffer
 * \details use streaming engine 1 to load numRows of B into the Bload buffer
 * \param  numRows, vQFactor
 */
/******************************************************************************/
__INLINE_FUNC(DSPLIB_UTIL_Q_SE1Bload)
template <uint32_t UNROLL_FACTOR> static inline void DSPLIB_UTIL_Q_SE1Bload(int32_t numRows, c7x::float_vec vQFactor)
{
   _nassert(numRows > 0);

   int32_t r;

   DSPLIB_UNROLL(UNROLL_FACTOR)
   /* Load numRows vectors into B panel */
   for (r = 0; r < numRows; r++) {

      /* Fetch B vector from SE1 into C7x register */
      c7x::float_vec vB = c7x::strm_eng<1, c7x::float_vec>::get_adv();

      /* Convert to Q */
      c7x::int_vec vBQ = __float_to_int((vB * vQFactor));

      /* Load B matrix panel with B vector */
      __HWALDB(c7x::reinterpret<__mma_vec>(vBQ));

      /* Advance HWA State Machine */
      __HWAADV();
   }

   return;
}
#endif /* COMMON_DSPLIB_UTIL_Q_SE1BLOAD_H_ */
