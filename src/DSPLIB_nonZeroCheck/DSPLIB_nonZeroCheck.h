// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_nonZeroCheck_IXX_IXX_OXX_H_
#define DSPLIB_nonZeroCheck_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_nonZeroCheck DSPLIB_nonZeroCheck
 * @brief Kernel for logical reduction operations on 2D arrays.
 *
 * This kernel takes a 2D matrix with dim_x columns (width) and dim_y rows (height),
 * performs a logical reduction operation (ALL or ANY) along the specified axis, and
 * produces a 1D output of size dim_x (ROW axis, reduction over the dim_y rows) or
 * dim_y (COLUMN axis, reduction over the dim_x elements of each row). Each output
 * element is 1 if the condition is met for that reduction, 0 otherwise.
 *
 * <b>Operation type (opType): </b>
 *
 * - **ALL (opType=0):** Returns 1 if ALL elements in the reduction are non-zero, else 0
 * - **ANY (opType=1):** Returns 1 if ANY element in the reduction is non-zero, else 0
 *
 * <b>Axis type (axisType): </b>
 *
 * - **ROW (axisType=0):** Reduce down the rows (over dim_y) for each column position;
 *   output has dim_x elements, out[j] = reduce over i of in[i][j]
 * - **COLUMN (axisType=1):** Reduce across the columns (over dim_x) of each row;
 *   output has dim_y elements, out[i] = reduce over j of in[i][j]
 *
 * @par Example:
 *
 * Input Matrix (dim_y = 3 rows, dim_x = 4 columns):\n
 * \f[
 * \text{In} =
 * \begin{bmatrix}
 * 5 & 0 & 3 & 7 \\
 * 0 & 2 & 0 & 0 \\
 * 8 & 4 & 1 & 6 \\
 * \end{bmatrix}
 * \f]
 *
 * <b>Case 1: opType = ALL, axisType = ROW</b>\n
 * Output (dim_x = 4 elements): [0, 0, 0, 0]\n
 * (Every column contains a zero)
 *
 * <b>Case 2: opType = ANY, axisType = ROW</b>\n
 * Output (dim_x = 4 elements): [1, 1, 1, 1]\n
 * (Every column has at least one non-zero element)
 *
 * <b>Case 3: opType = ALL, axisType = COLUMN</b>\n
 * Output (dim_y = 3 elements): [0, 0, 1]\n
 * (Row 0 has a zero; Row 1 has zeros; Row 2 is all non-zero)
 *
 * <b>Case 4: opType = ANY, axisType = COLUMN</b>\n
 * Output (dim_y = 3 elements): [1, 1, 1]\n
 * (Every row has at least one non-zero element)
 *
 * @note Supported input data types:
 *       - int8_t, uint8_t
 *       - int16_t, uint16_t
 *       - int32_t, uint32_t
 *       - int64_t, uint64_t
 *       - float
 *       - double
 *
 * @note The output data type is the same as the input data type; each output
 *       element holds 1 (1.0 for float/double) or 0 (0.0).
 *
 * @note An element is zero when it compares equal to zero, so -0.0 is zero and
 *       NaN is non-zero, in the natural C and the optimized variant alike.
 *
 * @par Preconditions:
 *   - dim_x >= 1 and dim_y >= 1
 *   - stride_y >= dim_x * element size (bytes)
 *   - the output buffer has dim_x elements (ROW axis) or dim_y elements
 *     (COLUMN axis), and bufParamsOut->dim_x says so
 *   - the input and output buffers do not overlap (the exec pointers are
 *     restrict)
 *
 * @ref DSPLIB_nonZeroCheck_init_checkParams returns
 *   - DSPLIB_ERR_NULL_POINTER for a NULL handle or buffer parameter pointer
 *   - DSPLIB_ERR_INVALID_TYPE for an unsupported input type or an output type
 *     different from the input type
 *   - DSPLIB_ERR_INVALID_VALUE for an opType or axisType out of range
 *   - DSPLIB_ERR_INVALID_DIMENSION when a dimension precondition above fails
 *
 * ### Implementation Details
 *
 * The optimized variant reads the input with both streaming engines (SE0 and
 * SE1) in lockstep, each delivering one vector per cycle, and stores with two
 * streaming address generators (SA0 and SA1). SE0/SA0 produce the first half
 * of the outputs and SE1/SA1 the second half; for an odd output count the two
 * halves overlap by one output, which both compute and store with the same
 * value, so neither stream needs a tail pass. Every element is compared with
 * zero and the compare result is folded into a per-lane vector with a select,
 * so the inner loops have no branches and run at 2 vectors per cycle for every
 * data type.
 *
 * - **ROW axis:** each stream reads whole column blocks of eleCount columns
 *   (a 3D stream; DECDIM zero-pads the lanes past dim_x of the last block) and
 *   the folded vector is the output block, stored with the SA predicate.
 * - **COLUMN axis:** each stream reads whole rows (a 4D stream; DECDIM pads
 *   the lanes past dim_x of the last vector of every row, with zero for ANY and
 *   with the maximum value for ALL) and the folded vector is reduced to the
 *   row's scalar with a predicate bit count.
 *
 * Each stream produces one or two adjacent outputs (column blocks or rows) per
 * outer iteration. @ref DSPLIB_nonZeroCheck_init picks whichever the cycle
 * model used by @ref DSPLIB_nonZeroCheck_perfEst predicts to be faster for the
 * given dimensions.
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
   /** @brief Selects the logical reduction operation (ALL=0, ANY=1), see @ref DSPLIB_nonZeroCheck_opType    */
   uint32_t opType;
   /** @brief Selects the reduction axis (ROW=0, COLUMN=1), see @ref DSPLIB_nonZeroCheck_axisType    */
   uint32_t axisType;
} DSPLIB_nonZeroCheck_InitArgs;

