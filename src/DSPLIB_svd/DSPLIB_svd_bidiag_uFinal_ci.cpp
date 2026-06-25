// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_priv.h"

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */

template <typename dataType> void DSPLIB_bidiag_uFinal_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_svd_PrivArgs *pKerPrivArgs = (DSPLIB_svd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              strideU      = pKerPrivArgs->strideU;

   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se3Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa1Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa2Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa3Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa4Params = __gen_SA_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;

   int32_t colUStride = strideU / sizeof(dataType);

   se1Params.ICNT0   = 1;
   se1Params.DIM1    = colUStride;
   se1Params.DIM2    = 0;
   se1Params.DIMFMT  = __SE_DIMFMT_3D;
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN  = __SE_VECLEN_1ELEM;
   se1Params.GRPDUP  = __SE_GRPDUP_ON;

   sa0Params.ICNT0   = eleCount;
   sa0Params.DIM1    = colUStride;
   sa0Params.DIM2    = eleCount * 2;
   sa0Params.DIMFMT  = __SA_DIMFMT_3D;
   sa0Params.VECLEN  = SA_VECLEN;
   sa0Params.DECDIM1 = __SA_DECDIM_DIM2;

   se3Params.ICNT0   = 1;
   se3Params.DIM1    = colUStride + 1;
   se3Params.DIMFMT  = __SE_DIMFMT_2D;
   se3Params.ELETYPE = SE_ELETYPE;
   se3Params.VECLEN  = __SE_VECLEN_1ELEM;

   sa3Params.ICNT0  = 1;
   sa3Params.DIM1   = colUStride + 1;
   sa3Params.DIMFMT = __SA_DIMFMT_2D;
   sa3Params.VECLEN = __SA_VECLEN_1ELEM;

   sa1Params.ICNT0  = 1;
   sa1Params.DIM1   = colUStride;
   sa1Params.DIMFMT = __SA_DIMFMT_2D;
   sa1Params.VECLEN = __SA_VECLEN_1ELEM;

   sa2Params.DIM1   = colUStride;
   sa2Params.DIMFMT = __SA_DIMFMT_2D;
   sa2Params.VECLEN = SA_VECLEN;

   sa4Params.ICNT0  = 1;
   sa4Params.DIM1   = colUStride;
   sa4Params.DIM2   = 0;
   sa4Params.DIMFMT = __SA_DIMFMT_3D;
   sa4Params.VECLEN = __SA_VECLEN_1ELEM;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (12 * SE_PARAM_SIZE)) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (13 * SE_PARAM_SIZE)) = sa0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (14 * SE_PARAM_SIZE)) = se3Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (15 * SE_PARAM_SIZE)) = sa3Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (16 * SE_PARAM_SIZE)) = sa1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (17 * SE_PARAM_SIZE)) = sa2Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (18 * SE_PARAM_SIZE)) = sa4Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_bidiag_uFinal_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_bidiag_uFinal_init_ci<double>(DSPLIB_kernelHandle handle);

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */

/*!
 *  @brief       This function expands columns of U matrix to get a square matrix and fill the
 *                columns with '0' values.
 */
