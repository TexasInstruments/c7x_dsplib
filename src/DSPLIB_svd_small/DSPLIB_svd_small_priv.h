// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_SVD_SMALL_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_SVD_SMALL_IXX_IXX_OXX_PRIV_H_

// #include "../DSPLIB_matTrans/DSPLIB_matTrans_priv.h"
#include "../common/DSPLIB_inlines.h"
#include "../common/DSPLIB_utility.h"
#include "DSPLIB_svd_small.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_svd_PrivArgs structure.
 *
 */
#define DSPLIB_SVD_SMALL_IXX_IXX_OXX_PBLOCK_SIZE (0 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_svd_exec_ci
 *         and @ref DSPLIB_svd_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_svd_small_exec)(DSPLIB_kernelHandle handle,
                                                   void *restrict pA,
                                                   void *restrict pU,
                                                   void *restrict pV,
                                                   void *restrict pDiag,
                                                   void *restrict pSuperDiag,
                                                   void *restrict pU1);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_svd_small_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_svd_small_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_svd_small_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_svd_small_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle             : Active handle to the kernel
 *  @param [in]  bufParamsIn        : Pointer to the structure containing dimensional
 *                                    information of input buffer
 *  @param [out] bufParamsU         : Pointer to the structure containing dimensional
 *                                    information of output buffer U
 *  @param [out] bufParamsV         : Pointer to the structure containing dimensional
 *                                    information of output buffer V
 *  @param [out] bufParamsDiag      : Pointer to the structure containing dimensional
 *                                    information of output buffer Diag
 *  @param [out] bufParamsSuperDiag : Pointer to the structure containing dimensional
 *                                    information of output buffer SuperDiag
 *  @param [in]  pKerInitArgs       : Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

