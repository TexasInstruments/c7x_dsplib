// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cholesky_common.h"

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_inplace_isPosDefinite_init(int32_t order, int32_t colStride, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;

   /* FOR POSITIVE DEFINITE TEST */
   int32_t          nVec                                            = DSPLIB_ceilingDiv(order, eleCount);
   int32_t          nVecPosCheck2                                   = nVec / 2;
   int32_t          nVecPosCheck1                                   = nVec - nVecPosCheck2;
   __SE_TEMPLATE_v1 sePosCheck1Params                               = __gen_SE_TEMPLATE_v1();
   sePosCheck1Params.ICNT0                                          = eleCount;
   sePosCheck1Params.ICNT1                                          = order;
   sePosCheck1Params.DIM1                                           = colStride;
   sePosCheck1Params.ICNT2                                          = nVecPosCheck1;
   sePosCheck1Params.DIM2                                           = eleCount * 2;
   sePosCheck1Params.DECDIM1                                        = __SE_DECDIM_DIM2;
   sePosCheck1Params.DECDIM1_WIDTH                                  = order;
   sePosCheck1Params.DIMFMT                                         = __SE_DIMFMT_3D;
   sePosCheck1Params.VECLEN                                         = SE_VECLEN;
   sePosCheck1Params.ELETYPE                                        = SE_ELETYPE;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = sePosCheck1Params;

   __SE_TEMPLATE_v1 sePosCheck2Params = __gen_SE_TEMPLATE_v1();
   sePosCheck2Params.ICNT0            = eleCount;
   sePosCheck2Params.ICNT1            = order;
   sePosCheck2Params.DIM1             = colStride;
   sePosCheck2Params.ICNT2            = nVecPosCheck2;
   sePosCheck2Params.DIM2             = eleCount * 2;
   sePosCheck2Params.DECDIM1          = __SE_DECDIM_DIM2;
   sePosCheck2Params.DECDIM1_WIDTH    = order - eleCount;
   sePosCheck2Params.DIMFMT           = __SE_DIMFMT_3D;
   sePosCheck2Params.VECLEN           = SE_VECLEN;
   sePosCheck2Params.ELETYPE          = SE_ELETYPE;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = sePosCheck2Params;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return status;
}

template DSPLIB_STATUS
DSPLIB_cholesky_inplace_isPosDefinite_init<float>(int32_t order, int32_t colStride, uint8_t *pBlock);
template DSPLIB_STATUS
DSPLIB_cholesky_inplace_isPosDefinite_init<double>(int32_t order, int32_t colStride, uint8_t *pBlock);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/

