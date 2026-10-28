// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_bitwiseANDConst_IXX_IXX_OXX_H_
#define DSPLIB_bitwiseANDConst_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_bitwiseANDConst DSPLIB_bitwiseANDConst
 * @brief Kernel for performing bitwise AND operation between input vector data and a constant value.
 *
 * This kernel takes one matrix of size NxM and another matrix of size 1X1, performs a bitwise AND operation and
 * produces an output matrix of size NxM.
 *
 * @par Example:
 *
 * Input0 Matrix (3x3):
 * \f[
 * \text{In0} =
 * \begin{bmatrix}
 * 23 & 146 & 179 \\
 * 57 & 131 & 29 \\
 * 38 & 43 & 88 \\
 * \end{bmatrix}
 * \f]
 *
 * Input1 Matrix (1x1):
 * \f[
 * \text{In1} =
 * \begin{bmatrix}
 * 0
 * \end{bmatrix}
 * \f]
 *
 *
 * Output Matrix (3x3):
 * \f[
 * \text{Output} =
 * \begin{bmatrix}
 * 0 & 0 & 0 \\
 * 0 & 0 & 0 \\
 * 0 & 0 & 0 \\
 * \end{bmatrix}
 * \f]

 * @note Supported input data types:
 *       - uint8_t
 *       - uint16_t
 *       - uint32_t
 *       - uint64_t
 *
 *
 * @ingroup  DSPLIB
 */
/**@{*/

/**
 * @brief Structure containing the parameters to initialize the kernel
 */
typedef struct {
   /** @brief Variant of the function, refer to @ref DSPLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
} DSPLIB_bitwiseANDConst_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_bitwiseANDConst_getHandleSize(DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_bitwiseANDConst_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as
 *               the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input0 buffer
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                                information of input1 buffer
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

DSPLIB_STATUS DSPLIB_bitwiseANDConst_init(DSPLIB_kernelHandle                    handle,
                                          DSPLIB_bufParams2D_t                  *bufParamsIn0,
                                          DSPLIB_bufParams1D_t                  *bufParamsIn1,
                                          DSPLIB_bufParams2D_t                  *bufParamsOut,
                                          const DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_bitwiseANDConst_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_bitwiseANDConst_init, and this function
 *               must be called before the
 *               @ref DSPLIB_bitwiseANDConst_init is called.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input0 buffer
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                                information of input1 buffer
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
DSPLIB_bitwiseANDConst_init_checkParams(DSPLIB_kernelHandle                    handle,
                                        const DSPLIB_bufParams2D_t            *bufParamsIn0,
                                        const DSPLIB_bufParams1D_t            *bufParamsIn1,
                                        const DSPLIB_bufParams2D_t            *bufParamsOut,
                                        const DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_bitwiseANDConst_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_bitwiseANDConst_init, and this function
 *               must be called before the
 *               @ref DSPLIB_bitwiseANDConst_init is called.
 *
 *  @param [in]  handle    :  Active handle to the kernel
 *  @param [in]  pIn0      :  Pointer to the structure input0 buffer
 *  @param [in]  pIn1      :  Pointer to the structure input1 buffer
 *  @param [out] pOut      :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */

DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_checkParams(DSPLIB_kernelHandle handle,
                                                      const void *restrict pIn0,
                                                      const void *restrict pIn1,
                                                      const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_bitwiseANDConst_exec
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pIn0       : Pointer to buffer holding the input0 buffer
 *  @param [in]  pIn1       : Pointer to buffer holding the input1 buffer
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
 *               @ref DSPLIB_bitwiseANDConst_init and
 *               This ensures resource configuration and error checks are done
 *               only once for several invocations of this function.
 */

DSPLIB_STATUS
DSPLIB_bitwiseANDConst_exec(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);

/**
 *  @brief        This funtion is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [out] archCycles     :  Arch cycles used in that particluar kernel
 *  @param [out] estCycles      :  Cycles estimated for that particular kenel
 *
 *  @remarks     None
 */
void DSPLIB_bitwiseANDConst_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_bitwiseANDConst_IXX_IXX_OXX_H_ */
