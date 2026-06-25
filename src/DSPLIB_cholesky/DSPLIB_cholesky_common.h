// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/DSPLIB_inlines.h"
#include "../common/DSPLIB_types.h"

#ifndef DSPLIB_CHOLESKY_COMMON_IXX_IXX_OXX_H_
#define DSPLIB_CHOLESKY_COMMON_IXX_IXX_OXX_H_

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SE2_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SE3_PARAM_OFFSET (SE_SE2_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_SA0_PARAM_OFFSET (SE_SE3_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_SA1_PARAM_OFFSET (SA_SA0_PARAM_OFFSET + SA_PARAM_SIZE)
#define SA_SA2_PARAM_OFFSET (SA_SA1_PARAM_OFFSET + SA_PARAM_SIZE)

/*******************************************************************************
 *
 * COMMON INIT DECLARATION
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_inplace_isPosDefinite_init(int32_t order, int32_t colStride, uint8_t *pBlock);

/*******************************************************************************
 *
 * COMMON IMPLEMENTATION DECLARATION
 *
 ******************************************************************************/

template <typename dataType>
dataType
DSPLIB_cholesky_inplace_isPosDefinite(dataType *A, const int32_t order, const int32_t eleCount, uint8_t *pBlock);

#endif /* DSPLIB_CHOLESKY_COMMON_IXX_IXX_OXX_H_ */
