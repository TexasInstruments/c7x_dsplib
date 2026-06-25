// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_priv.h"

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */

template <typename dataType> void DSPLIB_bidiag_v_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_svd_PrivArgs *pKerPrivArgs = (DSPLIB_svd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              strideV      = pKerPrivArgs->strideV;
   int32_t              dataSize     = sizeof(dataType);

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se2Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa3Params = __gen_SA_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;

   int32_t colVStride = strideV / sizeof(dataType);

   se0Params.ICNT1   = eleCount;
   se0Params.DIM1    = colVStride;
   se0Params.DIM2    = eleCount * colVStride;
   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   if (dataSize == 4) {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   sa3Params.ICNT0  = 1;
   sa3Params.DIM1   = colVStride;
   sa3Params.DIMFMT = __SA_DIMFMT_2D;
   sa3Params.VECLEN = __SA_VECLEN_1ELEM;

   se2Params.DIMFMT  = __SE_DIMFMT_1D;
   se2Params.ELETYPE = SE_ELETYPE;
   se2Params.VECLEN  = SE_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE))  = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (10 * SE_PARAM_SIZE)) = sa3Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE)) = se2Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_bidiag_v_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_bidiag_v_init_ci<double>(DSPLIB_kernelHandle handle);

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */

/*!
 *  @brief       This function implements the process corresponding to the "update V" loop in
 *               natural implementation.
 */
