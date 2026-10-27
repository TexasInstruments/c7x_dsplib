// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_LOGICALNOT_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_LOGICALNOT_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_logicalNOT.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_logicalNOT_PrivArgs structure.
 *
 */

#define DSPLIB_logicalNOT_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_logicalNOT_exec_ci
 *         and @ref DSPLIB_logicalNOT_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_logicalNOT_exec)(DSPLIB_kernelHandle handle,
                                                    void *restrict pIn,
                                                    void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_logicalNOT_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_logicalNOT_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_logicalNOT_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_logicalNOT_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                                information of input buffer
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of ouput buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_logicalNOT_init_ci(DSPLIB_kernelHandle               handle,
                                        const DSPLIB_bufParams2D_t       *bufParamsIn,
                                        const DSPLIB_bufParams2D_t       *bufParamsOut,
                                        const DSPLIB_logicalNOT_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which computes the element-wise logical NOT
 *         (1 if the input is zero, 0 otherwise). The function declaration conforms to the declaration of
 *         @ref DSPLIB_logicalNOT_exec.
 *
 * @details Input data is loaded using a streaming engine, compared against zero, and the result is stored
 *          into memory using a stream address generator.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn          : Pointer to buffer holding the input buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_logicalNOT_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel which computes the element-wise logical NOT
 *         (1 if the input is zero, 0 otherwise). The function declaration conforms to the declaration of
 *         @ref DSPLIB_logicalNOT_exec.
 *
 * @details
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn          : Pointer to buffer holding the input buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_logicalNOT_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_logicalNOT_exec_cn and
    *         @ref DSPLIB_logicalNOT_exec_ci.                        */
   pFxnDSPLIB_logicalNOT_exec execute;

   /*! @brief Number of vector to be processed for the given input buffer */
   uint32_t nVecs;

   /*! @brief Element data type of the buffers, used by the performance estimate */
   int32_t dataType;

   uint32_t dim_x;     /**< \brief Width of buffer in X dimension in elements. */
   uint32_t dim_y;     /**< \brief Height of buffer in Y dimension in elements. */
   int32_t  strideIn;  /**< \brief Stride in Y dimension in bytes of inputs. */
   int32_t  strideOut; /**< \brief Stride in Y dimension in bytes of outputs. */

   /*! @brief bufPblock array to store SE/SA template */
   uint8_t bufPblock[DSPLIB_logicalNOT_IXX_IXX_OXX_PBLOCK_SIZE];

} DSPLIB_logicalNOT_PrivArgs;

#endif /* DSPLIB_LOGICALNOT_IXX_IXX_OXX_PRIV_H_ */
