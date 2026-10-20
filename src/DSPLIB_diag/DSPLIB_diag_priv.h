// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_DIAG_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_DIAG_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_diag.h"

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_diag_PrivArgs structure.
 *
 */

#define DSPLIB_DIAG_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE)

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_diag_exec_ci
 *         and @ref DSPLIB_diag_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_diag_exec)(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn,
                                                     void *restrict pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_diag_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and streaming address generator based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_diag_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_diag_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_diag_exec_ci does not lose cycles
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
extern DSPLIB_STATUS DSPLIB_diag_init_ci(DSPLIB_kernelHandle               handle,
                                                const DSPLIB_bufParams2D_t *bufParamsIn,
                                                const DSPLIB_bufParams1D_t *bufParamsOut,
                                                const DSPLIB_diag_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_diag_exec.
 *
 * @details The function gathers the diagonal elements of the input matrix
 *          (stride of order + 1 elements) using a streaming engine, and
 *          stores them contiguously into the output buffer using a stream
 *          address generator.
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
template <typename dataType, int32_t dataIn>
extern DSPLIB_STATUS
DSPLIB_diag_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_diag_exec.
 *
 * @details Copies every (order + 1)th element of the input matrix to the output.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [out] pOut       : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType, int32_t dataIn>
extern DSPLIB_STATUS
DSPLIB_diag_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 * @brief Returns the number of diagonal elements gathered per SE fetch for a data type
 */
extern int32_t DSPLIB_diag_getRowsPerFetch(int32_t dataType);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /** @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_diag_exec_cn and
    *         @ref DSPLIB_diag_exec_ci.                        */
   pFxnDSPLIB_diag_exec execute;
   /** @brief Order of square matrix
    *         @ref DSPLIB_diag_init that will be retrieved
    *         and used by @ref DSPLIB_diag_exec                */
   int32_t order;
   /** @brief Number of diagonal elements gathered per SE fetch, set by the
    *         init function and used by @ref DSPLIB_diag_perfEst   */
   int32_t rowsPerFetch;
   uint8_t bufPblock[DSPLIB_DIAG_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_diag_PrivArgs;

#endif /* DSPLIB_DIAG_IXX_IXX_OXX_PRIV_H_ */
