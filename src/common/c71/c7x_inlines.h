// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_DSPLIB_C7X_INLINES_H_
#define COMMON_DSPLIB_C7X_INLINES_H_ 1

/******************************************************************************/

/******************************************************************************/
/**
 * \ingroup c7x_inlines
 * \brief   c7x specific inlines
 *
 * \details
 * \param
 * \return  void
 */
/******************************************************************************/

#ifdef __cplusplus

__INLINE_FUNC(c7x_permute_even_even_xxx)
template <typename dataType>
static inline c7x::uchar_vec c7x_permute_even_even_xxx(c7x::uchar_vec permCtrl, c7x::uchar_vec in1, c7x::uchar_vec in0)
{
   c7x::uchar_vec retVal;

   if (sizeof(dataType) == sizeof(int32_t)) {
      retVal = __permute_even_even_int(permCtrl, in1, in0);
   }
   else if (sizeof(dataType) == sizeof(int16_t)) {
      retVal = __permute_even_even_short(permCtrl, in1, in0);
   }
   else {
      retVal = (c7x::uchar_vec)(0);
   }
   return retVal;
}

__INLINE_FUNC(c7x_permute_odd_odd_xxx)
template <typename dataType>
static inline c7x::uchar_vec c7x_permute_odd_odd_xxx(c7x::uchar_vec permCtrl, c7x::uchar_vec in1, c7x::uchar_vec in0)
{
   c7x::uchar_vec retVal;

   if (sizeof(dataType) == sizeof(int32_t)) {
      retVal = __permute_odd_odd_int(permCtrl, in1, in0);
   }
   else if (sizeof(dataType) == sizeof(int16_t)) {
      retVal = __permute_odd_odd_short(permCtrl, in1, in0);
   }
   else {
      retVal = (c7x::uchar_vec)(0);
   }
   return retVal;
}
#endif
#endif /* COMMON_DSPLIB_C7X_INLINES_H_ */
