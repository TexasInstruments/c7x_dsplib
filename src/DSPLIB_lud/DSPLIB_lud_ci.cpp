// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/DSPLIB_inlines.h"
#include "DSPLIB_lud_common.h"
#include "DSPLIB_lud_priv.h"

#define LUD_HIGH_PRECISION
/**********************************************************************
 *
 * INITIALIZATION
 *
 *********************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_lud_init_ci(DSPLIB_kernelHandle         handle,
                                 const DSPLIB_bufParams2D_t *bufParamsA,
                                 const DSPLIB_bufParams2D_t *bufParamsL,
                                 const DSPLIB_bufParams2D_t *bufParamsU,
                                 const DSPLIB_bufParams2D_t *bufParamsP,
                                 const DSPLIB_ludInitArgs   *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_lud_PrivArgs *pKerPrivArgs   = (DSPLIB_lud_PrivArgs *) handle;
   uint8_t             *pBlock         = pKerPrivArgs->bufPblock;
   int32_t              strideOrder    = pKerPrivArgs->strideOrder;
   int32_t              strideP        = pKerPrivArgs->strideP;
   int32_t              order          = pKerPrivArgs->order;
   int32_t              colStrideOrder = strideOrder / sizeof(dataType);

   DSPLIB_lud_identity_matrix_generate_init_ci<unsigned short>(pBlock, order, strideP);
   DSPLIB_lud_blk_move_init_ci<dataType>(&pBlock[17 * SE_PARAM_SIZE], order, colStrideOrder);

   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;

   /******************************* INDEX FINDING ******************************/
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();

   se0Params.ICNT0   = 1;
   se0Params.ICNT1   = eleCount;
   se0Params.DIM1    = colStrideOrder;
   se0Params.DIM2    = colStrideOrder * eleCount;
   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   if (sizeof(dataType) == 4) {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = se0Params;
   /******************************* ARRAY SWAP ******************************/
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   se0Params                  = __gen_SE_TEMPLATE_v1();

   sa0Params.ICNT0  = order;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;
   sa0Params.VECLEN = SA_VECLEN;

   se0Params.ICNT0   = order;
   se0Params.DIMFMT  = __SE_DIMFMT_1D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE)) = sa0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = se0Params;

   /******************************* P ARRAY SWAP ******************************/
   typedef typename c7x::make_full_vector<unsigned short>::type vecShort;

   __SE_ELETYPE SE_ELETYPE_SHORT = c7x::se_eletype<vecShort>::value;
   __SE_VECLEN  SE_VECLEN_SHORT  = c7x::se_veclen<vecShort>::value;
   __SA_VECLEN  SA_VECLEN_SHORT  = c7x::sa_veclen<vecShort>::value;

   sa0Params = __gen_SA_TEMPLATE_v1();
   se0Params = __gen_SE_TEMPLATE_v1();

   sa0Params.ICNT0  = order;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;
   sa0Params.VECLEN = SA_VECLEN_SHORT;

   se0Params.ICNT0   = order;
   se0Params.DIMFMT  = __SE_DIMFMT_1D;
   se0Params.ELETYPE = SE_ELETYPE_SHORT;
   se0Params.VECLEN  = SE_VECLEN_SHORT;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE)) = sa0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (12 * SE_PARAM_SIZE)) = se0Params;

   /******************************* U_generateE PARAMS (MULTIPLE OF 4)**************************************/
   __SE_TEMPLATE_v1 seRefParams    = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 seMatrixParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saMatrixParams = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa2Params      = __gen_SA_TEMPLATE_v1();

   sa0Params = __gen_SA_TEMPLATE_v1();

   int32_t lenTile4 = 4;

   sa0Params.ICNT0  = 1;
   sa0Params.DIMFMT = __SA_DIMFMT_2D;
   sa0Params.VECLEN = __SA_VECLEN_1ELEM;
   sa0Params.DIM1   = colStrideOrder;

   sa2Params.ICNT0  = 1;
   sa2Params.DIMFMT = __SA_DIMFMT_2D;
   sa2Params.VECLEN = __SA_VECLEN_1ELEM;
   sa2Params.DIM1   = colStrideOrder;

   seRefParams.DIM1    = (eleCount * lenTile4);
   seRefParams.DIM2    = 0;
   seRefParams.DIMFMT  = __SE_DIMFMT_3D;
   seRefParams.VECLEN  = SE_VECLEN;
   seRefParams.ELETYPE = SE_ELETYPE;
   seRefParams.ICNT0   = (eleCount * lenTile4);
   seRefParams.DECDIM2 = __SE_DECDIM_DIM1;

   seMatrixParams.ICNT0   = (eleCount * lenTile4);
   seMatrixParams.DIM1    = colStrideOrder;
   seMatrixParams.ICNT1   = lenTile4;
   seMatrixParams.DIM2    = (eleCount * lenTile4);
   seMatrixParams.DIM3    = (colStrideOrder * lenTile4);
   seMatrixParams.DIMFMT  = __SE_DIMFMT_4D;
   seMatrixParams.ELETYPE = SE_ELETYPE;
   seMatrixParams.VECLEN  = SE_VECLEN;
   seMatrixParams.DECDIM2 = __SE_DECDIM_DIM2;

   saMatrixParams.ICNT0   = (eleCount * lenTile4);
   saMatrixParams.DIM1    = colStrideOrder;
   saMatrixParams.ICNT1   = lenTile4;
   saMatrixParams.DIM2    = (eleCount * lenTile4);
   saMatrixParams.DIM3    = (colStrideOrder * lenTile4);
   saMatrixParams.DIMFMT  = __SA_DIMFMT_4D;
   saMatrixParams.VECLEN  = SA_VECLEN;
   saMatrixParams.DECDIM2 = __SA_DECDIM_DIM2;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE)) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE)) = sa2Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE)) = seRefParams;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE)) = seMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE)) = saMatrixParams;

   /******************************* U_generateE PARAMS (NON-MULTIPLE OF 4)******************************/
   seMatrixParams = __gen_SE_TEMPLATE_v1();
   saMatrixParams = __gen_SA_TEMPLATE_v1();

   seMatrixParams.ICNT0   = (eleCount * lenTile4);
   seMatrixParams.DIM2    = colStrideOrder;
   seMatrixParams.DIM1    = (eleCount * lenTile4);
   seMatrixParams.ICNT3   = 1;
   seMatrixParams.DIM3    = 0;
   seMatrixParams.DIMFMT  = __SE_DIMFMT_4D;
   seMatrixParams.ELETYPE = SE_ELETYPE;
   seMatrixParams.VECLEN  = SE_VECLEN;
   seMatrixParams.DECDIM2 = __SE_DECDIM_DIM1;

   saMatrixParams.ICNT0   = (eleCount * lenTile4);
   saMatrixParams.DIM2    = colStrideOrder;
   saMatrixParams.DIM1    = (eleCount * lenTile4);
   saMatrixParams.ICNT3   = 1;
   saMatrixParams.DIM3    = 0;
   saMatrixParams.DIMFMT  = __SA_DIMFMT_4D;
   saMatrixParams.VECLEN  = SA_VECLEN;
   saMatrixParams.DECDIM2 = __SA_DECDIM_DIM1;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (9 * SE_PARAM_SIZE))  = seMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (10 * SE_PARAM_SIZE)) = saMatrixParams;

   /***************************************** U and L extraction ********************************************/

   seMatrixParams = __gen_SE_TEMPLATE_v1();
   saMatrixParams = __gen_SA_TEMPLATE_v1();

   seMatrixParams.ICNT0   = order;
   seMatrixParams.ICNT1   = order;
   seMatrixParams.DIM1    = colStrideOrder;
   seMatrixParams.DIMFMT  = __SE_DIMFMT_2D;
   seMatrixParams.ELETYPE = SE_ELETYPE;
   seMatrixParams.VECLEN  = SE_VECLEN;

   saMatrixParams.ICNT0  = order;
   saMatrixParams.ICNT1  = order;
   saMatrixParams.DIM1   = colStrideOrder;
   saMatrixParams.DIMFMT = __SA_DIMFMT_2D;
   saMatrixParams.VECLEN = SA_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (13 * SE_PARAM_SIZE)) = seMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (14 * SE_PARAM_SIZE)) = saMatrixParams;

   /***************************************** Col Update using 2 SE ********************************************/

   __SE_TEMPLATE_v1 seColParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saColParams = __gen_SA_TEMPLATE_v1();

   seColParams.ICNT0   = eleCount;
   seColParams.DIM1    = 2 * colStrideOrder;
   seColParams.DIMFMT  = __SE_DIMFMT_2D;
   seColParams.ELETYPE = SE_ELETYPE;
   seColParams.VECLEN  = SE_VECLEN;

   saColParams.ICNT0  = 1;
   saColParams.DIM1   = colStrideOrder;
   saColParams.DIMFMT = __SA_DIMFMT_2D;
   saColParams.VECLEN = __SA_VECLEN_1ELEM;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (15 * SE_PARAM_SIZE)) = seColParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (16 * SE_PARAM_SIZE)) = saColParams;

   /************************************ U Generating Params ************************************/

   int32_t lenTile8 = 8;
   int32_t nTiles_8 = DSPLIB_ceilingDiv(order, (eleCount * lenTile8));

   __SE_ELEDUP      SE_ELEDUP      = c7x::se_eledup<dataType>::value;
   __SE_TEMPLATE_v1 seScalarParams = __gen_SE_TEMPLATE_v1();
   seMatrixParams                  = __gen_SE_TEMPLATE_v1();
   saMatrixParams                  = __gen_SA_TEMPLATE_v1();

   seScalarParams.ICNT0   = 1;
   seScalarParams.DIM1    = colStrideOrder;
   seScalarParams.DIM2    = 0;
   seScalarParams.ELEDUP  = SE_ELEDUP;
   seScalarParams.DIMFMT  = __SE_DIMFMT_3D;
   seScalarParams.VECLEN  = SE_VECLEN;
   seScalarParams.ELETYPE = SE_ELETYPE;

   seMatrixParams.ICNT0         = (eleCount * lenTile8);
   seMatrixParams.DIM1          = colStrideOrder;
   seMatrixParams.ICNT2         = nTiles_8;
   seMatrixParams.DIM2          = (eleCount * lenTile8);
   seMatrixParams.DIMFMT        = __SE_DIMFMT_3D;
   seMatrixParams.ELETYPE       = SE_ELETYPE;
   seMatrixParams.VECLEN        = SE_VECLEN;
   seMatrixParams.DECDIM1       = __SE_DECDIM_DIM2;
   seMatrixParams.DECDIM1_WIDTH = order;

   saMatrixParams.ICNT0         = (eleCount * lenTile8);
   saMatrixParams.DIM1          = colStrideOrder;
   saMatrixParams.ICNT2         = nTiles_8;
   saMatrixParams.DIM2          = (eleCount * lenTile8);
   saMatrixParams.DIMFMT        = __SA_DIMFMT_3D;
   saMatrixParams.VECLEN        = SA_VECLEN;
   saMatrixParams.DECDIM1       = __SA_DECDIM_DIM2;
   saMatrixParams.DECDIM1_WIDTH = order;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (19 * SE_PARAM_SIZE)) = seScalarParams;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (20 * SE_PARAM_SIZE)) = seMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (21 * SE_PARAM_SIZE)) = saMatrixParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_lud_init_ci<float>(DSPLIB_kernelHandle         handle,
                                                 const DSPLIB_bufParams2D_t *bufParamsA,
                                                 const DSPLIB_bufParams2D_t *bufParamsL,
                                                 const DSPLIB_bufParams2D_t *bufParamsU,
                                                 const DSPLIB_bufParams2D_t *bufParamsP,
                                                 const DSPLIB_ludInitArgs   *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_lud_init_ci<double>(DSPLIB_kernelHandle         handle,
                                                  const DSPLIB_bufParams2D_t *bufParamsA,
                                                  const DSPLIB_bufParams2D_t *bufParamsL,
                                                  const DSPLIB_bufParams2D_t *bufParamsU,
                                                  const DSPLIB_bufParams2D_t *bufParamsP,
                                                  const DSPLIB_ludInitArgs   *pKerInitArgs);
/**********************************************************************/
/* IMPLEMENTATION                                                     */
/**********************************************************************/

template <typename dataType> inline __vpred getPMask(uint32_t idx);

template <> inline __vpred getPMask<float>(uint32_t idx) { return __mask_int(idx); }

template <> inline __vpred getPMask<double>(uint32_t idx) { return __mask_long(idx); }

#if defined(__C7504__) || defined(__C7524__)
float8  idx_float  = float8(0, 1, 2, 3, 4, 5, 6, 7);
double4 idx_double = double4(0, 1, 2, 3);

#else
float16 idx_float  = float16(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
double8 idx_double = double8(0, 1, 2, 3, 4, 5, 6, 7);
#endif

template <typename V, typename dataType> inline V getIdxVec();

template <> inline c7x::float_vec getIdxVec<c7x::float_vec, float>() { return idx_float; }

template <> inline c7x::double_vec getIdxVec<c7x::double_vec, double>() { return idx_double; }

template <typename dataType, typename vec = typename c7x::make_full_vector<dataType>::type>
void static inline DSPLIB_lud_maxMinIndex_exec_ci(dataType        *pCol,
                                                  int32_t          nRows,
                                                  vec              idx_0_to_eleCount,
                                                  int32_t          k,
                                                  int32_t          colStride,
                                                  int32_t         *max,
                                                  int32_t         *min,
                                                  __SE_TEMPLATE_v1 se0Params)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t eleCount = c7x::element_count_of<vec>::value;
   int32_t nVec     = nRows / eleCount;
   int32_t remRows  = nRows - (nVec * eleCount);

   vec maxValVec = (vec) (-std::numeric_limits<dataType>::max());
   vec minValVec = (vec) (std::numeric_limits<dataType>::max());

   vec zeroVec = (vec) 0;

   __vpred predMask = getPMask<dataType>(remRows);
   vec     minMask  = __select(predMask, zeroVec, minValVec);

   se0Params.ICNT2 = nVec + ((remRows > 0) ? 1 : 0);

   dataType maxVal    = -std::numeric_limits<dataType>::max();
   dataType minVal    = std::numeric_limits<dataType>::max();
   vec      offSetVec = (vec) k;

   idx_0_to_eleCount += offSetVec;

   vec vCurrIdx = idx_0_to_eleCount;
   vec vMaxIdx  = idx_0_to_eleCount;
   vec vMinIdx  = idx_0_to_eleCount;

   __SE0_OPEN(pCol, se0Params);
   __vpred vpMask;
   for (int32_t k = 0; k < nVec; k++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      v1     = __abs(v1);

      vpMask    = __cmp_le_pred(v1, maxValVec);
      maxValVec = __select(vpMask, maxValVec, v1);
      vMaxIdx   = __select(vpMask, vMaxIdx, vCurrIdx);

      vpMask    = __cmp_le_pred(minValVec, v1);
      minValVec = __select(vpMask, minValVec, v1);
      vMinIdx   = __select(vpMask, vMinIdx, vCurrIdx);

      vCurrIdx = vCurrIdx + (eleCount);
   }
   if (remRows) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      v1     = __abs(v1);
      vec v3 = c7x::reinterpret<vec>(__andn(c7x::as_uchar_vec(minMask), c7x::as_uchar_vec(v1)));
      vec v4 = v3 + minMask;

      vpMask    = __cmp_le_pred(v3, maxValVec);
      maxValVec = __select(vpMask, maxValVec, v3);
      vMaxIdx   = __select(vpMask, vMaxIdx, vCurrIdx);
      vpMask    = __cmp_lt_pred(minValVec, v4);
      minValVec = __select(vpMask, minValVec, v4);
      vMinIdx   = __select(vpMask, vMinIdx, vCurrIdx);

      vCurrIdx = vCurrIdx + (eleCount);
   }

   c7x_horizontal_max_with_index(maxValVec, vMaxIdx, &maxVal, max);
   vpMask     = __cmp_eq_pred(maxValVec, (vec) maxVal);
   vec tmpIdx = __select(vpMask, vMaxIdx, (vec) (std::numeric_limits<dataType>::max()));
   *max       = (int32_t) c7x_horizontal_min_fp<dataType, vec>(tmpIdx);

   c7x_horizontal_min_with_index(minValVec, vMinIdx, &minVal, min);
   vpMask = __cmp_eq_pred(minValVec, (vec) minVal);
   tmpIdx = __select(vpMask, vMinIdx, (vec) (std::numeric_limits<dataType>::max()));
   *min   = (int32_t) c7x_horizontal_min_fp<dataType, vec>(tmpIdx);

   __SE0_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_maxMinIndex_exec_ci<float>(float                                      *pCol,
                                                    int32_t                                     nRows,
                                                    typename c7x::make_full_vector<float>::type idx_0_to_eleCount,
                                                    int32_t                                     k,
                                                    int32_t                                     colStride,
                                                    int32_t                                    *max,
                                                    int32_t                                    *min,
                                                    __SE_TEMPLATE_v1                            se0Params);
template void DSPLIB_lud_maxMinIndex_exec_ci<double>(double                                      *pCol,
                                                     int32_t                                      nRows,
                                                     typename c7x::make_full_vector<double>::type idx_0_to_eleCount,
                                                     int32_t                                      k,
                                                     int32_t                                      colStride,
                                                     int32_t                                     *max,
                                                     int32_t                                     *min,
                                                     __SE_TEMPLATE_v1                             se0Params);

template <typename dataType>
static inline void DSPLIB_lud_array_swap_exec_ci(dataType        *pArray1,
                                                 dataType        *pArray2,
                                                 int32_t          nCols,
                                                 __SA_TEMPLATE_v1 sa1Params,
                                                 __SE_TEMPLATE_v1 se1Params)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   int32_t                                                nTiles   = DSPLIB_ceilingDiv(se1Params.ICNT0, eleCount);

   __SE0_OPEN(pArray1, se1Params);
   __SA0_OPEN(sa1Params);
   __SE1_OPEN(pArray2, se1Params);
   __SA1_OPEN(sa1Params);

   for (int32_t horizontal = 0; horizontal < nTiles; horizontal++) {
      vec v11 = c7x::strm_eng<0, vec>::get_adv();
      vec v21 = c7x::strm_eng<1, vec>::get_adv();

      __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pArray2);
      __vstore_pred(sPred, pStoreVec, v11);

      sPred     = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vec>::get_adv(pArray1);
      __vstore_pred(sPred, pStoreVec, v21);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
   __SE1_CLOSE();
   __SA1_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_array_swap_exec_ci<float>(float           *pArray1,
                                                   float           *pArray2,
                                                   int32_t          nCols,
                                                   __SA_TEMPLATE_v1 sa1Params,
                                                   __SE_TEMPLATE_v1 se1Params);
template void DSPLIB_lud_array_swap_exec_ci<double>(double          *pArray1,
                                                    double          *pArray2,
                                                    int32_t          nCols,
                                                    __SA_TEMPLATE_v1 sa1Params,
                                                    __SE_TEMPLATE_v1 se1Params);
template void DSPLIB_lud_array_swap_exec_ci<unsigned short>(unsigned short  *pArray1,
                                                            unsigned short  *pArray2,
                                                            int32_t          nCols,
                                                            __SA_TEMPLATE_v1 sa1Params,
                                                            __SE_TEMPLATE_v1 se1Params);
template <typename dataType>
void static inline DSPLIB_lud_U_colDiv_exec_ci(dataType        *pCol,
                                               int32_t          nRows,
                                               int32_t          colStride,
                                               __SA_TEMPLATE_v1 saColParams,
                                               __SE_TEMPLATE_v1 se0ColParams,
                                               __SE_TEMPLATE_v1 se1ColParams)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   nRows            = nRows - 1;
   int32_t nBlocks  = (int32_t) ((uint32_t) nRows >> (uint32_t) 1);
   int32_t se0ICNT1 = nRows - nBlocks;
   int32_t se1ICNT1 = nBlocks;

#ifndef LUD_HIGH_PRECISION
   dataType diag        = pCol[0];
   dataType recipScalar = __recip(diag);
   dataType twoP0       = 2.0;
   recipScalar          = recipScalar * (twoP0 - (diag * recipScalar));
   recipScalar          = recipScalar * (twoP0 - (diag * recipScalar));

   vec recip = (vec) recipScalar;
#else
   vec recip = (vec) (1 / pCol[0]);
#endif
   se0ColParams.ICNT1 = se0ICNT1;
   se1ColParams.ICNT1 = se1ICNT1;
   saColParams.ICNT1  = nRows;

   __SE0_OPEN(pCol + colStride, se0ColParams);
   __SA0_OPEN(saColParams);

   if (nBlocks > 0) {
      __SE1_OPEN(pCol + (2 * colStride), se1ColParams);
   }

   for (int32_t horizontal = 0; horizontal < nRows - 1; horizontal += 2) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();

      v1 *= recip;
      v2 *= recip;

      __vpred sPred     = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pCol + colStride);
      __vstore_pred(sPred, pStoreVec, v1);

      sPred     = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vec>::get_adv(pCol + colStride);
      __vstore_pred(sPred, pStoreVec, v2);
   }

   if (se0ICNT1 != se1ICNT1) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();

      v1 *= recip;

      __vpred sPred     = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pCol + colStride);
      __vstore_pred(sPred, pStoreVec, v1);
   }
   __SE0_CLOSE();
   __SA0_CLOSE();
   if (nBlocks > 0) {
      __SE1_CLOSE();
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_U_colDiv_exec_ci<float>(float           *pCol,
                                                 int32_t          nRows,
                                                 int32_t          colStride,
                                                 __SA_TEMPLATE_v1 saColParams,
                                                 __SE_TEMPLATE_v1 se0ColParams,
                                                 __SE_TEMPLATE_v1 se1ColParams);
template void DSPLIB_lud_U_colDiv_exec_ci<double>(double          *pCol,
                                                  int32_t          nRows,
                                                  int32_t          colStride,
                                                  __SA_TEMPLATE_v1 saColParams,
                                                  __SE_TEMPLATE_v1 se0ColParams,
                                                  __SE_TEMPLATE_v1 se1ColParams);

template <typename dataType>
static inline void DSPLIB_lud_U_generate_exec_ci(dataType *pLocalU, int32_t order, int32_t colStrideU, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   /* Updation params */
   __SE_TEMPLATE_v1 seScalarParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (19 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 seMatrixParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (20 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saMatrixParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (21 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saRefParams    = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

   int32_t lenTile8 = 8;
   int32_t lenTile4 = 4;
   int32_t lenTile2 = 2;
   int32_t lenTile1 = 1;

   int32_t nRows     = (order - 1);
   saRefParams.ICNT0 = nRows;

   int32_t nTiles1 = DSPLIB_ceilingDiv(nRows, (eleCount));
   int32_t nTiles8 = nTiles1 / lenTile8;
   nTiles1 -= nTiles8 * lenTile8;
   int32_t nTiles4 = nTiles1 / lenTile4;
   nTiles1 -= nTiles4 * lenTile4;
   int32_t nTiles2 = nTiles1 / lenTile2;
   nTiles1 -= nTiles2 * lenTile2;

   int32_t remainingCols = nRows;
   int32_t colLimit8     = nTiles8 * lenTile8 * eleCount;
   colLimit8             = (remainingCols < (colLimit8)) ? remainingCols : colLimit8;

   remainingCols     = remainingCols - colLimit8;
   int32_t colLimit4 = nTiles4 * lenTile4 * eleCount;
   colLimit4         = (remainingCols < (colLimit4)) ? remainingCols : colLimit4;

   remainingCols     = remainingCols - colLimit4;
   int32_t colLimit2 = nTiles2 * lenTile2 * eleCount;
   colLimit2         = (remainingCols < (colLimit2)) ? remainingCols : colLimit2;

   int32_t colLimit1 = remainingCols - colLimit2;

   dataType *pRefL   = pLocalU + 1;
   dataType *pStartL = pRefL + colStrideU;

   seScalarParams.ICNT1 = nRows;
   seScalarParams.ICNT2 = nTiles8 + nTiles4 + nTiles2 + nTiles1;

   __SE0_OPEN(pLocalU + colStrideU, seScalarParams);

   __SA0_OPEN(saRefParams);

   if (nTiles8 > 0) {
      /* 1 X (8*eleCount) TILE */

      seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile8;
      seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
      seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles8;
      seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile8;
      seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit8;

      __SE1_OPEN(pStartL, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles8; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
         vec sV2  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
         vec sV3  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
         vec sV4  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
         vec sV5  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
         vec sV6  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
         vec sV7  = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pRefL);
         vec sV8  = __vload_pred(lPred, pLoadVec);

         /* 7 + trip_cnt * 8 */
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

            v1 -= sV1 * scalarDup;
            v2 -= sV2 * scalarDup;
            v3 -= sV3 * scalarDup;
            v4 -= sV4 * scalarDup;
            v5 -= sV5 * scalarDup;
            v6 -= sV6 * scalarDup;
            v7 -= sV7 * scalarDup;
            v8 -= sV8 * scalarDup;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
            __vstore_pred(sPred, pStoreVec, v2);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
            __vstore_pred(sPred, pStoreVec, v3);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
            __vstore_pred(sPred, pStoreVec, v4);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
            __vstore_pred(sPred, pStoreVec, v5);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
            __vstore_pred(sPred, pStoreVec, v6);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
            __vstore_pred(sPred, pStoreVec, v7);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pStartL);
            __vstore_pred(sPred, pStoreVec, v8);
         }
      }
      __SE1_CLOSE();
      __SA1_CLOSE();
   }

   if (nTiles4 > 0) {
      /* 1 X (4*eleCount) TILE */
      seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile4;
      seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
      seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles4;
      seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile4;
      seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit4;

      dataType *pSE1 = pStartL + colLimit8;
      dataType *pSA1 = pStartL + colLimit8;
      dataType *pSA0 = pRefL;

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

         /* 7 + trip_cnt * 4 */
         for (int32_t vertical = 0; vertical < nRows; vertical++) {

            vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();
            vec v3 = c7x::strm_eng<1, vec>::get_adv();
            vec v4 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup;
            v2 -= sV2 * scalarDup;
            v3 -= sV3 * scalarDup;
            v4 -= sV4 * scalarDup;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
            __vstore_pred(sPred, pStoreVec, v2);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
            __vstore_pred(sPred, pStoreVec, v3);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
            __vstore_pred(sPred, pStoreVec, v4);
         }
      }
      __SE1_CLOSE();
      __SA1_CLOSE();
   }

   if (nTiles2 > 0) {
      /* 1 X (2*eleCount) TILE */

      seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile2;
      seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
      seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles2;
      seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile2;
      seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit2;

      dataType *pSE1 = pStartL + colLimit8 + colLimit4;
      dataType *pSA1 = pStartL + colLimit8 + colLimit4;
      dataType *pSA0 = pRefL;
      __SE1_OPEN(pSE1, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles2; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         lPred    = c7x::strm_agen<0, vec>::get_vpred();
         pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec sV2  = __vload_pred(lPred, pLoadVec);
         /* 7 + trip_cnt * 2 */
         for (int32_t vertical = 0; vertical < nRows; vertical++) {
            vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            vec v2 = c7x::strm_eng<1, vec>::get_adv();

            v1 -= sV1 * scalarDup;
            v2 -= sV2 * scalarDup;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
            __vstore_pred(sPred, pStoreVec, v1);

            sPred     = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
            __vstore_pred(sPred, pStoreVec, v2);
         }
      }

      __SE1_CLOSE();
      __SA1_CLOSE();
   }
   if (nTiles1 > 0) {
      /* 1 X eleCount TILE */

      seMatrixParams.ICNT0 = saMatrixParams.ICNT0 = eleCount * lenTile1;
      seMatrixParams.ICNT1 = saMatrixParams.ICNT1 = nRows;
      seMatrixParams.ICNT2 = saMatrixParams.ICNT2 = nTiles1;
      seMatrixParams.DIM2 = saMatrixParams.DIM2 = eleCount * lenTile1;
      seMatrixParams.DECDIM1_WIDTH = saMatrixParams.DECDIM1_WIDTH = colLimit1;

      dataType *pSE1 = pStartL + colLimit8 + colLimit4 + colLimit2;
      dataType *pSA1 = pStartL + colLimit8 + colLimit4 + colLimit2;
      dataType *pSA0 = pRefL;

      __SE1_OPEN(pSE1, seMatrixParams);
      __SA1_OPEN(saMatrixParams);

      for (int32_t tile = 0; tile < nTiles1; tile++) {
         __vpred lPred    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pLoadVec = c7x::strm_agen<0, vec>::get_adv(pSA0);
         vec     sV1      = __vload_pred(lPred, pLoadVec);

         /* 7 + trip_cnt * 2 */
         for (int32_t vertical = 0; vertical < nRows; vertical++) {
            vec scalarDup = c7x::strm_eng<0, vec>::get_adv();

            vec v1 = c7x::strm_eng<1, vec>::get_adv();
            v1 -= sV1 * scalarDup;

            __vpred sPred     = c7x::strm_agen<1, vec>::get_vpred();
            vec    *pStoreVec = c7x::strm_agen<1, vec>::get_adv(pSA1);
            __vstore_pred(sPred, pStoreVec, v1);
         }
      }

      __SE1_CLOSE();
      __SA1_CLOSE();
   }
   __SE0_CLOSE();
   __SA0_CLOSE();
   __SA2_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_lud_U_generate_exec_ci<float>(float *pLocalU, int32_t order, int32_t colStrideU, uint8_t *pBlock);
template void
DSPLIB_lud_U_generate_exec_ci<double>(double *pLocalU, int32_t order, int32_t colStrideU, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_lud_extract_exec_ci(dataType *pU, dataType *pL, int32_t order, int32_t colStride, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type      vec;
   typedef typename c7x::make_full_vector<unsigned char>::type ucharvec;
   uint32_t                                                    eleCount       = c7x::element_count_of<vec>::value;
   const uchar                                                 shiftConst     = 8 * sizeof(dataType);
   vec                                                         zeroVec        = (vec) 0;
   int32_t                                                     frontZeroCount = 0;
   int32_t                                                     iter1          = DSPLIB_ceilingDiv(order, eleCount);

   __SE_TEMPLATE_v1 seMatrixParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (13 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 saMatrixParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (14 * SE_PARAM_SIZE));

   __SA0_OPEN(saMatrixParams);
   __SA1_OPEN(saMatrixParams);
   __SE0_OPEN(pU, seMatrixParams);

   for (int32_t block = 0; block < iter1; block++) {

      ucharvec byteMask = (ucharvec) 0xFF;
      vec      oneVec   = 0;
      oneVec.s[0]       = 1;

      for (uint32_t vertical = 0; vertical < eleCount; vertical++) {
         int32_t horizontal = 0;
         __vpred pred;
         vec    *pStoreVec;
         vec     uV;
         for (; horizontal < frontZeroCount; horizontal++) {
            uV = c7x::strm_eng<0, vec>::get_adv();

            pred      = c7x::strm_agen<0, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<0, vec>::get_adv(pU);
            __vstore_pred(pred, pStoreVec, zeroVec);

            pred      = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pL);
            __vstore_pred(pred, pStoreVec, uV);
         }

         uV = c7x::strm_eng<0, vec>::get_adv();

         vec lV = c7x::reinterpret<vec>(__andn(byteMask, c7x::as_uchar_vec(uV)));
         uV     = c7x::reinterpret<vec>((byteMask & (c7x::as_uchar_vec(uV))));

         byteMask = c7x::as_uchar_vec(__shift_left_full(c7x::as_ulong_vec(byteMask), shiftConst));

         lV     = c7x::reinterpret<vec>(c7x::as_uchar_vec(oneVec) | c7x::as_uchar_vec(lV));
         oneVec = c7x::reinterpret<vec>(__shift_left_full(c7x::as_ulong_vec(oneVec), shiftConst));

         pred      = c7x::strm_agen<0, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<0, vec>::get_adv(pU);
         __vstore_pred(pred, pStoreVec, uV);

         pred      = c7x::strm_agen<1, vec>::get_vpred();
         pStoreVec = c7x::strm_agen<1, vec>::get_adv(pL);
         __vstore_pred(pred, pStoreVec, lV);

         for (; horizontal < iter1 - 1; horizontal++) {
            uV = c7x::strm_eng<0, vec>::get_adv();

            pStoreVec = c7x::strm_agen<0, vec>::get_adv(pU);

            pred      = c7x::strm_agen<1, vec>::get_vpred();
            pStoreVec = c7x::strm_agen<1, vec>::get_adv(pL);
            __vstore_pred(pred, pStoreVec, zeroVec);
         }
      }
      frontZeroCount++;
   }
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SE0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void
DSPLIB_lud_extract_exec_ci<float>(float *pU, float *pL, int32_t order, int32_t colStride, uint8_t *pBlock);
template void
DSPLIB_lud_extract_exec_ci<double>(double *pU, double *pL, int32_t order, int32_t colStride, uint8_t *pBlock);

template <typename dataType>
DSPLIB_STATUS DSPLIB_lud_exec_ci(DSPLIB_kernelHandle handle,
                                 void *restrict pA,
                                 void *restrict pL,
                                 void *restrict pU,
                                 void *restrict pP)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering C7x Optimized implementation");

   DSPLIB_lud_PrivArgs *pKerPrivArgs = (DSPLIB_lud_PrivArgs *) handle;
   int32_t              order        = pKerPrivArgs->order;
   int32_t              strideOrder  = pKerPrivArgs->strideOrder;
   int32_t              strideP      = pKerPrivArgs->strideP;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;

   /* Typecast void pointers to respective data type */
   dataType       *pALocal = (dataType *) pA;
   dataType       *pLLocal = (dataType *) pL;
   dataType       *pULocal = (dataType *) pU;
   unsigned short *pPLocal = (unsigned short *) pP;

   DSPLIB_DEBUGPRINTFN(0, "pALocal: %p pLLocal: %p pULocal: %p pPLocal: %p order: %d\n", pALocal, pLLocal, pULocal,
                       pPLocal, order);

   int min_row, max_row, k;

   int32_t dataSize  = sizeof(dataType);
   int32_t dataSizeP = sizeof(unsigned short);

   int32_t orderStride  = strideOrder / dataSize;
   int32_t orderPStride = strideP / dataSizeP;
   int32_t nRows        = order;

   __SA_TEMPLATE_v1 saSwap1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 seSwap1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   __SA_TEMPLATE_v1 saSwap2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (11 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 seSwap2Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (12 * SE_PARAM_SIZE));

   __SA_TEMPLATE_v1 saColParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (16 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 se0ColParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (15 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 se1ColParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (15 * SE_PARAM_SIZE));
   __SE_TEMPLATE_v1 se0MinMax    = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));

   /* ------------------------------------------------------------------- */
   /* generate identify matrix                                            */
   /* ------------------------------------------------------------------- */
   DSPLIB_lud_identity_matrix_generate_exec_ci<unsigned short>(pPLocal, nRows, orderPStride, pBlock);

   DSPLIB_lud_blk_move_exec_ci<dataType>(pULocal, pALocal, order, order, orderStride, orderStride,
                                         &pBlock[17 * SE_PARAM_SIZE]);

   /* ------------------------------------------------------------------- */
   /* LU decomposition                                          			*/
   /* ------------------------------------------------------------------- */
   typedef typename c7x::make_full_vector<dataType>::type vec;

   vec idx_0_to_eleCount = getIdxVec<vec, dataType>();

   for (k = 0; k < order - 1; k++) {
      DSPLIB_lud_maxMinIndex_exec_ci(&pULocal[k + (k * orderStride)], (order - k), idx_0_to_eleCount, k, orderStride,
                                     &max_row, &min_row, se0MinMax);
      if (k != max_row) {
         /* swap rows if necessary */
         DSPLIB_lud_array_swap_exec_ci<dataType>(&pULocal[min_row * orderStride], &pULocal[max_row * orderStride],
                                                 order, saSwap1Params, seSwap1Params);
         DSPLIB_lud_array_swap_exec_ci<unsigned short>(
             &pPLocal[min_row * orderPStride], &pPLocal[max_row * orderPStride], order, saSwap2Params, seSwap2Params);
      }
      /* generate U matrix */
      DSPLIB_lud_U_colDiv_exec_ci<dataType>(&pULocal[k + k * orderStride], (order - k), orderStride, saColParams,
                                            se0ColParams, se1ColParams);
      DSPLIB_lud_U_generate_exec_ci<dataType>(&pULocal[k + k * orderStride], (order - k), orderStride, pBlock);
   }

   /* Extract lower triangular entries from L into U and set L lower entries to zero */
   DSPLIB_lud_extract_exec_ci<dataType>(pULocal, pLLocal, order, orderStride, pBlock);

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting C7x Optimized implementation");

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_lud_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                 void *restrict pA,
                                                 void *restrict pL,
                                                 void *restrict pU,
                                                 void *restrict pP);

template DSPLIB_STATUS DSPLIB_lud_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                  void *restrict pA,
                                                  void *restrict pL,
                                                  void *restrict pU,
                                                  void *restrict pP);
