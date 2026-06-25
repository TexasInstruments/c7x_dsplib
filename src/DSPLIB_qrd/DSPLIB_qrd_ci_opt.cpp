// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_priv.h"

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/
template <typename dataType> void DSPLIB_R_column_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_qrd_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              strideR      = pKerPrivArgs->strideR;
   int32_t              colStrideR   = strideR / sizeof(dataType);
   __SA_TEMPLATE_v1     sa0Params    = __gen_SA_TEMPLATE_v1();

   sa0Params.ICNT0  = 1;
   sa0Params.DIMFMT = __SA_DIMFMT_2D;
   sa0Params.VECLEN = __SA_VECLEN_1ELEM;
   sa0Params.DIM1   = 2 * colStrideR;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = sa0Params;
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_R_column_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_R_column_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_Q_matrix_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_qrd_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              strideQ      = pKerPrivArgs->strideQ;
   int32_t              strideR      = pKerPrivArgs->strideR;
   int32_t              nRows        = pKerPrivArgs->heightA;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se2Params = __gen_SE_TEMPLATE_v1();

   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa1Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa2Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa5Params = __gen_SA_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;
   int32_t                                                colStrideQ = strideQ / sizeof(dataType);
   int32_t                                                colStrideR = strideR / sizeof(dataType);

   int32_t nVec      = DSPLIB_ceilingDiv(nRows, eleCount);
   int32_t se0TICNT2 = nVec / 2;
   int32_t se1TICNT2 = nVec - se0TICNT2;

   se0Params.ICNT1   = eleCount;
   se0Params.DIM1    = colStrideQ;
   se0Params.DIM2    = eleCount * colStrideQ;
   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.ICNT2   = se0TICNT2;
   if (sizeof(dataType) == 4) {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   se1Params.ICNT2   = se1TICNT2;
   se1Params.ICNT1   = eleCount;
   se1Params.DIM1    = colStrideQ;
   se1Params.DIM2    = eleCount * colStrideQ;
   se1Params.DIMFMT  = __SE_DIMFMT_3D;
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN  = SE_VECLEN;
   if (sizeof(dataType) == 4) {
      se1Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se1Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   sa2Params.DIM1   = 0;
   sa2Params.DIMFMT = __SA_DIMFMT_2D;
   sa2Params.VECLEN = __SA_VECLEN_1ELEM;
   sa2Params.ICNT0  = nRows;
   sa2Params.ICNT1  = se1TICNT2;

   sa0Params.ICNT0  = se0TICNT2 * eleCount;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;
   sa0Params.VECLEN = SA_VECLEN;

   sa1Params.DIMFMT = __SA_DIMFMT_1D;
   sa1Params.VECLEN = SA_VECLEN;
   sa1Params.ICNT0  = nRows - (se0TICNT2 * eleCount);

   int32_t se1ICNT1 = nRows / 2;
   int32_t se0ICNT1 = nRows - se1ICNT1;

   int32_t lenTile   = 8;
   se2Params.ICNT0   = (eleCount * lenTile);
   se2Params.DIM1    = colStrideR * 2;
   se2Params.DIM2    = (eleCount * lenTile);
   se2Params.ICNT1   = se0ICNT1;
   se2Params.DIMFMT  = __SE_DIMFMT_3D;
   se2Params.ELETYPE = SE_ELETYPE;
   se2Params.VECLEN  = SE_VECLEN;
   se2Params.DECDIM1 = __SE_DECDIM_DIM2;

   sa5Params.ICNT0  = eleCount;
   sa5Params.DIMFMT = __SA_DIMFMT_1D;
   sa5Params.VECLEN = SA_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE))  = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE))  = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE))  = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE))  = sa1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE))  = sa2Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE))  = se2Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE)) = sa5Params;

   int32_t          lenTile8       = 8;
   __SE_TEMPLATE_v1 seScalarParams = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 seMatrixParams = __gen_SE_TEMPLATE_v1();

   __SA_TEMPLATE_v1 saMatrixParams = __gen_SA_TEMPLATE_v1();

   __SE_ELEDUP SE_ELEDUP = c7x::se_eledup<dataType>::value;

   seScalarParams.DIM1    = 0;
   seScalarParams.ELEDUP  = SE_ELEDUP;
   seScalarParams.DIMFMT  = __SE_DIMFMT_2D;
   seScalarParams.VECLEN  = SE_VECLEN;
   seScalarParams.ELETYPE = SE_ELETYPE;

   seMatrixParams.ICNT0   = (eleCount * lenTile8);
   seMatrixParams.DIM1    = colStrideR;
   seMatrixParams.DIM2    = (eleCount * lenTile8);
   seMatrixParams.DIMFMT  = __SE_DIMFMT_3D;
   seMatrixParams.ELETYPE = SE_ELETYPE;
   seMatrixParams.VECLEN  = SE_VECLEN;
   seMatrixParams.DECDIM1 = __SE_DECDIM_DIM2;

   saMatrixParams.ICNT0   = (eleCount * lenTile8);
   saMatrixParams.DIM1    = colStrideR;
   saMatrixParams.DIM2    = (eleCount * lenTile8);
   saMatrixParams.DIMFMT  = __SA_DIMFMT_3D;
   saMatrixParams.VECLEN  = SA_VECLEN;
   saMatrixParams.DECDIM1 = __SA_DECDIM_DIM2;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (13 * SE_PARAM_SIZE)) = seScalarParams;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (14 * SE_PARAM_SIZE)) = seMatrixParams;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (15 * SE_PARAM_SIZE)) = saMatrixParams;

   int32_t lenTile4 = 4;
   seScalarParams   = __gen_SE_TEMPLATE_v1();
   seMatrixParams   = __gen_SE_TEMPLATE_v1();

   saMatrixParams = __gen_SA_TEMPLATE_v1();

   SE_ELEDUP = c7x::se_eledup<dataType>::value;

   seMatrixParams.ICNT0   = (eleCount * lenTile4);
   seMatrixParams.DIM1    = colStrideQ;
   seMatrixParams.DIM2    = (eleCount * lenTile4);
   seMatrixParams.DIMFMT  = __SE_DIMFMT_3D;
   seMatrixParams.ELETYPE = SE_ELETYPE;
   seMatrixParams.VECLEN  = SE_VECLEN;
   seMatrixParams.DECDIM1 = __SE_DECDIM_DIM2;

   saMatrixParams.ICNT0   = (eleCount * lenTile4);
   saMatrixParams.DIM1    = colStrideQ;
   saMatrixParams.DIM2    = (eleCount * lenTile4);
   saMatrixParams.DIMFMT  = __SA_DIMFMT_3D;
   saMatrixParams.VECLEN  = SA_VECLEN;
   saMatrixParams.DECDIM1 = __SA_DECDIM_DIM2;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (16 * SE_PARAM_SIZE)) = seMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (17 * SE_PARAM_SIZE)) = saMatrixParams;

   se2Params                                                         = __gen_SE_TEMPLATE_v1();
   se2Params.ICNT0                                                   = (eleCount * lenTile4);
   se2Params.DIM1                                                    = colStrideR * 2;
   se2Params.DIM2                                                    = (eleCount * lenTile4);
   se2Params.ICNT1                                                   = se0ICNT1;
   se2Params.DIMFMT                                                  = __SE_DIMFMT_3D;
   se2Params.ELETYPE                                                 = SE_ELETYPE;
   se2Params.VECLEN                                                  = SE_VECLEN;
   se2Params.DECDIM1                                                 = __SE_DECDIM_DIM2;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (12 * SE_PARAM_SIZE)) = se2Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_Q_matrix_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_Q_matrix_init_ci<double>(DSPLIB_kernelHandle handle);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/

