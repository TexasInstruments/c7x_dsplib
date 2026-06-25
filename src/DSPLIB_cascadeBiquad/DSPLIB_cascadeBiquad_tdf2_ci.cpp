// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_cascadeBiquad_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tdf2_init_ci(DSPLIB_kernelHandle                  handle,
                                                const DSPLIB_bufParams2D_t          *bufParamsIn,
                                                const DSPLIB_bufParams2D_t          *bufParamsFilterCoeff,
                                                const DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                                const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_TEMPLATE_v1                                       se0Params;
   __SE_TEMPLATE_v1                                       se1Params;
   __SA_TEMPLATE_v1                                       sa0Params;
   __SA_TEMPLATE_v1                                       sa1Params;
   __SA_TEMPLATE_v1                                       sa2Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs       = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock             = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize           = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numChannels        = pKerPrivArgs->initArgs.numChannels;
   uint32_t                       numStages          = pKerPrivArgs->initArgs.numStages;
   uint32_t                       dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t                       dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t                       filterVarPitch     = pKerPrivArgs->filterVarPitch;
   uint32_t                       enableNChCoeffs    = pKerPrivArgs->initArgs.enableNChCoeffs;

   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   // Compute number of channel groups
   uint32_t numChannelGroups = (numChannels + eleCount - 1) / eleCount;

   // Calculate number of stage groups
   uint32_t numStageGroups = numStages / DSPLIB_CASCADEBIQUAD_TDF2_STAGES_PER_STAGE_GROUP;
   // Calculate remaining stages
   uint32_t remStageCnt = numStages % DSPLIB_CASCADEBIQUAD_TDF2_STAGES_PER_STAGE_GROUP;
   uint32_t stage2Cnt   = remStageCnt >> 1; // whether two remaining stages
   uint32_t stage1Cnt   = remStageCnt & 1u; // whether one remaining stage

   // Store number of stages groups and number of remaining stages
   pKerPrivArgs->numStageGroups = numStageGroups;
   pKerPrivArgs->stage2Cnt      = stage2Cnt;
   pKerPrivArgs->stage1Cnt      = stage1Cnt;

   // Compute number of adjacent samples to read/write
   uint32_t icnt0Param = (numChannels > eleCount) ? eleCount : numChannels;

   /**********************************************************************/
   /* Prepare SE0 template to read the coefficients                      */
   /**********************************************************************/
   se0Params = __gen_SE_TEMPLATE_v1();
   if (enableNChCoeffs == 0) {
      se0Params.ICNT0   = 1;
      se0Params.ICNT1   = numStages * DSPLIB_NUM_BIQUAD_COEFS;
      se0Params.DIM1    = 1;
      se0Params.ICNT2   = numChannelGroups;
      se0Params.DIM2    = 0;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.ELEDUP  = c7x::se_eledup<dataType>::value;
      se0Params.DIMFMT  = __SE_DIMFMT_3D;
   }
   else {
      se0Params               = __gen_SE_TEMPLATE_v1();
      se0Params.ICNT0         = icnt0Param;
      se0Params.ICNT1         = numStages * DSPLIB_NUM_BIQUAD_COEFS;
      se0Params.DIM1          = (int32_t) numChannelGroups * eleCount;
      se0Params.ICNT2         = numChannelGroups;
      se0Params.DIM2          = (int32_t) eleCount;
      se0Params.ELETYPE       = SE_ELETYPE;
      se0Params.VECLEN        = SE_VECLEN;
      se0Params.DECDIM1_WIDTH = numChannels;
      se0Params.DECDIM1       = __SE_DECDIM_DIM2;
      se0Params.DIMFMT        = __SE_DIMFMT_3D;
   }

   /**********************************************************************/
   /* Prepare SE1 template to read the input                             */
   /**********************************************************************/
   se1Params         = __gen_SE_TEMPLATE_v1();
   se1Params.ICNT0   = 0; /* updated in process function */
   se1Params.ICNT1   = dataSize;
   se1Params.DIM1    = dataBufferInPitch;
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN  = SE_VECLEN;
   se1Params.DIMFMT  = __SE_DIMFMT_2D;

   /**********************************************************************/
   /* Prepare SA0 template to write the output                           */
   /**********************************************************************/
   sa0Params               = __gen_SA_TEMPLATE_v1();
   sa0Params.ICNT0         = icnt0Param;
   sa0Params.ICNT1         = dataSize;
   sa0Params.DIM1          = (int32_t) dataBufferOutPitch;
   sa0Params.ICNT2         = numStageGroups + stage2Cnt + stage1Cnt;
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

   /**********************************************************************/
   /* Prepare SA2 template to read the filter variables                 */
   /**********************************************************************/
   sa2Params               = __gen_SA_TEMPLATE_v1();
   sa2Params.ICNT0         = icnt0Param;
   sa2Params.ICNT1         = numStages * DSPLIB_NUM_STATE_VARS_DF2;
   sa2Params.DIM1          = (int32_t) filterVarPitch;
   sa2Params.ICNT2         = numChannelGroups;
   sa2Params.DIM2          = (int32_t) eleCount;
   sa2Params.DECDIM1_WIDTH = numChannels;
   sa2Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa2Params.VECLEN        = SA_VECLEN;
   sa2Params.DIMFMT        = __SA_DIMFMT_3D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET) = sa2Params;

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tdf2_init_ci<float>(DSPLIB_kernelHandle         handle,
                                                                const DSPLIB_bufParams2D_t *bufParamsIn,
                                                                const DSPLIB_bufParams2D_t *bufParamsFilterCoeff,
                                                                const DSPLIB_bufParams2D_t *bufParamsFilterVar,
                                                                const DSPLIB_bufParams2D_t *bufParamsOut,
                                                                const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tdf2_exec_ci(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pFilterCoeff,
                                                void *restrict pFilterVar,
                                                void *restrict pOut)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *pFilterCoeffLocal  = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal    = (dataType *) pFilterVar;
   dataType *restrict pOutLocal = (dataType *) pOut;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   // Filter coefficients read
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pFilterCoeffLocal, se0Params);

   // Output samples write
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   // Filter variables read
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET);
   __SA2_OPEN(sa2Params);

   // cascade input
   vec input;

   // stage outputs
   vec output_1, output_2, output_3;

   // coefficients
   vec b0_1, b1_1, b2_1, a1_1, a2_1; // stage 1
   vec b0_2, b1_2, b2_2, a1_2, a2_2; // stage 2
   vec b0_3, b1_3, b2_3, a1_3, a2_3; // stage 3

   // Input samples read
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   // Load number of stage groups and number of remaining stages
   uint32_t numStageGroups = pKerPrivArgs->numStageGroups;
   uint32_t stage2Cnt      = pKerPrivArgs->stage2Cnt;
   uint32_t stage1Cnt      = pKerPrivArgs->stage1Cnt;

   // Initialize number of channels to process
   uint32_t chProcCnt = numChannels;

