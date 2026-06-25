// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_BLKCOPYCONST2D_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_BLKCOPYCONST2D_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_blkCopyConst2D.h"

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_blkCopyConst2D_PrivArgs structure.
 *
 */
#define DSPLIB_BLKCOPYCONST2D_IXX_IXX_OXX_PBLOCK_SIZE (SE_PARAM_SIZE + SA_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_blkCopyConst2D_exec_ci
 *         and @ref DSPLIB_blkCopyConst2D_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_blkCopyConst2D_exec)(DSPLIB_kernelHandle handle,
                                                        void *restrict pIn1,
                                                        void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_blkCopyConst2D_exec_cn and
    *         @ref DSPLIB_blkCopyConst2D_exec_ci.                        */
   pFxnDSPLIB_blkCopyConst2D_exec execute;
   /*! @brief Width of the 2D block (X dimension) */
   int32_t width;
   /*! @brief Height of the 2D block (Y dimension) */
   int32_t height;
   /*! @brief Stride for output (in elements, not bytes) */
   uint32_t strideOut;
   /*! @brief Number of vectors read per row */
   uint32_t numVecs;
   /*! @brief Buffer to save SE & SA configuration parameters */
   uint8_t bufPblock[DSPLIB_BLKCOPYCONST2D_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_blkCopyConst2D_PrivArgs;

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_blkCopyConst2D_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_blkCopyConst2D_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_blkCopyConst2D_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_blkCopyConst2D_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn1  :  Pointer to the structure containing dimensional
 *                               information of constant buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci(DSPLIB_kernelHandle                  handle,
                                                   const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                   const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                   const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_blkCopyConst2D_exec.
 *
 * @details The function loads input constant data. The loaded data is then stored
 *          at a location determined by the streaming address generator.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn1        : Pointer to buffer holding the constant data
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
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_blkCopyConst2D_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn1        : Pointer to buffer holding the constant data
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
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_blkCopyConst2D_init.
 *
 * @details
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsIn1           :  Pointer to the structure containing
 * dimensional information of constant buffer
 *  @param [out] bufParamsOut           :  Pointer to the structure containing
 * dimensional information of ouput buffer
 *  @param [in]  pKerInitArgs           :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_cn(DSPLIB_kernelHandle                  handle,
                                            DSPLIB_bufParams1D_t                *bufParamsIn1,
                                            DSPLIB_bufParams2D_t                *bufParamsOut,
                                            const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

#endif /* DSPLIB_BLKCOPYCONST2D_IXX_IXX_OXX_PRIV_H_ */
