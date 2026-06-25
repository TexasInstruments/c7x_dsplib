// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_LUD_COMMON_IXX_IXX_OXX_H_
#define DSPLIB_LUD_COMMON_IXX_IXX_OXX_H_

#include "../common/DSPLIB_utility.h"

/* ********************** DECLARATION OF COMMON INIT FUNCTIONS ************************* */

template <typename dataType>
void DSPLIB_lud_identity_matrix_generate_init_ci(uint8_t *pBlock, int32_t nRows, int32_t stride);

template <typename dataType> void DSPLIB_lud_blk_move_init_ci(uint8_t *pBlock, int32_t nRows, int32_t stride);

/* **************************** DECLARATION OF COMMON OPT FUNCTIONS ************************* */
template <typename dataType>
void DSPLIB_lud_identity_matrix_generate_exec_ci(dataType *pMat, int32_t nRows, int32_t colStride, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_lud_blk_move_exec_ci(dataType *pOut,
                                 dataType *pIn,
                                 int32_t   Nrows,
                                 int32_t   Ncols,
                                 int32_t   colOutStride,
                                 int32_t   colInStride,
                                 uint8_t  *pBlock);

#endif /* DSPLIB_LUD_COMMON_IXX_IXX_OXX_H_ */
