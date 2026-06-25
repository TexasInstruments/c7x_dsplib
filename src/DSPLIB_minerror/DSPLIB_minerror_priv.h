// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_MINERROR_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_MINERROR_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_minerror.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *          @ref DSPLIB_minerror_PrivArgs structure.
 *
 */

#define DSPLIB_MINERROR_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE + 1 * DSPLIB_PARAM_SIZE + 2 * sizeof(int32_t))
/*!
 *  @brief This is a function pointer type that conforms to the
 *            declaration of @ref DSPLIB_minerror_exec_ci
 *            and @ref DSPLIB_minerror_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_minerror_exec)(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn,
                                                  void *restrict pErrCoefs,
                                                  const int *restrict pMaxIndex,
                                                  const void *restrict pMaxVal);

/*!
 *  @brief This function is the initialization function for the C7x
 *            implementation of the kernel. The function declaration conforms
 *            to the declaration of @ref DSPLIB_minerror_init.
 *
 * @details This function determines the configuration for the streaming engine
 *             and MMA hardware resources based on the function call parameters,
 *             and the configuration is saved in bufPBlock array. In the kernel
 *             call sequence, @ref DSPLIB_minerror_exec_ci would be
 *             called later independently by the application. When
 *             @ref DSPLIB_minerror_exec_ci runs, it merely retrieves
 *             the configuration from the bufPBlock and uses it to set up the
 *             hardware resources. This arrangement is so that
 *             @ref DSPLIB_minerror_exec_ci does not lose cycles
 *             to determine the hardware configuration.
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsIn            :  Pointer to the structure containing dimensional
 *                                         information of input buffer
 *  @param [in]  bufParamsErrCoefs      :  Pointer to the structure containing error coefficients
 *                                                 information of input buffer
 *  @param [out] pMaxIndex              :  Pointer to the structure containing
 *                                                 information of ouput buffer
 *  @param [in]  pKerInitArgs           :  Pointer to the structure holding init parameters
 *
 *  @return        Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_minerror_init_ci(DSPLIB_kernelHandle             handle,
                                             const DSPLIB_bufParams2D_t     *bufParamsIn,
                                             const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                             const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                             const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                             const DSPLIB_minerror_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *            implementation of the kernel. The function declaration conforms
 *            to the declaration of @ref DSPLIB_minerror_exec.
 *
 * @details The function uses streaming engine and C7x DSP to perform the
 *             minerror computation.
 *
 *  @param [in]  handle                   :  Active handle to the kernel
 *  @param [in]  bufParamsIn              :  Pointer to the structure containing dimensional
 *                                                      information of input buffer
 *  @param [in]  bufParamsErrCoefs        :  Pointer to the structure containing error coefficients
 *                                                      information of input buffer
 *  @param [out] bufParamsOutIndex        :  Pointer to the structure containing
 *                                                      information of ouput buffer
 *  @param [out] bufParamsOutVal          :  Pointer to the structure containing
 *                                                      information of ouput buffer
 *  @param [in]  pKerInitArgs             :  Pointer to the structure holding init parameters
 *
 *  @return        Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Performance Considerations:
 *     For best performance,
 *     - the input and output data buffers are expected to be in L2 memory
 *     - the buffer pointers are assumed to be 64-byte aligned
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_minerror_exec_ci(DSPLIB_kernelHandle handle,
                                             void *restrict pIn,
                                             void *restrict pErrCoefs,
                                             const int *restrict pMaxIndex,
                                             const void *restrict pMaxVal);

/*!
 *  @brief This function is the main execution function for the natural
 *            C implementation of the kernel. The function declaration conforms
 *            to the declaration of @ref DSPLIB_minerror_exec.
 *
 * @details
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn          : Pointer to buffer holding the input data
 *  @param [in]  pErrCoeff    : Pointer to buffer holding the error coefficient data
 *  @param [out] pMaxIndex    : Pointer to buffer holding the index to the element vector that resulted in the maximum
 * dot product
 *  @param [out] pMaxVal      : Pointer to buffer holding the value of the maximum dot product
 *
 *  @return        Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_minerror_exec_cn(DSPLIB_kernelHandle handle,
                                             void *restrict pIn,
                                             void *restrict pErrCoefs,
                                             const int *restrict pMaxIndex,
                                             const void *restrict pMaxVal);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *            @ref DSPLIB_minerror_exec_cn and
    *            @ref DSPLIB_minerror_exec_ci.                                  */
   pFxnDSPLIB_minerror_exec execute;
   /*! @brief Size of input data
    *            @ref DSPLIB_minerror_init that will be retrieved
    *            and used by @ref DSPLIB_minerror_exec                          */
   uint32_t vecInSize;
   /*! @brief Size of error coefficients vector                                 */
   uint32_t errCoefsSize;
   uint32_t strideIn;
   int32_t  mainLoopCount;

   uint8_t bufPblock[DSPLIB_MINERROR_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_minerror_PrivArgs;

/*!
 *  @brief This function is the initialization function for the natural C
 *            implementation of the kernel. The function declaration conforms
 *            to the declaration of @ref DSPLIB_minerror_init.
 *
 * @details
 *
 *  @param [in]  handle                   :  Active handle to the kernel
 *  @param [in]  bufParamsIn              :  Pointer to the structure containing dimensional
 *                                                      information of input buffer
 *  @param [in]  bufParamsErrCoefs        :  Pointer to the structure containing error coefficients
 *                                                      information of input buffer
 *  @param [out] bufParamsOutIndex        :  Pointer to the structure containing
 *                                                      information of ouput buffer
 *  @param [out] bufParamsOutVal          :  Pointer to the structure containing
 *                                                      information of ouput buffer
 *  @param [in]  pKerInitArgs             :  Pointer to the structure holding init parameters
 *
 *  @return        Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
DSPLIB_STATUS DSPLIB_minerror_init_cn(DSPLIB_kernelHandle   handle,
                                      DSPLIB_bufParams2D_t *bufParamsIn,
                                      DSPLIB_bufParams1D_t *bufParamsErrCoefs,
                                      const int            *pMaxIndex,
                                      const void           *pMaxVal);

#endif /* DSPLIB_MINERROR_IXX_IXX_OXX_PRIV_H_ */
