// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_realImagToComplex_IXX_IXX_OXX_H_
#define DSPLIB_realImagToComplex_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_realImagToComplex DSPLIB_realImagToComplex
 * @brief Kernel for multichannel realImagToComplex apply
 *
 * @details
 *          - Kernel for applying same realImagToComplex on multiple channels

 * @brief Combines real and imaginary parts into a complex matrix.
 *
 This kernel takes separate real and imaginary matrices of size NxM and produces
 * an output matrix of size Nx(2*M), where the columns are interleaved as:
 * real_0, imag_0, real_1, imag_1, ..., real_(M-1), imag_(M-1)
 *
 * @par Matrix Representation:
 *
 * Input Real Matrix (NxM):
 * \f[
 * \text{Real} =
 * \begin{bmatrix}
 * r_{0,0} & r_{0,1} & \cdots & r_{0,M-1} \\
 * r_{1,0} & r_{1,1} & \cdots & r_{1,M-1} \\
 * \vdots & \vdots & \ddots & \vdots \\
 * r_{N-1,0} & r_{N-1,1} & \cdots & r_{N-1,M-1} \\
 * \end{bmatrix}
 * \f]
 *
 * Input Imaginary Matrix (NxM):
 * \f[
 * \text{Imag} =
 * \begin{bmatrix}
 * i_{0,0} & i_{0,1} & \cdots & i_{0,M-1} \\
 * i_{1,0} & i_{1,1} & \cdots & i_{1,M-1} \\
 * \vdots & \vdots & \ddots & \vdots \\
 * i_{N-1,0} & i_{N-1,1} & \cdots & i_{N-1,M-1} \\
 * \end{bmatrix}
 * \f]
 *
 * Output Matrix (Nx2M):
 * \f[
 * \text{Output} =
 * \begin{bmatrix}
 * r_{0,0} & i_{0,0} & r_{0,1} & i_{0,1} & \cdots & r_{0,M-1} & i_{0,M-1} \\
 * r_{1,0} & i_{1,0} & r_{1,1} & i_{1,1} & \cdots & r_{1,M-1} & i_{1,M-1} \\
 * \vdots & \vdots & \vdots & \vdots & \ddots & \vdots & \vdots \\
 * r_{N-1,0} & i_{N-1,0} & r_{N-1,1} & i_{N-1,1} & \cdots & r_{N-1,M-1} & i_{N-1,M-1} \\
 * \end{bmatrix}
 * \f]

 * @note The input matrices `real` and `imag` must have the same dimensions.
 * @note Supported input data types:
 *       - float
 *       - double
 *       - int32_t
 *       - uint32_t
 *       - int16_t
 *       - uint16_t
 *
 * @tparam T Data type of input matrices (`float`, `double`, `int32_t`, `uint32_t`, `int16_t`, `uint16_t`)

 *
 * @ingroup  DSPLIB
 */
/**@{*/

/**
 * @brief Structure containing the parameters to initialize the kernel
 */
typedef struct {
   /** @brief Variant of the function refer to @ref DSPLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
} DSPLIB_realImagToComplex_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_realImagToComplex_getHandleSize(DSPLIB_realImagToComplex_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_realImagToComplex_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as
 *               the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input Real buffer
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                                information of input imag buffer
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of output buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */

DSPLIB_STATUS DSPLIB_realImagToComplex_init(DSPLIB_kernelHandle                      handle,
                                            DSPLIB_bufParams2D_t                    *bufParamsIn0,
                                            DSPLIB_bufParams2D_t                    *bufParamsIn1,
                                            DSPLIB_bufParams2D_t                    *bufParamsOut,
                                            const DSPLIB_realImagToComplex_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_realImagToComplex_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_realImagToComplex_init, and this function
 *               must be called before the
 *               @ref DSPLIB_realImagToComplex_init is called.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input real buffer
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                                information of input imag buffer
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of output buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_realImagToComplex_init_checkParams(DSPLIB_kernelHandle                      handle,
                                          const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                          const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                          const DSPLIB_bufParams2D_t              *bufParamsOut,
                                          const DSPLIB_realImagToComplex_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_realImagToComplex_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_realImagToComplex_init, and this function
 *               must be called before the
 *               @ref DSPLIB_realImagToComplex_init is called.
 *
 *  @param [in]  handle    :  Active handle to the kernel
 *  @param [in]  pIn0      :  Pointer to the structure input real buffer
 *  @param [in]  pIn1      :  Pointer to the structure input imag buffer
 *  @param [out] pOut      :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */

DSPLIB_STATUS DSPLIB_realImagToComplex_exec_checkParams(DSPLIB_kernelHandle handle,
                                                        const void *restrict pIn0,
                                                        const void *restrict pIn1,
                                                        const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_realImagToComplex_exec
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pIn0       : Pointer to buffer holding the input real buffer
 *  @param [in]  pIn1       : Pointer to buffer holding the input imag buffer
 *  @param [out] pOut       : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @par Assumptions:
 *    - None
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref DSPLIB_realImagToComplex_init and
 *               This ensures resource configuration and error checks are done
 *               only once for several invocations of this function.
 */

DSPLIB_STATUS
DSPLIB_realImagToComplex_exec(DSPLIB_kernelHandle handle,
                              void *restrict pIn0,
                              void *restrict pIn1,
                              void *restrict pOut);

/**
 *  @brief        This funtion is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [in]  archCycles     :  Arch cycles used in that purticluar kernel
 *  @param [in]  estCycles      :  Cycles estimated for that purticular kenel
 *
 *  @remarks     None
 */
void DSPLIB_realImagToComplex_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_realImagToComplex_IXX_IXX_OXX_H_ */
