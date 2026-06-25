// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_matMul_fixed_priv.h"

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#if !defined(__C7504__) && !defined(__C7524__)
#define DSPLIB_MATMUL_FIXED_UNROLL (32)
#else
#define DSPLIB_MATMUL_FIXED_UNROLL (128)
#endif

/***********************************
 *   Initialization
 ***********************************/
#if defined(__C7504__) || defined(__C7524__)
template <typename dataTypeIn, typename dataTypeOut>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci(__SE_TEMPLATE_v1 *se0Params, __SE_TEMPLATE_v1 *se1Params);
template <>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(__SE_TEMPLATE_v1 *se0Params,
                                                                                       __SE_TEMPLATE_v1 *se1Params)
{
   se0Params->PROMOTE   = __SE_PROMOTE_OFF;
   se1Params->TRANSPOSE = __SE_TRANSPOSE_64BIT;
}
template <>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(__SE_TEMPLATE_v1 *se0Params,
                                                                                     __SE_TEMPLATE_v1 *se1Params)
{
   se0Params->PROMOTE   = __SE_PROMOTE_2X_SIGNEXT;
   se1Params->TRANSPOSE = __SE_TRANSPOSE_32BIT;
   se1Params->PROMOTE   = __SE_PROMOTE_2X_SIGNEXT;
}

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_matMul_fixed_init_ci(DSPLIB_kernelHandle                 handle,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                          const DSPLIB_bufParams2D_t         *bufParamsOut,
                                          const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SE_ELETYPE     SE_ELETYPE = c7x::se_eletype<dataTypeOut>::value;
   __SE_VECLEN      SE_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
   ;
   __SA_VECLEN                   SA_VECLEN    = c7x::sa_veclen<dataTypeOut>::value;
   int32_t                       elementCount = c7x::element_count_of<dataTypeOut>::value;
   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;
   uint8_t                      *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                       M            = pKerPrivArgs->M;
   int32_t                       K            = pKerPrivArgs->K;
   int32_t                       N            = pKerPrivArgs->N;
   int32_t                       strideIn0    = pKerPrivArgs->strideIn0Elements;
   int32_t                       strideIn1    = pKerPrivArgs->strideIn1Elements;
   int32_t                       strideOut    = pKerPrivArgs->strideOutElements;
   int32_t                       unrollFactor = 1;
   if (N >= 32) {
      unrollFactor = 2;
   }

   int32_t KBlocks = ((K + 4 - 1)) / (4);
   int32_t MBlocks = ((M + 2 - 1)) / (2);
   int32_t NBlocks = ((N + (elementCount * unrollFactor) - 1)) / (elementCount * unrollFactor);
   ;

   pKerPrivArgs->KBlocks = KBlocks;
   pKerPrivArgs->NBlocks = NBlocks;
   pKerPrivArgs->MBlocks = MBlocks;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch A matrix                       */
   /**********************************************************************/
   se0Params           = __gen_SE_TEMPLATE_v1();
   se0Params.ELETYPE   = SE_ELETYPE;
   se0Params.DIMFMT    = __SE_DIMFMT_5D;
   se0Params.DECDIM1   = __SE_DECDIM_DIM2;
   se0Params.DECDIM2   = __SE_DECDIM_DIM4;
   se0Params.DECDIM1SD = __SE_DECDIMSD_DIM0;
   se0Params.DECDIM2SD = __SE_DECDIMSD_DIM1;

   se0Params.VECLEN        = __SE_VECLEN_4ELEMS;
   se0Params.GRPDUP        = __SE_GRPDUP_ON;
   se0Params.ICNT0         = 4;
   se0Params.ICNT1         = 2;
   se0Params.DIM1          = strideIn0;
   se0Params.ICNT2         = KBlocks;
   se0Params.DIM2          = 4;
   se0Params.ICNT3         = NBlocks;
   se0Params.DIM3          = 0;
   se0Params.DECDIM2_WIDTH = (uint32_t) M * strideIn0;
   se0Params.ICNT4         = MBlocks;
   se0Params.DIM4          = 2 * strideIn0;
   se0Params.DECDIM1_WIDTH = (uint32_t) K;

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch B matrix                       */
   /**********************************************************************/
   se1Params         = __gen_SE_TEMPLATE_v1();
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN  = SE_VECLEN;
   se1Params.DIMFMT  = __SE_DIMFMT_5D;

   se1Params.ICNT0 = 4 * 4 * unrollFactor;
   se1Params.ICNT1 = 4;
   se1Params.DIM1  = strideIn1;
   se1Params.DIM2  = 4 * strideIn1;
   se1Params.ICNT2 = KBlocks;
   se1Params.DIM3  = 4 * 4 * unrollFactor;
   se1Params.ICNT3 = NBlocks;
   se1Params.DIM4  = 0;
   se1Params.ICNT4 = MBlocks;

   DSPLIB_matMul_fixed_PromoteTranspose_init_ci<dataTypeIn, dataTypeOut>(&se0Params, &se1Params);
   /**********************************************************************/
   /* Prepare SA template to write C matrix                              */
   /**********************************************************************/
   sa0Params           = __gen_SA_TEMPLATE_v1();
   sa0Params.VECLEN    = SA_VECLEN;
   sa0Params.DIMFMT    = __SA_DIMFMT_4D;
   sa0Params.DECDIM1   = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1SD = __SA_DECDIMSD_DIM0;
   sa0Params.DECDIM2   = __SA_DECDIM_DIM3;
   sa0Params.DECDIM2SD = __SA_DECDIMSD_DIM1;

   sa0Params.ICNT0         = elementCount * unrollFactor;
   sa0Params.ICNT1         = 2;
   sa0Params.ICNT2         = NBlocks;
   sa0Params.DIM1          = strideOut;
   sa0Params.DECDIM1_WIDTH = N;
   sa0Params.DECDIM2_WIDTH = M * strideOut;
   sa0Params.DIM2          = elementCount * unrollFactor;
   sa0Params.ICNT3         = MBlocks;
   sa0Params.DIM3          = 2 * strideOut;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

template DSPLIB_STATUS
DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(DSPLIB_kernelHandle                 handle,
                                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                          const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                          const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                          const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(DSPLIB_kernelHandle                 handle,
                                                        const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                        const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                        const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                        const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);
#else
template <typename dataTypeIn, typename dataTypeOut>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci(__SE_TEMPLATE_v1 *se0Params, __SE_TEMPLATE_v1 *se1Params);
template <>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(__SE_TEMPLATE_v1 *se0Params,
                                                                                       __SE_TEMPLATE_v1 *se1Params)
{
   typedef typename c7x::make_full_vector<int16_t>::type vec;
   __SE_VECLEN                                           SE_VECLEN = c7x::se_veclen<vec>::value;
   se0Params->VECLEN                                               = SE_VECLEN;
   se0Params->TRANSPOSE                                            = __SE_TRANSPOSE_128BIT;
   se1Params->VECLEN                                               = SE_VECLEN;
   se1Params->TRANSPOSE                                            = __SE_TRANSPOSE_64BIT;
}
template <>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(__SE_TEMPLATE_v1 *se0Params,
                                                                                     __SE_TEMPLATE_v1 *se1Params)
{
   se0Params->VECLEN    = __SE_VECLEN_32ELEMS;
   se0Params->PROMOTE   = __SE_PROMOTE_2X_SIGNEXT;
   se0Params->TRANSPOSE = __SE_TRANSPOSE_64BIT;
   se1Params->VECLEN    = __SE_VECLEN_32ELEMS;
   se1Params->PROMOTE   = __SE_PROMOTE_2X_SIGNEXT;
   se1Params->TRANSPOSE = __SE_TRANSPOSE_32BIT;
}
template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_matMul_fixed_init_ci(DSPLIB_kernelHandle                 handle,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                          const DSPLIB_bufParams2D_t         *bufParamsOut,
                                          const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs)
{

   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   uint32_t M            = pKerPrivArgs->M;
   uint32_t N            = pKerPrivArgs->N;
   uint32_t K            = pKerPrivArgs->K;
   uint32_t MBlocks      = DSPLIB_ceilingDiv(M, 4);
   uint32_t NBlocks      = DSPLIB_ceilingDiv(N, 32);
   uint32_t KBlocks      = DSPLIB_ceilingDiv(K, 8);
   pKerPrivArgs->MBlocks = MBlocks;
   pKerPrivArgs->NBlocks = NBlocks;
   pKerPrivArgs->KBlocks = KBlocks;
   int32_t strideIn0     = pKerPrivArgs->strideIn0Elements;
   int32_t strideIn1     = pKerPrivArgs->strideIn1Elements;
   int32_t strideOut     = pKerPrivArgs->strideOutElements;

   __SE_TEMPLATE_v1                                         se0Params;
   __SE_TEMPLATE_v1                                         se1Params;
   __SA_TEMPLATE_v1                                         sa0Params;
   typedef typename c7x::make_full_vector<dataTypeIn>::type vec;
   __SE_ELETYPE                                             SE_ELETYPE = c7x::se_eletype<vec>::value;

   se0Params = __gen_SE_TEMPLATE_v1();

   se0Params.ICNT0   = K;
   se0Params.ICNT1   = (4u < M) ? 4 : M; // 4;
   se0Params.DIM1    = strideIn0;
   se0Params.ICNT2   = NBlocks;
   se0Params.DIM2    = 0;
   se0Params.ICNT3   = MBlocks;
   se0Params.DIM3    = strideIn0 * 4;
   se0Params.DIMFMT  = __SE_DIMFMT_4D;
   se0Params.ELETYPE = SE_ELETYPE;

   se1Params         = __gen_SE_TEMPLATE_v1();
   se1Params.ICNT0   = 32;
   se1Params.ICNT1   = (8u < K) ? 8 : K; // 8;
   se1Params.DIM1    = strideIn1;
   se1Params.ICNT2   = KBlocks;
   se1Params.DIM2    = strideIn1 * 8;
   se1Params.ICNT3   = NBlocks;
   se1Params.DIM3    = 32;
   se1Params.ICNT4   = MBlocks;
   se1Params.DIM4    = 0;
   se1Params.DIMFMT  = __SE_DIMFMT_5D;
   se1Params.ELETYPE = SE_ELETYPE;

   se1Params.DECDIM2       = __SE_DECDIM_DIM3;
   se1Params.DECDIM2_WIDTH = N;

   DSPLIB_matMul_fixed_PromoteTranspose_init_ci<dataTypeIn, dataTypeOut>(&se0Params, &se1Params);

   sa0Params        = __gen_SA_TEMPLATE_v1();
   sa0Params.ICNT0  = 32;
   sa0Params.ICNT1  = 4;
   sa0Params.DIM1   = strideOut;
   sa0Params.ICNT2  = NBlocks;
   sa0Params.DIM2   = 32;
   sa0Params.ICNT3  = MBlocks;
   sa0Params.DIM3   = 4 * strideOut;
   sa0Params.DIMFMT = __SA_DIMFMT_4D;
   sa0Params.VECLEN = __SA_VECLEN_32ELEMS;

   sa0Params.DECDIM1       = __SA_DECDIM_DIM3;
   sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM1;
   sa0Params.DECDIM1_WIDTH = M * strideOut;

   sa0Params.DECDIM2       = __SA_DECDIM_DIM2;
   sa0Params.DECDIM2SD     = __SA_DECDIMSD_DIM0;
   sa0Params.DECDIM2_WIDTH = N;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;
   return status;
}

template DSPLIB_STATUS
DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(DSPLIB_kernelHandle                 handle,
                                                        const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                        const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                        const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                        const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(DSPLIB_kernelHandle                 handle,
                                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                          const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                          const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                          const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

#endif
/***********************************
 *   Implementation
 ***********************************/
#if defined(__C7504__) || defined(__C7524__)
template <uint32_t id> static inline void loadMatSE(c7x::short_vec *a)
{
   *a = c7x::strm_eng<id, c7x::short_vec>::get_adv();
}
template <typename T, typename pVec, typename vecIn>
static inline void writeOutSA0(__vpred vPred, pVec *addr, T pOut, vecIn out1, vecIn out2)
{
   vPred = c7x::strm_agen<0, pVec>::get_vpred();
   addr  = c7x::strm_agen<0, pVec>::get_adv(pOut);
   __vstore_pred_packl_2src(vPred, addr, out1, out2);
   return;
}
template <typename T, typename vecIn>
static inline void writeOutSA0(__vpred vPred, c7x::char_hvec *addr, T pOut, vecIn out1, vecIn out2)
{
   vPred = c7x::strm_agen<0, c7x::char_hvec>::get_vpred();
   addr  = c7x::strm_agen<0, c7x::char_hvec>::get_adv(pOut);
   __vstore_pred_pack_byte_2src(vPred, addr, out1, out2);
   return;
}

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS
DSPLIB_matMul_fixed_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;
   int16_t                      *pOutLocal    = (int16_t *) pOut;
   int32_t                       KBlocks      = pKerPrivArgs->KBlocks;
   int32_t                       NBlocks      = pKerPrivArgs->NBlocks;
   int32_t                       MBlocks      = pKerPrivArgs->MBlocks;
   int32_t                       qs           = pKerPrivArgs->qs;
   uint8_t                      *pBlock       = pKerPrivArgs->bufPblock;
   __SE_TEMPLATE_v1              se0Params    = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1              se1Params    = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1              sa0Params    = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   __vpred                       vPred;
   dataTypeOut                  *addr;
   uchar32 vMask = uchar32(0, 1, 8, 9, 16, 17, 24, 25, 2, 3, 10, 11, 18, 19, 26, 27, 4, 5, 12, 13, 20, 21, 28, 29, 6, 7,
                           14, 15, 22, 23, 30, 31);

   __SE0_OPEN(pIn0, se0Params);
   __SE1_OPEN((pIn1), se1Params);
   __SA0_OPEN(sa0Params);

   if (pKerPrivArgs->N >= 32) {
      for (int32_t mn = 0; mn < MBlocks * NBlocks; mn++) {
         c7x::long_vec  c00 = (c7x::long_vec) 0;
         c7x::long_vec  c01 = (c7x::long_vec) 0;
         c7x::long_vec  c02 = (c7x::long_vec) 0;
         c7x::long_vec  c03 = (c7x::long_vec) 0;
         c7x::long_vec  c04 = (c7x::long_vec) 0;
         c7x::long_vec  c05 = (c7x::long_vec) 0;
         c7x::long_vec  c06 = (c7x::long_vec) 0;
         c7x::long_vec  c07 = (c7x::long_vec) 0;
         c7x::long_vec  c10 = (c7x::long_vec) 0;
         c7x::long_vec  c11 = (c7x::long_vec) 0;
         c7x::long_vec  c12 = (c7x::long_vec) 0;
         c7x::long_vec  c13 = (c7x::long_vec) 0;
         c7x::long_vec  c14 = (c7x::long_vec) 0;
         c7x::long_vec  c15 = (c7x::long_vec) 0;
         c7x::long_vec  c16 = (c7x::long_vec) 0;
         c7x::long_vec  c17 = (c7x::long_vec) 0;
         c7x::short_vec b0, b1, b2, b3, b4, b5, b6, b7;
         c7x::short_vec a0, a1;
         for (int32_t k = 0; k < KBlocks; k++) {
            loadMatSE<0>(&a0);
            loadMatSE<0>(&a1);

            loadMatSE<1>(&b0);
            loadMatSE<1>(&b1);
            loadMatSE<1>(&b2);
            loadMatSE<1>(&b3);

            loadMatSE<1>(&b4);
            loadMatSE<1>(&b5);
            loadMatSE<1>(&b6);
            loadMatSE<1>(&b7);

            b0 = __as_short16(__permute(vMask, __as_uchar32(b0)));
            b1 = __as_short16(__permute(vMask, __as_uchar32(b1)));
            b2 = __as_short16(__permute(vMask, __as_uchar32(b2)));
            b3 = __as_short16(__permute(vMask, __as_uchar32(b3)));

            b4 = __as_short16(__permute(vMask, __as_uchar32(b4)));
            b5 = __as_short16(__permute(vMask, __as_uchar32(b5)));
            b6 = __as_short16(__permute(vMask, __as_uchar32(b6)));
            b7 = __as_short16(__permute(vMask, __as_uchar32(b7)));

            c00 += __dotp4_ext(a0, b0);
            c01 += __dotp4_ext(a0, b1);
            c02 += __dotp4_ext(a0, b2);
            c03 += __dotp4_ext(a0, b3);

            c04 += __dotp4_ext(a0, b4);
            c05 += __dotp4_ext(a0, b5);
            c06 += __dotp4_ext(a0, b6);
            c07 += __dotp4_ext(a0, b7);

            c10 += __dotp4_ext(a1, b0);
            c11 += __dotp4_ext(a1, b1);
            c12 += __dotp4_ext(a1, b2);
            c13 += __dotp4_ext(a1, b3);

            c14 += __dotp4_ext(a1, b4);
            c15 += __dotp4_ext(a1, b5);
            c16 += __dotp4_ext(a1, b6);
            c17 += __dotp4_ext(a1, b7);
         }
         c7x::long_vec c00shfited = __shift_right(c00, (c7x::long_vec) qs);
         c7x::long_vec c01shfited = __shift_right(c01, (c7x::long_vec) qs);
         c7x::long_vec c02shfited = __shift_right(c02, (c7x::long_vec) qs);
         c7x::long_vec c03shfited = __shift_right(c03, (c7x::long_vec) qs);
         c7x::long_vec c04shfited = __shift_right(c04, (c7x::long_vec) qs);
         c7x::long_vec c05shfited = __shift_right(c05, (c7x::long_vec) qs);
         c7x::long_vec c06shfited = __shift_right(c06, (c7x::long_vec) qs);
         c7x::long_vec c07shfited = __shift_right(c07, (c7x::long_vec) qs);
         c7x::long_vec c10shfited = __shift_right(c10, (c7x::long_vec) qs);
         c7x::long_vec c11shfited = __shift_right(c11, (c7x::long_vec) qs);
         c7x::long_vec c12shfited = __shift_right(c12, (c7x::long_vec) qs);
         c7x::long_vec c13shfited = __shift_right(c13, (c7x::long_vec) qs);
         c7x::long_vec c14shfited = __shift_right(c14, (c7x::long_vec) qs);
         c7x::long_vec c15shfited = __shift_right(c15, (c7x::long_vec) qs);
         c7x::long_vec c16shfited = __shift_right(c16, (c7x::long_vec) qs);
         c7x::long_vec c17shfited = __shift_right(c17, (c7x::long_vec) qs);

         c7x::int_vec out01 = __vwpackl_vvv(c7x::as_int_vec(c01shfited), c7x::as_int_vec(c00shfited));
         c7x::int_vec out02 = __vwpackl_vvv(c7x::as_int_vec(c03shfited), c7x::as_int_vec(c02shfited));
         c7x::int_vec out03 = __vwpackl_vvv(c7x::as_int_vec(c05shfited), c7x::as_int_vec(c04shfited));
         c7x::int_vec out04 = __vwpackl_vvv(c7x::as_int_vec(c07shfited), c7x::as_int_vec(c06shfited));
         c7x::int_vec out11 = __vwpackl_vvv(c7x::as_int_vec(c11shfited), c7x::as_int_vec(c10shfited));
         c7x::int_vec out12 = __vwpackl_vvv(c7x::as_int_vec(c13shfited), c7x::as_int_vec(c12shfited));
         c7x::int_vec out13 = __vwpackl_vvv(c7x::as_int_vec(c15shfited), c7x::as_int_vec(c14shfited));
         c7x::int_vec out14 = __vwpackl_vvv(c7x::as_int_vec(c17shfited), c7x::as_int_vec(c16shfited));

         writeOutSA0(vPred, addr, pOutLocal, out01, out02);
         writeOutSA0(vPred, addr, pOutLocal, out03, out04);
         writeOutSA0(vPred, addr, pOutLocal, out11, out12);
         writeOutSA0(vPred, addr, pOutLocal, out13, out14);
      }
   }
   else {
      for (int32_t mn = 0; mn < MBlocks * NBlocks; mn++) {
         c7x::long_vec  c00 = (c7x::long_vec) 0;
         c7x::long_vec  c01 = (c7x::long_vec) 0;
         c7x::long_vec  c02 = (c7x::long_vec) 0;
         c7x::long_vec  c03 = (c7x::long_vec) 0;
         c7x::long_vec  c10 = (c7x::long_vec) 0;
         c7x::long_vec  c11 = (c7x::long_vec) 0;
         c7x::long_vec  c12 = (c7x::long_vec) 0;
         c7x::long_vec  c13 = (c7x::long_vec) 0;
         c7x::short_vec b0, b1, b2, b3;
         c7x::short_vec a0, a1;
         for (int32_t k = 0; k < KBlocks; k++) {
            loadMatSE<0>(&a0);
            loadMatSE<0>(&a1);

            loadMatSE<1>(&b0);
            loadMatSE<1>(&b1);
            loadMatSE<1>(&b2);
            loadMatSE<1>(&b3);

            b0 = __as_short16(__permute(vMask, __as_uchar32(b0)));
            b1 = __as_short16(__permute(vMask, __as_uchar32(b1)));
            b2 = __as_short16(__permute(vMask, __as_uchar32(b2)));
            b3 = __as_short16(__permute(vMask, __as_uchar32(b3)));

            c00 += __dotp4_ext(a0, b0);
            c01 += __dotp4_ext(a0, b1);
            c02 += __dotp4_ext(a0, b2);
            c03 += __dotp4_ext(a0, b3);

            c10 += __dotp4_ext(a1, b0);
            c11 += __dotp4_ext(a1, b1);
            c12 += __dotp4_ext(a1, b2);
            c13 += __dotp4_ext(a1, b3);
         }
         c7x::long_vec c00shfited = __shift_right(c00, (c7x::long_vec) qs);
         c7x::long_vec c01shfited = __shift_right(c01, (c7x::long_vec) qs);
         c7x::long_vec c02shfited = __shift_right(c02, (c7x::long_vec) qs);
         c7x::long_vec c03shfited = __shift_right(c03, (c7x::long_vec) qs);

         c7x::int_vec out01 = __vwpackl_vvv(c7x::as_int_vec(c01shfited), c7x::as_int_vec(c00shfited));
         c7x::int_vec out02 = __vwpackl_vvv(c7x::as_int_vec(c03shfited), c7x::as_int_vec(c02shfited));

         c7x::long_vec c10shfited = __shift_right(c10, (c7x::long_vec) qs);
         c7x::long_vec c11shfited = __shift_right(c11, (c7x::long_vec) qs);
         c7x::long_vec c12shfited = __shift_right(c12, (c7x::long_vec) qs);
         c7x::long_vec c13shfited = __shift_right(c13, (c7x::long_vec) qs);

         c7x::int_vec out11 = __vwpackl_vvv(c7x::as_int_vec(c11shfited), c7x::as_int_vec(c10shfited));
         c7x::int_vec out12 = __vwpackl_vvv(c7x::as_int_vec(c13shfited), c7x::as_int_vec(c12shfited));

         writeOutSA0(vPred, addr, pOutLocal, out01, out02);
         writeOutSA0(vPred, addr, pOutLocal, out11, out12);
      }
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}
template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);
#else

template <typename V> static inline c7x::uchar_vec setMask();
template <> inline c7x::uchar_vec                  setMask<int16_t>()
{
   uint8_t        mask[64] = {0,  1,  2,  3,  8,  9,  10, 11, 4,  5,  6,  7,  12, 13, 14, 15, 32, 33, 34, 35, 40, 41,
                              42, 43, 36, 37, 38, 39, 44, 45, 46, 47, 16, 17, 18, 19, 24, 25, 26, 27, 20, 21, 22, 23,
                              28, 29, 30, 31, 48, 49, 50, 51, 56, 57, 58, 59, 52, 53, 54, 55, 60, 61, 62, 63};
   c7x::uchar_vec vMaskOut = *stov_ptr(c7x::uchar_vec, (uint8_t *) &mask[0]);
   return vMaskOut;
}

template <> inline c7x::uchar_vec setMask<int8_t>()
{
   uint8_t        mask[64] = {0,  1,  4,  5,  8,  9,  12, 13, 2,  3,  6,  7,  10, 11, 14, 15, 32, 33, 36, 37, 40, 41,
                              44, 45, 34, 35, 38, 39, 42, 43, 46, 47, 16, 17, 20, 21, 24, 25, 28, 29, 18, 19, 22, 23,
                              26, 27, 30, 31, 48, 49, 52, 53, 56, 57, 60, 61, 50, 51, 54, 55, 58, 59, 62, 63};
   c7x::uchar_vec vMaskOut = *stov_ptr(c7x::uchar_vec, (uint8_t *) &mask[0]);
   return vMaskOut;
}

template <typename V, __SE_REG, __SE_REG> static inline void vecMulAcc(V outSum0, V outSum1);

template <>
inline void vecMulAcc<c7x::long_vec &, __SE_REG_0, __SE_REG_1_ADV>(c7x::long_vec &outSum0, c7x::long_vec &outSum1)
{
   c7x::long_vec vOut0, vOut1;
   __vmatmpyhd_vvw(__SE_REG_0, __SE_REG_1_ADV, vOut0, vOut1);
   outSum0 += vOut0;
   outSum1 += vOut1;
}

template <>
inline void vecMulAcc<c7x::long_vec &, __SE_REG_0_ADV, __SE_REG_1_ADV>(c7x::long_vec &outSum0, c7x::long_vec &outSum1)
{
   c7x::long_vec vOut0, vOut1;
   __vmatmpyhd_vvw(__SE_REG_0_ADV, __SE_REG_1_ADV, vOut0, vOut1);
   outSum0 += vOut0;
   outSum1 += vOut1;
}

template <>
inline void vecMulAcc<c7x::int_vec &, __SE_REG_0, __SE_REG_1_ADV>(c7x::int_vec &outSum0, c7x::int_vec &outSum1)
{
   c7x::int_vec vOut = __matmpy(__SE_REG_0, __SE_REG_1_ADV);
   outSum0 += vOut;
}

template <>
inline void vecMulAcc<c7x::int_vec &, __SE_REG_0_ADV, __SE_REG_1_ADV>(c7x::int_vec &outSum0, c7x::int_vec &outSum1)
{
   c7x::int_vec vOut = __matmpy(__SE_REG_0_ADV, __SE_REG_1_ADV);
   outSum0 += vOut;
}

template <typename V> static inline void vecShiftRight(V vecSR0, V vecSR1, V shiftVec);

template <>
inline void vecShiftRight<c7x::long_vec &>(c7x::long_vec &vecSRL0, c7x::long_vec &vecSRL1, c7x::long_vec &shiftVec)
{
   vecSRL0 = __shift_right(vecSRL0, shiftVec);
   vecSRL1 = __shift_right(vecSRL1, shiftVec);
}

template <>
inline void vecShiftRight<c7x::int_vec &>(c7x::int_vec &vecSRI0, c7x::int_vec &vecSRI1, c7x::int_vec &shiftVec)
{
   vecSRI0 = __shift_right(vecSRI0, shiftVec);
}

template <typename V> static inline void resetVec(V vecRes0, V vecRes1);

template <> inline void resetVec<c7x::long_vec &>(c7x::long_vec &vecResL0, c7x::long_vec &vecResL1)
{
   vecResL0 = (c7x::long_vec) 0;
   vecResL1 = (c7x::long_vec) 0;
}

template <> inline void resetVec<c7x::int_vec &>(c7x::int_vec &vecResI0, c7x::int_vec &vecResI1)
{
   vecResI0 = (c7x::int_vec) 0;
}

template <typename V>
static inline void packAlternate(V vecPA0, V vecPA1, V vecPA2, V vecPA3, V vecPAOut0, V vecPAOut1);

template <>
inline void packAlternate<c7x::long_vec &>(c7x::long_vec &vecPAL0,
                                           c7x::long_vec &vecPAL1,
                                           c7x::long_vec &vecPAL2,
                                           c7x::long_vec &vecPAL3,
                                           c7x::long_vec &vecPALOut0,
                                           c7x::long_vec &vecPALOut1)
{
   vecPALOut0 = c7x::as_long_vec(__vpackw_vvv(c7x::as_int_vec(vecPAL2), c7x::as_int_vec(vecPAL0)));
   vecPALOut1 = c7x::as_long_vec(__vpackw_vvv(c7x::as_int_vec(vecPAL3), c7x::as_int_vec(vecPAL1)));
}

template <>
inline void packAlternate<c7x::int_vec &>(c7x::int_vec &vecPAI0,
                                          c7x::int_vec &vecPAI1,
                                          c7x::int_vec &vecPAI2,
                                          c7x::int_vec &vecPAI3,
                                          c7x::int_vec &vecPAIOut0,
                                          c7x::int_vec &vecPAIOut1)
{
   vecPAIOut0 = c7x::as_int_vec(__vpackl2_vvv(c7x::as_short_vec(vecPAI2), c7x::as_short_vec(vecPAI0)));
}

template <typename V, typename W>
static inline void vecPermutePack(V              vecPerm1,
                                  V              vecPerm2,
                                  V              vecPerm3,
                                  V              vecPerm4,
                                  V              vecPerm5,
                                  V              vecPerm6,
                                  V              vecPerm7,
                                  V              vecPerm8,
                                  W              pOutLocal,
                                  c7x::uchar_vec vMask);

template <>
inline void vecPermutePack<c7x::long_vec &, int16_t *>(c7x::long_vec &vecPermL1,
                                                       c7x::long_vec &vecPermL2,
                                                       c7x::long_vec &vecPermL3,
                                                       c7x::long_vec &vecPermL4,
                                                       c7x::long_vec &vecPermL5,
                                                       c7x::long_vec &vecPermL6,
                                                       c7x::long_vec &vecPermL7,
                                                       c7x::long_vec &vecPermL8,
                                                       int16_t       *pOutLocal,
                                                       c7x::uchar_vec vMaskPerm)
{
   c7x::long_vec vecPermL9, vecPermL10, vecPermL11, vecPermL12, vecPermL13, vecPermL14, vecPermL15, vecPermL16;

   vecPermL9 =
       c7x::as_long_vec(__permute_low_low(vMaskPerm, c7x::as_uchar_vec(vecPermL3), c7x::as_uchar_vec(vecPermL1)));
   vecPermL10 =
       c7x::as_long_vec(__permute_low_low(vMaskPerm, c7x::as_uchar_vec(vecPermL4), c7x::as_uchar_vec(vecPermL2)));
   vecPermL11 =
       c7x::as_long_vec(__permute_low_low(vMaskPerm, c7x::as_uchar_vec(vecPermL7), c7x::as_uchar_vec(vecPermL5)));
   vecPermL12 =
       c7x::as_long_vec(__permute_low_low(vMaskPerm, c7x::as_uchar_vec(vecPermL8), c7x::as_uchar_vec(vecPermL6)));
   vecPermL13 =
       c7x::as_long_vec(__permute_high_high(vMaskPerm, c7x::as_uchar_vec(vecPermL3), c7x::as_uchar_vec(vecPermL1)));
   vecPermL14 =
       c7x::as_long_vec(__permute_high_high(vMaskPerm, c7x::as_uchar_vec(vecPermL4), c7x::as_uchar_vec(vecPermL2)));
   vecPermL15 =
       c7x::as_long_vec(__permute_high_high(vMaskPerm, c7x::as_uchar_vec(vecPermL7), c7x::as_uchar_vec(vecPermL5)));
   vecPermL16 =
       c7x::as_long_vec(__permute_high_high(vMaskPerm, c7x::as_uchar_vec(vecPermL8), c7x::as_uchar_vec(vecPermL6)));

   vecPermL1 = c7x::long_vec(vecPermL9.lo(), vecPermL11.lo());
   vecPermL2 = c7x::long_vec(vecPermL10.lo(), vecPermL12.lo());
   vecPermL3 = c7x::long_vec(vecPermL13.lo(), vecPermL15.lo());
   vecPermL4 = c7x::long_vec(vecPermL14.lo(), vecPermL16.lo());
   vecPermL5 = c7x::long_vec(vecPermL9.hi(), vecPermL11.hi());
   vecPermL6 = c7x::long_vec(vecPermL10.hi(), vecPermL12.hi());
   vecPermL7 = c7x::long_vec(vecPermL13.hi(), vecPermL15.hi());
   vecPermL8 = c7x::long_vec(vecPermL14.hi(), vecPermL16.hi());
}

template <>
inline void vecPermutePack<c7x::int_vec &, int8_t *>(c7x::int_vec  &vecPermI1,
                                                     c7x::int_vec  &vecPermI2,
                                                     c7x::int_vec  &vecPermI3,
                                                     c7x::int_vec  &vecPermI4,
                                                     c7x::int_vec  &vecPermI5,
                                                     c7x::int_vec  &vecPermI6,
                                                     c7x::int_vec  &vecPermI7,
                                                     c7x::int_vec  &vecPermI8,
                                                     int8_t        *pOutLocal,
                                                     c7x::uchar_vec vMaskPerm)
{
   c7x::int_vec vecPermI9, vecPermI11, vecPermI13, vecPermI15;

   vecPermI9 =
       c7x::as_int_vec(__permute_low_low(vMaskPerm, c7x::as_uchar_vec(vecPermI3), c7x::as_uchar_vec(vecPermI1)));
   vecPermI11 =
       c7x::as_int_vec(__permute_low_low(vMaskPerm, c7x::as_uchar_vec(vecPermI7), c7x::as_uchar_vec(vecPermI5)));
   vecPermI13 =
       c7x::as_int_vec(__permute_high_high(vMaskPerm, c7x::as_uchar_vec(vecPermI3), c7x::as_uchar_vec(vecPermI1)));
   vecPermI15 =
       c7x::as_int_vec(__permute_high_high(vMaskPerm, c7x::as_uchar_vec(vecPermI7), c7x::as_uchar_vec(vecPermI5)));

   vecPermI1 = c7x::int_vec(vecPermI9.lo(), vecPermI11.lo());
   vecPermI3 = c7x::int_vec(vecPermI13.lo(), vecPermI15.lo());
   vecPermI5 = c7x::int_vec(vecPermI9.hi(), vecPermI11.hi());
   vecPermI7 = c7x::int_vec(vecPermI13.hi(), vecPermI15.hi());
}

template <typename dataTypeIn> static inline void writeOutSA0(dataTypeIn *pOut, c7x::short_vec v1, c7x::short_vec v2);

template <> inline void writeOutSA0(int16_t *pOut, c7x::short_vec v1, c7x::short_vec v2)
{
   __vpred         tmp      = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
   c7x::short_vec *storevec = c7x::strm_agen<0, c7x::short_vec>::get_adv(pOut);
   __vstore_pred_interleave(tmp, storevec, v1, v2);
}

template <> inline void writeOutSA0(int8_t *pOut, c7x::short_vec v1, c7x::short_vec v2)
{
   __vpred         tmp      = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
   c7x::char_hvec *storevec = c7x::strm_agen<0, c7x::char_hvec>::get_adv(pOut);
   __vstore_pred_packl(tmp, storevec, v1);
}

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS
DSPLIB_matMul_fixed_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
#if ENABLE_PROFILE
   uint64_t start        = __TSC;
   uint64_t overhead     = __TSC - start;
   uint64_t loopCycle    = 0;
   uint64_t accloopCycle = 0;
   uint64_t count        = 0;
#endif

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;
   uint8_t                      *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                       MBlocks      = pKerPrivArgs->MBlocks;
   int32_t                       NBlocks      = pKerPrivArgs->NBlocks;
   int32_t                       KBlocks      = pKerPrivArgs->KBlocks;
   int32_t                       qs           = pKerPrivArgs->qs;

   dataTypeIn *pIn0Local = (dataTypeIn *) pIn0;
   dataTypeIn *pIn1Local = (dataTypeIn *) pIn1;
   dataTypeIn *pOutLocal = (dataTypeIn *) pOut;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn1Local, se1Params);
   __SA0_OPEN(sa0Params);

   c7x::uchar_vec vMask = setMask<dataTypeIn>();

   dataTypeOut shiftVec = (dataTypeOut) qs;
   dataTypeOut zeroVec  = (dataTypeOut) 0;
   dataTypeOut outSum0 = zeroVec, outSum1 = zeroVec, outSum2 = zeroVec, outSum3 = zeroVec;
   dataTypeOut outSum4 = zeroVec, outSum5 = zeroVec, outSum6 = zeroVec, outSum7 = zeroVec;
   dataTypeOut outSum8 = zeroVec, outSum9 = zeroVec, outSum10 = zeroVec, outSum11 = zeroVec;
   dataTypeOut outSum12 = zeroVec, outSum13 = zeroVec, outSum14 = zeroVec, outSum15 = zeroVec;
   dataTypeOut out1, out2, out3, out4, out5, out6, out7, out8;

   for (int32_t l = 0; l < MBlocks * NBlocks; l++) {
#if ENABLE_PROFILE
      start = __TSC;
#endif
      for (int32_t p = 0; p < KBlocks; p++) {
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum0, outSum1);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum2, outSum3);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum4, outSum5);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum6, outSum7);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum8, outSum9);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum10, outSum11);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum12, outSum13);
         vecMulAcc<dataTypeOut &, __SE_REG_0_ADV, __SE_REG_1_ADV>(outSum14, outSum15);
      }
