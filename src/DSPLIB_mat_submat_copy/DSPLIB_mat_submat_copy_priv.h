// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_MAT_SUBMAT_COPY_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_MAT_SUBMAT_COPY_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_mat_submat_copy.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_mat_submat_copy_PrivArgs structure.
 *
 */
#define DSPLIB_MAT_SUBMAT_COPY_IXX_IXX_OXX_PBLOCK_SIZE (4 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_mat_submat_copy_exec_ci
 *         and @ref DSPLIB_mat_submat_copy_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_mat_submat_copy_exec)(DSPLIB_kernelHandle handle,
                                                         void *restrict pX,
                                                         void *restrict pY);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_mat_submat_copy_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_mat_submat_copy_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_mat_submat_copy_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_mat_submat_copy_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

template <typename dataType>
extern DSPLIB_STATUS DSPLIB_mat_submat_copy_init_ci(DSPLIB_kernelHandle                   handle,
                                                    const DSPLIB_bufParams2D_t           *bufParamsX,
                                                    const DSPLIB_bufParams2D_t           *bufParamsY,
                                                    const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_mat_submat_copy_exec.
 *
 * @details The function uses streaming engine to load data in TRANSPOSE MODE. The result
 *           is then stored at separate location using a streaming address generator.
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pX         : Pointer to buffer holding the X matrix
 *  @param [out] pY         : Pointer to buffer holding the Y matrix
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 */

template <typename dataType>
extern DSPLIB_STATUS DSPLIB_mat_submat_copy_exec_ci(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_mat_submat_copy_init.
 *
 * @details
 *
 *  @param [in]  handle               :  Active handle to the kernel
 *  @param [in]  bufParamsX           :  Pointer to the structure containing dimensional
 *                                       information of input buffer
 *  @param [out] bufParamsY           :  Pointer to the structure containing dimensional
 *                                       information of ouput buffer
 *  @param [in]  pKerInitArgs         :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

DSPLIB_STATUS DSPLIB_mat_submat_copy_init_cn(DSPLIB_kernelHandle                   handle,
                                             DSPLIB_bufParams2D_t                 *bufParamsX,
                                             DSPLIB_bufParams2D_t                 *bufParamsY,
                                             const DSPLIB_mat_submat_copyInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_mat_submat_copy_exec.
 *
 * @details
 *
 *  @param [in]  handle    : Active handle to the kernel
 *  @param [in]  pX        : Pointer to buffer holding the X matrix
 *  @param [out] pY        : Pointer to buffer holding the Y matrix
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_mat_submat_copy_exec_cn(DSPLIB_kernelHandle handle, void *restrict pX, void *restrict pY);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_mat_submat_copy_exec_cn and
    *         @ref DSPLIB_mat_submat_copy_exec_ci.                        */
   pFxnDSPLIB_mat_submat_copy_exec execute;
   /*! @brief Size of input buffer for different batches
    *         @ref DSPLIB_mat_submat_copy_init that will be retrieved
    *         and used by @ref DSPLIB_mat_submat_copy_exec                */
   /*! @brief Width of input data matrix                           */
   uint32_t widthX;
   /*! @brief Height of input data matrix                          */
   uint32_t heightX;
   /*! @brief Stride between rows of input data matrix             */
   uint32_t strideX;
   /*! @brief Stride between rows of output data matrix            */
   uint32_t strideY;
   /*! @brief Data flow direction                                  */
   uint32_t dir;
   /*! @brief Copying starting from stRow row of matrix            */
   uint32_t stRow;
   /*! @brief Copying starting from stCol col of matrix            */
   uint32_t stCol;
   /*! @brief The number of rows to be copied                      */
   uint32_t nRows;
   /*! @brief The number of cols to be copied                      */
   uint32_t nCols;
   /*! @brief Buffer to save SE & SA configuration parameters      */
   uint8_t bufPblock[DSPLIB_MAT_SUBMAT_COPY_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_mat_submat_copy_PrivArgs;

#endif /* DSPLIB_mat_submat_copy_IXX_IXX_OXX_PRIV_H_ */
