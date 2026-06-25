// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cascadeBiquad_priv.h"

DSPLIB_STATUS DSPLIB_cascadeBiquad_init_cn(DSPLIB_kernelHandle            handle,
                                           DSPLIB_bufParams2D_t          *bufParamsIn,
                                           DSPLIB_bufParams2D_t          *bufParamsFilterCoeff,
                                           DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                           DSPLIB_bufParams2D_t          *bufParamsOut,
                                           DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_df2_exec_cn(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pFilterCoeff,
                                               void *restrict pFilterVar,
                                               void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   dataType                      *pInLocal;
   dataType                      *pFilterCoeffLocal;
   dataType                      *pFilterVarLocal;
   dataType                      *pOutLocal;
   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;

   uint32_t dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t filterVarPitch     = pKerPrivArgs->filterVarPitch;
   uint32_t enableNChCoeffs    = pKerPrivArgs->initArgs.enableNChCoeffs;

   uint32_t dataSize    = pKerPrivArgs->initArgs.dataSize;
   uint32_t numStages   = pKerPrivArgs->initArgs.numStages;
   uint32_t numChannels = pKerPrivArgs->initArgs.numChannels;

   dataType b0, b1, b2, a1, a2; // biquad coefficients
   dataType wd0, wd1, wd2;      // biquad state variables
   dataType input, output;      // biquad input, output

   for (uint32_t chCnt = 0; chCnt < numChannels; chCnt++) {
      // Initialize pointers
      pInLocal        = (dataType *) pIn + chCnt;
      pOutLocal       = (dataType *) pOut + chCnt;
      pFilterVarLocal = (dataType *) pFilterVar + chCnt;
      if (enableNChCoeffs == 0) {
         // Same coefficients for all channels
         pFilterCoeffLocal = (dataType *) pFilterCoeff;
      }
      else {
         // Different coefficients for each channel
         pFilterCoeffLocal = (dataType *) pFilterCoeff + numStages * DSPLIB_NUM_BIQUAD_COEFS * chCnt;
      }

      // For first stage:
      //    - reading input from input buffer and writing to output buffer
      //    - input reads use dataBufferInPitch, output writes use dataBufferOutPitch.
      //
      // For other stages:
      //    - reading input from output buffer and writing to output buffer
      //    - input reads and output writes use dataBufferOutPitch.

      // Read stage filter coefficients
      b0 = pFilterCoeffLocal[0];
      b1 = pFilterCoeffLocal[1];
      b2 = pFilterCoeffLocal[2];
      a1 = pFilterCoeffLocal[3];
      a2 = pFilterCoeffLocal[4];

      // Read stage filter variables
      wd1 = pFilterVarLocal[0];
      wd2 = pFilterVarLocal[filterVarPitch];

      for (uint32_t sampChCnt = 0; sampChCnt < dataSize; sampChCnt++) {
         input    = *(pInLocal);
         pInLocal = (dataType *) pInLocal + dataBufferInPitch;

         // Calculate stage output, Direct Form 2
         wd0    = input - a1 * wd1 - a2 * wd2;
         output = b0 * wd0 + b1 * wd1 + b2 * wd2;
         wd2    = wd1;
         wd1    = wd0;

         // Output samples per channel
         *(dataType *) pOutLocal = output;
         pOutLocal               = (dataType *) pOutLocal + dataBufferOutPitch;
      }

      // Write stage filter variables
      pFilterVarLocal[0]              = wd1;
      pFilterVarLocal[filterVarPitch] = wd2;

      // Set input pointer for next stage
      // Output buffer is input buffer after first stage
      pInLocal = (dataType *) pOut + chCnt;

      // Reset output pointer for next stage code
      pOutLocal = (dataType *) pOut + chCnt;

      // Update pointers
      pFilterCoeffLocal += DSPLIB_NUM_BIQUAD_COEFS;
      pFilterVarLocal += DSPLIB_NUM_STATE_VARS_DF2 * filterVarPitch;

      for (uint32_t stageCnt = 1; stageCnt < numStages; stageCnt++) {
         // Read stage filter coefficients
         b0 = pFilterCoeffLocal[0];
         b1 = pFilterCoeffLocal[1];
         b2 = pFilterCoeffLocal[2];
         a1 = pFilterCoeffLocal[3];
         a2 = pFilterCoeffLocal[4];

         // Read stage filter variables
         wd1 = pFilterVarLocal[0];
         wd2 = pFilterVarLocal[filterVarPitch];

         for (uint32_t sampStageCnt = 0; sampStageCnt < dataSize; sampStageCnt++) {
            input    = *(pInLocal);
            pInLocal = (dataType *) pInLocal + dataBufferOutPitch;

            // Calculate stage output, Direct Form 2
            wd0    = input - a1 * wd1 - a2 * wd2;
            output = b0 * wd0 + b1 * wd1 + b2 * wd2;
            wd2    = wd1;
            wd1    = wd0;

            // Output samples per channel
            *(dataType *) pOutLocal = output;
            pOutLocal               = (dataType *) pOutLocal + dataBufferOutPitch;
         }

         // Write stage filter variables
         pFilterVarLocal[0]              = wd1;
         pFilterVarLocal[filterVarPitch] = wd2;

         // Reset output pointer for next stage
         // Output buffer is input buffer after first stage
         pInLocal = (dataType *) pOut + chCnt;
         // Reset output pointer for next stage
         pOutLocal = (dataType *) pOut + chCnt;

         // Update pointers
         pFilterCoeffLocal += DSPLIB_NUM_BIQUAD_COEFS;
         pFilterVarLocal += DSPLIB_NUM_STATE_VARS_DF2 * filterVarPitch;
      }
   }

   return status;
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_cascadeBiquad_df2_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                               void *restrict pIn,
                                                               void *restrict pFilterCoeff,
                                                               void *restrict pFilterVar,
                                                               void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_df1_exec_cn(DSPLIB_kernelHandle handle,
                                               void *restrict pIn,
                                               void *restrict pFilterCoeff,
                                               void *restrict pFilterVar,
                                               void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   dataType                      *pInLocal;
   dataType                      *pFilterCoeffLocal;
   dataType                      *pFilterVarLocal;
   dataType                      *pOutLocal;
   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;

   uint32_t dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t filterVarPitch     = pKerPrivArgs->filterVarPitch;
   uint32_t enableNChCoeffs    = pKerPrivArgs->initArgs.enableNChCoeffs;

   uint32_t dataSize    = pKerPrivArgs->initArgs.dataSize;
   uint32_t numStages   = pKerPrivArgs->initArgs.numStages;
   uint32_t numChannels = pKerPrivArgs->initArgs.numChannels;

   dataType b0, b1, b2, a1, a2; // biquad coefficients
   dataType xd1, xd2, yd1, yd2; // biquad state variables
   dataType input, output;      // biquad input, output

   for (uint32_t chCnt = 0; chCnt < numChannels; chCnt++) {
      // Initialize pointers
      pInLocal        = (dataType *) pIn + chCnt;
      pOutLocal       = (dataType *) pOut + chCnt;
      pFilterVarLocal = (dataType *) pFilterVar + chCnt;
      if (enableNChCoeffs == 0) {
         // Same coefficients for all channels
         pFilterCoeffLocal = (dataType *) pFilterCoeff;
      }
      else {
         // Different coefficients for each channel
         pFilterCoeffLocal = (dataType *) pFilterCoeff + numStages * DSPLIB_NUM_BIQUAD_COEFS * chCnt;
      }

      // For first stage:
      //    - reading input from input buffer and writing to output buffer
      //    - input reads use dataBufferInPitch, output writes use dataBufferOutPitch.
      //
      // For other stages:
      //    - reading input from output buffer and writing to output buffer
      //    - input reads and output writes use dataBufferOutPitch.

      // Read stage filter coefficients
      b0 = pFilterCoeffLocal[0];
      b1 = pFilterCoeffLocal[1];
      b2 = pFilterCoeffLocal[2];
      a1 = pFilterCoeffLocal[3];
      a2 = pFilterCoeffLocal[4];

      // Read stage filter variables
      xd1 = pFilterVarLocal[0];
      xd2 = pFilterVarLocal[filterVarPitch];
      yd1 = pFilterVarLocal[2 * filterVarPitch];
      yd2 = pFilterVarLocal[3 * filterVarPitch];

      for (uint32_t sampChCnt = 0; sampChCnt < dataSize; sampChCnt++) {
         input    = *(pInLocal);
         pInLocal = (dataType *) pInLocal + dataBufferInPitch;

         // Calculate stage output, Direct Form 1
         output = b0 * input + b1 * xd1 + b2 * xd2 - a1 * yd1 - a2 * yd2;

         // Update state variables
         xd2 = xd1;
         xd1 = input;
         yd2 = yd1;
         yd1 = output;

         // Output samples per channel
         *(dataType *) pOutLocal = output;
         pOutLocal               = (dataType *) pOutLocal + dataBufferOutPitch;
      }

      // Write stage filter variables
      pFilterVarLocal[0]                  = xd1;
      pFilterVarLocal[filterVarPitch]     = xd2;
      pFilterVarLocal[2 * filterVarPitch] = yd1;
      pFilterVarLocal[3 * filterVarPitch] = yd2;

      // Set input pointer for next stage
      // Output buffer is input buffer after first stage
      pInLocal = (dataType *) pOut + chCnt;

      // Reset output pointer for next stage code
      pOutLocal = (dataType *) pOut + chCnt;

      // Update pointers
      pFilterCoeffLocal += DSPLIB_NUM_BIQUAD_COEFS;
      pFilterVarLocal += DSPLIB_NUM_STATE_VARS_DF1 * filterVarPitch;

      for (uint32_t stageCnt = 1; stageCnt < numStages; stageCnt++) {
         // Read stage filter coefficients
         b0 = pFilterCoeffLocal[0];
         b1 = pFilterCoeffLocal[1];
         b2 = pFilterCoeffLocal[2];
         a1 = pFilterCoeffLocal[3];
         a2 = pFilterCoeffLocal[4];

         // Read stage filter variables
         xd1 = pFilterVarLocal[0];
         xd2 = pFilterVarLocal[filterVarPitch];
         yd1 = pFilterVarLocal[2 * filterVarPitch];
         yd2 = pFilterVarLocal[3 * filterVarPitch];

         for (uint32_t sampStageCnt = 0; sampStageCnt < dataSize; sampStageCnt++) {
            input    = *(pInLocal);
            pInLocal = (dataType *) pInLocal + dataBufferOutPitch;

            // Calculate stage output, Direct Form 1
            output = b0 * input + b1 * xd1 + b2 * xd2 - a1 * yd1 - a2 * yd2;

            // Update state variables
            xd2 = xd1;
            xd1 = input;
            yd2 = yd1;
            yd1 = output;

            // Output samples per channel
            *(dataType *) pOutLocal = output;
            pOutLocal               = (dataType *) pOutLocal + dataBufferOutPitch;
         }

         // Write stage filter variables
         pFilterVarLocal[0]                  = xd1;
         pFilterVarLocal[filterVarPitch]     = xd2;
         pFilterVarLocal[2 * filterVarPitch] = yd1;
         pFilterVarLocal[3 * filterVarPitch] = yd2;

         // Reset output pointer for next stage
         // Output buffer is input buffer after first stage
         pInLocal = (dataType *) pOut + chCnt;
         // Reset output pointer for next stage
         pOutLocal = (dataType *) pOut + chCnt;

         // Update pointers
         pFilterCoeffLocal += DSPLIB_NUM_BIQUAD_COEFS;
         pFilterVarLocal += DSPLIB_NUM_STATE_VARS_DF1 * filterVarPitch;
      }
   }

   return status;
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_cascadeBiquad_df1_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                               void *restrict pIn,
                                                               void *restrict pFilterCoeff,
                                                               void *restrict pFilterVar,
                                                               void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tdf2_exec_cn(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pFilterCoeff,
                                                void *restrict pFilterVar,
                                                void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   dataType                      *pInLocal;
   dataType                      *pFilterCoeffLocal;
   dataType                      *pFilterVarLocal;
   dataType                      *pOutLocal;
   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;

   uint32_t dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t filterVarPitch     = pKerPrivArgs->filterVarPitch;
   uint32_t enableNChCoeffs    = pKerPrivArgs->initArgs.enableNChCoeffs;

   uint32_t dataSize    = pKerPrivArgs->initArgs.dataSize;
   uint32_t numStages   = pKerPrivArgs->initArgs.numStages;
   uint32_t numChannels = pKerPrivArgs->initArgs.numChannels;

   dataType b0, b1, b2, a1, a2; // biquad coefficients
   dataType wd1, wd2;           // biquad state variables
   dataType input, output;      // biquad input, output

   for (uint32_t chCnt = 0; chCnt < numChannels; chCnt++) {
      // Initialize pointers
      pInLocal        = (dataType *) pIn + chCnt;
      pOutLocal       = (dataType *) pOut + chCnt;
      pFilterVarLocal = (dataType *) pFilterVar + chCnt;
      if (enableNChCoeffs == 0) {
         // Same coefficients for all channels
         pFilterCoeffLocal = (dataType *) pFilterCoeff;
      }
      else {
         // Different coefficients for each channel
         pFilterCoeffLocal = (dataType *) pFilterCoeff + numStages * DSPLIB_NUM_BIQUAD_COEFS * chCnt;
      }

      // For first stage:
      //    - reading input from input buffer and writing to output buffer
      //    - input reads use dataBufferInPitch, output writes use dataBufferOutPitch.
      //
      // For other stages:
      //    - reading input from output buffer and writing to output buffer
      //    - input reads and output writes use dataBufferOutPitch.

      // Read stage filter coefficients
      b0 = pFilterCoeffLocal[0];
      b1 = pFilterCoeffLocal[1];
      b2 = pFilterCoeffLocal[2];
      a1 = pFilterCoeffLocal[3];
      a2 = pFilterCoeffLocal[4];

      // Read stage filter variables
      wd1 = pFilterVarLocal[0];
      wd2 = pFilterVarLocal[filterVarPitch];

      for (uint32_t sampChCnt = 0; sampChCnt < dataSize; sampChCnt++) {
         input    = *(pInLocal);
         pInLocal = (dataType *) pInLocal + dataBufferInPitch;

         // Calculate stage output, Transposed Direct Form 2
         output = wd1 + input * b0;
         wd1    = wd2 + input * b1 - a1 * output;
         wd2    = input * b2 - a2 * output;

         // Output samples per channel
         *(dataType *) pOutLocal = output;
         pOutLocal               = (dataType *) pOutLocal + dataBufferOutPitch;
      }

      // Write stage filter variables
      pFilterVarLocal[0]              = wd1;
      pFilterVarLocal[filterVarPitch] = wd2;

      // Set input pointer for next stage
      // Output buffer is input buffer after first stage
      pInLocal = (dataType *) pOut + chCnt;

      // Reset output pointer for next stage code
      pOutLocal = (dataType *) pOut + chCnt;

      // Update pointers
      pFilterCoeffLocal += DSPLIB_NUM_BIQUAD_COEFS;
      pFilterVarLocal += DSPLIB_NUM_STATE_VARS_DF2 * filterVarPitch;

      for (uint32_t stageCnt = 1; stageCnt < numStages; stageCnt++) {
         // Read stage filter coefficients
         b0 = pFilterCoeffLocal[0];
         b1 = pFilterCoeffLocal[1];
         b2 = pFilterCoeffLocal[2];
         a1 = pFilterCoeffLocal[3];
         a2 = pFilterCoeffLocal[4];

         // Read stage filter variables
         wd1 = pFilterVarLocal[0];
         wd2 = pFilterVarLocal[filterVarPitch];

         for (uint32_t sampStageCnt = 0; sampStageCnt < dataSize; sampStageCnt++) {
            input    = *(pInLocal);
            pInLocal = (dataType *) pInLocal + dataBufferOutPitch;

            // Calculate stage output, Transposed Direct Form 2
            output = wd1 + input * b0;
            wd1    = wd2 + input * b1 - a1 * output;
            wd2    = input * b2 - a2 * output;

            // Output samples per channel
            *(dataType *) pOutLocal = output;
            pOutLocal               = (dataType *) pOutLocal + dataBufferOutPitch;
         }

         // Write stage filter variables
         pFilterVarLocal[0]              = wd1;
         pFilterVarLocal[filterVarPitch] = wd2;

         // Reset output pointer for next stage
         // Output buffer is input buffer after first stage
         pInLocal = (dataType *) pOut + chCnt;
         // Reset output pointer for next stage
         pOutLocal = (dataType *) pOut + chCnt;

         // Update pointers
         pFilterCoeffLocal += DSPLIB_NUM_BIQUAD_COEFS;
         pFilterVarLocal += DSPLIB_NUM_STATE_VARS_DF2 * filterVarPitch;
      }
   }

   return status;
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_cascadeBiquad_tdf2_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                                void *restrict pIn,
                                                                void *restrict pFilterCoeff,
                                                                void *restrict pFilterVar,
                                                                void *restrict pOut);
