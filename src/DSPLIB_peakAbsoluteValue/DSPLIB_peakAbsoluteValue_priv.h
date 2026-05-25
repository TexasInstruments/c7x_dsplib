// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_peakAbsoluteValue_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_peakAbsoluteValue_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_peakAbsoluteValue.h"

/**
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface, please
 *        see @ref DSPLIB_peakAbsoluteValue
 */
/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/
#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_peakAbsoluteValue_PrivArgs structure.
 *
 */
#define DSPLIB_peakAbsoluteValue_IXX_IXX_OXX_PBLOCK_SIZE                                                               \
   (2 * SE_PARAM_SIZE + 1 * DSPLIB_PARAM_SIZE + 2 * sizeof(int32_t))

/* The per-type tables below are indexed in the order 8-bit int, 16-bit int, 32-bit int, 64-bit int, float, double. The
 * loop terms come from the compiled .asm (C7504 and C7524 are identical); the overhead terms are a least-squares fit to
 * the Loki cycles of all 213 test cases of the test suite (max error 3 cycles over the rows-of-at-most-4-vectors
 * cases, 1 cycle over the longer rows). */

/**
 * @brief Rows of 5 or more vectors (inner loop): cycles to fill the software pipeline of the inner loop and cycles per
 *        iteration of 4 vectors (the constants in "2 + trip_cnt * 2" in the compiled .asm, all data types).
 */
#define DSPLIB_PEAKABSOLUTEVALUE_PIPE_UP_CYCLES (2)
#define DSPLIB_PEAKABSOLUTEVALUE_CYCLES_PER_ITERATION (2)

/**
 * @brief Rows of 5 or more vectors: cycles per row outside the inner loop (merge of the four maxima, horizontal
 *        maximum, store, outer loop), by data type.
 */
#define DSPLIB_PEAKABSOLUTEVALUE_ROW_OVERHEAD_CYCLES                                                                   \
   {                                                                                                                   \
      22, 20, 12, 23, 11, 16                                                                                           \
   }

/**
 * @brief Rows of at most 4 vectors (two rows per loop iteration): ii of the loop (cycles per two rows) and the
 *        cycles to fill its software pipeline (the constants in "N + trip_cnt * ii" in the compiled .asm), by data
 *        type, for 1 and for 2 vectors per stream.
 */
#define DSPLIB_PEAKABSOLUTEVALUE_PAIR_II_1_VECTOR                                                                      \
   {                                                                                                                   \
      5, 4, 2, 6, 2, 5                                                                                                 \
   }
#define DSPLIB_PEAKABSOLUTEVALUE_PAIR_II_2_VECTORS                                                                     \
   {                                                                                                                   \
      7, 6, 5, 7, 4, 7                                                                                                 \
   }
#define DSPLIB_PEAKABSOLUTEVALUE_PAIR_PIPE_UP_1_VECTOR                                                                 \
   {                                                                                                                   \
      14, 12, 10, 22, 9, 16                                                                                            \
   }
#define DSPLIB_PEAKABSOLUTEVALUE_PAIR_PIPE_UP_2_VECTORS                                                                \
   {                                                                                                                   \
      18, 16, 10, 34, 11, 22                                                                                           \
   }

/**
 * @brief Rows of at most 4 vectors: cycles of the first row of an odd number of rows, which runs on its own before the
 *        loop (straight-line code), by data type.
 */
#define DSPLIB_PEAKABSOLUTEVALUE_ODD_ROW_CYCLES                                                                        \
   {                                                                                                                   \
      22, 19, 12, 22, 10, 16                                                                                           \
   }

/**
 * @brief Cycles outside the loops of a call (two SEOPEN, template loads, output store, call and return), by data type.
 */
#define DSPLIB_PEAKABSOLUTEVALUE_CALL_OVERHEAD_CYCLES                                                                  \
   {                                                                                                                   \
      40, 40, 45, 42, 47, 48                                                                                           \
   }

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_peakAbsoluteValue_exec_ci
 *         and @ref DSPLIB_peakAbsoluteValue_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_peakAbsoluteValue_exec)(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn,
                                                           void *restrict pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_peakAbsoluteValue_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_peakAbsoluteValue_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_peakAbsoluteValue_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_peakAbsoluteValue_exec_ci does not lose cycles
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
extern DSPLIB_STATUS DSPLIB_peakAbsoluteValue_init_ci(DSPLIB_kernelHandle                      handle,
                                                      const DSPLIB_bufParams2D_t              *bufParamsIn,
                                                      const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                      const DSPLIB_peakAbsoluteValue_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_peakAbsoluteValue_exec.
 *
 * @details Long-row variant (rows of 5 or more vectors), selected in @ref DSPLIB_peakAbsoluteValue_init_ci. Both
 *          streaming engines are opened once for the whole matrix with a 2D template (one row per ICNT1 step, DIM1 =
 *          row stride). In every row engine 0 starts at the first element and engine 1 ends at the last element,
 *          each delivering the same number of vectors; the overlap between the two ranges is harmless for a
 *          maximum. Each inner-loop iteration takes the absolute value of 2 vectors from each engine and keeps four
 *          running vector maxima. After the inner loop the four maxima are combined and reduced across lanes, and
 *          the row's peak is stored to the output. Rows of at most 4 vectors use a separate variant in the _ci.cpp
 *          that finishes one row per loop iteration.
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
/**
 *  @brief Variant of @ref DSPLIB_peakAbsoluteValue_exec_ci for rows of at most 4 vectors (vecsPerStream is 1 or 2):
 *         one loop iteration handles two rows (oddHeight: the first row is handled on its own). Selected by @ref
 * DSPLIB_peakAbsoluteValue_init.
 */
template <typename dataType, typename unsignedType, int32_t vecsPerStream, int32_t oddHeight>
extern DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_rowPerIteration_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template <typename dataType, typename unsignedType>
extern DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_peakAbsoluteValue_exec.
 *
 * @details Reference implementation in natural C: the largest absolute value of the input, where integers equal
 *          to the type minimum saturate to the type maximum.
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
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_peakAbsoluteValue_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /** @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_peakAbsoluteValue_exec_cn and
    *         @ref DSPLIB_peakAbsoluteValue_exec_ci.                        */
   pFxnDSPLIB_peakAbsoluteValue_exec execute;
   /** @brief Number of elements in a row of the input                        */
   int32_t width;
   /** @brief Number of rows of the input, which is also the number of outputs */
   int32_t height;
   /** @brief Distance between the starts of two input rows, in elements       */
   int32_t strideInElements;
   /** @brief Inner-loop trip count per row of the long-row exec variant (4 vectors per iteration), derived in the
    *         C7x init; 1 for the short-row variants                                                          */
   int32_t numIterations;
   /** @brief Vectors each streaming engine delivers per row (1, 2, or an even number >= 4), derived in the C7x init.
    *         Selects the exec variant: 1 or 2 is the short-row variant, larger values the long-row variant.  */
   int32_t vecsPerStream;
   /** @brief Element offset of the start of the second input stream, derived in the C7x init */
   int32_t stream1Offset;
   uint8_t bufPblock[DSPLIB_peakAbsoluteValue_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_peakAbsoluteValue_PrivArgs;

#endif /* DSPLIB_peakAbsoluteValue_IXX_IXX_OXX_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  DSPLIB_peakAbsoluteValue.h                                              */
/* ======================================================================== */
