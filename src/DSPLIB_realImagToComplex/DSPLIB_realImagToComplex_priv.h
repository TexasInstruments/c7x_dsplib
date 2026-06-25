// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_REALIMAGTOCOMPLEX_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_REALIMAGTOCOMPLEX_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_realImagToComplex.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_realImagToComplex_PrivArgs structure.
 *
 */

#define DSPLIB_realImagToComplex_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_realImagToComplex_exec_ci
 *         and @ref DSPLIB_realImagToComplex_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_realImagToComplex_exec)(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn0,
                                                           void *restrict pIn1,
                                                           void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_realImagToComplex_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_realImagToComplex_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_realImagToComplex_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_realImagToComplex_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input real buffer
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                                information of input imaginary buffer
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of ouput buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */

template <typename dataType>
DSPLIB_STATUS DSPLIB_realImagToComplex_init_ci(DSPLIB_kernelHandle                      handle,
                                               const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                               const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                               const DSPLIB_bufParams2D_t              *bufParamsOut,
                                               const DSPLIB_realImagToComplex_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_realImagToComplex_exec.
 *
 * @details The function uses MMA hardware accelerator to perform the
 *          convolution computation. Filter data is loaded into B panel of the
 *          MMA from memory using one streaming engine, while the input data is
 *          loaded into A vectors of the MMA using the other streaming engine.
 *          Result of the compute from MMA C panel is stored into memory using
 *          a stream realImagToComplexress generator.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input real buffer
 *  @param [in]  pIn1         : Pointer to buffer holding the input imag buffer.
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
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
DSPLIB_STATUS DSPLIB_realImagToComplex_exec_ci(DSPLIB_kernelHandle handle,
                                               void *restrict pIn0,
                                               void *restrict pIn1,
                                               void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_realImagToComplex_exec.
 *
 * @details
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input real buffer
 *  @param [in]  pIn1         : Pointer to buffer holding the input imag buffer.
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_realImagToComplex_exec_cn(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn0,
                                                      void *restrict pIn1,
                                                      void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_realImagToComplex_exec_cn and
    *         @ref DSPLIB_realImagToComplex_exec_ci.                        */
   pFxnDSPLIB_realImagToComplex_exec execute;

   /*! @brief Number of vector to be processed for the given input buffer */
   uint32_t nVecs;

   uint32_t dim_x;     /**< \brief Width of buffer in X dimension in elements. */
   uint32_t dim_y;     /**< \brief Height of buffer in Y dimension in elements. */
   int32_t  strideIn;  /**< \brief Stride in Y dimension in bytes of inputs. */
   int32_t  strideOut; /**< \brief Stride in Y dimension in bytes of outputs. */

   /*! @brief bufPblock array to store SE/SA template */
   uint8_t bufPblock[DSPLIB_realImagToComplex_IXX_IXX_OXX_PBLOCK_SIZE];

} DSPLIB_realImagToComplex_PrivArgs;

#endif /* DSPLIB_REALIMAGTOCOMPLEX_IXX_IXX_OXX_PRIV_H_ */
