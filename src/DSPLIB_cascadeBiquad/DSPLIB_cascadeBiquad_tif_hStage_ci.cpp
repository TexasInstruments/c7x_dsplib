// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_cascadeBiquad_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_hStage_init_ci(DSPLIB_kernelHandle                  handle,
                                                      const DSPLIB_bufParams2D_t          *bufParamsIn,
                                                      const DSPLIB_bufParams2D_t          *bufParamsFilterCoeff,
                                                      const DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                                      const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                      const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;
   __SA_TEMPLATE_v1 sa3Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs       = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock             = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize           = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numChannels        = pKerPrivArgs->initArgs.numChannels;
   uint32_t                       numStages          = pKerPrivArgs->initArgs.numStages;
   uint32_t                       dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t                       dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t                       filterVarPitch     = pKerPrivArgs->filterVarPitch;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   uint32_t                                               eleCount   = c7x::element_count_of<vec>::value;
   uint32_t                                               numChannelGroups;
   uint32_t                                               numStageGroups, stagesPerStageGroup;

   // Compute number of channel groups
   numChannelGroups = (numChannels + eleCount - 1) / eleCount;

   if (numStages <= DSPLIB_CASCADEBIQUAD_NUM_STAGES_8) {
      // Single stage group, all stages in single group

      // Set number of stage groups
      stagesPerStageGroup = numStages;
      // Set number of stage groups
      numStageGroups = 1;
   }
   else {
      // Multiple stage groups, each group contains 7 stages

      // Set number of stage groups
      stagesPerStageGroup = DSPLIB_CASCADEBIQUAD_TIF_STAGES_PER_STAGE_GROUP;
      // Compute number of stage groups
      numStageGroups = (numStages + DSPLIB_CASCADEBIQUAD_TIF_STAGES_PER_STAGE_GROUP - 1) /
                       DSPLIB_CASCADEBIQUAD_TIF_STAGES_PER_STAGE_GROUP;
   }

   // Store number of stage groups
   pKerPrivArgs->numStageGroups = numStageGroups;

   // Compute number of adjacent samples to read/write
   uint32_t icnt0Param = (numChannels > eleCount) ? eleCount : numChannels;

   /**********************************************************************/
   /* Prepare SE1 template to read the coefficients                      */
   /**********************************************************************/
   se1Params               = __gen_SE_TEMPLATE_v1();
   se1Params.ICNT0         = 1;
   se1Params.ICNT1         = DSPLIB_CASCADEBIQUAD_TIF_SE_NUM_BIQUAD_COEF;
   se1Params.DIM1          = 1;
   se1Params.ICNT2         = stagesPerStageGroup;
   se1Params.DIM2          = DSPLIB_NUM_TIF_BIQUAD_COEFS;
   se1Params.ICNT3         = dataSize;
   se1Params.DIM3          = 0;
   se1Params.ICNT4         = numStageGroups;
   se1Params.DIM4          = DSPLIB_NUM_TIF_BIQUAD_COEFS * stagesPerStageGroup;
   se1Params.ICNT5         = numChannelGroups;
   se1Params.DIM5          = 0;
   se1Params.ELETYPE       = SE_ELETYPE;
   se1Params.VECLEN        = SE_VECLEN;
   se1Params.DECDIM1_WIDTH = DSPLIB_NUM_TIF_BIQUAD_COEFS * numStages - DSPLIB_CASCADEBIQUAD_TIF_SE_NUM_BIQUAD_COEF;
   se1Params.DECDIM1       = __SE_DECDIM_DIM4;
   se1Params.DECDIM1SD     = __SE_DECDIMSD_DIM2;
   se1Params.ELEDUP        = c7x::se_eledup<dataType, vec>::value;
   se1Params.DIMFMT        = __SE_DIMFMT_6D;

   /**********************************************************************/
   /* Prepare SA2 template to read the input                             */
   /**********************************************************************/
   sa2Params               = __gen_SA_TEMPLATE_v1();
   sa2Params.ICNT0         = icnt0Param;
   sa2Params.ICNT1         = dataSize;
   sa2Params.DIM1          = (int32_t) dataBufferInPitch;
   sa2Params.ICNT2         = numStageGroups;
   sa2Params.DIM2          = 0;
   sa2Params.ICNT3         = numChannelGroups;
   sa2Params.DIM3          = (int32_t) eleCount;
   sa2Params.VECLEN        = SA_VECLEN;
   sa2Params.DECDIM1_WIDTH = numChannels;
   sa2Params.DECDIM1       = __SA_DECDIM_DIM3;
   sa2Params.DIMFMT        = __SA_DIMFMT_4D;

   /**********************************************************************/
   /* Prepare SA3 template to read the filter variables                  */
   /**********************************************************************/
   sa3Params               = __gen_SA_TEMPLATE_v1();
   sa3Params.ICNT0         = icnt0Param;
   sa3Params.ICNT1         = numStages * DSPLIB_NUM_STATE_VARS_DF2;
   sa3Params.DIM1          = (int32_t) filterVarPitch;
   sa3Params.ICNT2         = numChannelGroups;
   sa3Params.DIM2          = (int32_t) eleCount;
   sa3Params.VECLEN        = SA_VECLEN;
   sa3Params.DECDIM1_WIDTH = numChannels;
   sa3Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa3Params.DIMFMT        = __SA_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare SA0 template to write the output                           */
   /**********************************************************************/
   sa0Params               = __gen_SA_TEMPLATE_v1();
   sa0Params.ICNT0         = icnt0Param;
   sa0Params.ICNT1         = dataSize;
   sa0Params.DIM1          = (int32_t) dataBufferOutPitch;
   sa0Params.ICNT2         = numStageGroups;
   sa0Params.DIM2          = 0;
   sa0Params.ICNT3         = numChannelGroups;
   sa0Params.DIM3          = (int32_t) eleCount;
   sa0Params.DECDIM1_WIDTH = numChannels;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM3;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DIMFMT        = __SA_DIMFMT_4D;

   /**********************************************************************/
   /* Prepare SA1 template to write the filter variables                 */
   /**********************************************************************/
   sa1Params               = __gen_SA_TEMPLATE_v1();
   sa1Params.ICNT0         = icnt0Param;
   sa1Params.ICNT1         = numStages * DSPLIB_NUM_STATE_VARS_DF2;
   sa1Params.DIM1          = (int32_t) filterVarPitch;
   sa1Params.ICNT2         = numChannelGroups;
   sa1Params.DIM2          = (int32_t) eleCount;
   sa1Params.DECDIM1_WIDTH = numChannels;
   sa1Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa1Params.VECLEN        = SA_VECLEN;
   sa1Params.DIMFMT        = __SA_DIMFMT_3D;

   /* Store prepared SE/SA templates */
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET) = sa2Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA3_PARAM_OFFSET) = sa3Params;

   return status;
}

