// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_NONZEROCHECK_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_NONZEROCHECK_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_nonZeroCheck.h"

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_nonZeroCheck_PrivArgs structure.
 *
 */

#define DSPLIB_nonZeroCheck_IXX_IXX_OXX_PBLOCK_SIZE (4 * SE_PARAM_SIZE)

/*!
 * @brief Cycles per inner-loop iteration of the one-per-stream exec variants (ii from the compiled .asm, every data
 *        type, C7504 and C7524). Each iteration reads one vector from SE0 and one from SE1.
 */
#define DSPLIB_NONZEROCHECK_CYCLES_PER_ITERATION (1)

/*!
 * @brief Cycles per inner-loop iteration of the two-per-stream exec variants (ii from the compiled .asm, every data
 *        type, C7504 and C7524). Each iteration reads two vectors from SE0 and two from SE1.
 */
#define DSPLIB_NONZEROCHECK_TWO_PER_STREAM_CYCLES_PER_ITERATION (2)

/*!
 * @brief Cycles per outer iteration of @ref DSPLIB_nonZeroCheck_all_row_exec_ci and
 *        @ref DSPLIB_nonZeroCheck_any_row_exec_ci outside the inner loop: pipeline fill and drain and the two
 *        predicated vector stores. Least-squares fit to Loki C7504 cycles of test cases 1-80, each run with both
 *        variants forced (158 points). Over all 108 test cases estCycles is within 6.2 % of Loki on average, 17 % at
 *        worst (smallest cases).
 */
#define DSPLIB_NONZEROCHECK_ROW_BLOCK_OVERHEAD_CYCLES (9)

/*!
 * @brief Cycles per outer iteration of @ref DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci and
 *        @ref DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci outside the inner loop: pipeline fill and drain and the
 *        four predicated vector stores. Least-squares fit to Loki C7504 cycles of test cases 1-80, each run with both
 *        variants forced (158 points). Over all 108 test cases estCycles is within 6.2 % of Loki on average, 17 % at
 *        worst (smallest cases).
 */
#define DSPLIB_NONZEROCHECK_ROW_TWO_BLOCKS_OVERHEAD_CYCLES (10)

/*!
 * @brief Cycles per outer iteration of @ref DSPLIB_nonZeroCheck_all_column_exec_ci and
 *        @ref DSPLIB_nonZeroCheck_any_column_exec_ci outside the inner loop: pipeline fill and drain, the two
 *        vector-to-scalar reductions and the two scalar stores. Least-squares fit to Loki C7504 cycles of test cases
 *        1-80, each run with both variants forced (158 points). Over all 108 test cases estCycles is within 6.2 % of
 *        Loki on average, 17 % at worst (smallest cases).
 */
#define DSPLIB_NONZEROCHECK_COLUMN_ROW_OVERHEAD_CYCLES (14)

/*!
 * @brief Cycles per outer iteration of @ref DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci and
 *        @ref DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci outside the inner loop: pipeline fill and drain, the
 *        four vector-to-scalar reductions and the four scalar stores. Least-squares fit to Loki C7504 cycles of test
 *        cases 1-80, each run with both variants forced (158 points). Over all 108 test cases estCycles is within 6.2
 *        % of Loki on average, 17 % at worst (smallest cases).
 */
#define DSPLIB_NONZEROCHECK_COLUMN_TWO_ROWS_OVERHEAD_CYCLES (18)

/*!
 * @brief Cycles outside the loops: two SEOPEN, two SAOPEN, template loads, first-data latency and the closes.
 *        Least-squares fit to Loki C7504 cycles of test cases 1-80, each run with both variants forced (158 points).
 *        Over all 108 test cases estCycles is within 6.2 % of Loki on average, 17 % at worst (smallest cases).
 */
