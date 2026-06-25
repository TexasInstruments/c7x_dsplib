// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_common.h"
#include "DSPLIB_qrd_solver_priv.h"

#include <cstring>

template <typename dataType> void DSPLIB_qrd_solver_y_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_qrd_solver_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_solver_PrivArgs *) handle;
   uint8_t                    *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                     nCols        = pKerPrivArgs->widthR;
   int32_t                     nRows        = pKerPrivArgs->widthR;
   int32_t                     strideQ      = pKerPrivArgs->strideQ;
   int32_t                     colStrideQ   = strideQ / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t  lenTile8 = 8;
   uint32_t eleCount = c7x::element_count_of<vec>::value;
   int32_t  nTiles_8 = DSPLIB_ceilingDiv(nCols, (eleCount * lenTile8));

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELEDUP  SE_ELEDUP  = c7x::se_eledup<dataType>::value;

   __SE_TEMPLATE_v1 seScalarParams = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 seMatrixParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saMatrixParams = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saRefParams    = __gen_SA_TEMPLATE_v1();

   seScalarParams.DIM1    = 0;
   seScalarParams.ELEDUP  = SE_ELEDUP;
   seScalarParams.DIMFMT  = __SE_DIMFMT_2D;
   seScalarParams.VECLEN  = SE_VECLEN;
   seScalarParams.ELETYPE = SE_ELETYPE;
   seScalarParams.ICNT1   = nTiles_8;
   seScalarParams.ICNT0   = nRows;

   seMatrixParams.ICNT0         = (eleCount * lenTile8);
   seMatrixParams.DIM1          = colStrideQ;
   seMatrixParams.DIM2          = (eleCount * lenTile8);
   seMatrixParams.DIMFMT        = __SE_DIMFMT_3D;
   seMatrixParams.ELETYPE       = SE_ELETYPE;
   seMatrixParams.VECLEN        = SE_VECLEN;
   seMatrixParams.DECDIM1       = __SE_DECDIM_DIM2;
   seMatrixParams.ICNT2         = nTiles_8;
   seMatrixParams.DECDIM1_WIDTH = nCols;
   seMatrixParams.ICNT1         = nRows;

   saMatrixParams.ICNT0         = (eleCount * lenTile8);
   saMatrixParams.DIM1          = colStrideQ;
   saMatrixParams.DIM2          = (eleCount * lenTile8);
   saMatrixParams.DIMFMT        = __SA_DIMFMT_3D;
   saMatrixParams.VECLEN        = SA_VECLEN;
   saMatrixParams.DECDIM1       = __SA_DECDIM_DIM2;
   saMatrixParams.ICNT2         = nTiles_8;
   saMatrixParams.DECDIM1_WIDTH = nCols;
   saMatrixParams.ICNT1         = nRows;

   saRefParams.ICNT0  = nCols;
   saRefParams.DIMFMT = __SA_DIMFMT_1D;
   saRefParams.VECLEN = SA_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE)) = seScalarParams;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE)) = seMatrixParams;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = saMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = saRefParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_solver_y_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_qrd_solver_y_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType>