template <typename dataType>
void DSPLIB_qrd_R_matrix_exec_ci(dataType *pLocalR,
                                 dataType *pLocalU,
                                 dataType *pSum,
                                 dataType  scale,
                                 int32_t   colStrideR,
                                 int32_t   nRows,
                                 int32_t   nCols,
                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   dataType                                              *rStore = pLocalR + 1;

   int32_t eleCount = c7x::element_count_of<vec>::value;
   int32_t lenTile8 = 8;
   int32_t lenTile4 = 4;
   int32_t lenTile2 = 2;
   int32_t lenTile1 = 1;

   int32_t nTiles1 = DSPLIB_ceilingDiv(nCols, (eleCount));
   int32_t nTiles8 = nTiles1 / lenTile8;
   nTiles1 -= nTiles8 * lenTile8;
   int32_t nTiles4 = nTiles1 / lenTile4;
   nTiles1 -= nTiles4 * lenTile4;
   int32_t nTiles2 = nTiles1 / lenTile2;
   nTiles1 -= nTiles2 * lenTile2;

   int32_t remainingCols = nCols;
   int32_t colLimit8     = nTiles8 * lenTile8 * eleCount;
   colLimit8             = (remainingCols < (colLimit8)) ? remainingCols : colLimit8;

   remainingCols     = remainingCols - colLimit8;
   int32_t colLimit4 = nTiles4 * lenTile4 * eleCount;
   colLimit4         = (remainingCols < (colLimit4)) ? remainingCols : colLimit4;

   remainingCols     = remainingCols - colLimit4;
   int32_t colLimit2 = nTiles2 * lenTile2 * eleCount;
   colLimit2         = (remainingCols < (colLimit2)) ? remainingCols : colLimit2;

   int32_t colLimit1 = remainingCols - colLimit2;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE));
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE));

   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE));

   sa1Params.ICNT0 = nCols;

   __SA1_OPEN(sa1Params);

   sa0Params.ICNT0 = nRows;
   sa0Params.ICNT1 = nTiles8 + nTiles4 + nTiles2 + nTiles1;
   __SA0_OPEN(sa0Params);

   int32_t se1ICNT1 = nRows / 2;
   int32_t se0ICNT1 = nRows - se1ICNT1;
   se0Params.ICNT1  = se0ICNT1;
   se1Params.ICNT1  = se1ICNT1;

   if (nTiles8 > 0) {
      se0Params.ICNT2         = nTiles8;
      se0Params.DECDIM1_WIDTH = colLimit8;
      se0Params.ICNT0         = (eleCount * lenTile8);

      se1Params.ICNT2         = nTiles8;
      se1Params.DECDIM1_WIDTH = colLimit8;
      se1Params.ICNT0         = (eleCount * lenTile8);

      vec sV01 = (vec) 0;
      vec sV02 = (vec) 0;
      vec sV03 = (vec) 0;
      vec sV04 = (vec) 0;
      vec sV05 = (vec) 0;
      vec sV06 = (vec) 0;
      vec sV07 = (vec) 0;
      vec sV08 = (vec) 0;

      vec sV11 = (vec) 0;
      vec sV12 = (vec) 0;
      vec sV13 = (vec) 0;
      vec sV14 = (vec) 0;
      vec sV15 = (vec) 0;
      vec sV16 = (vec) 0;
      vec sV17 = (vec) 0;
      vec sV18 = (vec) 0;

      __SE0_OPEN(rStore, se0Params);
      __SE1_OPEN(rStore + colStrideR, se1Params);

      for (int tile = 0; tile < nTiles8; tile++) {

         sV01 = (vec) 0;
         sV02 = (vec) 0;
         sV03 = (vec) 0;
         sV04 = (vec) 0;
         sV05 = (vec) 0;
         sV06 = (vec) 0;
         sV07 = (vec) 0;
         sV08 = (vec) 0;

         sV11 = (vec) 0;
         sV12 = (vec) 0;
         sV13 = (vec) 0;
         sV14 = (vec) 0;
         sV15 = (vec) 0;
         sV16 = (vec) 0;
         sV17 = (vec) 0;
         sV18 = (vec) 0;
         for (int32_t vertical = 0; vertical < se1ICNT1; vertical++) {
            vec v01 = c7x::strm_eng<0, vec>::get_adv();
            vec v02 = c7x::strm_eng<0, vec>::get_adv();
            vec v03 = c7x::strm_eng<0, vec>::get_adv();
            vec v04 = c7x::strm_eng<0, vec>::get_adv();
            vec v05 = c7x::strm_eng<0, vec>::get_adv();
            vec v06 = c7x::strm_eng<0, vec>::get_adv();
            vec v07 = c7x::strm_eng<0, vec>::get_adv();
            vec v08 = c7x::strm_eng<0, vec>::get_adv();

            dataType *pU  = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec       uV1 = __vload_dup(pU);

            vec v11 = c7x::strm_eng<1, vec>::get_adv();
            vec v12 = c7x::strm_eng<1, vec>::get_adv();
            vec v13 = c7x::strm_eng<1, vec>::get_adv();
            vec v14 = c7x::strm_eng<1, vec>::get_adv();
            vec v15 = c7x::strm_eng<1, vec>::get_adv();
            vec v16 = c7x::strm_eng<1, vec>::get_adv();
            vec v17 = c7x::strm_eng<1, vec>::get_adv();
            vec v18 = c7x::strm_eng<1, vec>::get_adv();

            pU      = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec uV2 = __vload_dup(pU);

            sV01 += v01 * uV1;
            sV02 += v02 * uV1;
            sV03 += v03 * uV1;
            sV04 += v04 * uV1;
            sV05 += v05 * uV1;
            sV06 += v06 * uV1;
            sV07 += v07 * uV1;
            sV08 += v08 * uV1;

            sV11 += v11 * uV2;
            sV12 += v12 * uV2;
            sV13 += v13 * uV2;
            sV14 += v14 * uV2;
            sV15 += v15 * uV2;
            sV16 += v16 * uV2;
            sV17 += v17 * uV2;
            sV18 += v18 * uV2;
         }

         if (se1ICNT1 != se0ICNT1) /* For last odd numbered row */
         {
            dataType *pU  = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec       uV1 = __vload_dup(pU);

            vec v01 = c7x::strm_eng<0, vec>::get_adv();
            vec v02 = c7x::strm_eng<0, vec>::get_adv();
            vec v03 = c7x::strm_eng<0, vec>::get_adv();
            vec v04 = c7x::strm_eng<0, vec>::get_adv();
            vec v05 = c7x::strm_eng<0, vec>::get_adv();
            vec v06 = c7x::strm_eng<0, vec>::get_adv();
            vec v07 = c7x::strm_eng<0, vec>::get_adv();
            vec v08 = c7x::strm_eng<0, vec>::get_adv();

            sV01 += v01 * uV1;
            sV02 += v02 * uV1;
            sV03 += v03 * uV1;
            sV04 += v04 * uV1;
            sV05 += v05 * uV1;
            sV06 += v06 * uV1;
            sV07 += v07 * uV1;
            sV08 += v08 * uV1;
         }

         sV01 += sV11;
         sV02 += sV12;
         sV03 += sV13;
         sV04 += sV14;
         sV05 += sV15;
         sV06 += sV16;
         sV07 += sV17;
         sV08 += sV18;

         sV01 *= scale;
         sV02 *= scale;
         sV03 *= scale;
         sV04 *= scale;
         sV05 *= scale;
         sV06 *= scale;
         sV07 *= scale;
         sV08 *= scale;

         __vpred pred      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV01);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV02);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV03);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV04);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV05);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV06);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV07);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV08);
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   if (nTiles4 > 0) {

      se0Params.ICNT2         = nTiles4;
      se0Params.DECDIM1_WIDTH = colLimit4;
      se0Params.ICNT0         = (eleCount * lenTile4);

      se1Params.ICNT2         = nTiles4;
      se1Params.DECDIM1_WIDTH = colLimit4;
      se1Params.ICNT0         = (eleCount * lenTile4);

      vec sV01 = (vec) 0;
      vec sV02 = (vec) 0;
      vec sV03 = (vec) 0;
      vec sV04 = (vec) 0;

      vec sV11 = (vec) 0;
      vec sV12 = (vec) 0;
      vec sV13 = (vec) 0;
      vec sV14 = (vec) 0;

      dataType *pSE0 = rStore + colLimit8;
      dataType *pSE1 = pSE0 + colStrideR;

      __SE0_OPEN(pSE0, se0Params);

      __SE1_OPEN(pSE1, se1Params);

      for (int tile = 0; tile < nTiles4; tile++) {

         sV01 = (vec) 0;
         sV02 = (vec) 0;
         sV03 = (vec) 0;
         sV04 = (vec) 0;

         sV11 = (vec) 0;
         sV12 = (vec) 0;
         sV13 = (vec) 0;
         sV14 = (vec) 0;

         for (int32_t vertical = 0; vertical < se1ICNT1; vertical++) {

            vec       v01 = c7x::strm_eng<0, vec>::get_adv();
            vec       v02 = c7x::strm_eng<0, vec>::get_adv();
            vec       v03 = c7x::strm_eng<0, vec>::get_adv();
            vec       v04 = c7x::strm_eng<0, vec>::get_adv();
            dataType *pU  = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec       uV1 = __vload_dup(pU);

            vec v11 = c7x::strm_eng<1, vec>::get_adv();
            vec v12 = c7x::strm_eng<1, vec>::get_adv();
            vec v13 = c7x::strm_eng<1, vec>::get_adv();
            vec v14 = c7x::strm_eng<1, vec>::get_adv();

            pU      = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec uV2 = __vload_dup(pU);

            sV01 += v01 * uV1;
            sV02 += v02 * uV1;
            sV03 += v03 * uV1;
            sV04 += v04 * uV1;

            sV11 += v11 * uV2;
            sV12 += v12 * uV2;
            sV13 += v13 * uV2;
            sV14 += v14 * uV2;
         }

         if (se1ICNT1 != se0ICNT1) /* For last odd numbered row */
         {
            dataType *pU  = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec       uV1 = __vload_dup(pU);

            vec v01 = c7x::strm_eng<0, vec>::get_adv();
            vec v02 = c7x::strm_eng<0, vec>::get_adv();
            vec v03 = c7x::strm_eng<0, vec>::get_adv();
            vec v04 = c7x::strm_eng<0, vec>::get_adv();

            sV01 += v01 * uV1;
            sV02 += v02 * uV1;
            sV03 += v03 * uV1;
            sV04 += v04 * uV1;
         }

         sV01 += sV11;
         sV02 += sV12;
         sV03 += sV13;
         sV04 += sV14;

         sV01 *= scale;
         sV02 *= scale;
         sV03 *= scale;
         sV04 *= scale;

         __vpred pred      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV01);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV02);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV03);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV04);
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   if (nTiles2 > 0) {

      se0Params.ICNT2         = nTiles2;
      se0Params.DECDIM1_WIDTH = colLimit2;
      se0Params.ICNT0         = (eleCount * lenTile2);

      se1Params.ICNT2         = nTiles2;
      se1Params.DECDIM1_WIDTH = colLimit2;
      se1Params.ICNT0         = (eleCount * lenTile2);

      vec sV01 = (vec) 0;
      vec sV02 = (vec) 0;

      vec sV11 = (vec) 0;
      vec sV12 = (vec) 0;

      dataType *pSE0 = rStore + colLimit8 + colLimit4;
      dataType *pSE1 = pSE0 + colStrideR;

      __SE0_OPEN(pSE0, se0Params);
      __SE1_OPEN(pSE1, se1Params);

      for (int tile = 0; tile < nTiles2; tile++) {
         sV01 = (vec) 0;
         sV02 = (vec) 0;

         sV11 = (vec) 0;
         sV12 = (vec) 0;

         for (int32_t vertical = 0; vertical < se1ICNT1; vertical++) {
            vec v01 = c7x::strm_eng<0, vec>::get_adv();
            vec v02 = c7x::strm_eng<0, vec>::get_adv();

            dataType *pU  = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec       uV1 = __vload_dup(pU);

            vec v11 = c7x::strm_eng<1, vec>::get_adv();
            vec v12 = c7x::strm_eng<1, vec>::get_adv();

            pU      = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec uV2 = __vload_dup(pU);

            sV01 += v01 * uV1;
            sV02 += v02 * uV1;

            sV11 += v11 * uV2;
            sV12 += v12 * uV2;
         }

         if (se1ICNT1 != se0ICNT1) /* For last odd numbered row */
         {
            dataType *pU  = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec       uV1 = __vload_dup(pU);

            vec v01 = c7x::strm_eng<0, vec>::get_adv();
            vec v02 = c7x::strm_eng<0, vec>::get_adv();

            sV01 += v01 * uV1;
            sV02 += v02 * uV1;
         }

         sV01 += sV11;
         sV02 += sV12;

         sV01 *= scale;
         sV02 *= scale;

         __vpred pred      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV01);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV02);
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   if (nTiles1 > 0) {
      se0Params.ICNT1         = se0ICNT1;
      se0Params.ICNT2         = nTiles1;
      se0Params.DECDIM1_WIDTH = colLimit1;
      se0Params.ICNT0         = (eleCount * lenTile1);

      se1Params.ICNT1         = se1ICNT1;
      se1Params.ICNT2         = nTiles1;
      se1Params.DECDIM1_WIDTH = colLimit1;
      se1Params.ICNT0         = (eleCount * lenTile1);

      vec sV01 = (vec) 0;

      vec sV11 = (vec) 0;

      dataType *pSE0 = rStore + colLimit8 + colLimit4 + colLimit2;
      dataType *pSE1 = pSE0 + colStrideR;

      __SE0_OPEN(pSE0, se0Params);
      __SE1_OPEN(pSE1, se1Params);

      for (int tile = 0; tile < nTiles1; tile++) {

         sV01 = (vec) 0;

         sV11 = (vec) 0;

         for (int32_t vertical = 0; vertical < se1ICNT1; vertical++) {
            vec v01 = c7x::strm_eng<0, vec>::get_adv();

            dataType *pU  = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec       uV1 = __vload_dup(pU);

            vec v11 = c7x::strm_eng<1, vec>::get_adv();

            pU      = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec uV2 = __vload_dup(pU);

            sV01 += v01 * uV1;

            sV11 += v11 * uV2;
         }

         if (se1ICNT1 != se0ICNT1) /* For last odd numbered row */
         {
            dataType *pU  = c7x::strm_agen<0, dataType>::get_adv(pLocalU);
            vec       uV1 = __vload_dup(pU);

            vec v01 = c7x::strm_eng<0, vec>::get_adv();

            sV01 += v01 * uV1;
         }

         sV01 += sV11;

         sV01 *= scale;

         __vpred pred      = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSum);
         __vstore_pred(pred, pStoreVec, sV01);
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   __SA0_CLOSE();
   __SA1_CLOSE();

   __SE_TEMPLATE_v1 seScalarParams;
   __SE_TEMPLATE_v1 seMatrixParams;

   __SA_TEMPLATE_v1 saMatrixParams;

   seScalarParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (13 * SE_PARAM_SIZE));
   seMatrixParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (14 * SE_PARAM_SIZE));
   saMatrixParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (15 * SE_PARAM_SIZE));

   sa1Params.ICNT0      = nCols;
   seScalarParams.ICNT0 = nRows;
   seScalarParams.ICNT1 = nTiles8 + nTiles4 + nTiles2 + nTiles1;

   sa1Params.ICNT0 = nCols;

   __SA0_OPEN(sa1Params);
   __SE0_OPEN(pLocalU, seScalarParams);

   seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;

   if (nTiles8 > 0) {
      seMatrixParams.ICNT2         = nTiles8;
      seMatrixParams.DECDIM1_WIDTH = colLimit8;

      saMatrixParams.ICNT2         = nTiles8;
      saMatrixParams.DECDIM1_WIDTH = colLimit8;

      __SE1_OPEN(rStore, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles8; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSum);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSum);
         vec sV2  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSum);
         vec sV3  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSum);
         vec sV4  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSum);
         vec sV5  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSum);
         vec sV6  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSum);
         vec sV7  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSum);
         vec sV8  = __vload_pred(lPred, pLoadVec);

         for (int32_t vertical = 0; vertical < nRows; vertical++) {

            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();
            vec v3 = c7x::strm_eng<1, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();
            vec v5 = c7x::strm_eng<1, vec>::get_adv();
            vec v6 = c7x::strm_eng<1, vec>::get_adv();
            vec v7 = c7x::strm_eng<1, vec>::get_adv();
            vec v8 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup1;
            v2 -= sV2 * scalarDup1;
            v3 -= sV3 * scalarDup1;
            v4 -= sV4 * scalarDup1;
            v5 -= sV5 * scalarDup1;
            v6 -= sV6 * scalarDup1;
            v7 -= sV7 * scalarDup1;
            v8 -= sV8 * scalarDup1;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(rStore);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(rStore);
            __vstore_pred(sPred, pStoreVec, v2);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(rStore);
            __vstore_pred(sPred, pStoreVec, v3);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(rStore);
            __vstore_pred(sPred, pStoreVec, v4);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(rStore);
            __vstore_pred(sPred, pStoreVec, v5);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(rStore);
            __vstore_pred(sPred, pStoreVec, v6);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(rStore);
            __vstore_pred(sPred, pStoreVec, v7);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(rStore);
            __vstore_pred(sPred, pStoreVec, v8);
         }
      }

      __SE1_CLOSE();
      __SA1_CLOSE();
   }

   if (nTiles4 > 0) {

      seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile4;
      seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles4;
      seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit4;

      dataType *pSE1 = rStore + colLimit8;
      dataType *pSA0 = pSum;

      __SE1_OPEN(pSE1, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles4; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec sV2  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec sV3  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec sV4  = __vload_pred(lPred, pLoadVec);

         for (int32_t vertical = 0; vertical < nRows; vertical++) {
            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();
            vec v3 = c7x::strm_eng<1, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup1;
            v2 -= sV2 * scalarDup1;
            v3 -= sV3 * scalarDup1;
            v4 -= sV4 * scalarDup1;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v2);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v3);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v4);
         }
      }

      __SE1_CLOSE();
      __SA1_CLOSE();
   }
   if (nTiles2 > 0) {

      seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile2;
      seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles2;
      seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit2;

      dataType *pSE1 = rStore + colLimit8 + colLimit4;
      dataType *pSA0 = pSum;

      __SE1_OPEN(pSE1, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles2; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec sV2  = __vload_pred(lPred, pLoadVec);

         for (int32_t vertical = 0; vertical < nRows; vertical++) {
            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup1;
            v2 -= sV2 * scalarDup1;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v2);
         }
      }

      __SE1_CLOSE();
      __SA1_CLOSE();
   }
   if (nTiles1 > 0) {

      seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile1;
      seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles1;
      seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit1;

      dataType *pSE1 = rStore + colLimit8 + colLimit4 + colLimit2;
      dataType *pSA0 = pSum;

      __SE1_OPEN(pSE1, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles1; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         for (int32_t vertical = 0; vertical < nRows; vertical++) {
            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            v1 -= sV1 * scalarDup1;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v1);
         }
      }

      __SE1_CLOSE();
      __SA1_CLOSE();
   }
   __SE0_CLOSE();
   __SA0_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}

