// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blkCopyConst2D_priv.h"

void DSPLIB_blkCopyConst2D_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   uint64_t                        blkCopyConst2DStartupCycles   = 13;
   uint64_t                        blkCopyConst2DTeardownCycles  = 1;
   uint64_t                        blkCopyConst2DOperationCycles = 0;
   uint64_t                        blkCopyConst2DOverheadCycles  = 0;
   uint32_t                        numVecs;
   DSPLIB_blkCopyConst2D_PrivArgs *pKerPrivArgs = (DSPLIB_blkCopyConst2D_PrivArgs *) handle;

   numVecs                       = pKerPrivArgs->numVecs;
   blkCopyConst2DOperationCycles = (1 + numVecs * 1);
   blkCopyConst2DOverheadCycles  = blkCopyConst2DStartupCycles + blkCopyConst2DTeardownCycles;
   *estCycles                    = blkCopyConst2DOperationCycles + blkCopyConst2DOverheadCycles;
   *archCycles                   = blkCopyConst2DOperationCycles;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci(DSPLIB_kernelHandle                  handle,
                                            const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                            const DSPLIB_bufParams2D_t          *bufParamsOut,
                                            const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_blkCopyConst2D_init_ci function");

   DSPLIB_STATUS                   status       = DSPLIB_SUCCESS;
   DSPLIB_blkCopyConst2D_PrivArgs *pKerPrivArgs = (DSPLIB_blkCopyConst2D_PrivArgs *) handle;
   uint8_t                        *pBlock       = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   __SA_VECLEN      SA_VECLEN = c7x::sa_veclen<vec>::value;

   /* Prepare SA template for writing output */
   sa0Params.ICNT0  = bufParamsOut->dim_x;
   sa0Params.ICNT1  = bufParamsOut->dim_y;
   sa0Params.DIM1   = pKerPrivArgs->strideOut;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_2D;

   pKerPrivArgs->numVecs = DSPLIB_ceilingDiv(bufParamsOut->dim_x, eleCount) * bufParamsOut->dim_y;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_blkCopyConst2D_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_blkCopyConst2D_exec_ci function");

   DSPLIB_blkCopyConst2D_PrivArgs                        *pKerPrivArgs = (DSPLIB_blkCopyConst2D_PrivArgs *) handle;
   typedef typename c7x::make_full_vector<dataType>::type vec;

   dataType *restrict pInLocal  = (dataType *) pIn1;
   dataType *restrict pOutLocal = (dataType *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p\n", pInLocal, pOutLocal);

   uint8_t *pBlock  = pKerPrivArgs->bufPblock;
   uint32_t numVecs = pKerPrivArgs->numVecs;

   /* Prepare SA write output */
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   vec data = (vec) (*pInLocal);

   __SA0_OPEN(sa0Params);
   for (uint32_t vecCount = 0; vecCount < numVecs; vecCount++) {

      __vpred opWr    = c7x::strm_agen<0, vec>::get_vpred();
      vec    *opWrPtr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(opWr, opWrPtr, data);
   }
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

// Template instantiations for supported data types
template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<int8_t>(DSPLIB_kernelHandle                  handle,
                                                             const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                             const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                             const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<uint8_t>(DSPLIB_kernelHandle                  handle,
                                                              const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                              const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                              const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<int16_t>(DSPLIB_kernelHandle                  handle,
                                                              const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                              const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                              const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<uint16_t>(DSPLIB_kernelHandle                  handle,
                                                               const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                               const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                               const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<int32_t>(DSPLIB_kernelHandle                  handle,
                                                              const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                              const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                              const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<uint32_t>(DSPLIB_kernelHandle                  handle,
                                                               const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                               const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                               const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<int64_t>(DSPLIB_kernelHandle                  handle,
                                                              const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                              const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                              const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<uint64_t>(DSPLIB_kernelHandle                  handle,
                                                               const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                               const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                               const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<float>(DSPLIB_kernelHandle                  handle,
                                                            const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                            const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                            const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopyConst2D_init_ci<double>(DSPLIB_kernelHandle                  handle,
                                                             const DSPLIB_bufParams1D_t          *bufParamsIn1,
                                                             const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                             const DSPLIB_blkCopyConst2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopyConst2D_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pOut);
