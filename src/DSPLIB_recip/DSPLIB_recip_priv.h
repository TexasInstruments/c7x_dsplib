// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_RECIP_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_RECIP_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_recip.h"
#include "DSPLIB_types.h"

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_recip_PrivArgs structure.
 *
 */
#define DSPLIB_RECIP_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE + 1 * DSPLIB_PARAM_SIZE + 2 * sizeof(int32_t))

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_recip_exec_ci
 *         and @ref DSPLIB_recip_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_recip_exec)(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_recip_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_recip_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_recip_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_recip_exec_ci does not lose cycles
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
extern DSPLIB_STATUS DSPLIB_recip_init_ci(DSPLIB_kernelHandle          handle,
                                          const DSPLIB_bufParams1D_t  *bufParamsIn,
                                          const DSPLIB_bufParams1D_t  *bufParamsOut,
                                          const DSPLIB_recip_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_recip_exec.
 *

* @details The function uses MMA hardware accelerator to perform the
 *          convolution computation. Filter data is loaded into B panel of the
 *          MMA from memory using one streaming engine, while the input data is
 *          loaded into A vectors of the MMA using the other streaming engine.
 *          Result of the compute from MMA C panel is stored into memory using
 *          a stream sqrress generator.
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
template <typename dataType, int32_t dataIn>
extern DSPLIB_STATUS DSPLIB_recip_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_recip_exec.
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
template <typename dataType, int32_t dataIn>
extern DSPLIB_STATUS DSPLIB_recip_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /** @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_recip_exec_cn and
    *         @ref DSPLIB_recip_exec_ci.                        */
   pFxnDSPLIB_recip_exec execute;
   /** @brief Size of input buffer for different batches
    *         @ref DSPLIB_recip_init that will be retrieved
    *         and used by @ref DSPLIB_recip_exec                */
   int32_t blockSize;
   uint8_t bufPblock[DSPLIB_RECIP_IXX_IXX_OXX_PBLOCK_SIZE];

   // init args dsplib_recip
   DSPLIB_recip_InitArgs initArgs;
} DSPLIB_recip_PrivArgs;

// declare and define permute operation masks
/* #if defined(_HOST_BUILD) */

/* // clang-format off */
/* #if __C7X_VEC_SIZE_BYTES__ == 64 */
/* const c7x::uchar_vec DSPLIB_vperm_mask_hstack = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
 * 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
 * 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63); */

/* const c7x::uchar_vec DSPLIB_vperm_mask_interleave = c7x::uchar_vec(0, 1, 32, 33, 2, 3, 34, 35, 4, 5, 36, 37, 6, 7,
 * 38, 39, 8, 9, 40, 41, 10, 11, 42, 43, 12, 13, 44, 45, 14, 15, 46, 47, 16, 17, 48, 49, 18, 19, 50, 51, 20, 21, 52, 53,
 * 22, 23, 54, 55, 24, 25, 56, 57, 26, 27, 58, 59, 28, 29, 60, 61, 30, 31, 62, 63); */

/* #elif __C7X_VEC_SIZE_BYTES__ == 32 */
/* const c7x::uchar_vec DSPLIB_vperm_mask_hstack = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
 * 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31); */

/* const c7x::uchar_vec DSPLIB_vperm_mask_interleave = c7x::uchar_vec(0, 1, 16, 17, 2, 3, 18, 19, 4, 5, 20, 21, 6, 7,
 * 22, 23, 8, 9, 24, 25, 10, 11, 26, 27, 12, 13, 28, 29, 14, 15, 30, 31); */

/* #endif /\* #if __C7X_VEC_SIZE_BYTES__ *\/ */

/* #else /\* not host build *\/ */
/* #if __C7X_VEC_SIZE_BYTES__ == 64 */
/* const c7x::uchar_vec DSPLIB_vperm_mask_hstack = (const c7x::uchar_vec)(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
 * 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
 * 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63); */

/* const c7x::uchar_vec DSPLIB_vperm_mask_interleave = (const c7x::uchar_vec)(0, 1, 32, 33, 2, 3, 34, 35, 4, 5, 36, 37,
 * 6, 7, 38, 39, 8, 9, 40, 41, 10, 11, 42, 43, 12, 13, 44, 45, 14, 15, 46, 47, 16, 17, 48, 49, 18, 19, 50, 51, 20, 21,
 * 52, 53, 22, 23, 54, 55, 24, 25, 56, 57, 26, 27, 58, 59, 28, 29, 60, 61, 30, 31, 62, 63); */

/* #elif __C7X_VEC_SIZE_BYTES__ == 32 */
/* const c7x::uchar_vec DSPLIB_vperm_mask_hstack = (const c7x::uchar_vec)(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
 * 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31); */

/* const c7x::uchar_vec DSPLIB_vperm_mask_interleave = (const c7x::uchar_vec)(0, 1, 4, 5, 8, 9, 12, 13, 16, 17, 20, 21,
 * 24, 25, 28, 29, 2, 3, 6, 7, 10, 11, 14, 15, 18, 19, 22, 23, 26, 27, 30, 31); */

/* #endif /\* #if __C7X_VEC_SIZE_BYTES__ *\/ */

/* #endif /\* _HOST_BUILD *\/ */
/* // clang-format on */

/**
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_recip_init.
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
DSPLIB_recip_init_cn(DSPLIB_kernelHandle handle, DSPLIB_bufParams2D_t *bufParamsIn, DSPLIB_bufParams2D_t *bufParamsOut);

#endif /* DSPLIB_RECIP_IXX_IXX_OXX_PRIV_H_ */
