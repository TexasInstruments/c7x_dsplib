// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_QRD_CHECK_IXX_IXX_OXX_H_
#define DSPLIB_QRD_CHECK_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t DSPLIB_qrd_check_transformation(int32_t  testID,
                                        uint32_t dataType,
                                        int32_t  NRows,
                                        void    *Q,
                                        char     print_txt[16],
                                        int32_t  strideQ);
int32_t DSPLIB_qrd_check_decomposition(int32_t  testID,
                                       uint32_t dataType,
                                       int32_t  NRows,
                                       int32_t  NCols,
                                       void    *A,
                                       void    *Q,
                                       void    *R,
                                       char     print_txt[16],
                                       int32_t  strideA,
                                       int32_t  strideQ,
                                       int32_t  strideR);
int32_t DSPLIB_qrd_check_invertibility(uint32_t dataType, int32_t NRows, int32_t NCols, void *R, int32_t strideR);
int32_t DSPLIB_qrd_check_solution(int32_t  testID,
                                  uint32_t dataType,
                                  int32_t  NRows,
                                  int32_t  NCols,
                                  void    *A,
                                  void    *x,
                                  void    *b,
                                  char     print_txt[16],
                                  int32_t  strideA);
int32_t DSPLIB_qrd_check_inverse(int32_t  testID,
                                 uint32_t dataType,
                                 int32_t  NRows,
                                 int32_t  NCols,
                                 void    *A,
                                 void    *InvA,
                                 char     print_txt[16],
                                 int32_t  strideA,
                                 int32_t  strideInvA);

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_QRD_CHECK_IXX_IXX_OXX_H_ */
