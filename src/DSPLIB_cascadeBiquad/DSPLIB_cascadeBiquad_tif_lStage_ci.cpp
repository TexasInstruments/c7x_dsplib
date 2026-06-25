// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_cascadeBiquad_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_lStage_init_ci(DSPLIB_kernelHandle                  handle,
                                                      const DSPLIB_bufParams2D_t          *bufParamsIn,
                                                      const DSPLIB_bufParams2D_t          *bufParamsFilterCoeff,
                                                      const DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                                      const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                      const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs       = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock             = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize           = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numChannels        = pKerPrivArgs->initArgs.numChannels;
   uint32_t                       numStages          = pKerPrivArgs->initArgs.numStages;
   uint32_t                       dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t                       dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t                       filterVarPitch     = pKerPrivArgs->filterVarPitch;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   // Compute number of channel groups
   uint32_t numChannelGroups = (numChannels + eleCount - 1) / eleCount;
   // Compute number of adjacent samples to read/write
   uint32_t icnt0Param = (numChannels > eleCount) ? eleCount : numChannels;

   /**********************************************************************/
   /* Prepare SE1 template to read the input                             */
   /**********************************************************************/
   se1Params               = __gen_SE_TEMPLATE_v1();
   se1Params.ICNT0         = icnt0Param;
   se1Params.ICNT1         = dataSize;
   se1Params.DIM1          = (int32_t) dataBufferInPitch;
   se1Params.ICNT2         = numChannelGroups;
   se1Params.DIM2          = (int32_t) eleCount;
   se1Params.ELETYPE       = SE_ELETYPE;
   se1Params.VECLEN        = SE_VECLEN;
   se1Params.DECDIM1_WIDTH = numChannels;
   se1Params.DECDIM1       = __SE_DECDIM_DIM2;
   se1Params.DIMFMT        = __SE_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare SE0 template to read the filter variables                  */
   /**********************************************************************/
   se0Params               = __gen_SE_TEMPLATE_v1();
   se0Params.ICNT0         = icnt0Param;
   se0Params.ICNT1         = numStages * DSPLIB_NUM_STATE_VARS_DF2;
   se0Params.DIM1          = (int32_t) filterVarPitch;
   se0Params.ICNT2         = numChannelGroups;
   se0Params.DIM2          = (int32_t) eleCount;
   se0Params.ELETYPE       = SE_ELETYPE;
   se0Params.VECLEN        = SE_VECLEN;
   se0Params.DECDIM1_WIDTH = numChannels;
   se0Params.DECDIM1       = __SE_DECDIM_DIM2;
   se0Params.DIMFMT        = __SE_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare SA0 template to write the output                           */
   /**********************************************************************/
   sa0Params               = __gen_SA_TEMPLATE_v1();
   sa0Params.ICNT0         = icnt0Param;
   sa0Params.ICNT1         = dataSize;
   sa0Params.DIM1          = (int32_t) dataBufferOutPitch;
   sa0Params.ICNT2         = numChannelGroups;
   sa0Params.DIM2          = (int32_t) eleCount;
   sa0Params.DECDIM1_WIDTH = numChannels;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DIMFMT        = __SA_DIMFMT_3D;

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

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;

   return status;
}

template DSPLIB_STATUS
DSPLIB_cascadeBiquad_tif_lStage_init_ci<float>(DSPLIB_kernelHandle                  handle,
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
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_1Stage_exec_ci(DSPLIB_kernelHandle handle,
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

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numStages    = pKerPrivArgs->initArgs.numStages;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   // Filter variables read
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pFilterVarLocal, se0Params);

   // Input samples read
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pInLocal, se1Params);

   // Output samples write
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   // cascade input
   vec input;

   // stage output
   vec output1;

   // coefficients
   vec b11_a11, b12_a12, a11, a12;
   // aggregate gain
   vec outGain;

   // stage temporary variables
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_add_1;

   // Read stage 1 coefficients
   b11_a11 = (vec) pFilterCoeffLocal[0];
   b12_a12 = (vec) pFilterCoeffLocal[1];
   a12     = (vec) pFilterCoeffLocal[2];
   a11     = (vec) pFilterCoeffLocal[3];

   // Read aggregate gain
   outGain = pFilterCoeffLocal[numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS];

#pragma MUST_ITERATE(1, , )
   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      //
      // Read filter variables
      //

      // Stage 1
      vec filtVars_d0_1 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d1_1 = c7x::strm_eng<0, vec>::get_adv();

