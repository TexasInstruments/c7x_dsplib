// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_priv.h"

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */

template <typename dataType> void DSPLIB_diag_proc_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_svd_PrivArgs *pKerPrivArgs = (DSPLIB_svd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              strideVRow   = pKerPrivArgs->strideVRows;
   __SE_TEMPLATE_v1     se0Params    = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1     sa0Params    = __gen_SA_TEMPLATE_v1();
   __SE_TEMPLATE_v1     se1Params    = __gen_SE_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;

   int32_t rowVStride = strideVRow / sizeof(dataType);

   se0Params.ICNT0   = eleCount;
   se0Params.DIM1    = rowVStride;
   se0Params.DIM2    = eleCount * 2;
   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.DECDIM1 = __SE_DECDIM_DIM2;

   se1Params.ICNT0   = eleCount;
   se1Params.DIM1    = -eleCount;
   se1Params.DIMFMT  = __SE_DIMFMT_2D;
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN  = SE_VECLEN;

   sa0Params.ICNT0   = eleCount;
   sa0Params.DIM1    = rowVStride;
   sa0Params.DIM2    = eleCount * 2;
   sa0Params.DIMFMT  = __SA_DIMFMT_3D;
   sa0Params.VECLEN  = SA_VECLEN;
   sa0Params.DECDIM1 = __SA_DECDIM_DIM2;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (19 * SE_PARAM_SIZE)) = se1Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (20 * SE_PARAM_SIZE)) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (21 * SE_PARAM_SIZE)) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_diag_proc_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_diag_proc_init_ci<double>(DSPLIB_kernelHandle handle);

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */

template <typename dataType> inline dataType constEpsilon();
template <> inline float                     constEpsilon<float>() { return FLT_EPSILON; }
template <> inline double                    constEpsilon<double>() { return DBL_EPSILON; }

/*!
 *  @brief Updates "epsilon" value based on absolute max values from "diag" and "superdiag" vectors
 */
template <typename dataType>
void DSPLIB_diag_epsilon_ci(dataType *diag, dataType *superdiag, dataType *epsilon, int32_t Ncols, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params, se1Params;

   se0Params = se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   int32_t nVec = DSPLIB_ceilingDiv(Ncols, eleCount);

   se0Params.ICNT0 = Ncols;
   se1Params.ICNT0 = Ncols;

   __SE0_OPEN(diag, se0Params);
   __SE1_OPEN(superdiag, se1Params);

   vec max1, max2;
   max1 = max2        = (vec) 0;
   int32_t horizontal = 0;
   for (horizontal = 0; horizontal < nVec - 1; horizontal += 2) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();

      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<1, vec>::get_adv();

      vec add1 = __abs(v1) + __abs(v2);
      vec add2 = __abs(v3) + __abs(v4);

      max1 = __max(max1, add1);
      max2 = __max(max2, add2);
   }

   if (horizontal != nVec) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();

      vec add1 = __abs(v1) + __abs(v2);

      max1 = __max(max1, add1);
   }

   max1 = __max(max1, max2);

   dataType maxVal = c7x_horizontal_max_fp<dataType, vec>(max1);

   *epsilon = constEpsilon<dataType>() * maxVal;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void
DSPLIB_diag_epsilon_ci<float>(float *diag, float *superdiag, float *epsilon, int32_t Ncols, uint8_t *pBlock);
template void
DSPLIB_diag_epsilon_ci<double>(double *diag, double *superdiag, double *epsilon, int32_t Ncols, uint8_t *pBlock);

/*!
 *  @brief Moves predicate register to a 64-bit register.
 */

template <typename dataType> inline uint64_t vecMinIndex(__vpred pred);

template <> inline uint64_t vecMinIndex<float>(__vpred pred) { return (__rightmost_bit_detect_int(pred) / 4); }

template <> inline uint64_t vecMinIndex<double>(__vpred pred) { return (__rightmost_bit_detect_long(pred) / 8); }

/*!
 *  @brief Updates values of "m" and "rotation_test" flag vased on the values present in "diag",
 *        "superdiag" and  "epsilon".
 */