template <typename dataType>
void DSPLIB_bidiag_uFinal_expand_ci(dataType *U, int32_t Nrows, int32_t Ncols, int32_t colUStride, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SA_TEMPLATE_v1 sa0Params, sa1Params;
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (17 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (17 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   int32_t expandCols = Nrows - Ncols; /* Number of columns to be filled with zeros */
   int32_t sa0ICNT    = Nrows / 2;
   int32_t sa1ICNT    = Nrows - sa0ICNT;

   sa0Params.ICNT0 = sa1Params.ICNT0 = expandCols;
   sa0Params.ICNT1                   = sa0ICNT;
   sa1Params.ICNT1                   = sa1ICNT;

   int32_t nVec       = DSPLIB_ceilingDiv(expandCols, eleCount);
   int32_t totalIter  = sa0ICNT * nVec;
   int32_t uOffsetSA0 = Ncols;
   int32_t uOffsetSA1 = Ncols + (sa0ICNT * colUStride); /* start halfway through rows */

   __SA1_OPEN(sa1Params);
   /* if (sa0ICNT > 0) */ {
      __SA0_OPEN(sa0Params);
      for (int32_t iter = 0; iter < totalIter; iter++) {
         __vpred pred1      = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(U + uOffsetSA0);
         __vstore_pred(pred1, pStoreVec1, (vec) 0);

         __vpred pred2      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec2 = c7x::strm_agen<1, vec>::get_adv(U + uOffsetSA1);
         __vstore_pred(pred2, pStoreVec2, (vec) 0);
      }
      __SA0_CLOSE();
   }

   if (sa0ICNT != sa1ICNT) {
      for (int32_t horizontal = 0; horizontal < nVec; horizontal++) {
         __vpred pred2      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec2 = c7x::strm_agen<1, vec>::get_adv(U + uOffsetSA1);
         __vstore_pred(pred2, pStoreVec2, (vec) 0);
      }
   }
   __SA1_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void
DSPLIB_bidiag_uFinal_expand_ci<float>(float *U, int32_t Nrows, int32_t Ncols, int32_t colUStride, uint8_t *pBlock);
template void
DSPLIB_bidiag_uFinal_expand_ci<double>(double *U, int32_t Nrows, int32_t Ncols, int32_t colUStride, uint8_t *pBlock);

/*!
 *  @brief       This function implements the process corresponding to the "initial U" loop in
 *               natural implementation.
 */
template <typename dataType>
void DSPLIB_bidiag_uFinal_initalize_ci(dataType *U,
                                       int32_t   Nrows,
                                       int32_t   Ncols,
                                       int32_t   colUStride,
                                       dataType  s,
                                       dataType *U1,
                                       uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   if (s != 0) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      __SA_TEMPLATE_v1 sa0Params;
      __SA_TEMPLATE_v1 sa1Params;

      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (12 * SE_PARAM_SIZE));
      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (13 * SE_PARAM_SIZE));
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (13 * SE_PARAM_SIZE));

      int32_t nVec     = DSPLIB_ceilingDiv(Ncols, eleCount);
      int32_t sa1ICNT2 = nVec / 2;
      int32_t sa0ICNT2 = nVec - sa1ICNT2;

      sa0Params.ICNT1         = Nrows;
      sa0Params.ICNT2         = sa0ICNT2;
      sa0Params.DECDIM1_WIDTH = Ncols;

      sa1Params.ICNT1         = Nrows;
      sa1Params.ICNT2         = sa1ICNT2;
      sa1Params.DECDIM1_WIDTH = Ncols - eleCount;

      se0Params.ICNT2 = nVec;

      se1Params.ICNT1 = Nrows;
      se1Params.ICNT2 = sa0ICNT2;

      dataType reciprocalFactor = getRecip((U[0] * s));
      vec      reciprocalVec    = (vec) reciprocalFactor;

      __SE1_OPEN(U, se1Params);
      __SE0_OPEN(U + colUStride, se0Params);
      __SA0_OPEN(sa0Params);
      if (sa1ICNT2 > 0) {
         __SA1_OPEN(sa1Params);

         for (int32_t horizontal = 0; horizontal < sa1ICNT2; horizontal++) {
            vec uCol1 = c7x::strm_eng<0, vec>::get_adv();
            vec uCol2 = c7x::strm_eng<0, vec>::get_adv();
            for (int32_t vertical = 0; vertical < Nrows; vertical++) {
               vec uEle = c7x::strm_eng<1, vec>::get_adv();

               vec v1 = uCol1 * uEle * reciprocalVec;
               vec v2 = uCol2 * uEle * reciprocalVec;

               __vpred pred1      = c7x::strm_agen<0, vec>::get_vpred();
               vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(U + 1);
               __vstore_pred(pred1, pStoreVec1, v1);

               __vpred pred2      = c7x::strm_agen<1, vec>::get_vpred();
               vec    *pStoreVec2 = c7x::strm_agen<1, vec>::get_adv(U + 1 + eleCount);
               __vstore_pred(pred2, pStoreVec2, v2);
            }
         }
         __SA1_CLOSE();
      }

      if (sa1ICNT2 != sa0ICNT2) {
         vec uCol1 = c7x::strm_eng<0, vec>::get_adv();
         for (int32_t vertical = 0; vertical < Nrows; vertical++) {
            vec uEle = c7x::strm_eng<1, vec>::get_adv();

            vec v1 = uCol1 * uEle * reciprocalVec;

            __vpred pred1      = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(U + 1);
            __vstore_pred(pred1, pStoreVec1, v1);
         }
      }

      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();

      __SE_TEMPLATE_v1 se3Params;
      __SA_TEMPLATE_v1 sa3Params;

      se3Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (14 * SE_PARAM_SIZE));
      sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (15 * SE_PARAM_SIZE));

      se3Params.ICNT1 = sa3Params.ICNT1 = Ncols;

      __SE0_OPEN(U + 1 + colUStride, se3Params);
      __SA0_OPEN(sa3Params);

      for (int32_t diag = 0; diag < Ncols; diag++) {
         vec       diagEle = c7x::strm_eng<0, vec>::get_adv();
         dataType *pStore  = c7x::strm_agen<0, dataType>::get_adv(U + 1 + colUStride);
         *pStore           = diagEle.s[0] + 1;
      }
   }
   else {
      __SA_TEMPLATE_v1 sa3Params;

      sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (15 * SE_PARAM_SIZE));

      sa3Params.ICNT1 = Ncols;

      __SA0_OPEN(sa3Params);

      for (int32_t diag = 0; diag < Ncols; diag++) {
         dataType *pStore = c7x::strm_agen<0, dataType>::get_adv(U + 1 + colUStride);
         *pStore          = 1;
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_bidiag_uFinal_initalize_ci<float>(float   *U,
                                                       int32_t  Nrows,
                                                       int32_t  Ncols,
                                                       int32_t  colUStride,
                                                       float    s,
                                                       float   *U1,
                                                       uint8_t *pBlock);
template void DSPLIB_bidiag_uFinal_initalize_ci<double>(double  *U,
                                                        int32_t  Nrows,
                                                        int32_t  Ncols,
                                                        int32_t  colUStride,
                                                        double   s,
                                                        double  *U1,
                                                        uint8_t *pBlock);

/*!
 *  @brief       This function normalizes the column of input matrix U.
 */
template <typename dataType>
void DSPLIB_bidiag_uFinal_normalize_ci(dataType *U, int32_t Nrows, dataType s, int32_t colUStride, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (16 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   int32_t  nVec          = Nrows / eleCount;
   int32_t  remainingRows = Nrows - (nVec * eleCount);
   dataType normFactor    = getRecip(s);
   vec      invNormFactor = (vec) normFactor;

   se0Params.ICNT2 = DSPLIB_ceilingDiv(Nrows, eleCount);
   sa0Params.ICNT1 = Nrows;

   __SA0_OPEN(sa0Params);

   if (s != 0) {
      __SE0_OPEN(U, se0Params);

      for (int32_t vertical = 0; vertical < nVec; vertical++) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();

         v1 *= invNormFactor;

         for (int32_t i = 0; i < eleCount; i++) {
            dataType *pStoreVec1 = c7x::strm_agen<0, dataType>::get_adv(U);
            *pStoreVec1          = v1.s[i];
         }
      }

      if (remainingRows > 0) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         v1 *= invNormFactor;
         for (int32_t i = 0; i < remainingRows; i++) {
            dataType *pStoreVec1 = c7x::strm_agen<0, dataType>::get_adv(U);
            *pStoreVec1          = v1.s[i];
         }
      }
      __SE0_CLOSE();
   }
   else {

      for (int32_t vertical = 0; vertical < Nrows; vertical++) {
         dataType *pStoreVec1 = c7x::strm_agen<0, dataType>::get_adv(U);
         *pStoreVec1          = 0;
      }
   }
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void
DSPLIB_bidiag_uFinal_normalize_ci<float>(float *U, int32_t Nrows, float s, int32_t colUStride, uint8_t *pBlock);
template void
DSPLIB_bidiag_uFinal_normalize_ci<double>(double *U, int32_t Nrows, double s, int32_t colUStride, uint8_t *pBlock);

/*!
 *  @brief       This function implements the process corresponding to the "update U" loop in
 *               natural implementation.
 */
template <typename dataType>
void DSPLIB_bidiag_uFinal_ci(dataType *U,
                             int32_t   Nrows,
                             int32_t   Ncols,
                             int32_t   colUStride,
                             dataType  s,
                             dataType *U1,
                             uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SA_TEMPLATE_v1 sa2ParamsUpdate0;

   sa2ParamsUpdate0 = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

   int32_t nVec = DSPLIB_ceilingDiv(Ncols, eleCount);

   sa2ParamsUpdate0.ICNT0 = Ncols;
   /************************************ UPDATE 0  *********************************************
    ****************************************************************************************** */
   __SA2_OPEN(sa2ParamsUpdate0);
   for (int32_t horizontal = 0; horizontal < nVec; horizontal++) {
      __vpred pred      = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<2, vec>::get_adv(U + 1);
      __vstore_pred(pred, pStoreVec, (vec) 0);
   }
   __SA2_CLOSE();

   if (s != 0) {
      /************************************ CALCULATE si *****************************************
       ****************************************************************************************** */
      vec siNormFactor = (vec) getRecip((U[0] * s));

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

      dataType *siStore = (dataType *) U1;

      __SA_TEMPLATE_v1 sa3Params;
      sa3Params       = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (18 * SE_PARAM_SIZE));
      sa3Params.ICNT1 = Nrows;
      sa3Params.ICNT2 = 2 * nTiles;

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

            dataType *pU1 = c7x::strm_agen<3, dataType>::get_adv(U);
            vec       u1  = __vload_dup(pU1);
            dataType *pU2 = c7x::strm_agen<3, dataType>::get_adv(U);
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

            dataType *pU1 = c7x::strm_agen<3, dataType>::get_adv(U);
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
         __vstore_pred(pred, pStoreVec, acc1 * siNormFactor);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
         __vstore_pred(pred, pStoreVec, acc2 * siNormFactor);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
         __vstore_pred(pred, pStoreVec, acc3 * siNormFactor);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
         __vstore_pred(pred, pStoreVec, acc4 * siNormFactor);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
         __vstore_pred(pred, pStoreVec, acc5 * siNormFactor);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
         __vstore_pred(pred, pStoreVec, acc6 * siNormFactor);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
         __vstore_pred(pred, pStoreVec, acc7 * siNormFactor);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<2, vec>::get_adv(siStore);
         __vstore_pred(pred, pStoreVec, acc8 * siNormFactor);
      }

      /************************************ UPDATE COLUMNS ***************************************
       ****************************************************************************************** */

      __SA_TEMPLATE_v1 sa0Params;
      __SA_TEMPLATE_v1 sa1Params;

      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
      sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));

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
            dataType *pU1 = c7x::strm_agen<3, dataType>::get_adv(U);
            vec       u1  = __vload_dup(pU1);
            dataType *pU2 = c7x::strm_agen<3, dataType>::get_adv(U);
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

               pU1 = c7x::strm_agen<3, dataType>::get_adv(U);
               u1  = __vload_dup(pU1);
               pU2 = c7x::strm_agen<3, dataType>::get_adv(U);
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

            dataType *pU1 = c7x::strm_agen<3, dataType>::get_adv(U);
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
   }
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_bidiag_uFinal_ci<float>(float   *U,
                                             int32_t  Nrows,
                                             int32_t  Ncols,
                                             int32_t  colUStride,
                                             float    s,
                                             float   *U1,
                                             uint8_t *pBlock);
template void DSPLIB_bidiag_uFinal_ci<double>(double  *U,
                                              int32_t  Nrows,
                                              int32_t  Ncols,
                                              int32_t  colUStride,
                                              double   s,
                                              double  *U1,
                                              uint8_t *pBlock);
