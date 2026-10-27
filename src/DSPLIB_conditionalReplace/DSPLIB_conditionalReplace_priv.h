// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_CONDITIONALREPLACE_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_CONDITIONALREPLACE_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_conditionalReplace.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_conditionalReplace_PrivArgs structure.
 *
 */

#define DSPLIB_conditionalReplace_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_conditionalReplace_exec_ci
 *         and @ref DSPLIB_conditionalReplace_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_conditionalReplace_exec)(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_conditionalReplace_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and stream address generator, and the configuration is saved in the
 *          bufPBlock array. @ref DSPLIB_conditionalReplace_exec_ci merely
 *          retrieves the configuration from the bufPBlock and uses it to set up
 *          the hardware resources.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn   :  Pointer to the structure containing dimensional
 *                                information of input buffer
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of ouput buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_conditionalReplace_init_ci(DSPLIB_kernelHandle                       handle,
                                                const DSPLIB_bufParams2D_t               *bufParamsIn,
                                                const DSPLIB_bufParams2D_t               *bufParamsOut,
                                                const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms to the
 *         declaration of @ref DSPLIB_conditionalReplace_exec.
 *
 * @details For every channel (row) the compare type is resolved once, then a
 *          software pipelined loop compares a vector of samples against the
 *          channel constant and selects between the replacement value and the
 *          input. Input is read with a streaming engine, output is written with a
 *          stream address generator.
 *
 *  @param [in]  handle        : Active handle to the kernel
 *  @param [in]  pIn           : Pointer to buffer holding the input buffer
 *  @param [out] pOut          : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_conditionalReplace_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms to the
 *         declaration of @ref DSPLIB_conditionalReplace_exec.
 *
 *  @param [in]  handle        : Active handle to the kernel
 *  @param [in]  pIn           : Pointer to buffer holding the input buffer
 *  @param [out] pOut          : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 */
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_conditionalReplace_exec_cn and
    *         @ref DSPLIB_conditionalReplace_exec_ci.                        */
   pFxnDSPLIB_conditionalReplace_exec execute;

   /*! @brief Number of vectors to be processed per row (channel) */
   uint32_t nVecs;

   uint32_t dim_x;     /**< \brief Width of buffer in X dimension in elements. */
   uint32_t dim_y;     /**< \brief Height of buffer in Y dimension in elements. */
   int32_t  strideIn;  /**< \brief Stride in Y dimension in elements of inputs. */
   int32_t  strideOut; /**< \brief Stride in Y dimension in elements of outputs. */

   DSPLIB_conditionalReplace_SetArgs setArgs; /**< \brief A copy of the last-set runtime parameters. */

   /*! @brief bufPblock array to store SE/SA template */
   uint8_t bufPblock[DSPLIB_conditionalReplace_IXX_IXX_OXX_PBLOCK_SIZE];

} DSPLIB_conditionalReplace_PrivArgs;

#endif /* DSPLIB_CONDITIONALREPLACE_IXX_IXX_OXX_PRIV_H_ */
