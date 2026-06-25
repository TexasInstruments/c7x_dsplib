// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_Q15TOFL_IXX_IXX_OXX_H_
#define DSPLIB_Q15TOFL_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_q15tofl DSPLIB_q15tofl
 * @brief Kernel Converts the values stored in vector pIn in Q.15 format
 *  to IEEE floating point numbers (Single & Double Precision) in output vector pOut.
 * @details
 *          - The kernel support computation of vectors of several
 *            inputs in one kernel call.
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
   /*! @brief Size of input data                                              */
   uint32_t dataSize;
} DSPLIB_q15tofl_InitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_q15tofl_getHandleSize(DSPLIB_q15tofl_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_q15tofl_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as the
 *               streaming engine and streaming address generator. The results of these
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
DSPLIB_STATUS DSPLIB_q15tofl_init(DSPLIB_kernelHandle            handle,
                                  DSPLIB_bufParams1D_t          *bufParamsIn,
                                  DSPLIB_bufParams1D_t          *bufParamsOut,
                                  const DSPLIB_q15tofl_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_q15tofl_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_q15tofl_init, and this function
 *               must be called before the
 *               @ref DSPLIB_q15tofl_init is called.
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
DSPLIB_q15tofl_init_checkParams(DSPLIB_kernelHandle            handle,
                                const DSPLIB_bufParams1D_t    *bufParamsIn,
                                const DSPLIB_bufParams1D_t    *bufParamsOut,
                                const DSPLIB_q15tofl_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_q15tofl_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_q15tofl_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_q15tofl_exec is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the structure input buffer
 *  @param [out] pout   :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_q15tofl_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_q15tofl_exec
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
 *               @ref DSPLIB_q15tofl_init and
 *               @ref DSPLIB_q15tofl_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */

DSPLIB_STATUS DSPLIB_q15tofl_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**@}*/
#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_Q15TOFL_IXX_IXX_OXX_H_ */
