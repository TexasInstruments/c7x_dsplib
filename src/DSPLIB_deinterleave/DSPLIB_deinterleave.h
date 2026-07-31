// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_DEINTERLEAVE_IXX_IXX_OXX_H_
#define DSPLIB_DEINTERLEAVE_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_deinterleave DSPLIB_deinterleave
 * @brief Kernel for deinterleave of input vector data.
 *
 * @details
 *          - This kernel will deinterleave the given input vector data by using matrix transpose
 *            and stores it in an output vector.
 *          - This kernel makes use of the  of SE engine.
 *            - For the 64-bit datatypes, SE engine is configured with
 *            __SE_TRANSPOSE_64BIT mode
 *            - For the 8-bit, 16-bit and 32-bit datatypes, SE engine is
 *              configured with __SE_TRANSPOSE_32BIT mode
 *              .
 *          - For 8-bit datatypes the number of columns of input matrix should be multiple of 4
 *          - For 16-bit datatypes the number of columns of input matrix should be multiple of 2.
 *          - Following datatypes are supported :
 *             - DSPLIB_INT8, DSPLIB_INT16, DSPLIB_INT32, DSPLIB_INT64, DSPLIB_UINT8, DSPLIB_UINT16,
 *               DSPLIB_UINT32, DSPLIB_UINT64, DSPLIB_FLOAT32, DSPLIB_FLOAT64.
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
} DSPLIB_deinterleaveInitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_deinterleave_getHandleSize(DSPLIB_deinterleaveInitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_deinterleave_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the
 *               streaming engine and streaming address generator. The results of these
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
DSPLIB_STATUS DSPLIB_deinterleave_init(DSPLIB_kernelHandle                handle,
                                       DSPLIB_bufParams2D_t              *bufParamsIn,
                                       DSPLIB_bufParams2D_t              *bufParamsOut,
                                       const DSPLIB_deinterleaveInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_deinterleave_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_deinterleave_init, and this function
 *               must be called before the
 *               @ref DSPLIB_deinterleave_init is called.
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
DSPLIB_STATUS DSPLIB_deinterleave_init_checkParams(DSPLIB_kernelHandle                handle,
                                                   const DSPLIB_bufParams2D_t        *bufParamsIn,
                                                   const DSPLIB_bufParams2D_t        *bufParamsOut,
                                                   const DSPLIB_deinterleaveInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_deinterleave_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_deinterleave_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_deinterleave_exec is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the structure input buffer
 *  @param [out] pOut   :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_deinterleave_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_deinterleave_exec
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
 *               @ref DSPLIB_deinterleave_init and
 *               @ref DSPLIB_deinterleave_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 *				 DSPLIB_deinterleave_exec function calls functions from DSPLIB_matTrans_exec
 */

DSPLIB_STATUS DSPLIB_deinterleave_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief        This funtion is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [in]  archCycles     :  Arch compute cycles obtained from asm
 *  @param [in]  estCycles      :  Cycles estimated for that purticular kenel
 *  @param [in]  dataType       :  Datatype of purticular test case
 *
 *  @remarks     None
 */
void DSPLIB_deinterleave_perfEst(DSPLIB_kernelHandle handle,
                                 uint64_t           *archCycles,
                                 uint64_t           *estCycles,
                                 uint64_t            dataType);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_DEINTERLEAVE_IXX_IXX_OXX_H_ */
