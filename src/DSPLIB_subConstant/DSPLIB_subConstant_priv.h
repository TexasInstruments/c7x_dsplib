// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_SUBCONSTANT_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_SUBCONSTANT_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_subConstant.h"

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_subConstant_PrivArgs structure.
 *
 */

#define DSPLIB_SUBCONSTANT_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE + 1 * DSPLIB_PARAM_SIZE + 2 * sizeof(int32_t))
/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_subConstant_exec_ci
 *         and @ref DSPLIB_subConstant_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_subConstant_exec)(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_subConstant_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_subConstant_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_subConstant_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_subConstant_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsFilterCoeff :  Pointer to the structure containing dimensional
 *                               information of filter coefficient buffer
 *  @param [in/out]  bufParamsFilterVar :  Pointer to the structure containing dimensional
 *                               information of filter variable buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_subConstant_init_ci(DSPLIB_kernelHandle                handle,
                                                const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                const DSPLIB_subConstant_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_subConstant_exec.
 *
 * @details The function uses MMA hardware accelerator to perform the
 *          convolution computation. Filter data is loaded into B panel of the
 *          MMA from memory using one streaming engine, while the input data is
 *          loaded into A vectors of the MMA using the other streaming engine.
 *          Result of the compute from MMA C panel is stored into memory using
 *          a stream subConstantress generator.
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
DSPLIB_subConstant_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_subConstant_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilterCoeff: Pointer to buffer holding the filter coefficient data
 *  @param [in]  pFilterVar  : Pointer to buffer holding the filter variable data
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType, int32_t dataIn>
extern DSPLIB_STATUS
DSPLIB_subConstant_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /** @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_subConstant_exec_cn and
    *         @ref DSPLIB_subConstant_exec_ci.                        */
   pFxnDSPLIB_subConstant_exec execute;
   /** @brief Size of input buffer for different batches
    *         @ref DSPLIB_subConstant_init that will be retrieved
    *         and used by @ref DSPLIB_subConstant_exec                */
   int32_t blockSize;
   uint8_t bufPblock[DSPLIB_SUBCONSTANT_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_subConstant_PrivArgs;

/**
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_subConstant_init.
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
DSPLIB_STATUS DSPLIB_subConstant_init_cn(DSPLIB_kernelHandle   handle,
                                         DSPLIB_bufParams2D_t *bufParamsIn,
                                         DSPLIB_bufParams2D_t *bufParamsOut);

#endif /* DSPLIB_SUBCONSTANT_IXX_IXX_OXX_PRIV_H_ */
