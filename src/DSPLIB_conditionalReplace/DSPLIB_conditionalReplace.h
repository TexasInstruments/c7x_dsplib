// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_conditionalReplace_IXX_IXX_OXX_H_
#define DSPLIB_conditionalReplace_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DSPLIB_conditionalReplace DSPLIB_conditionalReplace
 * @brief Kernel for per-channel conditional replacement of integer and floating point data.
 *
 * This kernel takes one matrix of size NxM, where each row is a channel of M samples, and compares every
 * sample of a channel against a per-channel constant. If the comparison is true the sample is replaced by a
 * per-channel replacement value, otherwise the input sample is copied to the output.
 *
 * \f[
 * \text{Out}[c][i] = \begin{cases} \text{replaceValue}[c] & \text{if } \text{In}[c][i] \;\text{op}_c\;
 * \text{constValue}[c] \\
 * \text{In}[c][i] & \text{otherwise} \end{cases}
 * \f]
 *
 * where \f$\text{op}_c\f$ is selected by compareType[c], refer to @ref DSPLIB_conditionalReplace_compareType :
 *
 * | compareType                            | Value | Operation                  |
 * |----------------------------------------|-------|----------------------------|
 * | @ref DSPLIB_CONDITIONALREPLACE_EQ | 0     | Equal (==)                 |
 * | @ref DSPLIB_CONDITIONALREPLACE_NE | 1     | Not equal (!=)             |
 * | @ref DSPLIB_CONDITIONALREPLACE_LT | 2     | Less than (<)              |
 * | @ref DSPLIB_CONDITIONALREPLACE_LE | 3     | Less than or equal (<=)    |
 * | @ref DSPLIB_CONDITIONALREPLACE_GT | 4     | Greater than (>)           |
 * | @ref DSPLIB_CONDITIONALREPLACE_GE | 5     | Greater than or equal (>=) |
 *
 * A compareType outside 0..5 is rejected by @ref DSPLIB_conditionalReplace_set.
 *
 * @par Example:
 *
 * Input Matrix (2x4), compareType = {4, 0}, constValue = {10, 3}, replaceValue = {-1, 99}:
 * \f[
 * \text{In} =
 * \begin{bmatrix}
 * 5 & 12 & 10 & 40 \\
 * 3 & 7 & 3 & 1 \\
 * \end{bmatrix}
 * \quad
 * \text{Output} =
 * \begin{bmatrix}
 * 5 & -1 & 10 & -1 \\
 * 99 & 7 & 99 & 1 \\
 * \end{bmatrix}
 * \f]
 * @note Supported data types (input, output, constValue and replaceValue share the same data type, compareType is
 *       always int32_t):
 *       - int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double
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
} DSPLIB_conditionalReplace_InitArgs;

/**
 * @brief Compare operators supported by the kernel, used as per-channel values of the compare type array
 */
typedef enum {
   DSPLIB_CONDITIONALREPLACE_EQ = 0, /**< Equal (==) */
   DSPLIB_CONDITIONALREPLACE_NE = 1, /**< Not Equal (!=) */
   DSPLIB_CONDITIONALREPLACE_LT = 2, /**< Less Than (<) */
   DSPLIB_CONDITIONALREPLACE_LE = 3, /**< Less Than or Equal (<=) */
   DSPLIB_CONDITIONALREPLACE_GT = 4, /**< Greater Than (>) */
   DSPLIB_CONDITIONALREPLACE_GE = 5  /**< Greater Than or Equal (>=) */
} DSPLIB_conditionalReplace_compareType;

/**
 * @brief Structure containing the parameters to set the kernel
 */
