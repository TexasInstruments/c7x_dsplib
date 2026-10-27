// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_conditionalReplace_priv.h"
#include <type_traits>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

#define DSPLIB_CONDREPLACE_ROW_OVERHEAD (21)

void DSPLIB_conditionalReplace_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   uint64_t                            startupCycles   = 26; /* register save, SE/SA template load and open */
   uint64_t                            teardownCycles  = 11; /* SE/SA close, register restore and return */
   uint64_t                            operationCycles = 0;
   uint64_t                            overheadCycles  = 0;
   DSPLIB_conditionalReplace_PrivArgs *pKerPrivArgs    = (DSPLIB_conditionalReplace_PrivArgs *) handle;

   // per channel: nVecs loop iterations at II = 1 plus the row overhead
   operationCycles = (uint64_t) pKerPrivArgs->dim_y * (pKerPrivArgs->nVecs + DSPLIB_CONDREPLACE_ROW_OVERHEAD);
   overheadCycles  = startupCycles + teardownCycles;
   *estCycles      = operationCycles + overheadCycles;
   *archCycles     = operationCycles;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_conditionalReplace_init_ci(DSPLIB_kernelHandle                       handle,
                                                const DSPLIB_bufParams2D_t               *bufParamsIn,
                                                const DSPLIB_bufParams2D_t               *bufParamsOut,
                                                const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   DSPLIB_STATUS                       status       = DSPLIB_SUCCESS;
   DSPLIB_conditionalReplace_PrivArgs *pKerPrivArgs = (DSPLIB_conditionalReplace_PrivArgs *) handle;
   uint8_t                            *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                            eleCount     = c7x::element_count_of<vec>::value;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_conditionalReplace_init_ci \n");

   /**********************************************************************/
   /* Prepare SE/SA template                                             */
   /**********************************************************************/
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   se0Params.ELETYPE = c7x::se_eletype<vec>::value;
   se0Params.VECLEN  = c7x::se_veclen<vec>::value;
   sa0Params.VECLEN  = c7x::sa_veclen<vec>::value;

   // Always a 2D stream (one row per channel). The tail of every row is padded by the
   // SE and masked by the SA predicate, so each channel starts on a fresh vector.
   se0Params.ICNT0  = pKerPrivArgs->dim_x;
   se0Params.ICNT1  = pKerPrivArgs->dim_y;
   se0Params.DIM1   = pKerPrivArgs->strideIn;
   se0Params.DIMFMT = __SE_DIMFMT_2D;

   sa0Params.ICNT0  = pKerPrivArgs->dim_x;
   sa0Params.ICNT1  = pKerPrivArgs->dim_y;
   sa0Params.DIM1   = pKerPrivArgs->strideOut;
   sa0Params.DIMFMT = __SA_DIMFMT_2D;

   pKerPrivArgs->nVecs = DSPLIB_ceilingDiv(pKerPrivArgs->dim_x, eleCount);

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

template DSPLIB_STATUS
DSPLIB_conditionalReplace_init_ci<int8_t>(DSPLIB_kernelHandle                       handle,
                                          const DSPLIB_bufParams2D_t               *bufParamsIn,
                                          const DSPLIB_bufParams2D_t               *bufParamsOut,
                                          const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_init_ci<uint8_t>(DSPLIB_kernelHandle                       handle,
                                           const DSPLIB_bufParams2D_t               *bufParamsIn,
                                           const DSPLIB_bufParams2D_t               *bufParamsOut,
                                           const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_init_ci<int16_t>(DSPLIB_kernelHandle                       handle,
                                           const DSPLIB_bufParams2D_t               *bufParamsIn,
                                           const DSPLIB_bufParams2D_t               *bufParamsOut,
                                           const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_init_ci<uint16_t>(DSPLIB_kernelHandle                       handle,
                                            const DSPLIB_bufParams2D_t               *bufParamsIn,
                                            const DSPLIB_bufParams2D_t               *bufParamsOut,
                                            const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_init_ci<int32_t>(DSPLIB_kernelHandle                       handle,
                                           const DSPLIB_bufParams2D_t               *bufParamsIn,
                                           const DSPLIB_bufParams2D_t               *bufParamsOut,
                                           const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_init_ci<uint32_t>(DSPLIB_kernelHandle                       handle,
                                            const DSPLIB_bufParams2D_t               *bufParamsIn,
                                            const DSPLIB_bufParams2D_t               *bufParamsOut,
                                            const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_init_ci<int64_t>(DSPLIB_kernelHandle                       handle,
                                           const DSPLIB_bufParams2D_t               *bufParamsIn,
                                           const DSPLIB_bufParams2D_t               *bufParamsOut,
                                           const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_init_ci<uint64_t>(DSPLIB_kernelHandle                       handle,
                                            const DSPLIB_bufParams2D_t               *bufParamsIn,
                                            const DSPLIB_bufParams2D_t               *bufParamsOut,
                                            const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_conditionalReplace_init_ci<float>(DSPLIB_kernelHandle                       handle,
                                                                const DSPLIB_bufParams2D_t               *bufParamsIn,
                                                                const DSPLIB_bufParams2D_t               *bufParamsOut,
                                                                const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_init_ci<double>(DSPLIB_kernelHandle                       handle,
                                          const DSPLIB_bufParams2D_t               *bufParamsIn,
                                          const DSPLIB_bufParams2D_t               *bufParamsOut,
                                          const DSPLIB_conditionalReplace_InitArgs *pKerInitArgs);

// Define the compare templates (tag dispatch on compareType): the output is __select(cond, true value, false value)
template <int32_t compareType> using opTag = std::integral_constant<int32_t, compareType>;

// equal: replace where equal
template <typename vec>
inline vec conditionalReplace_op(opTag<DSPLIB_CONDITIONALREPLACE_EQ>, vec vecIn, vec constVec, vec replaceVec)
{
   return __select(__cmp_eq_pred(vecIn, constVec), replaceVec, vecIn);
}

// not equal: keep the input where equal
template <typename vec>
inline vec conditionalReplace_op(opTag<DSPLIB_CONDITIONALREPLACE_NE>, vec vecIn, vec constVec, vec replaceVec)
{
   return __select(__cmp_eq_pred(vecIn, constVec), vecIn, replaceVec);
}

// less than
template <typename vec>
inline vec conditionalReplace_op(opTag<DSPLIB_CONDITIONALREPLACE_LT>, vec vecIn, vec constVec, vec replaceVec)
{
   return __select(__cmp_lt_pred(vecIn, constVec), replaceVec, vecIn);
}

// less than or equal
template <typename vec>
inline vec conditionalReplace_op(opTag<DSPLIB_CONDITIONALREPLACE_LE>, vec vecIn, vec constVec, vec replaceVec)
{
   return __select(__cmp_le_pred(vecIn, constVec), replaceVec, vecIn);
}

// greater than
template <typename vec>
inline vec conditionalReplace_op(opTag<DSPLIB_CONDITIONALREPLACE_GT>, vec vecIn, vec constVec, vec replaceVec)
{
   return __select(__cmp_gt_pred(vecIn, constVec), replaceVec, vecIn);
}

// greater than or equal
template <typename vec>
inline vec conditionalReplace_op(opTag<DSPLIB_CONDITIONALREPLACE_GE>, vec vecIn, vec constVec, vec replaceVec)
{
   return __select(__cmp_ge_pred(vecIn, constVec), replaceVec, vecIn);
}

// One pipelined loop per compare type
template <typename dataType, int32_t compareType>
inline void conditionalReplace_loop(uint32_t                                       nVecs,
                                    typename c7x::make_full_vector<dataType>::type constVec,
                                    typename c7x::make_full_vector<dataType>::type replaceVec,
                                    dataType *restrict pOutLocal)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   for (uint32_t i = 0; i < nVecs; i++) {
      vec vecIn  = c7x::strm_eng<0, vec>::get_adv();
      vec vecOut = conditionalReplace_op(opTag<compareType>(), vecIn, constVec, replaceVec);

      __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, vecOut);
   }
}

// Selects the loop for the per channel compare type (validated by DSPLIB_conditionalReplace_set)
template <typename dataType>
inline void conditionalReplace_channel(int32_t                                        compareType,
                                       uint32_t                                       nVecs,
                                       typename c7x::make_full_vector<dataType>::type constVec,
                                       typename c7x::make_full_vector<dataType>::type replaceVec,
                                       dataType *restrict pOutLocal)
{
   if (compareType == DSPLIB_CONDITIONALREPLACE_EQ) {
      conditionalReplace_loop<dataType, DSPLIB_CONDITIONALREPLACE_EQ>(nVecs, constVec, replaceVec, pOutLocal);
   }
   else if (compareType == DSPLIB_CONDITIONALREPLACE_NE) {
      conditionalReplace_loop<dataType, DSPLIB_CONDITIONALREPLACE_NE>(nVecs, constVec, replaceVec, pOutLocal);
   }
   else if (compareType == DSPLIB_CONDITIONALREPLACE_LT) {
      conditionalReplace_loop<dataType, DSPLIB_CONDITIONALREPLACE_LT>(nVecs, constVec, replaceVec, pOutLocal);
   }
   else if (compareType == DSPLIB_CONDITIONALREPLACE_LE) {
      conditionalReplace_loop<dataType, DSPLIB_CONDITIONALREPLACE_LE>(nVecs, constVec, replaceVec, pOutLocal);
   }
   else if (compareType == DSPLIB_CONDITIONALREPLACE_GT) {
      conditionalReplace_loop<dataType, DSPLIB_CONDITIONALREPLACE_GT>(nVecs, constVec, replaceVec, pOutLocal);
   }
   else {
      conditionalReplace_loop<dataType, DSPLIB_CONDITIONALREPLACE_GE>(nVecs, constVec, replaceVec, pOutLocal);
   }
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_conditionalReplace_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   DSPLIB_STATUS                       status       = DSPLIB_SUCCESS;
   DSPLIB_conditionalReplace_PrivArgs *pKerPrivArgs = (DSPLIB_conditionalReplace_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_conditionalReplace_exec_ci nVecs %d\n", pKerPrivArgs->nVecs);

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   dataType *restrict pInLocal            = (dataType *) pIn;
   const int32_t *restrict pCompareLocal  = pKerPrivArgs->setArgs.pCompareType;
   const dataType *restrict pConstLocal   = (const dataType *) pKerPrivArgs->setArgs.pConstValue;
   const dataType *restrict pReplaceLocal = (const dataType *) pKerPrivArgs->setArgs.pReplaceValue;
   dataType *restrict pOutLocal           = (dataType *) pOut;

   const uint32_t nVecs = pKerPrivArgs->nVecs;
   const uint32_t dimY  = pKerPrivArgs->dim_y;

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   for (uint32_t ch = 0; ch < dimY; ch++) {
      const int32_t compareType = pCompareLocal[ch];
      const vec     replaceVec  = (vec) pReplaceLocal[ch];
      const vec     constVec    = (vec) pConstLocal[ch];

      conditionalReplace_channel<dataType>(compareType, nVecs, constVec, replaceVec, pOutLocal);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   return status;
}

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_conditionalReplace_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