template <typename dataType>
void DSPLIB_bidiag_v_ci(dataType *V,
                        int32_t   Nrows,
                        int32_t   Ncols,
                        int32_t   colVStride,
                        dataType  s,
                        dataType *U1,
                        dataType *U,
                        int32_t   colUStride,
                        uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   dataType *vColTemp   = (dataType *) U1;                // Pointer to store the single dimensional V data
   dataType *siStore    = (dataType *) (U1 + colUStride); // Pointer to store si values.
   int32_t   nVec       = 0;
   int32_t   horizontal = 0;
   int32_t   vertical   = 0;
   if (s != 0) {
      __SE_TEMPLATE_v1 se0Params, se1Params;
      __SA_TEMPLATE_v1 sa0Params, sa1Params;

      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE));
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE));
      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

      /* ******************************** CALCULATE SINGLE COL V ******************************
       *************************************************************************************** */
      nVec             = DSPLIB_ceilingDiv(Ncols - 1, eleCount);
      int32_t se0Iter  = nVec / 2;
      int32_t se1Iter  = nVec - se0Iter;
      int32_t se0ICNT0 = se0Iter * eleCount;
      int32_t se1ICNT0 = (Ncols - 1) - se0ICNT0;

      se0Params.ICNT0 = se0ICNT0;
      sa0Params.ICNT0 = se0ICNT0;

      se1Params.ICNT0 = se1ICNT0;
      sa1Params.ICNT0 = se1ICNT0;

      vec normFactor = (vec) U[1] * (vec) s;

      vec TwoP0 = (vec) 2.0;
      vec yy1   = __recip(normFactor);
      yy1       = yy1 * (TwoP0 - normFactor * yy1);
      yy1       = yy1 * (TwoP0 - normFactor * yy1);

      vec invNorm = yy1;

      __SE1_OPEN(U + 1 + se0ICNT0, se1Params);
      __SA1_OPEN(sa1Params);
      if (se0Iter > 0) {
         __SE0_OPEN(U + 1, se0Params);
         __SA0_OPEN(sa0Params);

         for (horizontal = 0; horizontal < se0Iter; horizontal++) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            v1 = v1 * invNorm;
            v2 = v2 * invNorm;

            __vpred pred1      = c7x::strm_agen<0, vec>::get_vpred();
            vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(vColTemp + 1);
            __vstore_pred(pred1, pStoreVec1, v1);

            __vpred pred2      = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec2 = c7x::strm_agen<1, vec>::get_adv(vColTemp + 1 + se0ICNT0);
            __vstore_pred(pred2, pStoreVec2, v2);
         }
         __SE0_CLOSE();
         __SA0_CLOSE();
      }

      if (se0Iter != se1Iter) {
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         v2 = v2 * invNorm;

         __vpred pred2      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec2 = c7x::strm_agen<1, vec>::get_adv(vColTemp + 1 + se0ICNT0);
         __vstore_pred(pred2, pStoreVec2, v2);
      }
      U1[0] = 0;
   }

   if (s != 0) {
      /* ******************************** CALCULATE V SI **************************************
       *************************************************************************************** */

      __SE_TEMPLATE_v1 se0ParamsT, se1ParamsT;
      __SA_TEMPLATE_v1 sa0ParamsT, sa1ParamsT, sa2ParamsT;
      se0ParamsT = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE));
      se1ParamsT = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE));
      sa2ParamsT = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
      sa0ParamsT = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
      sa1ParamsT = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

      nVec              = DSPLIB_ceilingDiv(Nrows, eleCount);
      int32_t se0TICNT2 = nVec / 2;
      int32_t se1TICNT2 = nVec - se0TICNT2;

      se0ParamsT.ICNT0 = se1ParamsT.ICNT0 = Ncols - 1;
      se0ParamsT.ICNT2                    = se0TICNT2;
      se1ParamsT.ICNT2                    = se1TICNT2;

      sa2ParamsT.ICNT0 = Ncols - 1;
      sa2ParamsT.ICNT1 = se1TICNT2;

      sa0ParamsT.ICNT0 = se0TICNT2 * eleCount;
      sa1ParamsT.ICNT0 = Nrows - (se0TICNT2 * eleCount);

      dataType *pSE0 = V + 1 + colVStride;
      dataType *pSE1 = V + 1 + colVStride + (se0TICNT2 * eleCount * colVStride);
      dataType *pSA0 = siStore;
      dataType *pSA1 = siStore + (se0TICNT2 * eleCount);

      __SA1_OPEN(sa1ParamsT);
      __SA2_OPEN(sa2ParamsT);
      __SE1_OPEN(pSE1, se1ParamsT);
      if (se0TICNT2 > 0) {
         __SA0_OPEN(sa0ParamsT);
         __SE0_OPEN(pSE0, se0ParamsT);
      }

      for (vertical = 0; vertical < se0TICNT2; vertical++) {
         vec acc1, acc2, acc3, acc4, acc5, acc6, acc7, acc8;
         acc1 = acc2 = acc3 = acc4 = acc5 = acc6 = acc7 = acc8 = (vec) 0;
         horizontal                                            = 0;
         for (; horizontal < (Ncols - 1) - 3; horizontal += 4) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            vec v3 = c7x::strm_eng<0, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();

            vec v5 = c7x::strm_eng<0, vec>::get_adv();
            vec v6 = c7x::strm_eng<1, vec>::get_adv();

            vec v7 = c7x::strm_eng<0, vec>::get_adv();
            vec v8 = c7x::strm_eng<1, vec>::get_adv();

            dataType *pU1 = c7x::strm_agen<2, dataType>::get_adv(U + 1);
            vec       u1  = __vload_dup(pU1);

            dataType *pU2 = c7x::strm_agen<2, dataType>::get_adv(U + 1);
            vec       u2  = __vload_dup(pU2);

            dataType *pU3 = c7x::strm_agen<2, dataType>::get_adv(U + 1);
            vec       u3  = __vload_dup(pU3);

            dataType *pU4 = c7x::strm_agen<2, dataType>::get_adv(U + 1);
            vec       u4  = __vload_dup(pU4);

            acc1 += v1 * u1;
            acc2 += v2 * u1;
            acc3 += v3 * u2;
            acc4 += v4 * u2;
            acc5 += v5 * u3;
            acc6 += v6 * u3;
            acc7 += v7 * u4;
            acc8 += v8 * u4;
         }

         for (; horizontal < (Ncols - 1) - 1; horizontal += 2) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            vec v3 = c7x::strm_eng<0, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();

            dataType *pU1 = c7x::strm_agen<2, dataType>::get_adv(U + 1);
            vec       u1  = __vload_dup(pU1);

            dataType *pU2 = c7x::strm_agen<2, dataType>::get_adv(U + 1);
            vec       u2  = __vload_dup(pU2);

            acc1 += v1 * u1;
            acc2 += v2 * u1;
            acc3 += v3 * u2;
            acc4 += v4 * u2;
         }

         for (; horizontal < (Ncols - 1); horizontal++) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            dataType *pU1 = c7x::strm_agen<2, dataType>::get_adv(U + 1);
            vec       u1  = __vload_dup(pU1);

            acc1 += v1 * u1;
            acc2 += v2 * u1;
         }

         acc1 = acc1 + acc3;
         acc2 = acc2 + acc4;
         acc5 = acc5 + acc7;
         acc6 = acc6 + acc8;

         acc1 = acc1 + acc5;
         acc2 = acc2 + acc6;

         __vpred pred1      = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(pSA0);
         __vstore_pred(pred1, pStoreVec1, acc1);

         __vpred pred2      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred2, pStoreVec2, acc2);
      }

      if (se1TICNT2 != se0TICNT2) {
         vec acc2 = (vec) 0;

         for (horizontal = 0; horizontal < Ncols - 1; horizontal++) {
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            dataType *pU1 = c7x::strm_agen<2, dataType>::get_adv(U + 1);
            vec       u1  = __vload_dup(pU1);

            acc2 += v2 * u1;
         }
         __vpred pred2      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(pred2, pStoreVec2, acc2);
      }
      __SE1_CLOSE();
      __SA1_CLOSE();
      __SA2_CLOSE();
      if (se0TICNT2 > 0) {
         __SE0_CLOSE();
         __SA0_CLOSE();
      }
   }

   if (s != 0) {
      /* ******************************** UPDATE COLS **************************************
       *************************************************************************************** */
      __SE_TEMPLATE_v1 se0Params, se1Params;
      __SA_TEMPLATE_v1 sa0Params, sa1Params, sa2Params, sa3Params;

      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
      sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));
      sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));

      int32_t lenTile  = 8;
      int32_t nTiles   = DSPLIB_ceilingDiv(Ncols, eleCount * lenTile);
      int32_t se1ICNT1 = Nrows / 2;
      int32_t se0ICNT1 = Nrows - se1ICNT1;

      se0Params.ICNT1 = sa0Params.ICNT1 = se0ICNT1;
      se0Params.ICNT2 = sa0Params.ICNT2 = nTiles;
      se0Params.DECDIM1_WIDTH = sa0Params.DECDIM1_WIDTH = Ncols;
      se0Params.DIM1 = sa0Params.DIM1 = colVStride * 2;

      se1Params.ICNT1 = sa1Params.ICNT1 = se1ICNT1;
      se1Params.ICNT2 = sa1Params.ICNT2 = nTiles;
      se1Params.DECDIM1_WIDTH = sa1Params.DECDIM1_WIDTH = Ncols;
      se1Params.DIM1 = sa1Params.DIM1 = colVStride * 2;

      sa2Params.ICNT1         = lenTile * nTiles;
      sa2Params.DECDIM1_WIDTH = Ncols;

      sa3Params.ICNT0 = Nrows;
      sa3Params.ICNT1 = nTiles;

      dataType *pSE0 = V + colVStride;
      dataType *pSE1 = V + (2 * colVStride);

      __SE0_OPEN(pSE0, se0Params);
      __SA0_OPEN(sa0Params);
      __SA2_OPEN(sa2Params);
      __SA3_OPEN(sa3Params);
      if (se1ICNT1 > 0) {
         __SE1_OPEN(pSE1, se1Params);
         __SA1_OPEN(sa1Params);
      }
      for (int32_t tile = 0; tile < nTiles; tile++) {
         __vpred pred  = c7x::strm_agen<2, vec>::get_vpred();
         vec    *pVCol = c7x::strm_agen<2, vec>::get_adv(vColTemp);
         vec     vCol1 = __vload_pred(pred, pVCol);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pVCol     = c7x::strm_agen<2, vec>::get_adv(vColTemp);
         vec vCol2 = __vload_pred(pred, pVCol);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pVCol     = c7x::strm_agen<2, vec>::get_adv(vColTemp);
         vec vCol3 = __vload_pred(pred, pVCol);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pVCol     = c7x::strm_agen<2, vec>::get_adv(vColTemp);
         vec vCol4 = __vload_pred(pred, pVCol);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pVCol     = c7x::strm_agen<2, vec>::get_adv(vColTemp);
         vec vCol5 = __vload_pred(pred, pVCol);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pVCol     = c7x::strm_agen<2, vec>::get_adv(vColTemp);
         vec vCol6 = __vload_pred(pred, pVCol);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pVCol     = c7x::strm_agen<2, vec>::get_adv(vColTemp);
         vec vCol7 = __vload_pred(pred, pVCol);

         pred      = c7x::strm_agen<2, vec>::get_vpred();
         pVCol     = c7x::strm_agen<2, vec>::get_adv(vColTemp);
         vec vCol8 = __vload_pred(pred, pVCol);

         if (Nrows >= 2) {
            dataType *pSi1 = c7x::strm_agen<3, dataType>::get_adv(siStore);
            vec       si1  = __vload_dup(pSi1);
            dataType *pSi2 = c7x::strm_agen<3, dataType>::get_adv(siStore);
            vec       si2  = __vload_dup(pSi2);

            for (vertical = 0; vertical < se1ICNT1 - 1; vertical++) {
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

               v01 += si1 * vCol1;
               v02 += si1 * vCol2;
               v03 += si1 * vCol3;
               v04 += si1 * vCol4;
               v05 += si1 * vCol5;
               v06 += si1 * vCol6;
               v07 += si1 * vCol7;
               v08 += si1 * vCol8;

               v11 += si2 * vCol1;
               v12 += si2 * vCol2;
               v13 += si2 * vCol3;
               v14 += si2 * vCol4;
               v15 += si2 * vCol5;
               v16 += si2 * vCol6;
               v17 += si2 * vCol7;
               v18 += si2 * vCol8;

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

               v01 += si1 * vCol1;
               v02 += si1 * vCol2;
               v03 += si1 * vCol3;
               v04 += si1 * vCol4;
               v05 += si1 * vCol5;
               v06 += si1 * vCol6;
               v07 += si1 * vCol7;
               v08 += si1 * vCol8;

               v11 += si2 * vCol1;
               v12 += si2 * vCol2;
               v13 += si2 * vCol3;
               v14 += si2 * vCol4;
               v15 += si2 * vCol5;
               v16 += si2 * vCol6;
               v17 += si2 * vCol7;
               v18 += si2 * vCol8;

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

            v01 += si1 * vCol1;
            v02 += si1 * vCol2;
            v03 += si1 * vCol3;
            v04 += si1 * vCol4;
            v05 += si1 * vCol5;
            v06 += si1 * vCol6;
            v07 += si1 * vCol7;
            v08 += si1 * vCol8;

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
      if (se1ICNT1 > 0) {
         __SE1_CLOSE();
         __SA1_CLOSE();
      }
   }
   /* UPDATE THE FIRST ROW AND FIRST COLUMN */
   __SA_TEMPLATE_v1 sa2ParamsUpdate, sa3ParamsUpdate;
   sa2ParamsUpdate = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   sa3ParamsUpdate = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (10 * SE_PARAM_SIZE));

   sa2ParamsUpdate.ICNT0 = Ncols;
   sa3ParamsUpdate.ICNT1 = Nrows;

   nVec = DSPLIB_ceilingDiv(Ncols, eleCount);

   /* Updating first row only */
   vec vec1  = (vec) 0;
   vec1.s[0] = 1;
   vec vec2  = (vec) 0;
   __SA2_OPEN(sa2ParamsUpdate);

   __vpred predUpdate      = c7x::strm_agen<2, vec>::get_vpred();
   vec    *pStoreVecUpdate = c7x::strm_agen<2, vec>::get_adv(V);
   __vstore_pred(predUpdate, pStoreVecUpdate, vec1);

   for (horizontal = 1; horizontal < nVec; horizontal++) {
      predUpdate      = c7x::strm_agen<2, vec>::get_vpred();
      pStoreVecUpdate = c7x::strm_agen<2, vec>::get_adv(V);
      __vstore_pred(predUpdate, pStoreVecUpdate, vec2);
   }
   __SA2_CLOSE();

   dataType *pSA3 = V + colVStride;
   __SA3_OPEN(sa3ParamsUpdate);
   for (vertical = 0; vertical < Nrows; vertical++) {
      dataType *pS = c7x::strm_agen<3, dataType>::get_adv(pSA3);
      *pS          = 0;
   }
   __SA3_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_bidiag_v_ci<float>(float   *V,
                                        int32_t  Nrows,
                                        int32_t  Ncols,
                                        int32_t  colVStride,
                                        float    s,
                                        float   *U1,
                                        float   *U,
                                        int32_t  colUStride,
                                        uint8_t *pBlock);
template void DSPLIB_bidiag_v_ci<double>(double  *V,
                                         int32_t  Nrows,
                                         int32_t  Ncols,
                                         int32_t  colVStride,
                                         double   s,
                                         double  *U1,
                                         double  *U,
                                         int32_t  colUStride,
                                         uint8_t *pBlock);
