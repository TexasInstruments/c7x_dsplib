// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_DSPLIB_UTIL_SE0ALOADSE1BLOADCOMPUTECSA0CSTORE_H_
#define COMMON_DSPLIB_UTIL_SE0ALOADSE1BLOADCOMPUTECSA0CSTORE_H_ 1

/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup dsplib_util
 * \brief
 * \details
 * \param[in] numRows
 * \param[in] cBackDst
 * \return  void
 */
/******************************************************************************/

__INLINE_FUNC(DSPLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore)
template <typename dataType>
static inline void DSPLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore(int32_t numRows, uint8_t cBackDst)
{

   _nassert(numRows > 0);

   int32_t r;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   /* Load A and B vectors numRows times into respective MMA panels */
   for (r = 0; r < numRows; r++) {

      /* Fetch A vector from SE0 */
      vec vA = c7x::strm_eng<0, vec>::get_adv();

      /* Fetch B vector from SE1 */
      vec vB = c7x::strm_eng<1, vec>::get_adv();

      /* Load vA into A vector panel and vB into B matrix panel */
      HWALDAB(c7x::reinterpret<__mma_vec>(vA), c7x::reinterpret<__mma_vec>(vB));

      /* Compute C vector into C panel and move C back panel to transfer buffer */
      /* Perform OP with values from A panel, not ARF */
      HWAOPXFER(__MMA_A_LDA);

      /* Move Xfer buffer to VB0 */
      __mma_vec VB0 = __HWARCV((uint32_t) 0);

      /* Advance HWA State Machine */
      HWAADV();

      /* Store VB0 to memory via SA0 */
      __vpred pred = c7x::strm_agen<0, vec>::get_vpred();
      vec*    VB1  = c7x::strm_agen<0, vec>::get_adv(cBackDst);

      __vstore_pred(pred, VB1, c7x::reinterpret<vec>(VB0));
   }

   return;
}

#endif // COMMON_DSPLIB_UTIL_SE0ALOADSE1BLOADCOMPUTECSA0CSTORE_H_
