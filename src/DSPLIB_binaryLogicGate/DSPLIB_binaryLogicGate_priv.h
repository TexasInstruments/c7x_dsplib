// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_BINARYLOGICGATE_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_BINARYLOGICGATE_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_binaryLogicGate.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_binaryLogicGate_PrivArgs structure.
 *
 */

#define DSPLIB_binaryLogicGate_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_binaryLogicGate_exec_ci
 *         and @ref DSPLIB_binaryLogicGate_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_binaryLogicGate_exec)(DSPLIB_kernelHandle handle,
                                                         void *restrict pIn0,
                                                         void *restrict pIn1,
                                                         void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_binaryLogicGate_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and stream address generator based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_binaryLogicGate_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_binaryLogicGate_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_binaryLogicGate_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input0 buffer
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                                information of input1 buffer
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
DSPLIB_STATUS DSPLIB_binaryLogicGate_init_ci(DSPLIB_kernelHandle                    handle,
                                             const DSPLIB_bufParams2D_t            *bufParamsIn0,
                                             const DSPLIB_bufParams2D_t            *bufParamsIn1,
                                             const DSPLIB_bufParams2D_t            *bufParamsOut,
                                             const DSPLIB_binaryLogicGate_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which compares the corresponding bits of two inputs using the bitwise AND (&),
 *         bitwise OR (|) or bitwise XOR (^) operator, selected at compile time by the opType template parameter.
 *         The function declaration conforms to the declaration of @ref DSPLIB_binaryLogicGate_exec.
 *
 * @details The function uses two streaming engines to load the two input
 *          buffers and a stream address generator to store the result. The
 *          bitwise operation is selected through an inline helper chosen by
 *          opType, so no runtime branching occurs in the compute loop.
 *
 *  @tparam dataType      : Element type of the buffers (uint8_t, uint16_t, uint32_t or uint64_t)
 *  @tparam opType        : Bitwise operator, one of @ref DSPLIB_binaryLogicGate_opType
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [in]  pIn1         : Pointer to buffer holding the input1 buffer.
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
template <typename dataType, uint32_t opType>
DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_ci(DSPLIB_kernelHandle handle,
                                             void *restrict pIn0,
                                             void *restrict pIn1,
                                             void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel which computes the bitwise AND (&), OR (|) or XOR (^) of two inputs,
 *         selected by the opType template parameter. The function declaration conforms to the declaration of
 *         @ref DSPLIB_binaryLogicGate_exec.
 *
 * @details
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [in]  pIn1         : Pointer to buffer holding the input1 buffer.
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType, uint32_t opType>
extern DSPLIB_STATUS DSPLIB_binaryLogicGate_exec_cn(DSPLIB_kernelHandle handle,
                                                    void *restrict pIn0,
                                                    void *restrict pIn1,
                                                    void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_binaryLogicGate_exec_cn and
    *         @ref DSPLIB_binaryLogicGate_exec_ci.                        */
   pFxnDSPLIB_binaryLogicGate_exec execute;

   /*! @brief Number of vector to be processed for the given input buffer */
   uint32_t nVecs;

   uint32_t                       dim_x;     /**< \brief Width of buffer in X dimension in elements. */
   uint32_t                       dim_y;     /**< \brief Height of buffer in Y dimension in elements. */
   int32_t                        strideIn;  /**< \brief Stride in Y dimension in bytes of inputs. */
   int32_t                        strideOut; /**< \brief Stride in Y dimension in bytes of outputs. */
   int8_t                         funcStyle; /**< \brief Variant of the function, copied from init args. */
   uint32_t                       dataType;  /**< \brief Data type of input0, copied from the buffer parameters. */
   DSPLIB_binaryLogicGate_SetArgs setArgs;   /**< \brief A copy of the last-set runtime parameters. */

   /*! @brief bufPblock array to store SE/SA template */
   uint8_t bufPblock[DSPLIB_binaryLogicGate_IXX_IXX_OXX_PBLOCK_SIZE];

} DSPLIB_binaryLogicGate_PrivArgs;

#endif /* DSPLIB_BINARYLOGICGATE_IXX_IXX_OXX_PRIV_H_ */