void DSPLIB_qrd_solver_y_exec_ci(dataType *pLocalQ,
                                 int32_t   nCols,
                                 int32_t   nRows,
                                 dataType *pLocalB,
                                 dataType *pLocalY,
                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 seScalarParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 seMatrixParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saMatrixParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saRefParams    = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   int32_t lenTile8 = 8;
   int32_t nTiles_8 = DSPLIB_ceilingDiv(nCols, (eleCount * lenTile8));

   __SE0_OPEN(pLocalB, seScalarParams);
   __SE1_OPEN(pLocalQ, seMatrixParams);
   __SA1_OPEN(saMatrixParams);
   __SA0_OPEN(saRefParams);
   __SA2_OPEN(saRefParams);

   for (int32_t tile = 0; tile < nTiles_8; tile++) {
      vec sV1 = (vec) 0;
      vec sV2 = (vec) 0;
      vec sV3 = (vec) 0;
      vec sV4 = (vec) 0;
      vec sV5 = (vec) 0;
      vec sV6 = (vec) 0;
      vec sV7 = (vec) 0;
      vec sV8 = (vec) 0;
      for (int32_t vertical = 0; vertical < nRows; vertical++) {
         vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

         vec v1 = c7x::strm_eng<1, vec>::get_adv();
         vec v2 = c7x::strm_eng<1, vec>::get_adv();
         vec v3 = c7x::strm_eng<1, vec>::get_adv();
         vec v4 = c7x::strm_eng<1, vec>::get_adv();
         vec v5 = c7x::strm_eng<1, vec>::get_adv();
         vec v6 = c7x::strm_eng<1, vec>::get_adv();
         vec v7 = c7x::strm_eng<1, vec>::get_adv();
         vec v8 = c7x::strm_eng<1, vec>::get_adv();

         sV1 += v1 * scalarDup;
         sV2 += v2 * scalarDup;
         sV3 += v3 * scalarDup;
         sV4 += v4 * scalarDup;
         sV5 += v5 * scalarDup;
         sV6 += v6 * scalarDup;
         sV7 += v7 * scalarDup;
         sV8 += v8 * scalarDup;
      }

      __vpred lPred = c7x::strm_agen<2, vec>::get_vpred();
      vec    *psV   = c7x::strm_agen<2, vec>::get_adv(pLocalY);
      __vstore_pred(lPred, psV, sV1);

      lPred = c7x::strm_agen<2, vec>::get_vpred();
      psV   = c7x::strm_agen<2, vec>::get_adv(pLocalY);
      __vstore_pred(lPred, psV, sV2);

      lPred = c7x::strm_agen<2, vec>::get_vpred();
      psV   = c7x::strm_agen<2, vec>::get_adv(pLocalY);
      __vstore_pred(lPred, psV, sV3);

      lPred = c7x::strm_agen<2, vec>::get_vpred();
      psV   = c7x::strm_agen<2, vec>::get_adv(pLocalY);
      __vstore_pred(lPred, psV, sV4);

      lPred = c7x::strm_agen<2, vec>::get_vpred();
      psV   = c7x::strm_agen<2, vec>::get_adv(pLocalY);
      __vstore_pred(lPred, psV, sV5);

      lPred = c7x::strm_agen<2, vec>::get_vpred();
      psV   = c7x::strm_agen<2, vec>::get_adv(pLocalY);
      __vstore_pred(lPred, psV, sV6);

      lPred = c7x::strm_agen<2, vec>::get_vpred();
      psV   = c7x::strm_agen<2, vec>::get_adv(pLocalY);
      __vstore_pred(lPred, psV, sV7);

      lPred = c7x::strm_agen<2, vec>::get_vpred();
      psV   = c7x::strm_agen<2, vec>::get_adv(pLocalY);
      __vstore_pred(lPred, psV, sV8);
   }
   __SE0_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_solver_y_exec_ci<float>(float   *pLocalQ,
                                                 int32_t  nCols,
                                                 int32_t  nRows,
                                                 float   *pLocalB,
                                                 float   *pLocalY,
                                                 uint8_t *pBlock);
template void DSPLIB_qrd_solver_y_exec_ci<double>(double  *pLocalQ,
                                                  int32_t  nCols,
                                                  int32_t  nRows,
                                                  double  *pLocalB,
                                                  double  *pLocalY,
                                                  uint8_t *pBlock);

template <typename dataType> void DSPLIB_qrd_solver_backSubstitution_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_qrd_solver_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_solver_PrivArgs *) handle;
   uint8_t                    *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                     nRows        = pKerPrivArgs->widthR;
   int32_t                     strideR      = pKerPrivArgs->strideR;
   int32_t                     colRstride   = strideR / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   __SE_TEMPLATE_v1 seDivReadParams = __gen_SE_TEMPLATE_v1();
   seDivReadParams.ICNT0            = nRows;
   seDivReadParams.DIMFMT           = __SE_DIMFMT_1D;
   seDivReadParams.ELETYPE          = SE_ELETYPE;
   seDivReadParams.VECLEN           = SE_VECLEN;

   __SE_TEMPLATE_v1 seBlockParams   = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saWriteXParams  = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saReverseParams = __gen_SA_TEMPLATE_v1();

   seBlockParams.ICNT0   = eleCount;
   seBlockParams.DIM1    = -colRstride;
   seBlockParams.DIMFMT  = __SE_DIMFMT_2D;
   seBlockParams.ELETYPE = SE_ELETYPE;
   seBlockParams.VECLEN  = SE_VECLEN;

   saWriteXParams.ICNT0  = 1;
   saWriteXParams.ICNT1  = nRows;
   saWriteXParams.DIM1   = -1;
   saWriteXParams.DIMFMT = __SA_DIMFMT_2D;
   saWriteXParams.VECLEN = SA_VECLEN;

   saReverseParams.ICNT0  = eleCount;
   saReverseParams.DIM1   = -eleCount;
   saReverseParams.DIMFMT = __SA_DIMFMT_2D;
   saReverseParams.VECLEN = SA_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE)) = seDivReadParams;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE)) = seBlockParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE)) = saWriteXParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE)) = saReverseParams;

   __SE_TEMPLATE_v1 seDiagReadParams                                = __gen_SE_TEMPLATE_v1();
   seDiagReadParams.ICNT0                                           = 1;
   seDiagReadParams.ICNT1                                           = nRows;
   seDiagReadParams.DIM1                                            = colRstride + 1;
   seDiagReadParams.DIMFMT                                          = __SE_DIMFMT_2D;
   seDiagReadParams.ELETYPE                                         = SE_ELETYPE;
   seDiagReadParams.VECLEN                                          = __SE_VECLEN_1ELEM;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE)) = seDiagReadParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_solver_backSubstitution_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_qrd_solver_backSubstitution_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType, typename V = typename c7x::make_full_vector<dataType>::type>
