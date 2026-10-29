// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_logicalComparator_priv.h"
#include <type_traits>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

void DSPLIB_logicalComparator_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   uint64_t                           logicalComparatorStartupCycles   = 30;
   uint64_t                           logicalComparatorTeardownCycles  = 4;
   uint64_t                           logicalComparatorOperationCycles = 0;
   uint64_t                           logicalComparatorOverheadCycles  = 0;
   uint32_t                           nVecs;
   DSPLIB_logicalComparator_PrivArgs *pKerPrivArgs = (DSPLIB_logicalComparator_PrivArgs *) handle;

   nVecs                            = pKerPrivArgs->nVecs;
   logicalComparatorOperationCycles = (2 + nVecs * 1);
   logicalComparatorOverheadCycles  = logicalComparatorStartupCycles + logicalComparatorTeardownCycles;
   *estCycles                       = logicalComparatorOperationCycles + logicalComparatorOverheadCycles;
   *archCycles                      = logicalComparatorOperationCycles;
}

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_logicalComparator_init_ci(DSPLIB_kernelHandle                      handle,
                                               const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                               const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                               const DSPLIB_bufParams2D_t              *bufParamsOut,
                                               const DSPLIB_logicalComparator_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                                            status       = DSPLIB_SUCCESS;
   DSPLIB_logicalComparator_PrivArgs                       *pKerPrivArgs = (DSPLIB_logicalComparator_PrivArgs *) handle;
   typedef typename c7x::make_full_vector<dataTypeIn>::type vecI;
   typedef typename c7x::make_full_vector<dataTypeOut>::type vecO;
   uint8_t                                                  *pBlock   = pKerPrivArgs->bufPblock;
   uint32_t                                                  eleCount = c7x::element_count_of<vecI>::value;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_logicalComparator_init_ci \n");
   DSPLIB_DEBUGPRINTFN(0, "dim_x %d dim_y %d eleCount %d\n", pKerPrivArgs->dim_x, pKerPrivArgs->dim_y, eleCount);

   /**********************************************************************/
   /* Prepare SE/SA template                                             */
   /**********************************************************************/
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vecI>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vecI>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vecO>::value;

   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   sa0Params.VECLEN  = SA_VECLEN;

   // For input buffer without stride
   if ((int32_t) pKerPrivArgs->dim_x == pKerPrivArgs->strideIn) {
      uint32_t blockSize = pKerPrivArgs->dim_y * pKerPrivArgs->dim_x;

      se0Params.ICNT0  = pKerPrivArgs->dim_y * pKerPrivArgs->dim_x;
      se0Params.DIMFMT = __SE_DIMFMT_1D;

      sa0Params.ICNT0  = blockSize;
      sa0Params.DIMFMT = __SA_DIMFMT_1D;

      pKerPrivArgs->nVecs = DSPLIB_ceilingDiv(blockSize, eleCount);
   }
   else {
      se0Params.ICNT0  = pKerPrivArgs->dim_x;
      se0Params.ICNT1  = pKerPrivArgs->dim_y;
      se0Params.DIM1   = pKerPrivArgs->strideIn;
      se0Params.DIMFMT = __SE_DIMFMT_2D;

      sa0Params.ICNT0  = pKerPrivArgs->dim_x;
      sa0Params.ICNT1  = pKerPrivArgs->dim_y;
      sa0Params.DIM1   = pKerPrivArgs->strideOut;
      sa0Params.DIMFMT = __SA_DIMFMT_2D;

      pKerPrivArgs->nVecs = pKerPrivArgs->dim_y * DSPLIB_ceilingDiv(pKerPrivArgs->dim_x, eleCount);
   }
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<float, uint32_t>(DSPLIB_kernelHandle                      handle,
                                                  const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                  const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                  const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                  const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<double, uint64_t>(DSPLIB_kernelHandle                      handle,
                                                   const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                   const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                   const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                   const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<int8_t, uint8_t>(DSPLIB_kernelHandle                      handle,
                                                  const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                  const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                  const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                  const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<uint8_t, uint8_t>(DSPLIB_kernelHandle                      handle,
                                                   const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                   const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                   const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                   const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<int16_t, uint16_t>(DSPLIB_kernelHandle                      handle,
                                                    const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                    const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                    const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                    const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<uint16_t, uint16_t>(DSPLIB_kernelHandle                      handle,
                                                     const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                     const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                     const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                     const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<int32_t, uint32_t>(DSPLIB_kernelHandle                      handle,
                                                    const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                    const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                    const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                    const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<uint32_t, uint32_t>(DSPLIB_kernelHandle                      handle,
                                                     const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                     const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                     const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                     const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<int64_t, uint64_t>(DSPLIB_kernelHandle                      handle,
                                                    const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                    const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                    const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                    const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_logicalComparator_init_ci<uint64_t, uint64_t>(DSPLIB_kernelHandle                      handle,
                                                     const DSPLIB_bufParams2D_t              *bufParamsIn0,
                                                     const DSPLIB_bufParams2D_t              *bufParamsIn1,
                                                     const DSPLIB_bufParams2D_t              *bufParamsOut,
                                                     const DSPLIB_logicalComparator_InitArgs *pKerInitArgs);

// Define the compare templates (tag dispatch on opType)
template <uint32_t opType> using opTag = std::integral_constant<uint32_t, opType>;

template <typename vecI, typename vecO>
inline vecO logicalComparator_op(opTag<DSPLIB_LOGICALCOMPARATOR_EQ>, vecI vec1, vecI vec2)
{
   return __select(__cmp_eq_pred(vec1, vec2), vecO(1), vecO(0));
}

template <typename vecI, typename vecO>
inline vecO logicalComparator_op(opTag<DSPLIB_LOGICALCOMPARATOR_NE>, vecI vec1, vecI vec2)
{
   return __select(__cmp_eq_pred(vec1, vec2), vecO(0), vecO(1));
}

template <typename vecI, typename vecO>
inline vecO logicalComparator_op(opTag<DSPLIB_LOGICALCOMPARATOR_LT>, vecI vec1, vecI vec2)
{
   return __select(__cmp_lt_pred(vec1, vec2), vecO(1), vecO(0));
}

template <typename vecI, typename vecO>
inline vecO logicalComparator_op(opTag<DSPLIB_LOGICALCOMPARATOR_LE>, vecI vec1, vecI vec2)
{
   return __select(__cmp_le_pred(vec1, vec2), vecO(1), vecO(0));
}

template <typename vecI, typename vecO>
inline vecO logicalComparator_op(opTag<DSPLIB_LOGICALCOMPARATOR_GT>, vecI vec1, vecI vec2)
{
   return __select(__cmp_gt_pred(vec1, vec2), vecO(1), vecO(0));
}

template <typename vecI, typename vecO>
inline vecO logicalComparator_op(opTag<DSPLIB_LOGICALCOMPARATOR_GE>, vecI vec1, vecI vec2)
{
   return __select(__cmp_ge_pred(vec1, vec2), vecO(1), vecO(0));
}

template <typename dataTypeIn, typename dataTypeOut, uint32_t opType>
DSPLIB_STATUS DSPLIB_logicalComparator_exec_ci(DSPLIB_kernelHandle handle,
                                               void *restrict pIn0,
                                               void *restrict pIn1,
                                               void *restrict pOut)
{
   DSPLIB_STATUS                      status       = DSPLIB_SUCCESS;
   DSPLIB_logicalComparator_PrivArgs *pKerPrivArgs = (DSPLIB_logicalComparator_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataTypeIn>::type  vecI;
   typedef typename c7x::make_full_vector<dataTypeOut>::type vecO;
   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_logicalComparator_exec_ci nVecs %d\n", pKerPrivArgs->nVecs);

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   dataTypeIn *restrict pIn0Local  = (dataTypeIn *) pIn0;
   dataTypeIn *restrict pIn1Local  = (dataTypeIn *) pIn1;
   dataTypeOut *restrict pOutLocal = (dataTypeOut *) pOut;

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn1Local, se0Params);
   __SA0_OPEN(sa0Params);

   // (2 + nVecs * 1)
   for (uint32_t i = 0; i < pKerPrivArgs->nVecs; i++) {

      vecI vec1 = c7x::strm_eng<0, vecI>::get_adv();
      vecI vec2 = c7x::strm_eng<1, vecI>::get_adv();

      vecO vecOut = logicalComparator_op<vecI, vecO>(opTag<opType>(), vec1, vec2);

      __vpred predTemp  = c7x::strm_agen<0, vecO>::get_vpred();
      vecO   *pStoreVec = c7x::strm_agen<0, vecO>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, vecOut);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return status;
}

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<float, uint32_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<float, uint32_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<float, uint32_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<float, uint32_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<float, uint32_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<float, uint32_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<double, uint64_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<double, uint64_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<double, uint64_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<double, uint64_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<double, uint64_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<double, uint64_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint8_t, uint8_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                                void *restrict pIn0,
                                                                                void *restrict pIn1,
                                                                                void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint16_t, uint16_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint32_t, uint32_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<int64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_EQ>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_NE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_LT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_LE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_GT>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_logicalComparator_exec_ci<uint64_t, uint64_t, DSPLIB_LOGICALCOMPARATOR_GE>(DSPLIB_kernelHandle handle,
                                                                                  void *restrict pIn0,
                                                                                  void *restrict pIn1,
                                                                                  void *restrict pOut);
