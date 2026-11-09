// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0


/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "DSPLIB_complexMatrixMultiply_priv.h"
#include "DSPLIB_types.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DSPLIB_complexMatrixMultiply_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the DSPLIB_complexMatrixMultiply kernel
int32_t DSPLIB_complexMatrixMultiply_getHandleSize(DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs)
{
    int32_t privBufSize = sizeof(DSPLIB_complexMatrixMultiply_PrivArgs);
    return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DSPLIB_complexMatrixMultiply_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init_checkParams(
    DSPLIB_kernelHandle handle,
    const DSPLIB_bufParams3D_t *bufParamsInA,
    const DSPLIB_bufParams3D_t *bufParamsInB,
    const DSPLIB_bufParams3D_t *bufParamsOut,
    const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs)
{
    DSPLIB_STATUS status = DSPLIB_SUCCESS;

    if ((handle == NULL) || (bufParamsInA == NULL) || (bufParamsInB == NULL) || (bufParamsOut == NULL) ||
        (pKerInitArgs == NULL))
    {
        status = DSPLIB_ERR_NULL_POINTER;
    }

    if (status == DSPLIB_SUCCESS)
    {
        uint32_t dTypeA = bufParamsInA->data_type;
        uint32_t widthA = bufParamsInA->dim_x;
        uint32_t heightA = bufParamsInA->dim_y;
        uint32_t strideA = bufParamsInA->stride_y;
        uint32_t batchA = bufParamsInA->dim_z;

        uint32_t dTypeB = bufParamsInB->data_type;
        uint32_t widthB = bufParamsInB->dim_x;
        uint32_t heightB = bufParamsInB->dim_y;
        uint32_t strideB = bufParamsInB->stride_y;
        uint32_t batchB = bufParamsInB->dim_z;

        uint32_t dTypeOut = bufParamsOut->data_type;
        uint32_t widthOut = bufParamsOut->dim_x;
        uint32_t heightOut = bufParamsOut->dim_y;
        uint32_t strideOut = bufParamsOut->stride_y;
        uint32_t batchOut = bufParamsOut->dim_z;

        uint32_t dataSizeA = DSPLIB_sizeof(dTypeA);
        uint32_t dataSizeB = DSPLIB_sizeof(dTypeB);
        uint32_t dataSizeOut = DSPLIB_sizeof(dTypeOut);

        if (!(dTypeA == DSPLIB_INT16) && !(dTypeA == DSPLIB_INT32))
        {
            status = DSPLIB_ERR_INVALID_TYPE;
        }
        // All datatypes must be the same
        else if ((dTypeA != dTypeB) || (dTypeA != dTypeOut))
        {
            status = DSPLIB_ERR_INVALID_TYPE;
        }
        else if ((widthA != heightB) || (widthB != widthOut) || (heightA != heightOut) || (batchA != batchB) || (batchB != batchOut))
        {
            status = DSPLIB_ERR_INVALID_DIMENSION;
        }
        else if ((strideA < widthA * dataSizeA * 2) || (strideB < widthB * dataSizeB * 2) || (strideOut < widthOut * dataSizeOut * 2))
        {
            status = DSPLIB_ERR_NOT_ALIGNED_PTRS_STRIDES;
        }
        else if ((dTypeA != DSPLIB_INT16) && (pKerInitArgs->params.scaleValue != 1))
        {
            status = DSPLIB_ERR_NOT_IMPLEMENTED;
        }
        else if ((pKerInitArgs->params.shift < 0) || (pKerInitArgs->params.shift > 255))
        {
            status = DSPLIB_ERR_INVALID_SHIFT;
        }
        else if ((pKerInitArgs->params.scaleValue < -128) || (pKerInitArgs->params.scaleValue > 127))
        {
            status = DSPLIB_ERR_INVALID_VALUE;
        }
        else
        {
            /* all checks passed */
        }
    }

    return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DSPLIB_complexMatrixMultiply_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the DSPLIB_complexMatrixMultiply kernel
DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_checkParams(
    DSPLIB_kernelHandle handle,
    const void *restrict pInA,
    const void *restrict pInB,
    const void *restrict pOut)
{
    DSPLIB_STATUS status;

    if ((handle == NULL) || (pInA == NULL) || (pInB == NULL) || (pOut == NULL))
    {
        status = DSPLIB_ERR_NULL_POINTER;
    }
    else
    {
        status = DSPLIB_SUCCESS;
    }

    return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DSPLIB_complexMatrixMultiply_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the DSPLIB_complexMatrixMultiply kernel
DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init(DSPLIB_kernelHandle handle,
                                                    const DSPLIB_bufParams3D_t *bufParamsInA,
                                                    const DSPLIB_bufParams3D_t *bufParamsInB,
                                                    const DSPLIB_bufParams3D_t *bufParamsOut,
                                                    const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs)
{
    DSPLIB_STATUS status = DSPLIB_SUCCESS;
    DSPLIB_complexMatrixMultiply_PrivArgs *pKerPrivArgs = (DSPLIB_complexMatrixMultiply_PrivArgs *)handle;

    // copy pKerinitArgs into pKerPrivargs
    pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

    pKerPrivArgs->strideA = bufParamsInA->stride_y / DSPLIB_sizeof(bufParamsInA->data_type);
    pKerPrivArgs->strideB = bufParamsInB->stride_y / DSPLIB_sizeof(bufParamsInB->data_type);
    pKerPrivArgs->strideC = bufParamsOut->stride_y / DSPLIB_sizeof(bufParamsOut->data_type);
    pKerPrivArgs->nBatch = bufParamsInA->dim_z;

    pKerPrivArgs->batchStrideA = bufParamsInA->stride_z / DSPLIB_sizeof(bufParamsInA->data_type);
    pKerPrivArgs->batchStrideB = bufParamsInB->stride_z / DSPLIB_sizeof(bufParamsInB->data_type);
    pKerPrivArgs->batchStrideC = bufParamsOut->stride_z / DSPLIB_sizeof(bufParamsOut->data_type);

    // obtain buffer datatypes
    uint32_t dType = bufParamsInA->data_type;

    // determine natural C  vs optimized function call
    if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC)
    {
        // set function pointer for natural C function with appropriate template parameters based on datatypes
        if (dType == DSPLIB_INT16)
        {
            pKerPrivArgs->execute = DSPLIB_complexMatrixMultiply_exec_cn<int16_t>;
            status = DSPLIB_complexMatrixMultiply_init_cn<int16_t>(handle, bufParamsInA, bufParamsInB, bufParamsOut, pKerInitArgs);
        }
        else if (dType == DSPLIB_INT32)
        {
            pKerPrivArgs->execute = DSPLIB_complexMatrixMultiply_exec_cn<int32_t>;
            status = DSPLIB_complexMatrixMultiply_init_cn<int32_t>(handle, bufParamsInA, bufParamsInB, bufParamsOut, pKerInitArgs);
        }
        else
        {
            status = DSPLIB_ERR_INVALID_TYPE;
        }
    }
    else
    { // Optimized function
        // set function pointer for natural C function with appropriate template parameters based on datatypes
        if (dType == DSPLIB_INT16)
        {
            pKerPrivArgs->execute = DSPLIB_complexMatrixMultiply_exec_ci<int16_t, DSPLIB_MMA_SIZE_16_BIT>;
            status = DSPLIB_complexMatrixMultiply_init_ci<int16_t>(handle, bufParamsInA, bufParamsInB, bufParamsOut, pKerInitArgs);
        }
        else if (dType == DSPLIB_INT32)
        {
            pKerPrivArgs->execute = DSPLIB_complexMatrixMultiply_exec_ci<int32_t, DSPLIB_MMA_SIZE_32_BIT>;
            status = DSPLIB_complexMatrixMultiply_init_ci<int32_t>(handle, bufParamsInA, bufParamsInB, bufParamsOut, pKerInitArgs);
        }
        else
        {
            status = DSPLIB_ERR_INVALID_TYPE;
        }
    }

    return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DSPLIB_complexMatrixMultiply_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/
// this method is the user-level execution function for the DSPLIB_complexMatrixMultiply kernel
DSPLIB_STATUS
DSPLIB_complexMatrixMultiply_exec(DSPLIB_kernelHandle handle,
                                    const void *restrict pInA,
                                    const void *restrict pInB,
                                    void *restrict pOut)
{
    DSPLIB_STATUS status;
    DSPLIB_complexMatrixMultiply_PrivArgs *pKerPrivArgs = (DSPLIB_complexMatrixMultiply_PrivArgs *)handle;

    status = pKerPrivArgs->execute(handle, pInA, pInB, pOut);

    return status;
}
