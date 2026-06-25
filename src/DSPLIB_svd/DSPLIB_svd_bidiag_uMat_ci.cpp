// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_priv.h"

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */

template <typename dataType> void DSPLIB_bidiag_u_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_svd_PrivArgs *pKerPrivArgs = (DSPLIB_svd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              strideU      = pKerPrivArgs->strideU;
   int32_t              dataSize     = sizeof(dataType);

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se2Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa2Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa3Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa4Params = __gen_SA_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se3Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa5Params = __gen_SA_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;

   int32_t colUStride = strideU / sizeof(dataType);

   se0Params.ICNT0   = 1;
   se0Params.ICNT1   = eleCount;
   se0Params.DIM1    = colUStride;
   se0Params.DIM2    = eleCount * colUStride;
   se0Params.ICNT3   = 1;
   se0Params.DIM3    = 0;
   se0Params.DIMFMT  = __SE_DIMFMT_4D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;

   if (dataSize == 4) {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   sa0Params.ICNT1  = 1;
   sa0Params.DIM1   = 0;
   sa0Params.DIMFMT = __SA_DIMFMT_2D;
   sa0Params.VECLEN = SA_VECLEN;

   se1Params.ICNT1   = 3;
   se1Params.DIM1    = 0;
   se1Params.DIMFMT  = __SE_DIMFMT_2D;
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN  = SE_VECLEN;

   int32_t lenTile   = 8;
   se2Params.ICNT0   = eleCount * lenTile;
   se2Params.DIM1    = colUStride * 2;
   se2Params.DIM2    = eleCount * lenTile;
   se2Params.ICNT3   = 2;
   se2Params.DIM3    = 0;
   se2Params.DIMFMT  = __SE_DIMFMT_4D;
   se2Params.ELETYPE = SE_ELETYPE;
   se2Params.VECLEN  = SE_VECLEN;
   se2Params.DECDIM1 = __SE_DECDIM_DIM2;

   sa2Params.ICNT0   = eleCount * lenTile;
   sa2Params.DIM1    = colUStride * 2;
   sa2Params.DIM2    = eleCount * lenTile;
   sa2Params.DIMFMT  = __SA_DIMFMT_3D;
   sa2Params.VECLEN  = SA_VECLEN;
   sa2Params.DECDIM1 = __SA_DECDIM_DIM2;

   sa3Params.DIM1   = 0;
   sa3Params.DIMFMT = __SA_DIMFMT_2D;
   sa3Params.VECLEN = __SA_VECLEN_1ELEM;

   sa4Params.ICNT0   = eleCount;
   sa4Params.DIM1    = eleCount;
   sa4Params.ICNT2   = 2;
   sa4Params.DIM2    = 0;
   sa4Params.DIMFMT  = __SA_DIMFMT_3D;
   sa4Params.VECLEN  = SA_VECLEN;
   sa4Params.DECDIM1 = __SA_DECDIM_DIM1;

   int32_t rowBlock  = 8;
   se3Params.ICNT0   = eleCount;
   se3Params.ICNT1   = rowBlock;
   se3Params.DIM1    = colUStride;
   se3Params.DIM2    = eleCount;
   se3Params.DIM3    = 2 * rowBlock * colUStride;
   se3Params.DIMFMT  = __SE_DIMFMT_4D;
   se3Params.ELETYPE = SE_ELETYPE;
   se3Params.VECLEN  = SE_VECLEN;
   se3Params.DECDIM2 = __SE_DECDIM_DIM2;

   sa5Params.ICNT0   = eleCount;
   sa5Params.ICNT1   = rowBlock;
   sa5Params.DIM1    = colUStride;
   sa5Params.DIM2    = eleCount;
   sa5Params.DIM3    = 2 * rowBlock * colUStride;
   sa5Params.DIMFMT  = __SA_DIMFMT_4D;
   sa5Params.VECLEN  = SA_VECLEN;
   sa5Params.DECDIM2 = __SA_DECDIM_DIM2;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE)) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE)) = sa0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = se1Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = se2Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE)) = sa2Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE)) = sa3Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE)) = sa4Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE)) = se3Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE)) = sa5Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_bidiag_u_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_bidiag_u_init_ci<double>(DSPLIB_kernelHandle handle);

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */

/*!
 *  @brief       This function calculates the half-norms corresponding to the column of input matrix
 *               U and returns scale.
 */
