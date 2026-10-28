// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_recip.h"
#include "DSPLIB_recip_priv.h"
#include "DSPLIB_types.h"
#include "c7x_scalable.h"
#include <cstdint>
#include <float.h>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

void DSPLIB_recip_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   uint64_t               recipStartupCycles   = 0;
   uint64_t               recipTeardownCycles  = 0;
   uint64_t               recipOperationCycles = 0;
   uint64_t               recipOverheadCycles  = 0;
   uint32_t               numVecs;
   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;

   numVecs = pKerPrivArgs->numVecs;

   if (pKerPrivArgs->initArgs.storeStyle == DSPLIB_HSTACK_ST) {
      recipStartupCycles   = 36;
      recipTeardownCycles  = 7;
      recipOperationCycles = (23 + numVecs * 21);
   }
   else if (pKerPrivArgs->initArgs.storeStyle == DSPLIB_INTERLEAVE_ST) {
      recipStartupCycles   = 39;
      recipTeardownCycles  = 3;
      recipOperationCycles = (46 + numVecs * 10);
   }
   else if (pKerPrivArgs->dataType == DSPLIB_FLOAT32) {
      recipStartupCycles   = 34;
      recipTeardownCycles  = 5;
      recipOperationCycles = (14 + numVecs * 8);
   }
   else {
      recipStartupCycles   = 32;
      recipTeardownCycles  = 7;
      recipOperationCycles = (13 + numVecs * 2);
   }

   recipOverheadCycles = recipStartupCycles + recipTeardownCycles;
   *estCycles          = recipOperationCycles + recipOverheadCycles;
   *archCycles         = recipOperationCycles;
}

