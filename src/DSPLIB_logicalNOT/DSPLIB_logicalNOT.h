// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_logicalNOT_IXX_IXX_OXX_H_
#define DSPLIB_logicalNOT_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_logicalNOT DSPLIB_logicalNOT
 * @brief Kernel for performing element-wise logical NOT (Boolean invert) of an input matrix.
 *
 * This kernel takes one matrix of size NxM and produces an output matrix of size NxM of the same data type.
 * Each output element is 1 if the corresponding input element is zero, and 0 if the input element is non-zero:
 * \f[
 * \text{Out}[i][j] = (\text{In}[i][j] == 0) \; ? \; 1 : 0
 * \f]
 *
 * @par Example:
 *
 * Input Matrix (3x3):
 * \f[
 * \text{In} =
 * \begin{bmatrix}
 * 23 & 0 & 179 \\
 * 0 & 131 & 0 \\
 * 38 & 43 & 0 \\
 * \end{bmatrix}
 * \f]
 *
 * Output Matrix (3x3):
 * \f[
 * \text{Output} =
 * \begin{bmatrix}
 * 0 & 1 & 0 \\
 * 1 & 0 & 1 \\
 * 0 & 0 & 1 \\
 * \end{bmatrix}
 * \f]
 * @note Supported data types (output data type is the same as the input data type):
 *       - uint8_t, int8_t
 *       - uint16_t, int16_t
 *       - uint32_t, int32_t
 *       - uint64_t, int64_t
 *       - float, double
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
} DSPLIB_logicalNOT_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_logicalNOT_getHandleSize(DSPLIB_logicalNOT_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_logicalNOT_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as
 *               the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                                information of input buffer
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

DSPLIB_STATUS DSPLIB_logicalNOT_init(DSPLIB_kernelHandle               handle,
                                     DSPLIB_bufParams2D_t             *bufParamsIn,
                                     DSPLIB_bufParams2D_t             *bufParamsOut,
                                     const DSPLIB_logicalNOT_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_logicalNOT_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_logicalNOT_init, and this function
 *               must be called before the
 *               @ref DSPLIB_logicalNOT_init is called.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                                information of input buffer
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
DSPLIB_logicalNOT_init_checkParams(DSPLIB_kernelHandle               handle,
                                   const DSPLIB_bufParams2D_t       *bufParamsIn,
                                   const DSPLIB_bufParams2D_t       *bufParamsOut,
                                   const DSPLIB_logicalNOT_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_logicalNOT_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_logicalNOT_init, and this function
 *               must be called before the
 *               @ref DSPLIB_logicalNOT_init is called.
 *
 *  @param [in]  handle    :  Active handle to the kernel
 *  @param [in]  pIn       :  Pointer to the input buffer
 *  @param [out] pOut      :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */

DSPLIB_STATUS
DSPLIB_logicalNOT_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_logicalNOT_exec
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pIn        : Pointer to buffer holding the input buffer
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
 *               @ref DSPLIB_logicalNOT_init and
 *               This ensures resource configuration and error checks are done
 *               only once for several invocations of this function.
 */

DSPLIB_STATUS
DSPLIB_logicalNOT_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief        This funtion is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [in]  archCycles     :  Arch cycles used in that particluar kernel
 *  @param [in]  estCycles      :  Cycles estimated for that particular kenel
 *
 *  @remarks     None
 */
void DSPLIB_logicalNOT_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_logicalNOT_IXX_IXX_OXX_H_ */
