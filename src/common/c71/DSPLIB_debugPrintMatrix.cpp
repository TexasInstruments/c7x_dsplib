// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../DSPLIB_bufParams.h"
#include "../DSPLIB_types.h"
#include "../DSPLIB_utility.h"

//----------------------------------------------------------------------------------

template <typename dataType> void DSPLIB_debugPrintMatrix_helper(dataType *matrix, const DSPLIB_bufParams2D_t *params)
{
   uint32_t  x, y;
   dataType *xPtr;
   dataType *yPtr = matrix;

   // stride_y is stored in bytes, but easier to use in elements
   uint32_t stride_y_elements = params->stride_y / DSPLIB_sizeof(params->data_type);

   for (y = 0; y < params->dim_y; y++) {
      xPtr = yPtr;
      DSPLIB_PRINTF("%p |", xPtr);
      for (x = 0; x < params->dim_x; x++) {
         DSPLIB_PRINTF("%3d ", static_cast<int>(*xPtr));
         xPtr++;
      }
      DSPLIB_PRINTF("%s", "|\n");
      yPtr += stride_y_elements;
   }

   return;
}

template void DSPLIB_debugPrintMatrix_helper<int8_t>(int8_t *matrix, const DSPLIB_bufParams2D_t *params);
template void DSPLIB_debugPrintMatrix_helper<int16_t>(int16_t *matrix, const DSPLIB_bufParams2D_t *params);
template void DSPLIB_debugPrintMatrix_helper<int32_t>(int32_t *matrix, const DSPLIB_bufParams2D_t *params);


template <typename dataType> void DSPLIB_debugPrintMatrix_helperU(dataType *matrix, const DSPLIB_bufParams2D_t *params)
{
   uint32_t  x, y;
   dataType *xPtr;
   dataType *yPtr = matrix;

   // stride_y is stored in bytes, but easier to use in elements
   uint32_t stride_y_elements = params->stride_y / DSPLIB_sizeof(params->data_type);

   for (y = 0; y < params->dim_y; y++) {
      xPtr = yPtr;
      DSPLIB_PRINTF("%p |", xPtr);
      for (x = 0; x < params->dim_x; x++) {
         DSPLIB_PRINTF("%3u ", static_cast<unsigned int>(*xPtr));
         xPtr++;
      }
      DSPLIB_PRINTF("%s", "|\n");
      yPtr += stride_y_elements;
   }

   return;
}

template void DSPLIB_debugPrintMatrix_helperU<uint8_t>(uint8_t *matrix, const DSPLIB_bufParams2D_t *params);
template void DSPLIB_debugPrintMatrix_helperU<uint16_t>(uint16_t *matrix, const DSPLIB_bufParams2D_t *params);

/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup
 * \brief
 * \details
 * \return  void
 */
/******************************************************************************/

// want this function to have C-linkage in library...
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void DSPLIB_debugPrintMatrix(void *matrix, const DSPLIB_bufParams2D_t *params)
{
   switch (params->data_type) {
   case DSPLIB_INT32:
      DSPLIB_debugPrintMatrix_helper<int32_t>((int32_t *) matrix, params);
      break;
   case DSPLIB_INT16:
      DSPLIB_debugPrintMatrix_helper<int16_t>((int16_t *) matrix, params);
      break;
   case DSPLIB_UINT16:
      DSPLIB_debugPrintMatrix_helperU<uint16_t>((uint16_t *) matrix, params);
      break;
   case DSPLIB_INT8:
      DSPLIB_debugPrintMatrix_helper<int8_t>((int8_t *) matrix, params);
      break;
   case DSPLIB_UINT8:
      DSPLIB_debugPrintMatrix_helperU<uint8_t>((uint8_t *) matrix, params);
      break;
   default:
      DSPLIB_PRINTF("\nERROR: Unrecognized data type in %s.\n", __FUNCTION__);
   }

   return;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
