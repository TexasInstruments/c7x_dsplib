// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_ABSOLUTEVALUE_IXX_IXX_OXX_H_
#define DSPLIB_ABSOLUTEVALUE_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_absoluteValue DSPLIB_absoluteValue
 * @brief Computes the absolute value of 2D matrix elements.
 * @details
 *          - The kernel supports absolute value computation on 2D matrices with specified dimensions and stride.
 *          - Following datatypes are supported:
 *             - DSPLIB_INT8, DSPLIB_INT16, DSPLIB_INT32, DSPLIB_INT64
 *             - DSPLIB_FLOAT32, DSPLIB_FLOAT64
 *          - Integer inputs equal to the most negative value of the type return the most positive value of the
 *            type (saturating absolute value, as the C7x VABS instructions do), for example abs(-128) = 127 for
 *            DSPLIB_INT8.
 *          - Floating point results have the sign bit cleared, so abs(-0.0) = +0.0.
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
} DSPLIB_absoluteValueInitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_absoluteValue_getHandleSize(DSPLIB_absoluteValueInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_absoluteValue_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_absoluteValue_init, and this function
 *               must be called before the
 *               @ref DSPLIB_absoluteValue_init is called.
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
DSPLIB_STATUS
DSPLIB_absoluteValue_init_checkParams(DSPLIB_kernelHandle                 handle,
                                      const DSPLIB_bufParams2D_t         *bufParamsIn,
                                      const DSPLIB_bufParams2D_t         *bufParamsOut,
                                      const DSPLIB_absoluteValueInitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_absoluteValue_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as the
 *               the streaming engine and streaming address generator.  The results of these
 *               operations are stored in the handle.
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
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_absoluteValue_init(DSPLIB_kernelHandle                 handle,
                                        DSPLIB_bufParams2D_t               *bufParamsIn,
                                        DSPLIB_bufParams2D_t               *bufParamsOut,
                                        const DSPLIB_absoluteValueInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_absoluteValue_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_absoluteValue_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_absoluteValue_exec is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the input buffer
 *  @param [out] pOut   :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_absoluteValue_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_absoluteValue_exec
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
 *    For best performance:
 *    - Input and output data buffers are expected to be in L2 memory
 *    - Buffer pointers are assumed to be 64-byte aligned
 *
 *  @remarks     Application is expected to call @ref DSPLIB_absoluteValue_init and
 *               @ref DSPLIB_absoluteValue_exec_checkParams before calling this function.
 *               This ensures resource configuration and error checks are done
 *               only once for several invocations.
 */

DSPLIB_STATUS DSPLIB_absoluteValue_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
/**
 *  @brief        This funtion is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [in]  archCycles     :  Arch compute cycles obtained from asm
 *  @param [in]  estCycles      :  Cycles estimated for that particular kernel
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */
void DSPLIB_absoluteValue_perfEst(DSPLIB_kernelHandle handle,
                                  uint64_t           *archCycles,
                                  uint64_t           *estCycles,
                                  uint32_t            dataType);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_absoluteValue_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  DSPLIB_absoluteValue.h                   */
/* ======================================================================== */