template DSPLIB_STATUS
DSPLIB_cascadeBiquad_tif_hStage_init_ci<float>(DSPLIB_kernelHandle                  handle,
                                               const DSPLIB_bufParams2D_t          *bufParamsIn,
                                               const DSPLIB_bufParams2D_t          *bufParamsFilterCoeff,
                                               const DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                               const DSPLIB_bufParams2D_t          *bufParamsOut,
                                               const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/
template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_4Stage_exec_ci(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn,
                                                      void *restrict pFilterCoeff,
                                                      void *restrict pFilterVar,
                                                      void *restrict pOut)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;
   __SA_TEMPLATE_v1 sa3Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numStages    = pKerPrivArgs->initArgs.numStages;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;
   dataType *pGain;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   vec input;
   vec outGain;

   // Filter coefficients read
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pFilterCoeffLocal, se0Params);
   __SE0_OPEN(pFilterCoeffLocal + DSPLIB_CASCADEBIQUAD_TIF_SE_NUM_BIQUAD_COEF, se0Params);

   // Output samples write
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   // Input samples read
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET);
   __SA2_OPEN(sa2Params);

   // Filter variables read
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA3_PARAM_OFFSET);
   __SA3_OPEN(sa3Params);

   // stage outputs
   vec output1, output2, output3;
   vec output4;

   // coefficients
   vec b11_a11, b21_a21, b31_a31, b41_a41;
   vec b12_a12, b22_a22, b32_a32, b42_a42;
   vec a11, a12, a22, a21, a31, a32, a41, a42;

   // stage temporary variables
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_mul_2, temp0_mul_2, d0_temp_2, d1_mul_2;
   vec temp1_mul_3, temp0_mul_3, d0_temp_3, d1_mul_3;
   vec temp1_mul_4, temp0_mul_4, d0_temp_4, d1_mul_4;
   vec temp1_add_1, temp1_add_2, temp1_add_3;
   vec temp1_add_4;

   // Read aggregate gain
   pGain   = (dataType *) pFilterCoeff + numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS;
   outGain = (vec) *pGain;

#pragma MUST_ITERATE(1, , )
   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      //
      // Read filter variables
      //

      // Stage 1
      __vpred filtVars      = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      vec    *VB4           = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec     filtVars_d0_1 = __vload_pred(filtVars, VB4);
      filtVars              = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4                   = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_1     = __vload_pred(filtVars, VB4);

      // Stage 2
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_2 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_2 = __vload_pred(filtVars, VB4);

      // Stage 3
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_3 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_3 = __vload_pred(filtVars, VB4);

      // Stage 4
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_4 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_4 = __vload_pred(filtVars, VB4);

#pragma MUST_ITERATE(1, , )
      for (uint32_t samp = 0; samp < dataSize; samp++) {
         // Get input
         __vpred tmpld = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         vec    *VB3   = c7x::strm_agen<2, c7x::float_vec>::get_adv(pInLocal);
         input         = __vload_pred(tmpld, VB3);

         // Calculate stage 1
         output1       = input + filtVars_d0_1; // output01 = input + vd00
         b11_a11       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_1   = input * b11_a11;
         temp1_add_1   = temp1_mul_1 + filtVars_d1_1; // t1 = (((b11 + a11) * input) + vd01)
         a12           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_1   = a12 * filtVars_d0_1; // t0 = (a12 * vd00)
         a11           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_1     = a11 * filtVars_d0_1; // vd00 = ((a11 * vd00) + t1)
         filtVars_d0_1 = d0_temp_1 + temp1_add_1;
         b12_a12       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_1      = b12_a12 * input; // vd01 = (((b12 + a12) * input) + t0)
         filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

         // Calculate stage 2
         output2       = output1 + filtVars_d0_2; // output02 = (output01 + vd02)
         b21_a21       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_2   = b21_a21 * output1;
         temp1_add_2   = temp1_mul_2 + filtVars_d1_2; // t3 = (((b21 + a21) * output01) + vd03)
         a22           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_2   = a22 * filtVars_d0_2; // t2 = (a22 * vd02)
         a21           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_2     = a21 * filtVars_d0_2; // vd02 = ((a21 * vd02) + t3)
         filtVars_d0_2 = d0_temp_2 + temp1_add_2;
         b22_a22       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_2      = b22_a22 * output1; // vd03 = (((b22 + a22) * output01) + t2)
         filtVars_d1_2 = d1_mul_2 + temp0_mul_2;

         // Calculate stage 3
         output3       = output2 + filtVars_d0_3; // output03 = (output02 + vd04)
         b31_a31       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_3   = b31_a31 * output2;
         temp1_add_3   = temp1_mul_3 + filtVars_d1_3; // t5 = (((b31 + a31) * output02) + vd05)
         a32           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_3   = a32 * filtVars_d0_3; // t4 = (a32 * vd04)
         a31           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_3     = a31 * filtVars_d0_3; // vd04 = ((a31 * vd04) + t5)
         filtVars_d0_3 = d0_temp_3 + temp1_add_3;
         b32_a32       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_3      = b32_a32 * output2; // vd05 = (((b32 + a32) * output02) + t4)
         filtVars_d1_3 = d1_mul_3 + temp0_mul_3;

         // Calculate stage 4
         output4       = output3 + filtVars_d0_4; // output04 = (output03 + vd06)
         b41_a41       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_4   = b41_a41 * output3;
         temp1_add_4   = temp1_mul_4 + filtVars_d1_4; // t7 = (((b41 + a41) * output03) + vd07)
         a42           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_4   = a42 * filtVars_d0_4; // t6 = (a42 * vd06)
         a41           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_4     = a41 * filtVars_d0_4; // vd06 = ((a41 * vd06) + t7)
         filtVars_d0_4 = d0_temp_4 + temp1_add_4;
         b42_a42       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_4      = b42_a42 * output3; // vd07 = (((b42 + a42) * output03) + t6)
         filtVars_d1_4 = d1_mul_4 + temp0_mul_4;

         // Apply aggregate gain
         vec output = output4 * outGain;

         // Write output
         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, output);
      }

      //
      // Write filter variables
      //

      // Stage 1
      __vpred tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_1);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_1);

      // Stage 2
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_2);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_2);

      // Stage 3
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_3);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_3);

      // Stage 4
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_4);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_4);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_4Stage_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                      void *restrict pIn,
                                                                      void *restrict pFilterCoeff,
                                                                      void *restrict pFilterVar,
                                                                      void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_5Stage_exec_ci(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn,
                                                      void *restrict pFilterCoeff,
                                                      void *restrict pFilterVar,
                                                      void *restrict pOut)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;
   __SA_TEMPLATE_v1 sa3Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numStages    = pKerPrivArgs->initArgs.numStages;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;
   dataType *pGain;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   vec input;
   vec outGain;

   // Filter coefficients read
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pFilterCoeffLocal, se0Params);
   __SE0_OPEN(pFilterCoeffLocal + DSPLIB_CASCADEBIQUAD_TIF_SE_NUM_BIQUAD_COEF, se0Params);

   // Output samples write
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   // Input samples read
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET);
   __SA2_OPEN(sa2Params);

   // Filter variables read
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA3_PARAM_OFFSET);
   __SA3_OPEN(sa3Params);

   // stage outputs
   vec output1, output2, output3;
   vec output4, output5;

   // coefficients
   vec b11_a11, b21_a21, b31_a31, b41_a41, b51_a51;
   vec b12_a12, b22_a22, b32_a32, b42_a42, b52_a52;
   vec a11, a12, a22, a21, a31, a32, a41, a42, a51, a52;

   // stage temporary variables
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_mul_2, temp0_mul_2, d0_temp_2, d1_mul_2;
   vec temp1_mul_3, temp0_mul_3, d0_temp_3, d1_mul_3;
   vec temp1_mul_4, temp0_mul_4, d0_temp_4, d1_mul_4;
   vec temp1_mul_5, temp0_mul_5, d0_temp_5, d1_mul_5;
   vec temp1_add_1, temp1_add_2, temp1_add_3;
   vec temp1_add_4, temp1_add_5;

   // Read aggregate gain
   pGain   = (dataType *) pFilterCoeff + numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS;
   outGain = (vec) *pGain;

