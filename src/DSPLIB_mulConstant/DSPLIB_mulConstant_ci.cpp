// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_mulConstant_priv.h"
#include <float.h>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

template <typename dataType>
DSPLIB_STATUS DSPLIB_mulConstant_init_ci(DSPLIB_kernelHandle                handle,
                                         const DSPLIB_bufParams1D_t        *bufParamsIn,
                                         const DSPLIB_bufParams1D_t        *bufParamsOut,
                                         const DSPLIB_mulConstant_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;

   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   uint32_t blockSize = pKerPrivArgs->blockSize;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

   se0Params = __gen_SE_TEMPLATE_v1();
   se0Params.ICNT0   = blockSize;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.DIMFMT  = __SE_DIMFMT_1D;

   sa0Params = __gen_SA_TEMPLATE_v1();
   sa0Params.ICNT0  = blockSize;
   sa0Params.DIM1   = blockSize;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   pKerPrivArgs->nElePerVec = c7x::element_count_of<vec>::value;
   /* cfloat uses VCMPYSP + VADDSP in series (latency 8); all other types latency 4 */
   pKerPrivArgs->prologCycles = std::is_same<dataType, cfloat>::value ? 7 : 4;

   return status;
}

void DSPLIB_mulConstant_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;

   uint64_t nVecs = ((uint64_t) pKerPrivArgs->blockSize + pKerPrivArgs->nElePerVec - 1) / pKerPrivArgs->nElePerVec;
   *archCycles    = 41 + pKerPrivArgs->prologCycles + nVecs;
   *estCycles     = *archCycles;
}

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<int8_t>(DSPLIB_kernelHandle                handle,
                                                          const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                          const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                          const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<uint8_t>(DSPLIB_kernelHandle                handle,
                                                           const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                           const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                           const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<int16_t>(DSPLIB_kernelHandle                handle,
                                                           const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                           const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                           const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<uint16_t>(DSPLIB_kernelHandle                handle,
                                                            const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                            const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                            const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<int32_t>(DSPLIB_kernelHandle                handle,
                                                           const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                           const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                           const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<uint32_t>(DSPLIB_kernelHandle                handle,
                                                            const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                            const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                            const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<float>(DSPLIB_kernelHandle                handle,
                                                         const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                         const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<double>(DSPLIB_kernelHandle                handle,
                                                          const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                          const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                          const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<cfloat>(DSPLIB_kernelHandle                handle,
                                                          const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                          const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                          const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<cshort>(DSPLIB_kernelHandle                handle,
                                                          const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                          const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                          const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_mulConstant_init_ci<cint>(DSPLIB_kernelHandle                handle,
                                                        const DSPLIB_bufParams1D_t        *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t        *bufParamsOut,
                                                        const DSPLIB_mulConstant_InitArgs *pKerInitArgs);

/*---------------------------------------------------------------------------
 * Real-type exec template (float, double, int8/16/32, uint8/16/32)
 *---------------------------------------------------------------------------*/
template <typename dataType, int32_t dataIn>
DSPLIB_STATUS
DSPLIB_mulConstant_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;
   uint32_t                     blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   dataType *restrict pInLocal1 = (dataType *) pIn1;
   dataType *restrict pInLocal2 = (dataType *) pIn2;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_mulConstant_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal2, se0Params);
   __SA0_OPEN(sa0Params);

   dataType x = *pInLocal1;
   vec      a = (vec) x;

   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec b   = c7x::strm_eng<0, vec>::get_adv();
      vec out = a * b;

      __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out);
   }
   __SE0_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_mulConstant_exec_ci<float, DSPLIB_FLOAT32>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);

template DSPLIB_STATUS DSPLIB_mulConstant_exec_ci<double, DSPLIB_FLOAT64>(DSPLIB_kernelHandle handle,
                                                                          void *restrict pIn1,
                                                                          void *restrict pIn2,
                                                                          void *restrict pOut);

template DSPLIB_STATUS DSPLIB_mulConstant_exec_ci<int8_t, DSPLIB_INT8>(DSPLIB_kernelHandle handle,
                                                                       void *restrict pIn1,
                                                                       void *restrict pIn2,
                                                                       void *restrict pOut);

template DSPLIB_STATUS DSPLIB_mulConstant_exec_ci<uint8_t, DSPLIB_UINT8>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);

template DSPLIB_STATUS DSPLIB_mulConstant_exec_ci<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);

template DSPLIB_STATUS DSPLIB_mulConstant_exec_ci<uint16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn1,
                                                                           void *restrict pIn2,
                                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_mulConstant_exec_ci<int32_t, DSPLIB_INT32>(DSPLIB_kernelHandle handle,
                                                                         void *restrict pIn1,
                                                                         void *restrict pIn2,
                                                                         void *restrict pOut);

template DSPLIB_STATUS DSPLIB_mulConstant_exec_ci<uint32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle,
                                                                           void *restrict pIn1,
                                                                           void *restrict pIn2,
                                                                           void *restrict pOut);

/* cfloat: complex float multiply via __complex_multiply */
template <>
DSPLIB_STATUS
DSPLIB_mulConstant_exec_ci<cfloat, DSPLIB_CFLOAT32>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pOut)
{
   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;
   uint32_t                     blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   cfloat *pInLocal2 = (cfloat *) pIn2;
   cfloat *pOutLocal = (cfloat *) pOut;

   typedef typename c7x::make_full_vector<cfloat>::type vec;
   int32_t eleCount = c7x::element_count_of<vec>::value;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal2, se0Params);
   __SA0_OPEN(sa0Params);

   cfloat x = *(cfloat *) pIn1;
   vec    a = __duplicate(x);

   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec b   = c7x::strm_eng<0, vec>::get_adv();

      vec out = __complex_multiply(a, b);

      __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out);
   }
   __SE0_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

