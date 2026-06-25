// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_QRD_INVERSE_IXX_IXX_OXX_H_
#define DSPLIB_QRD_INVERSE_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_qrd_inverse DSPLIB_qrd_inverse
 * @brief Kernel for find the inverse of a matrix from Q and R matrices.
 *
 *  @details     - This function processes an orthogonal matrix \f$Q\f$, and an upper triangular
 *               matrix \f$R\f$ obtained by QR Decomposition and generates the inverse of
 *               the \f$A\f$ matrix as:
 *              \f{eqnarray*}{
 *                A^{-1} = R^{-1} \times Q^{T}
 *               \f}
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
   /*! @brief Size of input data
              - dimX => Number of columns */
   uint32_t dimX;
   /*! @brief Size of input data
               - dimY => number of rows */
   uint32_t dimY;
} DSPLIB_qrdInvInitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_qrd_inverse_getHandleSize(DSPLIB_qrdInvInitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_qrd_inverse_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the
 *               streaming engine and streaming address generator. The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle             :  Active handle to the kernel
 *  @param [in]  bufParamsQ         :  Pointer to the structure containing dimensional
 *                                     information of Q matrix
 *  @param [in]  bufParamsR         :  Pointer to the structure containing dimensional
 *                                     information of R matrix
 *  @param [out] bufParamsInvA      :  Pointer to the structure containing dimensional
 *                                     information of A inverse buffer matrix
 *  @param [out] bufParamsInvAFinal :  Pointer to the structure containing dimensional
 *                                     information of final A inverse matrix allocated from
 *                                     MSMC memory for C7100 and C7120
 *  @param [in]  pKerInitArgs       :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_qrd_inverse_init(DSPLIB_kernelHandle          handle,
                                      DSPLIB_bufParams2D_t        *bufParamsQ,
                                      DSPLIB_bufParams2D_t        *bufParamsR,
                                      DSPLIB_bufParams2D_t        *bufParamsInvA,
                                      DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                      const DSPLIB_qrdInvInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_qrd_inverse_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_qrd_inverse_init, and this function
 *               must be called before the
 *               @ref DSPLIB_qrd_inverse_init is called.
 *
 *  @param [in]  handle             :  Active handle to the kernel
 *  @param [in]  bufParamsQ         :  Pointer to the structure containing dimensional
 *                                     information of Q matrix
 *  @param [in]  bufParamsR         :  Pointer to the structure containing dimensional
 *                                     information of R matrix
 *  @param [out] bufParamsInvA      :  Pointer to the structure containing dimensional
 *                                     information of A inverse buffer matrix
 *  @param [out] bufParamsInvAFinal :  Pointer to the structure containing dimensional
 *                                     information of final A inverse matrix allocated from
 *                                     MSMC memory for C7100 and C7120
 *  @param [in]  pKerInitArgs       :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_qrd_inverse_init_checkParams(DSPLIB_kernelHandle          handle,
                                                  DSPLIB_bufParams2D_t        *bufParamsQ,
                                                  DSPLIB_bufParams2D_t        *bufParamsR,
                                                  DSPLIB_bufParams2D_t        *bufParamsInvA,
                                                  DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                                  const DSPLIB_qrdInvInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_qrd_inverse_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_qrd_inverse_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_qrd_inverse_exec is called.
 *
 *  @param [in]  handle     :  Active handle to the kernel
 *  @param [in]  pQ         :  Pointer to matrix Q[NRows*NRows]
 *  @param [in]  pR         :  Pointer to upper triangular matrix R[NRows*NCols]
 *  @param [in]  pInvA      :  Pointer to A inverse buffer matrix
 *  @param [out] pInvAScratch : Pointer to Scratch buffer
 *  @param [in]  pScratch : Pointer to Scratch buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_qrd_inverse_exec_checkParams(DSPLIB_kernelHandle handle,
                                                  void *restrict pQ,
                                                  void *restrict pR,
                                                  void *restrict pInvA,
                                                  void *restrict pInvAScratch,
                                                  void *restrict pScratch);

/*!
 *  @brief       This function is the main kernel compute function.
 *  @details     Please refer to details under
 *               @ref DSPLIB_qrd_inverse_exec
 *
 *  @param [in]  handle     :  Active handle to the kernel
 *  @param [in]  pQ         :  Pointer to matrix Q
 *  @param [in]  pR         :  Pointer to upper triangular matrix R
 *  @param [in]  pInvA      :  Pointer to A inverse buffer matrix
 *  @param [out] pInvAScratch : Pointer to Scratch buffer
 *  @param [in] pScratch : Pointer to Scratch buffer
 *
 *  @par Memory Requirements
 * | Buffer       | dimY | dimX | Comments |
 * | :--          | :--: | :--: | :-----|
 * | pQ           | M    | M    | Input Square Matrix |
 * | pR           | M    | M    | Input Square Matrix |
 * | pInvA        | M    | M    | Output Square Matrix |
 * | pInvAScratch | M    | M    | Scratch buffer to store intermediate matrix operations |
 * | pScratch     | 1    | M    | Scratch buffer to store inverse factor |
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
 *               @ref DSPLIB_qrd_inverse_init and
 *               @ref DSPLIB_qrd_inverse_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */
DSPLIB_STATUS DSPLIB_qrd_inverse_exec(DSPLIB_kernelHandle handle,
                                      void *restrict pQ,
                                      void *restrict pR,
                                      void *restrict pInvA,
                                      void *restrict pInvAScratch,
                                      void *restrict pScratch);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_QRD_INVERSE_IXX_IXX_OXX_H_ */