#pragma MUST_ITERATE(1, , )
   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      //
      // Read filter variables
      //

      // Stage 1
      __vpred filtVars      = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      vec    *VB4           = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec     filtVars_d0_1 = __vload_pred(filtVars, VB4);
      filtVars              = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4                   = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_1     = __vload_pred(filtVars, VB4);

      // Stage 2
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_2 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_2 = __vload_pred(filtVars, VB4);

      // Stage 3
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_3 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_3 = __vload_pred(filtVars, VB4);

      // Stage 4
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_4 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_4 = __vload_pred(filtVars, VB4);

      // Stage 5
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_5 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_5 = __vload_pred(filtVars, VB4);

#pragma MUST_ITERATE(1, , )
      for (uint32_t samp = 0; samp < dataSize; samp++) {
         // Get input
         __vpred tmpld = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         vec    *VB3   = c7x::strm_agen<2, c7x::float_vec>::get_adv(pInLocal);
         input         = __vload_pred(tmpld, VB3);

         // Calculate stage 1
         output1       = input + filtVars_d0_1; // output01 = input + vd00
         b11_a11       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_1   = input * b11_a11;
         temp1_add_1   = temp1_mul_1 + filtVars_d1_1; // t1 = (((b11 + a11) * input) + vd01)
         a12           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_1   = a12 * filtVars_d0_1; // t0 = (a12 * vd00)
         a11           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_1     = a11 * filtVars_d0_1; // vd00 = ((a11 * vd00) + t1)
         filtVars_d0_1 = d0_temp_1 + temp1_add_1;
         b12_a12       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_1      = b12_a12 * input; // vd01 = (((b12 + a12) * input) + t0)
         filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

         // Calculate stage 2
         output2       = output1 + filtVars_d0_2; // output02 = (output01 + vd02)
         b21_a21       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_2   = b21_a21 * output1;
         temp1_add_2   = temp1_mul_2 + filtVars_d1_2; // t3 = (((b21 + a21) * output01) + vd03)
         a22           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_2   = a22 * filtVars_d0_2; // t2 = (a22 * vd02)
         a21           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_2     = a21 * filtVars_d0_2; // vd02 = ((a21 * vd02) + t3)
         filtVars_d0_2 = d0_temp_2 + temp1_add_2;
         b22_a22       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_2      = b22_a22 * output1; // vd03 = (((b22 + a22) * output01) + t2)
         filtVars_d1_2 = d1_mul_2 + temp0_mul_2;

         // Calculate stage 3
         output3       = output2 + filtVars_d0_3; // output03 = (output02 + vd04)
         b31_a31       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_3   = b31_a31 * output2;
         temp1_add_3   = temp1_mul_3 + filtVars_d1_3; // t5 = (((b31 + a31) * output02) + vd05)
         a32           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_3   = a32 * filtVars_d0_3; // t4 = (a32 * vd04)
         a31           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_3     = a31 * filtVars_d0_3; // vd04 = ((a31 * vd04) + t5)
         filtVars_d0_3 = d0_temp_3 + temp1_add_3;
         b32_a32       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_3      = b32_a32 * output2; // vd05 = (((b32 + a32) * output02) + t4)
         filtVars_d1_3 = d1_mul_3 + temp0_mul_3;

         // Calculate stage 4
         output4       = output3 + filtVars_d0_4; // output04 = (output03 + vd06)
         b41_a41       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_4   = b41_a41 * output3;
         temp1_add_4   = temp1_mul_4 + filtVars_d1_4; // t7 = (((b41 + a41) * output03) + vd07)
         a42           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_4   = a42 * filtVars_d0_4; // t6 = (a42 * vd06)
         a41           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_4     = a41 * filtVars_d0_4; // vd06 = ((a41 * vd06) + t7)
         filtVars_d0_4 = d0_temp_4 + temp1_add_4;
         b42_a42       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_4      = b42_a42 * output3; // vd07 = (((b42 + a42) * output03) + t6)
         filtVars_d1_4 = d1_mul_4 + temp0_mul_4;

         // Calculate stage 5
         output5       = output4 + filtVars_d0_5; // output05 = (output04 + vd08)
         b51_a51       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_5   = b51_a51 * output4;
         temp1_add_5   = temp1_mul_5 + filtVars_d1_5; // t9 = (((b51 + a51) * output04) + vd09)
         a52           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_5   = a52 * filtVars_d0_5; // t8 = (a52 * vd08)
         a51           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_5     = a51 * filtVars_d0_5; // vd08 = ((a51 * vd08) + t9)
         filtVars_d0_5 = d0_temp_5 + temp1_add_5;
         b52_a52       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_5      = b52_a52 * output4; // vd09 = (((b52 + a52) * output04) + t8)
         filtVars_d1_5 = d1_mul_5 + temp0_mul_5;

         // Apply aggregate gain
         vec output = output5 * outGain;

         // Write output
         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, output);
      }

      //
      // Write filter variables
      //

      // Stage 1
      __vpred tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_1);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_1);

      // Stage 2
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_2);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_2);

      // Stage 3
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_3);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_3);

      // Stage 4
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_4);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_4);

      // Stage 5
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_5);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_5);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_5Stage_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                      void *restrict pIn,
                                                                      void *restrict pFilterCoeff,
                                                                      void *restrict pFilterVar,
                                                                      void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_6Stage_exec_ci(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn,
                                                      void *restrict pFilterCoeff,
                                                      void *restrict pFilterVar,
                                                      void *restrict pOut)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;
   __SA_TEMPLATE_v1 sa3Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numStages    = pKerPrivArgs->initArgs.numStages;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;
   dataType *pGain;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   vec input;
   vec outGain;

   // Filter coefficients read
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pFilterCoeffLocal, se0Params);
   __SE0_OPEN(pFilterCoeffLocal + DSPLIB_CASCADEBIQUAD_TIF_SE_NUM_BIQUAD_COEF, se0Params);

   // Output samples write
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   // Input samples read
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET);
   __SA2_OPEN(sa2Params);

   // Filter variables read
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA3_PARAM_OFFSET);
   __SA3_OPEN(sa3Params);

   // stage outputs
   vec output1, output2, output3;
   vec output4, output5, output6;

   // coefficients
   vec b11_a11, b21_a21, b31_a31, b41_a41, b51_a51, b61_a61;
   vec b12_a12, b22_a22, b32_a32, b42_a42, b52_a52, b62_a62;
   vec a11, a12, a22, a21, a31, a32, a41, a42, a51, a52, a61, a62;

   // stage temporary variables
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_mul_2, temp0_mul_2, d0_temp_2, d1_mul_2;
   vec temp1_mul_3, temp0_mul_3, d0_temp_3, d1_mul_3;
   vec temp1_mul_4, temp0_mul_4, d0_temp_4, d1_mul_4;
   vec temp1_mul_5, temp0_mul_5, d0_temp_5, d1_mul_5;
   vec temp1_mul_6, temp0_mul_6, d0_temp_6, d1_mul_6;
   vec temp1_add_1, temp1_add_2, temp1_add_3;
   vec temp1_add_4, temp1_add_5, temp1_add_6;

   // Read aggregate gain
   pGain   = (dataType *) pFilterCoeff + numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS;
   outGain = (vec) *pGain;

