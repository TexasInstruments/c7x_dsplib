// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_QRD_INVERSE_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_QRD_INVERSE_IXX_IXX_OXX_PRIV_H_

#include "../DSPLIB_matMul/DSPLIB_matMul_priv.h"
#include "../DSPLIB_matTrans/DSPLIB_matTrans_priv.h"
#include "../common/DSPLIB_utility.h"
#include "DSPLIB_qrd_common.h"
#include "DSPLIB_qrd_inverse.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_qrd_inverse_PrivArgs structure.
 *
 */
#define DSPLIB_QRD_INVERSE_IXX_IXX_OXX_PBLOCK_SIZE (6 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_qrd_inverse_exec_ci
 *         and @ref DSPLIB_qrd_inverse_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_qrd_inverse_exec)(DSPLIB_kernelHandle handle,
                                                     void *restrict pQ,
                                                     void *restrict pR,
                                                     void *restrict pInvA,
                                                     void *restrict pInvAScratch,
                                                     void *restrict pScratch);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_qrd_inverse_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_qrd_inverse_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_qrd_inverse_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_qrd_inverse_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle             :  Active handle to the kernel
 *  @param [in]  bufParamsQ         :  Pointer to the structure containing dimensional
 *                                     information of Q matrix
 *  @param [in]  bufParamsR         :  Pointer to the structure containing dimensional
 *                                     information of R matrix
 *  @param [in]  bufParamsInvA      :  Pointer to the structure containing dimensional
 *                                     information of A inverse buffer matrix
 *  @param [out] bufParamsInvAFinal :  Pointer to the structure containing dimensional
 *                                     information of final A inverse matrix.
 *  @param [in]  pKerInitArgs       :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_inverse_init_ci(DSPLIB_kernelHandle          handle,
                                         DSPLIB_bufParams2D_t        *bufParamsQ,
                                         DSPLIB_bufParams2D_t        *bufParamsR,
                                         DSPLIB_bufParams2D_t        *bufParamsInvA,
                                         DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                         const DSPLIB_qrdInvInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_qrd_inverse_exec.
 *
 * @details The function implements the optimized code for computing inverse of input matrix A.
 *
 *  @param [in]  handle     :  Active handle to the kernel
 *  @param [in]  pQ         :  Pointer to matrix Q[NRows*NRows]
 *  @param [in]  pR         :  Pointer to upper triangular matrix R[NRows*NCols]
 *  @param [in]  pInvA      :  Pointer to A inverse buffer matrix
 *  @param [out] pInvAScratch : Pointer to the Scratch Buffer
 *  @param [in] pScratch : Pointer to Scratch buffer
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_qrd_inverse_exec_ci(DSPLIB_kernelHandle handle,
                                                void *restrict pQ,
                                                void *restrict pR,
                                                void *restrict pInvA,
                                                void *restrict pInvAScratch,
                                                void *restrict pScratch);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_qrd_inverse_init.
 *
 * @details
 *
 *  @param [in]  handle             :  Active handle to the kernel
 *  @param [in]  bufParamsQ         :  Pointer to the structure containing dimensional
 *                                     information of Q matrix
 *  @param [in]  bufParamsR         :  Pointer to the structure containing dimensional
 *                                     information of R matrix
 *  @param [in]  bufParamsInvA      :  Pointer to the structure containing dimensional
 *                                     information of A inverse buffer matrix
 *  @param [out] bufParamsInvAFinal :  Pointer to the structure containing dimensional
 *                                     information of final A inverse matrix.
 *  @param [in]  pKerInitArgs       :  Pointer to the structure holding init parameters
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_inverse_init_cn(DSPLIB_kernelHandle          handle,
                                         DSPLIB_bufParams2D_t        *bufParamsQ,
                                         DSPLIB_bufParams2D_t        *bufParamsR,
                                         DSPLIB_bufParams2D_t        *bufParamsInvA,
                                         DSPLIB_bufParams2D_t        *bufParamsInvAFinal,
                                         const DSPLIB_qrdInvInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_qrd_inverse_exec.
 *
 * @details
 *
 *  @param [in]  handle     :  Active handle to the kernel
 *  @param [in]  pQ         :  Pointer to matrix Q[NRows*NRows]
 *  @param [in]  pR         :  Pointer to upper triangular matrix R[NRows*NCols]
 *  @param [in]  pInvA      :  Pointer to A inverse buffer matrix
 *  @param [out] pInvAScratch : Pointer to the Scratch Buffer
 *  @param [in] pScratch : Pointer to Scratch buffer
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_qrd_inverse_exec_cn(DSPLIB_kernelHandle handle,
                                                void *restrict pQ,
                                                void *restrict pR,
                                                void *restrict pInvA,
                                                void *restrict pInvAScratch,
                                                void *restrict pScratch);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_qrd_inverse_exec_cn and
    *         @ref DSPLIB_qrd_inverse_exec_ci.                        */
   pFxnDSPLIB_qrd_inverse_exec execute;
   /*! @brief Size of input buffer for different batches
    *         @ref DSPLIB_qrd_inverse_init that will be retrieved
    *         and used by @ref DSPLIB_qrd_inverse_exec                */
   /*! @brief Width of input data matrix                           */
   uint32_t widthR;
   /*! @brief Height of input data matrix                          */
   uint32_t heightR;
   /*! @brief Stride between rows of Q output data matrix            */
   int32_t strideQ;
   /*! @brief Stride between rows of R output data matrix            */
   int32_t strideR;
   /*! @brief Stride between rows of input data matrix             */
   int32_t strideInvA;
   /** @brief Privargs for the matTrans kernel **/
   DSPLIB_matTrans_PrivArgs pMatTransKerPrivArgs;
   /** @brief Privargs for the matMul kernel **/
   DSPLIB_matMul_PrivArgs pMatMulKerPrivArgs;
   /*! @brief Buffer to save SE & SA configuration parameters      */
   uint8_t bufPblock[DSPLIB_QRD_INVERSE_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_qrd_inverse_PrivArgs;

#endif /* DSPLIB_QRD_INVERSE_IXX_IXX_OXX_PRIV_H_ */
