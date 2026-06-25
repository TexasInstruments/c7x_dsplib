// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_matMul_priv.h"
#include "DSPLIB_types.h"
#include <cstdint>
#include <float.h>

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA1_PARAM_OFFSET (SA0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA2_PARAM_OFFSET (SA1_PARAM_OFFSET + SE_PARAM_SIZE)

#define DSPLIB_MATMUL_DOUBLE_UNROLL_FACTOR (8)
#define DSPLIB_MATMUL_SE_DOUBLE_UNROLL_FACTOR (4)

#if !defined(__C7504__) && !defined(__C7524__)
#define DSPLIB_MATMUL_FLOAT_UNROLL_FACTOR (16)
#define DSPLIB_MATMUL_SE_FLOAT_UNROLL_FACTOR (8)
#else
#define DSPLIB_MATMUL_FLOAT_UNROLL_FACTOR (8)
#define DSPLIB_MATMUL_SE_FLOAT_UNROLL_FACTOR (4)
#endif

template <typename dataType> inline void setUnrollFactors(int32_t *unrollFactor, int32_t *seUnrollFactor);

template <> inline void setUnrollFactors<float>(int32_t *unrollFactor, int32_t *seUnrollFactor)
{
   *unrollFactor   = DSPLIB_MATMUL_FLOAT_UNROLL_FACTOR;
   *seUnrollFactor = DSPLIB_MATMUL_SE_FLOAT_UNROLL_FACTOR;
}
template <> inline void setUnrollFactors<double>(int32_t *unrollFactor, int32_t *seUnrollFactor)
{
   *unrollFactor   = DSPLIB_MATMUL_DOUBLE_UNROLL_FACTOR;
   *seUnrollFactor = DSPLIB_MATMUL_SE_DOUBLE_UNROLL_FACTOR;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_generic_init_ci(DSPLIB_kernelHandle           handle,
                                            const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                            const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                            const DSPLIB_bufParams2D_t   *bufParamsOut,
                                            const DSPLIB_matMul_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   int32_t M         = pKerPrivArgs->M;
   int32_t K         = pKerPrivArgs->K;
   int32_t N         = pKerPrivArgs->N;
   int32_t strideIn0 = pKerPrivArgs->strideIn0Elements;
   int32_t strideIn1 = pKerPrivArgs->strideIn1Elements;
   int32_t strideOut = pKerPrivArgs->strideOutElements;

   int32_t unrollFactor   = 0;
   int32_t seUnrollFactor = 0;

   setUnrollFactors<dataType>(&unrollFactor, &seUnrollFactor);

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t elementCount = c7x::element_count_of<vec>::value;
   SE_VECLEN            = c7x::se_veclen<vec>::value;
   SA_VECLEN            = c7x::sa_veclen<vec>::value;
   SE_ELETYPE           = c7x::se_eletype<vec>::value;
   int32_t KBlocks      = ((K + unrollFactor - 1)) / (unrollFactor);
   int32_t NBlocks      = ((N + elementCount - 1)) / (elementCount);

   pKerPrivArgs->KBlocks = KBlocks;
   pKerPrivArgs->NBlocks = NBlocks;

   /**********************************************************************/
   /* Prepare SA0 template to fetch A matrix                             */
   /**********************************************************************/

   sa0Params           = __gen_SA_TEMPLATE_v1();
   sa0Params.VECLEN    = SA_VECLEN;
   sa0Params.DIMFMT    = __SA_DIMFMT_5D;
   sa0Params.DECDIM1   = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1SD = __SA_DECDIMSD_DIM1;

   sa0Params.ICNT0         = 1;
   sa0Params.ICNT1         = seUnrollFactor;
   sa0Params.DIM1          = 2;
   sa0Params.ICNT2         = KBlocks;
   sa0Params.DIM2          = (int32_t) ((uint32_t) seUnrollFactor << (uint32_t) 1);
   sa0Params.DECDIM1_WIDTH = K;
   sa0Params.ICNT3         = NBlocks;
   sa0Params.DIM3          = 0;
   sa0Params.ICNT4         = M;
   sa0Params.DIM4          = strideIn0;

   /**********************************************************************/
   /* Prepare SA2 template to fetch A matrix                             */
   /**********************************************************************/

   sa2Params           = __gen_SA_TEMPLATE_v1();
   sa2Params.VECLEN    = SA_VECLEN;
   sa2Params.DIMFMT    = __SA_DIMFMT_5D;
   sa2Params.DECDIM1   = __SA_DECDIM_DIM2;
   sa2Params.DECDIM1SD = __SA_DECDIMSD_DIM1;

   sa2Params.ICNT0         = 1;
   sa2Params.ICNT1         = seUnrollFactor;
   sa2Params.DIM1          = 2;
   sa2Params.ICNT2         = KBlocks;
   sa2Params.DIM2          = (int32_t) ((uint32_t) seUnrollFactor << (uint32_t) 1);
   sa2Params.DECDIM1_WIDTH = (uint32_t) (K % 2 == 0 ? K : K - 1);
   sa2Params.ICNT3         = NBlocks;
   sa2Params.DIM3          = 0;
   sa2Params.ICNT4         = M;
   sa2Params.DIM4          = strideIn0;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch B matrix                       */
   /**********************************************************************/

   se0Params           = __gen_SE_TEMPLATE_v1();
   se0Params.ELETYPE   = SE_ELETYPE;
   se0Params.VECLEN    = SE_VECLEN;
   se0Params.DIMFMT    = __SE_DIMFMT_5D;
   se0Params.DECDIM1   = __SE_DECDIM_DIM2;
   se0Params.DECDIM2   = __SE_DECDIM_DIM3;
   se0Params.DECDIM1SD = __SE_DECDIMSD_DIM1;
   se0Params.DECDIM2SD = __SE_DECDIMSD_DIM0;

   se0Params.ICNT0         = elementCount;
   se0Params.ICNT1         = seUnrollFactor;
   se0Params.DIM1          = (int32_t) ((uint32_t) strideIn1 << (uint32_t) 1);
   se0Params.ICNT2         = KBlocks;
   se0Params.DIM2          = seUnrollFactor * (int32_t) ((uint32_t) strideIn1 << (uint32_t) 1);
   se0Params.DECDIM1_WIDTH = (uint32_t) K * strideIn1;
   se0Params.ICNT3         = NBlocks;
   se0Params.DIM3          = elementCount;
   se0Params.DECDIM2_WIDTH = (uint32_t) N;
   se0Params.ICNT4         = M;
   se0Params.DIM4          = 0;

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch B matrix                       */
   /**********************************************************************/

   se1Params           = __gen_SE_TEMPLATE_v1();
   se1Params.ELETYPE   = SE_ELETYPE;
   se1Params.VECLEN    = SE_VECLEN;
   se1Params.DIMFMT    = __SE_DIMFMT_5D;
   se1Params.DECDIM1   = __SE_DECDIM_DIM2;
   se1Params.DECDIM2   = __SE_DECDIM_DIM3;
   se1Params.DECDIM1SD = __SE_DECDIMSD_DIM1;
   se1Params.DECDIM2SD = __SE_DECDIMSD_DIM0;

   se1Params.ICNT0         = elementCount;
   se1Params.ICNT1         = seUnrollFactor;
   se1Params.DIM1          = (int32_t) ((uint32_t) strideIn1 << (uint32_t) 1);
   se1Params.ICNT2         = KBlocks;
   se1Params.DIM2          = seUnrollFactor * (int32_t) ((uint32_t) strideIn1 << (uint32_t) 1);
   se1Params.DECDIM1_WIDTH = (uint32_t) (K % 2 == 0 ? K : K - 1) * strideIn1;
   se1Params.ICNT3         = NBlocks;
   se1Params.DIM3          = elementCount;
   se1Params.DECDIM2_WIDTH = (uint32_t) N;
   se1Params.ICNT4         = M;
   se1Params.DIM4          = 0;

   /**********************************************************************/
   /* Prepare SA template to write C matrix                              */
   /**********************************************************************/

   sa1Params           = __gen_SA_TEMPLATE_v1();
   sa1Params.VECLEN    = SA_VECLEN;
   sa1Params.DIMFMT    = __SA_DIMFMT_3D;
   sa1Params.DECDIM1   = __SA_DECDIM_DIM1;
   sa1Params.DECDIM1SD = __SA_DECDIMSD_DIM0;

   sa1Params.ICNT0         = elementCount;
   sa1Params.ICNT1         = NBlocks;
   sa1Params.DIM1          = elementCount;
   sa1Params.DECDIM1_WIDTH = N;
   sa1Params.ICNT2         = M;
   sa1Params.DIM2          = strideOut;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_PARAM_OFFSET) = sa1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA2_PARAM_OFFSET) = sa2Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_matMul_generic_init_ci<float>(DSPLIB_kernelHandle           handle,
                                                            const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                                            const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                                            const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                            const DSPLIB_matMul_InitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_matMul_generic_init_ci<double>(DSPLIB_kernelHandle           handle,
                                                             const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                                             const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                                             const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                             const DSPLIB_matMul_InitArgs *pKerInitArgs);

template <typename T, typename vec, uint32_t id> static inline vec loadAMatSA(void *pIn)
{

   vec out;

   __vpred tmp = c7x::strm_agen<id, T>::get_vpred();
   out         = __vload_pred_dup(tmp, (c7x::strm_agen<id, T>::get_adv(pIn)));
   /* printf("\nOut vector below:\n"); */
   DSPLIB_debugPrintVector(out);

   return out;
}

template <typename T, typename vec> static inline void writeOutSA1(__vpred tmp, vec *addr, T pOut, vec out)
{
   /* printf("\nOut vector below:\n"); */
   /* DSPLIB_debugPrintVector(out); */
   tmp  = c7x::strm_agen<1, vec>::get_vpred();
   addr = c7x::strm_agen<1, vec>::get_adv(pOut);
   __vstore_pred(tmp, addr, out);
   return;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_generic_core_ci(DSPLIB_kernelHandle handle,
                                            void *restrict pIn0,
                                            void *restrict pIn1,
                                            void *restrict pOut);

template <>
inline DSPLIB_STATUS DSPLIB_matMul_generic_core_ci<double>(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn0,
                                                           void *restrict pIn1,
                                                           void *restrict pOut)
{
   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   int32_t M       = pKerPrivArgs->M;
   int32_t K       = pKerPrivArgs->K;
   int32_t KBlocks = pKerPrivArgs->KBlocks;
   int32_t NBlocks = pKerPrivArgs->NBlocks;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_matMul_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<double>::type vec;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   if (K > 1) {
      int32_t strideIn1 = pKerPrivArgs->strideIn1Elements;

      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);

      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_PARAM_OFFSET);
      sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA2_PARAM_OFFSET);

      // Input samples
      __SE0_OPEN(pIn1, se0Params);
      __SE1_OPEN(((double *) pIn1 + strideIn1), se1Params);

      // Output samples
      __SA0_OPEN(sa0Params);
      __SA2_OPEN(sa2Params);
      __SA1_OPEN(sa1Params);

      vec a;
      vec b;

      vec r00, r01, r03, r02, r04, r05, r06, r07;

      vec c0 = vec(0);
      vec c1 = vec(0);
      vec c2 = vec(0);
      vec c3 = vec(0);

      __vpred tmp;
      vec    *addr;

      DSPLIB_DEBUGPRINTFN(1, "\nIn _ci.cpp M = %d, NBlocks = %d, KBlocks = %d: \n", M, NBlocks, KBlocks);

      vec a00, a01, a02, a03;

      double *pIn0Local = ((double *) pIn0 + 1);

      a00 = loadAMatSA<double, vec, 2>(pIn0Local);
      a01 = loadAMatSA<double, vec, 2>(pIn0Local);
      a02 = loadAMatSA<double, vec, 2>(pIn0Local);
      a03 = loadAMatSA<double, vec, 2>(pIn0Local);

      /* uint64_t startCycle, endCycle; */

      for (int32_t mn = 0; mn < M * NBlocks; mn++) {

         r00 = (vec) 0;
         r01 = (vec) 0;
         r02 = (vec) 0;
         r03 = (vec) 0;
         r04 = (vec) 0;
         r05 = (vec) 0;
         r06 = (vec) 0;
         r07 = (vec) 0;

         for (int32_t k = 0; k < KBlocks; k++) {

            a = loadAMatSA<double, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r00 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r01 += a00 * b;

            a = loadAMatSA<double, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r02 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r03 += a01 * b;

            a = loadAMatSA<double, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r04 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r05 += a02 * b;

            a = loadAMatSA<double, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r06 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r07 += a03 * b;

            a00 = loadAMatSA<double, vec, 2>(pIn0Local);
            a01 = loadAMatSA<double, vec, 2>(pIn0Local);
            a02 = loadAMatSA<double, vec, 2>(pIn0Local);
            a03 = loadAMatSA<double, vec, 2>(pIn0Local);
         }

         c0 = r00 + r01;
         c1 = r02 + r03;
         c2 = r04 + r05;
         c3 = r06 + r07;

         c0 += c1;
         c2 += c3;
         c0 += c2;

         writeOutSA1(tmp, addr, pOut, c0);
      }

      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();
   }
   else {
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock);

      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));

      // Input samples
      __SE0_OPEN(pIn1, se0Params);
      __SA0_OPEN(sa0Params);

      // Output samples
      __SA1_OPEN(sa1Params);

      vec a;
      vec b;

      vec r00, r02, r04, r06;

      vec c0 = vec(0);
      vec c1 = vec(0);
      vec c2 = vec(0);
      vec c3 = vec(0);

      __vpred tmp;
      vec    *addr;

      DSPLIB_DEBUGPRINTFN(1, "\nIn _ci.cpp M = %d, NBlocks = %d, KBlocks = %d: \n", M, NBlocks, KBlocks);

      for (int32_t mn = 0; mn < M * NBlocks; mn++) {

         r00 = (vec) 0;
         r02 = (vec) 0;
         r04 = (vec) 0;
         r06 = (vec) 0;

         for (int32_t k = 0; k < KBlocks; k++) {

            a = loadAMatSA<double, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r00 += a * b;

            a = loadAMatSA<double, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r02 += a * b;

            a = loadAMatSA<double, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r04 += a * b;

            a = loadAMatSA<double, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r06 += a * b;
         }

         c0 = r00;
         c1 = r02;
         c2 = r04;
         c3 = r06;

         c0 += c1;
         c2 += c3;

         c0 += c2;

         writeOutSA1(tmp, addr, pOut, c0);
      }

      __SE0_CLOSE();
      __SA0_CLOSE();
      __SA1_CLOSE();
   }
   return DSPLIB_SUCCESS;
}
template <>
inline DSPLIB_STATUS DSPLIB_matMul_generic_core_ci<float>(DSPLIB_kernelHandle handle,
                                                          void *restrict pIn0,
                                                          void *restrict pIn1,
                                                          void *restrict pOut)
{
   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   int32_t M       = pKerPrivArgs->M;
   int32_t K       = pKerPrivArgs->K;
   int32_t KBlocks = pKerPrivArgs->KBlocks;
   int32_t NBlocks = pKerPrivArgs->NBlocks;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_matMul_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<float>::type vec;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   if (K > 1) {
      int32_t strideIn1 = pKerPrivArgs->strideIn1Elements;

      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);

      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_PARAM_OFFSET);
      sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA2_PARAM_OFFSET);

      // Input samples
      __SE0_OPEN(pIn1, se0Params);
      __SE1_OPEN(((float *) pIn1 + strideIn1), se1Params);

      // Output samples
      __SA0_OPEN(sa0Params);
      __SA2_OPEN(sa2Params);
      __SA1_OPEN(sa1Params);

      vec a;
      vec b;

      vec r00, r01, r03, r02, r04, r05, r06, r07;
