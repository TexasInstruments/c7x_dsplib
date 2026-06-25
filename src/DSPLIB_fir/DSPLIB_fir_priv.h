// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_FIR_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_FIR_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_fir.h"

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_fir_PrivArgs structure.
 *
 */
#define DSPLIB_FIR_IXX_IXX_OXX_PBLOCK_SIZE                                                                             \
   (5 * SE_PARAM_SIZE + 1 * DSPLIB_PARAM_SIZE + 2 * sizeof(int32_t) + sizeof(__HWA_OFFSET_REG) +                       \
    sizeof(__HWA_CONFIG_REG_v1)) +                                                                                     \
       sizeof(int32_t)

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of the various FIR execution variants including
 *         @ref DSPLIB_fir_MMA_exec_ci, @ref DSPLIB_fir_smallFirSS_exec_ci,
 *         @ref DSPLIB_fir_smallFir_exec_ci, @ref DSPLIB_fir_smallMediumFir_exec_ci,
 *         @ref DSPLIB_fir_mediumFir_exec_ci, @ref DSPLIB_fir_largeFir_exec_ci,
 *         and @ref DSPLIB_fir_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_fir_exec)(DSPLIB_kernelHandle handle,
                                             void *restrict pIn,
                                             void *restrict pFilter,
                                             void *restrict pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_fir_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, the appropriate execution variant (such as
 *          @ref DSPLIB_fir_MMA_exec_ci, @ref DSPLIB_fir_smallFirSS_exec_ci,
 *          @ref DSPLIB_fir_smallFir_exec_ci, @ref DSPLIB_fir_smallMediumFir_exec_ci,
 *          @ref DSPLIB_fir_mediumFir_exec_ci, or @ref DSPLIB_fir_largeFir_exec_ci)
 *          would be called later independently by the application. When the
 *          execution variant runs, it merely retrieves the configuration from
 *          the bufPBlock and uses it to set up the hardware resources. This
 *          arrangement is so that the execution variant does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsFilter :  Pointer to the structure containing
 * dimensional information of filter buffer
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
extern DSPLIB_STATUS DSPLIB_fir_init_ci(DSPLIB_kernelHandle         handle,
                                        const DSPLIB_bufParams2D_t *bufParamsIn,
                                        const DSPLIB_bufParams2D_t *bufParamsFilter,
                                        const DSPLIB_bufParams2D_t *bufParamsOut,
                                        const DSPLIB_fir_InitArgs  *pKerInitArgs);

/**
 *  @brief MMA execution variant for FIR filter
 *
 * @details The function uses MMA hardware accelerator to perform the
 *          convolution computation. Filter data is loaded into B panel of the
 *          MMA from memory using one streaming engine, while the input data is
 *          loaded into A vectors of the MMA using the other streaming engine.
 *          Result of the compute from MMA C panel is stored into memory using
 *          a stream address generator.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilter     : Pointer to buffer holding the filter data
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
DSPLIB_fir_MMA_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

/**
 *  @brief Small FIR with single sample - C7x execution variant
 *
 * @details This function is optimized for small FIR filters processing a single
 *          sample at a time. It uses C7x SIMD instructions to perform the
 *          convolution computation efficiently for small filter lengths with
 *          single sample input.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilter     : Pointer to buffer holding the filter data
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
extern DSPLIB_STATUS DSPLIB_fir_smallFirSS_exec_ci(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn,
                                                   void *restrict pFilter,
                                                   void *restrict pOut);

/**
 *  @brief Small FIR - C7x execution variant
 *
 * @details This function is optimized for small FIR filters. It uses C7x SIMD
 *          instructions to perform the convolution computation efficiently for
 *          small input data sizes.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilter     : Pointer to buffer holding the filter data
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
extern DSPLIB_STATUS DSPLIB_fir_smallFir_exec_ci(DSPLIB_kernelHandle handle,
                                                 void *restrict pIn,
                                                 void *restrict pFilter,
                                                 void *restrict pOut);

/**
 *  @brief Small Medium FIR - C7x execution variant
 *
 * @details This function is optimized for small to medium output data sizes. It uses
 *          C7x SIMD instructions to perform the convolution computation
 *          efficiently for output data sizes of 32 or more.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilter     : Pointer to buffer holding the filter data
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
DSPLIB_STATUS DSPLIB_fir_smallMediumFir_exec_ci(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pFilter,
                                                void *restrict pOut);

/**
 *  @brief Medium FIR - C7x execution variant
 *
 * @details This function is optimized for medium-sized FIR filters. It uses
 *          C7x SIMD instructions to perform the convolution computation
 *          efficiently for output data sizes of 64 or more.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilter     : Pointer to buffer holding the filter data
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
extern DSPLIB_STATUS DSPLIB_fir_mediumFir_exec_ci(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn,
                                                  void *restrict pFilter,
                                                  void *restrict pOut);

/**
 *  @brief Large FIR - C7x execution variant
 *
 * @details This function is optimized for large FIR filters. It uses C7x SIMD
 *          instructions to perform the convolution computation
 *          efficiently for output data sizes of 128 or more.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilter     : Pointer to buffer holding the filter data
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
extern DSPLIB_STATUS DSPLIB_fir_largeFir_exec_ci(DSPLIB_kernelHandle handle,
                                                 void *restrict pIn,
                                                 void *restrict pFilter,
                                                 void *restrict pOut);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_fir_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilter     : Pointer to buffer holding the filter data
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType, typename accumulateType>
extern DSPLIB_STATUS
DSPLIB_fir_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

/**
 *  @brief This function is a utility function that the application calls to
 *         obtain the sizes of data buffers.
 *
 * @details This function calculates the sizes of the buffers based on filter
 *          data length and input data length. Sizes of filter data buffer and
 *          input data buffer may be larger than the filter length and input
 *          data length respectively. This is to facilitate processing of data
 *          according to the design described under the FIR execution variants.
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsFilter :  Pointer to the structure containing
 * dimensional information of filter buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_getSizes(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                  DSPLIB_bufParams2D_t      *bufParamsIn,
                                  DSPLIB_bufParams2D_t      *bufParamsFilter,
                                  DSPLIB_bufParams2D_t      *bufParamsOut);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /** @brief Structure holding initialization parameters                 */
   DSPLIB_fir_InitArgs initArgs;
   /** @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_fir_exec_cn and C7x execution variants
    *         (@ref DSPLIB_fir_MMA_exec_ci, @ref DSPLIB_fir_smallFirSS_exec_ci,
    *         @ref DSPLIB_fir_smallFir_exec_ci, @ref DSPLIB_fir_smallMediumFir_exec_ci,
    *         @ref DSPLIB_fir_mediumFir_exec_ci, or @ref DSPLIB_fir_largeFir_exec_ci) */
   pFxnDSPLIB_fir_exec execute;
   /** @brief Pitch of input buffer for different batches
    *         @ref DSPLIB_fir_init that will be retrieved
    *         and used by @ref DSPLIB_fir_exec                */
   uint32_t dataBufferInPitch;
   /** @brief Pitch of output buffer for different batches
    *         @ref DSPLIB_fir_init that will be retrieved
    *         and used by @ref DSPLIB_fir_exec                */
   uint32_t dataBufferOutPitch;
   /** @brief Pitch of filter coefficient buffer for
    *         different batches @ref DSPLIB_fir_init that
    *         will be retrieved and used by
    *         @ref DSPLIB_fir_exec                            */
   uint32_t filterPitch;
   /** @brief Array to store the configuration prepared by
    *         @ref DSPLIB_fir_init_ci that will be retrieved
    *         and used by the C7x execution variants          */
   /** @brief flag to indicate small or large FIR
    * flag  = 0 is small and flag = 1 is large                                */
   uint32_t flag;

   int32_t dataLoopSize;
   int32_t outputSize;
   int32_t filterLoopSize;
   int32_t remFilterLoopSize;
   uint8_t bufPblock[DSPLIB_FIR_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_fir_PrivArgs;

/**
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_fir_init.
 *
 * @details
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsFilter :  Pointer to the structure containing
 * dimensional information of filter buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
DSPLIB_STATUS DSPLIB_fir_init_cn(DSPLIB_kernelHandle   handle,
                                 DSPLIB_bufParams2D_t *bufParamsIn,
                                 DSPLIB_bufParams2D_t *bufParamsFilter,
                                 DSPLIB_bufParams2D_t *bufParamsOut,
                                 DSPLIB_fir_InitArgs  *pKerInitArgs);

#endif /* DSPLIB_FIR_IXX_IXX_OXX_PRIV_H_ */
