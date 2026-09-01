// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_MATMUL_IXX_IXX_OXX_H_
#define DSPLIB_MATMUL_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_matMul DSPLIB_matMul
 * @brief Kernel for multiplying two matrices
 *
 * @details
 *          - Kernel supports only floating-point datatype, C = AxB
 *
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
} DSPLIB_matMul_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_matMul_getHandleSize(DSPLIB_matMul_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_matMul_exec function is called. This
 *               function takes care of any one-time operations such as setting
 * up the configuration of required hardware resources such as
 *              the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                               information of input matrix A
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                               information of input matrix B
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output matrix C
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */

DSPLIB_STATUS DSPLIB_matMul_init(DSPLIB_kernelHandle           handle,
                                 DSPLIB_bufParams2D_t         *bufParamsIn0,
                                 DSPLIB_bufParams2D_t         *bufParamsIn1,
                                 DSPLIB_bufParams2D_t         *bufParamsOut,
                                 const DSPLIB_matMul_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_matMul_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_matMul_init, and this function
 *               must be called before the
 *               @ref DSPLIB_matMul_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                               information of input matrix A
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                               information of input matrix B
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output matrix C
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_matMul_init_checkParams(DSPLIB_kernelHandle           handle,
                               const DSPLIB_bufParams2D_t   *bufParamsIn0,
                               const DSPLIB_bufParams2D_t   *bufParamsIn1,
                               const DSPLIB_bufParams2D_t   *bufParamsOut,
                               const DSPLIB_matMul_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_matMul_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_matMul_init, and this function
 *               must be called before the
 *               @ref DSPLIB_matMul_init is called.
 *
 *  @param [in]  handle    :  Active handle to the kernel
 *  @param [in]  pIn0      :  Pointer to the structure input matrix A
 *  @param [in]  pIn1      :  Pointer to the structure input matrix B
 *  @param [out] pOut      :  Pointer to the output matrix C
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */

DSPLIB_STATUS DSPLIB_matMul_exec_checkParams(DSPLIB_kernelHandle handle,
                                             const void *restrict pIn0,
                                             const void *restrict pIn1,
                                             const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_matMul_exec
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn0        : Pointer to buffer holding the input matrix A
 *  @param [in]  pIn1        : Pointer to buffer holding the input matrix B
 *  @param [out] pOut        : Pointer to buffer holding the output matrix C
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
 *               @ref DSPLIB_matMul_init and
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */

DSPLIB_STATUS
DSPLIB_matMul_exec(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_MATMUL_IXX_IXX_OXX_H_ */
