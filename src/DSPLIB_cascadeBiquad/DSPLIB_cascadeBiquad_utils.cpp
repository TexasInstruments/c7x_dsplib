// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cascadeBiquad_priv.h"

template <typename dataType> static inline dataType fast_recip(dataType x)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   vec a   = (vec) x;
   vec f   = __recip(a);
   f       = f * (2 - a * f);
   f       = f * (2 - a * f);
   f       = f * (2 - a * f);
   vec out = f * (2 - a * f);
   return out.s[0];
}

static inline c7x::float_vec recip_div(c7x::float_vec a)
{
   // typedef typename c7x::make_full_vector<float>::type vec;
   c7x::float_vec two, zero;

   two  = 2.0f;
   zero = 0.0f;

   c7x::float_vec y;
   c7x::float_vec p0, p1;
   c7x::float_vec d0, r0;

   p0 = __recip(a);
   d0 = p0 * a;
   r0 = p0;
   p1 = two - d0;
   y  = r0 * p1;

   __vpred cmp_eqzero = __cmp_eq_pred((c7x::float_vec) a, zero);
   y                  = __select(cmp_eqzero, zero, y);

   return (y);
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_get_coefficientsBufferDesc(uint32_t              dataType,
                                                              uint32_t              numStages,
                                                              uint32_t              numChannels,
                                                              uint32_t              biquadForm,
                                                              uint32_t              enableNChCoeffs,
                                                              DSPLIB_bufParams2D_t *pBufParamsFilterCoeffOpt)
{
   typedef typename c7x::make_full_vector<float>::type vec;

   uint32_t      stagesPerStageGroup, numStageGroups;
   uint32_t      numChannelsVecLen; // number of channels rounded up to vector length
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (pBufParamsFilterCoeffOpt == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else if (dataType != DSPLIB_FLOAT32) {
      status = DSPLIB_ERR_INVALID_TYPE;
   }
   else if ((biquadForm != DSPLIB_BIQUAD_TIF) && (biquadForm != DSPLIB_BIQUAD_DF1) &&
            (biquadForm != DSPLIB_BIQUAD_DF2) && (biquadForm != DSPLIB_BIQUAD_TDF2)) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }
   else if ((enableNChCoeffs != 0) && (enableNChCoeffs != 1)) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }
   else {
      /*No operation*/
   }

   if (status == DSPLIB_SUCCESS) {
      // Set data type
      pBufParamsFilterCoeffOpt->data_type = dataType;

      if (biquadForm == DSPLIB_BIQUAD_TIF) {
         // TI biquad form

         if (numStages <= DSPLIB_CASCADEBIQUAD_NUM_STAGES_7) {
            // Single stage group, all stages in single group
            stagesPerStageGroup = numStages;
            // Set number of stage groups
            numStageGroups = 1;
         }
         else {
            // Multiple stage groups
            // Each group contains 7 stages
            stagesPerStageGroup = DSPLIB_CASCADEBIQUAD_TIF_STAGES_PER_STAGE_GROUP;
            // Calculate number of stage groups
            numStageGroups = (numStages + stagesPerStageGroup - 1) / stagesPerStageGroup;
         }

         if (enableNChCoeffs == 0) {
            // Same coefficients for all channels

            // Set buffer dimX, add space for aggregate gain
            pBufParamsFilterCoeffOpt->dim_x =
                numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS + numStageGroups * DSPLIB_NUM_TIF_BIQUAD_GAINS;
            // Set buffer dimY & stride_y, effectively a 1D buffer
            pBufParamsFilterCoeffOpt->dim_y = 1;
            pBufParamsFilterCoeffOpt->stride_y =
                pBufParamsFilterCoeffOpt->dim_x * DSPLIB_sizeof(pBufParamsFilterCoeffOpt->data_type);
         }
         else { // (enableNChCoeffs == 1)
            // Different coefficients for each channel

            uint32_t eleCount = c7x::element_count_of<vec>::value;

            // Set buffer dimX
            pBufParamsFilterCoeffOpt->dim_x = numChannels;

            // Set buffer dimY, last rows are reserved for aggregate gains
            pBufParamsFilterCoeffOpt->dim_y = numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS + numStageGroups;

            // Set buffer stride_y
            numChannelsVecLen                  = (numChannels + eleCount - 1) / eleCount * eleCount;
            pBufParamsFilterCoeffOpt->stride_y = numChannelsVecLen * DSPLIB_sizeof(pBufParamsFilterCoeffOpt->data_type);
         }
      }
      else if ((biquadForm == DSPLIB_BIQUAD_DF1) || (biquadForm == DSPLIB_BIQUAD_DF2) ||
               (biquadForm == DSPLIB_BIQUAD_TDF2)) {
         // Direct Form 1

         if (enableNChCoeffs == 0) {
            // Same coefficients for all channels

            // Set buffer dimX
            pBufParamsFilterCoeffOpt->dim_x = numStages * DSPLIB_NUM_BIQUAD_COEFS;

            // Set buffer dimY & stride_y, ceffectively a 1D buffer
            pBufParamsFilterCoeffOpt->dim_y = 1;
            pBufParamsFilterCoeffOpt->stride_y =
                pBufParamsFilterCoeffOpt->dim_x * DSPLIB_sizeof(pBufParamsFilterCoeffOpt->data_type);
         }
         else { // (enableNChCoeffs == 1)
            // Different coefficients for each channel

            uint32_t eleCount = c7x::element_count_of<vec>::value;

            // Set buffer dimX
            pBufParamsFilterCoeffOpt->dim_x = numChannels;

            // Set buffer dimY, last rows are reserved for aggregate gains
            pBufParamsFilterCoeffOpt->dim_y = numStages * DSPLIB_NUM_BIQUAD_COEFS;

            // Set buffer stride_y
            numChannelsVecLen                  = (numChannels + eleCount - 1) / eleCount * eleCount;
            pBufParamsFilterCoeffOpt->stride_y = numChannelsVecLen * DSPLIB_sizeof(pBufParamsFilterCoeffOpt->data_type);
         }
      }
      else {
         status = DSPLIB_ERR_NOT_IMPLEMENTED;
      }
   }

   return status;
}

