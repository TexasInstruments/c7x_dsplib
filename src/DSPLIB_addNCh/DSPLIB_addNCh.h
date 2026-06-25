// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_ADDNCH_IXX_IXX_OXX_H_
#define DSPLIB_ADDNCH_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_addNCh DSPLIB_addNCh
 * @brief Kernel for addNCh to add the channel across multiple inputs.
 *
 * @details
 *          - The kernel support computation of  a batch of several
 *            inputs in one kernel call.
 *          - The buffer used for providing input data may be required
 *            to be bigger than data size itself.
 *
 *  * This kernel takes multiple input matrices (stacked as a single 3D matrix)
 * of size NxM for each input (number of inputs = C), and produces an output
 * matrix of size NxM, where each element is the sum across all input channels.
 *
 * @par Matrix Representation:
 *
 * Input Matrix (3 inputs stacked, size CxNxM):
 * \f[
 * X =
 * \begin{bmatrix}
 * x_{1,0,0} & x_{1,0,1} & \cdots & x_{1,0,M-1} \\
 * x_{1,1,0} & x_{1,1,1} & \cdots & x_{1,1,M-1} \\
 * \vdots & \vdots & \ddots & \vdots \\
 * x_{1,N-1,0} & x_{1,N-1,1} & \cdots & x_{1,N-1,M-1} \\
 * \hline
 * x_{2,0,0} & x_{2,0,1} & \cdots & x_{2,0,M-1} \\
 * x_{2,1,0} & x_{2,1,1} & \cdots & x_{2,1,M-1} \\
 * \vdots & \vdots & \ddots & \vdots \\
 * x_{2,N-1,0} & x_{2,N-1,1} & \cdots & x_{2,N-1,M-1} \\
 * \hline
 * x_{3,0,0} & x_{3,0,1} & \cdots & x_{3,0,M-1} \\
 * x_{3,1,0} & x_{3,1,1} & \cdots & x_{3,1,M-1} \\
 * \vdots & \vdots & \ddots & \vdots \\
 * x_{3,N-1,0} & x_{3,N-1,1} & \cdots & x_{3,N-1,M-1} \\
 * \end{bmatrix}
 * \f]
 *
 * Output Matrix (summation across inputs, size NxM):
 * \f[
 * Y =
 * \begin{bmatrix}
 * \sum_{c=1}^{3} x_{c,0,0} & \sum_{c=1}^{3} x_{c,0,1} & \cdots & \sum_{c=1}^{3} x_{c,0,M-1} \\
 * \sum_{c=1}^{3} x_{c,1,0} & \sum_{c=1}^{3} x_{c,1,1} & \cdots & \sum_{c=1}^{3} x_{c,1,M-1} \\
 * \vdots & \vdots & \ddots & \vdots \\
 * \sum_{c=1}^{3} x_{c,N-1,0} & \sum_{c=1}^{3} x_{c,N-1,1} & \cdots & \sum_{c=1}^{3} x_{c,N-1,M-1} \\
 * \end{bmatrix}
 * \f]
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
   /** @brief Size of input data                                              */
} DSPLIB_addNCh_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_addNCh_getHandleSize(DSPLIB_addNCh_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_addNCh_exec function is called. This
 *               function takes care of any one-time operations such as setting
 * up the configuration of required hardware resources such as the MMA
 *               accelerator and the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_addNCh_init(DSPLIB_kernelHandle           handle,
                                 DSPLIB_bufParams3D_t         *bufParamsIn,
                                 DSPLIB_bufParams2D_t         *bufParamsOut,
                                 const DSPLIB_addNCh_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_addNCh_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_addNCh_init, and this function
 *               must be called before the
 *               @ref DSPLIB_addNCh_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_addNCh_init_checkParams(DSPLIB_kernelHandle           handle,
                               const DSPLIB_bufParams3D_t   *bufParamsIn,
                               const DSPLIB_bufParams2D_t   *bufParamsOut,
                               const DSPLIB_addNCh_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_addNCh_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_addNCh_init, and this function
 *               must be called before the
 *               @ref DSPLIB_addNCh_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  pIn  :  Pointer to the structure input buffer
 *  @param [out] pout :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_addNCh_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_addNCh_exec
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [out] pOut        : Pointer to buffer holding the output data
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
 *               @ref DSPLIB_addNCh_init and
 *               @ref DSPLIB_addNCh_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */

DSPLIB_STATUS
DSPLIB_addNCh_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief        This funtion is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [in]  archCycles     :  Arch compute cycles obtained from asm
 *  @param [in]  estCycles      :  Cycles estimated for that purticular kenel
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * AUDIOLIB_STATUS.
 *
 *  @remarks     None
 */
void DSPLIB_addNCh_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles, uint32_t dataType);

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_ADDNCH_IXX_IXX_OXX_H_ */