template void DSPLIB_qrd_R_matrix_exec_ci<float>(float   *pLocalR,
                                                 float   *pLocalU,
                                                 float   *sum,
                                                 float    scale,
                                                 int32_t  colStrideR,
                                                 int32_t  nRows,
                                                 int32_t  nCols,
                                                 uint8_t *pBlock);
template void DSPLIB_qrd_R_matrix_exec_ci<double>(double  *pLocalR,
                                                  double  *pLocalU,
                                                  double  *sum,
                                                  double   scale,
                                                  int32_t  colStrideR,
                                                  int32_t  nRows,
                                                  int32_t  nCols,
                                                  uint8_t *pBlock);

template <typename dataType>
void DSPLIB_qrd_Q_matrix_exec_ci(dataType *pLocalQ,
                                 dataType *pLocalU,
                                 dataType *pSum,
                                 dataType  scale,
                                 int32_t   colStrideQ,
                                 int32_t   nRows,
                                 int32_t   nCols,
                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t eleCount = c7x::element_count_of<vec>::value;

   /************** Q SUM CALC ************/

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE));

   int32_t se0TICNT2 = se0Params.ICNT2;
   int32_t se1TICNT2 = se1Params.ICNT2;

   se0Params.ICNT0 = se1Params.ICNT0 = nCols;

   sa2Params.ICNT0 = nCols;

   dataType *pSE0      = pLocalQ;
   dataType *pSE1Local = pLocalQ + (se0TICNT2 * eleCount * colStrideQ);
   dataType *pSA0      = pSum;
   dataType *pSA1      = pSum + (se0TICNT2 * eleCount);

   vec scaleV = (vec) (scale);

   __SA1_OPEN(sa1Params);
   __SA2_OPEN(sa2Params);
   __SE1_OPEN(pSE1Local, se1Params);
   if (se0TICNT2 > 0) {
      __SA0_OPEN(sa0Params);
      __SE0_OPEN(pSE0, se0Params);
   }

   for (int32_t verticalCnt = 0; verticalCnt < se0TICNT2; verticalCnt++) {
      vec sV1 = (vec) 0;
      vec sV2 = (vec) 0;
      vec sV3 = (vec) 0;
      vec sV4 = (vec) 0;
      vec sV5 = (vec) 0;
      vec sV6 = (vec) 0;
      vec sV7 = (vec) 0;
      vec sV8 = (vec) 0;

      int32_t horizontal = 0;

      for (; horizontal < (nCols) -3; horizontal += 4) {

         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec v4 = c7x::strm_eng<1, vec>::get_adv();

         vec v5 = c7x::strm_eng<0, vec>::get_adv();
         vec v6 = c7x::strm_eng<1, vec>::get_adv();

         vec v7 = c7x::strm_eng<0, vec>::get_adv();
         vec v8 = c7x::strm_eng<1, vec>::get_adv();

         dataType *pU1 = c7x::strm_agen<2, dataType>::get_adv(pLocalU);
         vec       u1  = __vload_dup(pU1);

         dataType *pU2 = c7x::strm_agen<2, dataType>::get_adv(pLocalU);
         vec       u2  = __vload_dup(pU2);

         dataType *pU3 = c7x::strm_agen<2, dataType>::get_adv(pLocalU);
         vec       u3  = __vload_dup(pU3);

         dataType *pU4 = c7x::strm_agen<2, dataType>::get_adv(pLocalU);
         vec       u4  = __vload_dup(pU4);

         sV1 += v1 * u1;
         sV2 += v2 * u1;
         sV3 += v3 * u2;
         sV4 += v4 * u2;
         sV5 += v5 * u3;
         sV6 += v6 * u3;
         sV7 += v7 * u4;
         sV8 += v8 * u4;
      }
      sV5 = sV5 + sV7;
      sV6 = sV6 + sV8;

      for (; horizontal < (nCols) -1; horizontal += 2) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec v4 = c7x::strm_eng<1, vec>::get_adv();

         dataType *pU1 = c7x::strm_agen<2, dataType>::get_adv(pLocalU);
         vec       u1  = __vload_dup(pU1);

         dataType *pU2 = c7x::strm_agen<2, dataType>::get_adv(pLocalU);
         vec       u2  = __vload_dup(pU2);

         sV1 += v1 * u1;
         sV2 += v2 * u1;
         sV3 += v3 * u2;
         sV4 += v4 * u2;
      }
      sV1 = sV1 + sV3;
      sV2 = sV2 + sV4;

      for (; horizontal < (nCols); horizontal++) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         dataType *pU1 = c7x::strm_agen<2, dataType>::get_adv(pLocalU);
         vec       u1  = __vload_dup(pU1);

         sV1 += v1 * u1;
         sV2 += v2 * u1;
      }

      sV1 = sV1 + sV5;
      sV2 = sV2 + sV6;

      __vpred pred1      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec1 = c7x::strm_agen<0, vec>::get_adv(pSA0);
      __vstore_pred(pred1, pStoreVec1, sV1 * scaleV);

      __vpred pred2      = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreVec2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
      __vstore_pred(pred2, pStoreVec2, sV2 * scaleV);
   }

   if (se1TICNT2 != se0TICNT2) {

      vec sV2 = (vec) 0;
      for (int32_t horizontal = 0; horizontal < nCols; horizontal++) {
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         dataType *pU1 = c7x::strm_agen<2, dataType>::get_adv(pLocalU);
         vec       u1  = __vload_dup(pU1);

         sV2 += v2 * u1;
      }
      __vpred pred2      = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreVec2 = c7x::strm_agen<1, vec>::get_adv(pSA1);
      __vstore_pred(pred2, pStoreVec2, sV2 * scaleV);
   }

   __SA1_CLOSE();
   __SA2_CLOSE();
   __SE1_CLOSE();
   if (se0TICNT2 > 0) {
      __SA0_CLOSE();
      __SE0_CLOSE();
   }
   /****************************** Q UPDATION ****************************
    ***********************************************************************/
   dataType *qStore = pLocalQ;

   __SE_TEMPLATE_v1 seScalarParams;
   __SE_TEMPLATE_v1 seMatrixParams;

   __SA_TEMPLATE_v1 saMatrixParams;
   __SA_TEMPLATE_v1 saRefParams;

   seScalarParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (13 * SE_PARAM_SIZE));
   seMatrixParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (16 * SE_PARAM_SIZE));
   saMatrixParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (17 * SE_PARAM_SIZE));
   saRefParams    = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   int32_t lenTile4 = 4;
   int32_t lenTile2 = 2;
   int32_t lenTile1 = 1;

   int32_t nTiles1 = DSPLIB_ceilingDiv(nCols, (eleCount));
   int32_t nTiles4 = nTiles1 / lenTile4;
   nTiles1 -= nTiles4 * lenTile4;
   int32_t nTiles2 = nTiles1 / lenTile2;
   nTiles1 -= nTiles2 * lenTile2;

   int32_t remainingCols = nCols;
   int32_t colLimit4     = nTiles4 * lenTile4 * eleCount;
   colLimit4             = (remainingCols < (colLimit4)) ? remainingCols : colLimit4;

   remainingCols     = remainingCols - colLimit4;
   int32_t colLimit2 = nTiles2 * lenTile2 * eleCount;
   colLimit2         = (remainingCols < (colLimit2)) ? remainingCols : colLimit2;

   int32_t colLimit1 = remainingCols - colLimit2;

   seScalarParams.ICNT0 = nRows;
   seScalarParams.ICNT1 = nTiles4 + nTiles2 + nTiles1;

   seMatrixParams.ICNT1 = nRows;
   saMatrixParams.ICNT1 = nRows;

   saRefParams.ICNT0 = nCols;

   __SE0_OPEN(pSum, seScalarParams);
   __SA0_OPEN(saRefParams);
   if (nTiles4 > 0) {
      seMatrixParams.ICNT2         = nTiles4;
      seMatrixParams.DECDIM1_WIDTH = colLimit4;

      saMatrixParams.ICNT2         = nTiles4;
      saMatrixParams.DECDIM1_WIDTH = colLimit4;

      __SE1_OPEN(qStore, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles4; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLocalU);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLocalU);
         vec sV2  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLocalU);
         vec sV3  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLocalU);
         vec sV4  = __vload_pred(lPred, pLoadVec);

         int32_t vertical = 0;

         for (; vertical < nRows - 3; vertical += 4) {
            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();
            vec scalarDup2 = c7x::strm_eng<0, vec>::get_adv();
            vec scalarDup3 = c7x::strm_eng<0, vec>::get_adv();
            vec scalarDup4 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();
            vec v3 = c7x::strm_eng<1, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();
            vec v5 = c7x::strm_eng<1, vec>::get_adv();
            vec v6 = c7x::strm_eng<1, vec>::get_adv();
            vec v7 = c7x::strm_eng<1, vec>::get_adv();
            vec v8 = c7x::strm_eng<1, vec>::get_adv();

            vec v9  = c7x::strm_eng<1, vec>::get_adv();
            vec v10 = c7x::strm_eng<1, vec>::get_adv();
            vec v11 = c7x::strm_eng<1, vec>::get_adv();
            vec v12 = c7x::strm_eng<1, vec>::get_adv();
            vec v13 = c7x::strm_eng<1, vec>::get_adv();
            vec v14 = c7x::strm_eng<1, vec>::get_adv();
            vec v15 = c7x::strm_eng<1, vec>::get_adv();
            vec v16 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup1;
            v2 -= sV2 * scalarDup1;
            v3 -= sV3 * scalarDup1;
            v4 -= sV4 * scalarDup1;
            v5 -= sV1 * scalarDup2;
            v6 -= sV2 * scalarDup2;
            v7 -= sV3 * scalarDup2;
            v8 -= sV4 * scalarDup2;

            v9 -= sV1 * scalarDup3;
            v10 -= sV2 * scalarDup3;
            v11 -= sV3 * scalarDup3;
            v12 -= sV4 * scalarDup3;
            v13 -= sV1 * scalarDup4;
            v14 -= sV2 * scalarDup4;
            v15 -= sV3 * scalarDup4;
            v16 -= sV4 * scalarDup4;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v2);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v3);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v4);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v5);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v6);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v7);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v8);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v9);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v10);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v11);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v12);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v13);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v14);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v15);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v16);
         }

         for (; vertical < nRows; vertical++) {
            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();
            vec v3 = c7x::strm_eng<1, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup1;
            v2 -= sV2 * scalarDup1;
            v3 -= sV3 * scalarDup1;
            v4 -= sV4 * scalarDup1;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v2);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v3);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(qStore);
            __vstore_pred(sPred, pStoreVec, v4);
         }
      }
      __SE1_CLOSE();
      __SA1_CLOSE();
   }
   if (nTiles2 > 0) {
      seMatrixParams.ICNT2         = nTiles2;
      seMatrixParams.DECDIM1_WIDTH = colLimit2;
      seMatrixParams.ICNT0         = (eleCount * lenTile2);

      saMatrixParams.ICNT2         = nTiles2;
      saMatrixParams.DECDIM1_WIDTH = colLimit2;
      saMatrixParams.ICNT0         = (eleCount * lenTile2);

      dataType *pSE1 = qStore + colLimit4;

      __SE1_OPEN(pSE1, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles2; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLocalU);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLocalU);
         vec sV2  = __vload_pred(lPred, pLoadVec);

         int32_t vertical = 0;

         for (; vertical < nRows - 3; vertical += 4) {
            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();
            vec scalarDup2 = c7x::strm_eng<0, vec>::get_adv();
            vec scalarDup3 = c7x::strm_eng<0, vec>::get_adv();
            vec scalarDup4 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();
            vec v3 = c7x::strm_eng<1, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();
            vec v5 = c7x::strm_eng<1, vec>::get_adv();
            vec v6 = c7x::strm_eng<1, vec>::get_adv();
            vec v7 = c7x::strm_eng<1, vec>::get_adv();
            vec v8 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup1;
            v2 -= sV2 * scalarDup1;
            v3 -= sV1 * scalarDup2;
            v4 -= sV2 * scalarDup2;
            v5 -= sV1 * scalarDup3;
            v6 -= sV2 * scalarDup3;
            v7 -= sV1 * scalarDup4;
            v8 -= sV2 * scalarDup4;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v2);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v3);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v4);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v5);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v6);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v7);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v8);
         }

         for (; vertical < nRows; vertical++) {
            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup1;
            v2 -= sV2 * scalarDup1;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v2);
         }
      }
      __SE1_CLOSE();
      __SA1_CLOSE();
   }
   if (nTiles1 > 0) {
      seMatrixParams.ICNT2         = nTiles1;
      seMatrixParams.DECDIM1_WIDTH = colLimit1;
      seMatrixParams.ICNT0         = (eleCount * lenTile1);

      saMatrixParams.ICNT2         = nTiles1;
      saMatrixParams.DECDIM1_WIDTH = colLimit1;
      saMatrixParams.ICNT0         = (eleCount * lenTile1);

      dataType *pSE1 = qStore + colLimit4 + colLimit2;

      __SE1_OPEN(pSE1, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles1; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pLocalU);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         int32_t vertical = 0;

         for (; vertical < nRows - 3; vertical += 4) {
            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();
            vec scalarDup2 = c7x::strm_eng<0, vec>::get_adv();
            vec scalarDup3 = c7x::strm_eng<0, vec>::get_adv();
            vec scalarDup4 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();
            vec v3 = c7x::strm_eng<1, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup1;
            v2 -= sV1 * scalarDup2;
            v3 -= sV1 * scalarDup3;
            v4 -= sV1 * scalarDup4;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v2);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v3);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v4);
         }
         for (; vertical < nRows; vertical++) {
            vec scalarDup1 = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup1;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSE1);
            __vstore_pred(sPred, pStoreVec, v1);
         }
      }
      __SE1_CLOSE();
      __SA1_CLOSE();
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_Q_matrix_exec_ci<float>(float   *pLocalQ,
                                                 float   *pLocalU,
                                                 float   *sum,
                                                 float    scale,
                                                 int32_t  colStrideQ,
                                                 int32_t  nRows,
                                                 int32_t  nCols,
                                                 uint8_t *pBlock);
template void DSPLIB_qrd_Q_matrix_exec_ci<double>(double  *pLocalQ,
                                                  double  *pLocalU,
                                                  double  *sum,
                                                  double   scale,
                                                  int32_t  colStrideQ,
                                                  int32_t  nRows,
                                                  int32_t  nCols,
                                                  uint8_t *pBlock);
