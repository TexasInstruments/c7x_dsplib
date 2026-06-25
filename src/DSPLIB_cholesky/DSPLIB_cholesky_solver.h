// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_CHOLESKY_SOLVER_IXX_IXX_OXX_H_
#define DSPLIB_CHOLESKY_SOLVER_IXX_IXX_OXX_H_

// #include "../../common/DSPLIB_types.h"
#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_cholesky_solver DSPLIB_cholesky_solver
 * @brief Kernel for computing Cholesky Solver
 *
 *  @details     - This function solves \f$A \times x = b\f$ for \f$x\f$ by forward
 *                 and backward substitution methods using the decomposed upper triangular
 *                 matrix \f$U\f$ obtained from  Cholesky Decomposition as shown in the
 *                 following steps.
 *          \f{eqnarray*}{
 *              &1.&\;A \times x \;=\;  L \times U \times x \;=\; U^{T} \times U \times x \;=\; b\\
 *              &2.&\;y  =  U^{-T} \times b  \qquad done\;using\;Forward\;Substitution\\
 *              &3.&\;x  =  U^{-1} \times y  \qquad done\;using\;Backward\;Substitution\\ \f}
 *  *
 * @ingroup  DSPLIB
 */
/**@{*/

/*!
 * @brief Structure containing the parameters to initialize the kernel
 */
typedef struct {
   /*! @brief Variant of the function refer to @ref DSPLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /*! @brief order of input data*/
   int32_t order;
   /*! @brief stride of input/output data in ci*/
   int32_t stride;
   /*! @brief stride of input/output data in cn*/
   int32_t strideCn;

} DSPLIB_cholesky_solver_InitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_cholesky_solver_getHandleSize(DSPLIB_cholesky_solver_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_cholesky_solver_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the MMA
 *               accelerator and the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle            :  Active handle to the kernel
 *  @param [in]  bufParamsU        :  Pointer to the structure containing dimensional
 *                                    information of input U matrix
 *  @param [in]  bufParamsScratch  :  Pointer to the structure containing dimensional
 *                                    information of scratch buffer
 *  @param [in]  bufParamsY        :  Pointer to the structure containing dimensional
 *                                    information of Y vector
 *  @param [in]  bufParamsB        :  Pointer to the structure containing dimensional
 *                                    information of B vector
 *  @param [out]  bufParamsX       :  Pointer to the structure containing dimensional
 *                                    information of output X vector
 *  @param [in]  bufParamsDiv      :  Pointer to the structure containing dimensional
 *                                    information of Div vector
 *  @param [in]  pKerInitArgs      :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_cholesky_solver_init(DSPLIB_kernelHandle                    handle,
                                          DSPLIB_bufParams2D_t                  *bufParamsU,
                                          DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                          DSPLIB_bufParams1D_t                  *bufParamsY,
                                          DSPLIB_bufParams1D_t                  *bufParamsB,
                                          DSPLIB_bufParams1D_t                  *bufParamsX,
                                          DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                          const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_cholesky_solver_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_cholesky_solver_init, and this function
 *               must be called before the
 *               @ref DSPLIB_cholesky_solver_init is called.
 *
 *  @param [in]  handle            :  Active handle to the kernel
 *  @param [in]  bufParamsU        :  Pointer to the structure containing dimensional
 *                                    information of input U matrix
 *  @param [in]  bufParamsScratch  :  Pointer to the structure containing dimensional
 *                                    information of scratch buffer
 *  @param [in]  bufParamsY        :  Pointer to the structure containing dimensional
 *                                    information of Y vector
 *  @param [in]  bufParamsB        :  Pointer to the structure containing dimensional
 *                                    information of B vector
 *  @param [out]  bufParamsX       :  Pointer to the structure containing dimensional
 *                                    information of output X vector
 *  @param [in]  bufParamsDiv      :  Pointer to the structure containing dimensional
 *                                    information of Div vector
 *  @param [in]  pKerInitArgs      :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_cholesky_solver_init_checkParams(DSPLIB_kernelHandle                    handle,
                                                      DSPLIB_bufParams2D_t                  *bufParamsU,
                                                      DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                                      DSPLIB_bufParams1D_t                  *bufParamsY,
                                                      DSPLIB_bufParams1D_t                  *bufParamsB,
                                                      DSPLIB_bufParams1D_t                  *bufParamsX,
                                                      DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                                      const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_cholesky_solver_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_cholesky_solver_init, and this function
 *               must be called before the
 *               @ref DSPLIB_cholesky_solver_init is called.
 *
 *  @param [in]   handle   : Active handle to the kernel
 *  @param [in]   pU       : Pointer to buffer holding the input data U
 *  @param [in]   pScratch : Pointer to scratch buffer
 *  @param [in]   pY       : Pointer to buffer holding the intermediate vector Y
 *  @param [in]   pB       : Pointer to buffer holding the input vector B
 *  @param [out]  pX       : Pointer to buffer holding the output data X
 *  @param [in]   pDiv     : Pointer to buffer holding the intermediate data pDiv
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_cholesky_solver_exec_checkParams(DSPLIB_kernelHandle handle,
                                                      const void *restrict pU,
                                                      const void *restrict pScratch,
                                                      const void *restrict pY,
                                                      const void *restrict pB,
                                                      const void *restrict pX,
                                                      const void *restrict pDiv);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @param [in]  handle   : Active handle to the kernel
 *  @param [in]  pU       : Pointer to buffer holding the input data U[order][order]
 *  @param [in]  pScratch : Pointer to scratch buffer
 *  @param [in]  pY       : Pointer to buffer holding the intermediate vector Y[order]
 *  @param [in]  pB       : Pointer to buffer holding the input vector B[order]
 *  @param [in]  pX       : Pointer to buffer holding the output data X[order]
 *  @param [in]  pDiv     : Pointer to buffer holding the intermediate data pDiv[order]
 *
 *  @par Memory Requirements
 * | Buffer   | dimY | dimX | Comments |
 * | :--      | :--: | :--: | :-----|
 * | pU       | M    | M    | Input Upper Triangular Matrix |
 * | pScratch | M    | M    | Scratch Buffer to store transpose of U |
 * | pY       | 1    | M    | Intermediate solution vector |
 * | pB       | 1    | M    | Input Vector |
 * | pX       | 1    | M    | Output Vector |
 * | pDiv     | 1    | M    | Scratch buffer to store reciprocals of diagonal elements |
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Assumptions:
 *    - None
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - The input and output data buffers are expected to be in L2 memory
 *    - The buffer pointers are assumed to be 64-byte aligned
 *    - Ensure that the starting point of adjacent rows in a 2-D matrix are allocated from
 *      different banks in L2 memory with the help of memory padding.
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref DSPLIB_cholesky_solver_init and
 *               @ref DSPLIB_cholesky_solver_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */
DSPLIB_STATUS DSPLIB_cholesky_solver_exec(DSPLIB_kernelHandle handle,
                                          void *restrict pU,
                                          void *restrict pScratch,
                                          void *restrict pY,
                                          void *restrict pB,
                                          void *restrict pX,
                                          void *restrict pDiv);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_CHOLESKY_SOLVER_IXX_IXX_OXX_H_ */
