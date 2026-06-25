// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_BLK_ESWAP_IXX_IXX_OXX_H_
#define DSPLIB_BLK_ESWAP_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_blk_eswap DSPLIB_blk_eswap
 * @brief Kernel for endian-swap of all input data
 * @details
 *     - The  data in \f$dstPtr\f$ array is endian swapped, meaning that the
 *     byte-order of the bytes within each elements in input array \f$srcPtr\f$
 *     is reversed. This facilitates moving of big-endian data to a
 *     little-endian system or vice-versa.
 *     - The kernel supports both In-place and Out-of-place endian swap.
 *       - In-place endian swap occurs when \f$dstPtr = NULL\f$, allowing the swap to occur
 *          without using any additional storage
 *       - Out-of-place endian swap occurs when \f$dstPtr \neq NULL\f$
 *        .
 *
 *     - Following datatypes are supported :
 *        - DSPLIB_INT16, DSPLIB_INT32, DSPLIB_INT64, DSPLIB_UINT16, DSPLIB_UINT32, DSPLIB_UINT64,
 *          DSPLIB_FLOAT32, DSPLIB_FLOAT64
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
} DSPLIB_blk_eswap_InitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_blk_eswap_getHandleSize(DSPLIB_blk_eswap_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_blk_eswap_exec function is called. This
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
DSPLIB_STATUS DSPLIB_blk_eswap_init(DSPLIB_kernelHandle              handle,
                                    DSPLIB_bufParams1D_t            *bufParamsIn,
                                    DSPLIB_bufParams1D_t            *bufParamsOut,
                                    const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_blk_eswap_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_blk_eswap_init, and this function
 *               must be called before the
 *               @ref DSPLIB_blk_eswap_init is called.
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
DSPLIB_blk_eswap_init_checkParams(DSPLIB_kernelHandle              handle,
                                  const DSPLIB_bufParams1D_t      *bufParamsIn,
                                  const DSPLIB_bufParams1D_t      *bufParamsOut,
                                  const DSPLIB_blk_eswap_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_blk_eswap_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_blk_eswap_init, and this function
 *               must be called before the
 *               @ref DSPLIB_blk_eswap_init is called.
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
DSPLIB_blk_eswap_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut);

/*!
 *  @brief       This function is the main kernel swapping function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_blk_eswap_exec
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
 *               @ref DSPLIB_blk_eswap_init and
 *               @ref DSPLIB_blk_eswap_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */

DSPLIB_STATUS DSPLIB_blk_eswap_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_BLK_ESWAP_IXX_IXX_OXX_H_ */
