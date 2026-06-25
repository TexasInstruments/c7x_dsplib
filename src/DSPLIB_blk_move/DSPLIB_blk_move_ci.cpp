// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_blk_move_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_blk_move_init_ci(DSPLIB_kernelHandle             handle,
                                      const DSPLIB_bufParams1D_t     *bufParamsIn,
                                      const DSPLIB_bufParams1D_t     *bufParamsOut,
                                      const DSPLIB_blk_move_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS                                          status       = DSPLIB_SUCCESS;
   DSPLIB_blk_move_PrivArgs                              *pKerPrivArgs = (DSPLIB_blk_move_PrivArgs *) handle;
   uint32_t                                               blockSize    = pKerPrivArgs->blockSize;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_TEMPLATE_v1                                       se0Params;
   __SA_TEMPLATE_v1                                       sa0Params;

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch the input                      */
   /**********************************************************************/
   se0Params         = __gen_SE_TEMPLATE_v1();
   se0Params.ICNT0   = blockSize;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.DIMFMT  = __SE_DIMFMT_1D;

   /**********************************************************************/
   /* Prepare SA template to store output                                */
   /**********************************************************************/
   sa0Params        = __gen_SA_TEMPLATE_v1();
   sa0Params.ICNT0  = blockSize;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<int8_t>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                       const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<uint8_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<int16_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<uint16_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                         const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<int32_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<uint32_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                         const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<int64_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<uint64_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                         const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<float>(DSPLIB_kernelHandle             handle,
                                                      const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                      const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_blk_move_init_ci<double>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                       const DSPLIB_blk_move_InitArgs *pKerInitArgs);

template <typename dataType>
DSPLIB_STATUS DSPLIB_blk_move_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_blk_move_PrivArgs *pKerPrivArgs = (DSPLIB_blk_move_PrivArgs *) handle;
   uint32_t                  blockSize    = pKerPrivArgs->blockSize;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p blockSize: %d eleCount: %d\n", pInLocal, pOutLocal, blockSize,
                       eleCount);

   uint8_t         *pBlock    = pKerPrivArgs->bufPblock;
   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal, se0Params);
   // Output samples
   __SA0_OPEN(sa0Params);

   for (uint32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec     out   = c7x::strm_eng<0, vec>::get_adv();
      __vpred vpred = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(vpred, addr, out);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_blk_move_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
