// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_QRD_COMMON_IXX_IXX_OXX_H_
#define DSPLIB_QRD_COMMON_IXX_IXX_OXX_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_qrd.h"

/*******************************************************************************
 *
 * DECLARATION OF COMMON INIT FUNCTIONS
 *
 ******************************************************************************/
template <typename dataType>
void DSPLIB_qrd_identity_matrix_generate_init_ci(int32_t nRows, int32_t stride, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_qrd_blk_move_init_ci(int32_t nRows, int32_t nCols, int32_t strideR, uint8_t *pBlock);

/*******************************************************************************
 *
 * DECLARATION OF COMMON IMPLEMENTATION FUNCTIONS
 *
 ******************************************************************************/
template <typename dataType>
void DSPLIB_qrd_identity_matrix_generate_exec_ci(dataType *pMat, int32_t nRows, int32_t colStride, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_qrd_blk_move_exec_ci(dataType *pOut,
                                 dataType *pIn,
                                 int32_t   Nrows,
                                 int32_t   Ncols,
                                 int32_t   colOutStride,
                                 int32_t   colInStride,
                                 uint8_t  *pBlock);

#endif /* DSPLIB_QRD_COMMON_IXX_IXX_OXX_H_ */
