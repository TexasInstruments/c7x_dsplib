// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0


/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "DSPLIB_complexMatrixMultiply_priv.h"
#include "../common/c7120/DSPLIB_utility.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DSPLIB_complexMatrixMultiply_exec_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dType>
DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init_cn(
    DSPLIB_kernelHandle handle,
    const DSPLIB_bufParams3D_t *bufParamsInA,
    const DSPLIB_bufParams3D_t *bufParamsInB,
    const DSPLIB_bufParams3D_t *bufParamsOut,
    const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs)
{
    DSPLIB_STATUS status = DSPLIB_SUCCESS;

    // store parameters
    DSPLIB_complexMatrixMultiply_PrivArgs *pKerPrivArgs =
        (DSPLIB_complexMatrixMultiply_PrivArgs *)handle;

    pKerPrivArgs->M = bufParamsInA->dim_y;
    pKerPrivArgs->K = bufParamsInA->dim_x;
    pKerPrivArgs->N = bufParamsInB->dim_x;

    return status;
}

template DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init_cn<int16_t>(DSPLIB_kernelHandle handle,
                                                                         const DSPLIB_bufParams3D_t *bufParamsInA,
                                                                         const DSPLIB_bufParams3D_t *bufParamsInB,
                                                                         const DSPLIB_bufParams3D_t *bufParamsOut,
                                                                         const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init_cn<int32_t>(DSPLIB_kernelHandle handle,
                                                                         const DSPLIB_bufParams3D_t *bufParamsInA,
                                                                         const DSPLIB_bufParams3D_t *bufParamsInB,
                                                                         const DSPLIB_bufParams3D_t *bufParamsOut,
                                                                         const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs);


// this method computes interference stats via natural C code
template <typename dType>
DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_cn(DSPLIB_kernelHandle handle,
                                                       const void *restrict pInA,
                                                       const void *restrict pInB,
                                                       void *restrict pOut)
{
    DSPLIB_STATUS status = DSPLIB_SUCCESS; // assign status to success by default
    // typecast handle (void) to struct pointer type associated to kernel
    DSPLIB_complexMatrixMultiply_PrivArgs *pKerPrivArgs = (DSPLIB_complexMatrixMultiply_PrivArgs *)handle;

    int32_t M = pKerPrivArgs->M;
    int32_t K = pKerPrivArgs->K;
    int32_t N = pKerPrivArgs->N;
    int32_t nBatch = pKerPrivArgs->nBatch;
    int32_t m, k, n, b;

    // obtain input parameters
    size_t strideA = pKerPrivArgs->strideA;
    size_t strideB = pKerPrivArgs->strideB;
    size_t strideC = pKerPrivArgs->strideC;
    size_t bStrideA = pKerPrivArgs->batchStrideA;
    size_t bStrideB = pKerPrivArgs->batchStrideB;
    size_t bStrideC = pKerPrivArgs->batchStrideC;

    typedef int64_t accType;

    dType *restrict pA = (dType *)pInA;
    dType *restrict pB = (dType *)pInB;
    dType *restrict pC = (dType *)pOut;

    const int16_t shift = pKerPrivArgs->pKerInitArgs.params.shift;
    const int16_t scaleValue = pKerPrivArgs->pKerInitArgs.params.scaleValue;

    for (b = 0; b < nBatch; b++)
    {
        for (m = 0; m < M; m++)
        {
            for (n = 0; n < N; n++)
            {
                accType sRe = 0;
                accType sIm = 0;
                // printf("%d, %d : ", m, n);
                for (k = 0; k < K; k++)
                {
                    dType aRe = pA[m * strideA + 2 * k];
                    dType aIm = pA[m * strideA + 2 * k + 1];

                    dType bRe = pB[k * strideB + 2 * n];
                    dType bIm = pB[k * strideB + 2 * n + 1];

                    accType c_re = ((accType)aRe * (accType)bRe) - ((accType)aIm * (accType)bIm);
                    accType c_im = ((accType)aRe * (accType)bIm) + ((accType)aIm * (accType)bRe);

                    sRe += c_re;
                    sIm += c_im;
                }

                dType re = DSPLIB_UTIL_scaleShiftAndRound<accType, int8_t, dType>(sRe, (int8_t)scaleValue, (uint8_t)shift);
                dType im = DSPLIB_UTIL_scaleShiftAndRound<accType, int8_t, dType>(sIm, (int8_t)scaleValue, (uint8_t)shift);

                pC[m * strideC + 2 * n] = re;
                pC[m * strideC + 2 * n + 1] = im;
            }
        }
        pA += bStrideA;
        pB += bStrideB;
        pC += bStrideC;
    }

    return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiation for the different data type versions                                                        */
/*                                                                                                                    */
/**********************************************************************************************************************/

template DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_cn<int16_t>(DSPLIB_kernelHandle handle,
                                                                         const void *restrict pInA,
                                                                         const void *restrict pInB,
                                                                         void *restrict pOut);

template DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_cn<int32_t>(DSPLIB_kernelHandle handle,
                                                                         const void *restrict pInA,
                                                                         const void *restrict pInB,
                                                                         void *restrict pOut);