#if !defined(__C7504__) && !defined(__C7524__)
      vec r08, r09, r0a, r0b, r0c, r0d, r0e, r0f;
#endif

      vec c0 = vec(0);
      vec c1 = vec(0);
      vec c2 = vec(0);
      vec c3 = vec(0);
#if !defined(__C7504__) && !defined(__C7524__)
      vec c4, c5, c6, c7;
#endif

      __vpred tmp;
      vec    *addr;

      DSPLIB_DEBUGPRINTFN(1, "\nIn _ci.cpp M = %d, NBlocks = %d, KBlocks = %d: \n", M, NBlocks, KBlocks);

      vec a00, a01, a02, a03;
#if !defined(__C7504__) && !defined(__C7524__)
      vec a04, a05, a06, a07;
#endif

      float *pIn0Local = ((float *) pIn0 + 1);

      a00 = loadAMatSA<float, vec, 2>(pIn0Local);
      a01 = loadAMatSA<float, vec, 2>(pIn0Local);
      a02 = loadAMatSA<float, vec, 2>(pIn0Local);
      a03 = loadAMatSA<float, vec, 2>(pIn0Local);
#if !defined(__C7504__) && !defined(__C7524__)
      a04 = loadAMatSA<float, vec, 2>(pIn0Local);
      a05 = loadAMatSA<float, vec, 2>(pIn0Local);
      a06 = loadAMatSA<float, vec, 2>(pIn0Local);
      a07 = loadAMatSA<float, vec, 2>(pIn0Local);
