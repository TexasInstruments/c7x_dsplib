// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_LUD_INV_IXX_IXX_OXX_H_
#define DSPLIB_LUD_INV_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_lud_inv DSPLIB_lud_inv
 * @brief Kernel for transpose of input matrix.
 *
 * @details
 *      - This function processes a permutation matrix \f$P\f$, a lower triangular matrix \f$L\f$,
 *        and an upper triangular matrix \f$U\f$ generated using LU decomposition and computes
 *        the inverse of the \f$A\f$ matrix where:
 *          \f{eqnarray*}{
 *               A^{-1} = U^{-1} \times L^{-1} \times P
 *          \f}
 *      - The input matrices \f$P\f$, \f$L\f$ and \f$U\f$ are of shape (M, M)
 *      - The ouput matrix \f$A^{-1}\f$ is of shape (M, M)
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
              - order => Order of input matrix */
   int32_t order;
} DSPLIB_lud_invInitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_lud_inv_getHandleSize(DSPLIB_lud_invInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_lud_inv_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_lud_inv_init, and this function
 *               must be called before the
 *               @ref DSPLIB_lud_inv_init is called.
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsP             :  Pointer to the structure containing dimensional
 *                                         information of input buffer P
 *  @param [in]  bufParamsL             :  Pointer to the structure containing dimensional
 *                                         information of input buffer L
 *  @param [in]  bufParamsU             :  Pointer to the structure containing dimensional
 *                                         information of input buffer U
 *  @param [out] bufParamsinvA          :  Pointer to the structure containing dimensional
 *                                         information of ouput buffer invA
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_lud_inv_init_checkParams(DSPLIB_kernelHandle           handle,
                                              const DSPLIB_bufParams2D_t   *bufParamsP,
                                              const DSPLIB_bufParams2D_t   *bufParamsL,
                                              const DSPLIB_bufParams2D_t   *bufParamsU,
                                              const DSPLIB_bufParams2D_t   *bufParamsinvA,
                                              const DSPLIB_lud_invInitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_lud_inv_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the
 *               streaming engine and streaming address generator. The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsP             :  Pointer to the structure containing dimensional
 *                                         information of input buffer P
 *  @param [in]  bufParamsL             :  Pointer to the structure containing dimensional
 *                                         information of input buffer L
 *  @param [in]  bufParamsU             :  Pointer to the structure containing dimensional
 *                                         information of input buffer U
 *  @param [out] bufParamsinvA          :  Pointer to the structure containing dimensional
 *                                         information of ouput buffer invA
 *  @param [in]  pKerInitArgs           :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_lud_inv_init(DSPLIB_kernelHandle           handle,
                                  const DSPLIB_bufParams2D_t   *bufParamsP,
                                  const DSPLIB_bufParams2D_t   *bufParamsL,
                                  const DSPLIB_bufParams2D_t   *bufParamsU,
                                  const DSPLIB_bufParams2D_t   *bufParamsinvA,
                                  const DSPLIB_lud_invInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_lud_inv_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_lud_inv_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_lud_inv_exec is called.
 *
 *  @param [in]  handle         : Active handle to the kernel
 *  @param [in]  pP             : Pointer to permutation matrix P
 *  @param [in]  pL             : Pointer to lower triangular matrix L
 *  @param [in]  pU             : Pointer to upper triangular matrix U
 *  @param [out] pinvA          : Pointer to inverse of A matrix
 *  @param [out] pScratch       : Pointer to scratch buffer Scratch
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_lud_inv_exec_checkParams(DSPLIB_kernelHandle handle,
                                              void *restrict pP,
                                              void *restrict pL,
                                              void *restrict pU,
                                              void *restrict pinvA,
                                              void *restrict pScratch);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_lud_inv_exec
 *
 *  @param [in]  handle         : Active handle to the kernel
 *  @param [in]  pP             : Pointer to permutation matrix P
 *  @param [in]  pL             : Pointer to lower triangular matrix L
 *  @param [in]  pU             : Pointer to upper triangular matrix U
 *  @param [out] pinvA          : Pointer to inverse of A matrix
 *  @param [out] pScratch       : Pointer to scratch buffer Scratch
 *
 *  @par Memory Requirements
 * | Buffer   | dimY | dimX | Comments |
 * | :-----   | :--: | :--: | :-----|
 * | pP       | M    | M    | Input Permutation Matrix (Data Type = uint16_t) |
 * | pL       | M    | M    | Input Lower Triangular Matrix |
 * | pU       | M    | M    | Input Upper Triangular Matrix |
 * | pinvA    | M    | M    | Output Square Matrix |
 * | pScratch | 1    | M    | Scratch buffer to store inverse factor |
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
 *               @ref DSPLIB_lud_inv_init and
 *               @ref DSPLIB_lud_inv_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */

DSPLIB_STATUS DSPLIB_lud_inv_exec(DSPLIB_kernelHandle handle,
                                  void *restrict pP,
                                  void *restrict pL,
                                  void *restrict pU,
                                  void *restrict pinvA,
                                  void *restrict pScratch);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_LUD_INV_IXX_IXX_OXX_H_ */
