// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_fltoq15_priv.h"

/**********************************************************************/
/* INITIALIZATION                                                     */
/**********************************************************************/
template <typename dataTypeIn> inline void fltoq15_stream_init(uint8_t *pBlock, int32_t blockSize);

template <> inline void fltoq15_stream_init<float>(uint8_t *pBlock, int32_t blockSize)
{
   typedef typename c7x::make_full_vector<float>::type vecIn;
   int32_t                                             eleCount = c7x::element_count_of<vecIn>::value;
   typedef typename c7x::short_vec                     vecOut;
   int32_t                                             loopCount = blockSize / (eleCount * 2);

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vecIn>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vecIn>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vecOut>::value;

   /* Prepare streaming engine 0 to fetch the input */
   se0Params.ICNT0   = eleCount;
   se0Params.ICNT1   = loopCount;
   se0Params.DIM1    = eleCount * 2;
   se0Params.DIMFMT  = __SE_DIMFMT_2D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;

   /* Prepare SA template to store output */
   sa0Params.ICNT0  = blockSize;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
}

template <> inline void fltoq15_stream_init<double>(uint8_t *pBlock, int32_t blockSize)
{

   typedef typename c7x::make_full_vector<double>::type vecIn;
   int32_t                                              eleCount = c7x::element_count_of<vecIn>::value;
   typedef typename c7x::short_hvec                     vecOut;
   int32_t                                              loopCount = blockSize / (eleCount * 2);

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vecIn>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vecIn>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vecOut>::value;

   /* Prepare streaming engine 0 to fetch the input */
   se0Params.ICNT0   = eleCount;
   se0Params.ICNT1   = loopCount;
   se0Params.DIM1    = eleCount * 2;
   se0Params.DIMFMT  = __SE_DIMFMT_2D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;

   /* Prepare SA template to store output */
   sa0Params.ICNT0  = blockSize;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
}