#pragma MUST_ITERATE(1, , )
   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      //
      // Read filter variables
      //

      // Stage 1
      __vpred filtVars      = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      vec    *VB4           = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec     filtVars_d0_1 = __vload_pred(filtVars, VB4);
      filtVars              = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4                   = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_1     = __vload_pred(filtVars, VB4);

      // Stage 2
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_2 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_2 = __vload_pred(filtVars, VB4);

      // Stage 3
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_3 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_3 = __vload_pred(filtVars, VB4);

      // Stage 4
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_4 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_4 = __vload_pred(filtVars, VB4);

      // Stage 5
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_5 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_5 = __vload_pred(filtVars, VB4);

      // Stage 6
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_6 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_6 = __vload_pred(filtVars, VB4);

#pragma MUST_ITERATE(1, , )
      for (uint32_t samp = 0; samp < dataSize; samp++) {
         // Read input
         __vpred tmpld = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         vec    *VB3   = c7x::strm_agen<2, c7x::float_vec>::get_adv(pInLocal);
         input         = __vload_pred(tmpld, VB3);

         // Calculate stage 1
         output1       = input + filtVars_d0_1; // output01 = input + vd00
         b11_a11       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_1   = input * b11_a11;
         temp1_add_1   = temp1_mul_1 + filtVars_d1_1; // t1 = (((b11 + a11) * input) + vd01)
         a12           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_1   = a12 * filtVars_d0_1; // t0 = (a12 * vd00)
         a11           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_1     = a11 * filtVars_d0_1; // vd00 = ((a11 * vd00) + t1)
         filtVars_d0_1 = d0_temp_1 + temp1_add_1;
         b12_a12       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_1      = b12_a12 * input; // vd01 = (((b12 + a12) * input) + t0)
         filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

         // Calculate stage 2
         output2       = output1 + filtVars_d0_2; // output02 = (output01 + vd02)
         b21_a21       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_2   = b21_a21 * output1;
         temp1_add_2   = temp1_mul_2 + filtVars_d1_2; // t3 = (((b21 + a21) * output01) + vd03)
         a22           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_2   = a22 * filtVars_d0_2; // t2 = (a22 * vd02)
         a21           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_2     = a21 * filtVars_d0_2; // vd02 = ((a21 * vd02) + t3)
         filtVars_d0_2 = d0_temp_2 + temp1_add_2;
         b22_a22       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_2      = b22_a22 * output1; // vd03 = (((b22 + a22) * output01) + t2)
         filtVars_d1_2 = d1_mul_2 + temp0_mul_2;

         // Calculate stage 3
         output3       = output2 + filtVars_d0_3; // output03 = (output02 + vd04)
         b31_a31       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_3   = b31_a31 * output2;
         temp1_add_3   = temp1_mul_3 + filtVars_d1_3; // t5 = (((b31 + a31) * output02) + vd05)
         a32           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_3   = a32 * filtVars_d0_3; // t4 = (a32 * vd04)
         a31           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_3     = a31 * filtVars_d0_3; // vd04 = ((a31 * vd04) + t5)
         filtVars_d0_3 = d0_temp_3 + temp1_add_3;
         b32_a32       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_3      = b32_a32 * output2; // vd05 = (((b32 + a32) * output02) + t4)
         filtVars_d1_3 = d1_mul_3 + temp0_mul_3;

         // Calculate stage 4
         output4       = output3 + filtVars_d0_4; // output04 = (output03 + vd06)
         b41_a41       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_4   = b41_a41 * output3;
         temp1_add_4   = temp1_mul_4 + filtVars_d1_4; // t7 = (((b41 + a41) * output03) + vd07)
         a42           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_4   = a42 * filtVars_d0_4; // t6 = (a42 * vd06)
         a41           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_4     = a41 * filtVars_d0_4; // vd06 = ((a41 * vd06) + t7)
         filtVars_d0_4 = d0_temp_4 + temp1_add_4;
         b42_a42       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_4      = b42_a42 * output3; // vd07 = (((b42 + a42) * output03) + t6)
         filtVars_d1_4 = d1_mul_4 + temp0_mul_4;

         // Calculate stage 5
         output5       = output4 + filtVars_d0_5; // output05 = (output04 + vd08)
         b51_a51       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_5   = b51_a51 * output4;
         temp1_add_5   = temp1_mul_5 + filtVars_d1_5; // t9 = (((b51 + a51) * output04) + vd09)
         a52           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_5   = a52 * filtVars_d0_5; // t8 = (a52 * vd08)
         a51           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_5     = a51 * filtVars_d0_5; // vd08 = ((a51 * vd08) + t9)
         filtVars_d0_5 = d0_temp_5 + temp1_add_5;
         b52_a52       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_5      = b52_a52 * output4; // vd09 = (((b52 + a52) * output04) + t8)
         filtVars_d1_5 = d1_mul_5 + temp0_mul_5;

         // Calculate stage 6
         output6       = output5 + filtVars_d0_6; // output06 = (output05 + vd10)
         b61_a61       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_6   = b61_a61 * output5;
         temp1_add_6   = temp1_mul_6 + filtVars_d1_6; // t11 = (((b61 + a61) * output05) + vd11)
         a62           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_6   = a62 * filtVars_d0_6; // t10 = (a62 * vd10)
         a61           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_6     = a61 * filtVars_d0_6; // vd10 = ((a61 * vd10) + t11)
         filtVars_d0_6 = d0_temp_6 + temp1_add_6;
         b62_a62       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_6      = b62_a62 * output5; // vd11 = (((b62 + a62) * output05) + t10)
         filtVars_d1_6 = d1_mul_6 + temp0_mul_6;

         // Apply aggregate gain
         vec output = output6 * outGain;

         // Write output
         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, output);
      }

      //
      // Write filter variables
      //

      // Stage 1
      __vpred tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_1);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_1);

      // Stage 2
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_2);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_2);

      // Stage 3
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_3);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_3);

      // Stage 4
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_4);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_4);

      // Stage 5
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_5);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_5);

      // Stage 6
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_6);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_6);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_6Stage_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                      void *restrict pIn,
                                                                      void *restrict pFilterCoeff,
                                                                      void *restrict pFilterVar,
                                                                      void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_8Stage_exec_ci(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn,
                                                      void *restrict pFilterCoeff,
                                                      void *restrict pFilterVar,
                                                      void *restrict pOut)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;
   __SA_TEMPLATE_v1 sa3Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numStages    = pKerPrivArgs->initArgs.numStages;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;
   dataType *pGain;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   vec input;
   vec outGain;

   // Filter coefficients read
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pFilterCoeffLocal, se0Params);
   __SE0_OPEN(pFilterCoeffLocal + DSPLIB_CASCADEBIQUAD_TIF_SE_NUM_BIQUAD_COEF, se0Params);

   // Output samples write
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   // Input samples read
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET);
   __SA2_OPEN(sa2Params);

   // Filter variables read
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA3_PARAM_OFFSET);
   __SA3_OPEN(sa3Params);

   // stage outputs
   vec output1, output2, output3;
   vec output4, output5, output6, output7, output8;

   // coefficients
   vec b11_a11, b21_a21, b31_a31, b41_a41, b51_a51, b61_a61, b71_a71, b81_a81;
   vec b12_a12, b22_a22, b32_a32, b42_a42, b52_a52, b62_a62, b72_a72, b82_a82;
   vec a11, a12, a22, a21, a31, a32, a41, a42, a51, a52, a61, a62, a71, a72, a81, a82;

   // stage temporary variables
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_mul_2, temp0_mul_2, d0_temp_2, d1_mul_2;
   vec temp1_mul_3, temp0_mul_3, d0_temp_3, d1_mul_3;
   vec temp1_mul_4, temp0_mul_4, d0_temp_4, d1_mul_4;
   vec temp1_mul_5, temp0_mul_5, d0_temp_5, d1_mul_5;
   vec temp1_mul_6, temp0_mul_6, d0_temp_6, d1_mul_6;
   vec temp1_mul_7, temp0_mul_7, d0_temp_7, d1_mul_7;
   vec temp1_mul_8, temp0_mul_8, d0_temp_8, d1_mul_8;
   vec temp1_add_1, temp1_add_2, temp1_add_3;
   vec temp1_add_4, temp1_add_5, temp1_add_6, temp1_add_7, temp1_add_8;

   // Read aggregate gain
   pGain   = (dataType *) pFilterCoeff + numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS;
   outGain = (vec) *pGain;