template <typename dataType>
void DSPLIB_diag_rotation_check_ci(dataType *diag,
                                   dataType *superdiag,
                                   dataType  epsilon,
                                   int32_t  *m,
                                   int32_t  *rotation_test,
                                   int32_t   Ncols,
                                   uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (19 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount       = c7x::element_count_of<vec>::value;
   int32_t                                                horizontal     = Ncols;
   int32_t                                                nVec           = (Ncols - 1) / eleCount;
   dataType                                              *diagStart      = diag - Ncols + 2;
   dataType                                              *superdiagStart = superdiag - Ncols + 1;
   bool                                                   breakLoop      = false;

   if (nVec > 0) {
      se0Params.ICNT1 = nVec;

      dataType *pSE0 = diag - (eleCount - 1);
      dataType *pSE1 = superdiag - (eleCount - 1);

      __SE0_OPEN(pSE0, se0Params);
      __SE1_OPEN(pSE1, se0Params);

      vec vecEpsilon = (vec) epsilon;
      for (horizontal = Ncols; horizontal > eleCount; horizontal -= eleCount) {
         /* SUPERDIAG */
         vec     v1SD   = c7x::strm_eng<1, vec>::get_adv();
         vec     v2SD   = __abs(v1SD);
         __vpred predSD = __cmp_le_pred(v2SD, vecEpsilon);

         __vpred rpredSD      = __bit_reverse(predSD);
         int64_t rightIndexSD = vecMinIndex<dataType>(rpredSD);
         int32_t minIndexSD   = eleCount - 1 - rightIndexSD;

         /* DIAG */
         vec     v1D   = c7x::strm_eng<0, vec>::get_adv();
         vec     v2D   = __abs(v1D);
         __vpred predD = __cmp_le_pred(v2D, vecEpsilon);

         __vpred rpredD      = __bit_reverse(predD);
         int64_t rightIndexD = vecMinIndex<dataType>(rpredD);
         int32_t minIndexD   = eleCount - 1 - rightIndexD;

         if (minIndexD >= 0 || minIndexSD >= 0) {

#if !defined(ENABLE_LDRA_COVERAGE)
            /* This part of code checks for test f convergence / cancellation condition
               Ref. Singular Value Decomposition and Least Squares Solutions. G. H. Golub et al
               We use these conditions in order to calculte correct results if and when they occur */
            if (minIndexSD >= minIndexD) {
               /* update rotation flag */
               *rotation_test = 0;
               *m             = horizontal - eleCount + minIndexSD;
            }
            else {
               *m = horizontal - eleCount + minIndexD;
            }
#else
            /* update rotation flag */
            *rotation_test = 0;
            *m             = horizontal - eleCount + minIndexSD;

#endif
            breakLoop = true;
            break;
         }
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
   }

   if ((!breakLoop)) {
      int32_t i = 0;
      for (i = horizontal - 1; i > 0; i--) {
         if (fabs(superdiagStart[i]) <= epsilon) {
            *rotation_test = 0;
            break;
         }
         if (fabs(diagStart[i - 1]) <= epsilon) {
            break;
         }
      } /* for (m=k;m>=0;m--) */
      if (i == 0) {
         *rotation_test = 0;
      }
      *m = i;
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_diag_rotation_check_ci<float>(float   *diag,
                                                   float   *superdiag,
                                                   float    epsilon,
                                                   int32_t *m,
                                                   int32_t *rotation_test,
                                                   int32_t  Ncols,
                                                   uint8_t *pBlock);
template void DSPLIB_diag_rotation_check_ci<double>(double  *diag,
                                                    double  *superdiag,
                                                    double   epsilon,
                                                    int32_t *m,
                                                    int32_t *rotation_test,
                                                    int32_t  Ncols,
                                                    uint8_t *pBlock);

/*!
 *  @brief Negates the values of a row in V
 */
template <typename dataType>
void DSPLIB_diag_negate_v_ci(dataType *V, int32_t Ncols, int32_t colVStride, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params, se1Params;
   __SA_TEMPLATE_v1 sa0Params, sa1Params;

   se0Params = se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE));
   sa0Params = sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   int32_t           nVec = DSPLIB_ceilingDiv(Ncols, eleCount);
   int32_t totalIter DSPLIB_ceilingDiv(nVec, 2);
   int32_t           se0ICNT0 = (nVec / 2) * eleCount;
   int32_t           se1ICNT0 = Ncols - se0ICNT0;

   se0Params.ICNT0 = sa0Params.ICNT0 = se0ICNT0;
   se1Params.ICNT0 = sa1Params.ICNT0 = se1ICNT0;

   dataType *pSE0 = V;
   dataType *pSE1 = V + se0ICNT0;
   __SE1_OPEN(pSE1, se1Params);
   __SA1_OPEN(sa1Params);

   if (se0ICNT0 > 0) {
      __SE0_OPEN(pSE0, se0Params);
      __SA0_OPEN(sa0Params);
   }
   for (int32_t horizontal = 0; horizontal < totalIter; horizontal++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();

      __vpred pred1 = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pV1   = c7x::strm_agen<0, vec>::get_adv(pSE0);
      __vstore_pred(pred1, pV1, -v1);

      __vpred pred2 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pV2   = c7x::strm_agen<1, vec>::get_adv(pSE1);
      __vstore_pred(pred2, pV2, -v2);
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_diag_negate_v_ci<float>(float *V, int32_t Ncols, int32_t colVStride, uint8_t *pBlock);
template void DSPLIB_diag_negate_v_ci<double>(double *V, int32_t Ncols, int32_t colVStride, uint8_t *pBlock);

/*!
 *  @brief Calculates the reciprocal of square-roots of "diag" and "superdiag" vectors
 */
template <typename dataType>
void DSPLIB_diag_sqrt_ci(dataType *superdiag, dataType *diag, int32_t length, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   int32_t                                                nVec     = DSPLIB_ceilingDiv(length, eleCount);

   se0Params.ICNT0 = sa0Params.ICNT0 = length;

   __SE0_OPEN(superdiag, se0Params);
   __SE1_OPEN(diag, se0Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa0Params);

   /* Constant values */
   vec half     = (vec) 0.5;
   vec OneP5    = (vec) 1.5;
   vec zero     = (vec) 0;
   vec maxValue = (vec) std::numeric_limits<dataType>::max();

   for (int32_t i = 0; i < nVec; i++) {
      vec vSD = c7x::strm_eng<0, vec>::get_adv();
      vec vD  = c7x::strm_eng<1, vec>::get_adv();

      vec p0SD = __recip_sqrt(vSD);
      vec d0SD = p0SD * vSD;
      vec p1SD = OneP5 - d0SD * p0SD * half;
      vec ySD  = p0SD * p1SD;

      d0SD = ySD * vSD;
      p1SD = OneP5 - d0SD * ySD * half;
      ySD  = vSD * ySD * p1SD;

      vec p0D = __recip_sqrt(vD);
      vec d0D = p0D * vD;
      vec p1D = OneP5 - d0D * p0D * half;
      vec yD  = p0D * p1D;

      d0D = yD * vD;
      p1D = OneP5 - d0D * yD * half;
      yD  = vD * yD * p1D;

      __vpred cmp_lezeroSD = __cmp_le_pred(vSD, zero);
      ySD                  = __select(cmp_lezeroSD, zero, ySD);
      __vpred cmp_gtmaxSD  = __cmp_le_pred(maxValue, vSD);
      vec     outSD        = __select(cmp_gtmaxSD, maxValue, ySD);

      __vpred cmp_lezeroD = __cmp_le_pred(vD, zero);
      yD                  = __select(cmp_lezeroD, zero, yD);
      __vpred cmp_gtmaxD  = __cmp_le_pred(maxValue, vD);
      vec     outD        = __select(cmp_gtmaxD, maxValue, yD);

      __vpred predSD = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pSD    = c7x::strm_agen<0, vec>::get_adv(superdiag);
      __vstore_pred(predSD, pSD, outSD);

      __vpred predD = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pD    = c7x::strm_agen<1, vec>::get_adv(diag);
      __vstore_pred(predD, pD, outD);
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_diag_sqrt_ci<float>(float *superdiag, float *diag, int32_t length, uint8_t *pBlock);
template void DSPLIB_diag_sqrt_ci<double>(double *superdiag, double *diag, int32_t length, uint8_t *pBlock);

/*!
 *  @brief Updates rows of V' and U' based on the precalculated cV/cU and sV/sU vectors.
 */
template <typename dataType>
void DSPLIB_diag_proc_ci(dataType *V,
                         int32_t   startRow,
                         int32_t   Nrows,
                         int32_t   Ncols,
                         int32_t   rowVStride,
                         dataType *cV,
                         dataType *sV,
                         uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   dataType *vStart = V + startRow * rowVStride;

   __SE_TEMPLATE_v1 se0Params, se1Params;
   __SA_TEMPLATE_v1 sa0Params, sa1Params, sa2Params, sa3Params;
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (20 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (20 * SE_PARAM_SIZE));
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (21 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (21 * SE_PARAM_SIZE));
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));

   int32_t rowPair  = Nrows - 1;
   int32_t nVec     = DSPLIB_ceilingDiv(Ncols, eleCount);
   int32_t se1ICNT2 = nVec / 2;
   int32_t se0ICNT2 = nVec - se1ICNT2;

   se0Params.ICNT1 = sa0Params.ICNT1 = Nrows;
   se0Params.ICNT2 = sa0Params.ICNT2 = se0ICNT2;
   se0Params.DECDIM1_WIDTH = sa0Params.DECDIM1_WIDTH = Ncols;

   se1Params.ICNT1 = sa1Params.ICNT1 = Nrows;
   se1Params.ICNT2 = sa1Params.ICNT2 = se1ICNT2;
   se1Params.DECDIM1_WIDTH = sa1Params.DECDIM1_WIDTH = Ncols - eleCount;

   se0Params.DIM1 = sa0Params.DIM1 = se1Params.DIM1 = sa1Params.DIM1 = rowVStride;

   sa2Params.ICNT0 = sa3Params.ICNT0 = rowPair;
   sa2Params.ICNT1 = sa3Params.ICNT1 = se0ICNT2;

   __SE0_OPEN(vStart, se0Params);
   __SA0_OPEN(sa0Params);
   __SA2_OPEN(sa2Params);
   __SA3_OPEN(sa3Params);

   if (se1ICNT2 > 0) {
      __SE1_OPEN(vStart + eleCount, se1Params);
      __SA1_OPEN(sa1Params);
      __vpred pred;
      vec    *pStore;
      for (int32_t horizontal = 0; horizontal < se1ICNT2; horizontal++) {
         vec v1_0 = c7x::strm_eng<0, vec>::get_adv();
         vec v1_1 = c7x::strm_eng<1, vec>::get_adv();

         int32_t vertical = 0;

         if (rowPair >= 4) {
            dataType *pcv1 = c7x::strm_agen<2, dataType>::get_adv(cV);
            vec       cv1  = __vload_dup(pcv1);
            dataType *psv1 = c7x::strm_agen<3, dataType>::get_adv(sV);
            vec       sv1  = __vload_dup(psv1);
            dataType *pcv2 = c7x::strm_agen<2, dataType>::get_adv(cV);
            vec       cv2  = __vload_dup(pcv2);
            dataType *psv2 = c7x::strm_agen<3, dataType>::get_adv(sV);
            vec       sv2  = __vload_dup(psv2);
            dataType *pcv3 = c7x::strm_agen<2, dataType>::get_adv(cV);
            vec       cv3  = __vload_dup(pcv3);
            dataType *psv3 = c7x::strm_agen<3, dataType>::get_adv(sV);
            vec       sv3  = __vload_dup(psv3);
            dataType *pcv4 = c7x::strm_agen<2, dataType>::get_adv(cV);
            vec       cv4  = __vload_dup(pcv4);
            dataType *psv4 = c7x::strm_agen<3, dataType>::get_adv(sV);
            vec       sv4  = __vload_dup(psv4);
            vec       v2_0, v2_1, v3_0, v3_1, v4_0, v4_1, v5_0, v5_1;
            vec       vs1_0, temps2_0, vs1_1, temps2_1;
            vec       vs2_0, temps3_0, vs2_1, temps3_1;
            vec       vs3_0, temps4_0, vs3_1, temps4_1;
            vec       vs4_0, vs4_1;
            for (vertical = 0; vertical < rowPair - 7; vertical += 4) {
               v2_0 = c7x::strm_eng<0, vec>::get_adv();
               v2_1 = c7x::strm_eng<1, vec>::get_adv();

               v3_0 = c7x::strm_eng<0, vec>::get_adv();
               v3_1 = c7x::strm_eng<1, vec>::get_adv();

               v4_0 = c7x::strm_eng<0, vec>::get_adv();
               v4_1 = c7x::strm_eng<1, vec>::get_adv();

               v5_0 = c7x::strm_eng<0, vec>::get_adv();
               v5_1 = c7x::strm_eng<1, vec>::get_adv();

               vs1_0    = (v2_0 * sv1) + (v1_0 * cv1);
               temps2_0 = (v2_0 * cv1) - (v1_0 * sv1);
               vs1_1    = (v2_1 * sv1) + (v1_1 * cv1);
               temps2_1 = (v2_1 * cv1) - (v1_1 * sv1);

               vs2_0    = (v3_0 * sv2) + (temps2_0 * cv2);
               temps3_0 = (v3_0 * cv2) - (temps2_0 * sv2);
               vs2_1    = (v3_1 * sv2) + (temps2_1 * cv2);
               temps3_1 = (v3_1 * cv2) - (temps2_1 * sv2);

               vs3_0    = (v4_0 * sv3) + (temps3_0 * cv3);
               temps4_0 = (v4_0 * cv3) - (temps3_0 * sv3);
               vs3_1    = (v4_1 * sv3) + (temps3_1 * cv3);
               temps4_1 = (v4_1 * cv3) - (temps3_1 * sv3);

               vs4_0 = (v5_0 * sv4) + (temps4_0 * cv4);
               v1_0  = (v5_0 * cv4) - (temps4_0 * sv4);
               vs4_1 = (v5_1 * sv4) + (temps4_1 * cv4);
               v1_1  = (v5_1 * cv4) - (temps4_1 * sv4);

               pcv1 = c7x::strm_agen<2, dataType>::get_adv(cV);
               cv1  = __vload_dup(pcv1);
               psv1 = c7x::strm_agen<3, dataType>::get_adv(sV);
               sv1  = __vload_dup(psv1);
               pcv2 = c7x::strm_agen<2, dataType>::get_adv(cV);
               cv2  = __vload_dup(pcv2);
               psv2 = c7x::strm_agen<3, dataType>::get_adv(sV);
               sv2  = __vload_dup(psv2);
               pcv3 = c7x::strm_agen<2, dataType>::get_adv(cV);
               cv3  = __vload_dup(pcv3);
               psv3 = c7x::strm_agen<3, dataType>::get_adv(sV);
               sv3  = __vload_dup(psv3);
               pcv4 = c7x::strm_agen<2, dataType>::get_adv(cV);
               cv4  = __vload_dup(pcv4);
               psv4 = c7x::strm_agen<3, dataType>::get_adv(sV);
               sv4  = __vload_dup(psv4);

               pred   = c7x::strm_agen<0, vec>::get_vpred();
               pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
               __vstore_pred(pred, pStore, vs1_0);

               pred   = c7x::strm_agen<1, vec>::get_vpred();
               pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
               __vstore_pred(pred, pStore, vs1_1);

               pred   = c7x::strm_agen<0, vec>::get_vpred();
               pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
               __vstore_pred(pred, pStore, vs2_0);

               pred   = c7x::strm_agen<1, vec>::get_vpred();
               pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
               __vstore_pred(pred, pStore, vs2_1);

               pred   = c7x::strm_agen<0, vec>::get_vpred();
               pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
               __vstore_pred(pred, pStore, vs3_0);

               pred   = c7x::strm_agen<1, vec>::get_vpred();
               pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
               __vstore_pred(pred, pStore, vs3_1);

               pred   = c7x::strm_agen<0, vec>::get_vpred();
               pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
               __vstore_pred(pred, pStore, vs4_0);

               pred   = c7x::strm_agen<1, vec>::get_vpred();
               pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
               __vstore_pred(pred, pStore, vs4_1);
            }
            v2_0 = c7x::strm_eng<0, vec>::get_adv();
            v2_1 = c7x::strm_eng<1, vec>::get_adv();

            v3_0 = c7x::strm_eng<0, vec>::get_adv();
            v3_1 = c7x::strm_eng<1, vec>::get_adv();

            v4_0 = c7x::strm_eng<0, vec>::get_adv();
            v4_1 = c7x::strm_eng<1, vec>::get_adv();

            v5_0 = c7x::strm_eng<0, vec>::get_adv();
            v5_1 = c7x::strm_eng<1, vec>::get_adv();

            vs1_0    = (v2_0 * sv1) + (v1_0 * cv1);
            temps2_0 = (v2_0 * cv1) - (v1_0 * sv1);
            vs1_1    = (v2_1 * sv1) + (v1_1 * cv1);
            temps2_1 = (v2_1 * cv1) - (v1_1 * sv1);

            vs2_0    = (v3_0 * sv2) + (temps2_0 * cv2);
            temps3_0 = (v3_0 * cv2) - (temps2_0 * sv2);
            vs2_1    = (v3_1 * sv2) + (temps2_1 * cv2);
            temps3_1 = (v3_1 * cv2) - (temps2_1 * sv2);

            vs3_0    = (v4_0 * sv3) + (temps3_0 * cv3);
            temps4_0 = (v4_0 * cv3) - (temps3_0 * sv3);
            vs3_1    = (v4_1 * sv3) + (temps3_1 * cv3);
            temps4_1 = (v4_1 * cv3) - (temps3_1 * sv3);

            vs4_0 = (v5_0 * sv4) + (temps4_0 * cv4);
            v1_0  = (v5_0 * cv4) - (temps4_0 * sv4);
            vs4_1 = (v5_1 * sv4) + (temps4_1 * cv4);
            v1_1  = (v5_1 * cv4) - (temps4_1 * sv4);

            pred   = c7x::strm_agen<0, vec>::get_vpred();
            pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
            __vstore_pred(pred, pStore, vs1_0);

            pred   = c7x::strm_agen<1, vec>::get_vpred();
            pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
            __vstore_pred(pred, pStore, vs1_1);

            pred   = c7x::strm_agen<0, vec>::get_vpred();
            pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
            __vstore_pred(pred, pStore, vs2_0);

            pred   = c7x::strm_agen<1, vec>::get_vpred();
            pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
            __vstore_pred(pred, pStore, vs2_1);

            pred   = c7x::strm_agen<0, vec>::get_vpred();
            pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
            __vstore_pred(pred, pStore, vs3_0);

            pred   = c7x::strm_agen<1, vec>::get_vpred();
            pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
            __vstore_pred(pred, pStore, vs3_1);

            pred   = c7x::strm_agen<0, vec>::get_vpred();
            pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
            __vstore_pred(pred, pStore, vs4_0);

            pred   = c7x::strm_agen<1, vec>::get_vpred();
            pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
            __vstore_pred(pred, pStore, vs4_1);

            vertical += 4;
         }

         for (; vertical < rowPair - 1; vertical += 2) {
            dataType *pcv1 = c7x::strm_agen<2, dataType>::get_adv(cV);
            vec       cv1  = __vload_dup(pcv1);
            dataType *psv1 = c7x::strm_agen<3, dataType>::get_adv(sV);
            vec       sv1  = __vload_dup(psv1);
            dataType *pcv2 = c7x::strm_agen<2, dataType>::get_adv(cV);
            vec       cv2  = __vload_dup(pcv2);
            dataType *psv2 = c7x::strm_agen<3, dataType>::get_adv(sV);
            vec       sv2  = __vload_dup(psv2);

            vec v2_0 = c7x::strm_eng<0, vec>::get_adv();
            vec v2_1 = c7x::strm_eng<1, vec>::get_adv();

            vec v3_0 = c7x::strm_eng<0, vec>::get_adv();
            vec v3_1 = c7x::strm_eng<1, vec>::get_adv();

            vec vs1_0    = (v2_0 * sv1) + (v1_0 * cv1);
            vec temps2_0 = (v2_0 * cv1) - (v1_0 * sv1);
            vec vs1_1    = (v2_1 * sv1) + (v1_1 * cv1);
            vec temps2_1 = (v2_1 * cv1) - (v1_1 * sv1);

            vec vs2_0 = (v3_0 * sv2) + (temps2_0 * cv2);
            v1_0      = (v3_0 * cv2) - (temps2_0 * sv2);
            vec vs2_1 = (v3_1 * sv2) + (temps2_1 * cv2);
            v1_1      = (v3_1 * cv2) - (temps2_1 * sv2);

            pred   = c7x::strm_agen<0, vec>::get_vpred();
            pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
            __vstore_pred(pred, pStore, vs1_0);

            pred   = c7x::strm_agen<1, vec>::get_vpred();
            pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
            __vstore_pred(pred, pStore, vs1_1);

            pred   = c7x::strm_agen<0, vec>::get_vpred();
            pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
            __vstore_pred(pred, pStore, vs2_0);

            pred   = c7x::strm_agen<1, vec>::get_vpred();
            pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
            __vstore_pred(pred, pStore, vs2_1);
         }

         if (vertical != rowPair) {
            dataType *pcv1 = c7x::strm_agen<2, dataType>::get_adv(cV);
            vec       cv1  = __vload_dup(pcv1);
            dataType *psv1 = c7x::strm_agen<3, dataType>::get_adv(sV);
            vec       sv1  = __vload_dup(psv1);

            vec v2_0 = c7x::strm_eng<0, vec>::get_adv();
            vec v2_1 = c7x::strm_eng<1, vec>::get_adv();

            vec vs1_0 = (v2_0 * sv1) + (v1_0 * cv1);
            v1_0      = (v2_0 * cv1) - (v1_0 * sv1);
            vec vs1_1 = (v2_1 * sv1) + (v1_1 * cv1);
            v1_1      = (v2_1 * cv1) - (v1_1 * sv1);

            pred   = c7x::strm_agen<0, vec>::get_vpred();
            pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
            __vstore_pred(pred, pStore, vs1_0);

            pred   = c7x::strm_agen<1, vec>::get_vpred();
            pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
            __vstore_pred(pred, pStore, vs1_1);
         }

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
         __vstore_pred(pred, pStore, v1_0);

         pred   = c7x::strm_agen<1, vec>::get_vpred();
         pStore = c7x::strm_agen<1, vec>::get_adv(vStart + eleCount);
         __vstore_pred(pred, pStore, v1_1);
      }
   }

   if (se0ICNT2 != se1ICNT2) {
      vec     v1_0 = c7x::strm_eng<0, vec>::get_adv();
      __vpred pred;
      vec    *pStore;
      int32_t vertical = 0;

      for (vertical = 0; vertical < rowPair - 3; vertical += 4) {
         dataType *pcv1 = c7x::strm_agen<2, dataType>::get_adv(cV);
         vec       cv1  = __vload_dup(pcv1);
         dataType *psv1 = c7x::strm_agen<3, dataType>::get_adv(sV);
         vec       sv1  = __vload_dup(psv1);
         dataType *pcv2 = c7x::strm_agen<2, dataType>::get_adv(cV);
         vec       cv2  = __vload_dup(pcv2);
         dataType *psv2 = c7x::strm_agen<3, dataType>::get_adv(sV);
         vec       sv2  = __vload_dup(psv2);
         dataType *pcv3 = c7x::strm_agen<2, dataType>::get_adv(cV);
         vec       cv3  = __vload_dup(pcv3);
         dataType *psv3 = c7x::strm_agen<3, dataType>::get_adv(sV);
         vec       sv3  = __vload_dup(psv3);
         dataType *pcv4 = c7x::strm_agen<2, dataType>::get_adv(cV);
         vec       cv4  = __vload_dup(pcv4);
         dataType *psv4 = c7x::strm_agen<3, dataType>::get_adv(sV);
         vec       sv4  = __vload_dup(psv4);

         vec v2_0 = c7x::strm_eng<0, vec>::get_adv();
         vec v3_0 = c7x::strm_eng<0, vec>::get_adv();
         vec v4_0 = c7x::strm_eng<0, vec>::get_adv();
         vec v5_0 = c7x::strm_eng<0, vec>::get_adv();

         vec vs1_0    = (v2_0 * sv1) + (v1_0 * cv1);
         vec temps2_0 = (v2_0 * cv1) - (v1_0 * sv1);

         vec vs2_0    = (v3_0 * sv2) + (temps2_0 * cv2);
         vec temps3_0 = (v3_0 * cv2) - (temps2_0 * sv2);

         vec vs3_0    = (v4_0 * sv3) + (temps3_0 * cv3);
         vec temps4_0 = (v4_0 * cv3) - (temps3_0 * sv3);

         vec vs4_0 = (v5_0 * sv4) + (temps4_0 * cv4);
         v1_0      = (v5_0 * cv4) - (temps4_0 * sv4);

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
         __vstore_pred(pred, pStore, vs1_0);

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
         __vstore_pred(pred, pStore, vs2_0);

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
         __vstore_pred(pred, pStore, vs3_0);

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
         __vstore_pred(pred, pStore, vs4_0);
      }

      for (; vertical < rowPair - 1; vertical += 2) {
         dataType *pcv1 = c7x::strm_agen<2, dataType>::get_adv(cV);
         vec       cv1  = __vload_dup(pcv1);
         dataType *psv1 = c7x::strm_agen<3, dataType>::get_adv(sV);
         vec       sv1  = __vload_dup(psv1);
         dataType *pcv2 = c7x::strm_agen<2, dataType>::get_adv(cV);
         vec       cv2  = __vload_dup(pcv2);
         dataType *psv2 = c7x::strm_agen<3, dataType>::get_adv(sV);
         vec       sv2  = __vload_dup(psv2);

         vec v2_0 = c7x::strm_eng<0, vec>::get_adv();
         vec v3_0 = c7x::strm_eng<0, vec>::get_adv();

         vec vs1_0    = (v2_0 * sv1) + (v1_0 * cv1);
         vec temps2_0 = (v2_0 * cv1) - (v1_0 * sv1);

         vec vs2_0 = (v3_0 * sv2) + (temps2_0 * cv2);
         v1_0      = (v3_0 * cv2) - (temps2_0 * sv2);

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
         __vstore_pred(pred, pStore, vs1_0);

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
         __vstore_pred(pred, pStore, vs2_0);
      }

      if (vertical != rowPair) {
         dataType *pcv1 = c7x::strm_agen<2, dataType>::get_adv(cV);
         vec       cv1  = __vload_dup(pcv1);
         dataType *psv1 = c7x::strm_agen<3, dataType>::get_adv(sV);
         vec       sv1  = __vload_dup(psv1);

         vec v2_0 = c7x::strm_eng<0, vec>::get_adv();

         vec vs1_0 = (v2_0 * sv1) + (v1_0 * cv1);
         v1_0      = (v2_0 * cv1) - (v1_0 * sv1);

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
         __vstore_pred(pred, pStore, vs1_0);
      }

      pred   = c7x::strm_agen<0, vec>::get_vpred();
      pStore = c7x::strm_agen<0, vec>::get_adv(vStart);
      __vstore_pred(pred, pStore, v1_0);
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_diag_proc_ci<float>(float   *V,
                                         int32_t  startRow,
                                         int32_t  Nrows,
                                         int32_t  Ncols,
                                         int32_t  rowVStride,
                                         float   *cV,
                                         float   *sV,
                                         uint8_t *pBlock);
template void DSPLIB_diag_proc_ci<double>(double  *V,
                                          int32_t  startRow,
                                          int32_t  Nrows,
                                          int32_t  Ncols,
                                          int32_t  rowVStride,
                                          double  *cV,
                                          double  *sV,
                                          uint8_t *pBlock);

/*!
 *  @brief Updates rows of U' based on the precalculated cU and sU vectors.
 */
template <typename dataType>
void DSPLIB_diag_rotation_proc_ci(dataType *U,
                                  int32_t   startRow,
                                  int32_t   Nrows,
                                  int32_t   Ncols,
                                  int32_t   rowUStride,
                                  dataType *cU,
                                  dataType *sU,
                                  uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 se0Params; /* For First Row */
   __SA_TEMPLATE_v1 sa0Params;
   __SE_TEMPLATE_v1 se1Params; /* For Other Rows */
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params; /* For cU */
   __SA_TEMPLATE_v1 sa3Params; /* For sU */

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));

   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));

   int32_t lenTile = 8;
   int32_t nTile   = DSPLIB_ceilingDiv(Ncols, (lenTile * eleCount));

   se0Params.ICNT0 = Ncols;
   se0Params.ICNT1 = 1;

   sa0Params.ICNT0 = Ncols;
   sa0Params.ICNT1 = 1;

   se1Params.ICNT1         = Nrows;
   se1Params.DIM1          = rowUStride;
   se1Params.ICNT2         = nTile;
   se1Params.ICNT3         = 1;
   se1Params.DECDIM1_WIDTH = Ncols;

   sa1Params.ICNT1         = Nrows;
   sa1Params.DIM1          = rowUStride;
   sa1Params.ICNT2         = nTile;
   sa1Params.DECDIM1_WIDTH = Ncols;

   sa2Params.ICNT0 = Nrows;
   sa2Params.ICNT1 = nTile;

   sa3Params.ICNT0 = Nrows;
   sa3Params.ICNT1 = nTile;

   dataType *pSE0 = U;
   dataType *pSE1 = U + rowUStride;
   __SE0_OPEN(pSE0, se0Params);
   __SE1_OPEN(pSE1, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);
   __SA2_OPEN(sa2Params);
   __SA3_OPEN(sa3Params);

   for (int32_t tile = 0; tile < nTile; tile++) {
      vec r1 = c7x::strm_eng<0, vec>::get_adv();
      vec r2 = c7x::strm_eng<0, vec>::get_adv();
      vec r3 = c7x::strm_eng<0, vec>::get_adv();
      vec r4 = c7x::strm_eng<0, vec>::get_adv();
      vec r5 = c7x::strm_eng<0, vec>::get_adv();
      vec r6 = c7x::strm_eng<0, vec>::get_adv();
      vec r7 = c7x::strm_eng<0, vec>::get_adv();
      vec r8 = c7x::strm_eng<0, vec>::get_adv();

      dataType *pcU = c7x::strm_agen<2, dataType>::get_adv(cU);
      vec       vcU = __vload_dup(pcU);
      dataType *psU = c7x::strm_agen<3, dataType>::get_adv(sU);
      vec       vsU = __vload_dup(psU);
      for (int32_t vertical = 0; vertical < Nrows; vertical++) {
         vec v1 = c7x::strm_eng<1, vec>::get_adv();
         vec v2 = c7x::strm_eng<1, vec>::get_adv();
         vec v3 = c7x::strm_eng<1, vec>::get_adv();
         vec v4 = c7x::strm_eng<1, vec>::get_adv();
         vec v5 = c7x::strm_eng<1, vec>::get_adv();
         vec v6 = c7x::strm_eng<1, vec>::get_adv();
         vec v7 = c7x::strm_eng<1, vec>::get_adv();
         vec v8 = c7x::strm_eng<1, vec>::get_adv();

         vec vs1 = v1 * vcU - r1 * vsU;
         r1      = v1 * vsU + r1 * vcU;

         vec vs2 = v2 * vcU - r2 * vsU;
         r2      = v2 * vsU + r2 * vcU;

         vec vs3 = v3 * vcU - r3 * vsU;
         r3      = v3 * vsU + r3 * vcU;

         vec vs4 = v4 * vcU - r4 * vsU;
         r4      = v4 * vsU + r4 * vcU;

         vec vs5 = v5 * vcU - r5 * vsU;
         r5      = v5 * vsU + r5 * vcU;

         vec vs6 = v6 * vcU - r6 * vsU;
         r6      = v6 * vsU + r6 * vcU;

         vec vs7 = v7 * vcU - r7 * vsU;
         r7      = v7 * vsU + r7 * vcU;

         vec vs8 = v8 * vcU - r8 * vsU;
         r8      = v8 * vsU + r8 * vcU;

         pcU = c7x::strm_agen<2, dataType>::get_adv(cU);
         vcU = __vload_dup(pcU);
         psU = c7x::strm_agen<3, dataType>::get_adv(sU);
         vsU = __vload_dup(psU);

         __vpred pred   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStore = c7x::strm_agen<1, vec>::get_adv(pSE1);
         __vstore_pred(pred, pStore, vs1);

         pred   = c7x::strm_agen<1, vec>::get_vpred();
         pStore = c7x::strm_agen<1, vec>::get_adv(pSE1);
         __vstore_pred(pred, pStore, vs2);

         pred   = c7x::strm_agen<1, vec>::get_vpred();
         pStore = c7x::strm_agen<1, vec>::get_adv(pSE1);
         __vstore_pred(pred, pStore, vs3);

         pred   = c7x::strm_agen<1, vec>::get_vpred();
         pStore = c7x::strm_agen<1, vec>::get_adv(pSE1);
         __vstore_pred(pred, pStore, vs4);

         pred   = c7x::strm_agen<1, vec>::get_vpred();
         pStore = c7x::strm_agen<1, vec>::get_adv(pSE1);
         __vstore_pred(pred, pStore, vs5);

         pred   = c7x::strm_agen<1, vec>::get_vpred();
         pStore = c7x::strm_agen<1, vec>::get_adv(pSE1);
         __vstore_pred(pred, pStore, vs6);

         pred   = c7x::strm_agen<1, vec>::get_vpred();
         pStore = c7x::strm_agen<1, vec>::get_adv(pSE1);
         __vstore_pred(pred, pStore, vs7);

         pred   = c7x::strm_agen<1, vec>::get_vpred();
         pStore = c7x::strm_agen<1, vec>::get_adv(pSE1);
         __vstore_pred(pred, pStore, vs8);
      }
      __vpred pred1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStore1 = c7x::strm_agen<0, vec>::get_adv(pSE0);
      __vstore_pred(pred1, pStore1, r1);

      pred1   = c7x::strm_agen<0, vec>::get_vpred();
      pStore1 = c7x::strm_agen<0, vec>::get_adv(pSE0);
      __vstore_pred(pred1, pStore1, r2);

      pred1   = c7x::strm_agen<0, vec>::get_vpred();
      pStore1 = c7x::strm_agen<0, vec>::get_adv(pSE0);
      __vstore_pred(pred1, pStore1, r3);

      pred1   = c7x::strm_agen<0, vec>::get_vpred();
      pStore1 = c7x::strm_agen<0, vec>::get_adv(pSE0);
      __vstore_pred(pred1, pStore1, r4);

      pred1   = c7x::strm_agen<0, vec>::get_vpred();
      pStore1 = c7x::strm_agen<0, vec>::get_adv(pSE0);
      __vstore_pred(pred1, pStore1, r5);

      pred1   = c7x::strm_agen<0, vec>::get_vpred();
      pStore1 = c7x::strm_agen<0, vec>::get_adv(pSE0);
      __vstore_pred(pred1, pStore1, r6);

      pred1   = c7x::strm_agen<0, vec>::get_vpred();
      pStore1 = c7x::strm_agen<0, vec>::get_adv(pSE0);
      __vstore_pred(pred1, pStore1, r7);

      pred1   = c7x::strm_agen<0, vec>::get_vpred();
      pStore1 = c7x::strm_agen<0, vec>::get_adv(pSE0);
      __vstore_pred(pred1, pStore1, r8);
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_diag_rotation_proc_ci<float>(float   *U,
                                                  int32_t  startRow,
                                                  int32_t  Nrows,
                                                  int32_t  Ncols,
                                                  int32_t  rowUStride,
                                                  float   *cU,
                                                  float   *sU,
                                                  uint8_t *pBlock);
template void DSPLIB_diag_rotation_proc_ci<double>(double  *U,
                                                   int32_t  startRow,
                                                   int32_t  Nrows,
                                                   int32_t  Ncols,
                                                   int32_t  rowUStride,
                                                   double  *cU,
                                                   double  *sU,
                                                   uint8_t *pBlock);