/**
 * @brief Enum defining the logical reduction operation
 */
typedef enum {
   DSPLIB_NONZEROCHECK_ALL = 0, /* ALL */
   DSPLIB_NONZEROCHECK_ANY = 1, /* ANY */
} DSPLIB_nonZeroCheck_opType;

/**
 * @brief Enum defining the axis along which the reduction is performed
 */
typedef enum {
   DSPLIB_NONZEROCHECK_ROW    = 0, /* Operate on rows */
   DSPLIB_NONZEROCHECK_COLUMN = 1, /* Operate on columns */
} DSPLIB_nonZeroCheck_axisType;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_nonZeroCheck_getHandleSize(DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_nonZeroCheck_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as
 *               the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input0 buffer
 *  @param [in]  bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of output buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS. Without a prior @ref DSPLIB_nonZeroCheck_init_checkParams,
 * DSPLIB_ERR_INVALID_TYPE is returned for an unsupported data type and
 * DSPLIB_ERR_INVALID_VALUE for an opType or axisType out of range; the
 * dimension checks are done only by @ref DSPLIB_nonZeroCheck_init_checkParams.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */

DSPLIB_STATUS DSPLIB_nonZeroCheck_init(DSPLIB_kernelHandle                 handle,
                                       DSPLIB_bufParams2D_t               *bufParamsIn0,
                                       DSPLIB_bufParams1D_t               *bufParamsOut,
                                       const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_nonZeroCheck_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_nonZeroCheck_init, and this function
 *               must be called before the
 *               @ref DSPLIB_nonZeroCheck_init is called.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input0 buffer
 *  @param [in]  bufParamsOut  :  Pointer to the structure containing dimensional
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
DSPLIB_nonZeroCheck_init_checkParams(DSPLIB_kernelHandle                 handle,
                                     const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                     const DSPLIB_bufParams1D_t         *bufParamsOut,
                                     const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_nonZeroCheck_exec function. This function
 *               is called with the same parameters as
 *               @ref DSPLIB_nonZeroCheck_exec, and it
 *               must be called before @ref DSPLIB_nonZeroCheck_exec
 *               is called. It verifies that the handle and both buffer pointers are not NULL.
 *
 *  @param [in]  handle    :  Active handle to the kernel
 *  @param [in]  pIn0      :  Pointer to the structure input0 buffer
 *  @param [out] pOut      :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */

DSPLIB_STATUS
DSPLIB_nonZeroCheck_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pIn0, const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Reduces the input buffer along the axis selected at init
 *               (see @ref DSPLIB_nonZeroCheck_axisType) using the selected
 *               operation (see @ref DSPLIB_nonZeroCheck_opType), writing 1 or 0
 *               (same data type as the input) for each output element.
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pIn0       : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut       : Pointer to buffer holding the output buffer
 *
 * @par Memory Requirements
 * | Buffer | dimY  | dimX                                | Comments |
 * | :----- | :---: | :---------------------------------: | :------- |
 * | pIn0   | dim_y | dim_x (row pitch stride_y bytes)    | Input matrix |
 * | pOut   | 1     | dim_x (ROW axis) / dim_y (COLUMN axis) | One 1/0 result per reduction, input data type |
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
 *               @ref DSPLIB_nonZeroCheck_init_checkParams,
 *               @ref DSPLIB_nonZeroCheck_init and
 *               @ref DSPLIB_nonZeroCheck_exec_checkParams.
 *               This ensures resource configuration and error checks are done
 *               only once for several invocations of this function.
 */

DSPLIB_STATUS
DSPLIB_nonZeroCheck_exec(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/**
 *  @brief        This function is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @details      The model is outer iterations x (inner count x ii +
 *                overhead) for one and for two outputs per stream; the faster
 *                of the two is the variant @ref DSPLIB_nonZeroCheck_init runs.
 *                The function records that choice in the handle, and
 *                @ref DSPLIB_nonZeroCheck_init calls it to make the selection.
 *
 *  @param [in]  handle         :  Active handle to the kernel, after
 *                                 @ref DSPLIB_nonZeroCheck_init
 *  @param [out] archCycles     :  Modelled cycles of the optimized exec loops
 *  @param [out] estCycles      :  archCycles plus the per-call overhead
 *
 *  @remarks     None
 */
void DSPLIB_nonZeroCheck_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_nonZeroCheck_IXX_IXX_OXX_H_ */
