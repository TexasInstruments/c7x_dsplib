// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../DSPLIB_types.h"
#include "DSPLIB_debug.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup
 * \brief
 * \details
 * \return  void
 */
/******************************************************************************/

void DSPLIB_debugPrintBufParams1D(const DSPLIB_bufParams1D_t *params)
{
   DSPLIB_PRINTF("   dim_x        = %d\n", params->dim_x);
   DSPLIB_PRINTF("   data_type    = %d\n", params->data_type);

   return;
}

void DSPLIB_debugPrintBufParams(const DSPLIB_bufParams2D_t *params)
{
   DSPLIB_PRINTF("   dim_x        = %d\n", params->dim_x);
   DSPLIB_PRINTF("   dim_y        = %d\n", params->dim_y);
   DSPLIB_PRINTF("   data_type    = %d\n", params->data_type);
   DSPLIB_PRINTF("   stride_y     = %d\n", params->stride_y);

   return;
}

void DSPLIB_debugPrintBufParams3D(const DSPLIB_bufParams3D_t *params)
{
   DSPLIB_PRINTF("   dim_x        = %d\n", params->dim_x);
   DSPLIB_PRINTF("   dim_y        = %d\n", params->dim_y);
   DSPLIB_PRINTF("   dim_z        = %d\n", params->dim_z);
   DSPLIB_PRINTF("   data_type    = %d\n", params->data_type);
   DSPLIB_PRINTF("   stride_y     = %d\n", params->stride_y);
   DSPLIB_PRINTF("   stride_z     = %d\n", params->stride_z);

   return;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
