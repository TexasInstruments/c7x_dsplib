// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_addNCh_priv.h"

#include "DSPLIB_types.h"

#define SE_OPEN_CNT_SIZE 32
#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA1_PARAM_OFFSET (SE_SA0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_OPEN_CNT_OFFSET (SE_SA1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_OPEN_CNT_OFFSET (SE_OPEN_CNT_OFFSET + SE_OPEN_CNT_SIZE)

template <typename dataType>
DSPLIB_STATUS DSPLIB_addNCh_init_ci(DSPLIB_kernelHandle           handle,
                                    const DSPLIB_bufParams3D_t   *bufParamsIn,
                                    const DSPLIB_bufParams2D_t   *bufParamsOut,
                                    const DSPLIB_addNCh_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_addNCh_PrivArgs *pKerPrivArgs = (DSPLIB_addNCh_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount  = c7x::element_count_of<vec>::value;
   uint8_t                                               *pBlock    = pKerPrivArgs->bufPblock;
   uint32_t                                               se1penCnt = 0;
   uint32_t                                               sa1penCnt = 0;
   uint32_t                                               se0Icnt2  = 0;
   uint32_t                                               se1Icnt2  = 0;

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch the input                      */
   /**********************************************************************/

   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa1Params = __gen_SA_TEMPLATE_v1();

   se0Params.VECLEN  = SE_VECLEN;
   se0Params.ELETYPE = SE_ELETYPE;

   se1Params.VECLEN  = SE_VECLEN;
   se1Params.ELETYPE = SE_ELETYPE;

   sa0Params.VECLEN = SA_VECLEN;
   sa1Params.VECLEN = SA_VECLEN;

   if (pKerPrivArgs->channels == 1) {
      se1penCnt = 0;
      sa1penCnt = 0;
      se0Icnt2  = 1;
      se1Icnt2  = 1;
   }
   else {
      se0Icnt2 = DSPLIB_ceilingDiv(pKerPrivArgs->channels, 2);
      se1Icnt2 = pKerPrivArgs->channels / 2;

      se1penCnt = pKerPrivArgs->strideInElementsY;
      sa1penCnt = pKerPrivArgs->strideOutElements;
   }

   se0Params.ICNT0         = 4 * eleCount;
   se0Params.DIM1          = pKerPrivArgs->strideInElementsZ;
   se0Params.ICNT1         = pKerPrivArgs->inputs;
   se0Params.DIM2          = 2 * pKerPrivArgs->strideInElementsY;
   se0Params.ICNT2         = se0Icnt2;
   se0Params.DIM3          = 4 * eleCount;
   se0Params.ICNT3         = DSPLIB_ceilingDiv(pKerPrivArgs->samples, 4 * eleCount);
   se0Params.DIMFMT        = __SE_DIMFMT_4D;
   se0Params.DECDIM1_WIDTH = pKerPrivArgs->samples;
   se0Params.DECDIM1       = __SE_DECDIM_DIM3;

   se1Params.ICNT0         = 4 * eleCount;
   se1Params.DIM1          = pKerPrivArgs->strideInElementsZ;
   se1Params.ICNT1         = pKerPrivArgs->inputs;
   se1Params.DIM2          = 2 * pKerPrivArgs->strideInElementsY;
   se1Params.ICNT2         = se1Icnt2;
   se1Params.DIM3          = 4 * eleCount;
   se1Params.ICNT3         = DSPLIB_ceilingDiv(pKerPrivArgs->samples, 4 * eleCount);
   se1Params.DIMFMT        = __SE_DIMFMT_4D;
   se1Params.DECDIM1_WIDTH = pKerPrivArgs->samples;
   se1Params.DECDIM1       = __SE_DECDIM_DIM3;

   sa0Params.ICNT0         = 4 * eleCount;
   sa0Params.DIM1          = 2 * pKerPrivArgs->strideOutElements;
   sa0Params.ICNT1         = se0Icnt2;
   sa0Params.DIM2          = 4 * eleCount;
   sa0Params.ICNT2         = DSPLIB_ceilingDiv(pKerPrivArgs->samples, 4 * eleCount);
   sa0Params.DIMFMT        = __SA_DIMFMT_3D;
   sa0Params.DECDIM1_WIDTH = pKerPrivArgs->samples;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM2;

   sa1Params.ICNT0         = 4 * eleCount;
   sa1Params.DIM1          = 2 * pKerPrivArgs->strideOutElements;
   sa1Params.ICNT1         = se1Icnt2;
   sa1Params.DIM2          = 4 * eleCount;
   sa1Params.ICNT2         = DSPLIB_ceilingDiv(pKerPrivArgs->samples, 4 * eleCount);
   sa1Params.DIMFMT        = __SA_DIMFMT_3D;
   sa1Params.DECDIM1_WIDTH = pKerPrivArgs->samples;
   sa1Params.DECDIM1       = __SA_DECDIM_DIM2;

   pKerPrivArgs->nVecs =
       DSPLIB_ceilingDiv(pKerPrivArgs->channels, 2) * DSPLIB_ceilingDiv(pKerPrivArgs->samples, (4 * eleCount));

   if (pKerPrivArgs->channels == 1 && pKerPrivArgs->samples > 4 * eleCount) {

      se1penCnt = 4 * eleCount;
      sa1penCnt = 4 * eleCount;

      se0Params.ICNT0         = 4 * eleCount;
      se0Params.DIM1          = pKerPrivArgs->strideInElementsZ;
      se0Params.ICNT1         = pKerPrivArgs->inputs;
      se0Params.DIM2          = 8 * eleCount;
      se0Params.ICNT2         = DSPLIB_ceilingDiv(pKerPrivArgs->samples, 8 * eleCount);
      se0Params.DIMFMT        = __SE_DIMFMT_3D;
      se0Params.DECDIM1_WIDTH = pKerPrivArgs->samples;
      se0Params.DECDIM1       = __SE_DECDIM_DIM2;

      se1Params.ICNT0         = 4 * eleCount;
      se1Params.DIM1          = pKerPrivArgs->strideInElementsZ;
      se1Params.ICNT1         = pKerPrivArgs->inputs;
      se1Params.DIM2          = 8 * eleCount;
      se1Params.ICNT2         = DSPLIB_ceilingDiv(pKerPrivArgs->samples, 8 * eleCount);
      se1Params.DIMFMT        = __SE_DIMFMT_3D;
      se1Params.DECDIM1_WIDTH = pKerPrivArgs->samples - (4 * eleCount);
      se1Params.DECDIM1       = __SE_DECDIM_DIM2;

      sa0Params.ICNT0         = 4 * eleCount;
      sa0Params.DIM1          = 8 * eleCount;
      sa0Params.ICNT1         = DSPLIB_ceilingDiv(pKerPrivArgs->samples, 8 * eleCount);
      sa0Params.DIMFMT        = __SA_DIMFMT_2D;
      sa0Params.DECDIM1_WIDTH = pKerPrivArgs->samples;
      sa0Params.DECDIM1       = __SA_DECDIM_DIM1;

      sa1Params.ICNT0         = 4 * eleCount;
      sa1Params.DIM1          = 8 * eleCount;
      sa1Params.ICNT1         = DSPLIB_ceilingDiv(pKerPrivArgs->samples, 8 * eleCount);
      sa1Params.DIMFMT        = __SA_DIMFMT_2D;
      sa1Params.DECDIM1_WIDTH = pKerPrivArgs->samples - (4 * eleCount);
      sa1Params.DECDIM1       = __SA_DECDIM_DIM1;

      pKerPrivArgs->nVecs = DSPLIB_ceilingDiv(pKerPrivArgs->samples, (8 * eleCount));
   }

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
   *(uint32_t *) ((uint8_t *) pBlock + SE_OPEN_CNT_OFFSET)          = se1penCnt;
   *(uint32_t *) ((uint8_t *) pBlock + SA_OPEN_CNT_OFFSET)          = sa1penCnt;

   return status;
}

template DSPLIB_STATUS DSPLIB_addNCh_init_ci<float>(DSPLIB_kernelHandle           handle,
                                                    const DSPLIB_bufParams3D_t   *bufParamsIn,
                                                    const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                    const DSPLIB_addNCh_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_addNCh_init_ci<double>(DSPLIB_kernelHandle           handle,
                                                     const DSPLIB_bufParams3D_t   *bufParamsIn,
                                                     const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                     const DSPLIB_addNCh_InitArgs *pKerInitArgs);

template <typename dataType>
DSPLIB_STATUS DSPLIB_addNCh_4Inputs_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_addNCh_exec_ci\n");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_addNCh_PrivArgs *pKerPrivArgs = (DSPLIB_addNCh_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint8_t *pBlock   = pKerPrivArgs->bufPblock;
   uint32_t mainVecs = pKerPrivArgs->nVecs;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   uint32_t se1OpenCnt = *(uint32_t *) ((uint8_t *) pBlock + SE_OPEN_CNT_OFFSET);
   uint32_t sa1OpenCnt = *(uint32_t *) ((uint8_t *) pBlock + SA_OPEN_CNT_OFFSET);

   dataType *restrict pInLocal   = (dataType *) pIn;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOutLocal1 = pOutLocal + sa1OpenCnt;

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + se1OpenCnt, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   vec temp1 = (vec) 0;
   vec temp2 = (vec) 0;
   vec temp3 = (vec) 0;
   vec temp4 = (vec) 0;
   vec temp5 = (vec) 0;
   vec temp6 = (vec) 0;
   vec temp7 = (vec) 0;
   vec temp8 = (vec) 0;

   for (size_t row = 0; row < mainVecs; row++) {
      temp1 = (vec) 0;
      temp2 = (vec) 0;
      temp3 = (vec) 0;
      temp4 = (vec) 0;
      temp5 = (vec) 0;
      temp6 = (vec) 0;
      temp7 = (vec) 0;
      temp8 = (vec) 0;

      vec a1 = c7x::strm_eng<0, vec>::get_adv();
      vec b1 = c7x::strm_eng<1, vec>::get_adv();

      temp1 = a1;

      temp2 = b1;

      vec a2 = c7x::strm_eng<0, vec>::get_adv();
      vec b2 = c7x::strm_eng<1, vec>::get_adv();

      temp3 = a2;

      temp4 = b2;

      vec a3 = c7x::strm_eng<0, vec>::get_adv();
      vec b3 = c7x::strm_eng<1, vec>::get_adv();

      temp5 = a3;

      temp6 = b3;

      vec a4 = c7x::strm_eng<0, vec>::get_adv();
      vec b4 = c7x::strm_eng<1, vec>::get_adv();

      temp7 = a4;

      temp8 = b4;

      a1 = c7x::strm_eng<0, vec>::get_adv();
      b1 = c7x::strm_eng<1, vec>::get_adv();

      temp1 = temp1 + a1;

      temp2 = temp2 + b1;

      a2 = c7x::strm_eng<0, vec>::get_adv();
      b2 = c7x::strm_eng<1, vec>::get_adv();

      temp3 = temp3 + a2;

      temp4 = temp4 + b2;

      a3 = c7x::strm_eng<0, vec>::get_adv();
      b3 = c7x::strm_eng<1, vec>::get_adv();

      temp5 = temp5 + a3;

      temp6 = temp6 + b3;

      a4 = c7x::strm_eng<0, vec>::get_adv();
      b4 = c7x::strm_eng<1, vec>::get_adv();

      temp7 = temp7 + a4;

      temp8 = temp8 + b4;

      a1 = c7x::strm_eng<0, vec>::get_adv();
      b1 = c7x::strm_eng<1, vec>::get_adv();

      temp1 = temp1 + a1;

      temp2 = temp2 + b1;

      a2 = c7x::strm_eng<0, vec>::get_adv();
      b2 = c7x::strm_eng<1, vec>::get_adv();

      temp3 = temp3 + a2;

      temp4 = temp4 + b2;

      a3 = c7x::strm_eng<0, vec>::get_adv();
      b3 = c7x::strm_eng<1, vec>::get_adv();

      temp5 = temp5 + a3;

      temp6 = temp6 + b3;

      a4 = c7x::strm_eng<0, vec>::get_adv();
      b4 = c7x::strm_eng<1, vec>::get_adv();

      temp7 = temp7 + a4;

      temp8 = temp8 + b4;

      a1 = c7x::strm_eng<0, vec>::get_adv();
      b1 = c7x::strm_eng<1, vec>::get_adv();

      temp1 = temp1 + a1;

      temp2 = temp2 + b1;

      a2 = c7x::strm_eng<0, vec>::get_adv();
      b2 = c7x::strm_eng<1, vec>::get_adv();

      temp3 = temp3 + a2;

      temp4 = temp4 + b2;

      a3 = c7x::strm_eng<0, vec>::get_adv();
      b3 = c7x::strm_eng<1, vec>::get_adv();

      temp5 = temp5 + a3;

      temp6 = temp6 + b3;

      a4 = c7x::strm_eng<0, vec>::get_adv();
      b4 = c7x::strm_eng<1, vec>::get_adv();

      temp7 = temp7 + a4;

      temp8 = temp8 + b4;

      __vpred tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp1);

      __vpred tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp2);

      tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp3);

      tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp4);

      tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp5);

      tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp6);

      tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp7);

      tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp8);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA1_CLOSE();

   return status;
}
template <typename dataType>
DSPLIB_STATUS DSPLIB_addNCh_2Inputs_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_addNCh_exec_ci\n");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_addNCh_PrivArgs *pKerPrivArgs = (DSPLIB_addNCh_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint8_t *pBlock   = pKerPrivArgs->bufPblock;
   uint32_t mainVecs = pKerPrivArgs->nVecs;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   uint32_t se1penCnt = *(uint32_t *) ((uint8_t *) pBlock + SE_OPEN_CNT_OFFSET);
   uint32_t sa1penCnt = *(uint32_t *) ((uint8_t *) pBlock + SA_OPEN_CNT_OFFSET);

   dataType *restrict pInLocal   = (dataType *) pIn;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOutLocal1 = pOutLocal + sa1penCnt;

   __SE1_OPEN(pInLocal + se1penCnt, se1Params);
   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   vec temp1 = (vec) 0;
   vec temp2 = (vec) 0;
   vec temp3 = (vec) 0;
   vec temp4 = (vec) 0;
   vec temp5 = (vec) 0;
   vec temp6 = (vec) 0;
   vec temp7 = (vec) 0;
   vec temp8 = (vec) 0;

   for (size_t row = 0; row < mainVecs; row++) {
      temp1 = (vec) 0;
      temp2 = (vec) 0;
      temp3 = (vec) 0;
      temp4 = (vec) 0;
      temp5 = (vec) 0;
      temp6 = (vec) 0;
      temp7 = (vec) 0;
      temp8 = (vec) 0;

      vec a1 = c7x::strm_eng<0, vec>::get_adv();
      vec b1 = c7x::strm_eng<1, vec>::get_adv();

      temp1 = temp1 + a1;

      temp2 = temp2 + b1;

      vec a2 = c7x::strm_eng<0, vec>::get_adv();
      vec b2 = c7x::strm_eng<1, vec>::get_adv();

      temp3 = temp3 + a2;

      temp4 = temp4 + b2;

      vec a3 = c7x::strm_eng<0, vec>::get_adv();
      vec b3 = c7x::strm_eng<1, vec>::get_adv();

      temp5 = temp5 + a3;

      temp6 = temp6 + b3;

      vec a4 = c7x::strm_eng<0, vec>::get_adv();
      vec b4 = c7x::strm_eng<1, vec>::get_adv();

      temp7 = temp7 + a4;

      temp8 = temp8 + b4;

      a1 = c7x::strm_eng<0, vec>::get_adv();
      b1 = c7x::strm_eng<1, vec>::get_adv();

      temp1 = temp1 + a1;

      temp2 = temp2 + b1;

      a2 = c7x::strm_eng<0, vec>::get_adv();
      b2 = c7x::strm_eng<1, vec>::get_adv();

      temp3 = temp3 + a2;

      temp4 = temp4 + b2;

      a3 = c7x::strm_eng<0, vec>::get_adv();
      b3 = c7x::strm_eng<1, vec>::get_adv();

      temp5 = temp5 + a3;

      temp6 = temp6 + b3;

      a4 = c7x::strm_eng<0, vec>::get_adv();
      b4 = c7x::strm_eng<1, vec>::get_adv();

      temp7 = temp7 + a4;

      temp8 = temp8 + b4;

      __vpred tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp1);

      __vpred tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp2);

      tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp3);

      tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp4);

      tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp5);

      tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp6);

      tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp7);

      tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp8);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA1_CLOSE();

   return status;
}
template <typename dataType>
DSPLIB_STATUS DSPLIB_addNCh_1Inputs_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_addNCh_exec_ci\n");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_addNCh_PrivArgs *pKerPrivArgs = (DSPLIB_addNCh_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint8_t *pBlock   = pKerPrivArgs->bufPblock;
   uint32_t mainVecs = pKerPrivArgs->nVecs;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   uint32_t se1penCnt = *(uint32_t *) ((uint8_t *) pBlock + SE_OPEN_CNT_OFFSET);
   uint32_t sa1penCnt = *(uint32_t *) ((uint8_t *) pBlock + SA_OPEN_CNT_OFFSET);

   dataType *restrict pInLocal   = (dataType *) pIn;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOutLocal1 = pOutLocal + sa1penCnt;

   __SE1_OPEN(pInLocal + se1penCnt, se1Params);
   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   vec temp1 = (vec) 0;
   vec temp2 = (vec) 0;
   vec temp3 = (vec) 0;
   vec temp4 = (vec) 0;
   vec temp5 = (vec) 0;
   vec temp6 = (vec) 0;
   vec temp7 = (vec) 0;
   vec temp8 = (vec) 0;

   for (size_t row = 0; row < mainVecs; row++) {
      temp1 = (vec) 0;
      temp2 = (vec) 0;
      temp3 = (vec) 0;
      temp4 = (vec) 0;
      temp5 = (vec) 0;
      temp6 = (vec) 0;
      temp7 = (vec) 0;
      temp8 = (vec) 0;

      vec a1 = c7x::strm_eng<0, vec>::get_adv();
      vec b1 = c7x::strm_eng<1, vec>::get_adv();

      temp1 = temp1 + a1;

      temp2 = temp2 + b1;

      vec a2 = c7x::strm_eng<0, vec>::get_adv();
      vec b2 = c7x::strm_eng<1, vec>::get_adv();

      temp3 = temp3 + a2;

      temp4 = temp4 + b2;

      vec a3 = c7x::strm_eng<0, vec>::get_adv();
      vec b3 = c7x::strm_eng<1, vec>::get_adv();

      temp5 = temp5 + a3;

      temp6 = temp6 + b3;

      vec a4 = c7x::strm_eng<0, vec>::get_adv();
      vec b4 = c7x::strm_eng<1, vec>::get_adv();

      temp7 = temp7 + a4;

      temp8 = temp8 + b4;

      __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, temp1);

      __vpred tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp1, VB2, temp2);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, temp3);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp1, VB2, temp4);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, temp5);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp1, VB2, temp6);

      tmp = c7x::strm_agen<0, vec>::get_vpred();
      VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, temp7);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp1, VB2, temp8);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA1_CLOSE();

   return status;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_addNCh_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "Enter DSPLIB_addNCh_exec_ci\n");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_addNCh_PrivArgs *pKerPrivArgs = (DSPLIB_addNCh_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint32_t inputs   = pKerPrivArgs->inputs;
   uint8_t *pBlock   = pKerPrivArgs->bufPblock;
   uint32_t mainVecs = pKerPrivArgs->nVecs;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   uint32_t se1penCnt = *(uint32_t *) ((uint8_t *) pBlock + SE_OPEN_CNT_OFFSET);
   uint32_t sa1penCnt = *(uint32_t *) ((uint8_t *) pBlock + SA_OPEN_CNT_OFFSET);

   dataType *restrict pInLocal   = (dataType *) pIn;
   dataType *restrict pOutLocal  = (dataType *) pOut;
   dataType *restrict pOutLocal1 = pOutLocal + sa1penCnt;

   __SE1_OPEN(pInLocal + se1penCnt, se1Params);
   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   vec temp1 = (vec) 0;
   vec temp2 = (vec) 0;
   vec temp3 = (vec) 0;
   vec temp4 = (vec) 0;
   vec temp5 = (vec) 0;
   vec temp6 = (vec) 0;
   vec temp7 = (vec) 0;
   vec temp8 = (vec) 0;

   for (size_t row = 0; row < mainVecs; row++) {

      temp1 = (vec) 0;
      temp2 = (vec) 0;
      temp3 = (vec) 0;
      temp4 = (vec) 0;
      temp5 = (vec) 0;
      temp6 = (vec) 0;
      temp7 = (vec) 0;
      temp8 = (vec) 0;

      for (size_t col = 0; col < inputs; col++) {

         vec a1 = c7x::strm_eng<0, vec>::get_adv();
         vec b1 = c7x::strm_eng<1, vec>::get_adv();

         temp1 = temp1 + a1;

         temp2 = temp2 + b1;

         vec a2 = c7x::strm_eng<0, vec>::get_adv();
         vec b2 = c7x::strm_eng<1, vec>::get_adv();

         temp3 = temp3 + a2;

         temp4 = temp4 + b2;

         vec a3 = c7x::strm_eng<0, vec>::get_adv();
         vec b3 = c7x::strm_eng<1, vec>::get_adv();

         temp5 = temp5 + a3;

         temp6 = temp6 + b3;

         vec a4 = c7x::strm_eng<0, vec>::get_adv();
         vec b4 = c7x::strm_eng<1, vec>::get_adv();

         temp7 = temp7 + a4;

         temp8 = temp8 + b4;
      }

      __vpred tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp1);

      __vpred tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp2);

      tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp3);

      tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp4);

      tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp5);

      tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp6);

      tmp1 = c7x::strm_agen<0, vec>::get_vpred();
      VB1  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp1, VB1, temp7);

      tmp2 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pOutLocal1);
      __vstore_pred(tmp2, VB2, temp8);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA1_CLOSE();

   return status;
}
void DSPLIB_addNCh_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles, uint32_t dataType)
{
   DSPLIB_addNCh_PrivArgs *pKerPrivArgs = (DSPLIB_addNCh_PrivArgs *) handle;

   uint64_t addNChStartupCycles   = 0;
   uint64_t addNChTeardownCycles  = 0;
   uint64_t addNChOperationCycles = 0;
   uint64_t addNChOverheadCycles  = 0;
   uint32_t mainVecs              = pKerPrivArgs->nVecs;
   uint32_t inputs                = pKerPrivArgs->inputs;

   addNChStartupCycles += 29;

   if (pKerPrivArgs->inputs == 1) {
      addNChOperationCycles = 3 + mainVecs * 8;
   }
   else if (pKerPrivArgs->inputs == 2) {
      addNChOperationCycles = 7 + mainVecs * 8;
   }
   else if (pKerPrivArgs->inputs == 4) {
      addNChOperationCycles = 7 + mainVecs * 16;
   }
   else {
      addNChOverheadCycles += 8;
      addNChOperationCycles = ((3 + inputs * 4) + 8 + 8) * mainVecs;
   }
   addNChTeardownCycles += 2;
   addNChOverheadCycles += addNChStartupCycles + addNChTeardownCycles;
   *estCycles  = addNChOperationCycles + addNChOverheadCycles;
   *archCycles = addNChOperationCycles;
}

template DSPLIB_STATUS
DSPLIB_addNCh_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_addNCh_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_addNCh_1Inputs_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_addNCh_1Inputs_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_addNCh_2Inputs_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_addNCh_2Inputs_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_addNCh_4Inputs_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_addNCh_4Inputs_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