#pragma MUST_ITERATE(1, , )
      for (uint32_t samp = 0; samp < dataSize; samp++) {
         // Read input
         input = c7x::strm_eng<1, vec>::get();

         // Calculate stage 1
         output1       = input + filtVars_d0_1; // output1 = input + vd01;
         temp1_mul_1   = input * b11_a11;       // t1 = (((b11 + a11) * input) + vd11)
         temp1_add_1   = temp1_mul_1 + filtVars_d1_1;
         temp0_mul_1   = a12 * filtVars_d0_1; // t0 = (a12 * vd01)
         d0_temp_1     = a11 * filtVars_d0_1; // vd01 = ((a11 * vd01) + t1)
         filtVars_d0_1 = d0_temp_1 + temp1_add_1;
         input         = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_1      = b12_a12 * input; // vd11 = (((b12 + a12) * input) + t0)
         filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

         // Apply gain
         vec output = output1 * outGain;

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
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_1Stage_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                      void *restrict pIn,
                                                                      void *restrict pFilterCoeff,
                                                                      void *restrict pFilterVar,
                                                                      void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_2Stage_exec_ci(DSPLIB_kernelHandle handle,
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

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numStages    = pKerPrivArgs->initArgs.numStages;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   // Filter variables read
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pFilterVarLocal, se0Params);

   // Input samples read
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pInLocal, se1Params);

   // Output samples write
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables read
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);
   // cascade input
   vec input;

   // stage outputs
   vec output1, output2;

   // coefficients
   vec b11_a11, b12_a12, a11, a12;
   vec b21_a21, b22_a22, a21, a22;

   // stage temporary variables
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_mul_2, temp0_mul_2, d0_temp_2, d1_mul_2;
   vec temp1_add_1, temp1_add_2;

   // Read stage 1 coefficients
   b11_a11 = (vec) pFilterCoeffLocal[0];
   b12_a12 = (vec) pFilterCoeffLocal[1];
   a12     = (vec) pFilterCoeffLocal[2];
   a11     = (vec) pFilterCoeffLocal[3];

   // Read stage 2 coefficients
   b21_a21 = (vec) pFilterCoeffLocal[4];
   b22_a22 = (vec) pFilterCoeffLocal[5];
   a22     = (vec) pFilterCoeffLocal[6];
   a21     = (vec) pFilterCoeffLocal[7];

   // Read aggregate gain
   vec outGain = pFilterCoeffLocal[numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS];

#pragma MUST_ITERATE(1, , )
   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      //
      // Read filter variables
      //

      // Stage 1
      vec filtVars_d0_1 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d1_1 = c7x::strm_eng<0, vec>::get_adv();

      // Stage 2
      vec filtVars_d0_2 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d1_2 = c7x::strm_eng<0, vec>::get_adv();

#pragma MUST_ITERATE(1, , )
      for (uint32_t samp = 0; samp < dataSize; samp++) {
         // Read input
         input = c7x::strm_eng<1, vec>::get();

         // Calculate stage 1
         output1       = input + filtVars_d0_1; // output1 = input + vd01
         temp1_mul_1   = input * b11_a11;       // t1 = (((b11 + a11) * input) + vd11)
         temp1_add_1   = temp1_mul_1 + filtVars_d1_1;
         temp0_mul_1   = a12 * filtVars_d0_1; // t0 = (a12 * vd01)
         d0_temp_1     = a11 * filtVars_d0_1; // vd01 = ((a11 * vd01) + t1)
         filtVars_d0_1 = d0_temp_1 + temp1_add_1;
         input         = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_1      = b12_a12 * input; // vd11 = (((b12 + a12) * input) + t0)
         filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

         // Calculate stage 2
         output2       = output1 + filtVars_d0_2; // output2 = (output01 + vd02)
         temp1_mul_2   = b21_a21 * output1;       // t3 = (((b21 + a21) * output01) + vd12)
         temp1_add_2   = temp1_mul_2 + filtVars_d1_2;
         temp0_mul_2   = a22 * filtVars_d0_2; // t2 = (a22 * vd02)
         d0_temp_2     = a21 * filtVars_d0_2; // vd02 = ((a21 * vd02) + t3)
         filtVars_d0_2 = d0_temp_2 + temp1_add_2;
         d1_mul_2      = b22_a22 * output1; // vd12 = (((b22 + a22) * output01) + t2)
         filtVars_d1_2 = d1_mul_2 + temp0_mul_2;

         // Apply gain
         vec output = output2 * outGain;

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
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_2Stage_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                      void *restrict pIn,
                                                                      void *restrict pFilterCoeff,
                                                                      void *restrict pFilterVar,
                                                                      void *restrict pOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_3Stage_exec_ci(DSPLIB_kernelHandle handle,
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

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numStages    = pKerPrivArgs->initArgs.numStages;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   // Filter variables read
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pFilterVarLocal, se0Params);

   // Input samples read
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pInLocal, se1Params);

   // Output samples write
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   // cascade input
   vec input;

   // stage outputs
   vec output1, output2, output3;

   // coefficients
   vec b11_a11, b12_a12, a11, a12;
   vec b21_a21, b22_a22, a21, a22;
   vec b31_a31, b32_a32, a31, a32;

   // stage temporary variables
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_mul_2, temp0_mul_2, d0_temp_2, d1_mul_2;
   vec temp1_mul_3, temp0_mul_3, d0_temp_3, d1_mul_3;
   vec temp1_add_1, temp1_add_2, temp1_add_3;

   // Read stage 1 coefficients
   b11_a11 = (vec) pFilterCoeffLocal[0];
   b12_a12 = (vec) pFilterCoeffLocal[1];
   a12     = (vec) pFilterCoeffLocal[2];
   a11     = (vec) pFilterCoeffLocal[3];

   // Read stage 2 coefficients
   b21_a21 = (vec) pFilterCoeffLocal[4];
   b22_a22 = (vec) pFilterCoeffLocal[5];
   a22     = (vec) pFilterCoeffLocal[6];
   a21     = (vec) pFilterCoeffLocal[7];

   // Read stage 3 coefficients
   b31_a31 = (vec) pFilterCoeffLocal[8];
   b32_a32 = (vec) pFilterCoeffLocal[9];
   a32     = (vec) pFilterCoeffLocal[10];
   a31     = (vec) pFilterCoeffLocal[11];

   // Read aggregate gain
   vec outGain = pFilterCoeffLocal[numStages * DSPLIB_NUM_TIF_BIQUAD_COEFS];