static DSPLIB_STATUS DSPLIB_cascadeBiquad_calc_tif_coefficients(uint32_t              enableNChCoeffs,
                                                                DSPLIB_bufParams2D_t *pBufParamsFilterCoeff,
                                                                DSPLIB_bufParams2D_t *pBufParamsFilterCoeffOpt,
                                                                const void *restrict pFilterCoeff,
                                                                const void *restrict pFilterCoeffOpt)
{
   uint32_t      numCoeffs, remCoeffs;
   uint32_t      numStages;
   uint32_t      stagesPerStageGroup, numStageGroups, numRemStages;
   float        *pFilterCoeffLocal, *pFilterCoeffOptLocal;
   float        *pGainOpt;
   float         outGain; // aggregate gain for stage group
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   // Get number of coefficients
   numCoeffs = pBufParamsFilterCoeff->dim_x;
   remCoeffs = numCoeffs % DSPLIB_NUM_BIQUAD_COEFS;

   if (remCoeffs != 0) {
      status = DSPLIB_ERR_NOT_ALIGNED_WIDTHS;
   }

   if (status == DSPLIB_SUCCESS) {
      // Calculate number of stages
      numStages = numCoeffs / DSPLIB_NUM_BIQUAD_COEFS;

      // Determine number of stages per group
      if (numStages <= DSPLIB_CASCADEBIQUAD_NUM_STAGES_7) {
         // Single stage group, all stages in single group
         stagesPerStageGroup = numStages;
      }
      else {
         // Multiple stage groups
         // Each group contains 7 stages
         stagesPerStageGroup = DSPLIB_CASCADEBIQUAD_TIF_STAGES_PER_STAGE_GROUP;
      }

      if (enableNChCoeffs == 0) {
         // Same coefficients for all channels

         // Calculate number of stage groups
         numStageGroups = numStages / stagesPerStageGroup;
         // Calculate final remaining stages in last stage group
         numRemStages = numStages % stagesPerStageGroup;

         pFilterCoeffLocal    = (float *) pFilterCoeff;
         pFilterCoeffOptLocal = (float *) pFilterCoeffOpt;
         pGainOpt             = (float *) pFilterCoeffOpt + numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS;
         for (uint32_t stageGroupCtr = 0; stageGroupCtr < numStageGroups; stageGroupCtr++) {

            outGain = 1.0f; // initialize aggregate gain for stage group
            for (uint32_t stageCtr = 0; stageCtr < stagesPerStageGroup; stageCtr++) {
               // Fetch next stage coefficients, b0, b1, b2, a1, a2
               // Calculate TI biquad coefficients
               float b0    = *pFilterCoeffLocal++;
               float r     = fast_recip<float>(b0);
               float b1    = *pFilterCoeffLocal++ * r;
               float b2    = *pFilterCoeffLocal++ * r;
               float a1    = -(*pFilterCoeffLocal++);
               float a2    = -(*pFilterCoeffLocal++);
               float b1_a1 = b1 + a1;
               float b2_a2 = b2 + a2;

               // Update aggregate gain
               outGain = outGain * b0;

               // Write TI biquad coefficients
               *pFilterCoeffOptLocal++ = b1_a1;
               *pFilterCoeffOptLocal++ = b2_a2;
               *pFilterCoeffOptLocal++ = a2;
               *pFilterCoeffOptLocal++ = a1;
            }

            // Write TI biquad aggregate gain
            *pGainOpt++ = outGain;
         }

         if (numRemStages > 0) {
            outGain = 1.0f; // initialize aggregate gain for stage group

            for (uint32_t stageCtr = 0; stageCtr < numRemStages; stageCtr++) {
               // Fetch next stage coefficients, b0, b1, b2, a1, a2
               // Calculate TI biquad coefficients
               float b0    = *pFilterCoeffLocal++;
               float r     = fast_recip<float>(b0);
               float b1    = *pFilterCoeffLocal++ * r;
               float b2    = *pFilterCoeffLocal++ * r;
               float a1    = -(*pFilterCoeffLocal++);
               float a2    = -(*pFilterCoeffLocal++);
               float b1_a1 = b1 + a1;
               float b2_a2 = b2 + a2;

               // Update aggregate gain
               outGain = outGain * b0;

               // Write TI biquad coefficients
               *pFilterCoeffOptLocal++ = b1_a1;
               *pFilterCoeffOptLocal++ = b2_a2;
               *pFilterCoeffOptLocal++ = a2;
               *pFilterCoeffOptLocal++ = a1;
            }

            // Write TI biquad aggregate gain
            *pGainOpt++ = outGain;
         }
      }

      else { // (enableNChCoeffs == 1)
         // Different coefficients for each channel

         typedef typename c7x::make_full_vector<float>::type vec;

         __SA_TEMPLATE_v1 sa0Params;
         __SA_TEMPLATE_v1 sa1Params;
         uint32_t         numChannels;
         uint32_t         numChannelGroups;
         float           *pGainBuffOpt;
         vec              b10, b11, b12, a11, a12;
         vec              gain;

         __SA_VECLEN SA_VECLEN = c7x::sa_veclen<vec>::value;
         uint32_t    eleCount  = c7x::element_count_of<vec>::value;

         // Get number of channels
         numChannels = pBufParamsFilterCoeff->dim_y;

         // Calculate number of full stage groups
         numStageGroups = (numStages / stagesPerStageGroup);
         // Calculate number of remaining stages in final stage group
         numRemStages = numStages % stagesPerStageGroup;

         // Calculate number of channels per channel group
         numChannelGroups = (numChannels + eleCount - 1) / eleCount;

         /**********************************************************************/
         /* Prepare SA0 template to write output coefficients                  */
         /**********************************************************************/
         sa0Params               = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0         = (numChannels > eleCount) ? eleCount : numChannels;
         sa0Params.ICNT1         = DSPLIB_NUM_TIF_BIQUAD_COEFS;
         sa0Params.DIM1          = (int32_t) eleCount * numStages * numChannelGroups;
         sa0Params.ICNT2         = numStages;
         sa0Params.DIM2          = (int32_t) eleCount * numChannelGroups;
         sa0Params.ICNT3         = numChannelGroups;
         sa0Params.DIM3          = (int32_t) eleCount;
         sa0Params.DECDIM1_WIDTH = eleCount * numChannelGroups * numStages;
         sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
         sa0Params.DIMFMT        = __SA_DIMFMT_4D;
         sa0Params.VECLEN        = SA_VECLEN;

         /**********************************************************************/
         /* Prepare SA1 template to write output gains                         */
         /**********************************************************************/
         sa1Params        = __gen_SA_TEMPLATE_v1();
         sa1Params.ICNT0  = (numChannels > eleCount) ? eleCount : numChannels;
         sa1Params.ICNT1  = (numStages + stagesPerStageGroup - 1) / stagesPerStageGroup;
         sa1Params.DIM1   = (int32_t) eleCount * numChannelGroups;
         sa1Params.ICNT2  = numChannelGroups;
         sa1Params.DIM2   = (int32_t) eleCount;
         sa1Params.DIMFMT = __SA_DIMFMT_3D;
         sa1Params.VECLEN = SA_VECLEN;

         // Initialize gain buffer pointer
         uint32_t strideYElems = pBufParamsFilterCoeffOpt->stride_y / sizeof(float); // compute stride in elements
         pGainBuffOpt          = (float *) pFilterCoeffOpt + numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS * strideYElems;

         // Coefficient buffer
         __SA0_OPEN(sa0Params);
         // Gain buffer
         __SA1_OPEN(sa1Params);

         uint32_t lastChCount = (numChannels / eleCount) * eleCount;

         // Loop over channel groups
         for (uint32_t numCh = 0; numCh < numChannels; numCh = numCh + eleCount) {

            // Calculate number of channels in current channel group
            uint32_t eleChCount = (numCh == lastChCount) ? (numChannels - numCh) : eleCount;
            // Calculate channel group coefficient buffer offset
            uint32_t offsetChGrp = numCh * numStages * DSPLIB_NUM_BIQUAD_COEFS;

            // Loop over stage groups
            for (uint32_t nS = 0; nS < numStageGroups; nS++) {
               b10  = (vec) 1.0f; // initialize b10 vector for stage group
               gain = (vec) 1.0f; // initialize gain for stage group

               // Calculate stage group coefficient buffer offset
               uint32_t offsetStageGrp = offsetChGrp + nS * stagesPerStageGroup * DSPLIB_NUM_BIQUAD_COEFS;

               // Loop over stages in stage group
               for (uint32_t coeffCount = 0; coeffCount < DSPLIB_NUM_BIQUAD_COEFS * stagesPerStageGroup;
                    coeffCount          = coeffCount + DSPLIB_NUM_BIQUAD_COEFS) {
                  // Calculate stage-in-group coeffcient buffer offset
                  uint32_t offsetStageInGrp = offsetStageGrp + coeffCount;

                  // For each channel, read coefficents for current stage into vector
                  for (uint32_t chCount = 0; chCount < eleChCount; chCount++) {
                     // Calculate offset to next channel for this stage
                     uint32_t offsetChannel = chCount * numStages * DSPLIB_NUM_BIQUAD_COEFS;
                     pFilterCoeffLocal      = (float *) pFilterCoeff + offsetStageInGrp + offsetChannel;

                     // Read coefficients for this channel and place coefficient vector
                     b10.s[chCount] = pFilterCoeffLocal[0];
                     b11.s[chCount] = pFilterCoeffLocal[1];
                     b12.s[chCount] = pFilterCoeffLocal[2];
                     a11.s[chCount] = pFilterCoeffLocal[3];
                     a12.s[chCount] = pFilterCoeffLocal[4];
                  }

                  // Update aggregate gain for stage group
                  gain = b10 * gain;

                  // Compute TI biquad coefficients
                  vec r       = recip_div(b10);
                  b11         = b11 * r;
                  b12         = b12 * r;
                  a11         = -a11;
                  a12         = -a12;
                  vec b11_a11 = b11 + a11;
                  vec b12_a12 = b12 + a12;

                  // Write coefficients to optimized coefficient buffer
                  __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
                  vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
                  __vstore_pred(tmp, VB1, b11_a11);

                  tmp = c7x::strm_agen<0, vec>::get_vpred();
                  VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
                  __vstore_pred(tmp, VB1, b12_a12);

                  tmp = c7x::strm_agen<0, vec>::get_vpred();
                  VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
                  __vstore_pred(tmp, VB1, a12);

                  tmp = c7x::strm_agen<0, vec>::get_vpred();
                  VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
                  __vstore_pred(tmp, VB1, a11);
               }

               // Write aggregate gain to gain buffer
               __vpred tmpGain   = c7x::strm_agen<1, vec>::get_vpred();
               vec    *gainStage = c7x::strm_agen<1, vec>::get_adv(pGainBuffOpt);
               __vstore_pred(tmpGain, gainStage, gain);
            }

            if (numRemStages > 0) {
               gain = (vec) 1.0f; // initialize gain for stage group

               // Calculate stage group coefficient buffer offset
               uint32_t offsetStageGrp = offsetChGrp + numStageGroups * stagesPerStageGroup * DSPLIB_NUM_BIQUAD_COEFS;

               // Loop over stages in stage group
               for (uint32_t coeffCount = 0; coeffCount < DSPLIB_NUM_BIQUAD_COEFS * numRemStages;
                    coeffCount          = coeffCount + DSPLIB_NUM_BIQUAD_COEFS) {
                  // Calculate stage-in-group coeffcient buffer offset
                  uint32_t offsetStageInGrp = offsetStageGrp + coeffCount;

                  // For each channel, read coefficents for current stage into vector
                  for (uint32_t chCount = 0; chCount < eleChCount; chCount++) {
                     // Calculate offset to next channel for this stage
                     uint32_t offsetChannel = chCount * numStages * DSPLIB_NUM_BIQUAD_COEFS;
                     pFilterCoeffLocal      = (float *) pFilterCoeff + offsetStageInGrp + offsetChannel;

                     // Read coefficients for this channel and place coefficient vector
                     b10.s[chCount] = pFilterCoeffLocal[0];
                     b11.s[chCount] = pFilterCoeffLocal[1];
                     b12.s[chCount] = pFilterCoeffLocal[2];
                     a11.s[chCount] = pFilterCoeffLocal[3];
                     a12.s[chCount] = pFilterCoeffLocal[4];
                  }

                  // Update aggregate gain for stage group
                  gain = b10 * gain;

                  // Compute TI biquad coefficients
                  vec r       = recip_div(b10);
                  b11         = b11 * r;
                  b12         = b12 * r;
                  a11         = -a11;
                  a12         = -a12;
                  vec b11_a11 = b11 + a11;
                  vec b12_a12 = b12 + a12;

                  // Write coefficients to optimized coefficient buffer
                  __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
                  vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
                  __vstore_pred(tmp, VB1, b11_a11);

                  tmp = c7x::strm_agen<0, vec>::get_vpred();
                  VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
                  __vstore_pred(tmp, VB1, b12_a12);

                  tmp = c7x::strm_agen<0, vec>::get_vpred();
                  VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
                  __vstore_pred(tmp, VB1, a12);

                  tmp = c7x::strm_agen<0, vec>::get_vpred();
                  VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
                  __vstore_pred(tmp, VB1, a11);
               }

               // Write aggregate gain to gain buffer
               __vpred tmpGain   = c7x::strm_agen<1, vec>::get_vpred();
               vec    *gainStage = c7x::strm_agen<1, vec>::get_adv(pGainBuffOpt);
               __vstore_pred(tmpGain, gainStage, gain);
            }
         }

         __SA0_CLOSE();
         __SA1_CLOSE();
      }
   }

   return status;
}