#endif
      /* #pragma COALESCE_LOOP */
      /* #pragma MUST_ITERATE(2, , 2) */
      /* for (int32_t m = 0; m < M; m++) { */
      /* #pragma COALESCE_LOOP */
      /* #pragma MUST_ITERATE(2, , 2) */
      /* uint64_t startCycle, endCycle; */

      /* #pragma COALESCE_LOOP */
      /* #pragma MUST_ITERATE(2, , 2) */
      for (int32_t mn = 0; mn < M * NBlocks; mn++) {
         /* for (int32_t n = 0; n < 2; n++) { */

         r00 = (vec) 0;
         r01 = (vec) 0;
         r02 = (vec) 0;
         r03 = (vec) 0;
         r04 = (vec) 0;
         r05 = (vec) 0;
         r06 = (vec) 0;
         r07 = (vec) 0;
#if !defined(__C7504__) && !defined(__C7524__)
         r08 = (vec) 0;
         r09 = (vec) 0;
         r0a = (vec) 0;
         r0b = (vec) 0;
         r0c = (vec) 0;
         r0d = (vec) 0;
         r0e = (vec) 0;
         r0f = (vec) 0;
#endif
         /* startCycle = (__get_GTSC(1), __STSC); */
         /* #pragma MUST_ITERATE(2, , 2) */
         for (int32_t k = 0; k < KBlocks; k++) {

            a = loadAMatSA<float, vec, 0>(pIn0);
            /* printf("\na: \n"); */
            /* DSPLIB_debugPrintVector(a); */
            b = c7x::strm_eng<0, vec>::get_adv();
            /* printf("\nb: \n"); */
            /* DSPLIB_debugPrintVector(b); */
            r00 += a * b;

            /* printf("\na00: \n"); */
            /* DSPLIB_debugPrintVector(a00); */
            b = c7x::strm_eng<1, vec>::get_adv();
            /* printf("\nb: \n"); */
            /* DSPLIB_debugPrintVector(b); */
            r01 += a00 * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            /* printf("\na: \n"); */
            /* DSPLIB_debugPrintVector(a); */
            b = c7x::strm_eng<0, vec>::get_adv();
            /* printf("\nb: \n"); */
            /* DSPLIB_debugPrintVector(b); */
            r02 += a * b;

            /* printf("\na01: \n"); */
            /* DSPLIB_debugPrintVector(a01); */
            b = c7x::strm_eng<1, vec>::get_adv();
            /* printf("\nb: \n"); */
            /* DSPLIB_debugPrintVector(b); */
            r03 += a01 * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r04 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r05 += a02 * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r06 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r07 += a03 * b;

#if !defined(__C7504__) && !defined(__C7524__)
            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r08 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r09 += a04 * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r0a += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r0b += a05 * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r0c += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r0d += a06 * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r0e += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r0f += a07 * b;
#endif

            a00 = loadAMatSA<float, vec, 2>(pIn0Local);
            a01 = loadAMatSA<float, vec, 2>(pIn0Local);
            a02 = loadAMatSA<float, vec, 2>(pIn0Local);
            a03 = loadAMatSA<float, vec, 2>(pIn0Local);

#if !defined(__C7504__) && !defined(__C7524__)
            a04 = loadAMatSA<float, vec, 2>(pIn0Local);
            a05 = loadAMatSA<float, vec, 2>(pIn0Local);
            a06 = loadAMatSA<float, vec, 2>(pIn0Local);
            a07 = loadAMatSA<float, vec, 2>(pIn0Local);
#endif
         }

         /* startCycle = (__get_GTSC(1), __STSC); */
         /* c = r00 + r01 + r02 + r03 + r04 + r05 + r06 + r07 + r08 + r09 + r0a + r0b + r0c + r0d + r0e + r0f; */

         c0 = r00 + r01;
         c1 = r02 + r03;
         c2 = r04 + r05;
         c3 = r06 + r07;
#if !defined(__C7504__) && !defined(__C7524__)
         c4 = r08 + r09;
         c5 = r0a + r0b;
         c6 = r0c + r0d;
         c7 = r0e + r0f;
#endif

         c0 += c1;
         c2 += c3;
#if !defined(__C7504__) && !defined(__C7524__)
         c4 += c5;
         c6 += c7;
#endif

         c0 += c2;
#if !defined(__C7504__) && !defined(__C7524__)
         c4 += c6;
         c0 += c4;
#endif

         /* endCycle   = (__get_GTSC(1), __STSC); */
         writeOutSA1(tmp, addr, pOut, c0);
      }
      /* } */

      /* printf("\nTotaly cycles: %ld\n", endCycle - startCycle); */

      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();
   }
   else {
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock);

      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));

      // Input samples
      __SE0_OPEN(pIn1, se0Params);
      __SA0_OPEN(sa0Params);

      // Output samples
      __SA1_OPEN(sa1Params);

      vec a;
      vec b;

      vec r00, r02, r04, r06;
