// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_absoluteValue_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_absoluteValue_init_ci(DSPLIB_kernelHandle                 handle,
                                           const DSPLIB_bufParams2D_t         *bufParamsIn,
                                           const DSPLIB_bufParams2D_t         *bufParamsOut,
                                           const DSPLIB_absoluteValueInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_absoluteValue_init_ci function");

   DSPLIB_STATUS                  status       = DSPLIB_SUCCESS;
   DSPLIB_absoluteValue_PrivArgs *pKerPrivArgs = (DSPLIB_absoluteValue_PrivArgs *) handle;

   /* Initialize the 2D ABS parameters */
   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_TEMPLATE_v1                                       se0Params;
   __SA_TEMPLATE_v1                                       sa0Params;
   __SE_ELETYPE                                           SE_ELETYPE;
   __SE_VECLEN                                            SE_VECLEN;
   __SA_VECLEN                                            SA_VECLEN;

   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

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
DSPLIB_STATUS DSPLIB_absoluteValue_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering DSPLIB_absoluteValue_exec_ci function");

   DSPLIB_absoluteValue_PrivArgs *pKerPrivArgs = (DSPLIB_absoluteValue_PrivArgs *) handle;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;

   /* Execute the 2D ABS operation */
   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_TEMPLATE_v1                                       se0Params;
   __SA_TEMPLATE_v1                                       sa0Params;
   uint8_t                                               *pBlock  = pKerPrivArgs->bufPblock;
   int32_t                                                numVecs = pKerPrivArgs->numVecs;

   /* Prepare SE to read input */
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   /* Prepare SA write output */
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   /* ii = 1 cycle per vector for every data type on C7504 and C7524 (from the compiled .asm). The loop has one SE
    * read, one absolute value and one SA store per vector, so it sits on the one-vector-store-per-cycle limit and
    * cannot be unrolled any further. */
   for (int32_t vecIdx = 0; vecIdx < numVecs; vecIdx++) {
      vec     inVec     = c7x::strm_eng<0, vec>::get_adv();
      __vpred storePred = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pOutVec   = c7x::strm_agen<0, vec>::get_adv(pOutLocal);

      vec absVec = __abs(inVec);
      __vstore_pred(storePred, pOutVec, absVec);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}
void DSPLIB_absoluteValue_perfEst(DSPLIB_kernelHandle handle,
                                  uint64_t           *archCycles,
                                  uint64_t           *estCycles,
                                  uint32_t            dataType)
{
   DSPLIB_absoluteValue_PrivArgs *pKerPrivArgs = (DSPLIB_absoluteValue_PrivArgs *) handle;

   *archCycles =
       DSPLIB_ABSOLUTEVALUE_PIPE_UP_CYCLES + (uint64_t) pKerPrivArgs->numVecs * DSPLIB_ABSOLUTEVALUE_CYCLES_PER_VECTOR;
   *estCycles = *archCycles + DSPLIB_ABSOLUTEVALUE_CALL_OVERHEAD_CYCLES;
}
/* Template instantiations for supported data types */
template DSPLIB_STATUS DSPLIB_absoluteValue_init_ci<int8_t>(DSPLIB_kernelHandle                 handle,
                                                            const DSPLIB_bufParams2D_t         *bufParamsIn,
                                                            const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                            const DSPLIB_absoluteValueInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_absoluteValue_init_ci<int16_t>(DSPLIB_kernelHandle                 handle,
                                                             const DSPLIB_bufParams2D_t         *bufParamsIn,
                                                             const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                             const DSPLIB_absoluteValueInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_absoluteValue_init_ci<int32_t>(DSPLIB_kernelHandle                 handle,
                                                             const DSPLIB_bufParams2D_t         *bufParamsIn,
                                                             const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                             const DSPLIB_absoluteValueInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_absoluteValue_init_ci<int64_t>(DSPLIB_kernelHandle                 handle,
                                                             const DSPLIB_bufParams2D_t         *bufParamsIn,
                                                             const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                             const DSPLIB_absoluteValueInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_absoluteValue_init_ci<float>(DSPLIB_kernelHandle                 handle,
                                                           const DSPLIB_bufParams2D_t         *bufParamsIn,
                                                           const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                           const DSPLIB_absoluteValueInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_absoluteValue_init_ci<double>(DSPLIB_kernelHandle                 handle,
                                                            const DSPLIB_bufParams2D_t         *bufParamsIn,
                                                            const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                            const DSPLIB_absoluteValueInitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_absoluteValue_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