#pragma MUST_ITERATE(1, , )
   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      //
      // Read filter variables
      //

      // Stage 1
      __vpred filtVars      = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      vec    *VB4           = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec     filtVars_d0_1 = __vload_pred(filtVars, VB4);
      filtVars              = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4                   = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_1     = __vload_pred(filtVars, VB4);

      // Stage 2
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_2 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_2 = __vload_pred(filtVars, VB4);

      // Stage 3
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_3 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_3 = __vload_pred(filtVars, VB4);

      // Stage 4
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_4 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_4 = __vload_pred(filtVars, VB4);

      // Stage 5
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_5 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_5 = __vload_pred(filtVars, VB4);

      // Stage 6
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_6 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_6 = __vload_pred(filtVars, VB4);

      // Stage 7
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_7 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_7 = __vload_pred(filtVars, VB4);

      // Stage 8
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_8 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_8 = __vload_pred(filtVars, VB4);

#pragma MUST_ITERATE(1, , )
      for (uint32_t samp = 0; samp < dataSize; samp++) {
         // Get input
         __vpred tmpld = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         vec    *VB3   = c7x::strm_agen<2, c7x::float_vec>::get_adv(pInLocal);
         input         = __vload_pred(tmpld, VB3);

         // Calculate stage 1
         output1       = input + filtVars_d0_1; // output01 = input + vd00
         b11_a11       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_1   = input * b11_a11;
         temp1_add_1   = temp1_mul_1 + filtVars_d1_1; // t1 = (((b11 + a11) * input) + vd01)
         a12           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_1   = a12 * filtVars_d0_1; // t0 = (a12 * vd00)
         a11           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_1     = a11 * filtVars_d0_1; // vd00 = ((a11 * vd00) + t1)
         filtVars_d0_1 = d0_temp_1 + temp1_add_1;
         b12_a12       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_1      = b12_a12 * input; // vd01 = (((b12 + a12) * input) + t0)
         filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

         // Calculate stage 2
         output2       = output1 + filtVars_d0_2; // output02 = (output01 + vd02)
         b21_a21       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_2   = b21_a21 * output1;
         temp1_add_2   = temp1_mul_2 + filtVars_d1_2; // t3 = (((b21 + a21) * output01) + vd03)
         a22           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_2   = a22 * filtVars_d0_2; // t2 = (a22 * vd02)
         a21           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_2     = a21 * filtVars_d0_2; // vd02 = ((a21 * vd02) + t3)
         filtVars_d0_2 = d0_temp_2 + temp1_add_2;
         b22_a22       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_2      = b22_a22 * output1; // vd03 = (((b22 + a22) * output01) + t2)
         filtVars_d1_2 = d1_mul_2 + temp0_mul_2;

         // Calculate stage 3
         output3       = output2 + filtVars_d0_3; // output03 = (output02 + vd04)
         b31_a31       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_3   = b31_a31 * output2;
         temp1_add_3   = temp1_mul_3 + filtVars_d1_3; // t5 = (((b31 + a31) * output02) + vd05)
         a32           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_3   = a32 * filtVars_d0_3; // t4 = (a32 * vd04)
         a31           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_3     = a31 * filtVars_d0_3; // vd04 = ((a31 * vd04) + t5)
         filtVars_d0_3 = d0_temp_3 + temp1_add_3;
         b32_a32       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_3      = b32_a32 * output2; // vd05 = (((b32 + a32) * output02) + t4)
         filtVars_d1_3 = d1_mul_3 + temp0_mul_3;

         // Calculate stage 4
         output4       = output3 + filtVars_d0_4; // output04 = (output03 + vd06)
         b41_a41       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_4   = b41_a41 * output3;
         temp1_add_4   = temp1_mul_4 + filtVars_d1_4; // t7 = (((b41 + a41) * output03) + vd07)
         a42           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_4   = a42 * filtVars_d0_4; // t6 = (a42 * vd06)
         a41           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_4     = a41 * filtVars_d0_4; // vd06 = ((a41 * vd06) + t7)
         filtVars_d0_4 = d0_temp_4 + temp1_add_4;
         b42_a42       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_4      = b42_a42 * output3; // vd07 = (((b42 + a42) * output03) + t6)
         filtVars_d1_4 = d1_mul_4 + temp0_mul_4;

         // Calculate stage 5
         output5       = output4 + filtVars_d0_5; // output05 = (output04 + vd08)
         b51_a51       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_5   = b51_a51 * output4;
         temp1_add_5   = temp1_mul_5 + filtVars_d1_5; // t9 = (((b51 + a51) * output04) + vd09)
         a52           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_5   = a52 * filtVars_d0_5; // t8 = (a52 * vd08)
         a51           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_5     = a51 * filtVars_d0_5; // vd08 = ((a51 * vd08) + t9)
         filtVars_d0_5 = d0_temp_5 + temp1_add_5;
         b52_a52       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_5      = b52_a52 * output4; // vd09 = (((b52 + a52) * output04) + t8)
         filtVars_d1_5 = d1_mul_5 + temp0_mul_5;

         // Calculate stage 6
         output6       = output5 + filtVars_d0_6; // output06 = (output05 + vd10)
         b61_a61       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_6   = b61_a61 * output5;
         temp1_add_6   = temp1_mul_6 + filtVars_d1_6; // t11 = (((b61 + a61) * output05) + vd11)
         a62           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_6   = a62 * filtVars_d0_6; // t10 = (a62 * vd10)
         a61           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_6     = a61 * filtVars_d0_6; // vd10 = ((a61 * vd10) + t11)
         filtVars_d0_6 = d0_temp_6 + temp1_add_6;
         b62_a62       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_6      = b62_a62 * output5; // vd11 = (((b62 + a62) * output05) + t10)
         filtVars_d1_6 = d1_mul_6 + temp0_mul_6;

         // Calculate stage 7
         output7       = output6 + filtVars_d0_7; // output07 = (output06 + vd12)
         b71_a71       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_7   = b71_a71 * output6;
         temp1_add_7   = temp1_mul_7 + filtVars_d1_7; // t13 = (((b71 + a71) * output06) + vd13)
         a72           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_7   = a72 * filtVars_d0_7; // t12 = (a72 * vd12)
         a71           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_7     = a71 * filtVars_d0_7; // vd12 = ((a71 * vd12) + t13)
         filtVars_d0_7 = d0_temp_7 + temp1_add_7;
         b72_a72       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_7      = b72_a72 * output6; // vd13 = (((b72 + a72) * output06) + t12)
         filtVars_d1_7 = d1_mul_7 + temp0_mul_7;

         // Calculate stage 8
         output8       = output7 + filtVars_d0_8; // output08 = (output07 + vd14)
         b81_a81       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_8   = b81_a81 * output7;
         temp1_add_8   = temp1_mul_8 + filtVars_d1_8; // t15 = (((b81 + a81) * output07) + vd15)
         a82           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_8   = a82 * filtVars_d0_8; // t14 = (a82 * vd14)
         a81           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_8     = a81 * filtVars_d0_8; // vd14 = ((a81 * vd14) + t15)
         filtVars_d0_8 = d0_temp_8 + temp1_add_8;
         b82_a82       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_8      = b82_a82 * output7; // vd15 = (((b82 + a82) * output07) + t14)
         filtVars_d1_8 = d1_mul_8 + temp0_mul_8;

         // Apply aggregate gain
         vec output = output8 * outGain;

         // Write output
         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, output);
      }

      //
      // Write filter variables
      //

      // Stage 1
      __vpred tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_1);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_1);

      // Stage 2
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_2);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_2);

      // Stage 3
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_3);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_3);

      // Stage 4
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_4);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_4);

      // Stage 5
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_5);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_5);

      // Stage 6
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_6);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_6);

      // Stage 7
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_7);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_7);

      // Stage 8
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_8);
      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_8);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_8Stage_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                      void *restrict pIn,
                                                                      void *restrict pFilterCoeff,
                                                                      void *restrict pFilterVar,
                                                                      void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_MStage_exec_ci(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn,
                                                      void *restrict pFilterCoeff,
                                                      void *restrict pFilterVar,
                                                      void *restrict pOut)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;
   __SA_TEMPLATE_v1 sa3Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numStages    = pKerPrivArgs->initArgs.numStages;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;
   dataType *pGain;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   vec input;
   vec outGain;

   // Filter coefficients read
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pFilterCoeffLocal, se0Params);
   __SE0_OPEN(pFilterCoeffLocal + DSPLIB_CASCADEBIQUAD_TIF_SE_NUM_BIQUAD_COEF, se0Params);

   // Output samples write
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   // Input samples read
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET);
   __SA2_OPEN(sa2Params);

   // Filter variables read
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA3_PARAM_OFFSET);
   __SA3_OPEN(sa3Params);

   // stage outputs
   vec output1, output2, output3;
   vec output4, output5, output6, output7;

   // coefficients
   vec b11_a11, b21_a21, b31_a31, b41_a41, b51_a51, b61_a61, b71_a71;
   vec b12_a12, b22_a22, b32_a32, b42_a42, b52_a52, b62_a62, b72_a72;
   vec a11, a12, a22, a21, a31, a32, a41, a42, a51, a52, a61, a62, a71, a72;

   // stage temporary variables
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_mul_2, temp0_mul_2, d0_temp_2, d1_mul_2;
   vec temp1_mul_3, temp0_mul_3, d0_temp_3, d1_mul_3;
   vec temp1_mul_4, temp0_mul_4, d0_temp_4, d1_mul_4;
   vec temp1_mul_5, temp0_mul_5, d0_temp_5, d1_mul_5;
   vec temp1_mul_6, temp0_mul_6, d0_temp_6, d1_mul_6;
   vec temp1_mul_7, temp0_mul_7, d0_temp_7, d1_mul_7;
   vec temp1_add_1, temp1_add_2, temp1_add_3;
   vec temp1_add_4, temp1_add_5, temp1_add_6, temp1_add_7;

   // Get number of stage groups
   uint32_t numStageGroups = pKerPrivArgs->numStageGroups;

