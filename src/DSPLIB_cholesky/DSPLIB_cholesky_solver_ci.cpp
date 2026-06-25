// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_cholesky_solver_priv.h"

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/

template <typename dataType> void DSPLIB_cholesky_solver_backSubstitution_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_cholesky_solver_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_solver_PrivArgs *) handle;
   uint8_t                         *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                          order        = pKerPrivArgs->order;
   int32_t                          strideL      = pKerPrivArgs->stride;
   int32_t                          colLstride   = strideL / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELEDUP  SE_ELEDUP  = c7x::se_eledup<dataType>::value;

   __SE_TEMPLATE_v1 seDiagReadParams                                = __gen_SE_TEMPLATE_v1();
   seDiagReadParams.ICNT0                                           = 1;
   seDiagReadParams.ICNT1                                           = order;
   seDiagReadParams.DIM1                                            = colLstride + 1;
   seDiagReadParams.DIMFMT                                          = __SE_DIMFMT_2D;
   seDiagReadParams.ELETYPE                                         = SE_ELETYPE;
   seDiagReadParams.VECLEN                                          = __SE_VECLEN_1ELEM;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE)) = seDiagReadParams;

   __SA_TEMPLATE_v1 saWriteXParams                                  = __gen_SA_TEMPLATE_v1();
   saWriteXParams.ICNT0                                             = 1;
   saWriteXParams.ICNT1                                             = order;
   saWriteXParams.DIM1                                              = -1;
   saWriteXParams.DIMFMT                                            = __SA_DIMFMT_2D;
   saWriteXParams.VECLEN                                            = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE)) = saWriteXParams;

   __SE_TEMPLATE_v1 seDivReadParams                                 = __gen_SE_TEMPLATE_v1();
   seDivReadParams.ICNT0                                            = order;
   seDivReadParams.DIMFMT                                           = __SE_DIMFMT_1D;
   seDivReadParams.ELETYPE                                          = SE_ELETYPE;
   seDivReadParams.VECLEN                                           = SE_VECLEN;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = seDivReadParams;

   __SA_TEMPLATE_v1 saDivStoreParams                                = __gen_SA_TEMPLATE_v1();
   saDivStoreParams.ICNT0                                           = order;
   saDivStoreParams.DIMFMT                                          = __SA_DIMFMT_1D;
   saDivStoreParams.VECLEN                                          = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = saDivStoreParams;

   __SE_TEMPLATE_v1 seReadXParams                                   = __gen_SE_TEMPLATE_v1();
   seReadXParams.ICNT0                                              = 1;
   seReadXParams.DIM1                                               = -1;
   seReadXParams.DIMFMT                                             = __SE_DIMFMT_2D;
   seReadXParams.VECLEN                                             = SE_VECLEN;
   seReadXParams.ELETYPE                                            = SE_ELETYPE;
   seReadXParams.ELEDUP                                             = SE_ELEDUP;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE)) = seReadXParams;

   __SE_TEMPLATE_v1 seBlockParams                                   = __gen_SE_TEMPLATE_v1();
   seBlockParams.ICNT0                                              = eleCount;
   seBlockParams.DIM1                                               = -colLstride;
   seBlockParams.DIMFMT                                             = __SE_DIMFMT_2D;
   seBlockParams.ELETYPE                                            = SE_ELETYPE;
   seBlockParams.VECLEN                                             = SE_VECLEN;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE)) = seBlockParams;

   __SA_TEMPLATE_v1 saReverseParams                                 = __gen_SA_TEMPLATE_v1();
   saReverseParams.ICNT0                                            = eleCount;
   saReverseParams.DIM1                                             = -((int32_t) eleCount);
   saReverseParams.DIMFMT                                           = __SA_DIMFMT_2D;
   saReverseParams.VECLEN                                           = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE)) = saReverseParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_cholesky_solver_backSubstitution_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_cholesky_solver_backSubstitution_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_solver_init_ci(DSPLIB_kernelHandle                    handle,
                                             DSPLIB_bufParams2D_t                  *bufParamsU,
                                             DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                             DSPLIB_bufParams1D_t                  *bufParamsY,
                                             DSPLIB_bufParams1D_t                  *bufParamsB,
                                             DSPLIB_bufParams1D_t                  *bufParamsX,
                                             DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                             const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS                    status           = DSPLIB_SUCCESS;
   DSPLIB_cholesky_solver_PrivArgs *pKerPrivArgs     = (DSPLIB_cholesky_solver_PrivArgs *) handle;
   DSPLIB_matTrans_PrivArgs        *matTransPrivArgs = &pKerPrivArgs->matTransPrivArgs;
   DSPLIB_matTransInitArgs          matTransInitArgs;

   DSPLIB_cholesky_solver_backSubstitution_init_ci<dataType>(handle);

   matTransInitArgs.dimX      = pKerPrivArgs->order;
   matTransInitArgs.dimY      = pKerPrivArgs->order;
   matTransInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   matTransPrivArgs->widthIn   = bufParamsU->dim_x;
   matTransPrivArgs->heightIn  = bufParamsU->dim_y;
   matTransPrivArgs->strideIn  = bufParamsU->stride_y;
   matTransPrivArgs->strideOut = bufParamsScratch->stride_y;

   DSPLIB_matTrans_init_ci<dataType>(matTransPrivArgs, bufParamsU, bufParamsScratch, &matTransInitArgs);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_solver_init_ci<float>(DSPLIB_kernelHandle                    handle,
                                                             DSPLIB_bufParams2D_t                  *bufParamsU,
                                                             DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                                             DSPLIB_bufParams1D_t                  *bufParamsY,
                                                             DSPLIB_bufParams1D_t                  *bufParamsB,
                                                             DSPLIB_bufParams1D_t                  *bufParamsX,
                                                             DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                                             const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_cholesky_solver_init_ci<double>(DSPLIB_kernelHandle                    handle,
                                                              DSPLIB_bufParams2D_t                  *bufParamsU,
                                                              DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                                              DSPLIB_bufParams1D_t                  *bufParamsY,
                                                              DSPLIB_bufParams1D_t                  *bufParamsB,
                                                              DSPLIB_bufParams1D_t                  *bufParamsX,
                                                              DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                                              const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/

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
static inline void DSPLIB_cholesky_solver_forwardSubstitution_ci(dataType *pL,
                                                                 dataType *pY,
                                                                 dataType *pB,
                                                                 dataType *pDiv,
                                                                 int32_t   order,
                                                                 int32_t   colLstride,
                                                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 seDivReadParams;
   __SA_TEMPLATE_v1 saDivStoreParams;
   __SE_TEMPLATE_v1 seDiagReadParams;
   __SA_TEMPLATE_v1 saDiagWriteParams;
   seDiagReadParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   saDiagWriteParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   seDivReadParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   saDivStoreParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));

   saDiagWriteParams.DIM1 = 1;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   int32_t                                                nVec     = DSPLIB_ceilingDiv(order, eleCount);

   /* Calculate reciprocals of Diagonal Elements */
   __SE1_OPEN(pL, seDiagReadParams);
   __SA1_OPEN(saDiagWriteParams);
   int32_t row = 0;

   for (row = 0; row < order; row++) {
      vec vecDiag = c7x::strm_eng<1, vec>::get_adv();

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

   /*----------------------------------------------------------------------
    Forward Substitution
    -----------------------------------------------------------------------*/

   __SE_TEMPLATE_v1 seBlockParams;
   __SE_TEMPLATE_v1 seReadYParams;
   __SA_TEMPLATE_v1 saWriteYParams;
   __SA_TEMPLATE_v1 sa1DReadParams;

   saWriteYParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   seReadYParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   seBlockParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   sa1DReadParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   saWriteYParams.DIM1 = 1;
   seReadYParams.DIM1  = 1;
   seBlockParams.DIM1  = colLstride;

   dataType *pSE0 = pL;
   dataType *pSA1 = pY;
   dataType *pSA2 = pB;
   dataType *pSA3 = pDiv;

   __SA1_OPEN(saWriteYParams);
   __SA2_OPEN(sa1DReadParams);
   __SA3_OPEN(sa1DReadParams);

   for (int32_t block = 0; block < nVec; block++) { /* excl - FL: 20  DBL: 20*/

      __vpred predB  = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pLoadB = c7x::strm_agen<2, vec>::get_adv(pSA2);
      vec     vecB   = __vload_pred(predB, pLoadB);

      __vpred predDiv  = c7x::strm_agen<3, vec>::get_vpred();
      vec    *pLoadDiv = c7x::strm_agen<3, vec>::get_adv(pSA3);
      vec     vecDiv   = __vload_pred(predDiv, pLoadDiv);

      int32_t sumRows   = block * eleCount;
      int32_t totalRows = sumRows + eleCount;

      seBlockParams.ICNT1 = totalRows;
      seReadYParams.ICNT1 = sumRows;

      __SE0_OPEN(pSE0, seBlockParams);
      if (sumRows > 0) {
         __SE1_OPEN(pY, seReadYParams);
      }

      /* Calculate sum */
      vec     vecSum   = (vec) 0;
      vec     vecSum1  = (vec) 0;
      vec     vecSum2  = (vec) 0;
      vec     vecSum3  = (vec) 0;
      vec     vecSum4  = (vec) 0;
      int32_t vertical = 0;
      /* FL - 7 + rip_cnt * 4,  DBL - 7 + trip_cnt * 4*/
      for (vertical = 0; vertical < sumRows - 3; vertical += 4) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec y1 = c7x::strm_eng<1, vec>::get_adv();
         vecSum1 += v1 * y1;

         vec v2 = c7x::strm_eng<0, vec>::get_adv();
         vec y2 = c7x::strm_eng<1, vec>::get_adv();
         vecSum2 += v2 * y2;

         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec y3 = c7x::strm_eng<1, vec>::get_adv();
         vecSum3 += v3 * y3;

         vec v4 = c7x::strm_eng<0, vec>::get_adv();
         vec y4 = c7x::strm_eng<1, vec>::get_adv();
         vecSum4 += v4 * y4;
      }

      /* excl - FL: 11  DBL: 11*/
      vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;

      /* Calculate values */
      dataType resultEle1; /* FL - 377, DBL - 169 */
      for (vertical = 0; vertical < eleCount; vertical++) {
         vec v1      = c7x::strm_eng<0, vec>::get_adv();
         vec result1 = (vecB - vecSum) * vecDiv;
         getElement(result1, vertical, &resultEle1);
         vecSum += v1 * (resultEle1);
         __vpred predYCalc = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreY   = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(predYCalc, pStoreY, (vec) resultEle1);
      }
      /* excl - FL: 22  DBL: 22*/
      pSE0 += eleCount;
      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   __SA2_CLOSE();
   __SA3_CLOSE();
   __SA1_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_cholesky_solver_forwardSubstitution_ci<float>(float   *pL,
                                                                   float   *pX,
                                                                   float   *pY,
                                                                   float   *pDiv,
                                                                   int32_t  order,
                                                                   int32_t  colLstride,
                                                                   uint8_t *pBlock);
template void DSPLIB_cholesky_solver_forwardSubstitution_ci<double>(double  *pL,
                                                                    double  *pX,
                                                                    double  *pY,
                                                                    double  *pDiv,
                                                                    int32_t  order,
                                                                    int32_t  colLstride,
                                                                    uint8_t *pBlock);

template <typename dataType>
static inline void DSPLIB_cholesky_solver_backSubstitution_ci(dataType *pL,
                                                              dataType *pX,
                                                              dataType *pY,
                                                              dataType *pDiv,
                                                              int32_t   order,
                                                              int32_t   colLstride,
                                                              uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   /* Calculate reciprocals of Diagonal Elements */

   int32_t totalBlocks  = order / eleCount;
   int32_t remainingEle = order - (totalBlocks * eleCount);

   __SE_TEMPLATE_v1 seBlockParams;
   __SE_TEMPLATE_v1 seReadXParams;
   __SA_TEMPLATE_v1 saWriteXParams;
   __SA_TEMPLATE_v1 saReverseParams;

   saWriteXParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   seReadXParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   seBlockParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));
   saReverseParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));

   seReadXParams.ICNT0   = 1;
   seReadXParams.DIM1    = -1;
   saReverseParams.ICNT1 = totalBlocks;

   dataType *pLLastElem = &pL[(order - 1) + ((order - 1) * colLstride)];
   dataType *pXLastElem = &pX[order - 1];
   dataType *pSE0       = pLLastElem - (eleCount - 1);
   dataType *pSA1       = pX + order - 1;
   dataType *pSA2       = pY + order - eleCount;
   dataType *pSA3       = pDiv + order - eleCount;

   __SA1_OPEN(saWriteXParams);

   if (totalBlocks > 0) {
      __SA2_OPEN(saReverseParams);
      __SA3_OPEN(saReverseParams);

      for (int32_t block = 0; block < totalBlocks; block++) { /* Excl cyc FL: 20 DBL- 20*/
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

         /* FL: 7 + trip_cnt * 4  DBL: 7 + trip_cnt * 4*/
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

         /* Excl cyc FL: 11 DBL: 11 */
         vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;

         /* Calculate values */
         uint32_t vecIndex = eleCount - 1;
         dataType resultEle1;
         /* FL: 337 DBL: 169*/
         for (vertical = 0; vertical < eleCount; vertical++) {
            vec v1      = c7x::strm_eng<0, vec>::get_adv();
            vec result1 = (vecY - vecSum) * vecDiv;
            getElement(result1, vecIndex--, &resultEle1);
            vecSum += v1 * (resultEle1);
            dataType *pStoreX1 = c7x::strm_agen<1, dataType>::get_adv(pSA1);
            *pStoreX1          = resultEle1;
         }
         /* 22 */
         pSE0 -= eleCount;
         __SE0_CLOSE();
         __SE1_CLOSE();
      }
      __SA2_CLOSE();
      __SA3_CLOSE();
   }
   /* Processing Remaining Elements */
   if (remainingEle > 0) {
      seBlockParams.ICNT0 = remainingEle;
      seBlockParams.ICNT1 = order;

      seReadXParams.ICNT1 = order;

      saReverseParams.ICNT0 = remainingEle;
      saReverseParams.ICNT1 = 1;
      saReverseParams.DIM1  = 0;

      pSE0 = &pL[(order - 1) * colLstride];
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
template void DSPLIB_cholesky_solver_backSubstitution_ci<float>(float   *pL,
                                                                float   *pX,
                                                                float   *pY,
                                                                float   *pDiv,
                                                                int32_t  order,
                                                                int32_t  colLstride,
                                                                uint8_t *pBlock);
template void DSPLIB_cholesky_solver_backSubstitution_ci<double>(double  *pL,
                                                                 double  *pX,
                                                                 double  *pY,
                                                                 double  *pDiv,
                                                                 int32_t  order,
                                                                 int32_t  colLstride,
                                                                 uint8_t *pBlock);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_solver_exec_ci(DSPLIB_kernelHandle handle,
                                             void *restrict pU,
                                             void *restrict pScratch,
                                             void *restrict pY,
                                             void *restrict pB,
                                             void *restrict pX,
                                             void *restrict pDiv)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_cholesky_solver_PrivArgs *pKerPrivArgs     = (DSPLIB_cholesky_solver_PrivArgs *) handle;
   DSPLIB_matTrans_PrivArgs        *matTransPrivArgs = &pKerPrivArgs->matTransPrivArgs;

   uint8_t *pBlock     = pKerPrivArgs->bufPblock;
   int32_t  order      = pKerPrivArgs->order;
   int32_t  strideL    = pKerPrivArgs->stride;
   int32_t  colLStride = strideL / sizeof(dataType);

   dataType *pLocalU      = (dataType *) pU;
   dataType *pLocaltransU = (dataType *) pScratch;
   dataType *pLocalY      = (dataType *) pY;
   dataType *pLocalB      = (dataType *) pB;
   dataType *pLocalX      = (dataType *) pX;
   dataType *pLocalDiv    = (dataType *) pDiv;

   DSPLIB_DEBUGPRINTFN(0, "pLocalU: %p pLocalX: %p\n", pLocalU, pLocalX);

   /*-----------------------------------------------------------------------
   solve L*y=b for y using forward substitution
   -------------------------------------------------------------------------*/
   DSPLIB_cholesky_solver_forwardSubstitution_ci<dataType>(pLocalU, pLocalY, pLocalB, pLocalDiv, order, colLStride,
                                                           pBlock);

   /*-----------------------------------------------------------------------
    solve U*x=y for x using backward substitution
    ------------------------------------------------------------------------ */
   DSPLIB_matTrans_exec_ci<dataType>(matTransPrivArgs, pLocalU, pLocaltransU);

   DSPLIB_cholesky_solver_backSubstitution_ci<dataType>(pLocaltransU, pLocalX, pLocalY, pLocalDiv, order, colLStride,
                                                        pBlock);

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cholesky_solver_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                             void *restrict pU,
                                                             void *restrict pScratch,
                                                             void *restrict pY,
                                                             void *restrict pB,
                                                             void *restrict pX,
                                                             void *restrict pDiv);

template DSPLIB_STATUS DSPLIB_cholesky_solver_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                              void *restrict pU,
                                                              void *restrict pScratch,
                                                              void *restrict pY,
                                                              void *restrict pB,
                                                              void *restrict pX,
                                                              void *restrict pDiv);
