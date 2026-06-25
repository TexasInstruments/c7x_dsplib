// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_qrd_common.h"
#include "DSPLIB_qrd_priv.h"

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/
template <typename dataType> void DSPLIB_qrd_alpha_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_qrd_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   __SE_TEMPLATE_v1     se0Params    = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1     sa0Params    = __gen_SA_TEMPLATE_v1();
   int32_t              strideR      = pKerPrivArgs->strideR;
   int32_t              colStrideR   = strideR / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;

   se0Params.ICNT0   = 1;
   se0Params.ICNT1   = eleCount;
   se0Params.DIM1    = colStrideR;
   se0Params.DIM2    = colStrideR * eleCount;
   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   if (sizeof(dataType) == 4) {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   sa0Params.DIMFMT = __SA_DIMFMT_1D;
   sa0Params.VECLEN = SA_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE)) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_alpha_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_qrd_alpha_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_init_ci(DSPLIB_kernelHandle       handle,
                                 DSPLIB_bufParams2D_t     *bufParamsA,
                                 DSPLIB_bufParams2D_t     *bufParamsQ,
                                 DSPLIB_bufParams2D_t     *bufParamsR,
                                 DSPLIB_bufParams1D_t     *bufParamsU,
                                 const DSPLIB_qrdInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS        status       = DSPLIB_SUCCESS;
   DSPLIB_qrd_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_PrivArgs *) handle;
   DSPLIB_qrd_blk_move_init_ci<dataType>(pKerPrivArgs->heightA, pKerPrivArgs->widthA, pKerPrivArgs->strideR,
                                         &pKerPrivArgs->bufPblock[18 * SE_PARAM_SIZE]);
   DSPLIB_qrd_identity_matrix_generate_init_ci<dataType>(pKerPrivArgs->heightA, pKerPrivArgs->strideQ,
                                                         pKerPrivArgs->bufPblock);
   DSPLIB_qrd_alpha_init_ci<dataType>(handle);
   DSPLIB_R_column_init_ci<dataType>(handle);
   DSPLIB_Q_matrix_init_ci<dataType>(handle);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return status;
}
template DSPLIB_STATUS DSPLIB_qrd_init_ci<float>(DSPLIB_kernelHandle       handle,
                                                 DSPLIB_bufParams2D_t     *bufParamsA,
                                                 DSPLIB_bufParams2D_t     *bufParamsQ,
                                                 DSPLIB_bufParams2D_t     *bufParamsR,
                                                 DSPLIB_bufParams1D_t     *bufParamsU,
                                                 const DSPLIB_qrdInitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_qrd_init_ci<double>(DSPLIB_kernelHandle       handle,
                                                  DSPLIB_bufParams2D_t     *bufParamsA,
                                                  DSPLIB_bufParams2D_t     *bufParamsQ,
                                                  DSPLIB_bufParams2D_t     *bufParamsR,
                                                  DSPLIB_bufParams1D_t     *bufParamsU,
                                                  const DSPLIB_qrdInitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/
template <typename vec> static inline vec getSqrt(vec a)
{
   vec     zeroVec  = (vec) 0;
   vec     Half     = (vec) 0.5f;
   vec     OneP5    = (vec) 1.5f;
   __vpred zeroPred = __cmp_eq_pred(zeroVec, a);
   vec     nonZero  = (vec) 1e-12f;

   a = __select(zeroPred, nonZero, a);

   vec x = __recip_sqrt(a);

   vec tmp  = x * a;
   vec tmp1 = OneP5 - tmp * x * Half;
   vec y    = x * tmp1;

   tmp  = y * a;
   tmp1 = OneP5 - tmp * y * Half;
   y    = a * y * tmp1;

   return (y);
}

template <typename dataType>
static inline dataType DSPLIB_qrd_alpha_exec_ci(dataType        *pR,
                                                int32_t          colStrideR,
                                                int32_t          nRows,
                                                dataType        *pU,
                                                __SE_TEMPLATE_v1 se0Params,
                                                __SA_TEMPLATE_v1 sa0Params)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   dataType         sum       = 0;
   __SE_TEMPLATE_v1 se1Params = se0Params;
   __SA_TEMPLATE_v1 sa1Params = sa0Params;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;

   int32_t nVec         = nRows / eleCount;
   int32_t se0ICNT2     = nVec / 2;
   int32_t se1ICNT2_int = nVec - se0ICNT2;
   int32_t remainingEle = nRows - (nVec * eleCount);
   int32_t se1ICNT2     = (remainingEle > 0) ? se1ICNT2_int + 1 : se1ICNT2_int;
   se0Params.ICNT2      = se0ICNT2;
   se1Params.ICNT2      = se1ICNT2;

   dataType *pSE0 = pR;
   dataType *pSE1 = pR + (se0ICNT2 * colStrideR * eleCount);

   __SE1_OPEN(pSE1, se1Params);
   if (se0ICNT2 > 0) {
      __SE0_OPEN(pSE0, se0Params);
   }

   int32_t iterloop1 = se0ICNT2 / 3;
   int32_t vertical  = iterloop1 * 3;
   sa0Params.ICNT0   = (se0ICNT2 * eleCount);
   sa1Params.ICNT0   = nRows - ((se0ICNT2 * eleCount));
   dataType *pUHalf  = pU + (se0ICNT2 * eleCount);
   if (sa0Params.ICNT0) {
      __SA0_OPEN(sa0Params);
   }
   __SA1_OPEN(sa1Params);

   vec sumVec1 = (vec) 0;
   vec sumVec2 = (vec) 0;
   vec sumVec3 = (vec) 0;
   vec sumVec4 = (vec) 0;
   vec sumVec5 = (vec) 0;
   vec sumVec6 = (vec) 0;

   for (int32_t iter = 0; iter < iterloop1; iter++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<1, vec>::get_adv();
      vec v5 = c7x::strm_eng<0, vec>::get_adv();
      vec v6 = c7x::strm_eng<1, vec>::get_adv();

      __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pU);
      __vstore_pred(pred, pStoreVec, v1);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pUHalf);
      __vstore_pred(pred, pStoreVec, v2);

      pred      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pU);
      __vstore_pred(pred, pStoreVec, v3);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pUHalf);
      __vstore_pred(pred, pStoreVec, v4);

      pred      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pU);
      __vstore_pred(pred, pStoreVec, v5);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pUHalf);
      __vstore_pred(pred, pStoreVec, v6);

      sumVec1 += v1 * v1;
      sumVec2 += v2 * v2;
      sumVec3 += v3 * v3;
      sumVec4 += v4 * v4;
      sumVec5 += v5 * v5;
      sumVec6 += v6 * v6;
   }
   sumVec5 += sumVec6;

   for (; vertical < se0ICNT2 - 1; vertical += 2) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<1, vec>::get_adv();

      __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pU);
      __vstore_pred(pred, pStoreVec, v1);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pUHalf);
      __vstore_pred(pred, pStoreVec, v2);

      pred      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pU);
      __vstore_pred(pred, pStoreVec, v3);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pUHalf);
      __vstore_pred(pred, pStoreVec, v4);

      sumVec1 += v1 * v1;
      sumVec2 += v2 * v2;
      sumVec3 += v3 * v3;
      sumVec4 += v4 * v4;
   }
   sumVec3 += sumVec4;
   for (; vertical < se0ICNT2; vertical++) {
      vec     v1        = c7x::strm_eng<0, vec>::get_adv();
      vec     v2        = c7x::strm_eng<1, vec>::get_adv();
      __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv((dataType *) pU);
      __vstore_pred(pred, pStoreVec, v1);

      pred      = c7x::strm_agen<1, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pUHalf);
      __vstore_pred(pred, pStoreVec, v2);

      sumVec1 += v1 * v1;
      sumVec2 += v2 * v2;
   }
   if (se0ICNT2 != se1ICNT2_int) {
      vec     v1        = c7x::strm_eng<1, vec>::get_adv();
      __vpred pred      = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pUHalf);
      __vstore_pred(pred, pStoreVec, v1);
      sumVec1 += v1 * v1;
   }
   sumVec1 = sumVec1 + sumVec3 + sumVec5 + sumVec2;
   c7x_horizontal_add(sumVec1, &sum);

   if (remainingEle > 0) {
      vec v1 = c7x::strm_eng<1, vec>::get_adv();

      __vpred pred      = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv((dataType *) pUHalf);
      __vstore_pred(pred, pStoreVec, v1);

      for (int32_t i = 0; i < remainingEle; i++) {
         sum += v1.s[i] * v1.s[i];
      }
   }
   if (sa0Params.ICNT0) {
      __SA0_CLOSE();
   }
   __SA1_CLOSE();

   __SE1_CLOSE();

   if (se0ICNT2 > 0) {
      __SE0_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return sum;
}
template float  DSPLIB_qrd_alpha_exec_ci<float>(float           *pR,
                                               int32_t          colStrideR,
                                               int32_t          nRows,
                                               float           *pU,
                                               __SE_TEMPLATE_v1 se0Params,
                                               __SA_TEMPLATE_v1 sa0Params);