inline void                       getElement(V inVec, uint32_t index, dataType *element);
template <typename V> inline void getElement(V inVec, uint32_t index, float *element)
{
   *element = __as_float(__vgetw_vrd(c7x::as_int_vec(inVec), index));
}

template <typename V> inline void getElement(V inVec, uint32_t index, double *element)
{
   *element = __as_double(__vgetd_vrd(c7x::as_long_vec(inVec), index));
}

template <typename dataType>
void DSPLIB_qrd_solver_backSubstitution_ci(dataType *pR,
                                           dataType *pX,
                                           dataType *pY,
                                           dataType *pDiv,
                                           int32_t   nRows,
                                           int32_t   nCols,
                                           int32_t   colRstride,
                                           uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 seDivReadParams;
   __SA_TEMPLATE_v1 saDivStoreParams;
   __SE_TEMPLATE_v1 seDiagReadParams;
   __SA_TEMPLATE_v1 saDiagWriteParams;
   saDivStoreParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   seDivReadParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   saDiagWriteParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));
   seDiagReadParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE));

   saDiagWriteParams.DIM1 = 1;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   int32_t                                                nVec     = DSPLIB_ceilingDiv(nRows, eleCount);

   /* Calculate reciprocals of Diagonal Elements */

   __SE1_OPEN(pR, seDiagReadParams);
   __SA1_OPEN(saDiagWriteParams);
   int32_t row = 0;

   for (row = 0; row < nRows; row++) {
      vec     vecDiag    = c7x::strm_eng<1, vec>::get_adv();
      __vpred predDiag   = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreDiag = c7x::strm_agen<1, vec>::get_adv(pDiv);
      __vstore_pred(predDiag, pStoreDiag, vecDiag);
   }
   __SE1_CLOSE();
   __SA1_CLOSE();

   __SE0_OPEN(pDiv, seDivReadParams);
   __SA0_OPEN(saDivStoreParams);
   dataType TwoP0 = 2.0;
   int32_t  ii    = 0;

   for (ii = 0; ii < nVec - 3; ii += 4) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<0, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<0, vec>::get_adv();

      vec yy1 = __recip(v1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);

      vec yy2 = __recip(v2);
      yy2     = yy2 * (TwoP0 - v2 * yy2);
      yy2     = yy2 * (TwoP0 - v2 * yy2);

      vec yy3 = __recip(v3);
      yy3     = yy3 * (TwoP0 - v3 * yy3);
      yy3     = yy3 * (TwoP0 - v3 * yy3);

      vec yy4 = __recip(v4);
      yy4     = yy4 * (TwoP0 - v4 * yy4);
      yy4     = yy4 * (TwoP0 - v4 * yy4);

      __vpred predDiv1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv1 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv1, pStoreDiv1, yy1);

      __vpred predDiv2   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv2 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv2, pStoreDiv2, yy2);

      __vpred predDiv3   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv3 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv3, pStoreDiv3, yy3);

      __vpred predDiv4   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv4 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv4, pStoreDiv4, yy4);
   }

   for (; ii < nVec; ii++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();

      vec yy1 = __recip(v1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);

      __vpred predDiv1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv1 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv1, pStoreDiv1, yy1);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   /* Back Substitution */
   int32_t totalBlocks  = nCols / eleCount;
   int32_t remainingEle = nCols - (totalBlocks * eleCount);

   __SE_TEMPLATE_v1 seBlockParams;
   __SE_TEMPLATE_v1 seReadXParams;
   __SA_TEMPLATE_v1 saWriteXParams;
   __SA_TEMPLATE_v1 saReverseParams;

   seReadXParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   seBlockParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   saWriteXParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));
   saReverseParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));

   seReadXParams.ICNT0   = 1;
   seReadXParams.DIM1    = -1;
   saReverseParams.ICNT1 = totalBlocks;

   dataType *pRLastElem = &pR[(nRows - 1) + ((nRows - 1) * colRstride)];
   dataType *pXLastElem = &pX[nRows - 1];
   dataType *pSE0       = pRLastElem - (eleCount - 1);
   dataType *pSA1       = pX + nRows - 1;
   dataType *pSA2       = pY + nRows - eleCount;
   dataType *pSA3       = pDiv + nRows - eleCount;

   __SA1_OPEN(saWriteXParams);
   if (totalBlocks) {
      __SA2_OPEN(saReverseParams);
      __SA3_OPEN(saReverseParams);
   }

   for (int32_t block = 0; block < totalBlocks; block++) {
      __vpred predY  = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pLoadY = c7x::strm_agen<2, vec>::get_adv(pSA2);
      vec     vecY   = __vload_pred(predY, pLoadY);

      __vpred predDiv  = c7x::strm_agen<3, vec>::get_vpred();
      vec    *pLoadDiv = c7x::strm_agen<3, vec>::get_adv(pSA3);
      vec     vecDiv   = __vload_pred(predDiv, pLoadDiv);

      int32_t sumRows   = block * eleCount;
      int32_t totalRows = sumRows + eleCount;

      seBlockParams.ICNT1 = totalRows;
      seReadXParams.ICNT1 = sumRows;

      __SE0_OPEN(pSE0, seBlockParams);
      if (sumRows > 0) {
         __SE1_OPEN(pXLastElem, seReadXParams);
      }

      /* Calculate sum */
      vec     vecSum   = (vec) 0;
      vec     vecSum1  = (vec) 0;
      vec     vecSum2  = (vec) 0;
      vec     vecSum3  = (vec) 0;
      vec     vecSum4  = (vec) 0;
      int32_t vertical = 0;

      for (vertical = 0; vertical < sumRows - 3; vertical += 4) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec x1 = c7x::strm_eng<1, vec>::get_adv();
         vecSum1 += v1 * x1;

         vec v2 = c7x::strm_eng<0, vec>::get_adv();
         vec x2 = c7x::strm_eng<1, vec>::get_adv();
         vecSum2 += v2 * x2;

         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec x3 = c7x::strm_eng<1, vec>::get_adv();
         vecSum3 += v3 * x3;

         vec v4 = c7x::strm_eng<0, vec>::get_adv();
         vec x4 = c7x::strm_eng<1, vec>::get_adv();
         vecSum4 += v4 * x4;
      }

      vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;
      /* Calculate values */
      uint32_t vecIndex = eleCount - 1;
      dataType resultEle1, resultEle2, resultEle3, resultEle4;

      for (vertical = 0; vertical < eleCount - 3; vertical += 4) {
         vec v1      = c7x::strm_eng<0, vec>::get_adv();
         vec result1 = (vecY - vecSum) * vecDiv;
         getElement(result1, vecIndex--, &resultEle1);
         vecSum += v1 * (resultEle1);
         dataType *pStoreX1 = c7x::strm_agen<1, dataType>::get_adv(pSA1);
         *pStoreX1          = resultEle1;

         vec v2      = c7x::strm_eng<0, vec>::get_adv();
         vec result2 = (vecY - vecSum) * vecDiv;
         getElement(result2, vecIndex--, &resultEle2);
         vecSum += v2 * (resultEle2);
         dataType *pStoreX2 = c7x::strm_agen<1, dataType>::get_adv(pSA1);
         *pStoreX2          = resultEle2;

         vec v3      = c7x::strm_eng<0, vec>::get_adv();
         vec result3 = (vecY - vecSum) * vecDiv;
         getElement(result3, vecIndex--, &resultEle3);
         vecSum += v3 * (resultEle3);
         dataType *pStoreX3 = c7x::strm_agen<1, dataType>::get_adv(pSA1);
         *pStoreX3          = resultEle3;

         vec v4      = c7x::strm_eng<0, vec>::get_adv();
         vec result4 = (vecY - vecSum) * vecDiv;
         getElement(result4, vecIndex--, &resultEle4);
         vecSum += v4 * (resultEle4);
         dataType *pStoreX4 = c7x::strm_agen<1, dataType>::get_adv(pSA1);
         *pStoreX4          = resultEle4;
      }

      pSE0 -= eleCount;
      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   if (totalBlocks) {
      __SA2_CLOSE();
      __SA3_CLOSE();
   }

   /* Processing Remaining Elements */
   if (remainingEle > 0) {
      seBlockParams.ICNT0 = remainingEle;
      seBlockParams.ICNT1 = nRows;

      seReadXParams.ICNT1 = nRows;

      saReverseParams.ICNT0 = remainingEle;
      saReverseParams.ICNT1 = 1;
      saReverseParams.DIM1  = 0;

      pSE0 = &pR[(nRows - 1) * colRstride];
      __SE0_OPEN(pSE0, seBlockParams);
      __SE1_OPEN(pXLastElem, seReadXParams);
      __SA2_OPEN(saReverseParams);
      __SA3_OPEN(saReverseParams);

      int32_t sumRows = totalBlocks * eleCount;

      __vpred predY  = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pLoadY = c7x::strm_agen<2, vec>::get_adv(pY);
      vec     vecY   = __vload_pred(predY, pLoadY);

      __vpred predDiv  = c7x::strm_agen<3, vec>::get_vpred();
      vec    *pLoadDiv = c7x::strm_agen<3, vec>::get_adv(pDiv);
      vec     vecDiv   = __vload_pred(predDiv, pLoadDiv);

      vec     vecSum   = (vec) 0;
      vec     vecSum1  = (vec) 0;
      vec     vecSum2  = (vec) 0;
      vec     vecSum3  = (vec) 0;
      vec     vecSum4  = (vec) 0;
      int32_t vertical = 0;

      for (vertical = 0; vertical < sumRows - 3; vertical += 4) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec x1 = c7x::strm_eng<1, vec>::get_adv();
         vecSum1 += v1 * x1;

         vec v2 = c7x::strm_eng<0, vec>::get_adv();
         vec x2 = c7x::strm_eng<1, vec>::get_adv();
         vecSum2 += v2 * x2;

         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec x3 = c7x::strm_eng<1, vec>::get_adv();
         vecSum3 += v3 * x3;

         vec v4 = c7x::strm_eng<0, vec>::get_adv();
         vec x4 = c7x::strm_eng<1, vec>::get_adv();
         vecSum4 += v4 * x4;
      }

      vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;

      int32_t  vecIndex = remainingEle - 1;
      dataType resultEle;

      for (vertical = 0; vertical < remainingEle; vertical++) {
         vec v1     = c7x::strm_eng<0, vec>::get_adv();
         vec result = (vecY - vecSum) * vecDiv;

         getElement(result, vecIndex--, &resultEle);
         vecSum += v1 * (resultEle);

         dataType *pStoreX = c7x::strm_agen<1, dataType>::get_adv(pSA1);
         *pStoreX          = resultEle;
      }

      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA2_CLOSE();
      __SA3_CLOSE();
   }
   __SA1_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_qrd_solver_backSubstitution_ci<float>(float   *pR,
                                                           float   *pX,
                                                           float   *pY,
                                                           float   *pDiv,
                                                           int32_t  nRows,
                                                           int32_t  nCols,
                                                           int32_t  colRstride,
                                                           uint8_t *pBlock);
