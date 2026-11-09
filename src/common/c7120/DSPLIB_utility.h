// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef C7120_DSPLIB_UTILITY_H_
#define C7120_DSPLIB_UTILITY_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/
#include "../DSPLIB_types.h"

#ifdef __cplusplus

/*******************************************************************************
 *
 * Inline scale shift saturate operation
 *
 ******************************************************************************/

static inline void DSPLIB_UTIL_scaleShiftSaturate(int32_t x, uint8_t scale, uint8_t shift, int8_t *retVal)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7F) {
     *retVal = 0x7F;
  } else if (xVal < -0x80) {
     *retVal = -0x80;
  } else {
     *retVal = (int8_t)xVal;
  }
}

static inline void DSPLIB_UTIL_scaleShiftSaturate(int32_t x, uint8_t scale, uint8_t shift, uint8_t *retVal)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0xFF) {
     *retVal = 0xFF;
  } else if (xVal < 0) {
     *retVal = 0x0;
  } else {
     *retVal = (uint8_t)xVal;
  }
}

static inline void DSPLIB_UTIL_scaleShiftSaturate(uint32_t x, uint8_t scale, uint8_t shift, uint8_t *retVal)
{
   uint64_t xVal = x;
   uint64_t scaledValue = xVal*scale;

   uint32_t shift32_t = (uint32_t) shift;

   if (shift32_t > 0) {
      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
      xVal = (xVal >> 1);
   } else {
      xVal = scaledValue;
   }

   if (xVal > 0xFF) {
      *retVal = 0xFF;
   } else {
      *retVal = (uint8_t)xVal;
   }
}

static inline void DSPLIB_UTIL_scaleShiftSaturate(int64_t x, uint8_t scale, uint8_t shift, int16_t *retVal)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7FFF) {
     *retVal = 0x7FFF;
  } else if (xVal < -0x8000) {
     *retVal = -0x8000;
  } else {
     *retVal = (int16_t)xVal;
  }
}

static inline void DSPLIB_UTIL_scaleShiftSaturate(uint64_t x, uint8_t scale, uint8_t shift, uint16_t *retVal)
{
   uint64_t xVal = x;
   uint64_t scaledValue = xVal*scale;

   uint32_t shift32_t = (uint32_t) shift;

   if (shift32_t > 0) {
      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
      xVal = (xVal >> 1);
   } else {
      xVal = scaledValue;
   }

   if (xVal > 0xFFFF) {
      *retVal = 0xFFFF;
   } else {
      *retVal = (uint16_t)xVal;
   }
}

//int8_t scale
static inline void DSPLIB_UTIL_scaleShiftSaturate(int32_t x, int8_t scale, uint8_t shift, int8_t *retVal)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7F) {
     *retVal = 0x7F;
  } else if (xVal < -0x80) {
     *retVal = -0x80;
  } else {
     *retVal = (int8_t)xVal;
  }
}

static inline void DSPLIB_UTIL_scaleShiftSaturate(int32_t x, int8_t scale, uint8_t shift, uint8_t *retVal)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0xFF) {
     *retVal = 0xFF;
  } else if (xVal < 0x0) {
     *retVal = 0x0;
  } else {
     *retVal = (int8_t)xVal;
  }
}

static inline void DSPLIB_UTIL_scaleShiftSaturate(uint32_t x, int8_t scale, uint8_t shift, uint8_t *retVal)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

//   uint32_t shift32_t = (uint32_t) shift;
//
//   if (shift32_t > 0) {
//      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
//      xVal = (xVal >> 1);
//   } else {
//      xVal = scaledValue;
//   }
   int32_t tmp;
   if(shift > 0) {
      tmp = __shift_right(scaledValue, (shift - 1)) + 1;
      tmp = __shift_right(tmp, 1);
   } else {
      tmp = scaledValue;
   }

   if (tmp > 0xFF) {
      *retVal = 0xFF;
   } else {
      *retVal = (uint8_t)tmp;
   }
}

static inline void DSPLIB_UTIL_scaleShiftSaturate(int64_t x, int8_t scale, uint8_t shift, int16_t *retVal)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7FFF) {
     *retVal = 0x7FFF;
  } else if (xVal < -0x8000) {
     *retVal = -0x8000;
  } else {
     *retVal = (int16_t)xVal;
  }
}

static inline void DSPLIB_UTIL_scaleShiftSaturate(int64_t x, int8_t scale, uint8_t shift, int32_t *retVal)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7FFFFFFF) {
     *retVal = 0x7FFFFFFF;
  } else if (xVal < -0x80000000LL) {
     *retVal = (int32_t)(-0x80000000LL);
  } else {
     *retVal = (int32_t)xVal;
  }
}

static inline void DSPLIB_UTIL_scaleShiftSaturate(uint64_t x, int8_t scale, uint8_t shift, uint16_t *retVal)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

//   uint32_t shift32_t = (uint32_t) shift;
//
//   if (shift32_t > 0) {
//      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
//      xVal = (xVal >> 1);
//   } else {
//      xVal = scaledValue;
//   }
   int32_t tmp;
   if(shift > 0) {
      tmp = __shift_right(scaledValue, (shift - 1)) + 1;
      tmp = __shift_right(tmp, 1);
   } else {
      tmp = scaledValue;
   }

   if (tmp > 0xFFFF) {
      *retVal = 0xFFFF;
   } else {
      *retVal = (uint16_t)tmp;
   }
}

/*******************************************************************************
 *
 * Inline shift and round operation
 *
 ******************************************************************************/

template <typename dataType, typename dataTypeS, typename returnType>
inline returnType DSPLIB_UTIL_scaleShiftAndRound(dataType inVal, dataTypeS scale, uint8_t shift){
   returnType result;

   //result = DSPLIB_UTIL_scaleShiftSaturate(inVal, scale, shift);
   DSPLIB_UTIL_scaleShiftSaturate(inVal, scale, shift, &result);

   return result;
}

template int8_t  DSPLIB_UTIL_scaleShiftAndRound<int32_t, uint8_t, int8_t>  (int32_t inVal, uint8_t scale, uint8_t shift);
template int16_t DSPLIB_UTIL_scaleShiftAndRound<int64_t, uint8_t, int16_t> (int64_t inVal, uint8_t scale, uint8_t shift);
template int8_t  DSPLIB_UTIL_scaleShiftAndRound<int32_t, int8_t, int8_t>  (int32_t inVal, int8_t scale, uint8_t shift);
template int16_t DSPLIB_UTIL_scaleShiftAndRound<int64_t, int8_t, int16_t> (int64_t inVal, int8_t scale, uint8_t shift);

#endif //#ifdef __cplusplus

#endif