// Channel group loop
#pragma MUST_ITERATE(1, , )
   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      /* Update SE1 ICNT0 for current channel group */
      se1Params.ICNT0 = (chProcCnt >= eleCount) ? eleCount : chProcCnt;
      __SE1_OPEN(pInLocal + chCount, se1Params);

      // Stage group loop
      for (uint32_t stageGroupCnt = 0; stageGroupCnt < numStageGroups; stageGroupCnt++) {
         //
         // Read filter coefficients
         //
         // Stage 1
         b0_1 = c7x::strm_eng<0, vec>::get_adv();
         b1_1 = c7x::strm_eng<0, vec>::get_adv();
         b2_1 = c7x::strm_eng<0, vec>::get_adv();
         a1_1 = c7x::strm_eng<0, vec>::get_adv();
         a2_1 = c7x::strm_eng<0, vec>::get_adv();

         // Stage 2
         b0_2 = c7x::strm_eng<0, vec>::get_adv();
         b1_2 = c7x::strm_eng<0, vec>::get_adv();
         b2_2 = c7x::strm_eng<0, vec>::get_adv();
         a1_2 = c7x::strm_eng<0, vec>::get_adv();
         a2_2 = c7x::strm_eng<0, vec>::get_adv();

         // Stage 3
         b0_3 = c7x::strm_eng<0, vec>::get_adv();
         b1_3 = c7x::strm_eng<0, vec>::get_adv();
         b2_3 = c7x::strm_eng<0, vec>::get_adv();
         a1_3 = c7x::strm_eng<0, vec>::get_adv();
         a2_3 = c7x::strm_eng<0, vec>::get_adv();

         //
         // Read filter variables
         //
         // Stage 1
         __vpred filterVarsRd    = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         vec    *filterVarsRdPtr = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec     wd1_1           = __vload_pred(filterVarsRd, filterVarsRdPtr);
         filterVarsRd            = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         filterVarsRdPtr         = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec wd2_1               = __vload_pred(filterVarsRd, filterVarsRdPtr);

         // Stage 2
         filterVarsRd    = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         filterVarsRdPtr = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec wd1_2       = __vload_pred(filterVarsRd, filterVarsRdPtr);
         filterVarsRd    = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         filterVarsRdPtr = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec wd2_2       = __vload_pred(filterVarsRd, filterVarsRdPtr);

         // Stage 2
         filterVarsRd    = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         filterVarsRdPtr = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec wd1_3       = __vload_pred(filterVarsRd, filterVarsRdPtr);
         filterVarsRd    = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         filterVarsRdPtr = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec wd2_3       = __vload_pred(filterVarsRd, filterVarsRdPtr);

// Sample loop
#pragma MUST_ITERATE(1, , )
         for (uint32_t samp = 0; samp < dataSize; samp++) {
            // Read input
            input = c7x::strm_eng<1, vec>::get_adv();

            // Calculate stage 1
            output_1 = wd1_1 + input * b0_1;
            wd1_1    = wd2_1 + input * b1_1 - a1_1 * output_1;
            wd2_1    = input * b2_1 - a2_1 * output_1;

            // Calculate stage 2
            output_2 = wd1_2 + output_1 * b0_2;
            wd1_2    = wd2_2 + output_1 * b1_2 - a1_2 * output_2;
            wd2_2    = output_1 * b2_2 - a2_2 * output_2;

            // Calculate stage 3
            output_3 = wd1_3 + output_2 * b0_3;
            wd1_3    = wd2_3 + output_2 * b1_3 - a1_3 * output_3;
            wd2_3    = output_2 * b2_3 - a2_3 * output_3;

            // Write output
            __vpred outWr    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *outWrPtr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(outWr, outWrPtr, output_3);
         }

         /* First stage group reads from input, writes to output.
         Subsequent stage groups read from output, write to output. */
         pInLocal = (dataType *) pOut + chCount;
         __SE1_RESET(pInLocal);

         //
         // Write filter variables
         //
         // Stage 1
         __vpred filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         vec    *filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd1_1);
         filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd2_1);

         // Stage 2
         filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd1_2);
         filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd2_2);

         // Stage 3
         filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd1_3);
         filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd2_3);
      }

      if (stage2Cnt > 0) {
         //
         // Read filter coefficients
         //
         // Stage 1
         b0_1 = c7x::strm_eng<0, vec>::get_adv();
         b1_1 = c7x::strm_eng<0, vec>::get_adv();
         b2_1 = c7x::strm_eng<0, vec>::get_adv();
         a1_1 = c7x::strm_eng<0, vec>::get_adv();
         a2_1 = c7x::strm_eng<0, vec>::get_adv();

         // Stage 2
         b0_2 = c7x::strm_eng<0, vec>::get_adv();
         b1_2 = c7x::strm_eng<0, vec>::get_adv();
         b2_2 = c7x::strm_eng<0, vec>::get_adv();
         a1_2 = c7x::strm_eng<0, vec>::get_adv();
         a2_2 = c7x::strm_eng<0, vec>::get_adv();

         //
         // Read filter variables
         //
         // Stage 1
         __vpred filterVarsRd    = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         vec    *filterVarsRdPtr = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec     wd1_1           = __vload_pred(filterVarsRd, filterVarsRdPtr);
         filterVarsRd            = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         filterVarsRdPtr         = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec wd2_1               = __vload_pred(filterVarsRd, filterVarsRdPtr);

         // Stage 2
         filterVarsRd    = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         filterVarsRdPtr = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec wd1_2       = __vload_pred(filterVarsRd, filterVarsRdPtr);
         filterVarsRd    = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         filterVarsRdPtr = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec wd2_2       = __vload_pred(filterVarsRd, filterVarsRdPtr);

// Sample loop
#pragma MUST_ITERATE(1, , )
         for (uint32_t samp = 0; samp < dataSize; samp++) {
            // Read input
            input = c7x::strm_eng<1, vec>::get_adv();

            // Calculate stage 1
            output_1 = wd1_1 + input * b0_1;
            wd1_1    = wd2_1 + input * b1_1 - a1_1 * output_1;
            wd2_1    = input * b2_1 - a2_1 * output_1;

            // Calculate stage 2
            output_2 = wd1_2 + output_1 * b0_2;
            wd1_2    = wd2_2 + output_1 * b1_2 - a1_2 * output_2;
            wd2_2    = output_1 * b2_2 - a2_2 * output_2;

            // Write output
            __vpred outWr    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *outWrPtr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(outWr, outWrPtr, output_2);
         }

         //
         // Write filter variables
         //
         // Stage 1
         __vpred filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         vec    *filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd1_1);
         filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd2_1);

         // Stage 2
         filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd1_2);
         filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd2_2);
      }

      if (stage1Cnt > 0) {
         //
         // Process last remaining stage
         //

         // Read filter coefficients
         b0_1 = c7x::strm_eng<0, vec>::get_adv();
         b1_1 = c7x::strm_eng<0, vec>::get_adv();
         b2_1 = c7x::strm_eng<0, vec>::get_adv();
         a1_1 = c7x::strm_eng<0, vec>::get_adv();
         a2_1 = c7x::strm_eng<0, vec>::get_adv();

         // Read filter variables
         __vpred filterVarsRd    = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         vec    *filterVarsRdPtr = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec     wd1_1           = __vload_pred(filterVarsRd, filterVarsRdPtr);
         filterVarsRd            = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         filterVarsRdPtr         = c7x::strm_agen<2, c7x::float_vec>::get_adv(pFilterVarLocal);
         vec wd2_1               = __vload_pred(filterVarsRd, filterVarsRdPtr);

// Sample loop
#pragma MUST_ITERATE(1, , )
         for (uint32_t samp = 0; samp < dataSize; samp++) {
            // Read input
            input = c7x::strm_eng<1, vec>::get_adv();

            // Calculate stage 1
            output_1 = wd1_1 + input * b0_1;
            wd1_1    = wd2_1 + input * b1_1 - a1_1 * output_1;
            wd2_1    = input * b2_1 - a2_1 * output_1;

            // Write output
            __vpred outWr    = c7x::strm_agen<0, vec>::get_vpred();
            vec    *outWrPtr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(outWr, outWrPtr, output_1);
         }

         // Write filter variables
         __vpred filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         vec    *filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd1_1);
         filterVarsWr    = c7x::strm_agen<1, vec>::get_vpred();
         filterVarsWrPtr = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
         __vstore_pred(filterVarsWr, filterVarsWrPtr, wd2_1);
      }

      // Reset input buffer pointer for next channel group
      pInLocal = (dataType *) pIn;
      __SE1_CLOSE();

      // Update number of channels to process
      chProcCnt -= eleCount;
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tdf2_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                void *restrict pIn,
                                                                void *restrict pFilterCoeff,
                                                                void *restrict pFilterVar,
                                                                void *restrict pOut);