#if ENABLE_PROFILE
      loopCycle = __TSC - start;
      accloopCycle += loopCycle;
      printf("|INNER LOOP CNT : %d| %lu |\n", count++, loopCycle);
#endif

      vecShiftRight<dataTypeOut &>(outSum0, outSum1, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum2, outSum3, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum4, outSum5, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum6, outSum7, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum8, outSum9, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum10, outSum11, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum12, outSum13, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum14, outSum15, shiftVec);

      packAlternate<dataTypeOut &>(outSum0, outSum1, outSum2, outSum3, out1, out2);
      packAlternate<dataTypeOut &>(outSum4, outSum5, outSum6, outSum7, out3, out4);
      packAlternate<dataTypeOut &>(outSum8, outSum9, outSum10, outSum11, out5, out6);
      packAlternate<dataTypeOut &>(outSum12, outSum13, outSum14, outSum15, out7, out8);

      vecPermutePack<dataTypeOut &, dataTypeIn *>(out1, out2, out3, out4, out5, out6, out7, out8, pOutLocal, vMask);

      writeOutSA0(pOutLocal, c7x::as_short_vec(out1), c7x::as_short_vec(out2));
      writeOutSA0(pOutLocal, c7x::as_short_vec(out5), c7x::as_short_vec(out6));
      writeOutSA0(pOutLocal, c7x::as_short_vec(out3), c7x::as_short_vec(out4));
      writeOutSA0(pOutLocal, c7x::as_short_vec(out7), c7x::as_short_vec(out8));

      resetVec<dataTypeOut &>(outSum0, outSum1);
      resetVec<dataTypeOut &>(outSum2, outSum3);
      resetVec<dataTypeOut &>(outSum4, outSum5);
      resetVec<dataTypeOut &>(outSum6, outSum7);
      resetVec<dataTypeOut &>(outSum8, outSum9);
      resetVec<dataTypeOut &>(outSum10, outSum11);
      resetVec<dataTypeOut &>(outSum12, outSum13);
      resetVec<dataTypeOut &>(outSum14, outSum15);
   }

#if ENABLE_PROFILE
   printf("|CORE LOOP IN16_T | %lu |\n", accloopCycle);
#endif

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   return (status);
}

template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);

template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);
#endif
