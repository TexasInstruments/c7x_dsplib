// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_utility.h"
#include "DSPLIB_tableSourceRowColSelector.h"

/**
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface, please
 *        see @ref DSPLIB_tableSourceRowColSelector
 */
/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/
#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SE1_PARAM_OFFSET (SE_SA0_PARAM_OFFSET + SA_PARAM_SIZE)
#define SE_SA1_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_tableSourceRowColSelector_PrivArgs structure.
 *
 */
#define DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_PBLOCK_SIZE (2 * SE_PARAM_SIZE + 2 * SA_PARAM_SIZE)

/* Constants of @ref DSPLIB_tableSourceRowColSelector_perfEst, fitted to the Loki cycles */
/** @brief Startup cycles, non-interleaved output */
#define DSPLIB_TABLESOURCEROWCOLSELECTOR_NONINTERLEAVED_STARTUP_CYCLES (46U)
/** @brief Startup cycles, interleaved output (adds the transpose set-up) */
#define DSPLIB_TABLESOURCEROWCOLSELECTOR_INTERLEAVED_STARTUP_CYCLES (73U)
/** @brief Cycles per group of four indices */
#define DSPLIB_TABLESOURCEROWCOLSELECTOR_GROUP_CYCLES (26U)
/** @brief Cycles for the leftover pair or single index */
#define DSPLIB_TABLESOURCEROWCOLSELECTOR_TAIL_CYCLES (23U)
/** @brief Extra cycles when both a pair and a single index are left */
#define DSPLIB_TABLESOURCEROWCOLSELECTOR_TAIL_PAIR_AND_SINGLE_CYCLES (5U)

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_tableSourceRowColSelector_exec_ci
 *         and @ref DSPLIB_tableSourceRowColSelector_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_tableSourceRowColSelector_exec)(DSPLIB_kernelHandle handle,
                                                                   void *restrict pIn0,
                                                                   void *restrict pIn1,
                                                                   void *restrict pOut,
                                                                   void *restrict pScratch);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_tableSourceRowColSelector_init.
 *
 * @details This function determines the configuration for the streaming engine and
 *          streaming address generator resources of the execution variant that
 *          @ref DSPLIB_tableSourceRowColSelector_init selects from isOutputInterleave and numIndices.
 *          The configuration is saved in bufPBlock array. In the kernel call sequence,
 *          the selected execution function would be called later independently by the
 *          application. When it runs, it merely retrieves the configuration from the
 *          bufPBlock and uses it to set up the hardware resources. This arrangement is
 *          so that the execute function does not lose cycles to determine the hardware
 *          configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn0 :  Pointer to the structure containing dimensional
 *                               information of index buffer
 *  @param [in]  bufParamsIn1 :  Pointer to the structure containing dimensional
 *                               information of table buffer
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS
DSPLIB_tableSourceRowColSelector_init_ci(DSPLIB_kernelHandle                              handle,
                                         const DSPLIB_bufParams1D_t                      *bufParamsIn0,
                                         const DSPLIB_bufParams2D_t                      *bufParamsIn1,
                                         const DSPLIB_bufParams2D_t                      *bufParamsOut,
                                         const DSPLIB_tableSourceRowColSelector_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_tableSourceRowColSelector_exec.
 *
 * @details Non-interleaved: the selected vectors are written straight to the output. Interleaved: they are written
 *          to the scratch buffer, then read transposed into the output.
 *          Each group of four indices uses SE0 for two and SE1 for two. The variant is picked in
 *          @ref DSPLIB_tableSourceRowColSelector_init, so this function has no data dependent branch.
 *
 *  @tparam      isOutputInterleave : 1 for the interleaved output, 0 for the non-interleaved output
 *  @tparam      pairTail           : (numIndices / 2) % 2
 *  @tparam      singleTail         : numIndices % 2
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn0        : Pointer to buffer holding the indices
 *  @param [in]  pIn1        : Pointer to buffer holding the table
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *  @param [in]  pScratch    : Pointer to the scratch buffer, used only by the interleaved output
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 */
template <typename dataType, int32_t isOutputInterleave, int32_t pairTail, int32_t singleTail>
extern DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_ci(DSPLIB_kernelHandle handle,
                                                              void *restrict pIn0,
                                                              void *restrict pIn1,
                                                              void *restrict pOut,
                                                              void *restrict pScratch);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_tableSourceRowColSelector_exec.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn0        : Pointer to buffer holding the indices
 *  @param [in]  pIn1        : Pointer to buffer holding the table
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *  @param [in]  pScratch    : Pointer to the scratch buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_tableSourceRowColSelector_exec_cn(DSPLIB_kernelHandle handle,
                                                              void *restrict pIn0,
                                                              void *restrict pIn1,
                                                              void *restrict pOut,
                                                              void *restrict pScratch);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /** @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_tableSourceRowColSelector_exec_cn and
    *         @ref DSPLIB_tableSourceRowColSelector_exec_ci.                          */
   pFxnDSPLIB_tableSourceRowColSelector_exec execute;
   /** @brief Number of rows in the table */
   int32_t numRows;
   /** @brief Number of columns in the table */
   int32_t numCols;
   /** @brief Number of indices, each selecting one table vector */
   int32_t numIndices;
   /** @brief Stride between consecutive output rows (in elements, not bytes) */
   uint32_t strideOut;
   /** @brief Flag indicating if an index selects a table column instead of a table row */
   uint8_t isColSelect;
   /** @brief Flag indicating if the output is in interleaved or non-interleaved format */
   uint8_t isOutputInterleave;
   /** @brief Length of one selected table vector: numCols for row select, numRows for column select */
   int32_t selLen;
   /** @brief Largest valid index: numRows - 1 for row select, numCols - 1 for column select */
   int32_t selMax;
   /** @brief Elements between two selectable table vectors: numCols for row select, 1 for column select */
   int32_t selStride;
   /** @brief Number of vectors read for one selected table vector */
   int32_t nVecs;
   /** @brief Number of vectors needed to hold one element per index */
   int32_t numBlocks;
   /** @brief Buffer to save SE & SA configuration parameters */
   uint8_t bufPblock[DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_tableSourceRowColSelector_PrivArgs;

#endif /* DSPLIB_TABLESOURCEROWCOLSELECTOR_IXX_IXX_OXX_PRIV_H_ */
