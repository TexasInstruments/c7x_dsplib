// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_SVD_CHECK_IXX_IXX_OXX_H_
#define DSPLIB_SVD_CHECK_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t cholesky_check_solution(int32_t  testId,
                                uint32_t dataType,
                                int32_t  order,
                                void*    A,
                                void*    X,
                                void*    B,
                                char*    label,
                                int32_t  colAStride);

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_SVD_CHECK_IXX_IXX_OXX_H_ */
