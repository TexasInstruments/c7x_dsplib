// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0


#ifndef DSPLIB_COMPLEXMATRIXMULTIPLY_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_COMPLEXMATRIXMULTIPLY_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_complexMatrixMultiply.h"
#include "DSPLIB_types.h"
#include "DSPLIB_intr.h"

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_complexMatrixMultiply_PrivArgs structure.
 *
 */

#define DSPLIB_COMPLEXMATRIXMULTIPLY_IXX_IXX_OXX_PBLOCK_SIZE \
    (3 * SE_PARAM_SIZE + sizeof(__HWA_CONFIG_REG_v1) + sizeof(__HWA_OFFSET_REG))

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_complexMatrixMultiply_exec_ci
 *         and @ref DSPLIB_complexMatrixMultiply_exec_cn.
 */

typedef DSPLIB_STATUS (*pFxnDSPLIB_complexMatrixMultiply_exec)(DSPLIB_kernelHandle handle,
                                                                   const void *restrict pInA,
                                                                   const void *restrict pInB,
                                                                   void *restrict pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_complexMatrixMultiply_init.
 *
 * @details This function determines the configuration for the streaming engine
 *           resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_complexMatrixMultiply_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_complexMatrixMultiply_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_complexMatrixMultiply_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  bufParamsInA : Pointer to the structure containing dimensional
 *                              information of input data 1
 *  @param [in]  bufParamsInB : Pointer to the structure containing dimensional
 *                              information of input data 2
 *  @param [in]  bufParamsOut : Pointer to the structure containing dimensional information
 *                              of output
 *  @param [in]  pKerInitArgs : Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */

template <typename dType>
extern DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init_ci(
    DSPLIB_kernelHandle handle,
    const DSPLIB_bufParams3D_t *bufParamsInA,
    const DSPLIB_bufParams3D_t *bufParamsInB,
    const DSPLIB_bufParams3D_t *bufParamsOut,
    const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs);

template <typename dType>
extern DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init_cn(
    DSPLIB_kernelHandle handle,
    const DSPLIB_bufParams3D_t *bufParamsInA,
    const DSPLIB_bufParams3D_t *bufParamsInB,
    const DSPLIB_bufParams3D_t *bufParamsOut,
    const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_complexMatrixMultiply_exec.
 *
 *  @param [in]  handle    : Active handle to the kernel
 *  @param [in]  pInA      : Pointer to buffer holding the input data 1
 *  @param [in]  pInB      : Pointer to buffer holding the input data 2
 *  @param [out] pOut      : Pointer to buffer holding magnitude output
 *  @param [in]  params    : Pointer to buffer holding scaling parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 */

template <typename dType, uint32_t MMA_SIZE>
DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_ci(
    DSPLIB_kernelHandle handle,
    const void *restrict pInA,
    const void *restrict pInB,
    void *restrict pOut);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_complexMatrixMultiply_exec.
 *
 * @details
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pInA       : Pointer to buffer holding the input data
 *  @param [in]  pInB       : Pointer to buffer holding the input data
 *  @param [out] pOut       : Pointer to buffer holding magnitude output
 *  @param [in]  params     : Pointer to buffer holding scaling parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dType>
extern DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_cn(
    DSPLIB_kernelHandle handle,
    const void *restrict pInA,
    const void *restrict pInB,
    void *restrict pOut);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */

typedef struct
{
    /** @brief Initargs of the kernel **/
    DSPLIB_complexMatrixMultiply_InitArgs pKerInitArgs;

    /** @brief Function pointer to point to the right execution variant between
     *         @ref DSPLIB_complexMatrixMultiply_exec_cn and
     *         @ref DSPLIB_complexMatrixMultiply_exec_ci.
     */
    pFxnDSPLIB_complexMatrixMultiply_exec execute;

    /** @brief Number of blocks in the vertical dimension of the first input matrix.  In the case of natural C code, the block size == 1.  **/
    size_t M;
    /** @brief Number of blocks in the horizontal dimension of the first input matrix.  In the case of natural C code, the block size == 1.  **/
    size_t K;
    /** @brief Number of blocks in the horizontal dimension of the second input matrix.  In the case of natural C code, the block size == 1 **/
    size_t N;
    /** @brief Size of the batch in terms of number of channels of matricesd     **/
    size_t nBatch;
    /** @brief MMA size **/
    size_t MMA_SIZE;
    /** @brief Row stride for the A matrix  **/
    size_t strideA;
    /** @brief Row stride for the B matrix  **/
    size_t strideB;
    /** @brief Row stride for the C (out) matrix **/
    size_t strideC;

    /** @brief Batch stride for the A matrix **/
    size_t batchStrideA;
    /** @brief Batch stride for the B matrix **/
    size_t batchStrideB;
    /** @brief Batch stride for the C matrix **/
    size_t batchStrideC;

    /** @brief Array to hold SE/SA params */
    uint8_t bufPblock[DSPLIB_COMPLEXMATRIXMULTIPLY_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_complexMatrixMultiply_PrivArgs;

#endif /* DSPLIB_COMPLEXMATRIXMULTIPLY_IXX_IXX_OXX_PRIV_H_ */