template <typename dataType>
dataType
DSPLIB_cholesky_inplace_isPosDefinite(dataType *A, const int32_t order, const int32_t eleCount, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 sePosCheck1Params, sePosCheck2Params;
   sePosCheck1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sePosCheck2Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   typedef typename c7x::make_full_vector<dataType>::type vec;

   /* test A for positive definite matrix:    */
   /* z_transpose*A*z>0 where z=1,2,...order  */
   int32_t nVec = DSPLIB_ceilingDiv(order, eleCount);

   vec vZTrans1 = (vec) 1.0;
   vec vZTrans2 = (vec) 2.0;
   vec vZTrans3 = (vec) 3.0;
   vec vZTrans4 = (vec) 4.0;

   vec vOne   = (vec) 1.0;
   vec vTwo   = (vec) 2.0;
   vec vThree = (vec) 3.0;
   vec vFour  = (vec) 4.0;
   vec vZero  = (vec) 0.0;
   vec vZ1    = vZero;
   vec sum1   = vZero;
   vec sum2   = vZero;
   vec sum3   = vZero;
   vec sum4   = vZero;
   vec sum5   = vZero;
   vec sum6   = vZero;
   vec sum7   = vZero;
   vec sum8   = vZero;

   vec finalSum1 = vZero;
   vec finalSum2 = vZero;

   for (int32_t i = 0; i < eleCount; i++) {
      vZ1.s[i] = (dataType) i;
   }
   vZ1 += vOne;
   vec vZ2 = vZ1 + eleCount;

   __SE0_OPEN(A, sePosCheck1Params);
   if (nVec > 1) {
      __SE1_OPEN(A + eleCount, sePosCheck2Params);
   }

   int32_t horizontal = 0;
   int32_t vertical   = 0;
   for (horizontal = 0; horizontal < nVec - 1; horizontal += 2) {
      for (vertical = 0; vertical < order - 3; vertical += 4) {

         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec v2 = c7x::strm_eng<1, vec>::get_adv();
         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec v4 = c7x::strm_eng<1, vec>::get_adv();
         vec v5 = c7x::strm_eng<0, vec>::get_adv();
         vec v6 = c7x::strm_eng<1, vec>::get_adv();
         vec v7 = c7x::strm_eng<0, vec>::get_adv();
         vec v8 = c7x::strm_eng<1, vec>::get_adv();

         sum1 += v1 * vZTrans1;
         sum2 += v2 * vZTrans1;
         sum3 += v3 * vZTrans2;
         sum4 += v4 * vZTrans2;
         sum5 += v5 * vZTrans3;
         sum6 += v6 * vZTrans3;
         sum7 += v7 * vZTrans4;
         sum8 += v8 * vZTrans4;

         vZTrans1 += vFour;
         vZTrans2 += vFour;
         vZTrans3 += vFour;
         vZTrans4 += vFour;
      }

      for (; vertical < order; vertical++) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec v2 = c7x::strm_eng<1, vec>::get_adv();

         sum1 += v1 * vZTrans1;
         sum2 += v2 * vZTrans1;

         vZTrans1 += vOne;
      }
      sum1 += sum3;
      sum1 += sum5;
      sum1 += sum7;

      sum2 += sum4;
      sum2 += sum6;
      sum2 += sum8;

      finalSum1 += sum1 * vZ1;
      finalSum2 += sum2 * vZ2;

      vZ1 += 2 * eleCount;
      vZ2 += 2 * eleCount;

      vZTrans1 = vOne;
      vZTrans2 = vTwo;
      vZTrans3 = vThree;
      vZTrans4 = vFour;

      sum1 = vZero;
      sum2 = vZero;
      sum3 = vZero;
      sum4 = vZero;
      sum5 = vZero;
      sum6 = vZero;
      sum7 = vZero;
      sum8 = vZero;
   }

   if (horizontal != nVec) {
      for (vertical = 0; vertical < order - 3; vertical += 4) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec v5 = c7x::strm_eng<0, vec>::get_adv();
         vec v7 = c7x::strm_eng<0, vec>::get_adv();

         sum1 += v1 * vZTrans1;
         sum3 += v3 * vZTrans2;
         sum5 += v5 * vZTrans3;
         sum7 += v7 * vZTrans4;

         vZTrans1 += vFour;
         vZTrans2 += vFour;
         vZTrans3 += vFour;
         vZTrans4 += vFour;
      }
      for (; vertical < order; vertical++) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         sum1 += v1 * vZTrans1;
         vZTrans1 += vOne;
      }

      sum1 += sum3;
      sum1 += sum5;
      sum1 += sum7;

      finalSum1 += sum1 * vZ1;

      vZ1 += 2 * eleCount;
   }

   vec      finalSum = finalSum1 + finalSum2;
   dataType sum      = 0.0;
   c7x_horizontal_add(finalSum, &sum);
   __SE0_CLOSE();
   __SE1_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return SUM: %7.5e\n", sum);
   return sum;
}
template float
DSPLIB_cholesky_inplace_isPosDefinite<float>(float *A, const int32_t order, const int32_t eleCount, uint8_t *pBlock);

template double
DSPLIB_cholesky_inplace_isPosDefinite<double>(double *A, const int32_t order, const int32_t eleCount, uint8_t *pBlock);
