// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_MATMUL_FIXED_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_MATMUL_FIXED_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_matMul_fixed.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_matMul_fixed_PrivArgs structure.
 *
 */

#define DSPLIB_MATMUL_FIXED_IXX_IXX_OXX_PBLOCK_SIZE (3 * SE_PARAM_SIZE)
#if defined(__C7504__) || defined(__C7524__)
#define DSPLIB_MATMAPY_FXD_I8S_O8S int8_t, c7x::char_hvec
#define DSPLIB_MATMAPY_FXD_I16S_O16S int16_t, c7x::short_vec
#else
#define DSPLIB_MATMAPY_FXD_I8S_O8S int8_t, c7x::int_vec
#define DSPLIB_MATMAPY_FXD_I16S_O16S int16_t, c7x::long_vec
#endif
/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_matMul_fixed_PrivArgs structure.
 *
 */

/* #define DSPLIB_matMul_fixed_SE_UNROLL_FACTOR (8) */

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_matMul_fixed_exec_ci
 *         and @ref DSPLIB_matMul_fixed_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_matMul_fixed_exec)(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn1,
                                                      void *restrict pIn2,
                                                      void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_matMul_fixed_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_matMul_fixed_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_matMul_fixed_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_matMul_fixed_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                               information of input buffer 1
 *  @param [in]  bufParamsIn2 :  Pointer to the structure containing dimensional
 *                               information of input buffer 2
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_matMul_fixed_init_ci(DSPLIB_kernelHandle                 handle,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                          const DSPLIB_bufParams2D_t         *bufParamsOut,
                                          const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_matMul_fixed_exec.
 *
 * @details The function uses MMA hardware accelerator to perform the
 *          convolution computation. Filter data is loaded into B panel of the
 *          MMA from memory using one streaming engine, while the input data is
 *          loaded into A vectors of the MMA using the other streaming engine.
 *          Result of the compute from MMA C panel is stored into memory using
 *          a stream matMul_fixedress generator.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
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

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS
DSPLIB_matMul_fixed_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_matMul_fixed_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilterCoeff: Pointer to buffer holding the filter coefficient
 * data
 *  @param [in]  pFilterVar  : Pointer to buffer holding the filter variable
 * data
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_matMul_fixed_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_matMul_fixed_exec_cn and
    *         @ref DSPLIB_matMul_fixed_exec_ci.                        */
   pFxnDSPLIB_matMul_fixed_exec execute;
   int32_t                      M;
   int32_t                      N;
   int32_t                      K;
   int32_t                      qs;
   int32_t                      strideIn0Elements;
   int32_t                      strideIn1Elements;
   int32_t                      strideOutElements;
   int32_t                      MBlocks;
   int32_t                      KBlocks;
   int32_t                      NBlocks;
   uint8_t                      bufPblock[DSPLIB_MATMUL_FIXED_IXX_IXX_OXX_PBLOCK_SIZE];

} DSPLIB_matMul_fixed_PrivArgs;

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_matMul_fixed_init.
 *
 * @details
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsIn            :  Pointer to the structure containing
 * dimensional information of input buffer
 *  @param [out] bufParamsOut           :  Pointer to the structure containing
 * dimensional information of ouput buffer
 *  @param [in]  pKerInitArgs           :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
DSPLIB_STATUS
DSPLIB_matMul_fixed_init_cn(DSPLIB_kernelHandle   handle,
                            DSPLIB_bufParams2D_t *bufParamsIn0,
                            DSPLIB_bufParams2D_t *bufParamsIn1,
                            DSPLIB_bufParams2D_t *bufParamsOut);

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_matMul_fixed_generic_init_ci(DSPLIB_kernelHandle                 handle,
                                                  const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                  const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                  const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                  const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_matMul_fixed_generic_exec_ci(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn0,
                                                  void *restrict pIn1,
                                                  void *restrict pOut);

template <typename dataType> void printStridedMat(char *inStr, int Nrows, int colStride, dataType *inMat)
{
   printf("Nrwo: %d, colStride: %d\n", Nrows, colStride);
   printf("%s:\n", inStr);
   dataType *printMat = inMat;
   for (int i = 0; i < Nrows; i++) {
      for (int j = 0; j < colStride; j++) {
         printf("%8d", (int16_t) *printMat++);
      }
      printf("\n");
   }
   printf("\n");
}
template void printStridedMat<int16_t>(char *inStr, int Nrows, int colStride, int16_t *inMat);
template void printStridedMat<int8_t>(char *inStr, int Nrows, int colStride, int8_t *inMat);
// template void printStridedMat<float>(char *inStr, int Nrows, int colStride, float *inMat);
// template void printStridedMat<double>(char *inStr, int Nrows, int colStride, double *inMat);

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_fixed_generic_init_ci(DSPLIB_kernelHandle                 handle,
                                                  const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                  const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                  const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                  const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_matMul_fixed_generic_exec_ci(DSPLIB_kernelHandle handle,
                                                  void *restrict pIn0,
                                                  void *restrict pIn1,
                                                  void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_fixed_N_unroll_init_ci(DSPLIB_kernelHandle                 handle,
                                                   const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                   const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                   const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                   const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_fixed_N_unroll_exec_ci(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn0,
                                                   void *restrict pIn1,
                                                   void *restrict pOut);

#endif /* DSPLIB_MATMUL_FIXED_IXX_IXX_OXX_PRIV_H_ */
