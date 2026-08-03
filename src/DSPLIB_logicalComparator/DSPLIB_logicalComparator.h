// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_logicalComparator_IXX_IXX_OXX_H_
#define DSPLIB_logicalComparator_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_logicalComparator DSPLIB_logicalComparator
 * @brief Kernel for multichannel comparison using relational operators.
 *
 * This kernel takes two matrices of size NxM, performs a specified relational operation and produces an output
 * matrix of size NxM which will be having 0's or 1's or both.
 *
 * <b>Operation type(opType): </b>
 *
 *  |  opType |         Operation         |
 *  |---------|---------------------------|
 *  |    0    |          equal to         |
 *  |    1    |        not equal to       |
 *  |    2    |        less than          |
 *  |    3    |   less than or equal to   |
 *  |    4    |        greater than       |
 *  |    5    |  greater than or equal to |
 *
 * @par Example:
 *
 * Input0 Matrix (3x3):
 * \f[
 * \text{In0} =
 * \begin{bmatrix}
 * 23 & 146 & 179 \\
 * 57 & 131 & 29 \\
 * 38 & 43 & 88 \\
 * \end{bmatrix}
 * \f]
 *
 * Input1 Matrix (3x3):
 * \f[
 * \text{In1} =
 * \begin{bmatrix}
 * 64 & 75 & 119 \\
 * 12 & 48 & 32 \\
 * 21 & 96 & 88 \\
 * \end{bmatrix}
 * \f]
 *
 * opType = 3.
 *
 * Output Matrix (3x3):
 * \f[
 * \text{Output} =
 * \begin{bmatrix}
 * 1 & 0 & 0 \\
 * 0 & 0 & 1 \\
 * 0 & 1 & 1 \\
 * \end{bmatrix}
 * \f]
 *
 * @note The input matrices `in0` and `in1` must have the same dimensions.
 * @note Supported input data types:
 *       - int8_t
 *       - uint8_t
 *       - int16_t
 *       - uint16_t
 *       - int32_t
 *       - uint32_t
 *       - int64_t
 *       - uint64_t
 *       - float
 *       - double
 *
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
} DSPLIB_logicalComparator_InitArgs;

/**
 * @brief Structure containing the parameters to set the kernel
 */
typedef struct {
   /** @brief Selects the relational operator to compute, refer to @ref DSPLIB_logicalComparator_opType. */
   uint32_t opType;
} DSPLIB_logicalComparator_SetArgs;

/**
 * @brief Relational operators supported by the kernel
 */
typedef enum {
   DSPLIB_LOGICALCOMPARATOR_EQ = 0, /**< Equal (==) */
   DSPLIB_LOGICALCOMPARATOR_NE = 1, /**< Not Equal (!=) */
   DSPLIB_LOGICALCOMPARATOR_LT = 2, /**< Less Than (<) */
   DSPLIB_LOGICALCOMPARATOR_LE = 3, /**< Less Than or Equal (<=) */
   DSPLIB_LOGICALCOMPARATOR_GT = 4, /**< Greater Than (>) */
   DSPLIB_LOGICALCOMPARATOR_GE = 5  /**< Greater Than or Equal (>=) */
} DSPLIB_logicalComparator_opType;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_logicalComparator_getHandleSize(DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_logicalComparator_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as
 *               the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input0 buffer
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                                information of input1 buffer
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of output buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */

DSPLIB_STATUS DSPLIB_logicalComparator_init(DSPLIB_kernelHandle                      handle,
                                            DSPLIB_bufParams2D_t                    *bufParamsIn0,
                                            DSPLIB_bufParams2D_t                    *bufParamsIn1,
                                            DSPLIB_bufParams2D_t                    *bufParamsOut,
                                            const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

/**
 *  @brief       Sets the relational operator used by the kernel.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pKerSetArgs : Pointer to the structure holding the set parameters
 *
 *  @details     This function validates the parameters, stores a copy of them in
 *               the handle and selects the execution variant used by
 *               @ref DSPLIB_logicalComparator_exec. It can be called any number of
 *               times between calls of @ref DSPLIB_logicalComparator_exec to change the operator.
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Must be called after @ref DSPLIB_logicalComparator_init and before
 *               @ref DSPLIB_logicalComparator_exec.
 */
DSPLIB_STATUS DSPLIB_logicalComparator_set(DSPLIB_kernelHandle                     handle,
                                           const DSPLIB_logicalComparator_SetArgs *pKerSetArgs);

/**
 *  @brief       Retrieves the parameters last passed to @ref DSPLIB_logicalComparator_set.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [out] pKerSetArgs  : Pointer to the structure where the current parameters are stored
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 */
DSPLIB_STATUS DSPLIB_logicalComparator_get(DSPLIB_kernelHandle handle, DSPLIB_logicalComparator_SetArgs *pKerSetArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_logicalComparator_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_logicalComparator_init, and this function
 *               must be called before the
 *               @ref DSPLIB_logicalComparator_init is called.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input0 buffer
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                                information of input1 buffer
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
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
DSPLIB_logicalComparator_init_checkParams(DSPLIB_kernelHandle                      handle,
                                          const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                          const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                          const DSPLIB_bufParams2D_t              *bufParamsOut,
                                          const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_logicalComparator_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_logicalComparator_init, and this function
 *               must be called before the
 *               @ref DSPLIB_logicalComparator_init is called.
 *
 *  @param [in]  handle    :  Active handle to the kernel
 *  @param [in]  pIn0      :  Pointer to the structure input0 buffer
 *  @param [in]  pIn1      :  Pointer to the structure input1 buffer
 *  @param [out] pOut      :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */

DSPLIB_STATUS DSPLIB_logicalComparator_exec_checkParams(DSPLIB_kernelHandle handle,
                                                        const void *restrict pIn0,
                                                        const void *restrict pIn1,
                                                        const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_logicalComparator_exec
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pIn0       : Pointer to buffer holding the input0 buffer
 *  @param [in]  pIn1       : Pointer to buffer holding the input1 buffer
 *  @param [out] pOut       : Pointer to buffer holding the output buffer
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
 *               @ref DSPLIB_logicalComparator_init and
 *               This ensures resource configuration and error checks are done
 *               only once for several invocations of this function.
 */

DSPLIB_STATUS
DSPLIB_logicalComparator_exec(DSPLIB_kernelHandle handle,
                              void *restrict pIn0,
                              void *restrict pIn1,
                              void *restrict pOut);

/**
 *  @brief        This funtion is called to calculate the arch cycles and
 *                estimate cycles of the loop used in the execution kernel.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [in]  archCycles     :  Arch cycles used in that particluar kernel
 *  @param [in]  estCycles      :  Cycles estimated for that particular kenel
 *
 *  @remarks     None
 */
void DSPLIB_logicalComparator_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_logicalComparator_IXX_IXX_OXX_H_ */