template <typename dataTypeIn>
DSPLIB_STATUS DSPLIB_fltoq15_init_ci(DSPLIB_kernelHandle            handle,
                                     const DSPLIB_bufParams1D_t    *bufParamsIn,
                                     const DSPLIB_bufParams1D_t    *bufParamsOut,
                                     const DSPLIB_fltoq15_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS            status       = DSPLIB_SUCCESS;
   DSPLIB_fltoq15_PrivArgs *pKerPrivArgs = (DSPLIB_fltoq15_PrivArgs *) handle;
   uint32_t                 blockSize    = pKerPrivArgs->blockSize;
   uint8_t                 *pBlock       = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataTypeIn>::type vecIn;
   int32_t                                                  eleCount  = c7x::element_count_of<vecIn>::value;
   int32_t                                                  loopCount = blockSize / (eleCount * 2);

   if (loopCount == 0) {
      pKerPrivArgs->isSEReadEnabled = false;
   }
   else {
      pKerPrivArgs->isSEReadEnabled = true;
      fltoq15_stream_init<dataTypeIn>(pBlock, blockSize);
   }

   DSPLIB_DEBUGPRINTFN(0, "isSEReadEnabled: %d\n", pKerPrivArgs->isSEReadEnabled);
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
template DSPLIB_STATUS DSPLIB_fltoq15_init_ci<float>(DSPLIB_kernelHandle            handle,
                                                     const DSPLIB_bufParams1D_t    *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t    *bufParamsOut,
                                                     const DSPLIB_fltoq15_InitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_fltoq15_init_ci<double>(DSPLIB_kernelHandle            handle,
                                                      const DSPLIB_bufParams1D_t    *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t    *bufParamsOut,
                                                      const DSPLIB_fltoq15_InitArgs *pKerInitArgs);

/**********************************************************************/
/* IMPLEMENTATION                                                     */
/**********************************************************************/
template <typename dataTypeIn>
inline void fltoq15_convert(void *restrict pIn,
                            void *restrict pOut,
                            __SE_TEMPLATE_v1 se0Params,
                            __SA_TEMPLATE_v1 sa0Params,
                            uint32_t         blockSize,
                            bool             isSEReadEnabled);

template <>
inline void fltoq15_convert<float>(void *restrict pIn,
                                   void *restrict pOut,
                                   __SE_TEMPLATE_v1 se0Params,
                                   __SA_TEMPLATE_v1 sa0Params,
                                   uint32_t         blockSize,
                                   bool             isSEReadEnabled)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for FLOAT32 to Q15 conversion");

   float *restrict pInLocal    = (float *) pIn;
   int16_t *restrict pOutLocal = (int16_t *) pOut;

   typedef typename c7x::make_full_vector<float>::type vecIn;
   typedef typename c7x::short_vec                     vecOut;

   int32_t eleCount          = c7x::element_count_of<vecIn>::value;
   int32_t loopCount         = blockSize / (eleCount * 2);
   int32_t remainingElements = blockSize - (loopCount * eleCount * 2);
   int32_t i;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p blockSize: %d loopCount: %d remainingElements: %d\n", pInLocal,
                       pOutLocal, blockSize, loopCount, remainingElements);

   if (isSEReadEnabled) {
      vecIn mulFactor = (vecIn) 32768.0;

      __SE0_OPEN(pInLocal, se0Params);
      __SE1_OPEN(pInLocal + eleCount, se0Params);
      __SA0_OPEN(sa0Params);

      for (i = 0; i < loopCount; i++) {
         vecIn loadVec1 = c7x::strm_eng<0, vecIn>::get_adv();
         vecIn loadVec2 = c7x::strm_eng<1, vecIn>::get_adv();

         vecIn mulLoadVec1 = loadVec1 * mulFactor;
         vecIn mulLoadVec2 = loadVec2 * mulFactor;

         vecOut intermediate1 = __float_to_short(mulLoadVec1);
         vecOut intermediate2 = __float_to_short(mulLoadVec2);

         c7x::short_vec *addr = c7x::strm_agen<0, c7x::short_vec>::get_adv(pOutLocal);
         __vstore_packl_2src(addr, c7x::as_int_vec(intermediate1), c7x::as_int_vec(intermediate2));
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();
   }

   pInLocal += loopCount * eleCount * 2;
   pOutLocal += loopCount * eleCount * 2;

#pragma PROB_ITERATE(, 31)
   for (i = 0; i < remainingElements; i++) {
      float  inData      = *pInLocal++;
      float  mult        = inData * 32768.00f;
      short2 convertData = __float_to_short(mult);
      *pOutLocal++       = convertData.s[0];
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for FLOAT32 to Q15 conversion");
}

template <>
inline void fltoq15_convert<double>(void *restrict pIn,
                                    void *restrict pOut,
                                    __SE_TEMPLATE_v1 se0Params,
                                    __SA_TEMPLATE_v1 sa0Params,
                                    uint32_t         blockSize,
                                    bool             isSEReadEnabled)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for FLOAT64 to Q15 conversion");

   double *restrict pInLocal   = (double *) pIn;
   int16_t *restrict pOutLocal = (int16_t *) pOut;

   typedef typename c7x::make_full_vector<double>::type vecIn;

   int32_t eleCount          = c7x::element_count_of<vecIn>::value;
   int32_t loopCount         = blockSize / (eleCount * 2);
   int32_t remainingElements = blockSize - (loopCount * eleCount * 2);
   int32_t i;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p blockSize: %d loopCount: %d remainingElements: %d\n", pInLocal,
                       pOutLocal, blockSize, loopCount, remainingElements);

   if (isSEReadEnabled) {
      vecIn mulFactor = (vecIn) 32768.0;

      __SE0_OPEN(pInLocal, se0Params);
      __SE1_OPEN(pInLocal + eleCount, se0Params);
      __SA0_OPEN(sa0Params);

      for (i = 0; i < loopCount; i++) {

         vecIn loadVec1 = c7x::strm_eng<0, vecIn>::get_adv();
         vecIn loadVec2 = c7x::strm_eng<1, vecIn>::get_adv();

         loadVec1 = loadVec1 * mulFactor;
         loadVec2 = loadVec2 * mulFactor;

         c7x::int_vec dpToInt1 = __double_to_int(loadVec1);
         c7x::int_vec dpToInt2 = __double_to_int(loadVec2);

         c7x::int_vec pack1 = __pack_consec_low(dpToInt2, dpToInt1);

         pack1 = __int_to_short_sat(pack1);

         c7x::short_hvec *addr = c7x::strm_agen<0, c7x::short_hvec>::get_adv(pOutLocal);
         __vstore_packl(addr, c7x::as_int_vec(pack1));
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();
   }

   pInLocal += loopCount * eleCount * 2;
   pOutLocal += loopCount * eleCount * 2;
#pragma PROB_ITERATE(, 15)
   for (i = 0; i < remainingElements; i++) {
      double inData      = *pInLocal++;
      double mult        = inData * 32768.00;
      int2   convTemp1   = __double_to_int(mult);
      int2   convertData = __int_to_short_sat(convTemp1);
      *pOutLocal++       = convertData.s[0];
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting tempate function for FLOAT64 to Q15 conversion");
}

template <typename dataTypeIn>
DSPLIB_STATUS DSPLIB_fltoq15_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_fltoq15_PrivArgs *pKerPrivArgs    = (DSPLIB_fltoq15_PrivArgs *) handle;
   uint32_t                 blockSize       = pKerPrivArgs->blockSize;
   bool                     isSEReadEnabled = pKerPrivArgs->isSEReadEnabled;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   se0Params       = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params       = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   fltoq15_convert<dataTypeIn>(pIn, pOut, se0Params, sa0Params, blockSize, isSEReadEnabled);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}
template DSPLIB_STATUS
DSPLIB_fltoq15_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_fltoq15_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
