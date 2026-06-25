// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_CHOLESKY_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_CHOLESKY_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_inlines.h"
#include "../common/DSPLIB_utility.h"
#include "DSPLIB_cholesky.h"
#include "DSPLIB_cholesky_common.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_cholesky_PrivArgs structure.
 *
 */
#define DSPLIB_cholesky_IXX_IXX_OXX_PBLOCK_SIZE (7 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_cholesky_exec_ci
 *         and @ref DSPLIB_cholesky_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_cholesky_exec)(DSPLIB_kernelHandle handle,
                                                  void *restrict pInA,
                                                  void *restrict pOutU,
                                                  void *restrict multiplierPtr);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_cholesky_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_cholesky_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_cholesky_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_cholesky_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn   :  Pointer to the structure containing dimensional information of input buffer
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional information of ouput buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init parameters
 *
 *  @return Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_cholesky_init_ci(DSPLIB_kernelHandle             handle,
                                             const DSPLIB_bufParams2D_t     *bufParamsIn,
                                             const DSPLIB_bufParams2D_t     *bufParamsOut,
                                             const DSPLIB_cholesky_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_cholesky_exec.
 *
 *  @details The function uses the Streaming Engine and Streaming Address generators to
 *           vectorize the operations of Cholesky Decomposition. The function process square
 *           matrix A[order][order] and stores the result in upper triangular matrix
 *           L[order][order].
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pInA         : Pointer to buffer holding the input data A[order][order]
 *  @param [out] pOutU        : Pointer to buffer holding the temporary output data U[order][order]
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
extern DSPLIB_STATUS DSPLIB_cholesky_exec_ci(DSPLIB_kernelHandle handle,
                                             void *restrict pInA,
                                             void *restrict pOutU,
                                             void *restrict multiplierPtr);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_cholesky_exec.
 *
 * @details
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pInA         : Pointer to buffer holding the input data A[order][order]
 *  @param [out] pOutU        : Pointer to buffer holding the temporary output data U[order][order]
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_cholesky_exec_cn(DSPLIB_kernelHandle handle,
                                             void *restrict pInA,
                                             void *restrict pOutU,
                                             void *restrict multiplierPtr);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_cholesky_exec_cn and
    *         @ref DSPLIB_cholesky_exec_ci.                        */
   pFxnDSPLIB_cholesky_exec execute;
   /*! @brief Order of input buffer for different batches
    *         @ref DSPLIB_cholesky_init that will be retrieved
    *         and used by @ref DSPLIB_cholesky_exec                */
   int32_t order;      // Order of input A
   int32_t stride;     // stride to go from one row to the other
   int32_t strideCn;   // stride to go from one row to the other in cn
   int32_t enableTest; // Enable the positive definite check
   int32_t shiftForVecLenDiv;
   uint8_t bufPblock[DSPLIB_cholesky_IXX_IXX_OXX_PBLOCK_SIZE]; // se init param
} DSPLIB_cholesky_PrivArgs;

#endif /* DSPLIB_cholesky_IXX_IXX_OXX_PRIV_H_ */
