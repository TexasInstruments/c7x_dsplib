// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_MATTRANS_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_MATTRANS_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_matTrans.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_matTrans_PrivArgs structure.
 *
 */
#define DSPLIB_MAT_TRANS_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE + sizeof(int32_t))

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_matTrans_exec_ci
 *         and @ref DSPLIB_matTrans_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_matTrans_exec)(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_matTrans_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_matTrans_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_matTrans_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_matTrans_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

template <typename dataType>
extern DSPLIB_STATUS DSPLIB_matTrans_init_ci(DSPLIB_kernelHandle            handle,
                                             const DSPLIB_bufParams2D_t    *bufParamsIn,
                                             const DSPLIB_bufParams2D_t    *bufParamsOut,
                                             const DSPLIB_matTransInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_matTrans_exec.
 *
 * @details The function uses streaming engine to load data in TRANSPOSE MODE. The result
 *           is then stored at separate location using a streaming address generator.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [out] pOut        : Pointer to buffer holding the output data
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
extern DSPLIB_STATUS DSPLIB_matTrans_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_matTrans_init.
 *
 * @details
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsIn            :  Pointer to the structure containing dimensional
 *                                         information of input buffer
 *  @param [out] bufParamsOut           :  Pointer to the structure containing dimensional
 *                                         information of ouput buffer
 *  @param [in]  pKerInitArgs           :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

DSPLIB_STATUS DSPLIB_matTrans_init_cn(DSPLIB_kernelHandle            handle,
                                      DSPLIB_bufParams2D_t          *bufParamsIn,
                                      DSPLIB_bufParams2D_t          *bufParamsOut,
                                      const DSPLIB_matTransInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_matTrans_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_matTrans_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_matTrans_exec_cn and
    *         @ref DSPLIB_matTrans_exec_ci.                        */
   pFxnDSPLIB_matTrans_exec execute;
   /*! @brief Size of input buffer for different batches
    *         @ref DSPLIB_matTrans_init that will be retrieved
    *         and used by @ref DSPLIB_matTrans_exec                */
   /*! @brief Width of input data matrix                           */
   uint32_t widthIn;
   /*! @brief Height of input data matrix                          */
   uint32_t heightIn;
   /*! @brief Stride between rows of input data matrix             */
   int32_t strideIn;
   /*! @brief Stride between rows of output data matrix            */
   int32_t strideOut;
   /*! @brief Buffer to save SE & SA configuration parameters      */
   uint8_t bufPblock[DSPLIB_MAT_TRANS_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_matTrans_PrivArgs;

#endif /* DSPLIB_matTrans_IXX_IXX_OXX_PRIV_H_ */
