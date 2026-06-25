// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_CHOLESKY_INPLACE_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_CHOLESKY_INPLACE_IXX_IXX_OXX_PRIV_H_

// #include "../../common/DSPLIB_utility.h"
#include "../common/DSPLIB_inlines.h"
#include "../common/DSPLIB_utility.h"

#include "DSPLIB_cholesky_common.h"
#include "DSPLIB_cholesky_inplace.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_cholesky_inplace_PrivArgs structure.
 *
 */

#define DSPLIB_CHOLESKY_INPLACE_IXX_IXX_OXX_PBLOCK_SIZE (7 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_cholesky_inplace_exec_ci
 *         and @ref DSPLIB_cholesky_inplace_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_cholesky_inplace_exec)(DSPLIB_kernelHandle handle,
                                                          void *restrict pA,
                                                          void *restrict pMul);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_cholesky_inplace_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_cholesky_inplace_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_cholesky_inplace_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_cholesky_inplace_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle          :  Active handle to the kernel
 *  @param [in]  bufParamsA      :  Pointer to the structure containing dimensional
 *                                  information of input buffer
 *  @param [in]  bufParamsMul    :  Pointer to the structure containing dimensional information
 *                                  of scratch buffer
 *  @param [in]  pKerInitArgs    :  Pointer to the structure holding init parameters
 *
 *  @return Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_cholesky_inplace_init_ci(DSPLIB_kernelHandle                     handle,
                                                     DSPLIB_bufParams2D_t                   *bufParamsA,
                                                     DSPLIB_bufParams1D_t                   *bufParamsMul,
                                                     const DSPLIB_cholesky_inplace_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_cholesky_inplace_exec.
 *
 *  @details The function uses the Streaming Engine and Streaming Address generators to
 *           vectorize the operations of Cholesky Inplace Decomposition. The function operates
 *           on matrix A and stores the result in same matrix.
 *
 *  @param [in]  handle : Active handle to the kernel
 *  @param [in]  pA     : Pointer to buffer holding the input data A[order][order]
 *  @param [in]  pMul   : Pointer to scratch buffer
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
extern DSPLIB_STATUS
DSPLIB_cholesky_inplace_exec_ci(DSPLIB_kernelHandle handle, void *restrict pA, void *restrict pMul);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_cholesky_inplace_exec.
 *
 * @details
 *
 *  @param [in]  handle : Active handle to the kernel
 *  @param [in]  pA     : Pointer to buffer holding the input data A[order][order]
 *  @param [in]  pMul   : Pointer to scratch buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_cholesky_inplace_exec_cn(DSPLIB_kernelHandle handle, void *restrict pA, void *restrict pMul);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_cholesky_inplace_exec_cn and
    *         @ref DSPLIB_cholesky_inplace_exec_ci.                        */
   pFxnDSPLIB_cholesky_inplace_exec execute;
   /*! @brief Order of input buffer for different batches
    *         @ref DSPLIB_cholesky_inplace_init that will be retrieved
    *         and used by @ref DSPLIB_cholesky_inplace_exec                */
   int32_t order;      // Order of input A
   int32_t stride;     // stride to go from one row to the other
   int32_t enableTest; // flag to check if input matrix is positive definite
   int32_t shiftForVecLenDiv;
   uint8_t bufPblock[DSPLIB_CHOLESKY_INPLACE_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_cholesky_inplace_PrivArgs;

#endif /* DSPLIB_CHOLESKY_INPLACE_IXX_IXX_OXX_PRIV_H_ */