template <typename dataType>
dataType DSPLIB_bidiag_uCol_halfnorm_ci(dataType *U,
                                        int32_t   Nrows,
                                        int32_t   Ncols,
                                        int32_t   colUStride,
                                        dataType *half_norm_squared,
                                        dataType *U1,
                                        dataType *s,
                                        uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params, se1Params;
   __SA_TEMPLATE_v1 sa0Params, sa1Params;
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   int32_t nVec         = Nrows / eleCount;
   int32_t se0ICNT2     = nVec / 2;
   int32_t se1ICNT2     = nVec - se0ICNT2;
   int32_t remainingEle = Nrows - (nVec * eleCount);
   /* ******************************** CALCULATE COL SCALE *********************************
    *************************************************************************************** */

   se0Params.ICNT2 = se0ICNT2;
   se0Params.ICNT3 = 2;

   se1Params.ICNT2 = se1ICNT2;
   se1Params.ICNT3 = 2;

   sa0Params.ICNT0 = se0ICNT2 * eleCount;
   sa1Params.ICNT0 = Nrows - (se0ICNT2 * eleCount);

   vec acc1, acc2, acc3, acc4, acc5, acc6;
   acc1 = acc2 = acc3 = acc4 = acc5 = acc6 = (vec) 0;

   dataType *pSE0 = U;
   dataType *pSE1 = U + (se0ICNT2 * colUStride * eleCount);

   if (se1ICNT2 > 0) {
      __SE1_OPEN(pSE1, se1Params);
   }
   if (se0ICNT2 > 0) {
      __SE0_OPEN(pSE0, se0Params);
   }
   int32_t iterloop1 = se0ICNT2 / 3;
   int32_t vertical  = iterloop1 * 3;
   for (int32_t iter = 0; iter < iterloop1; iter++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<1, vec>::get_adv();
      vec v5 = c7x::strm_eng<0, vec>::get_adv();
      vec v6 = c7x::strm_eng<1, vec>::get_adv();

      acc1 += __abs(v1);
      acc2 += __abs(v2);
      acc3 += __abs(v3);
      acc4 += __abs(v4);
      acc5 += __abs(v5);
      acc6 += __abs(v6);
   }
   for (; vertical < se0ICNT2 - 1; vertical += 2) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<1, vec>::get_adv();

      acc1 += __abs(v1);
      acc2 += __abs(v2);
      acc3 += __abs(v3);
      acc4 += __abs(v4);
   }
   for (; vertical < se0ICNT2; vertical++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();

      acc1 += __abs(v1);
      acc2 += __abs(v2);
   }

   if (se0ICNT2 != se1ICNT2) {
      vec v1 = c7x::strm_eng<1, vec>::get_adv();

      acc1 += __abs(v1);
   }

   acc1 = acc1 + acc2;
   acc3 = acc3 + acc4;
   acc5 = acc5 + acc6;
   acc1 = acc1 + acc3 + acc5;

   dataType scale = 0;
   c7x_horizontal_add(acc1, &scale);

   if (remainingEle > 0) {

      dataType *remU = U + (nVec * eleCount * colUStride);
      for (int32_t i = 0; i < remainingEle; i++) {
         scale += __abs(remU[i * colUStride]);
      }
   }

   /* ******************************** CALCULATE COL S2 *********************************
    *************************************************************************************** */

   if (scale > 0) {

      dataType *normUStore      = U1 + colUStride;
      dataType *pSA0            = normUStore;
      dataType *pSA1            = normUStore + (se0ICNT2 * eleCount);
      dataType  scalarRecip     = getRecip(scale);
      vec       reciprocalScale = (vec) scalarRecip;
      dataType  s2              = 0;

      acc1 = (vec) 0;
      acc2 = (vec) 0;
      acc3 = (vec) 0;
      acc4 = (vec) 0;

      __SA1_OPEN(sa1Params);
      if (se0ICNT2 > 0) {
         __SA0_OPEN(sa0Params);
      }
      vertical = 0;
      for (vertical = 0; vertical < se0ICNT2 - 1; vertical += 2) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec v2 = c7x::strm_eng<1, vec>::get_adv();
         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec v4 = c7x::strm_eng<1, vec>::get_adv();

         v1 = v1 * reciprocalScale;
         v2 = v2 * reciprocalScale;
         v3 = v3 * reciprocalScale;
         v4 = v4 * reciprocalScale;

         acc1 += v1 * v1;
         acc2 += v2 * v2;
         acc3 += v3 * v3;
         acc4 += v4 * v4;

         __vpred pred1   = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pNormU1 = c7x::strm_agen<0, vec>::get_adv(pSA0);
         __vstore_pred(pred1, pNormU1, v1);

         __vpred pred2   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pNormU2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred2, pNormU2, v2);

         __vpred pred3   = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pNormU3 = c7x::strm_agen<0, vec>::get_adv(pSA0);
         __vstore_pred(pred3, pNormU3, v3);

         __vpred pred4   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pNormU4 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred4, pNormU4, v4);
      }

      for (; vertical < se0ICNT2; vertical++) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         v1 = v1 * reciprocalScale;
         v2 = v2 * reciprocalScale;

         acc1 += v1 * v1;
         acc2 += v2 * v2;

         __vpred pred1   = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pNormU1 = c7x::strm_agen<0, vec>::get_adv(pSA0);
         __vstore_pred(pred1, pNormU1, v1);

         __vpred pred2   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pNormU2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred2, pNormU2, v2);
      }

      if (se0ICNT2 != se1ICNT2) {
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         v2 = v2 * reciprocalScale;

         acc2 += v2 * v2;

         __vpred pred2   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pNormU2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred2, pNormU2, v2);
      }

      acc1 = acc1 + acc2;
      acc3 = acc3 + acc4;
      acc1 = acc1 + acc3;
      c7x_horizontal_add(acc1, &s2);

      if (remainingEle > 0) {
         __SE1_CLOSE();
         dataType *remU  = U + (nVec * eleCount * colUStride);
         se1Params.ICNT1 = remainingEle;
         se1Params.ICNT2 = 1;
         __SE1_OPEN(remU, se1Params);
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         v2 = v2 * reciprocalScale;

         vec vn = v2 * v2;

         __vpred pred2   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pNormU2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred2, pNormU2, v2);

         for (int32_t i = 0; i < remainingEle; i++) {
            s2 += vn.s[i];
         }
      }

      dataType diagEle = U[0] * scalarRecip;

      const dataType Half  = 0.5;
      const dataType OneP5 = 1.5;
      dataType       x     = __recip_sqrt(s2);
      x                    = x * (OneP5 - (s2 * x * x * Half));
      x                    = x * (OneP5 - (s2 * x * x * Half));
      dataType y           = s2 * x;

      if (diagEle < 0) {
         *s = y;
      }
      else {
         *s = -y;
      }
      *half_norm_squared = diagEle * (*s) - s2;
      *pSA0              = (*pSA0) - (*s);
   }

   __SE1_CLOSE();
   __SA1_CLOSE();
   if (se0ICNT2 > 0) {
      __SE0_CLOSE();
      __SA0_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return scale;
}
template float  DSPLIB_bidiag_uCol_halfnorm_ci<float>(float   *U,
                                                     int32_t  Nrows,
                                                     int32_t  Ncols,
                                                     int32_t  colUStride,
                                                     float   *half_norm_squared,
                                                     float   *U1,
                                                     float   *s,
                                                     uint8_t *pBlock);
template double DSPLIB_bidiag_uCol_halfnorm_ci<double>(double  *U,
                                                       int32_t  Nrows,
                                                       int32_t  Ncols,
                                                       int32_t  colUStride,
                                                       double  *half_norm_squared,
                                                       double  *U1,
                                                       double  *s,
                                                       uint8_t *pBlock);

/*!
 *  @brief       This function implements the Household processing on columns of input U matrix
 *               corresponding to the natural implementation.
 */
