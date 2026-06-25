// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_SVD_IXX_IXX_OXX_H_
#define DSPLIB_SVD_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#define DSPLIB_CYCLE_CALC 0

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_svd DSPLIB_svd
 * @brief Kernel for calculating Singular Value Decomposition (SVD).
 *
 *
 * @details
 *        - This kernel factorizes a given input matrix \f$A\f$ into two orthogonal matrices
 *          \f$U\f$ and \f$V\f$, and a 1-D array \f$D\f$ consisting of real non-negative
 *          singular values in descending order such that
 *          \f{eqnarray*}{
 *               A = U \times \Sigma \times V^{T}  \qquad where\;&(1)&V^{T}\;is\;transpose\;of\;V.\\
 *                              &(2)&
 *                              \Sigma \;is\;a\;diagonal\;matrix\;with\;main\;diagonal\;D.
 *          \f}
 *        - \f$U\f$ and \f$V\f$ are orthogonal matrices such that
 *          \f{eqnarray*}{
 *               \hspace{-2em} \hspace{-2em} \hspace{-2em} \hspace{-2em} \hspace{-2em}
 *                U \times U^{T} = V \times V^{T} = I \qquad
 *                                                where\;I\;is\;Identity\;matrix\\.
 *          \f}
 *
 *        - The kernel supports REDUCED FORM and FULL FORM generation.
 *          Refer @ref DSPLIB_svdInitArgs.enableReducedForm
 *        - For a given input matrix \f$A\f$ of shape (M, N)
 *          1. If FULL FORM: The output matrix \f$U\f$ is of shape (M, M) and \f$V\f$ is
 *             of shape (N, N)
 *          2. If REDUCED FORM: The output matrix \f$U\f$ is of shape (M, K) and \f$V\f$
 *             is of shape (N, K) where K is minimum of M,N.
 *
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
   /*! @brief Size of input data dimX => Number of columns */
   uint32_t dimX;
   /*! @brief Size of input data dimY => Number of rows */
   uint32_t dimY;
   /*! @brief Flag to activate reduced form calculation | 1 => REDUCED FORM | 0 => NON-REDUCED FORM */
   uint32_t enableReducedForm;
   /*! @brief Stride value for U matrix in the transposed form */
   uint32_t strideURows;
   /*! @brief Stride value for V matrix in the transposed form */
   uint32_t strideVRows;
} DSPLIB_svdInitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_svd_getHandleSize(DSPLIB_svdInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_svd_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_svd_init, and this function
 *               must be called before the
 *               @ref DSPLIB_svd_init is called.
 *
 *  @param [in]  handle             : Active handle to the kernel
 *  @param [in]  bufParamsA        : Pointer to the structure containing dimensional
 *                                    information of input buffer A
 *  @param [out] bufParamsU         : Pointer to the structure containing dimensional
 *                                    information of output buffer U
 *  @param [out] bufParamsV         : Pointer to the structure containing dimensional
 *                                    information of output buffer V
 *  @param [out] bufParamsDiag      : Pointer to the structure containing dimensional
 *                                    information of output buffer Diag
 *  @param [out] bufParamsSuperDiag : Pointer to the structure containing dimensional
 *                                    information of output buffer SuperDiag
 *  @param [in]  pKerInitArgs       : Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_svd_init_checkParams(DSPLIB_kernelHandle         handle,
                                          const DSPLIB_bufParams2D_t *bufParamsA,
                                          const DSPLIB_bufParams2D_t *bufParamsU,
                                          const DSPLIB_bufParams2D_t *bufParamsV,
                                          const DSPLIB_bufParams1D_t *bufParamsDiag,
                                          const DSPLIB_bufParams1D_t *bufParamsSuperDiag,
                                          const DSPLIB_svdInitArgs   *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_svd_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the
 *               streaming engine and streaming address generator. The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle             : Active handle to the kernel
 *  @param [in]  bufParamsA        : Pointer to the structure containing dimensional
 *                                    information of input buffer A
 *  @param [out] bufParamsU         : Pointer to the structure containing dimensional
 *                                    information of output buffer U
 *  @param [out] bufParamsV         : Pointer to the structure containing dimensional
 *                                    information of output buffer V
 *  @param [out] bufParamsDiag      : Pointer to the structure containing dimensional
 *                                    information of output buffer Diag
 *  @param [out] bufParamsSuperDiag : Pointer to the structure containing dimensional
 *                                    information of output buffer SuperDiag
 *  @param [in]  pKerInitArgs       : Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_svd_init(DSPLIB_kernelHandle         handle,
                              const DSPLIB_bufParams2D_t *bufParamsA,
                              const DSPLIB_bufParams2D_t *bufParamsU,
                              const DSPLIB_bufParams2D_t *bufParamsV,
                              const DSPLIB_bufParams1D_t *bufParamsDiag,
                              const DSPLIB_bufParams1D_t *bufParamsSuperDiag,
                              const DSPLIB_svdInitArgs   *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_svd_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_svd_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_svd_exec is called.
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pA         : Pointer to the input buffer pA
 *  @param [out] pU         : Pointer to the output buffer pU
 *  @param [out] pV         : Pointer to the output buffer pV
 *  @param [out] pDiag      : Pointer to the output buffer pDiag
 *  @param [out] pSuperDiag : Pointer to the output buffer pSuperDiag
 *  @param [in]  pU1        : Pointer to the intermediate transpose buffer pU1
 *  @param [in]  pV1        : Pointer to the intermediate transpose buffer pV1
 *  @param [out] pScratch   : Pointer to the intermediate scratch buffer pScratch
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_svd_exec_checkParams(DSPLIB_kernelHandle handle,
                                          const void *restrict pA,
                                          const void *restrict pU,
                                          const void *restrict pV,
                                          const void *restrict pDiag,
                                          const void *restrict pSuperDiag,
                                          const void *restrict pU1,
                                          const void *restrict pV1,
                                          const void *restrict pScratch);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_svd_exec
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pA         : Pointer to the input buffer pA
 *  @param [out] pU         : Pointer to the output buffer pU
 *  @param [out] pV         : Pointer to the output buffer pV
 *  @param [out] pDiag      : Pointer to the output buffer pDiag
 *  @param [out] pSuperDiag : Pointer to the output buffer pSuperDiag
 *  @param [in]  pU1        : Pointer to the intermediate transpose buffer pU1
 *  @param [in]  pV1        : Pointer to the intermediate transpose buffer pV1
 *  @param [out] pScratch   : Pointer to the intermediate scratch buffer pScratch
 *
 *  @par Memory Requirements
 * | Buffer     | dimY     | dimX     | Comments |
 * | :--------- | :--:     | :--:     | :-----|
 * | pA         | M        | N        | Input Matrix |
 * | pU         | max(M,N) | max(M,N) | Output Matrix |
 * | pV         | N        | N        | Output Matrix |
 * | pDiag      | 1        | min(M,N) | Output Vector |
 * | pSuperDiag | 1        | min(M,N) | Intermediate Vector |
 * | pU1        | max(M,N) | max(M,N) | Scratch buffer to store transpose of U |
 * | pV1        | N        | N        | Scratch buffer to store transpose of V |
 * | pScratch   | 4        | max(M,N) | Scratch buffer to store coefficient calculations, sorting singular values etc. |
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Assumptions: - None
 *
 *  @par Performance Considerations:
 *       For best performance,
 *        - The input and output data buffers are expected to be in L2 memory
 *        - The buffer pointers are assumed to be 64-byte aligned
 *        - Ensure that the starting point of adjacent rows in a 2-D matrix are allocated from
 *          different banks in L2 memory with the help of memory padding.
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref DSPLIB_svd_init and
 *               @ref DSPLIB_svd_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */
DSPLIB_STATUS DSPLIB_svd_exec(DSPLIB_kernelHandle handle,
                              void *restrict pA,
                              void *restrict pU,
                              void *restrict pV,
                              void *restrict pDiag,
                              void *restrict pSuperDiag,
                              void *restrict pU1,
                              void *restrict pV1,
                              void *restrict pScratch);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_SVD_IXX_IXX_OXX_H_ */
