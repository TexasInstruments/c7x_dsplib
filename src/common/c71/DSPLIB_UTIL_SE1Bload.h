// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_DSPLIB_UTIL_SE1BLOAD_H_
#define COMMON_DSPLIB_UTIL_SE1BLOAD_H_ 1

/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup dsplib_util
 * \brief   load numRows of B into the Bload buffer
 * \details use streaming engine 1 to load numRows of B into the Bload buffer
 * \param  numRows
 * \return  void
 */
/******************************************************************************/
__INLINE_FUNC(DSPLIB_UTIL_SE1Bload)
template <typename dataType, uint32_t UNROLL_FACTOR> static inline void DSPLIB_UTIL_SE1Bload(int32_t numRows)
{
   _nassert(numRows > 0);

   int32_t r;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   DSPLIB_UNROLL(UNROLL_FACTOR)
   /* Load numRows vectors into B panel */
   for (r = 0; r < numRows; r++) {

      /* Fetch B vector from SE1 into C7x register */
      vec valB = c7x::strm_eng<1, vec>::get_adv();

      /* Load B matrix panel with B vector */
      __HWALDB(c7x::reinterpret<__mma_vec>(valB));

      /* Advance HWA State Machine */
      __HWAADV();
   }

   return;
}
#endif /* COMMON_DSPLIB_UTIL_SE1BLOAD_H_ */
