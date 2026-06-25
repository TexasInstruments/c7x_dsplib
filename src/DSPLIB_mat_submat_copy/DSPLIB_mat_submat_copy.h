// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_MAT_SUBMAT_COP_IXX_IXX_OXX_H_
#define DSPLIB_MAT_SUBMAT_COP_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_mat_submat_copy DSPLIB_mat_submat_copy
 * @brief Kernel for sub matrix copy.
 *
 * @details
 *          - This kernel copies a sub-matrix from one matrix to another.
 *          - This kernel provides functionaly to set direction of copy. See @ref DSPLIB_mat_submat_copyInitArgs.dir
 *               - If @ref DSPLIB_mat_submat_copyInitArgs.dir = 0, copy 'nRows' number of rows and 'nCols' number of
 *                 columns starting from 'stRow' th row and 'stCols' th column from the
 *                 matrix  \f$X\f$ to the 0th index of a matrix  \f$Y\f$.
 *               - If @ref DSPLIB_mat_submat_copyInitArgs.dir = 1, copy 'nRows' number of rows and 'nCols' number of
 *                 columns starting from 0th index of a  matrix  \f$Y\f$ into 'stRow' th row and 'stCols' th column of
 *                 matrix  \f$X\f$.
 *          - Matrix \f$X\f$ is of the shape (M, N).
 *          - Matrix \f$Y\f$ is of the shape (nRows, nCols).
 *          - Following datatypes are supported :
 *            DSPLIB_INT8, DSPLIB_INT16, DSPLIB_INT32,DSPLIB_INT64,
 *            DSPLIB_UINT8, DSPLIB_UINT16, DSPLIB_UINT32, DSPLIB_UINT64,
 *            DSPLIB_FLOAT32, DSPLIB_FLOAT64
 *
 * @ingroup  DSPLIB
 */
/**@{*/

/**
 * @brief Structure containing the parameters to initialize the kernel
 */
typedef struct {
   /*! @brief Variant of the function refer to @ref DSPLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /*! @brief The number of columns in X matrix */
   uint32_t dimX;
   /*! @brief The number of rows in X matrix */
   uint32_t dimY;
   /*! @brief Direction of the data flow                                      */
   uint32_t dir;
   /*! @brief Copying starting from stRow row of matrix            */
   uint32_t stRow;
   /*! @brief Copying starting from stCol column of matrix            */
   uint32_t stCol;
   /*! @brief The number of rows to be copied                                 */
   uint32_t nRows;
   /*! @brief The number of coulmns to be copied                      */
   uint32_t nCols;
} DSPLIB_mat_submat_copyInitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_mat_submat_copy_getHandleSize(DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_mat_submat_copy_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the
 *               streaming engine and streaming address generator. The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of X matrix
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of Y matrix
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_mat_submat_copy_init(DSPLIB_kernelHandle                   handle,
                                          DSPLIB_bufParams2D_t                 *bufParamsX,
                                          DSPLIB_bufParams2D_t                 *bufParamsY,
                                          const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_mat_submat_copy_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_mat_submat_copy_init, and this function
 *               must be called before the
 *               @ref DSPLIB_mat_submat_copy_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of X matrix
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of Y matrix
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_mat_submat_copy_init_checkParams(DSPLIB_kernelHandle                   handle,
                                                      const DSPLIB_bufParams2D_t           *bufParamsX,
                                                      const DSPLIB_bufParams2D_t           *bufParamsY,
                                                      const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_mat_submat_copy_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_mat_submat_copy_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_mat_submat_copy_exec is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pX         : Pointer to buffer holding the X matrix
 *  @param [out] pY         : Pointer to buffer holding the Y matrix
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS
DSPLIB_mat_submat_copy_exec_checkParams(DSPLIB_kernelHandle handle, const void *restrict pX, const void *restrict pY);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_mat_submat_copy_exec
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pX         : Pointer to buffer holding the X matrix
 *  @param [out] pY         : Pointer to buffer holding the Y matrix
 *
 * @par Memory Requirements
 * | Buffer | dimY | dimX | Comments |
 * | :----- | :--: | :--: | :-----|
 * | pX     | M    | N    | X Matrix |
 * | pY     | nRows | nCols | Y Matrix |
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Assumptions:
 *    - None
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - The input and output data buffers are expected to be in L2 memory
 *    - The buffer pointers are assumed to be 64-byte aligned
 *    - Ensure that the starting point of adjacent rows in a 2-D matrix are allocated from
 *      different banks in L2 memory with the help of memory padding.
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref DSPLIB_mat_submat_copy_init and
 *               @ref DSPLIB_mat_submat_copy_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */

DSPLIB_STATUS DSPLIB_mat_submat_copy_exec(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_MAT_SUBMAT_COP_IXX_IXX_OXX_H_ */
