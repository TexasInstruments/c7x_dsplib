// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_LUD_DECOMP_IXX_IXX_OXX_H_
#define DSPLIB_LUD_DECOMP_IXX_IXX_OXX_H_

#include "../common/DSPLIB_test.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t DSPLIB_lud_decomposition_check(int           testID,
                                       int           order,
                                       void*         pA,
                                       void*         pL,
                                       void*         pU,
                                       void*         pP,
                                       void*         pScratchU,
                                       const int32_t strideOrder,
                                       const int32_t strideP,
                                       int           data_type,
                                       char*         print_str);
#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_LUD_DECOMP_IXX_IXX_OXX_H_ */
