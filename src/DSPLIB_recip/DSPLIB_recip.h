// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_RECIP_IXX_IXX_OXX_H_
#define DSPLIB_RECIP_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_recip DSPLIB_recip
 * @brief Kernel for computing the reciprocal of input data vector.
 *
 * @details
 *          - The kernel computes the reciprocal of every element of a 2D
 *            input buffer of dim_y rows of dim_x elements, with independent
 *            input and output row strides (stride_y).
 *          - Kernel supports float, double and int16_t datatypes.
 *          - For float and double, the output buffer has the same dim_x and
 *            dim_y as the input.
 *          - For int16_t (Q15), each output row holds 2 * dim_x values: the
 *            exponent and fraction of each input element, stored as per
 *            @ref DSPLIB_recip_storeStyle.
 *
 * @ingroup  DSPLIB
 */
/**@{*/

/**
 * @brief enumeration to determine store style of the fixed point
 */
typedef enum {

   /** @brief input is floating point, so store pattern is linear.    */
   DSPLIB_LINEAR_ST = 0,

   /** @brief input is fixed point, store pattern is horizontal stack;
    *         each output row is | exp0 .. expN-1 | frac0 .. fracN-1 |   */
   DSPLIB_HSTACK_ST = 1,

   /** @brief input is fixed point, store pattern is interleaved;
    *         each output row is | exp0 | frac0 | exp1 | frac1 | ..  |   */
   DSPLIB_INTERLEAVE_ST = 2
} DSPLIB_recip_storeStyle;
/**
 * @brief Structure containing the parameters to initialize the kernel
 */
typedef struct {
   /** @brief Variant of the function refer to @ref DSPLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /** @brief Number of elements in each input row, must match dim_x of the
    *         input buffer                                                  */
   uint32_t dataSize;
   /** @brief Fixed Point Output Store Style                                  */
   DSPLIB_recip_storeStyle storeStyle;
} DSPLIB_recip_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_recip_getHandleSize(DSPLIB_recip_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_recip_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the MMA
 *               accelerator and the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_recip_init(DSPLIB_kernelHandle          handle,
                                DSPLIB_bufParams2D_t        *bufParamsIn,
                                DSPLIB_bufParams2D_t        *bufParamsOut,
                                const DSPLIB_recip_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_recip_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_recip_init, and this function
 *               must be called before the
 *               @ref DSPLIB_recip_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_recip_init_checkParams(DSPLIB_kernelHandle          handle,
                                            const DSPLIB_bufParams2D_t  *bufParamsIn,
                                            const DSPLIB_bufParams2D_t  *bufParamsOut,
                                            const DSPLIB_recip_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_recip_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_recip_init, and this function
 *               must be called before the
 *               @ref DSPLIB_recip_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  pIn  :  Pointer to the structure input buffer
 *  @param [out] pOut :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_recip_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_recip_exec
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
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
 *               @ref DSPLIB_recip_init and
 *               @ref DSPLIB_recip_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */

DSPLIB_STATUS DSPLIB_recip_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief        This function is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [out] archCycles     :  Arch cycles used in that particular kernel
 *  @param [out] estCycles      :  Cycles estimated for that particular kernel
 *
 *  @remarks     Application is expected to call @ref DSPLIB_recip_init with the
 *               optimized function style before calling this function.
 */
void DSPLIB_recip_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_recip_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_recip_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_recip_exec is called.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilter     : Pointer to buffer holding the input data
 *  @param [out] pOut        : Pointer to buffer holding the output matrix
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_RECIP_IXX_IXX_OXX_H_ */