#if !defined(__C7504__) && !defined(__C7524__)
      vec r08, r0a, r0c, r0e;
#endif

      vec c0 = vec(0);
      vec c1 = vec(0);
      vec c2 = vec(0);
      vec c3 = vec(0);
#if !defined(__C7504__) && !defined(__C7524__)
      vec c4, c5, c6, c7;
#endif

      __vpred tmp;
      vec    *addr;

      DSPLIB_DEBUGPRINTFN(1, "\nIn _ci.cpp M = %d, NBlocks = %d, KBlocks = %d: \n", M, NBlocks, KBlocks);

      for (int32_t mn = 0; mn < M * NBlocks; mn++) {

         r00 = (vec) 0;
         r02 = (vec) 0;
         r04 = (vec) 0;
         r06 = (vec) 0;
#if !defined(__C7504__) && !defined(__C7524__)
         r08 = (vec) 0;
         r0a = (vec) 0;
         r0c = (vec) 0;
         r0e = (vec) 0;
#endif

         for (int32_t k = 0; k < KBlocks; k++) {

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r00 += a * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r02 += a * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r04 += a * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r06 += a * b;

#if !defined(__C7504__) && !defined(__C7524__)
            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r08 += a * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r0a += a * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r0c += a * b;

            a = loadAMatSA<float, vec, 0>(pIn0);
            b = c7x::strm_eng<0, vec>::get_adv();
            r0e += a * b;

#endif
         }

         c0 = r00;
         c1 = r02;
         c2 = r04;
         c3 = r06;
#if !defined(__C7504__) && !defined(__C7524__)
         c4 = r08;
         c5 = r0a;
         c6 = r0c;
         c7 = r0e;
#endif

         c0 += c1;
         c2 += c3;
#if !defined(__C7504__) && !defined(__C7524__)
         c4 += c5;
         c6 += c7;
#endif

         c0 += c2;
#if !defined(__C7504__) && !defined(__C7524__)
         c4 += c6;
         c0 += c4;
#endif

         writeOutSA1(tmp, addr, pOut, c0);
      }

      __SE0_CLOSE();
      __SA0_CLOSE();
      __SA1_CLOSE();
   }

   return DSPLIB_SUCCESS;
}

template <typename dataType>
DSPLIB_STATUS
DSPLIB_matMul_generic_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_matMul_generic_core_ci<dataType>(handle, pIn0, pIn1, pOut);
   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_matMul_generic_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn0,
                                                            void *restrict pIn1,
                                                            void *restrict pOut);
template DSPLIB_STATUS DSPLIB_matMul_generic_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                             void *restrict pIn0,
                                                             void *restrict pIn1,
                                                             void *restrict pOut);
