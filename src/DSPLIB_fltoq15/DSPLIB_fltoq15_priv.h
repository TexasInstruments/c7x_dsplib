// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_FLTOQ15_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_FLTOQ15_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_fltoq15.h"

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_fltoq15_PrivArgs structure.
 *
 */
#define DSPLIB_FLTOQ15_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_fltoq15_exec_ci
 *         and @ref DSPLIB_fltoq15_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_fltoq15_exec)(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_fltoq15_exec_cn and
    *         @ref DSPLIB_fltoq15_exec_ci.                        */
   pFxnDSPLIB_fltoq15_exec execute;
   /*! @brief Size of input buffer for different batches
    *         @ref DSPLIB_fltoq15_init that will be retrieved
    *         and used by @ref DSPLIB_fltoq15_exec                */
   int32_t blockSize;
   /*! @brief Flag indicating if SE reads and stores are enabled, set by
    *         @ref DSPLIB_fltoq15_init_ci that will be retrieved
    *         and used by @ref DSPLIB_bexp_exec                */
   bool isSEReadEnabled;

   uint8_t bufPblock[DSPLIB_FLTOQ15_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_fltoq15_PrivArgs;

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_fltoq15_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_fltoq15_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_fltoq15_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_fltoq15_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataTypeIn>
extern DSPLIB_STATUS DSPLIB_fltoq15_init_ci(DSPLIB_kernelHandle            handle,
                                            const DSPLIB_bufParams1D_t    *bufParamsIn,
                                            const DSPLIB_bufParams1D_t    *bufParamsOut,
                                            const DSPLIB_fltoq15_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_fltoq15_exec.
 *
 * @details The function uses two streaming engines to load the data from
 *          memory and converts the input float/double data into Q15 format.
 *          The result is stored into memory using a stream address generator.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [out] pOut        : Pointer to buffer holding the output data
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
template <typename dataTypeIn>
extern DSPLIB_STATUS DSPLIB_fltoq15_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_fltoq15_init.
 *
 * @details
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsIn            :  Pointer to the structure containing
 * dimensional information of input buffer
 *  @param [out] bufParamsOut           :  Pointer to the structure containing
 * dimensional information of ouput buffer
 *  @param [in]  pKerInitArgs           :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
DSPLIB_STATUS DSPLIB_fltoq15_init_cn(DSPLIB_kernelHandle            handle,
                                     DSPLIB_bufParams1D_t          *bufParamsIn,
                                     DSPLIB_bufParams1D_t          *bufParamsOut,
                                     const DSPLIB_fltoq15_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_fltoq15_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */

template <typename dataTypeIn>
DSPLIB_STATUS DSPLIB_fltoq15_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

#endif /* DSPLIB_FLTOQ15_IXX_IXX_OXX_PRIV_H_ */