template <typename dataType>
void DSPLIB_bidiag_uCol_ci(dataType *U,
                           int32_t   Nrows,
                           int32_t   Ncols,
                           int32_t   colUStride,
                           dataType  half_norm_squared,
                           dataType *U1,
                           dataType  scale,
                           uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa2Params;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));

   int32_t lenTile  = 8;
   int32_t nTiles   = DSPLIB_ceilingDiv(Ncols, eleCount * lenTile);
   int32_t se1ICNT1 = Nrows / 2;
   int32_t se0ICNT1 = Nrows - se1ICNT1;

   se0Params.ICNT1         = se0ICNT1;
   se0Params.ICNT2         = nTiles;
   se0Params.DECDIM1_WIDTH = Ncols;

   se1Params.ICNT1         = se1ICNT1;
   se1Params.ICNT2         = nTiles;
   se1Params.DECDIM1_WIDTH = Ncols;

   sa2Params.ICNT1         = lenTile * nTiles;
   sa2Params.DECDIM1_WIDTH = Ncols;

   dataType *siStore        = (dataType *) U1;
   dataType *reciprocalLoad = (dataType *) U1 + colUStride;

   vec reciprocalHalfNorm = (vec) getRecip(half_norm_squared);

   __SA_TEMPLATE_v1 sa3Params;
   sa3Params       = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   sa3Params.ICNT0 = Nrows;
   sa3Params.ICNT1 = 2 * nTiles;

   /* ******************************** CALCULATE COL SI **************************************
    *************************************************************************************** */

   __SE0_OPEN(U + 1, se0Params);
   __SA2_OPEN(sa2Params);
   __SA3_OPEN(sa3Params);

   /* if (se1ICNT1 > 0) */ /* if number of rows > 1 */
   {
      __SE1_OPEN(U + 1 + colUStride, se1Params);
   }

   for (int32_t tile = 0; tile < nTiles; tile++) {
      vec acc1, acc2, acc3, acc4, acc5, acc6, acc7, acc8;
      acc1 = acc2 = acc3 = acc4 = acc5 = acc6 = acc7 = acc8 = (vec) 0;
      for (int32_t vertical = 0; vertical < se1ICNT1; vertical++) {
         vec v01 = c7x::strm_eng<0, vec>::get_adv();
         vec v02 = c7x::strm_eng<0, vec>::get_adv();
         vec v03 = c7x::strm_eng<0, vec>::get_adv();
         vec v04 = c7x::strm_eng<0, vec>::get_adv();
         vec v05 = c7x::strm_eng<0, vec>::get_adv();
         vec v06 = c7x::strm_eng<0, vec>::get_adv();
         vec v07 = c7x::strm_eng<0, vec>::get_adv();
         vec v08 = c7x::strm_eng<0, vec>::get_adv();

         vec v11 = c7x::strm_eng<1, vec>::get_adv();
         vec v12 = c7x::strm_eng<1, vec>::get_adv();
         vec v13 = c7x::strm_eng<1, vec>::get_adv();
         vec v14 = c7x::strm_eng<1, vec>::get_adv();
         vec v15 = c7x::strm_eng<1, vec>::get_adv();
         vec v16 = c7x::strm_eng<1, vec>::get_adv();
         vec v17 = c7x::strm_eng<1, vec>::get_adv();
         vec v18 = c7x::strm_eng<1, vec>::get_adv();

         dataType *pU1 = c7x::strm_agen<3, dataType>::get_adv(reciprocalLoad);
         vec       u1  = __vload_dup(pU1);
         dataType *pU2 = c7x::strm_agen<3, dataType>::get_adv(reciprocalLoad);
         vec       u2  = __vload_dup(pU2);

         acc1 += v01 * u1 + v11 * u2;
         acc2 += v02 * u1 + v12 * u2;
         acc3 += v03 * u1 + v13 * u2;
         acc4 += v04 * u1 + v14 * u2;
         acc5 += v05 * u1 + v15 * u2;
         acc6 += v06 * u1 + v16 * u2;
         acc7 += v07 * u1 + v17 * u2;
         acc8 += v08 * u1 + v18 * u2;
      }

      if (se1ICNT1 != se0ICNT1) /* For last odd numbered row */
      {
         vec v01 = c7x::strm_eng<0, vec>::get_adv();
         vec v02 = c7x::strm_eng<0, vec>::get_adv();
         vec v03 = c7x::strm_eng<0, vec>::get_adv();
         vec v04 = c7x::strm_eng<0, vec>::get_adv();
         vec v05 = c7x::strm_eng<0, vec>::get_adv();
         vec v06 = c7x::strm_eng<0, vec>::get_adv();
         vec v07 = c7x::strm_eng<0, vec>::get_adv();
         vec v08 = c7x::strm_eng<0, vec>::get_adv();

         dataType *pU1 = c7x::strm_agen<3, dataType>::get_adv(reciprocalLoad);
         vec       u1  = __vload_dup(pU1);

         acc1 += v01 * u1;
         acc2 += v02 * u1;
         acc3 += v03 * u1;
         acc4 += v04 * u1;
         acc5 += v05 * u1;
         acc6 += v06 * u1;
         acc7 += v07 * u1;
         acc8 += v08 * u1;
      }

      __vpred pred      = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
      __vstore_pred(pred, pStoreVec, acc1 * reciprocalHalfNorm);

      pred      = c7x::strm_agen<2, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
      __vstore_pred(pred, pStoreVec, acc2 * reciprocalHalfNorm);

      pred      = c7x::strm_agen<2, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
      __vstore_pred(pred, pStoreVec, acc3 * reciprocalHalfNorm);

      pred      = c7x::strm_agen<2, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
      __vstore_pred(pred, pStoreVec, acc4 * reciprocalHalfNorm);

      pred      = c7x::strm_agen<2, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
      __vstore_pred(pred, pStoreVec, acc5 * reciprocalHalfNorm);

      pred      = c7x::strm_agen<2, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
      __vstore_pred(pred, pStoreVec, acc6 * reciprocalHalfNorm);

      pred      = c7x::strm_agen<2, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
      __vstore_pred(pred, pStoreVec, acc7 * reciprocalHalfNorm);

      pred      = c7x::strm_agen<2, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
      __vstore_pred(pred, pStoreVec, acc8 * reciprocalHalfNorm);
   }

   /* ******************************** UPDATE COL ******************************************
    *************************************************************************************** */
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));

   sa0Params.ICNT1         = se0ICNT1;
   sa0Params.ICNT2         = nTiles;
   sa0Params.DECDIM1_WIDTH = Ncols;

   sa1Params.ICNT1         = se1ICNT1;
   sa1Params.ICNT2         = nTiles;
   sa1Params.DECDIM1_WIDTH = Ncols;

   __SA0_OPEN(sa0Params);
   /* if (se1ICNT1 > 0) */ {
      __SA1_OPEN(sa1Params);
   }
   for (int32_t tile = 0; tile < nTiles; tile++) {
      __vpred pred = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pSi  = c7x::strm_agen<2, vec>::get_adv(siStore);
      vec     si1  = __vload_pred(pred, pSi);

      pred    = c7x::strm_agen<2, vec>::get_vpred();
      pSi     = c7x::strm_agen<2, vec>::get_adv(siStore);
      vec si2 = __vload_pred(pred, pSi);

      pred    = c7x::strm_agen<2, vec>::get_vpred();
      pSi     = c7x::strm_agen<2, vec>::get_adv(siStore);
      vec si3 = __vload_pred(pred, pSi);

      pred    = c7x::strm_agen<2, vec>::get_vpred();
      pSi     = c7x::strm_agen<2, vec>::get_adv(siStore);
      vec si4 = __vload_pred(pred, pSi);

      pred    = c7x::strm_agen<2, vec>::get_vpred();
      pSi     = c7x::strm_agen<2, vec>::get_adv(siStore);
      vec si5 = __vload_pred(pred, pSi);

      pred    = c7x::strm_agen<2, vec>::get_vpred();
      pSi     = c7x::strm_agen<2, vec>::get_adv(siStore);
      vec si6 = __vload_pred(pred, pSi);

      pred    = c7x::strm_agen<2, vec>::get_vpred();
      pSi     = c7x::strm_agen<2, vec>::get_adv(siStore);
      vec si7 = __vload_pred(pred, pSi);

      pred    = c7x::strm_agen<2, vec>::get_vpred();
      pSi     = c7x::strm_agen<2, vec>::get_adv(siStore);
      vec si8 = __vload_pred(pred, pSi);

      /* if (Nrows >= 2) */ {
         dataType *pU1 = c7x::strm_agen<3, dataType>::get_adv(reciprocalLoad);
         vec       u1  = __vload_dup(pU1);
         dataType *pU2 = c7x::strm_agen<3, dataType>::get_adv(reciprocalLoad);
         vec       u2  = __vload_dup(pU2);
         for (int32_t vertical = 0; vertical < se1ICNT1 - 1; vertical++) {
            vec v01 = c7x::strm_eng<0, vec>::get_adv();
            vec v02 = c7x::strm_eng<0, vec>::get_adv();
            vec v03 = c7x::strm_eng<0, vec>::get_adv();
            vec v04 = c7x::strm_eng<0, vec>::get_adv();
            vec v05 = c7x::strm_eng<0, vec>::get_adv();
            vec v06 = c7x::strm_eng<0, vec>::get_adv();
            vec v07 = c7x::strm_eng<0, vec>::get_adv();
            vec v08 = c7x::strm_eng<0, vec>::get_adv();

            vec v11 = c7x::strm_eng<1, vec>::get_adv();
            vec v12 = c7x::strm_eng<1, vec>::get_adv();
            vec v13 = c7x::strm_eng<1, vec>::get_adv();
            vec v14 = c7x::strm_eng<1, vec>::get_adv();
            vec v15 = c7x::strm_eng<1, vec>::get_adv();
            vec v16 = c7x::strm_eng<1, vec>::get_adv();
            vec v17 = c7x::strm_eng<1, vec>::get_adv();
            vec v18 = c7x::strm_eng<1, vec>::get_adv();

            vec ele1 = u1;
            vec ele2 = u2;

            v01 += ele1 * si1;
            v02 += ele1 * si2;
            v03 += ele1 * si3;
            v04 += ele1 * si4;
            v05 += ele1 * si5;
            v06 += ele1 * si6;
            v07 += ele1 * si7;
            v08 += ele1 * si8;

            v11 += ele2 * si1;
            v12 += ele2 * si2;
            v13 += ele2 * si3;
            v14 += ele2 * si4;
            v15 += ele2 * si5;
            v16 += ele2 * si6;
            v17 += ele2 * si7;
            v18 += ele2 * si8;

            pU1 = c7x::strm_agen<3, dataType>::get_adv(reciprocalLoad);
            u1  = __vload_dup(pU1);
            pU2 = c7x::strm_agen<3, dataType>::get_adv(reciprocalLoad);
            u2  = __vload_dup(pU2);

            __vpred pred1 = c7x::strm_agen<0, vec>::get_vpred();
            vec    *p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v01);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v02);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v03);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v04);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v05);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v06);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v07);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v08);

            __vpred pred2 = c7x::strm_agen<1, vec>::get_vpred();
            vec    *p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v11);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v12);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v13);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v14);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v15);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v16);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v17);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v18);
         }

         {
            vec v01 = c7x::strm_eng<0, vec>::get_adv();
            vec v02 = c7x::strm_eng<0, vec>::get_adv();
            vec v03 = c7x::strm_eng<0, vec>::get_adv();
            vec v04 = c7x::strm_eng<0, vec>::get_adv();
            vec v05 = c7x::strm_eng<0, vec>::get_adv();
            vec v06 = c7x::strm_eng<0, vec>::get_adv();
            vec v07 = c7x::strm_eng<0, vec>::get_adv();
            vec v08 = c7x::strm_eng<0, vec>::get_adv();

            vec v11 = c7x::strm_eng<1, vec>::get_adv();
            vec v12 = c7x::strm_eng<1, vec>::get_adv();
            vec v13 = c7x::strm_eng<1, vec>::get_adv();
            vec v14 = c7x::strm_eng<1, vec>::get_adv();
            vec v15 = c7x::strm_eng<1, vec>::get_adv();
            vec v16 = c7x::strm_eng<1, vec>::get_adv();
            vec v17 = c7x::strm_eng<1, vec>::get_adv();
            vec v18 = c7x::strm_eng<1, vec>::get_adv();

            vec ele1 = u1;
            vec ele2 = u2;

            v01 += ele1 * si1;
            v02 += ele1 * si2;
            v03 += ele1 * si3;
            v04 += ele1 * si4;
            v05 += ele1 * si5;
            v06 += ele1 * si6;
            v07 += ele1 * si7;
            v08 += ele1 * si8;

            v11 += ele2 * si1;
            v12 += ele2 * si2;
            v13 += ele2 * si3;
            v14 += ele2 * si4;
            v15 += ele2 * si5;
            v16 += ele2 * si6;
            v17 += ele2 * si7;
            v18 += ele2 * si8;

            __vpred pred1 = c7x::strm_agen<0, vec>::get_vpred();
            vec    *p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v01);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v02);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v03);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v04);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v05);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v06);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v07);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, p1, v08);

            __vpred pred2 = c7x::strm_agen<1, vec>::get_vpred();
            vec    *p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v11);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v12);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v13);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v14);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v15);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v16);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v17);

            pred2 = c7x::strm_agen<1, vec>::get_vpred();
            p2    = c7x::strm_agen<1, vec>::get_adv(U + 1 + colUStride);
            __vstore_pred(pred2, p2, v18);
         }
      }

      if (se0ICNT1 != se1ICNT1) {
         vec v01 = c7x::strm_eng<0, vec>::get_adv();
         vec v02 = c7x::strm_eng<0, vec>::get_adv();
         vec v03 = c7x::strm_eng<0, vec>::get_adv();
         vec v04 = c7x::strm_eng<0, vec>::get_adv();
         vec v05 = c7x::strm_eng<0, vec>::get_adv();
         vec v06 = c7x::strm_eng<0, vec>::get_adv();
         vec v07 = c7x::strm_eng<0, vec>::get_adv();
         vec v08 = c7x::strm_eng<0, vec>::get_adv();

         dataType *pU1 = c7x::strm_agen<3, dataType>::get_adv(reciprocalLoad);
         vec       u1  = __vload_dup(pU1);

         v01 += u1 * si1;
         v02 += u1 * si2;
         v03 += u1 * si3;
         v04 += u1 * si4;
         v05 += u1 * si5;
         v06 += u1 * si6;
         v07 += u1 * si7;
         v08 += u1 * si8;

         __vpred pred1 = c7x::strm_agen<0, vec>::get_vpred();
         vec    *p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
         __vstore_pred(pred1, p1, v01);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
         __vstore_pred(pred1, p1, v02);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
         __vstore_pred(pred1, p1, v03);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
         __vstore_pred(pred1, p1, v04);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
         __vstore_pred(pred1, p1, v05);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
         __vstore_pred(pred1, p1, v06);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
         __vstore_pred(pred1, p1, v07);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(U + 1);
         __vstore_pred(pred1, p1, v08);
      }
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
   __SA2_CLOSE();
   __SA3_CLOSE();
   /* if (se1ICNT1 > 0) */ {
      __SE1_CLOSE();
      __SA1_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_bidiag_uCol_ci<float>(float   *U,
                                           int32_t  Nrows,
                                           int32_t  Ncols,
                                           int32_t  colUStride,
                                           float    half_norm_squared,
                                           float   *U1,
                                           float    scale,
                                           uint8_t *pBlock);
template void DSPLIB_bidiag_uCol_ci<double>(double  *U,
                                            int32_t  Nrows,
                                            int32_t  Ncols,
                                            int32_t  colUStride,
                                            double   half_norm_squared,
                                            double  *U1,
                                            double   scale,
                                            uint8_t *pBlock);

/*!
 *  @brief       This function calculates the half-norm corresponding to the row of input matrix
 *               U and returns scale.
 */
template <typename dataType>
dataType DSPLIB_bidiag_uRow_halfnorm_ci(dataType *U,
                                        int32_t   Nrows,
                                        int32_t   Ncols,
                                        int32_t   colUStride,
                                        dataType *half_norm_squared,
                                        dataType *U1,
                                        dataType *s,
                                        dataType *superdiag,
                                        uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 se0Params, se1Params;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   int32_t nVec = DSPLIB_ceilingDiv(Ncols, eleCount);

   /* Calculating loopCount for 12 vector loads */
   int32_t iterloop1    = nVec / 12;
   int32_t remainingVec = nVec - (iterloop1 * 12);

   /* Calculating loopCount for 4 vector loads */
   int32_t iterloop2 = remainingVec / 4;
   remainingVec      = remainingVec - (iterloop2 * 4);

   /* Calculating loopCount for remaining vectors */
   int32_t iterloop3 = remainingVec / 2;

   int32_t se0Iter = nVec / 2;
   int32_t se1Iter = nVec - se0Iter;

   int32_t se0ICNT0 = se0Iter * eleCount;
   int32_t se1ICNT0 = Ncols - se0ICNT0;

   se0Params.ICNT0 = se0ICNT0;
   se1Params.ICNT0 = se1ICNT0;

   /* ******************************** CALCULATE ROW SCALE **************************************
    *************************************************************************************** */
   // vec acc = (vec)0;
   vec acc1 = (vec) 0;
   vec acc2 = (vec) 0;
   vec acc3 = (vec) 0;
   vec acc4 = (vec) 0;
   vec acc5 = (vec) 0;
   vec acc6 = (vec) 0;

   __SE1_OPEN(&U[se0ICNT0], se1Params);
   if (se0ICNT0 > 0) {
      __SE0_OPEN(U, se0Params);
   }
   int32_t iter = 0;
   for (iter = 0; iter < iterloop1; iter++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<0, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<0, vec>::get_adv();
      vec v5 = c7x::strm_eng<0, vec>::get_adv();
      vec v6 = c7x::strm_eng<0, vec>::get_adv();

      vec v7  = c7x::strm_eng<1, vec>::get_adv();
      vec v8  = c7x::strm_eng<1, vec>::get_adv();
      vec v9  = c7x::strm_eng<1, vec>::get_adv();
      vec v10 = c7x::strm_eng<1, vec>::get_adv();
      vec v11 = c7x::strm_eng<1, vec>::get_adv();
      vec v12 = c7x::strm_eng<1, vec>::get_adv();

      acc1 += __abs(v1);
      acc2 += __abs(v2);
      acc3 += __abs(v3);
      acc4 += __abs(v4);
      acc5 += __abs(v5);
      acc6 += __abs(v6);

      acc1 += __abs(v7);
      acc2 += __abs(v8);
      acc3 += __abs(v9);
      acc4 += __abs(v10);
      acc5 += __abs(v11);
      acc6 += __abs(v12);
   }

   for (iter = 0; iter < iterloop2; iter++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<0, vec>::get_adv();
      vec v3 = c7x::strm_eng<1, vec>::get_adv();
      vec v4 = c7x::strm_eng<1, vec>::get_adv();

      acc1 += __abs(v1);
      acc2 += __abs(v2);
      acc3 += __abs(v3);
      acc4 += __abs(v4);
   }

   for (iter = 0; iter < iterloop3; iter++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();

      acc1 += __abs(v1);
      acc2 += __abs(v2);
   }

   acc1 = acc1 + acc2;
   acc3 = acc3 + acc4;
   acc5 = acc5 + acc6;

   acc1 = acc1 + acc3 + acc5;

   if (se0Iter != se1Iter) {
      vec v = c7x::strm_eng<1, vec>::get_adv();
      acc1 += __abs(v);
   }

   dataType scale = 0;
   c7x_horizontal_add(acc1, &scale);
   if (scale > 0) {
      /* ******************************** CALCULATE ROW S2 **************************************
       *************************************************************************************** */
      dataType *normUStore = U1 + colUStride;
      dataType *pSA0       = normUStore;
      dataType *pSA1       = normUStore + se0ICNT0;

      vec     lastV     = 0;
      int32_t lastIndex = 0;

      /* Calculating loopCount for 4 vector loads */
      iterloop1    = nVec / 4;
      remainingVec = nVec - (iterloop1 * 4);

      /* Calculating loopCount for remaining vectors */
      iterloop2 = remainingVec / 2;

      vec reciprocalScale = (vec) getRecip(scale);
      acc1                = (vec) 0;
      acc2                = (vec) 0;
      acc3                = (vec) 0;
      acc4                = (vec) 0;

      __SA_TEMPLATE_v1 sa0Params, sa1Params;
      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

      sa0Params.ICNT0 = se0ICNT0;
      sa1Params.ICNT0 = se1ICNT0;
      sa0Params.ICNT1 = sa1Params.ICNT1 = 2;

      __SA1_OPEN(sa1Params);
      if (se0ICNT0 > 0) {
         __SA0_OPEN(sa0Params);
      }
      vec v1;
      vec v2;
      vec v3;
      vec v4;
      for (iter = 0; iter < iterloop1; iter++) {
         v1 = c7x::strm_eng<0, vec>::get_adv();
         v2 = c7x::strm_eng<0, vec>::get_adv();
         v3 = c7x::strm_eng<1, vec>::get_adv();
         v4 = c7x::strm_eng<1, vec>::get_adv();

         v1 = v1 * reciprocalScale;
         v2 = v2 * reciprocalScale;
         v3 = v3 * reciprocalScale;
         v4 = v4 * reciprocalScale;

         acc1 += v1 * v1;
         acc2 += v2 * v2;
         acc3 += v3 * v3;
         acc4 += v4 * v4;

         __vpred pred1   = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pNormU1 = c7x::strm_agen<0, vec>::get_adv(pSA0);
         __vstore_pred(pred1, pNormU1, v1);

         __vpred pred2   = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pNormU2 = c7x::strm_agen<0, vec>::get_adv(pSA0);
         __vstore_pred(pred2, pNormU2, v2);

         __vpred pred3   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pNormU3 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred3, pNormU3, v3);

         __vpred pred4   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pNormU4 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred4, pNormU4, v4);
      }

      if (iterloop1 * 4 == nVec) {
         lastIndex = se1ICNT0 - ((se1Iter - 1) * eleCount) - 1;
         lastV     = v4;
      }

      for (iter = 0; iter < iterloop2; iter++) {
         v1 = c7x::strm_eng<0, vec>::get_adv();
         v2 = c7x::strm_eng<1, vec>::get_adv();

         v1 = v1 * reciprocalScale;
         v2 = v2 * reciprocalScale;

         acc1 += v1 * v1;
         acc2 += v2 * v2;

         __vpred pred1   = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pNormU1 = c7x::strm_agen<0, vec>::get_adv(pSA0);
         __vstore_pred(pred1, pNormU1, v1);

         __vpred pred2   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pNormU2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred2, pNormU2, v2);
      }

      if (iterloop2 * 2 == nVec) {
         lastIndex = se1ICNT0 - ((se1Iter - 1) * eleCount) - 1;
         lastV     = v2;
      }
      acc1 = acc1 + acc2;
      acc3 = acc3 + acc4;
      acc1 = acc1 + acc3;

      if (se0Iter != se1Iter) {
         v1 = c7x::strm_eng<1, vec>::get_adv();
         v1 = v1 * reciprocalScale;
         acc1 += v1 * v1;

         __vpred pred2   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pNormU2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred2, pNormU2, v1);

         lastIndex = se1ICNT0 - ((se1Iter - 1) * eleCount) - 1;
         lastV     = v1;
      }

      __SE1_CLOSE();
      if (se0ICNT0 > 0) {
         __SE0_CLOSE();
      }

      dataType s2 = 0;
      c7x_horizontal_add(acc1, &s2);

      const dataType Half  = 0.5;
      const dataType OneP5 = 1.5;
      dataType       x     = __recip_sqrt(s2);
      x                    = x * (OneP5 - (s2 * x * x * Half));
      x                    = x * (OneP5 - (s2 * x * x * Half));
      dataType y           = s2 * x;

      if (lastV.s[lastIndex] < 0) {
         *s = y;
      }
      else {
         *s = -y;
      }

      dataType recipScale = getRecip(scale);
      *half_norm_squared  = (U[0] * recipScale) * (*s) - s2;

      U[0]  = U[0] - ((*s) * scale);
      *pSA0 = (*pSA0) - (*s);

      /* ******************************** UPDATE ROW SUPERDIAG *********************************
       *************************************************************************************** */
      vec reciprocalHalfNorm = (vec) getRecip(*half_norm_squared);

      __SE1_OPEN(pSA1, se1Params);
      if (se0ICNT0 > 0) {
         __SE0_OPEN(pSA0, se0Params);
      }
      for (int32_t horizontal = 0; horizontal < nVec - 1; horizontal += 2) {
         v1 = c7x::strm_eng<0, vec>::get_adv();
         v2 = c7x::strm_eng<1, vec>::get_adv();

         v1 = v1 * reciprocalHalfNorm;
         v2 = v2 * reciprocalHalfNorm;

         __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(superdiag);
         __vstore_pred(pred, pStoreVec, v1);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(&superdiag[se0ICNT0]);
         __vstore_pred(pred, pStoreVec, v2);
      }

      if (se0Iter != se1Iter) {
         v1 = c7x::strm_eng<1, vec>::get_adv();

         v1 = v1 * reciprocalHalfNorm;

         __vpred pred      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(&superdiag[se0ICNT0]);
         __vstore_pred(pred, pStoreVec, v1);
      }

      __SE1_CLOSE();
      __SA1_CLOSE();
      if (se0ICNT0) {
         __SE0_CLOSE();
         __SA0_CLOSE();
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return scale;
}
template float  DSPLIB_bidiag_uRow_halfnorm_ci<float>(float   *U,
                                                     int32_t  Nrows,
                                                     int32_t  Ncols,
                                                     int32_t  colUStride,
                                                     float   *half_norm_squared,
                                                     float   *U1,
                                                     float   *s,
                                                     float   *superdiag,
                                                     uint8_t *pBlock);