static DSPLIB_STATUS DSPLIB_cascadeBiquad_reorder_coefficients(uint32_t              stagesPerStageGroup,
                                                               DSPLIB_bufParams2D_t *pBufParamsFilterCoeff,
                                                               DSPLIB_bufParams2D_t *pBufParamsFilterCoeffOpt,
                                                               const void *restrict pFilterCoeff,
                                                               const void *restrict pFilterCoeffOpt)
{
   typedef typename c7x::make_full_vector<float>::type vec;

   __SA_TEMPLATE_v1 sa0Params;

   uint32_t      numCoeffs, remCoeffs;
   uint32_t      numStages;
   uint32_t      numStageGroups, numRemStages;
   uint32_t      numChannels;
   uint32_t      numChannelGroups;
   float        *pFilterCoeffLocal;
   vec           b0, b1, b2, a1, a2;
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   __SA_VECLEN SA_VECLEN = c7x::sa_veclen<vec>::value;
   uint32_t    eleCount  = c7x::element_count_of<vec>::value;

   // Get number of coefficients
   numCoeffs = pBufParamsFilterCoeff->dim_x;
   remCoeffs = numCoeffs % DSPLIB_NUM_BIQUAD_COEFS;

   if (remCoeffs != 0) {
      status = DSPLIB_ERR_NOT_ALIGNED_WIDTHS;
   }

   if (status == DSPLIB_SUCCESS) {
      // Calculate number of stages
      numStages = numCoeffs / DSPLIB_NUM_BIQUAD_COEFS;

      // Get number of channels
      numChannels = pBufParamsFilterCoeff->dim_y;

      // Calculate number of full stage groups
      numStageGroups = (numStages / stagesPerStageGroup);
      // Calculate number of remaining stages in final stage group
      numRemStages = numStages % stagesPerStageGroup;

      // Calculate number of channels per channel group
      numChannelGroups = (numChannels + eleCount - 1) / eleCount;

      /**********************************************************************/
      /* Prepare SA template to write output coefficients                   */
      /**********************************************************************/
      sa0Params               = __gen_SA_TEMPLATE_v1();
      sa0Params.ICNT0         = (numChannels > eleCount) ? eleCount : numChannels;
      sa0Params.ICNT1         = numStages * DSPLIB_NUM_BIQUAD_COEFS;
      sa0Params.DIM1          = (int32_t) eleCount * numChannelGroups;
      sa0Params.ICNT2         = numChannelGroups;
      sa0Params.DIM2          = (int32_t) eleCount;
      sa0Params.DECDIM1_WIDTH = numChannels;
      sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa0Params.DIMFMT        = __SA_DIMFMT_3D;
      sa0Params.VECLEN        = SA_VECLEN;

      // Coefficient buffer
      __SA0_OPEN(sa0Params);

      uint32_t lastChCount = (numChannels / eleCount) * eleCount;

      // Loop over channel groups
      for (uint32_t numCh = 0; numCh < numChannels; numCh = numCh + eleCount) {

         // Calculate number of channels in current channel group
         uint32_t eleChCount = (numCh == lastChCount) ? (numChannels - numCh) : eleCount;
         // Calculate channel group coefficient buffer offset
         uint32_t offsetChGrp = numCh * numStages * DSPLIB_NUM_BIQUAD_COEFS;

         // Loop over stage groups
         for (uint32_t nS = 0; nS < numStageGroups; nS++) {

            // Calculate stage group coefficient buffer offset
            uint32_t offsetStageGrp = offsetChGrp + nS * stagesPerStageGroup * DSPLIB_NUM_BIQUAD_COEFS;

            // Loop over stages in stage group
            for (uint32_t coeffCount = 0; coeffCount < DSPLIB_NUM_BIQUAD_COEFS * stagesPerStageGroup;
                 coeffCount          = coeffCount + DSPLIB_NUM_BIQUAD_COEFS) {
               // Calculate stage-in-group coeffcient buffer offset
               uint32_t offsetStageInGrp = offsetStageGrp + coeffCount;

               // For each channel, read coefficents for current stage into vector
               for (uint32_t chCount = 0; chCount < eleChCount; chCount++) {
                  // Calculate offset to next channel for this stage
                  uint32_t offsetChannel = chCount * numStages * DSPLIB_NUM_BIQUAD_COEFS;
                  pFilterCoeffLocal      = (float *) pFilterCoeff + offsetStageInGrp + offsetChannel;

                  // Read coefficients for this channel and place coefficient vector
                  b0.s[chCount] = pFilterCoeffLocal[0];
                  b1.s[chCount] = pFilterCoeffLocal[1];
                  b2.s[chCount] = pFilterCoeffLocal[2];
                  a1.s[chCount] = pFilterCoeffLocal[3];
                  a2.s[chCount] = pFilterCoeffLocal[4];
               }

               // Write coefficients to optimized coefficient buffer
               __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
               vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, b0);

               tmp = c7x::strm_agen<0, vec>::get_vpred();
               VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, b1);

               tmp = c7x::strm_agen<0, vec>::get_vpred();
               VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, b2);

               tmp = c7x::strm_agen<0, vec>::get_vpred();
               VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, a1);

               tmp = c7x::strm_agen<0, vec>::get_vpred();
               VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, a2);
            }
         }

         if (numRemStages > 0) {
            // Calculate stage group coefficient buffer offset
            uint32_t offsetStageGrp = offsetChGrp + numStageGroups * stagesPerStageGroup * DSPLIB_NUM_BIQUAD_COEFS;

            // Loop over stages in stage group
            for (uint32_t coeffCount = 0; coeffCount < DSPLIB_NUM_BIQUAD_COEFS * numRemStages;
                 coeffCount          = coeffCount + DSPLIB_NUM_BIQUAD_COEFS) {
               // Calculate stage-in-group coeffcient buffer offset
               uint32_t offsetStageInGrp = offsetStageGrp + coeffCount;

               // For each channel, read coefficents for current stage into vector
               for (uint32_t chCount = 0; chCount < eleChCount; chCount++) {
                  // Calculate offset to next channel for this stage
                  uint32_t offsetChannel = chCount * numStages * DSPLIB_NUM_BIQUAD_COEFS;
                  pFilterCoeffLocal      = (float *) pFilterCoeff + offsetStageInGrp + offsetChannel;

                  // Read coefficients for this channel and place coefficient vector
                  b0.s[chCount] = pFilterCoeffLocal[0];
                  b1.s[chCount] = pFilterCoeffLocal[1];
                  b2.s[chCount] = pFilterCoeffLocal[2];
                  a1.s[chCount] = pFilterCoeffLocal[3];
                  a2.s[chCount] = pFilterCoeffLocal[4];
               }

               // Write coefficients to optimized coefficient buffer
               __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
               vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, b0);

               tmp = c7x::strm_agen<0, vec>::get_vpred();
               VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, b1);

               tmp = c7x::strm_agen<0, vec>::get_vpred();
               VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, b2);

               tmp = c7x::strm_agen<0, vec>::get_vpred();
               VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, a1);

               tmp = c7x::strm_agen<0, vec>::get_vpred();
               VB1 = c7x::strm_agen<0, vec>::get_adv(pFilterCoeffOpt);
               __vstore_pred(tmp, VB1, a2);
            }
         }
      }

      __SA0_CLOSE();
   }

   return status;
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_get_coefficients(uint32_t              biquadForm,
                                                    uint32_t              enableNChCoeffs,
                                                    DSPLIB_bufParams2D_t *pBufParamsFilterCoeff,
                                                    DSPLIB_bufParams2D_t *pBufParamsFilterCoeffOpt,
                                                    const void *restrict pFilterCoeff,
                                                    const void *restrict pFilterCoeffOpt)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if ((pBufParamsFilterCoeff == NULL) || (pBufParamsFilterCoeffOpt == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else if ((pBufParamsFilterCoeff->data_type != DSPLIB_FLOAT32) ||
            (pBufParamsFilterCoeffOpt->data_type != DSPLIB_FLOAT32)) {
      status = DSPLIB_ERR_INVALID_TYPE;
   }
   else if ((biquadForm != DSPLIB_BIQUAD_TIF) && (biquadForm != DSPLIB_BIQUAD_DF1) &&
            (biquadForm != DSPLIB_BIQUAD_DF2) && (biquadForm != DSPLIB_BIQUAD_TDF2)) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }
   else if ((enableNChCoeffs != 0) && (enableNChCoeffs != 1)) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }
   else {
      /*No operation*/
   }

   if (status == DSPLIB_SUCCESS) {
      switch (biquadForm) {
      case DSPLIB_BIQUAD_TIF:
         // TI biquad form
         status = DSPLIB_cascadeBiquad_calc_tif_coefficients(enableNChCoeffs, pBufParamsFilterCoeff,
                                                             pBufParamsFilterCoeffOpt, pFilterCoeff, pFilterCoeffOpt);
         break;

      case DSPLIB_BIQUAD_DF1:
      case DSPLIB_BIQUAD_DF2:
      case DSPLIB_BIQUAD_TDF2:
         // DF1 / DF2 / TDF2
         if (enableNChCoeffs == 0) {
            // No coefficient reordering required for all channels using same coefficients
            status = DSPLIB_ERR_INVALID_VALUE;
         }
         else { // (enableNChCoeffs == 1)
            status = DSPLIB_cascadeBiquad_reorder_coefficients(DSPLIB_CASCADEBIQUAD_DF1_STAGES_PER_STAGE_GROUP,
                                                               pBufParamsFilterCoeff, pBufParamsFilterCoeffOpt,
                                                               pFilterCoeff, pFilterCoeffOpt);
         }
         break;

      default:
         status = DSPLIB_ERR_NOT_IMPLEMENTED;
         break;
      }
   }

   return status;
}
