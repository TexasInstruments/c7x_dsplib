// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_ABSOLUTEVALUE_PRIV_H_
#define DSPLIB_ABSOLUTEVALUE_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_absoluteValue.h"

/*!
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface,
 * please see @ref DSPLIB_absoluteValue
 */

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/
#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_absoluteValue_PrivArgs structure.
 *
 */
#define DSPLIB_absoluteValue_PBLOCK_SIZE (SE_PARAM_SIZE + SA_PARAM_SIZE)

/*!
 * @brief Cycles to fill the software pipeline of the exec loop (the constant in "2 + trip_cnt * 1" in the compiled
 *        .asm, same on C7504 and C7524).
 */
#define DSPLIB_ABSOLUTEVALUE_PIPE_UP_CYCLES (2)

/*!
 * @brief Cycles per vector of the exec loop (ii from the compiled .asm, all data types, C7504 and C7524). One SE read
 *        and one SA store per cycle.
 */
#define DSPLIB_ABSOLUTEVALUE_CYCLES_PER_VECTOR (1)

/*!
 * @brief Cycles outside the loop: SEOPEN, SAOPEN, template loads and first-data latency. Mean of
 *        (Loki cycles - loop cycles) over all 123 test cases on C7504 and C7524. The measured overhead is 43 or 44
 *        cycles, so the estimate is 0 to 1 cycle above Loki.
 */
#define DSPLIB_ABSOLUTEVALUE_CALL_OVERHEAD_CYCLES (44)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_absoluteValue_exec_ci
 *         and @ref DSPLIB_absoluteValue_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_absoluteValue_exec)(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn,
                                                       void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_absoluteValue_exec_cn and
    *         @ref DSPLIB_absoluteValue_exec_ci.                        */
   pFxnDSPLIB_absoluteValue_exec execute;
   /*! @brief Width of the 2D block (X dimension) */
   int32_t width;
   /*! @brief Height of the 2D block (Y dimension) */
   int32_t height;
   /*! @brief Stride for input (in elements, not bytes) */
   uint32_t strideIn;
   /*! @brief Stride for output (in elements, not bytes) */
   uint32_t strideOut;
   /*! @brief Total number of vectors the exec loop processes (all rows) */
   int32_t numVecs;
   /*! @brief Buffer to save SE & SA configuration parameters */
   uint8_t bufPblock[DSPLIB_absoluteValue_PBLOCK_SIZE];
} DSPLIB_absoluteValue_PrivArgs;

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_absoluteValue_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_absoluteValue_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_absoluteValue_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_absoluteValue_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input buffer
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
extern DSPLIB_STATUS DSPLIB_absoluteValue_init_ci(DSPLIB_kernelHandle                 handle,
                                                  const DSPLIB_bufParams2D_t         *bufParamsIn,
                                                  const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                  const DSPLIB_absoluteValueInitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_absoluteValue_exec.
 *
 * @details The function uses a streaming engine to read the 2D data, compute absolute value,
 *          and store the result using the streaming address generator.
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
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_absoluteValue_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_absoluteValue_exec.
 *
 * @details
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
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_absoluteValue_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_absoluteValue_init.
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

#endif /* DSPLIB_absoluteValue_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  DSPLIB_absoluteValue_priv.h                                        */
/* ======================================================================== */
