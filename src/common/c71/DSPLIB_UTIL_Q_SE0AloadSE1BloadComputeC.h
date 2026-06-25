// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_DSPLIB_UTIL_Q_SE0ALOADSE1BLOADCOMPUTEC_H_
#define COMMON_DSPLIB_UTIL_Q_SE0ALOADSE1BLOADCOMPUTEC_H_ 1

/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup dsplib_util
 * \brief
 * \details
 * \param[in] numRows
 * \return  void
 */
/******************************************************************************/

__INLINE_FUNC(DSPLIB_UTIL_Q_SE0AloadSE1BloadComputeC)
template <uint32_t UNROLL_FACTOR>
static inline void DSPLIB_UTIL_Q_SE0AloadSE1BloadComputeC(int32_t numRows, c7x::float_vec vQFactor)
{

   _nassert(numRows > 0);

   int32_t r;

   DSPLIB_UNROLL(UNROLL_FACTOR)
   /* Load A and B vectors numRows times into respective MMA panels */
   for (r = 0; r < numRows; r++) {

      /* Fetch A vector from SE0 */
      c7x::float_vec vA = c7x::strm_eng<0, c7x::float_vec>::get_adv();

      /*Convert to Q Format */
      c7x::int_vec vAQ = __float_to_int((vA * vQFactor));

      /* Fetch B vector from SE1 */
      c7x::float_vec vB = c7x::strm_eng<1, c7x::float_vec>::get_adv();

      /*Convert to Q Format */
      c7x::int_vec vBQ = __float_to_int((vB * vQFactor));

      /* Load vA into A vector panel and vB into B matrix panel */
      __HWALDAB(c7x::reinterpret<__mma_vec>(vAQ), c7x::reinterpret<__mma_vec>(vBQ));

      /* Compute C vector into C panel */
      /* Perform OP with values from A panel, not ARF */
      __HWAOP(__MMA_A_LDA);

      /* Advance HWA State Machine */
      __HWAADV();
   }

   return;
}

#endif // COMMON_DSPLIB_UTIL_Q_SE0ALOADSE1BLOADCOMPUTEC_H_
