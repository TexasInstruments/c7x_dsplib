// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_BLKCOPYCONST2D_IXX_IXX_OXX_H_
#define DSPLIB_BLKCOPYCONST2D_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_blkCopyConst2D DSPLIB_blkCopyConst2D
 * @brief This Kernel copies a constant value from one memory location pointed to by pIn1 to another pointed to by
 * pOut.
 * @details
 *          - The kernel supports copying 2D blocks of data with specified dimensions and stride.
 *
 *          - Following datatypes are supported :
 *             - DSPLIB_INT8, DSPLIB_INT16, DSPLIB_INT32, DSPLIB_INT64, DSPLIB_UINT8,
 *               DSPLIB_UINT16, DSPLIB_UINT32, DSPLIB_UINT64, DSPLIB_FLOAT32, DSPLIB_FLOAT64
 *
 * <b>Example: </b>
 *
 * <b>Input constant value</b> (pIn1): -2 <br>
 *
 * <b>Output</b> (pY): No. of samples = N; No. of channels = M.
 *  | Channel/Sample |  s0  |  s1  | ... | sN-1 |
 *  |--------------- |------|------|-----|------|
 *  |      ch0       |  -2  |  -2  | ... |  -2  |
 *  |      ch1       |  -2  |  -2  | ... |  -2  |
 *  |      .         |   .  |   .  | ... |   .  |
 *  |      .         |   .  |   .  | ... |   .  |
 *  |      .         |   .  |   .  | ... |   .  |
 *  |     chM-1      |  -2  |  -2  | ... |  -2  |
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
} DSPLIB_blkCopyConst2DInitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_blkCopyConst2D_getHandleSize(DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_blkCopyConst2D_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_blkCopyConst2D_init, and this function
 *               must be called before the
 *               @ref DSPLIB_blkCopyConst2D_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                               information of constant buffer
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
DSPLIB_blkCopyConst2D_init_checkParams(DSPLIB_kernelHandle                  handle,
                                       const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                       const DSPLIB_bufParams2D_t          *bufParamsOut,
                                       const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_blkCopyConst2D_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as the
 *               the streaming engine and streaming address generator.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                               information of constant buffer
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
DSPLIB_STATUS DSPLIB_blkCopyConst2D_init(DSPLIB_kernelHandle                  handle,
                                         DSPLIB_bufParams1D_t                *bufParamsIn1,
                                         DSPLIB_bufParams2D_t                *bufParamsOut,
                                         const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_blkCopyConst2D_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_blkCopyConst2D_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_blkCopyConst2D_exec is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn1   :  Pointer to the structure constant buffer
 *  @param [out] pout   :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_checkParams(DSPLIB_kernelHandle handle,
                                       const void *restrict pIn1,
                                       const void *restrict pOut);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_blkCopyConst2D_exec
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn1        : Pointer to buffer holding the constant data
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
 *               @ref DSPLIB_blkCopyConst2D_init and
 *               @ref DSPLIB_blkCopyConst2D_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */

DSPLIB_STATUS DSPLIB_blkCopyConst2D_exec(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

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
void DSPLIB_blkCopyConst2D_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_BLKCOPYCONST2D_IXX_IXX_OXX_H_ */