#define DSPLIB_NONZEROCHECK_CALL_OVERHEAD_CYCLES (41)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of the C7x execution functions (for example
 *         @ref DSPLIB_nonZeroCheck_all_row_exec_ci) and the natural C
 *         execution functions (for example @ref DSPLIB_nonZeroCheck_all_row_exec_cn).
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_nonZeroCheck_exec)(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn0,
                                                      void *restrict pOut);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_nonZeroCheck_init.
 *
 * @details This function determines the configuration for the streaming engine and stream address generator
 *          resources based on the function call parameters, and the configuration is saved in bufPBlock array. In
 *          the kernel call sequence, the C7x execution function would be called later independently by the
 *          application. When the C7x execution function runs, it merely retrieves the configuration from the
 *          bufPBlock and uses it to set up the hardware resources. This arrangement is so that the C7x execution
 *          function does not lose cycles to determine the hardware configuration.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                                information of input0 buffer
 *  @param [in]  bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of output buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_nonZeroCheck_init_ci(DSPLIB_kernelHandle                 handle,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                          const DSPLIB_bufParams1D_t         *bufParamsOut,
                                          const DSPLIB_nonZeroCheck_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which computes the ALL operation on rows,
 *         one column block per stream per outer iteration.
 *
 * @details Output element j is 1 if every element of column j (over the dim_y rows) is non-zero, else 0. SE0 and
 *          SE1 each read one block of eleCount columns per outer iteration, over all dim_y rows, and fold the
 *          per-element "is zero" compares into a vector with a select; SA0 and SA1 store the result vectors with
 *          the SA predicate dropping the lanes past dim_x. SE0 covers the first half of the blocks and SE1 the
 *          second half, overlapping by one when the count is odd. Selected in init by the cycle model of @ref
 *          DSPLIB_nonZeroCheck_perfEst.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
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
DSPLIB_STATUS DSPLIB_nonZeroCheck_all_row_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which computes the ALL operation on rows,
 *         two column blocks per stream per outer iteration.
 *
 * @details Output element j is 1 if every element of column j (over the dim_y rows) is non-zero, else 0. SE0 and
 *          SE1 each read two adjacent blocks of eleCount columns per outer iteration, over all dim_y rows, and fold
 *          the per-element "is zero" compares into a vector with a select; SA0 and SA1 store the result vectors
 *          with the SA predicate dropping the lanes past dim_x. SE0 covers the first half of the blocks and SE1 the
 *          second half, overlapping by one when the count is odd. Selected in init by the cycle model of @ref
 *          DSPLIB_nonZeroCheck_perfEst.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
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
DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_twoBlocks_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which computes the ALL operation on columns,
 *         one row per stream per outer iteration.
 *
 * @details Output element j is 1 if every element of row j (over the dim_x columns) is non-zero, else 0. SE0 and
 *          SE1 each read one row per outer iteration and fold the per-element "is zero" compares into a vector with
 *          a select; the vector is reduced to a scalar with a predicate bit count and SA0 and SA1 store the
 *          scalars. SE0 covers the first half of the rows and SE1 the second half, overlapping when the row count
 *          does not divide evenly. Selected in init by the cycle model of @ref DSPLIB_nonZeroCheck_perfEst.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
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
DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which computes the ALL operation on columns,
 *         two rows per stream per outer iteration.
 *
 * @details Output element j is 1 if every element of row j (over the dim_x columns) is non-zero, else 0. SE0 and
 *          SE1 each read two adjacent rows per outer iteration and fold the per-element "is zero" compares into a
 *          vector with a select; the vector is reduced to a scalar with a predicate bit count and SA0 and SA1 store
 *          the scalars. SE0 covers the first half of the rows and SE1 the second half, overlapping when the row
 *          count does not divide evenly. Selected in init by the cycle model of @ref DSPLIB_nonZeroCheck_perfEst.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
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
DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_twoRows_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which computes the ANY operation on rows,
 *         one column block per stream per outer iteration.
 *
 * @details Output element j is 1 if any element of column j (over the dim_y rows) is non-zero, else 0. SE0 and SE1
 *          each read one block of eleCount columns per outer iteration, over all dim_y rows, and fold the
 *          per-element "is zero" compares into a vector with a select; SA0 and SA1 store the result vectors with
 *          the SA predicate dropping the lanes past dim_x. SE0 covers the first half of the blocks and SE1 the
 *          second half, overlapping by one when the count is odd. Selected in init by the cycle model of @ref
 *          DSPLIB_nonZeroCheck_perfEst.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
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
DSPLIB_STATUS DSPLIB_nonZeroCheck_any_row_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which computes the ANY operation on rows,
 *         two column blocks per stream per outer iteration.
 *
 * @details Output element j is 1 if any element of column j (over the dim_y rows) is non-zero, else 0. SE0 and SE1
 *          each read two adjacent blocks of eleCount columns per outer iteration, over all dim_y rows, and fold the
 *          per-element "is zero" compares into a vector with a select; SA0 and SA1 store the result vectors with
 *          the SA predicate dropping the lanes past dim_x. SE0 covers the first half of the blocks and SE1 the
 *          second half, overlapping by one when the count is odd. Selected in init by the cycle model of @ref
 *          DSPLIB_nonZeroCheck_perfEst.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
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
DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_twoBlocks_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which computes the ANY operation on columns,
 *         one row per stream per outer iteration.
 *
 * @details Output element j is 1 if any element of row j (over the dim_x columns) is non-zero, else 0. SE0 and SE1
 *          each read one row per outer iteration and fold the per-element "is zero" compares into a vector with a
 *          select; the vector is reduced to a scalar with a predicate bit count and SA0 and SA1 store the scalars.
 *          SE0 covers the first half of the rows and SE1 the second half, overlapping when the row count does not
 *          divide evenly. Selected in init by the cycle model of @ref DSPLIB_nonZeroCheck_perfEst.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
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
DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel which computes the ANY operation on columns,
 *         two rows per stream per outer iteration.
 *
 * @details Output element j is 1 if any element of row j (over the dim_x columns) is non-zero, else 0. SE0 and SE1
 *          each read two adjacent rows per outer iteration and fold the per-element "is zero" compares into a
 *          vector with a select; the vector is reduced to a scalar with a predicate bit count and SA0 and SA1 store
 *          the scalars. SE0 covers the first half of the rows and SE1 the second half, overlapping when the row
 *          count does not divide evenly. Selected in init by the cycle model of @ref DSPLIB_nonZeroCheck_perfEst.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
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
DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_twoRows_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel which computes ALL operation on rows.
 *
 * @details Output element j is 1 if every element of column j (over dim_y rows) is non-zero, else 0.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_row_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel which computes ALL operation on columns.
 *
 * @details Output element j is 1 if every element of row j (over dim_x elements) is non-zero, else 0.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_nonZeroCheck_all_column_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel which computes ANY operation on rows.
 *
 * @details Output element j is 1 if any element of column j (over dim_y rows) is non-zero, else 0.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_row_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel which computes ANY operation on columns.
 *
 * @details Output element j is 1 if any element of row j (over dim_x elements) is non-zero, else 0.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn0         : Pointer to buffer holding the input0 buffer
 *  @param [out] pOut         : Pointer to buffer holding the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_nonZeroCheck_any_column_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         the natural C (_cn) and C7x (_ci) execution functions,
    *         for example DSPLIB_nonZeroCheck_all_row_exec_cn and
    *         DSPLIB_nonZeroCheck_all_row_exec_ci.             */
   pFxnDSPLIB_nonZeroCheck_exec execute;

   /*! @brief Number of vectors covering one row of dim_x elements */
   uint32_t numVecs;

   /*! @brief Outer-loop iteration count of the C7x variant; each iteration
    *         produces one (or, with twoPerStream, two) column blocks (ROW axis)
    *         or output elements (COLUMN axis) per stream */
   uint32_t numIterations;

   /*! @brief 1 when the C7x variant processes two column blocks (ROW axis) or
    *         two rows (COLUMN axis) per stream per outer iteration; set by
    *         @ref DSPLIB_nonZeroCheck_perfEst, which init calls to select the
    *         variant; 0 for one */
   uint32_t twoPerStream;

   /*! @brief Offset in elements from pIn0 to the first element read by SE1 */
   uint32_t secondHalfInOffset;

   /*! @brief Offset in elements from pOut to the first element stored by SA1 */
   uint32_t secondHalfOutOffset;

   uint32_t dim_x;    /**< \brief Width of buffer in X dimension in elements. */
   uint32_t dim_y;    /**< \brief Height of buffer in Y dimension in elements. */
   int32_t  strideIn; /**< \brief Stride in Y dimension in elements of the input. */
   uint32_t opType;   /**< \brief Selects all or any to compute. */
   uint32_t axisType; /**< \brief Selects row or column to consider. */
   int32_t  dataType; /**< \brief Data type of input/output, refer to @ref DSPLIB_data_type_e. */

   /*! @brief bufPblock array to store SE/SA template */
   uint8_t bufPblock[DSPLIB_nonZeroCheck_IXX_IXX_OXX_PBLOCK_SIZE];

} DSPLIB_nonZeroCheck_PrivArgs;

#endif /* DSPLIB_NONZEROCHECK_IXX_IXX_OXX_PRIV_H_ */
