// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_peakAbsoluteValue_IXX_IXX_OXX_H_
#define DSPLIB_peakAbsoluteValue_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_peakAbsoluteValue DSPLIB_peakAbsoluteValue
 * @brief Finds the maximum absolute value of every row of a 2D input (one peak per row, for example per channel).
 *
 * @details
 *          - The input is a matrix of dim_y rows with dim_x elements each. The row stride (stride_y, in bytes) may
 *            be larger than a row. The output holds one value of the same type per input row: out[r] is the largest
 *            absolute value of row r, so the output buffer has dim_x = dim_y of the input and dim_y = 1.
 *          - A single vector is the case dim_y = 1.
 *          - Following datatypes are supported:
 *             - DSPLIB_INT8, DSPLIB_INT16, DSPLIB_INT32, DSPLIB_INT64
 *             - DSPLIB_FLOAT32, DSPLIB_FLOAT64
 *          - Integer inputs equal to the most negative value of the type count as the most positive value of the
 *            type (saturating absolute value, as the C7x VABS instructions do), for example the absolute value of
 *            -128 is 127 for DSPLIB_INT8.
 *          - dim_x and dim_y of the input must be at least 1, and stride_y must be a multiple of the element size and
 *            at least dim_x elements.
 *          - Implementation (C7x): rows are read by two streaming engines (start and end of each row). Rows of up to
 *            4 vectors finish two rows per loop iteration; longer rows use an inner loop and one horizontal maximum
 *            per row. When there are at least 8 rows (4 for 64-bit types), the rows are 4-byte (8-byte for 64-bit
 *            types) aligned, the row length is a whole number of 4-byte (8-byte) units and the cycle model prefers
 *            it, a transposed stream is used instead: one vector holds the same column of 8 (4) rows, so the peaks
 *            of a whole group of rows are a vertical maximum with a single store and no horizontal maximum. The
 *            two streaming engines walk the groups of rows from the top and from the bottom. The cycle model
 *            includes the memory stalls measured on the EVM, which grow when the row stride is not a multiple of
 *            32 bytes, so the transposed variant is mainly chosen for many short or medium rows. The variant is
 *            chosen in @ref DSPLIB_peakAbsoluteValue_init and has no effect on the results.
 *
 * @ingroup  DSPLIB
 */
/**@{**/

/**
 * @brief Structure containing the parameters to initialize the kernel
 */
typedef struct {
   /** @brief Variant of the function refer to @ref DSPLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
} DSPLIB_peakAbsoluteValue_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_peakAbsoluteValue_getHandleSize(DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_peakAbsoluteValue_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the
 *               streaming engine.  The results of these operations are
 *               stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of the input buffer (dim_x elements per row, dim_y rows, stride_y bytes)
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of the output buffer (dim_x = dim_y of the input, dim_y = 1)
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init(DSPLIB_kernelHandle                      handle,
                                            DSPLIB_bufParams2D_t                    *bufParamsIn,
                                            DSPLIB_bufParams2D_t                    *bufParamsOut,
                                            const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_peakAbsoluteValue_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_peakAbsoluteValue_init, and this function
 *               must be called before the
 *               @ref DSPLIB_peakAbsoluteValue_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of the input buffer (dim_x elements per row, dim_y rows, stride_y bytes)
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of the output buffer (dim_x = dim_y of the input, dim_y = 1)
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_checkParams(DSPLIB_kernelHandle                      handle,
                                                        const DSPLIB_bufParams2D_t              *bufParamsIn,
                                                        const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                        const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_peakAbsoluteValue_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_peakAbsoluteValue_init, and this function
 *               must be called before the
 *               @ref DSPLIB_peakAbsoluteValue_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  pIn  :  Pointer to the structure input buffer
 *  @param [out] pout :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_checkParams(DSPLIB_kernelHandle handle,
                                          const void *restrict pIn1,
                                          const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_peakAbsoluteValue_exec
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  pIn  :  Pointer to the structure input buffer
 *  @param [out] pout :  Pointer to the output buffer
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
 *               @ref DSPLIB_peakAbsoluteValue_init and
 *               @ref DSPLIB_peakAbsoluteValue_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */

DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

void DSPLIB_peakAbsoluteValue_perfEst(DSPLIB_kernelHandle handle,
                                      uint64_t           *archCycles,
                                      uint64_t           *estCycles,
                                      uint32_t            dataType);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_peakAbsoluteValue_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  DSPLIB_peakAbsoluteValue.h                                              */
/* ======================================================================== */
