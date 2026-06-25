// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_MAXINDEX_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_MAXINDEX_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_maxIndex.h"

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_maxIndex_PrivArgs structure.
 *
 */
#define DSPLIB_MAXINDEX_IXX_IXX_OXX_PBLOCK_SIZE (1 * SE_PARAM_SIZE + 1 * DSPLIB_PARAM_SIZE + 2 * sizeof(int32_t))

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_maxIndex_exec_ci
 *         and @ref DSPLIB_maxIndex_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_maxIndex_exec)(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_maxIndex_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_maxIndex_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_maxIndex_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_maxIndex_exec_ci does not lose cycles
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
extern DSPLIB_STATUS DSPLIB_maxIndex_init_ci(DSPLIB_kernelHandle             handle,
                                             const DSPLIB_bufParams1D_t     *bufParamsIn,
                                             const DSPLIB_bufParams1D_t     *bufParamsOut,
                                             const DSPLIB_maxIndex_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_maxIndex_exec.
 *
 * @details The function uses MMA hardware accelerator to perform the
 *          convolution computation. Filter data is loaded into B panel of the
 *          MMA from memory using one streaming engine, while the input data is
 *          loaded into A vectors of the MMA using the other streaming engine.
 *          Result of the compute from MMA C panel is stored into memory using
 *          a stream minress generator.
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
template <typename T, typename TIndex>
extern DSPLIB_STATUS DSPLIB_maxIndex_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_maxIndex_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn        : Pointer to buffer holding the input data
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_maxIndex_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /** @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_maxIndex_exec_cn and
    *         @ref DSPLIB_maxIndex_exec_ci.                        */
   pFxnDSPLIB_maxIndex_exec execute;
   /** @brief Size of input buffer for different batches
    *         @ref DSPLIB_maxIndex_init that will be retrieved
    *         and used by @ref DSPLIB_maxIndex_exec                */
   int32_t blockSize;
   uint8_t bufPblock[DSPLIB_MAXINDEX_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_maxIndex_PrivArgs;

template <typename T, typename TIndex> struct metadata {
   // derive c7x vector type from value template and index template
   typedef typename c7x::make_full_vector<T>::type      vec;
   typedef typename c7x::make_full_vector<TIndex>::type index_vec;

   vec       maxVals;
   index_vec maxIndices; // maxIndices[i] = index of location of maxVals[i]
};

/**
 *  @brief This function is the kernel loop helper function for the optimized
 *         implementation of the kernel. The function is called within @ref
 *         DSPLIB_maxIndex_exec_c and this method performs the loops needed to
 *         calculate the vector containing all of the maxIndeximum values.
 *
 * @details
 *
 *  @param [in]  length      : length of the input buffer
 *  @param [in]  width        : width of one SIMD buffer
 *  @param [out] pSrc        : Pointer to buffer holding the input buffer
 *
 *  @return      Scalar containing max index.
 *
 */
template <typename T, typename TIndex> metadata<T, TIndex> DSPLIB_maxIndex_loopLogic(size_t length, void *pSrc);
#endif /* DSPLIB_MAXINDEX_IXX_IXX_OXX_PRIV_H_ */