// this method initializes the reciprocal kernel with appropriate parameters for floating point computations
template <typename dataType>
DSPLIB_STATUS DSPLIB_recip_init_ci(DSPLIB_kernelHandle          handle,
                                   const DSPLIB_bufParams2D_t  *bufParamsIn,
                                   const DSPLIB_bufParams2D_t  *bufParamsOut,
                                   const DSPLIB_recip_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS          status       = DSPLIB_SUCCESS;
   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;
   int32_t                width        = pKerPrivArgs->width;
   int32_t                height       = pKerPrivArgs->height;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   SE_VECLEN        = c7x::se_veclen<vec>::value;
   SA_VECLEN        = c7x::sa_veclen<vec>::value;
   SE_ELETYPE       = c7x::se_eletype<vec>::value;
   int32_t eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch the input                      */
   /**********************************************************************/
   se0Params         = __gen_SE_TEMPLATE_v1();
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;

   /**********************************************************************/
   /* Prepare SA template to store output                                */
   /**********************************************************************/
   sa0Params        = __gen_SA_TEMPLATE_v1();
   sa0Params.VECLEN = SA_VECLEN;

   // float processes 8 vectors per loop iteration, double 1
   int32_t unrollFactor = (sizeof(dataType) == sizeof(float)) ? 8 : 1;

   // For input buffer without stride
   if ((pKerPrivArgs->strideIn == width) && (pKerPrivArgs->strideOut == width)) {
      pKerPrivArgs->numVecs = DSPLIB_ceilingDiv(DSPLIB_ceilingDiv(width * height, eleCount), unrollFactor);

      se0Params.ICNT0  = width * height;
      se0Params.DIMFMT = __SE_DIMFMT_1D;

      sa0Params.ICNT0  = width * height;
      sa0Params.DIMFMT = __SA_DIMFMT_1D;
   }
   else {
      pKerPrivArgs->numVecs = DSPLIB_ceilingDiv(DSPLIB_ceilingDiv(width, eleCount) * height, unrollFactor);

      se0Params.ICNT0  = width;
      se0Params.ICNT1  = height;
      se0Params.DIM1   = pKerPrivArgs->strideIn;
      se0Params.DIMFMT = __SE_DIMFMT_2D;

      sa0Params.ICNT0  = width;
      sa0Params.ICNT1  = height;
      sa0Params.DIM1   = pKerPrivArgs->strideOut;
      sa0Params.DIMFMT = __SA_DIMFMT_2D;
   }

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_recip_init_ci<float>(DSPLIB_kernelHandle          handle,
                                                   const DSPLIB_bufParams2D_t  *bufParamsIn,
                                                   const DSPLIB_bufParams2D_t  *bufParamsOut,
                                                   const DSPLIB_recip_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_recip_init_ci<double>(DSPLIB_kernelHandle          handle,
                                                    const DSPLIB_bufParams2D_t  *bufParamsIn,
                                                    const DSPLIB_bufParams2D_t  *bufParamsOut,
                                                    const DSPLIB_recip_InitArgs *pKerInitArgs);

// this method initializes the reciprocal kernel with appropriate parameters for fixed point computations
template <>
DSPLIB_STATUS DSPLIB_recip_init_ci<int16_t>(DSPLIB_kernelHandle          handle,
                                            const DSPLIB_bufParams2D_t  *bufParamsIn,
                                            const DSPLIB_bufParams2D_t  *bufParamsOut,
                                            const DSPLIB_recip_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   //  setup se, sa parameters for fixed point
   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;
   int32_t                width        = pKerPrivArgs->width;
   int32_t                height       = pKerPrivArgs->height;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;
   __SE_PROMOTE SE_PROMOTE;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<int16_t>::type vec;
   SE_VECLEN  = c7x::se_veclen<c7x::int_vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;
   SE_PROMOTE = __SE_PROMOTE_2X_SIGNEXT;

   int32_t eleCount = c7x::element_count_of<c7x::int_vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   // setup streaming engine 0 to fetch data
   se0Params = __gen_SE_TEMPLATE_v1();

   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.PROMOTE = SE_PROMOTE;

   // setup streaming address 0,1 to store data
   sa0Params = __gen_SA_TEMPLATE_v1();

   sa0Params.VECLEN = SA_VECLEN;

   if (pKerPrivArgs->initArgs.storeStyle == DSPLIB_INTERLEAVE_ST) {
      // For input buffer without stride
      if ((pKerPrivArgs->strideIn == width) && (pKerPrivArgs->strideOut == (2 * width))) {
         pKerPrivArgs->numVecs = DSPLIB_ceilingDiv(width * height, eleCount);

         se0Params.ICNT0  = width * height;
         se0Params.DIMFMT = __SE_DIMFMT_1D;

         sa0Params.ICNT0  = 2 * width * height;
         sa0Params.DIMFMT = __SA_DIMFMT_1D;
      }
      else {
         pKerPrivArgs->numVecs = DSPLIB_ceilingDiv(width, eleCount) * height;

         se0Params.ICNT0  = width;
         se0Params.ICNT1  = height;
         se0Params.DIM1   = pKerPrivArgs->strideIn;
         se0Params.DIMFMT = __SE_DIMFMT_2D;

         sa0Params.ICNT0  = 2 * width;
         sa0Params.ICNT1  = height;
         sa0Params.DIM1   = pKerPrivArgs->strideOut;
         sa0Params.DIMFMT = __SA_DIMFMT_2D;
      }
   }
   else if (height == 1) {
      // SA0 stores the exponent half and SA1 the fraction half of the single row
      pKerPrivArgs->numVecs = DSPLIB_ceilingDiv(width, 2 * eleCount);

      se0Params.ICNT0  = width;
      se0Params.DIMFMT = __SE_DIMFMT_1D;

      sa0Params.ICNT0  = width;
      sa0Params.DIMFMT = __SA_DIMFMT_1D;
   }
   else {
      // SA0 stores the exponent half and SA1 the fraction half of each row. Each loop iteration consumes two SE
      // vectors, so every row is padded to a multiple of 2 * eleCount elements; DECDIM limits the fetch to width
      // elements per row.
      pKerPrivArgs->numVecs = DSPLIB_ceilingDiv(width, 2 * eleCount) * height;

      se0Params.ICNT0         = DSPLIB_ceilingDiv(width, 2 * eleCount) * (2 * eleCount);
      se0Params.ICNT1         = 1;
      se0Params.DIM1          = 0;
      se0Params.ICNT2         = height;
      se0Params.DIM2          = pKerPrivArgs->strideIn;
      se0Params.DECDIM1       = __SE_DECDIM_DIM1;
      se0Params.DECDIM1_WIDTH = width;
      se0Params.DIMFMT        = __SE_DIMFMT_3D;

      sa0Params.ICNT0  = width;
      sa0Params.ICNT1  = height;
      sa0Params.DIM1   = pKerPrivArgs->strideOut;
      sa0Params.DIMFMT = __SA_DIMFMT_2D;
   }

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

// this method performs single precision floating point reciprocal computations
template <typename dataType, int32_t dataIn>
DSPLIB_STATUS DSPLIB_recip_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;
   int32_t                numVecs      = pKerPrivArgs->numVecs;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_recip_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;

#if DSPLIB_DEBUGPRINT
   int32_t eleCount = c7x::element_count_of<vec>::value;
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   // Input samples
   __SE0_OPEN(pInLocal, se0Params);

   // Output samples
   __SA0_OPEN(sa0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT numVecs %d\n", numVecs);
#endif

   vec Two;

   Two = (vec) 2.0;

   /* 14 + trip_cnt * 8 */
   for (int32_t counter = 0; counter < numVecs; counter++) {
      vec a0 = c7x::strm_eng<0, vec>::get_adv();
      vec a1 = c7x::strm_eng<0, vec>::get_adv();
      vec a2 = c7x::strm_eng<0, vec>::get_adv();
      vec a3 = c7x::strm_eng<0, vec>::get_adv();
      vec a4 = c7x::strm_eng<0, vec>::get_adv();
      vec a5 = c7x::strm_eng<0, vec>::get_adv();
      vec a6 = c7x::strm_eng<0, vec>::get_adv();
      vec a7 = c7x::strm_eng<0, vec>::get_adv();

      // Calculate reciprocal
      vec p0 = __recip(a0);
      vec p1 = __recip(a1);
      vec p2 = __recip(a2);
      vec p3 = __recip(a3);
      vec p4 = __recip(a4);
      vec p5 = __recip(a5);
      vec p6 = __recip(a6);
      vec p7 = __recip(a7);

      vec d0 = p0 * a0;
      vec d1 = p1 * a1;
      vec d2 = p2 * a2;
      vec d3 = p3 * a3;
      vec d4 = p4 * a4;
      vec d5 = p5 * a5;
      vec d6 = p6 * a6;
      vec d7 = p7 * a7;

      vec out0 = p0 * (Two - d0);
      vec out1 = p1 * (Two - d1);
      vec out2 = p2 * (Two - d2);
      vec out3 = p3 * (Two - d3);
      vec out4 = p4 * (Two - d4);
      vec out5 = p5 * (Two - d5);
      vec out6 = p6 * (Two - d6);
      vec out7 = p7 * (Two - d7);

      __vpred tmp;
      vec    *VB1;

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out0);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out1);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out2);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out3);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out4);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out5);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out6);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, out7);
   }
   __SE0_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS
