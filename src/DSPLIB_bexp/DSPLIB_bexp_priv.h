// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_BEXP_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_BEXP_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_inlines.h"
#include "../common/DSPLIB_utility.h"
#include "DSPLIB_bexp.h"

/*!
 * @brief Macro to define the maximum number of bits in an element
 *        @ref @ref DSPLIB_bexp_exec_ci
 *
 */

#define DSPLIB_BEXP_IXX_IXX_OXX_MAX_BITS_PER_ELEMENT (70)

/*!
 * @brief Macro to define the minimum number of vector lengths that the input
 *        data should occupy before SE is used for data reads
 *        @ref @ref DSPLIB_bexp_exec_ci
 *
 */

#define DSPLIB_BEXP_IXX_IXX_OXX_MIN_VECS_TO_ENABLE_SE_READS (4)

/*!
 * @brief Macro to print the debug message
 */

#define DSPLIB_BEXP_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE + 512)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_bexp_exec_ci
 *         and @ref DSPLIB_bexp_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_bexp_exec)(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_bexp_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_bexp_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_bexp_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_bexp_exec_ci does not lose cycles
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
extern DSPLIB_STATUS DSPLIB_bexp_init_ci(DSPLIB_kernelHandle         handle,
                                         const DSPLIB_bufParams1D_t *bufParamsIn,
                                         const DSPLIB_bufParams1D_t *bufParamsOut,
                                         const DSPLIB_bexp_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_bexp_exec.
 *
 * @details The function uses C7x DSP to perform the
 *          block exponent. The input data is read as blocks of vectors using the streaming engine. A vertical OR
 * operation is performed on the blocks of vectors that are read using the streaming engine. Finally, once the entire
 * data is read and the final OR result is computed, a horizontal OR operation is performed to get the final mask value.
 * The number of leading zeros or redundant sign bits are computed as the exponent.
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
extern DSPLIB_STATUS DSPLIB_bexp_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_bexp_exec.
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
extern DSPLIB_STATUS DSPLIB_bexp_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_bexp_exec_cn and
    *         @ref DSPLIB_bexp_exec_ci.                        */
   pFxnDSPLIB_bexp_exec execute;
   /*! @brief Size of input buffer for different batches
    *         @ref DSPLIB_bexp_init that will be retrieved
    *         and used by @ref DSPLIB_bexp_exec                */
   uint32_t blockSize;

   /*! @brief Flag indicating if SE reads are enabled, set by
    *         @ref DSPLIB_bexp_init that will be retrieved
    *         and used by @ref DSPLIB_bexp_exec                */
   bool isSEReadEnabled;

   /*! @brief Number of times to loop over data with
    *         isSEReadEnabled == true will be ceil_division(number of native vectors, 2)
    *         isSEReadEnabled == false will be number of native vectors
    *         set by @ref DSPLIB_bexp_init that will be retrieved
    *         and used by @ref DSPLIB_bexp_exec                */
   uint32_t mainLoopCount;

   /*! @brief The offset in bytes of SE1 start address with respect to start
    *         address of the input data.
    *         Set by @ref DSPLIB_bexp_init that will be retrieved
    *         and used by @ref DSPLIB_bexp_exec                */
   uint32_t se1StartAddressByteOffset;

   uint8_t bufPblock[DSPLIB_BEXP_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_bexp_PrivArgs;

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_bexp_init.
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
DSPLIB_STATUS
DSPLIB_bexp_init_cn(DSPLIB_kernelHandle handle, DSPLIB_bufParams2D_t *bufParamsIn, DSPLIB_bufParams2D_t *bufParamsOut);

#endif /* DSPLIB_BEXP_IXX_IXX_OXX_PRIV_H_ */