#pragma MUST_ITERATE(1, , )
   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      // Read filter variables
      vec filtVars_d0_1 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d1_1 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d0_2 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d1_2 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d0_3 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d1_3 = c7x::strm_eng<0, vec>::get_adv();

#pragma MUST_ITERATE(1, , )
      for (uint32_t samp = 0; samp < dataSize; samp++) {
         // Read input
         input = c7x::strm_eng<1, vec>::get();

         // Calculate stage 1
         output1       = input + filtVars_d0_1; // output1 = input + vd01
         temp1_mul_1   = input * b11_a11;       // t1 = (((b11 + a11) * input) + vd11)
         temp1_add_1   = temp1_mul_1 + filtVars_d1_1;
         temp0_mul_1   = a12 * filtVars_d0_1; // t0 = (a12 * vd01)
         d0_temp_1     = a11 * filtVars_d0_1; // vd01 = ((a11 * vd01) + t1)
         filtVars_d0_1 = d0_temp_1 + temp1_add_1;
         input         = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_1      = b12_a12 * input; // vd11 = (((b12 + a12) * input) + t0)
         filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

         // Calculate stage 2
         output2       = output1 + filtVars_d0_2; // output2 = (output01 + vd02)
         temp1_mul_2   = b21_a21 * output1;       // t3 = (((b21 + a21) * output01) + vd12)
         temp1_add_2   = temp1_mul_2 + filtVars_d1_2;
         temp0_mul_2   = a22 * filtVars_d0_2; // t2 = (a22 * vd02)
         d0_temp_2     = a21 * filtVars_d0_2; // vd02 = ((a21 * vd02) + t3)
         filtVars_d0_2 = d0_temp_2 + temp1_add_2;
         d1_mul_2      = b22_a22 * output1; // vd12 = (((b22 + a22) * output01) + t2)
         filtVars_d1_2 = d1_mul_2 + temp0_mul_2;

         // Calculate stage 3
         output3       = output2 + filtVars_d0_3; // output3 = (output02 + vd03)
         temp1_mul_3   = b31_a31 * output2;
         temp1_add_3   = temp1_mul_3 + filtVars_d1_3; // t5 = (((b31 + a31) * output02) + vd13)
         temp0_mul_3   = a32 * filtVars_d0_3;         // t4 = (a32 * vd03)
         d0_temp_3     = a31 * filtVars_d0_3;         // vd03 = ((a31 * vd03) + t5)
         filtVars_d0_3 = d0_temp_3 + temp1_add_3;
         d1_mul_3      = b32_a32 * output2; // vd13 = (((b32 + a32) * output02) + t4)
         filtVars_d1_3 = d1_mul_3 + temp0_mul_3;

         // Apply gain
         vec output = output3 * outGain;

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
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_tif_3Stage_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                      void *restrict pIn,
                                                                      void *restrict pFilterCoeff,
                                                                      void *restrict pFilterVar,
                                                                      void *restrict pOut);