template double DSPLIB_qrd_alpha_exec_ci<double>(double          *pR,
                                                 int32_t          colStrideR,
                                                 int32_t          nRows,
                                                 double          *pU,
                                                 __SE_TEMPLATE_v1 se0Params,
                                                 __SA_TEMPLATE_v1 sa0Params);

template <typename dataType>
static inline void
DSPLIB_qrd_R_column_exec_ci(dataType *pR, int32_t colStrideR, int32_t nRows, __SA_TEMPLATE_v1 sa0Params)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   __SA_TEMPLATE_v1 sa1Params = sa0Params;

   sa1Params.ICNT1 = nRows / 2;
   sa0Params.ICNT1 = nRows - sa1Params.ICNT1;

   __SA0_OPEN(sa0Params);
   if (sa1Params.ICNT1) {
      __SA1_OPEN(sa1Params);
   }
   for (int vertical = 0; vertical < nRows / 2; vertical++) {
      dataType *pR1 = c7x::strm_agen<0, dataType>::get_adv(pR);
      *pR1          = 0.0;

      dataType *pR2 = c7x::strm_agen<1, dataType>::get_adv(pR + colStrideR);
      *pR2          = 0.0;
   }
   if (sa0Params.ICNT1 != sa1Params.ICNT1) {
      dataType *pR1 = c7x::strm_agen<0, dataType>::get_adv(pR);
      *pR1          = 0.0;
   }
   __SA0_CLOSE();
   if (sa1Params.ICNT1) {
      __SA1_CLOSE();
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void
DSPLIB_qrd_R_column_exec_ci<float>(float *pR, int32_t colStrideR, int32_t nRows, __SA_TEMPLATE_v1 sa0Params);
template void
DSPLIB_qrd_R_column_exec_ci<double>(double *pR, int32_t colStrideR, int32_t nRows, __SA_TEMPLATE_v1 sa0Params);

template <typename dataType>
DSPLIB_STATUS DSPLIB_qrd_exec_ci(DSPLIB_kernelHandle handle,
                                 const void *restrict pA,
                                 const void *restrict pQ,
                                 const void *restrict pR,
                                 const void *restrict pU,
                                 const void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_qrd_PrivArgs *pKerPrivArgs = (DSPLIB_qrd_PrivArgs *) handle;
   int32_t              nRows        = pKerPrivArgs->heightA;
   int32_t              nCols        = pKerPrivArgs->widthA;
   int32_t              strideA      = pKerPrivArgs->strideA;
   int32_t              strideQ      = pKerPrivArgs->strideQ;
   int32_t              strideR      = pKerPrivArgs->strideR;
   int32_t              dataSize     = sizeof(dataType);
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              colStrideQ   = strideQ / dataSize;
   int32_t              colStrideR   = strideR / dataSize;
   int32_t              colStrideA   = strideA / dataSize;
   dataType             alpha;
   dataType             sum;
   int32_t              col;
   int32_t              loopCount;

   /* Typecast void pointers to respective data type */
   dataType *pLocalA = (dataType *) pA;
   dataType *pLocalQ = (dataType *) pQ;
   dataType *pLocalR = (dataType *) pR;
   dataType *pLocalU = (dataType *) pU;
   dataType *pSum    = (dataType *) pScratch;

   DSPLIB_DEBUGPRINTFN(0, "pALocal: %p pLocalQ: %p pLocalR: %p pLocalU: %p nCols: %d nRows: %d\n", pLocalA, pLocalQ,
                       pLocalR, pLocalU, nCols, nRows);

   __SE_TEMPLATE_v1 seAlphaParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saAlphaParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saRColParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));

   /* ------------------------------------------------------------------- */
   /*  Write each column of 'pALocal' to a row of 'pLocalR'.              */
   /* ------------------------------------------------------------------- */
   DSPLIB_qrd_blk_move_exec_ci<dataType>(pLocalR, pLocalA, nRows, nCols, colStrideR, colStrideA,
                                         &pBlock[18 * SE_PARAM_SIZE]);

   /* ------------------------------------------------------------------- */
   /* generate identify matrix                                            */
   /* ------------------------------------------------------------------- */

   DSPLIB_qrd_identity_matrix_generate_exec_ci<dataType>(pLocalQ, nRows, colStrideQ, pBlock);

   if (nRows <= nCols) {
      loopCount = nRows - 2;
   }
   else {
      loopCount = nCols - 1;
   }

   for (col = 0; col <= loopCount; col++) {
      sum = DSPLIB_qrd_alpha_exec_ci<dataType>(&pLocalR[col + col * colStrideR], colStrideR, (nRows - col),
                                               &pLocalU[col], seAlphaParams, saAlphaParams);
      if (sum != 0) {
         alpha = getSqrt(sum);
         if (pLocalR[col + (col * colStrideR)] >= 0) {
            alpha = -alpha;
         }
         pLocalU[col]                      = pLocalR[col + (col * colStrideR)] + alpha;
         pLocalR[col + (col * colStrideR)] = -alpha;
         DSPLIB_qrd_R_column_exec_ci<dataType>(&pLocalR[col + ((col + 1) * colStrideR)], colStrideR,
                                               (nRows - (col + 1)), saRColParams);
         if (alpha * pLocalU[col] != 0.0) {
            dataType product = (alpha * pLocalU[col]);
            dataType scale   = __recip(product);
            dataType twoP0   = 2.0;

            scale = scale * (twoP0 - (product * scale));
            scale = scale * (twoP0 - (product * scale));
            /* R=Q1*R */
            if ((col + 1) < nCols) {
               DSPLIB_qrd_R_matrix_exec_ci<dataType>(&pLocalR[(col) + (col * colStrideR)], &pLocalU[col],
                                                     &pSum[col + 1], scale, colStrideR, (nRows - col),
                                                     (nCols - (col + 1)), pBlock);
            }
            /* Q=A*Q1 */
            DSPLIB_qrd_Q_matrix_exec_ci<dataType>(&pLocalQ[(col)], &pLocalU[col], &pSum[0], scale, colStrideQ, nRows,
                                                  (nRows - col), pBlock);
         }
      } /* if (sum!=0) */
   }    /* for (col=0;col<=loopCount;col++) */

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return (status);
}

template DSPLIB_STATUS DSPLIB_qrd_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                 const void *restrict pA,
                                                 const void *restrict pQ,
                                                 const void *restrict pR,
                                                 const void *restrict pU,
                                                 const void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_qrd_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                  const void *restrict pA,
                                                  const void *restrict pQ,
                                                  const void *restrict pR,
                                                  const void *restrict pU,
                                                  const void *restrict pScratch);
