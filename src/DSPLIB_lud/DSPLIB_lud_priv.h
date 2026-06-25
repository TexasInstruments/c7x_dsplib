// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_LUD_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_LUD_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_lud.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_lud_PrivArgs structure.
 *
 */
#define DSPLIB_LUD_IXX_IXX_OXX_PBLOCK_SIZE (22 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_lud_exec_ci
 *         and @ref DSPLIB_lud_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_lud_exec)(DSPLIB_kernelHandle handle,
                                             void *restrict pA,
                                             void *restrict pL,
                                             void *restrict pU,
                                             void *restrict pP);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_lud_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_lud_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_lud_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_lud_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsA   :  Pointer to the structure containing dimensional
 *                               information of input buffer A
 *  @param [out] bufParamsL   :  Pointer to the structure containing dimensional
 *                               information of output buffer L
 *  @param [out] bufParamsU   :  Pointer to the structure containing dimensional
 *                               information of output buffer U
 *  @param [out] bufParamsP   :  Pointer to the structure containing dimensional
 *                               information of output buffer P
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

template <typename dataType>
extern DSPLIB_STATUS DSPLIB_lud_init_ci(DSPLIB_kernelHandle         handle,
                                        const DSPLIB_bufParams2D_t *bufParamsA,
                                        const DSPLIB_bufParams2D_t *bufParamsL,
                                        const DSPLIB_bufParams2D_t *bufParamsU,
                                        const DSPLIB_bufParams2D_t *bufParamsP,
                                        const DSPLIB_ludInitArgs   *pKerInitArgs);

/*!
 * @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_lud_exec.
 *
 * @details The function uses streaming engine to load data. The result
 *           is then stored at separate location using a streaming address generator.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pA          : Pointer to matrix A
 *  @param [out] pL          : Pointer to lower triangular matrix L
 *  @param [out] pU          : Pointer to upper triangular matrix U
 *  @param [out] pP          : Pointer to permutation matrix P
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
extern DSPLIB_STATUS DSPLIB_lud_exec_ci(DSPLIB_kernelHandle handle,
                                        void *restrict pA,
                                        void *restrict pL,
                                        void *restrict pU,
                                        void *restrict pP);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_lud_init.
 *
 * @details
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsA             :  Pointer to the structure containing dimensional
 *                                         information of input buffer A
 *  @param [out] bufParamsL             :  Pointer to the structure containing dimensional
 *                                         information of output buffer L
 *  @param [out] bufParamsU             :  Pointer to the structure containing dimensional
 *                                         information of output buffer U
 *  @param [out] bufParamsP             :  Pointer to the structure containing dimensional
 *                                         information of output buffer P
 *  @param [in]  pKerInitArgs           :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

DSPLIB_STATUS DSPLIB_lud_init_cn(DSPLIB_kernelHandle         handle,
                                 const DSPLIB_bufParams2D_t *bufParamsA,
                                 const DSPLIB_bufParams2D_t *bufParamsL,
                                 const DSPLIB_bufParams2D_t *bufParamsU,
                                 const DSPLIB_bufParams2D_t *bufParamsP,
                                 const DSPLIB_ludInitArgs   *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_lud_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pA          : Pointer to matrix A
 *  @param [out] pL          : Pointer to lower triangular matrix L
 *  @param [out] pU          : Pointer to upper triangular matrix U
 *  @param [out] pP          : Pointer to permutation matrix P
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

template <typename dataType>
extern DSPLIB_STATUS DSPLIB_lud_exec_cn(DSPLIB_kernelHandle handle,
                                        void *restrict pA,
                                        void *restrict pL,
                                        void *restrict pU,
                                        void *restrict pP);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_lud_exec_cn and
    *         @ref DSPLIB_lud_exec_ci.                        */
   pFxnDSPLIB_lud_exec execute;
   /*! @brief Size of input buffer for different batches
    *         @ref DSPLIB_lud_init that will be retrieved
    *         and used by @ref DSPLIB_lud_exec                */
   /*! @brief order of the input data matrix A                          */
   int32_t order;
   /*! @brief Stride between rows of input  and output data matrix             */
   int32_t strideOrder;
   /*! @brief Stride between rows of output data matrix P           */
   int32_t strideP;
   /*! @brief Buffer to save SE & SA configuration parameters      */
   uint8_t bufPblock[DSPLIB_LUD_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_lud_PrivArgs;

#endif /* DSPLIB_LUD_IXX_IXX_OXX_PRIV_H_ */