template void DSPLIB_qrd_solver_backSubstitution_ci<double>(double  *pR,
                                                            double  *pX,
                                                            double  *pY,
                                                            double  *pDiv,
                                                            int32_t  nRows,
                                                            int32_t  nCols,
                                                            int32_t  colRstride,
                                                            uint8_t *pBlock);

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_solver_init_ci(DSPLIB_kernelHandle             handle,
                                        DSPLIB_bufParams2D_t           *bufParamsQ,
                                        DSPLIB_bufParams2D_t           *bufParamsR,
                                        DSPLIB_bufParams1D_t           *bufParamsB,
                                        DSPLIB_bufParams1D_t           *bufParamsY,
                                        DSPLIB_bufParams1D_t           *bufParamsX,
                                        const DSPLIB_qrdSolverInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_qrd_solver_PrivArgs *pKerPrivArgs         = (DSPLIB_qrd_solver_PrivArgs *) handle;
   DSPLIB_matTrans_PrivArgs   *pMatTransKerPrivArgs = &pKerPrivArgs->pMatTransKerPrivArgs;

   DSPLIB_matTransInitArgs kerInitArgsMatTrans;

   kerInitArgsMatTrans.funcStyle = pKerInitArgs->funcStyle;
   kerInitArgsMatTrans.dimX      = bufParamsR->dim_x;
   kerInitArgsMatTrans.dimY      = bufParamsR->dim_y;

   pMatTransKerPrivArgs->widthIn   = bufParamsR->dim_x;
   pMatTransKerPrivArgs->heightIn  = bufParamsR->dim_y;
   pMatTransKerPrivArgs->strideIn  = bufParamsR->stride_y;
   pMatTransKerPrivArgs->strideOut = bufParamsR->stride_y;

   DSPLIB_qrd_solver_y_init_ci<dataType>(handle);
   DSPLIB_matTrans_init_ci<dataType>(pMatTransKerPrivArgs, bufParamsR, bufParamsR, &kerInitArgsMatTrans);
   DSPLIB_qrd_solver_backSubstitution_init_ci<dataType>(handle);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}
