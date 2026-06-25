// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_q15tofl_priv.h"

template <typename dataTypeOut> inline __SE_TEMPLATE_v1 q15tofl_se_init(int32_t blockSize);

template <> inline __SE_TEMPLATE_v1 q15tofl_se_init<float>(int32_t blockSize)
{
   typedef typename c7x::short_hvec                    vecIn;
   typedef typename c7x::make_full_vector<float>::type vecOut;
   __SE_ELETYPE                                        SE_ELETYPE = c7x::se_eletype<vecIn>::value;
   __SE_VECLEN                                         SE_VECLEN  = c7x::se_veclen<vecOut>::value;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   se0Params.ICNT0            = blockSize;
   se0Params.ELETYPE          = SE_ELETYPE;
   se0Params.VECLEN           = SE_VECLEN;
   se0Params.DIMFMT           = __SE_DIMFMT_1D;
   se0Params.PROMOTE          = __SE_PROMOTE_2X_SIGNEXT;

   return se0Params;
}

template <> inline __SE_TEMPLATE_v1 q15tofl_se_init<double>(int32_t blockSize)
{
   typedef typename c7x::short_qvec                     vecIn;
   typedef typename c7x::make_full_vector<double>::type vecOut;
   __SE_ELETYPE                                         SE_ELETYPE = c7x::se_eletype<vecIn>::value;
   __SE_VECLEN                                          SE_VECLEN  = c7x::se_veclen<vecOut>::value;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   se0Params.ICNT0            = blockSize;
   se0Params.ELETYPE          = SE_ELETYPE;
   se0Params.VECLEN           = SE_VECLEN;
   se0Params.DIMFMT           = __SE_DIMFMT_1D;
   se0Params.PROMOTE          = __SE_PROMOTE_4X_SIGNEXT;

   return se0Params;
}

template <typename dataTypeOut>
DSPLIB_STATUS DSPLIB_q15tofl_init_ci(DSPLIB_kernelHandle            handle,
                                     const DSPLIB_bufParams1D_t    *bufParamsIn,
                                     const DSPLIB_bufParams1D_t    *bufParamsOut,
                                     const DSPLIB_q15tofl_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS            status       = DSPLIB_SUCCESS;
   DSPLIB_q15tofl_PrivArgs *pKerPrivArgs = (DSPLIB_q15tofl_PrivArgs *) handle;
   uint32_t                 blockSize    = pKerPrivArgs->blockSize;
   uint8_t                 *pBlock       = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataTypeOut>::type vecOut;

   __SA_VECLEN SA_VECLEN = c7x::sa_veclen<vecOut>::value;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch the input                      */
   /**********************************************************************/
   __SE_TEMPLATE_v1 se0Params = q15tofl_se_init<dataTypeOut>(blockSize);
   DSPLIB_DEBUGPRINTFN(0, "se0Params.PROMOTE: %d\n", se0Params.PROMOTE);

   /**********************************************************************/
   /* Prepare SA template to store output                                */
   /**********************************************************************/
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   sa0Params.ICNT0            = blockSize;
   sa0Params.VECLEN           = SA_VECLEN;
   sa0Params.DIMFMT           = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

template DSPLIB_STATUS DSPLIB_q15tofl_init_ci<float>(DSPLIB_kernelHandle            handle,
                                                     const DSPLIB_bufParams1D_t    *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t    *bufParamsOut,
                                                     const DSPLIB_q15tofl_InitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_q15tofl_init_ci<double>(DSPLIB_kernelHandle            handle,
                                                      const DSPLIB_bufParams1D_t    *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t    *bufParamsOut,
                                                      const DSPLIB_q15tofl_InitArgs *pKerInitArgs);

template <typename dataTypeOut> inline void q15tofl_convert(dataTypeOut *restrict pOutLocal, int32_t blockSize);

template <> inline void q15tofl_convert<float>(float *restrict pOutLocal, int32_t blockSize)
{
   typedef typename c7x::int_vec                       vecLoad;
   typedef typename c7x::make_full_vector<float>::type vecOut;
   int32_t                                             eleCount = c7x::element_count_of<vecOut>::value;
   vecOut                                              outDat, scale;

   scale = (vecOut) (1.0 / 32768);

   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vecLoad inDat     = c7x::strm_eng<0, vecLoad>::get_adv();
      vecOut  inDatConv = __int_to_float(c7x::as_int_vec(inDat));
      outDat            = inDatConv * scale;

      /* Store the result */
      __vpred vpred = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr  = c7x::strm_agen<0, vecOut>::get_adv(pOutLocal);
      __vstore_pred(vpred, addr, outDat);
   }
}

template <> inline void q15tofl_convert<double>(double *restrict pOutLocal, int32_t blockSize)
{
   typedef typename c7x::long_vec                       vecLoad;
   typedef typename c7x::make_full_vector<double>::type vecOut;
   int32_t                                              eleCount = c7x::element_count_of<vecOut>::value;
   vecOut                                               outDat, scale;

   scale = (vecOut) (1.0 / 32768);

   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vecLoad inDat     = c7x::strm_eng<0, vecLoad>::get_adv();
      vecOut  inDatConv = __low_int_to_double(c7x::as_int_vec(inDat));
      outDat            = inDatConv * scale;

      /* Store the result */
      __vpred vpred = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr  = c7x::strm_agen<0, vecOut>::get_adv(pOutLocal);
      __vstore_pred(vpred, addr, outDat);
   }
}

template <typename dataTypeOut>
DSPLIB_STATUS DSPLIB_q15tofl_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_q15tofl_PrivArgs *pKerPrivArgs = (DSPLIB_q15tofl_PrivArgs *) handle;
   int32_t                  blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   int16_t *restrict pInLocal      = (int16_t *) pIn;
   dataTypeOut *restrict pOutLocal = (dataTypeOut *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p blockSize: %d\n", pInLocal, pOutLocal, blockSize);

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   se0Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params       = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   q15tofl_convert<dataTypeOut>(pOutLocal, blockSize);

   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS
DSPLIB_q15tofl_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_q15tofl_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
