// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_bitwiseANDConst_priv.h"
#include <type_traits>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

void DSPLIB_bitwiseANDConst_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   uint64_t                         bitwiseANDConstStartupCycles   = 28;
   uint64_t                         bitwiseANDConstTeardownCycles  = 4;
   uint64_t                         bitwiseANDConstOperationCycles = 0;
   uint64_t                         bitwiseANDConstOverheadCycles  = 0;
   uint32_t                         nVecs;
   DSPLIB_bitwiseANDConst_PrivArgs *pKerPrivArgs = (DSPLIB_bitwiseANDConst_PrivArgs *) handle;

   nVecs                          = pKerPrivArgs->nVecs;
   bitwiseANDConstOperationCycles = (1 + nVecs * 1);
   bitwiseANDConstOverheadCycles  = bitwiseANDConstStartupCycles + bitwiseANDConstTeardownCycles;
   *estCycles                     = bitwiseANDConstOperationCycles + bitwiseANDConstOverheadCycles;
   *archCycles                    = bitwiseANDConstOperationCycles;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_bitwiseANDConst_init_ci(DSPLIB_kernelHandle                    handle,
                                             const DSPLIB_bufParams2D_t            *bufParamsIn0,
                                             const DSPLIB_bufParams1D_t            *bufParamsIn1,
                                             const DSPLIB_bufParams2D_t            *bufParamsOut,
                                             const DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                                          status       = DSPLIB_SUCCESS;
   DSPLIB_bitwiseANDConst_PrivArgs                       *pKerPrivArgs = (DSPLIB_bitwiseANDConst_PrivArgs *) handle;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint8_t                                               *pBlock   = pKerPrivArgs->bufPblock;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;

   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_bitwiseANDConst_init_ci \n");
   DSPLIB_DEBUGPRINTFN(0, "dim_x %d dim_y %d eleCount %d\n", pKerPrivArgs->dim_x, pKerPrivArgs->dim_y, eleCount);

   /**********************************************************************/
   /* Prepare SE/SA template                                             */
   /**********************************************************************/
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;

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

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_init_ci<uint8_t>(DSPLIB_kernelHandle                    handle,
                                                               const DSPLIB_bufParams2D_t            *bufParamsIn0,
                                                               const DSPLIB_bufParams1D_t            *bufParamsIn1,
                                                               const DSPLIB_bufParams2D_t            *bufParamsOut,
                                                               const DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_init_ci<uint16_t>(DSPLIB_kernelHandle                    handle,
                                                                const DSPLIB_bufParams2D_t            *bufParamsIn0,
                                                                const DSPLIB_bufParams1D_t            *bufParamsIn1,
                                                                const DSPLIB_bufParams2D_t            *bufParamsOut,
                                                                const DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_init_ci<uint32_t>(DSPLIB_kernelHandle                    handle,
                                                                const DSPLIB_bufParams2D_t            *bufParamsIn0,
                                                                const DSPLIB_bufParams1D_t            *bufParamsIn1,
                                                                const DSPLIB_bufParams2D_t            *bufParamsOut,
                                                                const DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_init_ci<uint64_t>(DSPLIB_kernelHandle                    handle,
                                                                const DSPLIB_bufParams2D_t            *bufParamsIn0,
                                                                const DSPLIB_bufParams1D_t            *bufParamsIn1,
                                                                const DSPLIB_bufParams2D_t            *bufParamsOut,
                                                                const DSPLIB_bitwiseANDConst_InitArgs *pKerInitArgs);

template <typename dataType>
DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_ci(DSPLIB_kernelHandle handle,
                                             void *restrict pIn0,
                                             void *restrict pIn1,
                                             void *restrict pOut)
{
   DSPLIB_STATUS                    status       = DSPLIB_SUCCESS;
   DSPLIB_bitwiseANDConst_PrivArgs *pKerPrivArgs = (DSPLIB_bitwiseANDConst_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_bitwiseANDConst_and_exec_ci nVecs %d\n", pKerPrivArgs->nVecs);

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   dataType *restrict pIn0Local = (dataType *) pIn0;
   dataType *restrict pIn1Local = (dataType *) pIn1;
   dataType *restrict pOutLocal = (dataType *) pOut;

   __SE0_OPEN(pIn0Local, se0Params);
   __SA0_OPEN(sa0Params);

   vec valueVec = (vec) (*pIn1Local);

   // (1 + nVecs * 1)
   for (uint32_t i = 0; i < pKerPrivArgs->nVecs; i++) {

      vec vec1 = c7x::strm_eng<0, vec>::get_adv();

      vec vecOut = vec1 & valueVec;

      __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, vecOut);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   return status;
}

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_ci<uint8_t>(DSPLIB_kernelHandle handle,
                                                               void *restrict pIn0,
                                                               void *restrict pIn1,
                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_ci<uint16_t>(DSPLIB_kernelHandle handle,
                                                                void *restrict pIn0,
                                                                void *restrict pIn1,
                                                                void *restrict pOut);

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_ci<uint32_t>(DSPLIB_kernelHandle handle,
                                                                void *restrict pIn0,
                                                                void *restrict pIn1,
                                                                void *restrict pOut);

template DSPLIB_STATUS DSPLIB_bitwiseANDConst_exec_ci<uint64_t>(DSPLIB_kernelHandle handle,
                                                                void *restrict pIn0,
                                                                void *restrict pIn1,
                                                                void *restrict pOut);