template DSPLIB_STATUS DSPLIB_qrd_solver_init_ci<float>(DSPLIB_kernelHandle             handle,
                                                        DSPLIB_bufParams2D_t           *bufParamsQ,
                                                        DSPLIB_bufParams2D_t           *bufParamsR,
                                                        DSPLIB_bufParams1D_t           *bufParamsB,
                                                        DSPLIB_bufParams1D_t           *bufParamsY,
                                                        DSPLIB_bufParams1D_t           *bufParamsX,
                                                        const DSPLIB_qrdSolverInitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_qrd_solver_init_ci<double>(DSPLIB_kernelHandle             handle,
                                                         DSPLIB_bufParams2D_t           *bufParamsQ,
                                                         DSPLIB_bufParams2D_t           *bufParamsR,
                                                         DSPLIB_bufParams1D_t           *bufParamsB,
                                                         DSPLIB_bufParams1D_t           *bufParamsY,
                                                         DSPLIB_bufParams1D_t           *bufParamsX,
                                                         const DSPLIB_qrdSolverInitArgs *pKerInitArgs);

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_solver_exec_ci(DSPLIB_kernelHandle handle,
                                        void *restrict pQ,
                                        void *restrict pR,
                                        void *restrict pB,
                                        void *restrict pY,
                                        void *restrict pX,
                                        void *restrict pD,
                                        void *restrict pR1)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_qrd_solver_PrivArgs *pKerPrivArgs         = (DSPLIB_qrd_solver_PrivArgs *) handle;
   DSPLIB_matTrans_PrivArgs   *pMatTransKerPrivArgs = &pKerPrivArgs->pMatTransKerPrivArgs;
   int32_t                     nRows                = pKerPrivArgs->heightR;
   int32_t                     nCols                = pKerPrivArgs->widthR;
   int32_t                     strideR              = pKerPrivArgs->strideR;
   int32_t                     dataSize             = sizeof(dataType);
   uint8_t                    *pBlock               = pKerPrivArgs->bufPblock;
   int32_t                     loopCnt;

   /* Typecast void pointers to respective data type */
   dataType *pLocalQ  = (dataType *) pQ;
   dataType *pLocalR  = (dataType *) pR;
   dataType *pLocalB  = (dataType *) pB;
   dataType *pLocalY  = (dataType *) pY;
   dataType *pLocalX  = (dataType *) pX;
   dataType *pLocalD  = (dataType *) pD;
   dataType *pLocalR1 = (dataType *) pR1;

   int32_t colRstride = strideR / dataSize;
   DSPLIB_DEBUGPRINTFN(0, "pLocalQ: %p pLocalR: %p pLocalB: %p pLocalY: %p pLocalX: %p  nCols: %d nRows: %d\n", pLocalQ,
                       pLocalR, pLocalB, pLocalY, pLocalX, nCols, nRows);

   /* ------------------------------------------------------------------- */
   /*  Write each column of 'pLocal' to a row of 'pLocalX'.           */
   /* ------------------------------------------------------------------- */

   DSPLIB_qrd_solver_y_exec_ci<dataType>(pLocalQ, nCols, nRows, pLocalB, pLocalY, pBlock);

   /* use backward substitution to solve x=inv(R)*y */
   DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs, pLocalR, pLocalR1);

   memset(pLocalX, 0, sizeof(dataType) * nCols);
   loopCnt = nCols;
   DSPLIB_qrd_solver_backSubstitution_ci<dataType>(pLocalR1, pLocalX, pLocalY, pLocalD, loopCnt, loopCnt, colRstride,
                                                   pBlock);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}
// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_qrd_solver_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                        void *restrict pQ,
                                                        void *restrict pR,
                                                        void *restrict pB,
                                                        void *restrict pY,
                                                        void *restrict pX,
                                                        void *restrict pD,
                                                        void *restrict pR1);

template DSPLIB_STATUS DSPLIB_qrd_solver_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                         void *restrict pQ,
                                                         void *restrict pR,
                                                         void *restrict pB,
                                                         void *restrict pY,
                                                         void *restrict pX,
                                                         void *restrict pD,
                                                         void *restrict pR1);