#pragma MUST_ITERATE(1, , )
   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      // Set gain pointer for current channel group
      pGain = (dataType *) pFilterCoeff + numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS;

#pragma MUST_ITERATE(1, , )
      for (uint32_t stageCount = 0; stageCount < numStageGroups; stageCount++) {
         // Get gain for current stage group
         outGain = (vec) *pGain;
         pGain++;

         //
         // Read filter variables
         //

         // Stage 1
         __vpred filtVars      = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         vec    *VB4           = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec     filtVars_d0_1 = __vload_pred(filtVars, VB4);
         filtVars              = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4                   = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d1_1     = __vload_pred(filtVars, VB4);

         // Stage 2
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d0_2 = __vload_pred(filtVars, VB4);
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d1_2 = __vload_pred(filtVars, VB4);

         // Stage 3
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d0_3 = __vload_pred(filtVars, VB4);
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d1_3 = __vload_pred(filtVars, VB4);

         // Stage 4
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d0_4 = __vload_pred(filtVars, VB4);
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d1_4 = __vload_pred(filtVars, VB4);

         // Stage 5
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d0_5 = __vload_pred(filtVars, VB4);
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d1_5 = __vload_pred(filtVars, VB4);

         // Stage 6
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d0_6 = __vload_pred(filtVars, VB4);
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d1_6 = __vload_pred(filtVars, VB4);

         // Stage 7
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d0_7 = __vload_pred(filtVars, VB4);
         filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
         VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec filtVars_d1_7 = __vload_pred(filtVars, VB4);

#pragma MUST_ITERATE(1, , )
         for (uint32_t samp = 0; samp < dataSize; samp++) {
            // Get input
            __vpred tmpld = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
            vec    *VB3   = c7x::strm_agen<2, c7x::float_vec>::get_adv(pInLocal);
            input         = __vload_pred(tmpld, VB3);

            // Calculate stage 1
            output1       = input + filtVars_d0_1; // output01 = input + vd00
            b11_a11       = c7x::strm_eng<1, vec>::get_adv();
            temp1_mul_1   = input * b11_a11;
            temp1_add_1   = temp1_mul_1 + filtVars_d1_1; // t1 = (((b11 + a11) * input) + vd01)
            a12           = c7x::strm_eng<0, vec>::get_adv();
            temp0_mul_1   = a12 * filtVars_d0_1; // t0 = (a12 * vd00)
            a11           = c7x::strm_eng<0, vec>::get_adv();
            d0_temp_1     = a11 * filtVars_d0_1; // vd00 = ((a11 * vd00) + t1)
            filtVars_d0_1 = d0_temp_1 + temp1_add_1;
            b12_a12       = c7x::strm_eng<1, vec>::get_adv();
            d1_mul_1      = b12_a12 * input; // vd01 = (((b12 + a12) * input) + t0)
            filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

            // Calculate stage 2
            output2       = output1 + filtVars_d0_2; // output02 = (output01 + vd02)
            b21_a21       = c7x::strm_eng<1, vec>::get_adv();
            temp1_mul_2   = b21_a21 * output1; // t3 = (((b21 + a21) * output01) + vd03)
            temp1_add_2   = temp1_mul_2 + filtVars_d1_2;
            a22           = c7x::strm_eng<0, vec>::get_adv();
            temp0_mul_2   = a22 * filtVars_d0_2; // t2 = (a22 * vd02)
            a21           = c7x::strm_eng<0, vec>::get_adv();
            d0_temp_2     = a21 * filtVars_d0_2; // vd02 = ((a21 * vd02) + t3)
            filtVars_d0_2 = d0_temp_2 + temp1_add_2;
            b22_a22       = c7x::strm_eng<1, vec>::get_adv();
            d1_mul_2      = b22_a22 * output1; // vd03 = (((b22 + a22) * output01) + t2)
            filtVars_d1_2 = d1_mul_2 + temp0_mul_2;

            // Calculate stage 3
            output3       = output2 + filtVars_d0_3; // output03 = (output02 + vd04)
            b31_a31       = c7x::strm_eng<1, vec>::get_adv();
            temp1_mul_3   = b31_a31 * output2;
            temp1_add_3   = temp1_mul_3 + filtVars_d1_3; // t5 = (((b31 + a31) * output02) + vd05)
            a32           = c7x::strm_eng<0, vec>::get_adv();
            temp0_mul_3   = a32 * filtVars_d0_3; // t4 = (a32 * vd04)
            a31           = c7x::strm_eng<0, vec>::get_adv();
            d0_temp_3     = a31 * filtVars_d0_3; // vd04 = ((a31 * vd04) + t5)
            filtVars_d0_3 = d0_temp_3 + temp1_add_3;
            b32_a32       = c7x::strm_eng<1, vec>::get_adv();
            d1_mul_3      = b32_a32 * output2; // vd05 = (((b32 + a32) * output02) + t4)
            filtVars_d1_3 = d1_mul_3 + temp0_mul_3;

            // Calculate stage 4
            output4       = output3 + filtVars_d0_4; // output04 = (output03 + vd06)
            b41_a41       = c7x::strm_eng<1, vec>::get_adv();
            temp1_mul_4   = b41_a41 * output3;
            temp1_add_4   = temp1_mul_4 + filtVars_d1_4; // t7 = (((b41 + a41) * output03) + vd07)
            a42           = c7x::strm_eng<0, vec>::get_adv();
            temp0_mul_4   = a42 * filtVars_d0_4; // t6 = (a42 * vd06)
            a41           = c7x::strm_eng<0, vec>::get_adv();
            d0_temp_4     = a41 * filtVars_d0_4; // vd06 = ((a41 * vd06) + t7)
            filtVars_d0_4 = d0_temp_4 + temp1_add_4;
            b42_a42       = c7x::strm_eng<1, vec>::get_adv();
            d1_mul_4      = b42_a42 * output3; // vd07 = (((b42 + a42) * output03) + t6)
            filtVars_d1_4 = d1_mul_4 + temp0_mul_4;

            // Calculate stage 5
            output5       = output4 + filtVars_d0_5; // output05 = (output04 + vd08)
            b51_a51       = c7x::strm_eng<1, vec>::get_adv();
            temp1_mul_5   = b51_a51 * output4;
            temp1_add_5   = temp1_mul_5 + filtVars_d1_5; // t9 = (((b51 + a51) * output04) + vd09)
            a52           = c7x::strm_eng<0, vec>::get_adv();
            temp0_mul_5   = a52 * filtVars_d0_5; // t8 = (a52 * vd08)
            a51           = c7x::strm_eng<0, vec>::get_adv();
            d0_temp_5     = a51 * filtVars_d0_5; // vd08 = ((a51 * vd08) + t9)
            filtVars_d0_5 = d0_temp_5 + temp1_add_5;
            b52_a52       = c7x::strm_eng<1, vec>::get_adv();
            d1_mul_5      = b52_a52 * output4; // vd09 = (((b52 + a52) * output04) + t8)
            filtVars_d1_5 = d1_mul_5 + temp0_mul_5;

            // Calculate stage 6
            output6       = output5 + filtVars_d0_6; // output06 = (output05 + vd10)
            b61_a61       = c7x::strm_eng<1, vec>::get_adv();
            temp1_mul_6   = b61_a61 * output5;
            temp1_add_6   = temp1_mul_6 + filtVars_d1_6; // t11 = (((b61 + a61) * output05) + vd11)
            a62           = c7x::strm_eng<0, vec>::get_adv();
            temp0_mul_6   = a62 * filtVars_d0_6; // t10 = (a62 * vd10)
            a61           = c7x::strm_eng<0, vec>::get_adv();
            d0_temp_6     = a61 * filtVars_d0_6; // vd10 = ((a61 * vd10) + t11)
            filtVars_d0_6 = d0_temp_6 + temp1_add_6;
            b62_a62       = c7x::strm_eng<1, vec>::get_adv();
            d1_mul_6      = b62_a62 * output5; // vd11 = (((b62 + a62) * output05) + t10)
            filtVars_d1_6 = d1_mul_6 + temp0_mul_6;

            // Calculate stage 7
            output7       = output6 + filtVars_d0_7; // output07 = (output06 + vd12)
            b71_a71       = c7x::strm_eng<1, vec>::get_adv();
            temp1_mul_7   = b71_a71 * output6;
            temp1_add_7   = temp1_mul_7 + filtVars_d1_7; // t13 = (((b71 + a71) * output06) + vd13)
            a72           = c7x::strm_eng<0, vec>::get_adv();
            temp0_mul_7   = a72 * filtVars_d0_7; // t12 = (a72 * vd12)
            a71           = c7x::strm_eng<0, vec>::get_adv();
            d0_temp_7     = a71 * filtVars_d0_7; // vd12 = ((a71 * vd12) + t13)
            filtVars_d0_7 = d0_temp_7 + temp1_add_7;
            b72_a72       = c7x::strm_eng<1, vec>::get_adv();
            d1_mul_7      = b72_a72 * output6; // vd13 = (((b72 + a72) * output06) + t12)
            filtVars_d1_7 = d1_mul_7 + temp0_mul_7;

            // Apply aggregate gain
            vec output = output7 * outGain;

            // Write output
            __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
            vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, output);
         }

         // First stage group reads from input, writes to output.
         // Subsequent stage groups read from output, write to output.
         pInLocal = (dataType *) pOut;

         //
         // Write filter variables
         //

         // Stage 1
         __vpred tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d0_1);
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d1_1);

         // Stage 2
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d0_2);
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d1_2);

         // Stage 3
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d0_3);
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d1_3);

         // Stage 4
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d0_4);
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d1_4);

         // Stage 5
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d0_5);
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d1_5);

         // Stage 6
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d0_6);
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d1_6);

         // Stage 7
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d0_7);
         tmp1 = c7x::strm_agen<1, vec>::get_vpred();
         VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(tmp1, VB2, filtVars_d1_7);
      }

      // Reset local src pointer for next channel group
      pInLocal = (dataType *) pIn;
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_MStage_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                      void *restrict pIn,
                                                                      void *restrict pFilterCoeff,
                                                                      void *restrict pFilterVar,
                                                                      void *restrict pOut);
