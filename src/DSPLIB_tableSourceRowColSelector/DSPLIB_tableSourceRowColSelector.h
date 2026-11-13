// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_H_
#define DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_tableSourceRowColSelector DSPLIB_tableSourceRowColSelector
 * @brief This kernel copies the table row or column picked by each index to the output, interleaved or
 *        non-interleaved. It covers the AudioWeaver TableSource and TableSourceV2 modules.
 * @details
 *          - The table is a numRows x numCols row-major matrix: element (row, col) is at
 *            pIn1[row * numCols + col].
 *          - pIn0 holds numIndices int32_t indices. Each index picks one table vector
 *            (@ref DSPLIB_tableSourceRowColSelector_InitArgs::isColSelect) :
 *             - Row select : row idx, clamped to [0, numRows - 1]. selLen = numCols.
 *             - Column select : column idx, clamped to [0, numCols - 1]. selLen = numRows.
 *          - Element k of the vector for index i is written to
 *            (@ref DSPLIB_tableSourceRowColSelector_InitArgs::isOutputInterleave) :
 *             - Interleaved : pOut[k * strideOut + i] (dim_x = numIndices, dim_y = selLen)
 *             - Non-interleaved : pOut[i * strideOut + k] (dim_x = selLen, dim_y = numIndices)
 *            strideOut is bufParamsOut->stride_y in elements.
 *
 *          - Following datatypes are supported for the table and output :
 *             - DSPLIB_INT32, DSPLIB_INT64, DSPLIB_UINT32, DSPLIB_UINT64, DSPLIB_FLOAT32, DSPLIB_FLOAT64
 *          - The index buffer is always DSPLIB_INT32.
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
   /** @brief Flag indicating if an index selects a table column instead of a table row */
   uint8_t isColSelect;
   /** @brief Flag indicating if the output is in interleaved or non-interleaved format */
   uint8_t isOutputInterleave;
} DSPLIB_tableSourceRowColSelector_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_tableSourceRowColSelector_getHandleSize(DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_tableSourceRowColSelector_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as the
 *               streaming engine and streaming address generator.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                               information of index buffer (dim_x = numIndices)
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                               information of table buffer (dim_x = numCols, dim_y = numRows,
 *                               stride_y = numCols * element size)
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output buffer (dim_x = numIndices, dim_y = selLen for
 *                               interleaved, dim_x = selLen, dim_y = numIndices for non-interleaved)
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_init(DSPLIB_kernelHandle                              handle,
                                                    DSPLIB_bufParams1D_t                            *bufParamsIn0,
                                                    DSPLIB_bufParams2D_t                            *bufParamsIn1,
                                                    DSPLIB_bufParams2D_t                            *bufParamsOut,
                                                    const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_tableSourceRowColSelector_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_tableSourceRowColSelector_init, and this function
 *               must be called before the
 *               @ref DSPLIB_tableSourceRowColSelector_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                               information of index buffer (dim_x = numIndices)
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                               information of table buffer (dim_x = numCols, dim_y = numRows,
 *                               stride_y = numCols * element size)
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output buffer (dim_x = numIndices, dim_y = selLen for
 *                               interleaved, dim_x = selLen, dim_y = numIndices for non-interleaved)
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_init_checkParams(DSPLIB_kernelHandle                              handle,
                                                  const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                                  const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                                  const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                                  const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_tableSourceRowColSelector_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_tableSourceRowColSelector_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_tableSourceRowColSelector_exec is called.
 *
 *  @param [in]  handle   :  Active handle to the kernel
 *  @param [in]  pIn0     :  Pointer to the index buffer
 *  @param [in]  pIn1     :  Pointer to the table buffer
 *  @param [out] pOut     :  Pointer to the output buffer
 *  @param [in]  pScratch :  Pointer to the scratch buffer, may be NULL for the non-interleaved output
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     The handle must be initialized by @ref DSPLIB_tableSourceRowColSelector_init.
 */
DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_exec_checkParams(DSPLIB_kernelHandle handle,
                                                  const void *restrict pIn0,
                                                  const void *restrict pIn1,
                                                  const void *restrict pOut,
                                                  const void *restrict pScratch);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_tableSourceRowColSelector
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn0        : Pointer to buffer holding the indices
 *  @param [in]  pIn1        : Pointer to buffer holding the table
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *  @param [in]  pScratch    : Pointer to the scratch buffer, may be NULL for the non-interleaved output
 *
 *  @par Memory Requirements
 * | Buffer   | dimY                       | dimX                   | Comments |
 * | :------- | :--:                       | :--:                   | :------- |
 * | pScratch | roundup(numIndices, lanes) | roundup(selLen, lanes) | Selected vectors of all indices, used only by the
 * interleaved output (lanes = __C7X_VEC_SIZE_BYTES__ / element size) |
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Assumptions:
 *    - pOut does not overlap pIn0 or pIn1. In-place processing is not allowed.
 *    - pScratch does not overlap pIn0, pIn1 or pOut.
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers, including pScratch, are assumed to be 64-byte aligned
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref DSPLIB_tableSourceRowColSelector_init and
 *               @ref DSPLIB_tableSourceRowColSelector_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only once for several invocations of this
 * function.
 */

DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec(DSPLIB_kernelHandle handle,
                                                    void *restrict pIn0,
                                                    void *restrict pIn1,
                                                    void *restrict pOut,
                                                    void *restrict pScratch);

/**
 *  @brief        This function is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [in]  archCycles     :  Arch cycles used in that particular kernel
 *  @param [in]  estCycles      :  Cycles estimated for that particular kernel
 *
 *  @return      Void.
 *
 *  @remarks     None
 */
void DSPLIB_tableSourceRowColSelector_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_H_ */