/* cshort: Q15 complex multiply via __cmpy_fx (VCMPYR1HH) */
template <>
DSPLIB_STATUS
DSPLIB_mulConstant_exec_ci<cshort, DSPLIB_CINT16>(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn1,
                                                   void *restrict pIn2,
                                                   void *restrict pOut)
{
   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;
   uint32_t                     blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   cshort *pInLocal2 = (cshort *) pIn2;
   cshort *pOutLocal = (cshort *) pOut;

   typedef typename c7x::make_full_vector<cshort>::type vec;
   int32_t                                              eleCount = c7x::element_count_of<vec>::value;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal2, se0Params);
   __SA0_OPEN(sa0Params);

   cshort x = *(cshort *) pIn1;
   vec    a = __duplicate(x);

   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec b   = c7x::strm_eng<0, vec>::get_adv();
      vec out = __cmpy_fx(a, b);

      __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out);
   }
   __SE0_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

/* cint: Q31 complex multiply via __cmpy_fx (VCMPYR1WW) */
template <>
DSPLIB_STATUS
DSPLIB_mulConstant_exec_ci<cint, DSPLIB_CINT32>(DSPLIB_kernelHandle handle,
                                                 void *restrict pIn1,
                                                 void *restrict pIn2,
                                                 void *restrict pOut)
{
   DSPLIB_mulConstant_PrivArgs *pKerPrivArgs = (DSPLIB_mulConstant_PrivArgs *) handle;
   uint32_t                     blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   cint *pInLocal2 = (cint *) pIn2;
   cint *pOutLocal = (cint *) pOut;

   typedef typename c7x::make_full_vector<cint>::type vec;
   int32_t                                            eleCount = c7x::element_count_of<vec>::value;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal2, se0Params);
   __SA0_OPEN(sa0Params);

   cint x = *(cint *) pIn1;
   vec  a = __duplicate(x);

   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec b   = c7x::strm_eng<0, vec>::get_adv();
      vec out = __cmpy_fx(a, b);

      __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out);
   }
   __SE0_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}