template <typename dataType>
extern DSPLIB_STATUS DSPLIB_svd_small_init_ci(DSPLIB_kernelHandle              handle,
                                              const DSPLIB_bufParams2D_t      *bufParamsIn,
                                              const DSPLIB_bufParams2D_t      *bufParamsU,
                                              const DSPLIB_bufParams2D_t      *bufParamsV,
                                              const DSPLIB_bufParams1D_t      *bufParamsDiag,
                                              const DSPLIB_bufParams1D_t      *bufParamsSuperDiag,
                                              const DSPLIB_svd_small_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_svd_small_exec.
 *
 * @details
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pA         : Pointer to the input buffer pA
 *  @param [out] pU         : Pointer to the output buffer pU
 *  @param [out] pV         : Pointer to the output buffer pV
 *  @param [out] pDiag      : Pointer to the output buffer pDiag
 *  @param [out] pSuperDiag : Pointer to the output buffer pSuperDiag
 *  @param [in]  pU1        : Pointer to the buffer pU1
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Performance Considerations:
 *       For best performance,
 *        - the input and output data buffers are expected to be in L2 memory.
 *        - the buffer pointers are assumed to be 64-byte aligned
 *
 * @par Assumptions:
 *       The arrays A, U, V, Diag, SuperDiag, U1, V1 and Scratch are stored in distinct arrays
 *       In-place processing of A is not allowed.
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_svd_small_exec_ci(DSPLIB_kernelHandle handle,
                                              void *restrict pA,
                                              void *restrict pU,
                                              void *restrict pV,
                                              void *restrict pDiag,
                                              void *restrict pSuperDiag,
                                              void *restrict pU1);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_svd_small_init.
 *
 * @details
 *
 *  @param [in]  handle             : Active handle to the kernel
 *  @param [in]  bufParamsIn        : Pointer to the structure containing dimensional
 *                                    information of input buffer
 *  @param [out] bufParamsU         : Pointer to the structure containing dimensional
 *                                    information of output buffer U
 *  @param [out] bufParamsV         : Pointer to the structure containing dimensional
 *                                    information of output buffer V
 *  @param [out] bufParamsDiag      : Pointer to the structure containing dimensional
 *                                    information of output buffer Diag
 *  @param [out] bufParamsSuperDiag : Pointer to the structure containing dimensional
 *                                    information of output buffer SuperDiag
 *  @param [in]  pKerInitArgs       : Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
DSPLIB_STATUS DSPLIB_svd_small_init_cn(DSPLIB_kernelHandle              handle,
                                       const DSPLIB_bufParams2D_t      *bufParamsIn,
                                       const DSPLIB_bufParams2D_t      *bufParamsU,
                                       const DSPLIB_bufParams2D_t      *bufParamsV,
                                       const DSPLIB_bufParams1D_t      *bufParamsDiag,
                                       const DSPLIB_bufParams1D_t      *bufParamsSuperDiag,
                                       const DSPLIB_svd_small_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_svd_small_exec.
 *
 * @details
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pA         : Pointer to the input buffer pA
 *  @param [out] pU         : Pointer to the output buffer pU
 *  @param [out] pV         : Pointer to the output buffer pV
 *  @param [out] pDiag      : Pointer to the output buffer pDiag
 *  @param [out] pSuperDiag : Pointer to the output buffer pSuperDiag
 *  @param [in]  pU1        : Pointer to the buffer pU1
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_svd_small_exec_cn(DSPLIB_kernelHandle handle,
                                              void *restrict pA,
                                              void *restrict pU,
                                              void *restrict pV,
                                              void *restrict pDiag,
                                              void *restrict pSuperDiag,
                                              void *restrict pU1);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_svd_small_exec_cn and
    *         @ref DSPLIB_svd_small_exec_ci.                        */
   pFxnDSPLIB_svd_small_exec execute;
   /*! @brief Size of input buffer for different batches
    *         @ref DSPLIB_svd_small_init that will be retrieved
    *         and used by @ref DSPLIB_svd_small_exec                */
   /*! @brief Width of input data matrix                           */
   uint32_t widthIn;
   /*! @brief Height of input data matrix                          */
   uint32_t heightIn;
   /*! @brief Stride between rows of input data matrix             */
   int32_t strideIn;
   /*! @brief Stride between rows of U matrix            */
   uint32_t strideU;
   /*! @brief Stride between rows of V matrix            */
   uint32_t strideV;
   uint32_t strideURows;
   uint32_t strideVRows;
   /*! @brief Flag for enabling the calculation of reduced form
              enableReducedForm = 1 for reduced form SVD calc
              enableReducedForm = 0 for normal SVD calc            */
   uint32_t enableReducedForm;
   /*! @brief Data Type */
   uint32_t data_type;
   /*! @brief Buffer to save SE & SA configuration parameters      */
   uint8_t bufPblock[DSPLIB_SVD_SMALL_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_svd_small_PrivArgs;

/* *****************************************************************************
 *
 * COMMON UTILITY FUNCTIONS
 *
 ***************************************************************************** */

/* Common function to calculate reciprocal value */
template <typename dataType> inline dataType getRecip(dataType value)
{
   dataType TwoP0 = 2.0;

   dataType yy1 = __recip(value);

   yy1 = yy1 * (TwoP0 - value * yy1);
   yy1 = yy1 * (TwoP0 - value * yy1);
#if defined(ENABLE_LDRA_COVERAGE)
   /* Higher precision calculations to enter the rotation_check path. We only use this
      as a coverage component. We do not include these operations for normal execution
      considering the PERFORMANCE */
   yy1 = yy1 * (TwoP0 - value * yy1);
   yy1 = yy1 * (TwoP0 - value * yy1);
#endif

   return yy1;
}
template float  getRecip(float value);
template double getRecip(double value);

template <typename dataType> inline dataType getSqrt(dataType a)
{
   const dataType Half  = 0.5f;
   const dataType OneP5 = 1.5f;
   dataType       x, y;

   x = __recip_sqrt(a); /* compute square root reciprocal */

   // #pragma UNROLL(1) /* PRAGMA: do not unroll this loop */
   {
      x = x * (OneP5 - (a * x * x * Half));
      x = x * (OneP5 - (a * x * x * Half));
   }
   y = a * x;

   return (y);
}
template float           getSqrt<float>(float a);
template double          getSqrt<double>(double a);
template c7x::double_vec getSqrt<c7x::double_vec>(c7x::double_vec a);

template <typename dataType> inline dataType getRecipSqrt(dataType a)
{
   const dataType Half  = 0.5f;
   const dataType OneP5 = 1.5f;
   dataType       x;

   x = __recip_sqrt(a); // compute square root reciprocal

   // PRAGMA: do not unroll this loop
   // #pragma UNROLL(1)
   {
      x = x * (OneP5 - (a * x * x * Half));
      x = x * (OneP5 - (a * x * x * Half));
   }

   return x;
}
template float  getRecipSqrt<float>(float a);
template double getRecipSqrt<double>(double a);

#endif /* DSPLIB_SVD_SMALL_IXX_IXX_OXX_PRIV_H_ */
