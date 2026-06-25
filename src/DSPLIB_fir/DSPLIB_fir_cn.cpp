// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_fir_priv.h"
#include "DSPLIB_utility.h"
#include <cstdint>

DSPLIB_STATUS DSPLIB_fir_init_cn(DSPLIB_kernelHandle   handle,
                                 DSPLIB_bufParams2D_t *bufParamsIn,
                                 DSPLIB_bufParams2D_t *bufParamsFilter,
                                 DSPLIB_bufParams2D_t *bufParamsOut,
                                 DSPLIB_fir_InitArgs  *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}

template <typename dataType, typename accumulateType>
DSPLIB_STATUS
DSPLIB_fir_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_fir_exec_cn\n");
#endif

   int32_t              j, k, l;
   dataType             outProd, outTemp;
   dataType            *pInLocal       = (dataType *) pIn;
   dataType            *pFilterLocal   = (dataType *) pFilter;
   dataType            *pOutLocal      = (dataType *) pOut;
   DSPLIB_fir_PrivArgs *pKerPrivArgs   = (DSPLIB_fir_PrivArgs *) handle;
   int32_t              batchSize      = pKerPrivArgs->initArgs.batchSize;
   int32_t              filterSize     = pKerPrivArgs->initArgs.filterSize;
   int32_t              outputSize     = pKerPrivArgs->outputSize;
   uint32_t             enableNchCoefs = pKerPrivArgs->initArgs.enableNchCoefs;
   uint32_t             MMA_SIZE       = pKerPrivArgs->initArgs.MMA_SIZE;
   uint32_t             enableMMA      = pKerPrivArgs->initArgs.enableMMA;

   uint32_t dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
#if DSPLIB_DEBUGPRINT
   printf("Enter pFilter %p\n", pFilterLocal);
#endif
   int32_t dataOffset    = 0;
   int32_t dataOffsetOut = 0;

   uint32_t filterPitchMMA = (enableNchCoefs == 0) ? 0 : (2 * MMA_SIZE + filterSize + MMA_SIZE - 1);
   uint32_t filterPitchC7x = (enableNchCoefs == 0) ? 0 : filterSize;
   uint32_t filterPitch    = (enableMMA == 0) ? filterPitchC7x : filterPitchMMA;

   for (l = 0; l < batchSize; l++) {
      for (j = 0; j < outputSize; j++) {
         outTemp           = 0;
         dataOffset        = l * dataBufferInPitch + j;
         dataOffsetOut     = l * dataBufferOutPitch + j;
         int32_t filterCtr = 0;
         for (k = filterSize - 1; k > -1; k--) {
            outProd = pInLocal[dataOffset + filterCtr] * pFilterLocal[k];
            outTemp = outTemp + outProd;
#if DSPLIB_DEBUGPRINT
//            printf("DSPLIB_DEBUGPRINT l %d  dataOffset %d filterCtr %d
//            dataBufferInPitch %d dataBufferOutPitch %d\n", l, dataOffset,
//            filterCtr, dataBufferInPitch, dataBufferOutPitch);
//            printf("DSPLIB_DEBUGPRINT pInLocal: %f, pFilterLocal[k]: %f,
//            outputSize: %d outTemp: %f outProd %f j %d k %d\n",
//            pInLocal[dataOffset + filterCtr], pFilterLocal[k], outputSize,
//            outTemp, outProd, j, k);
#endif
            filterCtr++;
         }
         pOutLocal[dataOffsetOut] = outTemp;

#if DSPLIB_DEBUGPRINT
//       printf("DSPLIB_DEBUGPRINT outTemp: %f, filterSize: %d, outputSize: %d
//       batchSize: %d j %d k %d\n", outTemp, filterSize, outputSize, batchSize,
//       j, k);
#endif
      }

      pFilterLocal += filterPitch;
   }

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_fir_exec_cn<int16_t, int64_t>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pFilter,
                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_exec_cn<int32_t, DSPLIB_int128_t>(DSPLIB_kernelHandle handle,
                                                                    void *restrict pIn,
                                                                    void *restrict pFilter,
                                                                    void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_exec_cn<int8_t, int32_t>(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn,
                                                           void *restrict pFilter,
                                                           void *restrict pOut);
template DSPLIB_STATUS DSPLIB_fir_exec_cn<float, int32_t>(DSPLIB_kernelHandle handle,
                                                          void *restrict pIn,
                                                          void *restrict pFilter,
                                                          void *restrict pOut);