template double DSPLIB_bidiag_uRow_halfnorm_ci<double>(double  *U,
                                                       int32_t  Nrows,
                                                       int32_t  Ncols,
                                                       int32_t  colUStride,
                                                       double  *half_norm_squared,
                                                       double  *U1,
                                                       double  *s,
                                                       double  *superdiag,
                                                       uint8_t *pBlock);

/*!
 *  @brief       This function implements the Household processing on rows of input U matrix
 *               corresponding to the natural implementation.
 */
template <typename dataType>
void DSPLIB_bidiag_uRow_ci(dataType *U,
                           int32_t   Nrows,
                           int32_t   Ncols,
                           int32_t   colUStride,
                           dataType *superdiag,
                           dataType *U1,
                           dataType  scale,
                           uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 se0Params, se1Params;
   __SA_TEMPLATE_v1 sa0Params, sa1Params, sa2Params, sa3Params;
   dataType        *siStore = (dataType *) U1;

   /* ******************************** CALCULATE ROW SI **************************************
    *************************************************************************************** */
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE));
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));

   int32_t nVec     = DSPLIB_ceilingDiv(Ncols, eleCount);
   int32_t rowBlock = 8;

   int32_t numBlocks = Nrows / (rowBlock);
   int32_t se1ICNT3  = numBlocks / 2;
   int32_t se0ICNT3  = numBlocks - se1ICNT3;

   int32_t remainingRows = Nrows - (numBlocks * rowBlock);

   int32_t remSE1ICNT1 = remainingRows / 2;
   int32_t remSE0ICNT1 = remainingRows - remSE1ICNT1;

   se0Params.ICNT2 = se1Params.ICNT2 = nVec;
   se0Params.ICNT3                   = se0ICNT3;
   se1Params.ICNT3                   = se1ICNT3;
   se0Params.DECDIM2_WIDTH = se1Params.DECDIM2_WIDTH = Ncols;

   sa2Params.ICNT0 = Ncols;
   sa2Params.ICNT1 = (se0ICNT3 * rowBlock) + remSE0ICNT1;

   sa3Params.ICNT0 = Nrows;
   sa3Params.ICNT1 = 1;

   dataType *reciprocalLoad = (dataType *) U1 + colUStride;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

   int32_t se0ICNT0 = Ncols / 2;
   int32_t se1ICNT0 = Ncols - se0ICNT0;
   int32_t se1ICNT2 = DSPLIB_ceilingDiv(Nrows, eleCount);

   se0Params.ICNT0 = se0ICNT0;
   se0Params.ICNT2 = se1ICNT2;

   se1Params.ICNT0 = se1ICNT0;
   se1Params.ICNT2 = se1ICNT2;

   sa2Params.ICNT0 = Nrows;

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));

   sa0Params.ICNT0 = se0ICNT0;
   sa1Params.ICNT0 = se1ICNT0;
   sa0Params.ICNT1 = sa1Params.ICNT1 = se1ICNT2;

   __SA1_OPEN(sa1Params);
   __SE1_OPEN(&U[se0ICNT0 + colUStride], se1Params);
   __SA2_OPEN(sa2Params);
   if (se0ICNT0 > 0) {
      __SA0_OPEN(sa0Params);
      __SE0_OPEN(&U[colUStride], se0Params);
      for (int32_t vertical = 0; vertical < se1ICNT2; vertical++) {
         vec     acc1       = (vec) 0;
         vec     acc2       = (vec) 0;
         vec     acc3       = (vec) 0;
         vec     acc4       = (vec) 0;
         int32_t horizontal = 0;
         for (horizontal = 0; horizontal < se0ICNT0 - 1; horizontal += 2) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec v2 = c7x::strm_eng<0, vec>::get_adv();

            vec v3 = c7x::strm_eng<1, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();

            dataType *pU1     = c7x::strm_agen<0, dataType>::get_adv(reciprocalLoad);
            vec       refCol1 = __vload_dup(pU1);
            dataType *pU2     = c7x::strm_agen<0, dataType>::get_adv(reciprocalLoad);
            vec       refCol2 = __vload_dup(pU2);
            dataType *pU3     = c7x::strm_agen<1, dataType>::get_adv(reciprocalLoad + se0ICNT0);
            vec       refCol3 = __vload_dup(pU3);
            dataType *pU4     = c7x::strm_agen<1, dataType>::get_adv(reciprocalLoad + se0ICNT0);
            vec       refCol4 = __vload_dup(pU4);

            acc1 += v1 * refCol1;
            acc2 += v2 * refCol2;
            acc3 += v3 * refCol3;
            acc4 += v4 * refCol4;
         }

         for (; horizontal < se0ICNT0; horizontal++) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            dataType *pU1     = c7x::strm_agen<0, dataType>::get_adv(reciprocalLoad);
            vec       refCol1 = __vload_dup(pU1);
            dataType *pU2     = c7x::strm_agen<1, dataType>::get_adv(reciprocalLoad + se0ICNT0);
            vec       refCol2 = __vload_dup(pU2);

            acc1 += v1 * refCol1;
            acc2 += v2 * refCol2;
         }
         /*Last Column process for odd NCols*/
         if (se0ICNT0 != se1ICNT0) {
            vec v1 = c7x::strm_eng<1, vec>::get_adv();

            dataType *pU1     = c7x::strm_agen<1, dataType>::get_adv(reciprocalLoad + se0ICNT0);
            vec       refCol1 = __vload_dup(pU1);

            acc1 += v1 * refCol1;
         }

         acc1 = acc1 + acc2;
         acc3 = acc3 + acc4;

         acc1 = acc1 + acc3;

         __vpred pred      = c7x::strm_agen<2, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
         __vstore_pred(pred, pStoreVec, acc1);
      }
      __SE0_CLOSE();
   }
   else /* Only one column Single SE process */
   {
      dataType *pU1     = c7x::strm_agen<1, dataType>::get_adv(reciprocalLoad + se0ICNT0);
      vec       refCol1 = __vload_dup(pU1);

      for (int32_t vertical = 0; vertical < se1ICNT2; vertical++) {
         vec v1 = c7x::strm_eng<1, vec>::get_adv();

         vec acc1 = v1 * refCol1;

         __vpred pred      = c7x::strm_agen<2, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
         __vstore_pred(pred, pStoreVec, acc1);
      }
   }

   __SE1_CLOSE();
   __SA2_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   /* ******************************** UPDATE ROWS **************************************
    *************************************************************************************** */

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));

   int32_t lenTile  = 8;
   int32_t nTiles   = DSPLIB_ceilingDiv(Ncols, eleCount * lenTile);
   int32_t se1ICNT1 = Nrows / 2;
   int32_t se0ICNT1 = Nrows - se1ICNT1;

   se0Params.ICNT1         = se0ICNT1;
   se0Params.ICNT2         = nTiles;
   se0Params.DECDIM1_WIDTH = Ncols;

   se1Params.ICNT1         = se1ICNT1;
   se1Params.ICNT2         = nTiles;
   se1Params.DECDIM1_WIDTH = Ncols;

   sa2Params.ICNT0 = Ncols;

   sa3Params.ICNT0 = Nrows;
   sa3Params.ICNT1 = nTiles;

   sa0Params.ICNT1         = se0ICNT1;
   sa0Params.ICNT2         = nTiles;
   sa0Params.DECDIM1_WIDTH = Ncols;

   sa1Params.ICNT1         = se1ICNT1;
   sa1Params.ICNT2         = nTiles;
   sa1Params.DECDIM1_WIDTH = Ncols;

   dataType *pSE0 = U + colUStride;
   dataType *pSE1 = U + (2 * colUStride);

   __SE0_OPEN(pSE0, se0Params);
   __SA0_OPEN(sa0Params);
   __SA2_OPEN(sa2Params);
   __SA3_OPEN(sa3Params);
   if (se1ICNT1 > 0) {
      __SE1_OPEN(pSE1, se1Params);
      __SA1_OPEN(sa1Params);

      for (int32_t tile = 0; tile < nTiles; tile++) {
         __vpred pred = c7x::strm_agen<2, vec>::get_vpred();
         vec    *pSd  = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec     sd1  = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd2 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd3 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd4 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd5 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd6 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd7 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd8 = __vload_pred(pred, pSd);

         /* if (Nrows >= 2) */ {
            dataType *pSi1 = c7x::strm_agen<3, dataType>::get_adv(siStore);
            vec       si1  = __vload_dup(pSi1);
            dataType *pSi2 = c7x::strm_agen<3, dataType>::get_adv(siStore);
            vec       si2  = __vload_dup(pSi2);
            for (int32_t vertical = 0; vertical < se1ICNT1 - 1; vertical++) {
               vec v01 = c7x::strm_eng<0, vec>::get_adv();
               vec v02 = c7x::strm_eng<0, vec>::get_adv();
               vec v03 = c7x::strm_eng<0, vec>::get_adv();
               vec v04 = c7x::strm_eng<0, vec>::get_adv();
               vec v05 = c7x::strm_eng<0, vec>::get_adv();
               vec v06 = c7x::strm_eng<0, vec>::get_adv();
               vec v07 = c7x::strm_eng<0, vec>::get_adv();
               vec v08 = c7x::strm_eng<0, vec>::get_adv();

               vec v11 = c7x::strm_eng<1, vec>::get_adv();
               vec v12 = c7x::strm_eng<1, vec>::get_adv();
               vec v13 = c7x::strm_eng<1, vec>::get_adv();
               vec v14 = c7x::strm_eng<1, vec>::get_adv();
               vec v15 = c7x::strm_eng<1, vec>::get_adv();
               vec v16 = c7x::strm_eng<1, vec>::get_adv();
               vec v17 = c7x::strm_eng<1, vec>::get_adv();
               vec v18 = c7x::strm_eng<1, vec>::get_adv();

               v01 += si1 * sd1;
               v02 += si1 * sd2;
               v03 += si1 * sd3;
               v04 += si1 * sd4;
               v05 += si1 * sd5;
               v06 += si1 * sd6;
               v07 += si1 * sd7;
               v08 += si1 * sd8;
               v11 += si2 * sd1;
               v12 += si2 * sd2;
               v13 += si2 * sd3;
               v14 += si2 * sd4;
               v15 += si2 * sd5;
               v16 += si2 * sd6;
               v17 += si2 * sd7;
               v18 += si2 * sd8;

               pSi1 = c7x::strm_agen<3, dataType>::get_adv(siStore);
               si1  = __vload_dup(pSi1);
               pSi2 = c7x::strm_agen<3, dataType>::get_adv(siStore);
               si2  = __vload_dup(pSi2);

               __vpred pred1 = c7x::strm_agen<0, vec>::get_vpred();
               vec    *p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v01);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v02);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v03);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v04);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v05);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v06);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v07);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v08);

               __vpred pred2 = c7x::strm_agen<1, vec>::get_vpred();
               vec    *p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v11);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v12);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v13);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v14);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v15);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v16);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v17);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v18);
            }

            {
               vec v01 = c7x::strm_eng<0, vec>::get_adv();
               vec v02 = c7x::strm_eng<0, vec>::get_adv();
               vec v03 = c7x::strm_eng<0, vec>::get_adv();
               vec v04 = c7x::strm_eng<0, vec>::get_adv();
               vec v05 = c7x::strm_eng<0, vec>::get_adv();
               vec v06 = c7x::strm_eng<0, vec>::get_adv();
               vec v07 = c7x::strm_eng<0, vec>::get_adv();
               vec v08 = c7x::strm_eng<0, vec>::get_adv();

               vec v11 = c7x::strm_eng<1, vec>::get_adv();
               vec v12 = c7x::strm_eng<1, vec>::get_adv();
               vec v13 = c7x::strm_eng<1, vec>::get_adv();
               vec v14 = c7x::strm_eng<1, vec>::get_adv();
               vec v15 = c7x::strm_eng<1, vec>::get_adv();
               vec v16 = c7x::strm_eng<1, vec>::get_adv();
               vec v17 = c7x::strm_eng<1, vec>::get_adv();
               vec v18 = c7x::strm_eng<1, vec>::get_adv();

               v01 += si1 * sd1;
               v02 += si1 * sd2;
               v03 += si1 * sd3;
               v04 += si1 * sd4;
               v05 += si1 * sd5;
               v06 += si1 * sd6;
               v07 += si1 * sd7;
               v08 += si1 * sd8;
               v11 += si2 * sd1;
               v12 += si2 * sd2;
               v13 += si2 * sd3;
               v14 += si2 * sd4;
               v15 += si2 * sd5;
               v16 += si2 * sd6;
               v17 += si2 * sd7;
               v18 += si2 * sd8;

               __vpred pred1 = c7x::strm_agen<0, vec>::get_vpred();
               vec    *p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v01);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v02);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v03);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v04);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v05);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v06);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v07);

               pred1 = c7x::strm_agen<0, vec>::get_vpred();
               p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
               __vstore_pred(pred1, p1, v08);

               __vpred pred2 = c7x::strm_agen<1, vec>::get_vpred();
               vec    *p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v11);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v12);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v13);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v14);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v15);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v16);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v17);

               pred2 = c7x::strm_agen<1, vec>::get_vpred();
               p2    = c7x::strm_agen<1, vec>::get_adv(pSE1);
               __vstore_pred(pred2, p2, v18);
            }
         }

         if (se0ICNT1 != se1ICNT1) {
            vec v01 = c7x::strm_eng<0, vec>::get_adv();
            vec v02 = c7x::strm_eng<0, vec>::get_adv();
            vec v03 = c7x::strm_eng<0, vec>::get_adv();
            vec v04 = c7x::strm_eng<0, vec>::get_adv();
            vec v05 = c7x::strm_eng<0, vec>::get_adv();
            vec v06 = c7x::strm_eng<0, vec>::get_adv();
            vec v07 = c7x::strm_eng<0, vec>::get_adv();
            vec v08 = c7x::strm_eng<0, vec>::get_adv();

            dataType *pSi1 = c7x::strm_agen<3, dataType>::get_adv(siStore);
            vec       si1  = __vload_dup(pSi1);

            v01 += si1 * sd1;
            v02 += si1 * sd2;
            v03 += si1 * sd3;
            v04 += si1 * sd4;
            v05 += si1 * sd5;
            v06 += si1 * sd6;
            v07 += si1 * sd7;
            v08 += si1 * sd8;

            __vpred pred1 = c7x::strm_agen<0, vec>::get_vpred();
            vec    *p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
            __vstore_pred(pred1, p1, v01);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
            __vstore_pred(pred1, p1, v02);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
            __vstore_pred(pred1, p1, v03);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
            __vstore_pred(pred1, p1, v04);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
            __vstore_pred(pred1, p1, v05);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
            __vstore_pred(pred1, p1, v06);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
            __vstore_pred(pred1, p1, v07);

            pred1 = c7x::strm_agen<0, vec>::get_vpred();
            p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
            __vstore_pred(pred1, p1, v08);
         }
      }
      __SE1_CLOSE();
      __SA1_CLOSE();
   }
   else {

      for (int32_t tile = 0; tile < nTiles; tile++) {
         __vpred pred = c7x::strm_agen<2, vec>::get_vpred();
         vec    *pSd  = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec     sd1  = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd2 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd3 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd4 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd5 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd6 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd7 = __vload_pred(pred, pSd);

         pred    = c7x::strm_agen<2, vec>::get_vpred();
         pSd     = c7x::strm_agen<2, vec>::get_adv(superdiag);
         vec sd8 = __vload_pred(pred, pSd);

         vec v01 = c7x::strm_eng<0, vec>::get_adv();
         vec v02 = c7x::strm_eng<0, vec>::get_adv();
         vec v03 = c7x::strm_eng<0, vec>::get_adv();
         vec v04 = c7x::strm_eng<0, vec>::get_adv();
         vec v05 = c7x::strm_eng<0, vec>::get_adv();
         vec v06 = c7x::strm_eng<0, vec>::get_adv();
         vec v07 = c7x::strm_eng<0, vec>::get_adv();
         vec v08 = c7x::strm_eng<0, vec>::get_adv();

         dataType *pSi1 = c7x::strm_agen<3, dataType>::get_adv(siStore);
         vec       si1  = __vload_dup(pSi1);

         v01 += si1 * sd1;
         v02 += si1 * sd2;
         v03 += si1 * sd3;
         v04 += si1 * sd4;
         v05 += si1 * sd5;
         v06 += si1 * sd6;
         v07 += si1 * sd7;
         v08 += si1 * sd8;

         __vpred pred1 = c7x::strm_agen<0, vec>::get_vpred();
         vec    *p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
         __vstore_pred(pred1, p1, v01);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
         __vstore_pred(pred1, p1, v02);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
         __vstore_pred(pred1, p1, v03);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
         __vstore_pred(pred1, p1, v04);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
         __vstore_pred(pred1, p1, v05);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
         __vstore_pred(pred1, p1, v06);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
         __vstore_pred(pred1, p1, v07);

         pred1 = c7x::strm_agen<0, vec>::get_vpred();
         p1    = c7x::strm_agen<0, vec>::get_adv(pSE0);
         __vstore_pred(pred1, p1, v08);
      }
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
   __SA2_CLOSE();
   __SA3_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_bidiag_uRow_ci<float>(float   *U,
                                           int32_t  Nrows,
                                           int32_t  Ncols,
                                           int32_t  colUStride,
                                           float   *superdiag,
                                           float   *U1,
                                           float    scale,
                                           uint8_t *pBlock);
template void DSPLIB_bidiag_uRow_ci<double>(double  *U,
                                            int32_t  Nrows,
                                            int32_t  Ncols,
                                            int32_t  colUStride,
                                            double  *superdiag,
                                            double  *U1,
                                            double   scale,
                                            uint8_t *pBlock);