typedef struct {
   /** @brief Pointer to dim_y int32_t compare types (of type @ref DSPLIB_conditionalReplace_compareType), one per
    * channel. The array is not copied: it must remain valid and unchanged until it is replaced by another call of @ref
    * DSPLIB_conditionalReplace_set. */
   const int32_t *pCompareType;
   /** @brief Pointer to dim_y constants the input is compared against, one per channel. The elements have the same
    * data type as the input buffer. The array is not copied: it must remain valid and unchanged until it is replaced
    * by another call of @ref DSPLIB_conditionalReplace_set. */
   const void *pConstValue;
   /** @brief Pointer to dim_y replacement values, one per channel. The elements have the same data type as the input
    * buffer. The array is not copied: it must remain valid and unchanged until it is replaced by another call of @ref
    * DSPLIB_conditionalReplace_set. */
   const void *pReplaceValue;
} DSPLIB_conditionalReplace_SetArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_conditionalReplace_getHandleSize(DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref DSPLIB_conditionalReplace_exec function is called. This
 *               function takes care of any one-time operations such as setting
 *               up the configuration of required hardware resources such as
 *               the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn   :  Pointer to the structure containing dimensional
 *                                information of input buffer. dim_y is the number of channels.
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
DSPLIB_STATUS DSPLIB_conditionalReplace_init(DSPLIB_kernelHandle                       handle,
                                             DSPLIB_bufParams2D_t                     *bufParamsIn,
                                             DSPLIB_bufParams2D_t                     *bufParamsOut,
                                             const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

/**
 *  @brief       Sets the per-channel compare types, compare constants and replacement values used by the kernel.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pKerSetArgs : Pointer to the structure holding the set parameters
 *
 *  @details     This function validates the pointers and stores the set parameters in the handle.
 *               The compareType, constValue and replaceValue arrays themselves are not copied, so they must stay
 *               valid until the next call of this function. Compare types outside 0..5 are rejected with
 *               DSPLIB_ERR_INVALID_VALUE and the previous parameters are kept.
 *               Each of the three arrays must contain at least dim_y elements, where dim_y is the number of channels
 *               (bufParamsIn->dim_y) given to @ref DSPLIB_conditionalReplace_init. This function and
 *               @ref DSPLIB_conditionalReplace_exec read dim_y elements from each array, and the array
 *               length cannot be checked, so shorter arrays cause out of bounds reads.
 *               It can be called any number of times between calls of
 *               @ref DSPLIB_conditionalReplace_exec, which allows the compare types, constants and
 *               replacement values to be changed at runtime.
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Must be called after @ref DSPLIB_conditionalReplace_init and before
 *               @ref DSPLIB_conditionalReplace_exec.
 */
DSPLIB_STATUS DSPLIB_conditionalReplace_set(DSPLIB_kernelHandle                      handle,
                                            const DSPLIB_conditionalReplace_SetArgs *pKerSetArgs);

/**
 *  @brief       Retrieves the parameters last passed to @ref DSPLIB_conditionalReplace_set.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [out] pKerSetArgs  : Pointer to the structure where the current parameters are stored
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 */
DSPLIB_STATUS DSPLIB_conditionalReplace_get(DSPLIB_kernelHandle handle, DSPLIB_conditionalReplace_SetArgs *pKerSetArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_conditionalReplace_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_conditionalReplace_init, and this function
 *               must be called before the
 *               @ref DSPLIB_conditionalReplace_init is called.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn   :  Pointer to the structure containing dimensional
 *                                information of input buffer
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
DSPLIB_conditionalReplace_init_checkParams(DSPLIB_kernelHandle                       handle,
                                           const DSPLIB_bufParams2D_t               *bufParamsIn,
                                           const DSPLIB_bufParams2D_t               *bufParamsOut,
                                           const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_conditionalReplace_exec function.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  pIn           :  Pointer to the input buffer
 *  @param [out] pOut          :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_conditionalReplace_exec_checkParams(DSPLIB_kernelHandle handle,
                                                         const void *restrict pIn,
                                                         const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @param [in]  handle        : Active handle to the kernel
 *  @param [in]  pIn           : Pointer to buffer holding the input buffer (dim_y channels)
 *  @param [out] pOut          : Pointer to buffer holding the output buffer
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
 *    - rows (channels) of at least one full vector of samples amortize the per-channel setup
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref DSPLIB_conditionalReplace_init and
 *               @ref DSPLIB_conditionalReplace_set.
 *               This ensures resource configuration and error checks are done
 *               only once for several invocations of this function.
 */
DSPLIB_STATUS DSPLIB_conditionalReplace_exec(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

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
void DSPLIB_conditionalReplace_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_conditionalReplace_IXX_IXX_OXX_H_ */
