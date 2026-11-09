// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0


#ifndef DSPLIB_complexMatrixMultiply_IXX_IXX_OXX_H_
#define DSPLIB_complexMatrixMultiply_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define COMPLEX_MUL 1

  /**
   * \ingroup DSPLIB_KERNELS
   */

  /**
   * @defgroup DSPLIB_complexMatrixMultiply DSPLIB_complexMatrixMultiply
   * @brief Kernel for computing complex matrix multiplication
   *
   *
   * @details
   *  This kernel multiplies two complex matrices. As is expected with matrix
   *  multiplication, the number of columns of input A must match the number of
   *  rows of input B. Data is stored interleaved (real, imaginary).
   *
   * @par Method
   * For \f$A \in \mathbb{C}^{M \times K}\f$ and \f$B \in \mathbb{C}^{K \times N}\f$,
   * each output element is
   * \f[
   *   C_{m,n} = \sum_{k=0}^{K-1} A_{m,k}\, B_{k,n}
   * \f]
   * which, in terms of real and imaginary parts, is
   * \f[
   *   \Re(C_{m,n}) = \sum_{k=0}^{K-1} \left( \Re(A_{m,k})\Re(B_{k,n}) - \Im(A_{m,k})\Im(B_{k,n}) \right)
   * \f]
   * \f[
   *   \Im(C_{m,n}) = \sum_{k=0}^{K-1} \left( \Re(A_{m,k})\Im(B_{k,n}) + \Im(A_{m,k})\Re(B_{k,n}) \right)
   * \f]
   * For 16-bit data the accumulator is scaled/shifted by the \c shift and
   * \c scaleValue parameters and saturated; 32-bit data requires
   * \c scaleValue == 1.
   *
   * @note Platform limitations: the optimized implementation uses the MMA, whose
   *       width depends on data type (see DSPLIB_MMA_SIZE_16_BIT /
   *       DSPLIB_MMA_SIZE_32_BIT). Strides are in bytes and must cover
   *       dim_x complex elements (2 * sizeof(element) * dim_x).
   *
   * @ingroup  DSPLIB
   */

  /**@{*/

  /**
   * @brief Structure containing kernel parameters
   */

  typedef struct
  {
    int16_t shift;
    int16_t scaleValue;
  } DSPLIB_complexMatrixMultiply_Params;

  /**
   * @brief Structure containing the parameters to initialize the kernel
   */

  typedef struct
  {
    /** @brief Variant of the function, refer to @ref DSPLIB_FUNCTION_STYLE     */
    int8_t funcStyle;
    DSPLIB_complexMatrixMultiply_Params params;
  } DSPLIB_complexMatrixMultiply_InitArgs;

  /**
   *  @brief        This is a query function to calculate the size of internal
   *                handle
   *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
   *  @return       Size of the buffer in bytes
   *  @remarks      Application is expected to allocate buffer of the requested
   *                size and provide it as input to other functions requiring it.
   */
  int32_t DSPLIB_complexMatrixMultiply_getHandleSize(DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs);

  /*!
   *  @brief       This is a utility function that gives an estimate of the cycles
   *               consumed for the kernel execution.
   *
   *  @param [in]  handle       :  Active handle to the kernel
   *                               information of the output buffer
   *  @param [in]  bufParamsInA :  Pointer to the structure containing dimensional
   *                               information of input matrix A
   *  @param [in]  bufParamsInB :  Pointer to the structure containing dimensional
   *                               information of input matrix B
   *  @param [out] archCycles   :  Cycles estimated for the compute, startup and
   *                               teardown
   *  @param [out] estCycles    :  Cycles estimated for the compute, startup,
   *                               teardown and any associated overhead
   *  @remarks     None
   */
  void DSPLIB_complexMatrixMultiply_perfEst(DSPLIB_kernelHandle handle,
                                              const DSPLIB_bufParams3D_t *bufParamsInA,
                                              const DSPLIB_bufParams3D_t *bufParamsInB,
                                              uint64_t *archCycles,
                                              uint64_t *estCycles);

  /**
   *  @brief       This function should be called before the
   *               @ref DSPLIB_complexMatrixMultiply_exec function is called. This
   *               function takes care of any one-time operations such as setting
   * up the configuration of required hardware resources such as the streaming engine.  The results of these
   *               operations are stored in the handle.
   *
   *  @param [in]  handle       :  Active handle to the kernel
   *  @param [in]  bufParamsInA :  Pointer to the structure containing dimensional
   *                               information of input matrix A
   *  @param [in]  bufParamsInB :  Pointer to the structure containing dimensional
   *                               information of input matrix B
   *  @param [in]  bufParamsOut :  Pointer to the structure containing dimensional
   *                               information of magnitude output
   *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
   *                               parameters
   *
   *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
   *
   *  @remarks     Application is expected to provide a valid handle.
   */

  DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init(DSPLIB_kernelHandle handle,
                                                      const DSPLIB_bufParams3D_t *bufParamsInA,
                                                      const DSPLIB_bufParams3D_t *bufParamsInB,
                                                      const DSPLIB_bufParams3D_t *bufParamsOut,
                                                      const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs);

  /**
   *  @brief       This function checks the validity of the parameters passed to
   *               @ref DSPLIB_complexMatrixMultiply_init function. This function
   *               is called with the same parameters as the
   *               @ref DSPLIB_complexMatrixMultiply_init, and this function
   *               must be called before the
   *               @ref DSPLIB_complexMatrixMultiply_init is called.
   *
   *  @param [in]  handle       :  Active handle to the kernel
   *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
   *                               parameters
   *
   *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
   *
   *
   *  @remarks     None
   */

  DSPLIB_STATUS
  DSPLIB_complexMatrixMultiply_init_checkParams(DSPLIB_kernelHandle handle,
                                                  const DSPLIB_bufParams3D_t *bufParamsInA,
                                                  const DSPLIB_bufParams3D_t *bufParamsInB,
                                                  const DSPLIB_bufParams3D_t *bufParamsOut,
                                                  const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs);

  /**
   *  @brief       This function checks the validity of the parameters passed to
   *               @ref DSPLIB_complexMatrixMultiply_exec function. This function
   *               is called with the same parameters as the
   *               @ref DSPLIB_complexMatrixMultiply_init, and this function
   *               must be called before the
   *               @ref DSPLIB_complexMatrixMultiply_init is called.
   *
   *
   *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
   *
   *  @remarks     None
   */

  DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_checkParams(DSPLIB_kernelHandle handle,
                                                                  const void *restrict pInA,
                                                                  const void *restrict pInB,
                                                                  const void *restrict pOut);

  /**
   *  @brief       This function is the main kernel compute function.
   *
   *  @details     Please refer to details under
   *               @ref DSPLIB_complexMatrixMultiply_exec
   *
   *  @param [in]  handle    :  Active handle to the kernel
   *  @param [in]  pInA      :  Pointer to the 2d input data matrix A
   *  @param [in]  pInB      :  Pointer to the 2d input data matrix B
   *  @param [out] pOut      :  Pointer to buffer holding 2d output
   *
   *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
   *
   *  @par Assumptions:
   *    - None
   *
   *  @par Performance Considerations:
   *    For best performance,
   *    - the input and output data buffers are expected to be in L2 memory
   *    - the buffer pointers are assumed to be 64-byte aligned
   *
   *  @remarks     Before calling this function, application is expected to call
   *               @ref DSPLIB_complexMatrixMultiply_init and
   *               @ref DSPLIB_complexMatrixMultiply_exec_checkParams functions.
   *               This ensures resource configuration and error checks are done
   * only once for several invocations of this function.
   */

  DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec(DSPLIB_kernelHandle handle,
                                                      const void *restrict pInA,
                                                      const void *restrict pInB,
                                                      void *restrict pOut);

  /** @} */

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_complexMatrixMultiply_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  DSPLIB_complexMatrixMultiply.h                             */
/* ======================================================================== */