DSPLIB_recip_exec_ci<float, DSPLIB_FLOAT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

// this method performs double precision floating point reciprocal computations
template <> DSPLIB_STATUS
DSPLIB_recip_exec_ci<double, DSPLIB_FLOAT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   typedef double dataType;

   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;
   int32_t                numVecs      = pKerPrivArgs->numVecs;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_recip_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;

#if DSPLIB_DEBUGPRINT
   int32_t eleCount = c7x::element_count_of<vec>::value;
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   // Input samples
   __SE0_OPEN(pInLocal, se0Params);

   // Output samples
   __SA0_OPEN(sa0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT numVecs %d\n", numVecs);
#endif

   vec out;
   vec Two;

   Two = (vec) 2.0;

   /* 13 + trip_cnt * 2 */
   for (int32_t counter = 0; counter < numVecs; counter++) {
      vec a = c7x::strm_eng<0, vec>::get_adv();

      vec d0, p0, p1;

      // Calculate reciprocal
      p0 = __recip(a);
      d0 = p0 * a;

      p1  = Two - d0;
      out = p0 * p1;

      __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);

      __vstore_pred(tmp, VB1, out);
   }
   __SE0_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

// this method performs fixed point reciprocal computations
template <> DSPLIB_STATUS
DSPLIB_recip_exec_ci<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   DSPLIB_STATUS          status       = DSPLIB_SUCCESS;
   DSPLIB_recip_PrivArgs *pKerPrivArgs = (DSPLIB_recip_PrivArgs *) handle;

   // setup iteration count, open streaming engine/address, etc.
   uint32_t                width      = pKerPrivArgs->width;
   int32_t                 numVecs    = pKerPrivArgs->numVecs;
   DSPLIB_recip_storeStyle storeStyle = pKerPrivArgs->initArgs.storeStyle;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   int16_t *restrict pInLocal  = (int16_t *) pIn;
   int16_t *restrict pOutLocal = (int16_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_recip_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<int16_t>::type vec;

#if DSPLIB_DEBUGPRINT
   int32_t eleCount = c7x::element_count_of<c7x::int_vec>::value;
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   // Input samples
   __SE0_OPEN(pInLocal, se0Params);

   // Output samples
   __SA0_OPEN(sa0Params);

   c7x::int_vec zero    = (c7x::int_vec) 0;
   c7x::int_vec one     = (c7x::int_vec) 1;
   c7x::int_vec fifteen = (c7x::int_vec) 15;

   /* Horizontal stack implementation of fixed point store */
   /* ========================================================== */
   if (storeStyle == DSPLIB_HSTACK_ST) {

      /* Horizontal store uses one more SA for offset store */
      __SA1_OPEN(sa0Params);

      //  setup outerloop
      /* 23 + trip_cnt * 21 */
      for (int32_t counter = 0; counter < numVecs; counter++) {

         // place into 32b container vectors and do computation
         c7x::int_vec a_hi = c7x::strm_eng<0, c7x::int_vec>::get_adv();
         c7x::int_vec a_lo = c7x::strm_eng<0, c7x::int_vec>::get_adv();

         __vpred cmp_sign_hi = __cmp_gt_pred(zero, a_hi);
         __vpred cmp_sign_lo = __cmp_gt_pred(zero, a_lo);

         // determine negate input
         c7x::int_vec neg_hi = __select(cmp_sign_hi, one, zero);
         c7x::int_vec neg_lo = __select(cmp_sign_lo, one, zero);

         a_hi = __select(cmp_sign_hi, -a_hi, a_hi);
         a_lo = __select(cmp_sign_lo, -a_lo, a_lo);

         // normalize
         c7x::int_vec norm_hi = __norm(a_hi);
         c7x::int_vec norm_lo = __norm(a_lo);

         a_hi = a_hi << norm_hi;
         a_lo = a_lo << norm_lo;

         // take only lower 16b from hi and lo (via permute) and store in rexp (16b vector)
         c7x::int_vec exp_hi = norm_hi - fifteen;
         c7x::int_vec exp_lo = norm_lo - fifteen;

         /* original implementation */
         /* ================================================*/
         //  permute exp_hi and exp_lo into rexp
         /* c7x::uchar_vec exp_hi_byte = c7x::reinterpret<c7x::uchar_vec>(exp_hi); */
         /* c7x::uchar_vec exp_lo_byte = c7x::reinterpret<c7x::uchar_vec>(exp_lo); */

         /* c7x::uchar_vec permuteExp = __permute_even_even_short(DSPLIB_vperm_mask_hstack, exp_lo_byte, exp_hi_byte);
          */

         // store rexp via sa0
         /* vec rexp = c7x::reinterpret<vec>(permuteExp); */
         /* =================================================*/

         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);

         /* __vstore_pred(tmp, VB1, rexp); */

         __vstore_pred_packl_2src(tmp, VB1, exp_hi, exp_lo);
         // set dividends
         c7x::uint_vec ub_hi = (c7x::uint_vec) 0x80000000U;
         c7x::uint_vec ub_lo = (c7x::uint_vec) 0x80000000U;

         c7x::uint_vec ua_hi = c7x::reinterpret<c7x::uint_vec>(a_hi);
         c7x::uint_vec ua_lo = c7x::reinterpret<c7x::uint_vec>(a_lo);

         // setup inner loop and unroll it 15x

#pragma UNROLL(15)
         for (int32_t i = 0; i < 15; i++) {

            ub_hi = __sub_cond(ub_hi, ua_hi);
            ub_lo = __sub_cond(ub_lo, ua_lo);
         }

         c7x::uint_vec divMask = (c7x::uint_vec) 0x7FFF;

         ub_hi = ub_hi & divMask;
         ub_lo = ub_lo & divMask;

         c7x::int_vec b_hi = c7x::reinterpret<c7x::int_vec>(ub_hi);
         c7x::int_vec b_lo = c7x::reinterpret<c7x::int_vec>(ub_lo);

         __vpred cmp_neg_hi = __cmp_eq_pred(neg_hi, one);
         __vpred cmp_neg_lo = __cmp_eq_pred(neg_lo, one);

         b_hi = __select(cmp_neg_hi, -b_hi, b_hi);
         b_lo = __select(cmp_neg_lo, -b_lo, b_lo);

         // store frac
         /* c7x::uchar_vec b_hi_byte = c7x::reinterpret<c7x::uchar_vec>(b_hi); */
         /* c7x::uchar_vec b_lo_byte = c7x::reinterpret<c7x::uchar_vec>(b_lo); */

         /* c7x::uchar_vec permuteFrac = __permute_even_even_short(DSPLIB_vperm_mask_hstack, b_lo_byte, b_hi_byte); */

         /* vec rFrac = c7x::reinterpret<vec>(permuteFrac); */

         tmp = c7x::strm_agen<1, vec>::get_vpred();
         VB1 = c7x::strm_agen<1, vec>::get_adv(pOutLocal + width);

         /* __vstore_pred(tmp, VB1, rFrac); */
         __vstore_pred_packl_2src(tmp, VB1, b_hi, b_lo);
      }

      __SA1_CLOSE();
   }

   /* Interleaved implementation of fixed point store */
   /* ========================================================== */
   else if (storeStyle == DSPLIB_INTERLEAVE_ST) {

      /* 46 + trip_cnt * 10 */
      for (int32_t counter = 0; counter < numVecs; counter++) {

         // grab values as 32b integers
         c7x::int_vec a = c7x::strm_eng<0, c7x::int_vec>::get_adv();

         __vpred cmp_sign = __cmp_gt_pred(zero, a);

         // determine negate input
         c7x::int_vec neg = __select(cmp_sign, one, zero);

         a = __select(cmp_sign, -a, a);

         // normalize
         c7x::int_vec norm = __norm(a);

         a = a << norm;

         // take only lower 16b (via permute) and store in rexp (16b vector)
         c7x::int_vec expVec = norm - fifteen;
         /* c7x::uchar_vec exp_byte = c7x::reinterpret<c7x::uchar_vec>(expVec); */

         // set dividends
         c7x::uint_vec ub = (c7x::uint_vec) 0x80000000U;
         c7x::uint_vec ua = c7x::reinterpret<c7x::uint_vec>(a);

         // setup inner loop and unroll it 15x
#pragma UNROLL(15)
         for (int32_t i = 0; i < 15; i++) {

            ub = __sub_cond(ub, ua);
         }

         c7x::uint_vec divMask = (c7x::uint_vec) 0x7FFF;

         ub = ub & divMask;

         c7x::int_vec b = c7x::reinterpret<c7x::int_vec>(ub);

         __vpred cmp_neg = __cmp_eq_pred(neg, one);

         b = __select(cmp_neg, -b, b);

         // store exp and frac
         /* c7x::uchar_vec frac_byte = c7x::reinterpret<c7x::uchar_vec>(b); */

         /* c7x::uchar_vec permuteExpFrac = __permute_even_even_short(DSPLIB_vperm_mask_interleave, frac_byte,
          * exp_byte); */

         /* vec rExpFrac = c7x::reinterpret<vec>(permuteExpFrac); */

         vec exp_short  = c7x::reinterpret<vec>(expVec);
         vec frac_short = c7x::reinterpret<vec>(b);

         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);

         /* __vstore_pred(tmp, VB1, rExpFrac); */
         __vstore_pred_interleave(tmp, VB1, exp_short, frac_short);
      }
   }

   /*  not implemented store method for fixed point */
   else {
      status = DSPLIB_ERR_NOT_IMPLEMENTED;
   }
   // close se, sa, etc.
   __SE0_CLOSE();
   __SA0_CLOSE();

   return status;
}
