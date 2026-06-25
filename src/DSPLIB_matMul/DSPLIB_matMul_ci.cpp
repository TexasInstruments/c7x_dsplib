// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_matMul_priv.h"
#include "DSPLIB_types.h"
#include "TI_platforms.h"
#include <cstdint>
#include <float.h>

#if !defined(__C7504__) && !defined(__C7524__)
#define DSPLIB_MATMUL_UNROLL (1024)
#else
#define DSPLIB_MATMUL_UNROLL (256)
#endif

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_init_ci(DSPLIB_kernelHandle           handle,
                                    const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                    const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                    const DSPLIB_bufParams2D_t   *bufParamsOut,
                                    const DSPLIB_matMul_InitArgs *pKerInitArgs)
{

   DSPLIB_STATUS           status       = DSPLIB_SUCCESS;
   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   int32_t N = pKerPrivArgs->N;

   if ((N * sizeof(dataType)) % DSPLIB_MATMUL_UNROLL == 0) {
      status = DSPLIB_matMul_N_unroll_init_ci<dataType>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
   }
   else {
      status = DSPLIB_matMul_generic_init_ci<dataType>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_matMul_init_ci<float>(DSPLIB_kernelHandle           handle,
                                                    const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                                    const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                                    const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                    const DSPLIB_matMul_InitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_matMul_init_ci<double>(DSPLIB_kernelHandle           handle,
                                                     const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                                     const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                                     const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                     const DSPLIB_matMul_InitArgs *pKerInitArgs);
template <typename dataType>
DSPLIB_STATUS
DSPLIB_matMul_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{

   DSPLIB_STATUS           status       = DSPLIB_SUCCESS;
   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   int32_t N = pKerPrivArgs->N;

   if ((N * sizeof(dataType)) % DSPLIB_MATMUL_UNROLL == 0) {

      status = DSPLIB_matMul_N_unroll_exec_ci<dataType>(handle, pIn0, pIn1, pOut);
   }
   else {

      status = DSPLIB_matMul_generic_exec_ci<dataType>(handle, pIn0, pIn1, pOut);
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_matMul_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS DSPLIB_matMul_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn0,
                                                     void *restrict pIn1,
                                                     void *restrict pOut);
