// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_LUD_IXX_IXX_OXX_H_
#define DSPLIB_LUD_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_lud DSPLIB_lud
 * @brief Kernel for LU decomposition of input matrix.
 *
 * @details
 *      - This function decomposes the square matrix \f$A\f$ into a lower triangular matrix,
 *      upper triangular matrix, and a permutation matrix where:
 *          \f{eqnarray*}{
 *               A = P^{T} \times L \times U \qquad
 *                         where &(1)&\;P\;is\;a\;permutation\;matrix\\
 *                               &(2)&\;L\;is\;a\;lower\;triangular\;matrix\\
 *                               &(3)&\;U\;is\;a\;upper\;triangular\;matrix\\ \f}
 *      - The Gaussian elimination outer product algorithm is used.
 *      - The permutation matrix \f$P\f$ keeps track of the reordering of rows
 *        in the \f$A\f$ matrix such that the first element of the permuted matrix is maximum
 *        and nonzero.
 *      - Input \f$A\f$ is of the shape (M, M).
 *      - Output matrices \f$P\f$, \f$L\f$ and \f$U\f$ are of the shape (M, M)
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
   /*! @brief Size of input data
              - order => Order of input matrix A */
   int32_t order;
} DSPLIB_ludInitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_lud_getHandleSize(DSPLIB_ludInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_lud_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_lud_init, and this function
 *               must be called before the
 *               @ref DSPLIB_lud_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsA   :  Pointer to the structure containing dimensional
 *                               information of input buffer A
 *  @param [out] bufParamsL   :  Pointer to the structure containing dimensional
 *                               information of output buffer L
 *  @param [out] bufParamsU   :  Pointer to the structure containing dimensional
 *                               information of output buffer U
 *  @param [out] bufParamsP   :  Pointer to the structure containing dimensional
 *                               information of output buffer P
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_lud_init_checkParams(DSPLIB_kernelHandle         handle,
                                          const DSPLIB_bufParams2D_t *bufParamsA,
                                          const DSPLIB_bufParams2D_t *bufParamsL,
                                          const DSPLIB_bufParams2D_t *bufParamsU,
                                          const DSPLIB_bufParams2D_t *bufParamsP,
                                          const DSPLIB_ludInitArgs   *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_lud_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the
 *               streaming engine and streaming address generator. The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsA   :  Pointer to the structure containing dimensional
 *                               information of input buffer A
 *  @param [out] bufParamsL   :  Pointer to the structure containing dimensional
 *                               information of output buffer L
 *  @param [out] bufParamsU   :  Pointer to the structure containing dimensional
 *                               information of output buffer U
 *  @param [out] bufParamsP   :  Pointer to the structure containing dimensional
 *                               information of output buffer P
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_lud_init(DSPLIB_kernelHandle         handle,
                              const DSPLIB_bufParams2D_t *bufParamsA,
                              const DSPLIB_bufParams2D_t *bufParamsL,
                              const DSPLIB_bufParams2D_t *bufParamsU,
                              const DSPLIB_bufParams2D_t *bufParamsP,
                              const DSPLIB_ludInitArgs   *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_lud_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_lud_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_lud_exec is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pA     :  Pointer to matrix A
 *  @param [out] pL     :  Pointer to lower triangular matrix L
 *  @param [out] pU     :  Pointer to upper triangular matrix U
 *  @param [out] pP     :  Pointer to permutation matrix P
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_lud_exec_checkParams(DSPLIB_kernelHandle handle,
                                          const void *restrict pA,
                                          const void *restrict pL,
                                          const void *restrict pU,
                                          const void *restrict pP);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_lud_exec
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pA          : Pointer to matrix A
 *  @param [out] pL          : Pointer to lower triangular matrix L
 *  @param [out] pU          : Pointer to upper triangular matrix U
 *  @param [out] pP          : Pointer to permutation matrix P
 *
 *  @par Memory Requirements
 * | Buffer | dimY | dimX | Comments |
 * | :----- | :--: | :--: | :-----|
 * | pA     | M    | M    | Input Square Matrix |
 * | pL     | M    | M    | Output Lower Triangular Matrix |
 * | pU     | M    | M    | Output Upper Triangular Matrix |
 * | pP     | M    | M    | Output Permutation Matrix (Data Type = uint16_t) |
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Assumptions:
 *    - None
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - The input and output data buffers are expected to be in L2 memory
 *    - The buffer pointers are assumed to be 64-byte aligned
 *    - Ensure that the starting point of adjacent rows in a 2-D matrix are allocated from
 *      different banks in L2 memory with the help of memory padding.
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref DSPLIB_lud_init and
 *               @ref DSPLIB_lud_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */

DSPLIB_STATUS
DSPLIB_lud_exec(DSPLIB_kernelHandle handle, void *restrict pA, void *restrict pL, void *restrict pU, void *restrict pP);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_LUD_IXX_IXX_OXX_H_ */
