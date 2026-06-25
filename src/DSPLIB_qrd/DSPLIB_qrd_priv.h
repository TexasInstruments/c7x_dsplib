// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_QRD_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_QRD_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_inlines.h"
#include "../common/DSPLIB_utility.h"
#include "DSPLIB_qrd.h"
#include "DSPLIB_qrd_common.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_qrd_PrivArgs structure.
 *
 */
#define DSPLIB_QRD_IXX_IXX_OXX_PBLOCK_SIZE (20 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_qrd_exec_ci
 *         and @ref DSPLIB_qrd_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_qrd_exec)(DSPLIB_kernelHandle handle,
                                             const void *restrict pA,
                                             const void *restrict pQ,
                                             const void *restrict pR,
                                             const void *restrict pU,
                                             const void *restrict pScratch);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_qrd_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_qrd_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_qrd_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_qrd_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsA   :  Pointer to the structure containing dimensional
 *                               information of input matrix
 *  @param [out] bufParamsQ   :  Pointer to the structure containing dimensional
 *                               information of Q matrix
 *  @param [out] bufParamsR   :  Pointer to the structure containing dimensional
 *                               information of R matrix
 *  @param [in]  bufParamsU   :  Pointer to the structure containing dimensional
 *                               information of temporary vector U.
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

template <typename dataType>
extern DSPLIB_STATUS DSPLIB_qrd_init_ci(DSPLIB_kernelHandle       handle,
                                        DSPLIB_bufParams2D_t     *bufParamsA,
                                        DSPLIB_bufParams2D_t     *bufParamsQ,
                                        DSPLIB_bufParams2D_t     *bufParamsR,
                                        DSPLIB_bufParams1D_t     *bufParamsU,
                                        const DSPLIB_qrdInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_qrd_exec.
 *
 * @details The function permforms the optimized QR decomposition.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pA     :  Pointer to the input matrix
 *  @param [out] pQ     :  Pointer to the output matrix Q
 *  @param [out] pR     :  Pointer to the output matrix R
 *  @param [in]  pU     :  Pointer to the temporary vector
 *  @param [in]  pScratch:  Pointer to the Scratch buffer
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
extern DSPLIB_STATUS DSPLIB_qrd_exec_ci(DSPLIB_kernelHandle handle,
                                        const void *restrict pA,
                                        const void *restrict pQ,
                                        const void *restrict pR,
                                        const void *restrict pU,
                                        const void *restrict pScratch);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_qrd_init.
 *
 * @details
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsA   :  Pointer to the structure containing dimensional
 *                               information of input matrix
 *  @param [out] bufParamsQ   :  Pointer to the structure containing dimensional
 *                               information of Q matrix
 *  @param [out] bufParamsR   :  Pointer to the structure containing dimensional
 *                               information of R matrix
 *  @param [in]  bufParamsU   :  Pointer to the structure containing dimensional
 *                               information of temporary vector U.
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

template <typename dataType>
extern DSPLIB_STATUS DSPLIB_qrd_init_cn(DSPLIB_kernelHandle       handle,
                                        DSPLIB_bufParams2D_t     *bufParamsA,
                                        DSPLIB_bufParams2D_t     *bufParamsQ,
                                        DSPLIB_bufParams2D_t     *bufParamsR,
                                        DSPLIB_bufParams1D_t     *bufParamsU,
                                        const DSPLIB_qrdInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_qrd_exec.
 *
 * @details
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pA     :  Pointer to the input matrix
 *  @param [out] pQ     :  Pointer to the output matrix Q
 *  @param [out] pR     :  Pointer to the output matrix R
 *  @param [in]  pU     :  Pointer to the temporary vector
 *  @param [in]  pScratch:  Pointer to the Scratch buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_qrd_exec_cn(DSPLIB_kernelHandle handle,
                                        const void *restrict pA,
                                        const void *restrict pQ,
                                        const void *restrict pR,
                                        const void *restrict pU,
                                        const void *restrict pScratch);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_qrd_exec_cn and
    *         @ref DSPLIB_qrd_exec_ci.                        */
   pFxnDSPLIB_qrd_exec execute;
   /*! @brief Size of input buffer for different batches
    *         @ref DSPLIB_qrd_init that will be retrieved
    *         and used by @ref DSPLIB_qrd_exec                */
   /*! @brief Width of input data matrix                      */
   uint32_t widthA;
   /*! @brief Height of input data matrix                     */
   uint32_t heightA;
   /*! @brief Stride between rows of input data matrix        */
   int32_t strideA;
   /*! @brief Stride between rows of Q output data matrix     */
   int32_t strideQ;
   /*! @brief Stride between rows of R output data matrix     */
   int32_t strideR;
   /*! @brief Buffer to save SE & SA configuration parameters */
   uint8_t bufPblock[DSPLIB_QRD_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_qrd_PrivArgs;

template <typename dataType> void DSPLIB_qrd_blk_move_init_ci(DSPLIB_kernelHandle handle);
template <typename dataType> void DSPLIB_qrd_alpha_init_ci(DSPLIB_kernelHandle handle);
template <typename dataType> void DSPLIB_R_column_init_ci(DSPLIB_kernelHandle handle);
template <typename dataType> void DSPLIB_Q_matrix_init_ci(DSPLIB_kernelHandle handle);

template <typename dataType>
void DSPLIB_qrd_R_matrix_exec_ci(dataType *pLocalR,
                                 dataType *pLocalU,
                                 dataType *pSum,
                                 dataType  scale,
                                 int32_t   colStrideR,
                                 int32_t   nRows,
                                 int32_t   nCols,
                                 uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_qrd_Q_matrix_exec_ci(dataType *pLocalQ,
                                 dataType *pLocalU,
                                 dataType *pSum,
                                 dataType  scale,
                                 int32_t   colStrideQ,
                                 int32_t   nRows,
                                 int32_t   nCols,
                                 uint8_t  *pBlock);

#endif /* DSPLIB_QRD_IXX_IXX_OXX_PRIV_H_ */
