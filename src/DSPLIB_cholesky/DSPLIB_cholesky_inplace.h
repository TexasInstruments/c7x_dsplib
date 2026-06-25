// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_CHOLESKY_INPLACE_IXX_IXX_OXX_H_
#define DSPLIB_CHOLESKY_INPLACE_IXX_IXX_OXX_H_

// #include "../../common/DSPLIB_types.h"
#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_cholesky_inplace DSPLIB_cholesky_inplace
 * @brief    Kernel for computing Inplace Cholesky Decomposition
 * @details
 *          - This kernel decomposes the input square matrix \f$A\f$ into a
 *            upper triangular matrix \f$U\f$ where:
 *          \f{eqnarray*}{
 *               A = U^{T} \times U
 *          \f}
 *          - Processing is done inplace of the \f$A\f$ matrix i.e. the upper triangular entries of
 *            \f$A\f$ matrix represent output \f$U\f$ matrix.
 *          - This kernel provide fuctionality for checking of symmetric positive definiteness of
 *            input matrix \f$A\f$. Check @ref DSPLIB_cholesky_inplace_InitArgs.enableTest
 *          - Input matrix \f$A\f$ shape (M, M)
 *
 * @ingroup  DSPLIB
 */
/**@{*/

/*!
 * @brief Structure containing the parameters to initialize the kernel
 */
typedef struct {
   /*! @brief Variant of the function refer to @ref DSPLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /*! @brief order of input data*/
   int32_t order;
   /*! @brief stride of input/output data in ci*/
   int32_t stride;
   /*! @brief flag to check for positive definiteness of input matrix*/
   int32_t enableTest;
} DSPLIB_cholesky_inplace_InitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_cholesky_inplace_getHandleSize(DSPLIB_cholesky_inplace_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_cholesky_inplace_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the MMA
 *               accelerator and the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle          :  Active handle to the kernel
 *  @param [in]  bufParamsA      :  Pointer to the structure containing dimensional
 *                                  information of input buffer
 *  @param [in]  bufParamsMul    :  Pointer to the structure containing dimensional information
 *                                  of scratch buffer
 *  @param [in]  pKerInitArgs    :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_cholesky_inplace_init(DSPLIB_kernelHandle                     handle,
                                           DSPLIB_bufParams2D_t                   *bufParamsA,
                                           DSPLIB_bufParams1D_t                   *bufParamsMul,
                                           const DSPLIB_cholesky_inplace_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_cholesky_inplace_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_cholesky_inplace_init, and this function
 *               must be called before the
 *               @ref DSPLIB_cholesky_inplace_init is called.
 *
 *  @param [in]  handle          :  Active handle to the kernel
 *  @param [in]  bufParamsA      :  Pointer to the structure containing dimensional
 *                                  information of input buffer
 *  @param [in]  bufParamsMul    :  Pointer to the structure containing dimensional information
 *                                  of scratch buffer
 *  @param [in]  pKerInitArgs    :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_cholesky_inplace_init_checkParams(DSPLIB_kernelHandle                     handle,
                                                       DSPLIB_bufParams2D_t                   *bufParamsA,
                                                       DSPLIB_bufParams1D_t                   *bufParamsMul,
                                                       const DSPLIB_cholesky_inplace_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_cholesky_inplace_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_cholesky_inplace_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_cholesky_inplace_exec is called.
 *
 *  @param [in]  handle : Active handle to the kernel
 *  @param [in]  pA     : Pointer to buffer holding the input data A
 *  @param [in]  pMul   : Pointer to scratch buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_cholesky_inplace_exec_checkParams(DSPLIB_kernelHandle handle,
                                                       const void *restrict pA,
                                                       const void *restrict pMul);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *
 *  @param [in]  handle : Active handle to the kernel
 *  @param [in]  pA     : Pointer to buffer holding the input data A[order][order]
 *  @param [in]  pMul   : Pointer to scratch buffer
 *
 * @par Memory Requirements
 * | Buffer | dimY | dimX | Comments |
 * | :--    | :--: | :--: | :-----|
 * | pInA   | M    | M    | Input Square / Output Upper Triangular Matrix |
 * | pMul   | 1    | M    | Scratch Buffer to store multipliers |
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Assumptions:
 *    - In-place processing is done on the matrix A.
 *    - Only upper triangular part of matrix A is to be considered as result
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - The input and output data buffers are expected to be in L2 memory
 *    - The buffer pointers are assumed to be 64-byte aligned
 *    - Ensure that the starting point of adjacent rows in a 2-D matrix are allocated from
 *      different banks in L2 memory with the help of memory padding.
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref DSPLIB_cholesky_inplace_init and
 *               @ref DSPLIB_cholesky_inplace_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */
DSPLIB_STATUS DSPLIB_cholesky_inplace_exec(DSPLIB_kernelHandle handle, void *restrict pA, void *restrict pMul);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_CHOLESKY_INPLACE_IXX_IXX_OXX_H_ */
