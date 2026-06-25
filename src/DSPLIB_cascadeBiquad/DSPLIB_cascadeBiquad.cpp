// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_cascadeBiquad_priv.h"

void DSPLIB_cascadeBiquad_perfEst(DSPLIB_kernelHandle         handle,
                                  const DSPLIB_bufParams2D_t *bufParamsIn,
                                  const DSPLIB_bufParams2D_t *bufParamsFilterCoeff,
                                  const DSPLIB_bufParams2D_t *bufParamsFilterVar,
                                  const DSPLIB_bufParams2D_t *bufParamsOut,
                                  uint64_t                   *archCycles,
                                  uint64_t                   *estCycles)
{
   uint32_t eleCount = DSPLIB_max_simd<DSPLIB_FLOAT32>::value;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs     = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint64_t                       entryCycles      = 0; // function entry cycles
   uint64_t                       exitCycles       = 0; // function exit cycles
   uint64_t                       startupCyclesL1  = 0; // loop level1 startup cycles
   uint64_t                       teardownCyclesL1 = 0; // loop level1 teardown cycles
   uint64_t                       startupCyclesL2  = 0; // loop level2 startup cycles
   uint64_t                       teardownCyclesL2 = 0; // loop level2 teardown cycles
   uint64_t                       swPipelineCycles = 0; // software pipeline cycles
   uint64_t                       computeCyclesL1;      // loop level1 compute cycles
   uint64_t                       computeCycles = 0;    // total compute cycles

   // get parameters
   uint32_t dataSize        = pKerPrivArgs->initArgs.dataSize;
   uint32_t numChannels     = pKerPrivArgs->initArgs.numChannels;
   uint32_t numStages       = pKerPrivArgs->initArgs.numStages;
   uint32_t biquadForm      = pKerPrivArgs->initArgs.biquadForm;
   uint32_t enableNChCoeffs = pKerPrivArgs->initArgs.enableNChCoeffs;
   uint32_t numStageGroups  = pKerPrivArgs->numStageGroups;
   uint32_t stage2Cnt;
   uint32_t stage1Cnt;

   // Compute number of channel groups
   uint32_t numChannelGroups = (numChannels + eleCount - 1) / eleCount;

   switch (biquadForm) {
   case DSPLIB_BIQUAD_TIF:
      // TI Form

      if (enableNChCoeffs == 0) {
         if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_1) {
            entryCycles      = 30;
            exitCycles       = 6;
            startupCyclesL1  = 4;
            teardownCyclesL1 = 6;
            swPipelineCycles = 5 + dataSize * 7;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_2) {
            entryCycles      = 30;
            exitCycles       = 7;
            startupCyclesL1  = 0;
            teardownCyclesL1 = 12;
            swPipelineCycles = 7 + dataSize * 7;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_3) {
            entryCycles      = 26;
            exitCycles       = 7;
            startupCyclesL1  = 6;
            teardownCyclesL1 = 10;
            swPipelineCycles = 16 + dataSize * 7;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_4) {
            entryCycles      = 24;
            exitCycles       = 6;
            startupCyclesL1  = 13;
            teardownCyclesL1 = 8;
            swPipelineCycles = 15 + dataSize * 11;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_5) {
            entryCycles      = 24;
            exitCycles       = 6;
            startupCyclesL1  = 11;
            teardownCyclesL1 = 9;
            swPipelineCycles = 17 + dataSize * 12;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_6) {
            entryCycles      = 24;
            exitCycles       = 6;
            startupCyclesL1  = 14;
            teardownCyclesL1 = 6;
            swPipelineCycles = 18 + dataSize * 14;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_8) {
            entryCycles      = 24;
            exitCycles       = 7;
            startupCyclesL1  = 24;
            teardownCyclesL1 = 16;
            swPipelineCycles = 16 + dataSize * 22;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else {
            // 7 stages or more than 8 stages
            entryCycles      = 26;
            exitCycles       = 7;
            startupCyclesL2  = 1;
            teardownCyclesL2 = 2;
            startupCyclesL1  = 17;
            teardownCyclesL1 = 13;
            swPipelineCycles = 20 + dataSize * 15;

            computeCycles = ((startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numStageGroups +
                             (startupCyclesL2 + teardownCyclesL2)) *
                                numChannelGroups +
                            entryCycles + exitCycles;
         }
      }
      else { // (pKerPrivArgs->initArgs.enableNChCoeffs == 1)
         if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_1) {
            entryCycles      = 40;
            exitCycles       = 6;
            startupCyclesL1  = 7;
            teardownCyclesL1 = 5;
            swPipelineCycles = 5 + dataSize * 7;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_2) {
            entryCycles      = 45;
            exitCycles       = 6;
            startupCyclesL1  = 7;
            teardownCyclesL1 = 5;
            swPipelineCycles = 10 + dataSize * 7;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_3) {
            entryCycles      = 45;
            exitCycles       = 7;
            startupCyclesL1  = 13;
            teardownCyclesL1 = 10;
            swPipelineCycles = 16 + dataSize * 7;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_4) {
            entryCycles      = 29;
            exitCycles       = 6;
            startupCyclesL1  = 14;
            teardownCyclesL1 = 8;
            swPipelineCycles = 15 + dataSize * 11;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_5) {
            entryCycles      = 29;
            exitCycles       = 6;
            startupCyclesL1  = 15;
            teardownCyclesL1 = 9;
            swPipelineCycles = 17 + dataSize * 12;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_6) {
            entryCycles      = 29;
            exitCycles       = 6;
            startupCyclesL1  = 15;
            teardownCyclesL1 = 13;
            swPipelineCycles = 18 + dataSize * 14;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else if (numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_8) {
            entryCycles      = 29;
            exitCycles       = 7;
            startupCyclesL1  = 26;
            teardownCyclesL1 = 16;
            swPipelineCycles = 16 + dataSize * 22;

            computeCycles =
                (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numChannelGroups + entryCycles + exitCycles;
         }
         else {
            // 7 stages or more than 8 stages
            entryCycles      = 35;
            exitCycles       = 8;
            startupCyclesL2  = 4;
            teardownCyclesL2 = 4;
            startupCyclesL1  = 17;
            teardownCyclesL1 = 13;
            swPipelineCycles = 20 + dataSize * 15;

            computeCycles = ((startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numStageGroups +
                             (startupCyclesL2 + teardownCyclesL2)) *
                                numChannelGroups +
                            entryCycles + exitCycles;
         }
      }

      break;

   case DSPLIB_BIQUAD_DF1:
      // Direct Form 1

      stage1Cnt = pKerPrivArgs->stage1Cnt;

      // 2-stage group loop
      startupCyclesL1  = 9;
      teardownCyclesL1 = 13;
      swPipelineCycles = 24 + dataSize * 11;
      computeCyclesL1  = (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numStageGroups;

      // Final stage
      computeCyclesL1 += 6; // final stage test
      startupCyclesL1  = 6;
      teardownCyclesL1 = 4;
      swPipelineCycles = 7 + dataSize * 11;
      computeCyclesL1 += (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * stage1Cnt;

      // Channel group loop
      startupCyclesL2  = 22;
      teardownCyclesL2 = 4;
      computeCycles    = (startupCyclesL2 + computeCyclesL1 + teardownCyclesL2) * numChannelGroups;

      // Entry & exit cycles
      entryCycles = 23;
      exitCycles  = 7;
      computeCycles += entryCycles + exitCycles;

      break;

   case DSPLIB_BIQUAD_DF2:
      // Direct Form 2

      stage1Cnt = pKerPrivArgs->stage1Cnt;

      // 2-stage group loop
      startupCyclesL1  = 6;
      teardownCyclesL1 = 0;
      swPipelineCycles = 26 + dataSize * 11;
      computeCyclesL1  = (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numStageGroups;

      // Final stage, if any
      computeCyclesL1 += 3; // final stage test
      startupCyclesL1  = 7;
      teardownCyclesL1 = 2;
      swPipelineCycles = 10 + dataSize * 11;
      computeCyclesL1 += (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * stage1Cnt;

      // Channel group loop
      startupCyclesL2  = 24;
      teardownCyclesL2 = 2;
      computeCycles    = (startupCyclesL2 + computeCyclesL1 + teardownCyclesL2) * numChannelGroups;

      // Entry & exit cycles
      entryCycles = 24;
      exitCycles  = 6;
      computeCycles += entryCycles + exitCycles;

      break;

   case DSPLIB_BIQUAD_TDF2:
      // Transposed Direct Form 2

      stage2Cnt = pKerPrivArgs->stage2Cnt;
      stage1Cnt = pKerPrivArgs->stage1Cnt;

      // 3-stage group loop
      startupCyclesL1  = 18;
      teardownCyclesL1 = 0;
      swPipelineCycles = 19 + dataSize * 10;
      computeCyclesL1  = (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * numStageGroups;

      // Final 2-stage group
      computeCyclesL1 += 3; // final 2-stage test
      startupCyclesL1  = 9;
      teardownCyclesL1 = 5;
      swPipelineCycles = 13 + dataSize * 10;
      computeCyclesL1 += (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * stage2Cnt;

      // Final 1-stage group
      computeCyclesL1 += 2; // final 1-stage test
      startupCyclesL1  = 3;
      teardownCyclesL1 = 3;
      swPipelineCycles = 5 + dataSize * 10;
      computeCyclesL1 += (startupCyclesL1 + swPipelineCycles + teardownCyclesL1) * stage1Cnt;

      // Channel group loop
      startupCyclesL2  = 30;
      teardownCyclesL2 = 5;
      computeCycles    = (startupCyclesL2 + computeCyclesL1 + teardownCyclesL2) * numChannelGroups;

      // Entry & exit cycles
      entryCycles = 24;
      exitCycles  = 8;
      computeCycles += entryCycles + exitCycles;

      break;

   default:
      computeCycles = 0;
      break;
   }

   *archCycles = computeCycles;
   *estCycles  = computeCycles;
}

int32_t DSPLIB_cascadeBiquad_getHandleSize(DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_cascadeBiquad_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_init_checkParams(DSPLIB_kernelHandle                  handle,
                                                    const DSPLIB_bufParams2D_t          *bufParamsIn,
                                                    const DSPLIB_bufParams2D_t          *bufParamsFilterCoeff,
                                                    const DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                                    const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                    const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if (bufParamsIn->data_type != DSPLIB_FLOAT32) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsFilterCoeff->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsFilterVar->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }

   if (status == DSPLIB_SUCCESS) {
      // Enforce same input and output buffer pitch
      if (bufParamsIn->stride_y != bufParamsOut->stride_y) {
         status = DSPLIB_ERR_NOT_IMPLEMENTED;
      }
   }

   if (status == DSPLIB_SUCCESS) {
      if ((pKerInitArgs->funcStyle != DSPLIB_FUNCTION_NATC) && (pKerInitArgs->funcStyle != DSPLIB_FUNCTION_OPTIMIZED)) {
         status = DSPLIB_ERR_INVALID_VALUE;
      }
   }

   if (status == DSPLIB_SUCCESS) {
      if ((pKerInitArgs->dataSize == 0) || (pKerInitArgs->numChannels == 0) || (pKerInitArgs->numStages == 0)) {
         status = DSPLIB_ERR_INVALID_VALUE;
      }
   }

   if (status == DSPLIB_SUCCESS) {
      if ((pKerInitArgs->biquadForm != DSPLIB_BIQUAD_TIF) && (pKerInitArgs->biquadForm != DSPLIB_BIQUAD_DF1) &&
          (pKerInitArgs->biquadForm != DSPLIB_BIQUAD_DF2) && (pKerInitArgs->biquadForm != DSPLIB_BIQUAD_TDF2)) {
         status = DSPLIB_ERR_INVALID_VALUE;
      }
   }

   if (status == DSPLIB_SUCCESS) {
      if ((pKerInitArgs->enableNChCoeffs != 0) && (pKerInitArgs->enableNChCoeffs != 1)) {
         status = DSPLIB_ERR_INVALID_VALUE;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_exec_checkParams(DSPLIB_kernelHandle handle,
                                                    const void *restrict pIn,
                                                    const void *restrict pFilterCoeff,
                                                    const void *restrict pFilterVar,
                                                    const void *restrict pOut)
{
   DSPLIB_STATUS status;

   if ((pIn == NULL) || (pFilterCoeff == NULL) || (pFilterVar == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

   return status;
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_init(DSPLIB_kernelHandle            handle,
                                        DSPLIB_bufParams2D_t          *bufParamsIn,
                                        DSPLIB_bufParams2D_t          *bufParamsFilterCoeff,
                                        DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                        DSPLIB_bufParams2D_t          *bufParamsOut,
                                        DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                  status       = DSPLIB_SUCCESS;
   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;

   pKerPrivArgs->initArgs = *pKerInitArgs;

   int32_t ele_size = 1;

   if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
      ele_size = 4;
   }
   else {
      status = DSPLIB_ERR_INVALID_TYPE;
   }

   if (status == DSPLIB_SUCCESS) {
      // Enforce same input and output buffer pitch
      if (bufParamsIn->stride_y != bufParamsOut->stride_y) {
         status = DSPLIB_ERR_NOT_IMPLEMENTED;
      }
   }

   if (status == DSPLIB_SUCCESS) {
      pKerPrivArgs->dataBufferInPitch  = bufParamsIn->stride_y / ele_size;
      pKerPrivArgs->dataBufferOutPitch = bufParamsOut->stride_y / ele_size;
      pKerPrivArgs->filterVarPitch     = bufParamsFilterVar->stride_y / ele_size;

      if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
         // Using Natural-C code

         status = DSPLIB_cascadeBiquad_init_cn(handle, bufParamsIn, bufParamsFilterCoeff, bufParamsFilterVar,
                                               bufParamsOut, pKerInitArgs);

         if (status == DSPLIB_SUCCESS) {
            if (pKerInitArgs->biquadForm == DSPLIB_BIQUAD_DF1) {
               pKerPrivArgs->execute = DSPLIB_cascadeBiquad_df1_exec_cn<float>;
            }
            else if (pKerInitArgs->biquadForm == DSPLIB_BIQUAD_DF2) {
               pKerPrivArgs->execute = DSPLIB_cascadeBiquad_df2_exec_cn<float>;
            }
            else if (pKerInitArgs->biquadForm == DSPLIB_BIQUAD_TDF2) {
               pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tdf2_exec_cn<float>;
            }
            else if (pKerInitArgs->biquadForm == DSPLIB_BIQUAD_TIF) {
               // Use DF2 Natural-C code for TI biquad form
               pKerPrivArgs->execute = DSPLIB_cascadeBiquad_df2_exec_cn<float>;
            }
            else {
               status = DSPLIB_ERR_NOT_IMPLEMENTED;
            }
         }
      }
      else { // DSPLIB_FUNCTION_OPTIMIZED
         // Using optimized code

         switch (pKerInitArgs->biquadForm) {
         case DSPLIB_BIQUAD_DF1:
            status = DSPLIB_cascadeBiquad_df1_init_ci<float>(handle, bufParamsIn, bufParamsFilterCoeff,
                                                             bufParamsFilterVar, bufParamsOut, pKerInitArgs);
            if (status == DSPLIB_SUCCESS) {
               pKerPrivArgs->execute = DSPLIB_cascadeBiquad_df1_exec_ci<float>;
            }

            break;

         case DSPLIB_BIQUAD_DF2:
            status = DSPLIB_cascadeBiquad_df2_init_ci<float>(handle, bufParamsIn, bufParamsFilterCoeff,
                                                             bufParamsFilterVar, bufParamsOut, pKerInitArgs);
            if (status == DSPLIB_SUCCESS) {
               pKerPrivArgs->execute = DSPLIB_cascadeBiquad_df2_exec_ci<float>;
            }

            break;

         case DSPLIB_BIQUAD_TDF2:
            status = DSPLIB_cascadeBiquad_tdf2_init_ci<float>(handle, bufParamsIn, bufParamsFilterCoeff,
                                                              bufParamsFilterVar, bufParamsOut, pKerInitArgs);
            if (status == DSPLIB_SUCCESS) {
               pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tdf2_exec_ci<float>;
            }

            break;

         case DSPLIB_BIQUAD_TIF:
            if (pKerInitArgs->enableNChCoeffs == 0) {
               //
               // Same coefficients for all channels
               //

               if (pKerPrivArgs->initArgs.numStages <= DSPLIB_CASCADEBIQUAD_TIF_THR_LOW_STAGE_COUNT) {
                  // Low-stage count implementation

                  status = DSPLIB_cascadeBiquad_tif_lStage_init_ci<float>(
                      handle, bufParamsIn, bufParamsFilterCoeff, bufParamsFilterVar, bufParamsOut, pKerInitArgs);

                  if (status == DSPLIB_SUCCESS) {
                     if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_1) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_1Stage_exec_ci<float>;
                     }
                     else if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_2) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_2Stage_exec_ci<float>;
                     }
                     else { // pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_3
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_3Stage_exec_ci<float>;
                     }
                  }
               }
               else {
                  // High-stage count implementation

                  status = DSPLIB_cascadeBiquad_tif_hStage_init_ci<float>(
                      handle, bufParamsIn, bufParamsFilterCoeff, bufParamsFilterVar, bufParamsOut, pKerInitArgs);

                  if (status == DSPLIB_SUCCESS) {
                     if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_4) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_4Stage_exec_ci<float>;
                     }
                     else if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_5) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_5Stage_exec_ci<float>;
                     }
                     else if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_6) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_6Stage_exec_ci<float>;
                     }
                     else if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_8) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_8Stage_exec_ci<float>;
                     }
                     else {
                        // 7 stages is handled with M-stage implementation
                        // More than 8 stages is handled with M-stage implementation
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_MStage_exec_ci<float>;
                     }
                  }
               }
            }
            else { // (pKerInitArgs->enableNChCoeffs == 1)
               //
               // Different coefficients for each channel
               //

               if (pKerPrivArgs->initArgs.numStages <= DSPLIB_CASCADEBIQUAD_TIF_THR_LOW_STAGE_COUNT) {
                  // Low-stage count implementation

                  status = DSPLIB_cascadeBiquad_tif_lStage_Nch_init_ci<float>(
                      handle, bufParamsIn, bufParamsFilterCoeff, bufParamsFilterVar, bufParamsOut, pKerInitArgs);

                  if (status == DSPLIB_SUCCESS) {
                     if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_1) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_1Stage_Nch_exec_ci<float>;
                     }
                     else if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_2) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_2Stage_Nch_exec_ci<float>;
                     }
                     else { // pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_3
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_3Stage_Nch_exec_ci<float>;
                     }
                  }
               }
               else {
                  // High-stage count implementation

                  status = DSPLIB_cascadeBiquad_tif_hStage_Nch_init_ci<float>(
                      handle, bufParamsIn, bufParamsFilterCoeff, bufParamsFilterVar, bufParamsOut, pKerInitArgs);

                  if (status == DSPLIB_SUCCESS) {
                     if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_4) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_4Stage_Nch_exec_ci<float>;
                     }
                     else if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_5) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_5Stage_Nch_exec_ci<float>;
                     }
                     else if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_6) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_6Stage_Nch_exec_ci<float>;
                     }
                     else if (pKerPrivArgs->initArgs.numStages == DSPLIB_CASCADEBIQUAD_NUM_STAGES_8) {
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_8Stage_Nch_exec_ci<float>;
                     }
                     else {
                        // 7 stages is handled with M-stage implementation
                        // More than 8 stages is handled with M-stage implementation
                        pKerPrivArgs->execute = DSPLIB_cascadeBiquad_tif_MStage_Nch_exec_ci<float>;
                     }
                  }
               }
            }

            break;

         default:
            status = DSPLIB_ERR_NOT_IMPLEMENTED;
            break;
         }
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_exec(DSPLIB_kernelHandle handle,
                                        void *restrict pIn,
                                        void *restrict pFilterCoeff,
                                        void *restrict pFilterVar,
                                        void *restrict pOut)
{
   DSPLIB_STATUS status;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pFilterCoeff, pFilterVar, pOut);

   return status;
}
