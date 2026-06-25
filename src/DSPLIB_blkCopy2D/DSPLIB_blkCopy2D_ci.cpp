// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blkCopy2D_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci(DSPLIB_kernelHandle             handle,
                                       const DSPLIB_bufParams2D_t     *bufParamsIn,
                                       const DSPLIB_bufParams2D_t     *bufParamsOut,
                                       const DSPLIB_blkCopy2DInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_blkCopy2D_init_ci function");

   DSPLIB_STATUS              status       = DSPLIB_SUCCESS;
   DSPLIB_blkCopy2D_PrivArgs *pKerPrivArgs = (DSPLIB_blkCopy2D_PrivArgs *) handle;

   // Initialize the 2D block copy parameters
   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_TEMPLATE_v1                                       se0Params;
   __SA_TEMPLATE_v1                                       sa0Params;
   __SE_ELETYPE                                           SE_ELETYPE;
   __SE_VECLEN                                            SE_VECLEN;
   __SA_VECLEN                                            SA_VECLEN;

   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

   // Calculate number of vector reads per the whole block
   int32_t eleCount = c7x::element_count_of<vec>::value;
   if (bufParamsIn->dim_x == pKerPrivArgs->strideIn && bufParamsOut->dim_x == pKerPrivArgs->strideOut) {
      pKerPrivArgs->numVecs = ((bufParamsIn->dim_x * bufParamsIn->dim_y + eleCount - 1) / eleCount);

      /* Prepare SE template for reading input */
      se0Params         = __gen_SE_TEMPLATE_v1();
      se0Params.ICNT0   = bufParamsIn->dim_x * bufParamsIn->dim_y;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DIMFMT  = __SE_DIMFMT_1D;

      /* Prepare SA template for writing output */
      sa0Params        = __gen_SA_TEMPLATE_v1();
      sa0Params.ICNT0  = bufParamsIn->dim_x * bufParamsIn->dim_y;
      sa0Params.VECLEN = SA_VECLEN;
      sa0Params.DIMFMT = __SA_DIMFMT_1D;
   }
   else {
      pKerPrivArgs->numVecs = ((bufParamsIn->dim_x + eleCount - 1) / eleCount) * bufParamsIn->dim_y;

      /* Prepare SE template for reading input */
      se0Params         = __gen_SE_TEMPLATE_v1();
      se0Params.ICNT0   = bufParamsIn->dim_x;
      se0Params.ICNT1   = bufParamsIn->dim_y;
      se0Params.DIM1    = pKerPrivArgs->strideIn;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DIMFMT  = __SE_DIMFMT_2D;

      /* Prepare SA template for writing output */
      sa0Params        = __gen_SA_TEMPLATE_v1();
      sa0Params.ICNT0  = bufParamsOut->dim_x;
      sa0Params.ICNT1  = bufParamsOut->dim_y;
      sa0Params.DIM1   = pKerPrivArgs->strideOut;
      sa0Params.VECLEN = SA_VECLEN;
      sa0Params.DIMFMT = __SA_DIMFMT_2D;
   }

   /* Store prepared SE/SA templates */
   uint8_t *pBlock                                                  = pKerPrivArgs->bufPblock;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_blkCopy2D_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_blkCopy2D_exec_ci function");

   DSPLIB_blkCopy2D_PrivArgs *pKerPrivArgs = (DSPLIB_blkCopy2D_PrivArgs *) handle;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p\n", pInLocal, pOutLocal);

   // Execute the 2D block copy
   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_TEMPLATE_v1                                       se0Params;
   __SA_TEMPLATE_v1                                       sa0Params;
   uint32_t                                               vecCount;
   uint8_t                                               *pBlock  = pKerPrivArgs->bufPblock;
   uint32_t                                               numVecs = pKerPrivArgs->numVecs;

   /* Prepare SE to read input */
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   /* Prepare SA write output */
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   for (vecCount = 0; vecCount < numVecs; vecCount++) {
      vec     data    = c7x::strm_eng<0, vec>::get_adv();
      __vpred opWr    = c7x::strm_agen<0, vec>::get_vpred();
      vec    *opWrPtr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(opWr, opWrPtr, data);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

// Template instantiations for supported data types
template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<int8_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                        const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<uint8_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                         const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<int16_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                         const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<uint16_t>(DSPLIB_kernelHandle             handle,
                                                          const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                          const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                          const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<int32_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                         const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<uint32_t>(DSPLIB_kernelHandle             handle,
                                                          const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                          const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                          const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<int64_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                         const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<uint64_t>(DSPLIB_kernelHandle             handle,
                                                          const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                          const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                          const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<float>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                       const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blkCopy2D_init_ci<double>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                        const DSPLIB_blkCopy2DInitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blkCopy2D_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
