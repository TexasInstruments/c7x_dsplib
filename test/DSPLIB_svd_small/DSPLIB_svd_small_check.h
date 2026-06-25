// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_SVD_CHECK_IXX_IXX_OXX_H_
#define DSPLIB_SVD_CHECK_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t svd_check_decomposition(uint32_t dataType,
                                int32_t  Nrows,
                                int32_t  Ncols,
                                void*    A,
                                void*    U,
                                void*    diag,
                                void*    V,
                                void*    U1,
                                char*    label,
                                int32_t  colAStride,
                                int32_t  colUStride,
                                int32_t  colVStride,
                                uint32_t enableReducedForm);

int32_t svd_check_transformation(uint32_t dataType,
                                 int32_t  Nrows,
                                 int32_t  Ncols,
                                 void*    U,
                                 void*    V,
                                 char*    label,
                                 int32_t  colUStride,
                                 int32_t  colVStride,
                                 uint32_t enableReducedForm);
#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_SVD_CHECK_IXX_IXX_OXX_H_ */
