// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "../common/c71/init_mma.h"
#include "DSPLIB_fir_priv.h"
#include "c7x_scalable.h"
#include <cstdint>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA1_PARAM_OFFSET (SE_SA0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA2_PARAM_OFFSET (SE_SA1_PARAM_OFFSET + SE_PARAM_SIZE)
#define MMA_CONFIGREG_OFFSET (SE_SA2_PARAM_OFFSET + SE_PARAM_SIZE)
#define MMA_OFFSETREG_OFFSET (MMA_CONFIGREG_OFFSET + sizeof(__HWA_CONFIG_REG_v1))
#define MMA_FILTER_OFFSET (MMA_OFFSETREG_OFFSET + sizeof(__HWA_OFFSET_REG))

/* FIR execution mode flags */
#define FIR_EXEC_MODE_MMA (1)

/* Unroll factor for Q fixed point processing */
#define Q_UNROLL_FACTOR (3)

/* FIR mode flags */
#define FIR_MODE_SMALL_FIR (0)
#define FIR_MODE_LARGE_FIR (1)
#define FIR_MODE_SMALL_FIR_SS (2)
#define FIR_MODE_MEDIUM_FIR (3)
#define FIR_MODE_SMALL_MEDIUM_FIR (4)

/* Small FIR macros */
#define SMALL_FIR_LOOP_UNROLL_FACTOR (4)

/* Long FIR macros */
#define LONG_FIR_OUT_SAMPLES_PER_BLOCK (8)
#define LONG_FIR_NUM_OUT_SAMP_GROUPS (2)
#define LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK (LONG_FIR_OUT_SAMPLES_PER_BLOCK * LONG_FIR_NUM_OUT_SAMP_GROUPS)

/* Medium FIR macros */
#define MEDIUM_FIR_OUT_SAMPLES_PER_BLOCK (4)
#define MEDIUM_FIR_NUM_OUT_SAMP_GROUPS (2)
#define MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK (MEDIUM_FIR_OUT_SAMPLES_PER_BLOCK * MEDIUM_FIR_NUM_OUT_SAMP_GROUPS)

#define SMALL_MEDIUM_FIR_OUT_SAMPLES_PER_BLOCK (2)
#define SMALL_MEDIUM_FIR_NUM_OUT_SAMP_GROUPS (2)
#define SMALL_MEDIUM_FIR_FILTER_BANKS (2)
#define SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK                                                                     \
   (SMALL_MEDIUM_FIR_OUT_SAMPLES_PER_BLOCK * SMALL_MEDIUM_FIR_NUM_OUT_SAMP_GROUPS)

void DSPLIB_fir_perfEst(DSPLIB_kernelHandle         handle,
                        const DSPLIB_bufParams2D_t *bufParamsIn,
                        const DSPLIB_bufParams2D_t *bufParamsFilter,
                        const DSPLIB_bufParams2D_t *bufParamsOut,
                        uint64_t                   *archCycles,
                        uint64_t                   *estCycles)
{
   return;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_getSizes(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                  DSPLIB_bufParams2D_t      *bufParamsIn,
                                  DSPLIB_bufParams2D_t      *bufParamsFilter,
                                  DSPLIB_bufParams2D_t      *bufParamsOut)
{
   uint32_t dataSize   = pKerInitArgs->dataSize;
   uint32_t filterSize = pKerInitArgs->filterSize;
   uint32_t enableMMA  = pKerInitArgs->enableMMA;
   uint32_t MMA_SIZE   = pKerInitArgs->MMA_SIZE;
   uint32_t computeLoopOutputSize;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_fir_getSizes\n");
#endif

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   /* Number of output points produced in the C7x compute loop for a single
      channel in the batch */
   computeLoopOutputSize = (dataSize - filterSize + 1);
   bufParamsOut->dim_x   = computeLoopOutputSize;

   bufParamsFilter->dim_y = (pKerInitArgs->enableNchCoefs == 0) ? 1 : (pKerInitArgs->batchSize);

   if (enableMMA) {

      // clang-format off
       bufParamsFilter->dim_x = 2 * MMA_SIZE /* left padding */
    + filterSize /* actual filter size */
       + MMA_SIZE - 1 /* right padding */;

      // clang-format on
      bufParamsFilter->stride_y = bufParamsFilter->dim_x * sizeof(dataType);
   }

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_fir_getSizes bufParamsOut->dim_x %d\n", bufParamsOut->dim_x);
#endif
   return status;
}

template DSPLIB_STATUS DSPLIB_fir_getSizes<float>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                  DSPLIB_bufParams2D_t      *bufParamsIn,
                                                  DSPLIB_bufParams2D_t      *bufParamsFilter,
                                                  DSPLIB_bufParams2D_t      *bufParamsOut);

template DSPLIB_STATUS DSPLIB_fir_getSizes<int16_t>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                    DSPLIB_bufParams2D_t      *bufParamsIn,
                                                    DSPLIB_bufParams2D_t      *bufParamsFilter,
                                                    DSPLIB_bufParams2D_t      *bufParamsOut);
template DSPLIB_STATUS DSPLIB_fir_getSizes<int32_t>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                    DSPLIB_bufParams2D_t      *bufParamsIn,
                                                    DSPLIB_bufParams2D_t      *bufParamsFilter,
                                                    DSPLIB_bufParams2D_t      *bufParamsOut);
template DSPLIB_STATUS DSPLIB_fir_getSizes<int8_t>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                   DSPLIB_bufParams2D_t      *bufParamsIn,
                                                   DSPLIB_bufParams2D_t      *bufParamsFilter,
                                                   DSPLIB_bufParams2D_t      *bufParamsOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_init_ci(DSPLIB_kernelHandle         handle,
                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                 const DSPLIB_bufParams2D_t *bufParamsFilter,
                                 const DSPLIB_bufParams2D_t *bufParamsOut,
                                 const DSPLIB_fir_InitArgs  *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params; // =__gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params; // =__gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params; // =__gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa1Params; // =__gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa2Params; // =__gen_SA_TEMPLATE_v1();

   DSPLIB_fir_PrivArgs *pKerPrivArgs       = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock             = pKerPrivArgs->bufPblock;
   int32_t              dataSize           = pKerPrivArgs->initArgs.dataSize;
   int32_t              batchSize          = pKerPrivArgs->initArgs.batchSize;
   int32_t              filterSize         = pKerPrivArgs->initArgs.filterSize;
   uint32_t             enableNchCoefs     = pKerPrivArgs->initArgs.enableNchCoefs;
   uint32_t             dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t             dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t             filterPitch        = pKerPrivArgs->filterPitch;
   int32_t              outputSize         = pKerPrivArgs->outputSize;

   uint32_t enableMMA = pKerPrivArgs->initArgs.enableMMA;

#if !defined(__C7X_MMA_2_256F__)
   uint32_t Q = 0u;
   if ((bufParamsIn->data_type == (uint32_t) DSPLIB_FLOAT32) && (enableMMA != 0U)) {

      Q = pKerPrivArgs->initArgs.Q;
   }
   else {
      Q = 0u;
   }
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_ELEDUP                                            SE_ELEDUP  = c7x::se_eledup<dataType>::value;

   int32_t  eleCount = c7x::element_count_of<vec>::value;
   uint32_t flag;              /* flag indicating small or large FIR */
   int32_t  blkElements;       /* number of outputs produce per loop iteration */
   int32_t  dataLoopSize;      /* number of output data loops */
   int32_t  filterLoopSize;    /* number of primary filter loops */
   int32_t  remFilterLoopSize; /* number of remaining filter loops */

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d "
          "batchSize %d outputSize %d\n",
          SE_VECLEN, SA_VECLEN, SE_ELETYPE, batchSize, outputSize);
#endif

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d "
          "SE_ELEDUP %d eleCount %d\n",
          SE_VECLEN, SA_VECLEN, SE_ELETYPE, SE_ELEDUP, eleCount);
#endif

   /* Init FIR on MMA */
   if (enableMMA == FIR_EXEC_MODE_MMA) {

#if DSPLIB_DEBUGPRINT
      printf("FIR MMA EXEC MODE\n");
#endif

      /* Set MMA execution function pointer */
      pKerPrivArgs->execute = DSPLIB_fir_MMA_exec_ci<dataType>;

      // clear sticky fields from parity checker
      init_mma();

      // TODO: fill in SE/SA parameters and MMA CONFIG/OFFSET registers for MMA here

      int32_t MMA_SIZE; /* mma matrix panel dimensions */

      __HWA_OFFSET_REG    mmaOffsetReg;
      __HWA_CONFIG_REG_v1 mmaConfigReg;

      se0Params = __gen_SE_TEMPLATE_v1();
      se1Params = __gen_SE_TEMPLATE_v1();
      sa0Params = __gen_SA_TEMPLATE_v1();

      MMA_SIZE = c7x::element_count_of<vec>::value;

      filterLoopSize = (dataSize >= filterSize) ? DSPLIB_ceilingDiv((filterSize + MMA_SIZE - 1), MMA_SIZE)
                                                : DSPLIB_ceilingDiv(dataSize, (MMA_SIZE * MMA_SIZE));
      dataLoopSize   = DSPLIB_ceilingDiv((dataSize - filterSize + 1), (MMA_SIZE * MMA_SIZE));

      pKerPrivArgs->dataLoopSize   = dataLoopSize;
      pKerPrivArgs->filterLoopSize = filterLoopSize;

      if (bufParamsIn->data_type == DSPLIB_INT32) {
         mmaConfigReg = configRegisterStruct_i32s_i32s_o32s;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT16) {
         mmaConfigReg = configRegisterStruct_i16s_i16s_o16s;
      }
      else if (bufParamsIn->data_type == DSPLIB_INT8) {
         mmaConfigReg = configRegisterStruct_i8s_i8s_o8s;
      }
      else if (bufParamsIn->data_type == DSPLIB_FLOAT32) {

#if !defined(__C7X_MMA_2_256F__)

         mmaConfigReg         = configRegisterStruct_i32s_i32s_o32s;
         mmaConfigReg.X_SHIFT = Q;
#else
         mmaConfigReg = configRegisterStruct_i32f_i32f_o32f;
#endif // #if !defined(__C7X_MMA_2_256F__)
      }

      else {
         status = DSPLIB_ERR_NOT_IMPLEMENTED;
      }

      if (status == DSPLIB_SUCCESS) {

         /**********************************************************************/
         /* Prepare streaming engine 0 to fetch data                           */
         /**********************************************************************/

         se0Params.ICNT0 = MMA_SIZE * MMA_SIZE;

         se0Params.DIM1  = MMA_SIZE;
         se0Params.ICNT1 = filterLoopSize;

         se0Params.DIM2  = MMA_SIZE * MMA_SIZE;
         se0Params.ICNT2 = dataLoopSize;

         se0Params.DIM3  = dataBufferInPitch;
         se0Params.ICNT3 = batchSize;

         se0Params.DECDIM1       = __SE_DECDIM_DIM2;
         se0Params.DECDIM1_WIDTH = dataSize;
         se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM1;

         se0Params.DIMFMT  = __SE_DIMFMT_4D;
         se0Params.VECLEN  = SE_VECLEN;
         se0Params.ELETYPE = SE_ELETYPE;

         /**********************************************************************/
         /* Prepare streaming engine 1 to fetch the filter                     */
         /**********************************************************************/

         se1Params.ICNT0 = MMA_SIZE;

         se1Params.DIM1  = -1;
         se1Params.ICNT1 = MMA_SIZE;

         se1Params.DIM2  = -MMA_SIZE;
         se1Params.ICNT2 = filterLoopSize;

         se1Params.DIM3  = 0;
         se1Params.ICNT3 = dataLoopSize;

         se1Params.DIM4  = (enableNchCoefs == 0) ? 0 : filterPitch;
         se1Params.ICNT4 = batchSize;

         se1Params.DIMFMT  = __SE_DIMFMT_5D;
         se1Params.VECLEN  = SE_VECLEN;
         se1Params.ELETYPE = SE_ELETYPE;

         /**********************************************************************/
         /* Prepare SA template to store output                                */
         /**********************************************************************/

         sa0Params.ICNT0 = MMA_SIZE;

         sa0Params.DIM1  = MMA_SIZE;
         sa0Params.ICNT1 = MMA_SIZE;

         sa0Params.DIM2  = MMA_SIZE * MMA_SIZE;
         sa0Params.ICNT2 = filterLoopSize;

         // overwrite the data
         sa0Params.DIM3  = MMA_SIZE * MMA_SIZE;
         sa0Params.ICNT3 = dataLoopSize;

         sa0Params.DIM4  = dataBufferOutPitch;
         sa0Params.ICNT4 = batchSize;

         sa0Params.DECDIM1       = __SA_DECDIM_DIM3;
         sa0Params.DECDIM1_WIDTH = dataSize - filterSize + 1;
         sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM1;

         // never store beyond matrix size
         sa0Params.DECDIM2       = __SA_DECDIM_DIM2;
         sa0Params.DECDIM2_WIDTH = MMA_SIZE * MMA_SIZE;

         sa0Params.DIMFMT = __SA_DIMFMT_5D;
         sa0Params.VECLEN = SA_VECLEN;

         /**********************************************************************/
         /* Prepare MMA                                                        */
         /**********************************************************************/
         mmaOffsetReg = offsetRegStruct_zeros;

         /* Relevant Default MMACONFIG Settings */
         /* mmaConfigReg.C_OP0PER = MMA_SIZE;   */
         /* mmaConfigReg.C_OP0 = MUL;           */
         /* mmaConfigReg.C_OP1 = MUL_PLUS;      */
         /* mmaConfigReg.C_OPSTART = OP0;       */

         mmaConfigReg.C_OP1PER  = (filterLoopSize - 1) * MMA_SIZE;
         mmaConfigReg.C_CWSWPER = filterLoopSize * MMA_SIZE;
         mmaConfigReg.C_CRSWPER = filterLoopSize * MMA_SIZE;

         mmaConfigReg.X_CSWPER   = filterLoopSize * MMA_SIZE;
         mmaConfigReg.X_CRRSTPER = MMA_SIZE;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET)       = se0Params;
         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET)       = se1Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET)       = sa0Params;
         *((__HWA_CONFIG_REG_v1 *) ((uint8_t *) pBlock + MMA_CONFIGREG_OFFSET)) = mmaConfigReg;
         *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock + MMA_OFFSETREG_OFFSET))    = mmaOffsetReg;
         /* Pad filter with zeros in the beginning (2*MMA_SIZE) and end (MMA_SIZE - 1) */
         *((int32_t *) ((uint8_t *) pBlock + MMA_FILTER_OFFSET)) = 2 * MMA_SIZE;
      }
   }

   /* Init FIR on C7x */
   else {
      if ((dataSize >= (eleCount * LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK)) &&
          (filterSize <= (dataSize - LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount + 1))) {

         flag = FIR_MODE_LARGE_FIR; /* set flag for large FIR */

         blkElements  = LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         dataLoopSize = (outputSize + blkElements - 1) / blkElements;
         dataLoopSize = (dataLoopSize == 0) ? 1 : dataLoopSize;

#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT dataLoopSize: %d (dataSize - filterSize) %d 16*eleCount "
                "%d blkElements %d ((dataSize - filterSize) / blkElements) %d\n",
                dataLoopSize, (dataSize - filterSize), LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount, blkElements,
                ((dataSize - filterSize) / blkElements));
#endif

         filterLoopSize    = filterSize;
         remFilterLoopSize = 0;
      }
      else {
         if (outputSize >= MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount) {
            flag = FIR_MODE_MEDIUM_FIR; // set flag for medium FIR

            blkElements       = (eleCount * MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK);
            dataLoopSize      = DSPLIB_ceilingDiv(outputSize, blkElements);
            filterLoopSize    = filterSize;
            remFilterLoopSize = 0;
         }
         else if (outputSize >= SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount) {
            flag = FIR_MODE_SMALL_MEDIUM_FIR; // set flag for small medium FIR

            blkElements       = (eleCount * SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK);
            dataLoopSize      = DSPLIB_ceilingDiv(outputSize, blkElements);
            filterLoopSize    = filterSize / SMALL_MEDIUM_FIR_FILTER_BANKS;
            remFilterLoopSize = filterSize % SMALL_MEDIUM_FIR_FILTER_BANKS;
         }
         else if (outputSize > 1) {
            flag = FIR_MODE_SMALL_FIR; /* set flag for small FIR */

            blkElements  = (eleCount > filterSize) ? filterSize : eleCount;
            dataLoopSize = (outputSize + blkElements - 1) / blkElements;
            dataLoopSize = (dataLoopSize == 0) ? 1 : dataLoopSize;

            filterLoopSize    = filterSize / SMALL_FIR_LOOP_UNROLL_FACTOR;
            remFilterLoopSize = filterSize % SMALL_FIR_LOOP_UNROLL_FACTOR;
         }
         else {
            flag = FIR_MODE_SMALL_FIR_SS; /* set flag for small FIR w/ single output sample */

            dataLoopSize = outputSize;

            int32_t numFilterVec = (filterSize + eleCount - 1) / eleCount;
            filterLoopSize       = numFilterVec / SMALL_FIR_LOOP_UNROLL_FACTOR;
            remFilterLoopSize    = numFilterVec % SMALL_FIR_LOOP_UNROLL_FACTOR;
         }
      }
      pKerPrivArgs->dataLoopSize      = dataLoopSize;
      pKerPrivArgs->filterLoopSize    = filterLoopSize;
      pKerPrivArgs->remFilterLoopSize = remFilterLoopSize;
      pKerPrivArgs->flag              = flag;

#if DSPLIB_DEBUGPRINT
      printf("DSPLIB_DEBUGPRINT FIR size flag: %d\n", flag);
#endif

      if (flag == FIR_MODE_SMALL_FIR) {
         pKerPrivArgs->execute = DSPLIB_fir_smallFir_exec_ci<dataType>;
         /**********************************************************************/
         /* Prepare streaming engine 1 to fetch the filter values              */
         /**********************************************************************/
         se1Params         = __gen_SE_TEMPLATE_v1();
         se1Params.ICNT0   = 1;
         se1Params.ICNT1   = filterSize;
         se1Params.DIM1    = -(int32_t) 1;
         se1Params.ICNT2   = dataLoopSize;
         se1Params.DIM2    = 0;
         se1Params.ICNT3   = batchSize;
         se1Params.DIM3    = (enableNchCoefs == 0) ? 0 : filterPitch;
         se1Params.ELETYPE = SE_ELETYPE;
         se1Params.VECLEN  = SE_VECLEN;
         se1Params.ELEDUP  = SE_ELEDUP;
         se1Params.DIMFMT  = __SE_DIMFMT_4D;
         se1Params.DIR     = __SE_DIR_DEC;

         /**********************************************************************/
         /* Prepare streaming engine 0 to fetch data                           */
         /**********************************************************************/
         se0Params               = __gen_SE_TEMPLATE_v1();
         se0Params.ICNT0         = blkElements;
         se0Params.ICNT1         = filterSize;
         se0Params.DIM1          = (int32_t) 1;
         se0Params.ICNT2         = dataLoopSize;
         se0Params.DIM2          = blkElements;
         se0Params.ICNT3         = batchSize;
         se0Params.DIM3          = dataBufferInPitch;
         se0Params.DECDIM1       = __SE_DECDIM_DIM2;
         se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM1;
         se0Params.DECDIM1_WIDTH = dataSize;
         se0Params.ELETYPE       = SE_ELETYPE;
         se0Params.VECLEN        = SE_VECLEN;
         se0Params.DIMFMT        = __SE_DIMFMT_4D;

         /**********************************************************************/
         /* Prepare SA template to store output                                */
         /**********************************************************************/
         sa0Params               = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0         = blkElements;
         sa0Params.ICNT1         = dataLoopSize;
         sa0Params.DIM1          = blkElements;
         sa0Params.ICNT2         = batchSize;
         sa0Params.DIM2          = dataBufferOutPitch;
         sa0Params.DECDIM1_WIDTH = outputSize;
         sa0Params.DECDIM1       = __SA_DECDIM_DIM1;
         sa0Params.VECLEN        = SA_VECLEN;
         sa0Params.DIMFMT        = __SA_DIMFMT_3D;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      }
      else if (flag == FIR_MODE_SMALL_MEDIUM_FIR) {
         pKerPrivArgs->execute = DSPLIB_fir_smallMediumFir_exec_ci<dataType>;

         /*********************************************
          * Prepare SA template to read filter values *
          *********************************************/
         sa1Params        = __gen_SA_TEMPLATE_v1();
         sa1Params.ICNT0  = (uint32_t) 1;
         sa1Params.ICNT1  = (uint32_t) (filterLoopSize + remFilterLoopSize);
         sa1Params.DIM1   = -(int32_t) 2;
         sa1Params.ICNT2  = dataLoopSize;
         sa1Params.DIM2   = 0;
         sa1Params.ICNT3  = batchSize;
         sa1Params.DIM3   = (enableNchCoefs == 0) ? 0 : filterPitch;
         sa1Params.DIMFMT = __SA_DIMFMT_4D;
         sa1Params.VECLEN = __SA_VECLEN_1ELEM;

         sa2Params        = __gen_SA_TEMPLATE_v1();
         sa2Params.ICNT0  = (uint32_t) 1;
         sa2Params.ICNT1  = (uint32_t) ((filterLoopSize == 0) ? 1 : filterLoopSize);
         sa2Params.DIM1   = -(int32_t) 2;
         sa2Params.ICNT2  = dataLoopSize;
         sa2Params.DIM2   = 0;
         sa2Params.ICNT3  = batchSize;
         sa2Params.DIM3   = (enableNchCoefs == 0) ? 0 : filterPitch;
         sa2Params.DIMFMT = __SA_DIMFMT_4D;
         sa2Params.VECLEN = __SA_VECLEN_1ELEM;

         /**********************************************************************/
         /* Prepare streaming engine 0 to fetch data                           */
         /**********************************************************************/
         se0Params               = __gen_SE_TEMPLATE_v1();
         se0Params.ICNT0         = SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         se0Params.ICNT1         = filterLoopSize + remFilterLoopSize;
         se0Params.DIM1          = (int32_t) 2;
         se0Params.ICNT2         = dataLoopSize;
         se0Params.DIM2          = (int32_t) SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         se0Params.ICNT3         = batchSize;
         se0Params.DIM3          = dataBufferInPitch;
         se0Params.DECDIM1       = __SE_DECDIM_DIM2;
         se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM1;
         se0Params.DECDIM1_WIDTH = dataSize;
         se0Params.ELETYPE       = SE_ELETYPE;
         se0Params.VECLEN        = SE_VECLEN;
         se0Params.DIMFMT        = __SE_DIMFMT_4D;

         /**********************************************************************/
         /* Prepare streaming engine 2 to fetch data                           */
         /**********************************************************************/
         se1Params               = __gen_SE_TEMPLATE_v1();
         se1Params.ICNT0         = SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         se1Params.ICNT1         = filterLoopSize;
         se1Params.DIM1          = (int32_t) 2;
         se1Params.ICNT2         = dataLoopSize;
         se1Params.DIM2          = (int32_t) SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         se1Params.ICNT3         = batchSize;
         se1Params.DIM3          = dataBufferInPitch;
         se1Params.DECDIM1       = __SE_DECDIM_DIM2;
         se1Params.DECDIM1SD     = __SE_DECDIMSD_DIM1;
         se1Params.DECDIM1_WIDTH = dataSize - 1;
         se1Params.ELETYPE       = SE_ELETYPE;
         se1Params.VECLEN        = SE_VECLEN;
         se1Params.DIMFMT        = __SE_DIMFMT_4D;

         /**********************************************************************/
         /* Prepare SA template to store output                                */
         /**********************************************************************/
         sa0Params               = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0         = eleCount;
         sa0Params.ICNT1         = SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK;
         sa0Params.DIM1          = (int32_t) eleCount;
         sa0Params.ICNT2         = dataLoopSize;
         sa0Params.DIM2          = (int32_t) SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         sa0Params.ICNT3         = batchSize;
         sa0Params.DIM3          = dataBufferOutPitch;
         sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
         sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM1;
         sa0Params.DECDIM1_WIDTH = outputSize;
         sa0Params.VECLEN        = SA_VECLEN;
         sa0Params.DIMFMT        = __SA_DIMFMT_4D;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET) = sa2Params;
      }
      else if (flag == FIR_MODE_MEDIUM_FIR) {
         pKerPrivArgs->execute = DSPLIB_fir_mediumFir_exec_ci<dataType>;

         /*********************************************
          * Prepare SA template to read filter values *
          *********************************************/
         sa1Params        = __gen_SA_TEMPLATE_v1();
         sa1Params.ICNT0  = (uint32_t) 1;
         sa1Params.ICNT1  = (uint32_t) filterSize;
         sa1Params.DIM1   = -(int32_t) 1;
         sa1Params.ICNT2  = dataLoopSize;
         sa1Params.DIM2   = 0;
         sa1Params.ICNT3  = batchSize;
         sa1Params.DIM3   = (enableNchCoefs == 0) ? 0 : filterPitch;
         sa1Params.DIMFMT = __SA_DIMFMT_4D;
         sa1Params.VECLEN = __SA_VECLEN_1ELEM;

         /**********************************************************************/
         /* Prepare streaming engine 0 to fetch data                           */
         /**********************************************************************/
         se0Params               = __gen_SE_TEMPLATE_v1();
         se0Params.ICNT0         = SMALL_MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         se0Params.ICNT1         = filterSize;
         se0Params.DIM1          = (int32_t) 1;
         se0Params.ICNT2         = dataLoopSize;
         se0Params.DIM2          = (int32_t) MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         se0Params.ICNT3         = batchSize;
         se0Params.DIM3          = dataBufferInPitch;
         se0Params.DECDIM1       = __SE_DECDIM_DIM2;
         se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM1;
         se0Params.DECDIM1_WIDTH = dataSize;
         se0Params.ELETYPE       = SE_ELETYPE;
         se0Params.VECLEN        = SE_VECLEN;
         se0Params.DIMFMT        = __SE_DIMFMT_4D;

         /**********************************************************************/
         /* Prepare streaming engine 2 to fetch data                           */
         /**********************************************************************/
         se1Params = se0Params;
         se1Params.DECDIM1_WIDTH -= MEDIUM_FIR_OUT_SAMPLES_PER_BLOCK * eleCount; /* SE1 is advanced w.r.t. SE0 */

         /**********************************************************************/
         /* Prepare SA template to store output                                */
         /**********************************************************************/
         sa0Params               = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0         = eleCount;
         sa0Params.ICNT1         = MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK;
         sa0Params.DIM1          = (int32_t) eleCount;
         sa0Params.ICNT2         = dataLoopSize;
         sa0Params.DIM2          = (int32_t) MEDIUM_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         sa0Params.ICNT3         = batchSize;
         sa0Params.DIM3          = dataBufferOutPitch;
         sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
         sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM1;
         sa0Params.DECDIM1_WIDTH = outputSize;
         sa0Params.VECLEN        = SA_VECLEN;
         sa0Params.DIMFMT        = __SA_DIMFMT_4D;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
      }
      else if (flag == FIR_MODE_LARGE_FIR) {
         pKerPrivArgs->execute = DSPLIB_fir_largeFir_exec_ci<dataType>;

         /*********************************************
          * Prepare SA template to read filter values *
          *********************************************/
         sa1Params        = __gen_SA_TEMPLATE_v1();
         sa1Params.ICNT0  = (uint32_t) 1;
         sa1Params.ICNT1  = (uint32_t) filterSize;
         sa1Params.DIM1   = -(int32_t) 1;
         sa1Params.ICNT2  = dataLoopSize;
         sa1Params.DIM2   = 0;
         sa1Params.ICNT3  = batchSize;
         sa1Params.DIM3   = (enableNchCoefs == 0) ? 0 : filterPitch;
         sa1Params.DIMFMT = __SA_DIMFMT_4D;
         sa1Params.VECLEN = __SA_VECLEN_1ELEM;

         /**********************************************************************/
         /* Prepare streaming engine 0 to fetch data                           */
         /**********************************************************************/
         se0Params               = __gen_SE_TEMPLATE_v1();
         se0Params.ICNT0         = LONG_FIR_OUT_SAMPLES_PER_BLOCK * eleCount;
         se0Params.ICNT1         = filterSize;
         se0Params.DIM1          = (int32_t) 1;
         se0Params.ICNT2         = dataLoopSize;
         se0Params.DIM2          = (int32_t) LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         se0Params.ICNT3         = batchSize;
         se0Params.DIM3          = dataBufferInPitch;
         se0Params.DECDIM1       = __SE_DECDIM_DIM2;
         se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM1;
         se0Params.DECDIM1_WIDTH = dataSize;
         se0Params.ELETYPE       = SE_ELETYPE;
         se0Params.VECLEN        = SE_VECLEN;
         se0Params.DIMFMT        = __SE_DIMFMT_4D;

         /**********************************************************************/
         /* Prepare streaming engine 2 to fetch data                           */
         /**********************************************************************/
         se1Params = se0Params;
         se1Params.DECDIM1_WIDTH -= LONG_FIR_OUT_SAMPLES_PER_BLOCK * eleCount; /* SE1 is advanced w.r.t. SE0 */

         /**********************************************************************/
         /* Prepare SA template to store output                                */
         /**********************************************************************/
         sa0Params               = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0         = eleCount;
         sa0Params.ICNT1         = LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK;
         sa0Params.DIM1          = (int32_t) eleCount;
         sa0Params.ICNT2         = dataLoopSize;
         sa0Params.DIM2          = (int32_t) LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
         sa0Params.ICNT3         = batchSize;
         sa0Params.DIM3          = dataBufferOutPitch;
         sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
         sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM1;
         sa0Params.DECDIM1_WIDTH = outputSize;
         sa0Params.VECLEN        = SA_VECLEN;
         sa0Params.DIMFMT        = __SA_DIMFMT_4D;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
      }
      else if (flag == FIR_MODE_SMALL_FIR_SS) {
         pKerPrivArgs->execute = DSPLIB_fir_smallFirSS_exec_ci<dataType>;
         /**********************************************************************/
         /* Prepare streaming engine 1 to fetch the filter values              */
         /**********************************************************************/
         se1Params         = __gen_SE_TEMPLATE_v1();
         se1Params.ICNT0   = filterSize;
         se1Params.ICNT1   = dataLoopSize;
         se1Params.DIM1    = 0;
         se1Params.ICNT2   = batchSize;
         se1Params.DIM2    = (enableNchCoefs == 0) ? 0 : filterPitch;
         se1Params.ELETYPE = SE_ELETYPE;
         se1Params.VECLEN  = SE_VECLEN;
         se1Params.DIMFMT  = __SE_DIMFMT_3D;
         se1Params.DIR     = __SE_DIR_DEC;

         /**********************************************************************/
         /* Prepare streaming engine 0 to fetch data                           */
         /**********************************************************************/
         se0Params               = __gen_SE_TEMPLATE_v1();
         se0Params.ICNT0         = filterSize;
         se0Params.ICNT1         = dataLoopSize;
         se0Params.DIM1          = (int32_t) 1;
         se0Params.ICNT2         = batchSize;
         se0Params.DIM2          = dataBufferInPitch;
         se0Params.DECDIM1       = __SE_DECDIM_DIM1;
         se0Params.DECDIM1_WIDTH = dataSize;
         se0Params.ELETYPE       = SE_ELETYPE;
         se0Params.VECLEN        = SE_VECLEN;
         se0Params.DIMFMT        = __SE_DIMFMT_3D;

         /**********************************************************************/
         /* Prepare SA template to store output                                */
         /**********************************************************************/
         sa0Params        = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0  = 1;
         sa0Params.ICNT1  = dataLoopSize;
         sa0Params.DIM1   = 1;
         sa0Params.ICNT2  = batchSize;
         sa0Params.DIM2   = dataBufferOutPitch;
         sa0Params.VECLEN = SA_VECLEN;
         sa0Params.DIMFMT = __SA_DIMFMT_3D;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      }
      else {
         /* Do nothing here */
      }
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_fir_init_ci<int16_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams2D_t *bufParamsFilter,
                                                   const DSPLIB_bufParams2D_t *bufParamsOut,
                                                   const DSPLIB_fir_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_fir_init_ci<int32_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams2D_t *bufParamsFilter,
                                                   const DSPLIB_bufParams2D_t *bufParamsOut,
                                                   const DSPLIB_fir_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_fir_init_ci<int8_t>(DSPLIB_kernelHandle         handle,
                                                  const DSPLIB_bufParams2D_t *bufParamsIn,
                                                  const DSPLIB_bufParams2D_t *bufParamsFilter,
                                                  const DSPLIB_bufParams2D_t *bufParamsOut,
                                                  const DSPLIB_fir_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_fir_init_ci<float>(DSPLIB_kernelHandle         handle,
                                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                                 const DSPLIB_bufParams2D_t *bufParamsFilter,
                                                 const DSPLIB_bufParams2D_t *bufParamsOut,
                                                 const DSPLIB_fir_InitArgs  *pKerInitArgs);

/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/

/*
 * Small FIR with single sample - C7x execution
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_smallFirSS_exec_ci(DSPLIB_kernelHandle handle,
                                            void *restrict pIn,
                                            void *restrict pFilter,
                                            void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              batchSize    = pKerPrivArgs->initArgs.batchSize;

   int32_t dataLoopSize;
   int32_t filterLoopSize;
   int32_t remFilterLoopSize;

   dataType *pInLocal     = (dataType *) pIn;
   dataType *pFilterLocal = (dataType *) pFilter;
   dataType *pOutLocal    = (dataType *) pOut;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   vec      accumulator_0, accumulator_1, accumulator_2, accumulator_3;
   vec      accumulator_x, accumulator_y;
   dataType acc;

   int32_t filterSize = pKerPrivArgs->initArgs.filterSize;
   pFilterLocal       = pFilterLocal + filterSize;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pInLocal, se0Params);

   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pFilterLocal, se1Params);

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   dataLoopSize      = pKerPrivArgs->dataLoopSize;
   filterLoopSize    = pKerPrivArgs->filterLoopSize;
   remFilterLoopSize = pKerPrivArgs->remFilterLoopSize;

   for (int32_t batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
      for (int32_t dataLoopCount = 0; dataLoopCount < dataLoopSize; dataLoopCount++) {
         accumulator_0 = (vec) 0;
         accumulator_1 = (vec) 0;
         accumulator_2 = (vec) 0;
         accumulator_3 = (vec) 0;

         for (int32_t filterLoopCount = 0; filterLoopCount < filterLoopSize; filterLoopCount++) {
            vec tmp0 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp1 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_0 += tmp0 * tmp1;

            vec tmp2 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp3 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1 += tmp2 * tmp3;

            vec tmp4 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp5 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_2 += tmp4 * tmp5;

            vec tmp6 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp7 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_3 += tmp6 * tmp7;
         }

         for (int32_t filterLoopCount = 0; filterLoopCount < remFilterLoopSize; filterLoopCount++) {
            vec tmp0 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp1 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_0 += tmp0 * tmp1;
         }

         accumulator_x = accumulator_0 + accumulator_1;
         accumulator_y = accumulator_2 + accumulator_3;
         accumulator_x = accumulator_x + accumulator_y;

         /* horizontal add */
         accumulator_x.lo()      = accumulator_x.hi() + accumulator_x.lo();
         accumulator_x.lo().lo() = accumulator_x.lo().hi() + accumulator_x.lo().lo();
#if defined(__C7100__) || defined(__C7120__)
         accumulator_x.lo().lo().lo() = accumulator_x.lo().lo().hi() + accumulator_x.lo().lo().lo();
#endif
         acc = (float) accumulator_x.s[0] + (float) accumulator_x.s[1];

         /* write output */
         __vpred   tmp = c7x::strm_agen<0, dataType>::get_vpred();
         dataType *VB1 = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, acc);
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

/*
 * Small FIR - C7x execution
 */
template <typename dataType>
DSPLIB_STATUS
DSPLIB_fir_smallFir_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              batchSize    = pKerPrivArgs->initArgs.batchSize;

   int32_t dataLoopSize;
   int32_t filterLoopSize;
   int32_t remFilterLoopSize;

   dataType *pInLocal     = (dataType *) pIn;
   dataType *pFilterLocal = (dataType *) pFilter;
   dataType *pOutLocal    = (dataType *) pOut;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   vec accumulator_0, accumulator_1, accumulator_2, accumulator_3;
   vec accumulator_x, accumulator_y;

   int32_t filterSize = pKerPrivArgs->initArgs.filterSize;
   pFilterLocal       = pFilterLocal + filterSize;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pInLocal, se0Params);

   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pFilterLocal, se1Params);

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   dataLoopSize      = pKerPrivArgs->dataLoopSize;
   filterLoopSize    = pKerPrivArgs->filterLoopSize;
   remFilterLoopSize = pKerPrivArgs->remFilterLoopSize;

   for (int32_t batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
      for (int32_t dataLoopCount = 0; dataLoopCount < dataLoopSize; dataLoopCount++) {
         accumulator_0 = (vec) 0;
         accumulator_1 = (vec) 0;
         accumulator_2 = (vec) 0;
         accumulator_3 = (vec) 0;

         for (int32_t filterLoopCount = 0; filterLoopCount < filterLoopSize; filterLoopCount++) {
            vec tmp0 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp1 = c7x::strm_eng<1, vec>::get_adv();
#if DSPLIB_DEBUGPRINT
#endif
            accumulator_0 += tmp0 * tmp1;

            vec tmp2 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp3 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1 += tmp2 * tmp3;

            vec tmp4 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp5 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_2 += tmp4 * tmp5;

            vec tmp6 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp7 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_3 += tmp6 * tmp7;
         }

         for (int32_t filterLoopCount = 0; filterLoopCount < remFilterLoopSize; filterLoopCount++) {
            vec tmp0 = c7x::strm_eng<0, vec>::get_adv();
            vec tmp1 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_0 += tmp0 * tmp1;
         }

         accumulator_x = accumulator_0 + accumulator_1;
         accumulator_y = accumulator_2 + accumulator_3;
         accumulator_x = accumulator_x + accumulator_y;
         __vpred tmp   = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1   = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_x);

#if DSPLIB_DEBUGPRINT
         DSPLIB_debugPrintVector(accumulator_x);
#endif
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

/*
 * Small Medium FIR - C7x execution
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_smallMediumFir_exec_ci(DSPLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pFilter,
                                                void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;

   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              batchSize    = pKerPrivArgs->initArgs.batchSize;

   int32_t dataLoopSize;
   int32_t filterLoopSize;
   int32_t remFilterLoopSize;

   dataType *pInLocal      = (dataType *) pIn;
   dataType *pFilterLocal0 = (dataType *) pFilter;
   dataType *pOutLocal     = (dataType *) pOut;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   vec accumulator_0;
   vec accumulator_1;
   vec accumulator_2;
   vec accumulator_3;
   vec accumulator_4;
   vec accumulator_5;
   vec accumulator_6;
   vec accumulator_7;

   int32_t filterSize      = pKerPrivArgs->initArgs.filterSize;
   pFilterLocal0           = pFilterLocal0 + filterSize - 1;
   dataType *pFilterLocal1 = pFilterLocal0 - 1;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pInLocal, se0Params);

   pInLocal  = pInLocal + 1;
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pInLocal, se1Params);

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   /* Filter coefficient 0 */
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   /* Filter coefficient 1 */
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET);
   __SA2_OPEN(sa2Params);

   dataLoopSize      = pKerPrivArgs->dataLoopSize;
   filterLoopSize    = pKerPrivArgs->filterLoopSize;
   remFilterLoopSize = pKerPrivArgs->remFilterLoopSize;

   for (int32_t batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
      for (int32_t dataLoopCount = 0; dataLoopCount < dataLoopSize; dataLoopCount++) {
         accumulator_0 = (vec) 0;
         accumulator_1 = (vec) 0;
         accumulator_2 = (vec) 0;
         accumulator_3 = (vec) 0;
         accumulator_4 = (vec) 0;
         accumulator_5 = (vec) 0;
         accumulator_6 = (vec) 0;
         accumulator_7 = (vec) 0;

         for (int32_t filterLoopCount = 0; filterLoopCount < filterLoopSize; filterLoopCount++) {
            dataType *VB1Dup0 = c7x::strm_agen<1, dataType>::get_adv(pFilterLocal0);
            vec       flt0    = __vload_dup(VB1Dup0);
            dataType *VB1Dup1 = c7x::strm_agen<2, dataType>::get_adv(pFilterLocal1);
            vec       flt1    = __vload_dup(VB1Dup1);

            vec tmp0_0 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0 += tmp0_0 * flt0;

            vec tmp0_1 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_1 += tmp0_1 * flt0;

            vec tmp0_2 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_2 += tmp0_2 * flt0;

            vec tmp0_3 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_3 += tmp0_3 * flt0;

            vec tmp0_4 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_4 += tmp0_4 * flt1;

            vec tmp0_5 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_5 += tmp0_5 * flt1;

            vec tmp0_6 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_6 += tmp0_6 * flt1;

            vec tmp0_7 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_7 += tmp0_7 * flt1;
         }

         if (remFilterLoopSize > 0) {
            dataType *VB1Dup0 = c7x::strm_agen<1, dataType>::get_adv(pFilterLocal0);
            vec       flt0    = __vload_dup(VB1Dup0);

            vec tmp0_0 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0 += tmp0_0 * flt0;

            vec tmp0_1 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_1 += tmp0_1 * flt0;

            vec tmp0_2 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_2 += tmp0_2 * flt0;

            vec tmp0_3 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_3 += tmp0_3 * flt0;
         }

         accumulator_0 += accumulator_4;
         accumulator_1 += accumulator_5;
         accumulator_2 += accumulator_6;
         accumulator_3 += accumulator_7;

         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_2);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_3);
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();

   return DSPLIB_SUCCESS;
}

/*
 * Medium FIR - C7x execution
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_mediumFir_exec_ci(DSPLIB_kernelHandle handle,
                                           void *restrict pIn,
                                           void *restrict pFilter,
                                           void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              batchSize    = pKerPrivArgs->initArgs.batchSize;

   int32_t dataLoopSize;
   int32_t filterLoopSize;

   dataType *pInLocal     = (dataType *) pIn;
   dataType *pFilterLocal = (dataType *) pFilter;
   dataType *pOutLocal    = (dataType *) pOut;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int                                                    eleCount = c7x::element_count_of<vec>::value;

   vec accumulator_0;
   vec accumulator_1;
   vec accumulator_2;
   vec accumulator_3;
   vec accumulator_4;
   vec accumulator_5;
   vec accumulator_6;
   vec accumulator_7;

   int32_t filterSize = pKerPrivArgs->initArgs.filterSize;
   pFilterLocal       = pFilterLocal + filterSize - 1;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pInLocal, se0Params);

   pInLocal  = pInLocal + MEDIUM_FIR_OUT_SAMPLES_PER_BLOCK * eleCount;
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pInLocal, se1Params);

   /* Filter coefficient */
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   dataLoopSize   = pKerPrivArgs->dataLoopSize;
   filterLoopSize = pKerPrivArgs->filterLoopSize;

   for (int32_t batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
      for (int32_t dataLoopCount = 0; dataLoopCount < dataLoopSize; dataLoopCount++) {
         accumulator_0 = (vec) 0;
         accumulator_1 = (vec) 0;
         accumulator_2 = (vec) 0;
         accumulator_3 = (vec) 0;
         accumulator_4 = (vec) 0;
         accumulator_5 = (vec) 0;
         accumulator_6 = (vec) 0;
         accumulator_7 = (vec) 0;

#pragma MUST_ITERATE(1, , 1)
         for (int32_t filterLoopCount = 0; filterLoopCount < filterLoopSize; filterLoopCount++) {
            dataType *VB1Dup = c7x::strm_agen<1, dataType>::get_adv(pFilterLocal);
            vec       flt    = __vload_dup(VB1Dup);

            vec tmp0_0 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0 += tmp0_0 * flt;

            vec tmp0_1 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_1 += tmp0_1 * flt;

            vec tmp0_2 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_2 += tmp0_2 * flt;

            vec tmp0_3 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_3 += tmp0_3 * flt;

            vec tmp0_4 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_4 += tmp0_4 * flt;

            vec tmp0_5 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_5 += tmp0_5 * flt;

            vec tmp0_6 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_6 += tmp0_6 * flt;

            vec tmp0_7 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_7 += tmp0_7 * flt;
         }

         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_2);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_3);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_4);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_5);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_6);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_7);
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   return DSPLIB_SUCCESS;
}

/*
 * Large FIR - C7x execution
 */
template <typename dataType>
DSPLIB_STATUS
DSPLIB_fir_largeFir_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              batchSize    = pKerPrivArgs->initArgs.batchSize;

   int32_t dataLoopSize;
   int32_t filterLoopSize;

   dataType *pInLocal     = (dataType *) pIn;
   dataType *pFilterLocal = (dataType *) pFilter;
   dataType *pOutLocal    = (dataType *) pOut;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int                                                    eleCount = c7x::element_count_of<vec>::value;

   vec accumulator_0_0;
   vec accumulator_0_1;
   vec accumulator_0_2;
   vec accumulator_0_3;
   vec accumulator_0_4;
   vec accumulator_0_5;
   vec accumulator_0_6;
   vec accumulator_0_7;
   vec accumulator_1_0;
   vec accumulator_1_1;
   vec accumulator_1_2;
   vec accumulator_1_3;
   vec accumulator_1_4;
   vec accumulator_1_5;
   vec accumulator_1_6;
   vec accumulator_1_7;

   int32_t filterSize = pKerPrivArgs->initArgs.filterSize;
   pFilterLocal       = pFilterLocal + filterSize - 1;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pInLocal, se0Params);

   pInLocal  = pInLocal + 8 * eleCount;
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pInLocal, se1Params);

   /* Filter coefficient */
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   dataLoopSize   = pKerPrivArgs->dataLoopSize;
   filterLoopSize = pKerPrivArgs->filterLoopSize;

   for (int32_t batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
      for (int32_t dataLoopCount = 0; dataLoopCount < dataLoopSize; dataLoopCount++) {
         accumulator_0_0 = (vec) 0;
         accumulator_0_1 = (vec) 0;
         accumulator_0_2 = (vec) 0;
         accumulator_0_3 = (vec) 0;
         accumulator_0_4 = (vec) 0;
         accumulator_0_5 = (vec) 0;
         accumulator_0_6 = (vec) 0;
         accumulator_0_7 = (vec) 0;
         accumulator_1_0 = (vec) 0;
         accumulator_1_1 = (vec) 0;
         accumulator_1_2 = (vec) 0;
         accumulator_1_3 = (vec) 0;
         accumulator_1_4 = (vec) 0;
         accumulator_1_5 = (vec) 0;
         accumulator_1_6 = (vec) 0;
         accumulator_1_7 = (vec) 0;

         for (int32_t filterLoopCount = 0; filterLoopCount < filterLoopSize; filterLoopCount++) {
            dataType *VB1Dup = c7x::strm_agen<1, dataType>::get_adv(pFilterLocal);
            vec       flt    = __vload_dup(VB1Dup);

            vec tmp0_0 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0_0 += tmp0_0 * flt;

            vec tmp0_1 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0_1 += tmp0_1 * flt;

            vec tmp0_2 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0_2 += tmp0_2 * flt;

            vec tmp0_3 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0_3 += tmp0_3 * flt;

            vec tmp0_4 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0_4 += tmp0_4 * flt;

            vec tmp0_5 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0_5 += tmp0_5 * flt;

            vec tmp0_6 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0_6 += tmp0_6 * flt;

            vec tmp0_7 = c7x::strm_eng<0, vec>::get_adv();
            accumulator_0_7 += tmp0_7 * flt;

            vec tmp1_0 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1_0 += tmp1_0 * flt;

            vec tmp1_1 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1_1 += tmp1_1 * flt;

            vec tmp1_2 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1_2 += tmp1_2 * flt;

            vec tmp1_3 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1_3 += tmp1_3 * flt;

            vec tmp1_4 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1_4 += tmp1_4 * flt;

            vec tmp1_5 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1_5 += tmp1_5 * flt;

            vec tmp1_6 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1_6 += tmp1_6 * flt;

            vec tmp1_7 = c7x::strm_eng<1, vec>::get_adv();
            accumulator_1_7 += tmp1_7 * flt;
#if DSPLIB_DEBUGPRINT
#endif
         }
         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0_0);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0_1);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0_2);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0_3);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0_4);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0_5);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0_6);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_0_7);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1_0);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1_1);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1_2);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1_3);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1_4);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1_5);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1_6);

         tmp = c7x::strm_agen<0, vec>::get_vpred();
         VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, accumulator_1_7);

#if DSPLIB_DEBUGPRINT
#endif
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   return DSPLIB_SUCCESS;
}

/*
 * MMA execution variant
 */
template <typename dataType>
DSPLIB_STATUS
DSPLIB_fir_MMA_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              batchSize    = pKerPrivArgs->initArgs.batchSize;
   int32_t              filterSize   = pKerPrivArgs->initArgs.filterSize;

   dataType *pInLocal     = (dataType *) pIn;
   dataType *pFilterLocal = (dataType *) pFilter;
   dataType *pOutLocal    = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_fir_MMA_exec_ci\n");
#endif

   int32_t MMA_SIZE = pKerPrivArgs->initArgs.MMA_SIZE;

   /* Retrieve SE/SA and MMA Config Params */
   __HWA_CONFIG_REG_v1 mmaConfigReg = *((__HWA_CONFIG_REG_v1 *) ((uint8_t *) pBlock + MMA_CONFIGREG_OFFSET));
   __HWA_OFFSET_REG    mmaOffsetReg = *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock + MMA_OFFSETREG_OFFSET));

   se0Params = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET));
   se1Params = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET));
   sa0Params = *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET));

   int32_t filterOffset   = *((int32_t *) ((uint8_t *) pBlock + MMA_FILTER_OFFSET));
   int32_t filterLoopSize = pKerPrivArgs->filterLoopSize;
   int32_t dataLoopSize   = pKerPrivArgs->dataLoopSize;

   /* Set filter pointer to last element after zero padding */
   pFilterLocal = &pFilterLocal[filterOffset + filterSize - 1];

   __SE0_OPEN((void *) pInLocal, se0Params);
   __SE1_OPEN((void *) pFilterLocal, se1Params);
   __SA0_OPEN(sa0Params);
   __HWAOPEN(mmaConfigReg, mmaOffsetReg, __MMA_OPEN_FSM_RESET);

   __HWAADV();

   // load Bback(0, 0)
   DSPLIB_UTIL_SE1Bload<dataType, 1>(MMA_SIZE);

   // swap Bback(0, 0) to Bfore(0, 0)
   // load Afore(0, 0)
   // load Bback(0, 1)
   // OP0 (Period = MMA_SIZE): compute Cfore(0, 0) = Afore(0, 0) * Bfore(0, 0)
   // swap operation from OP0 to OP1
   // OP1 (Period = (filterLoopSize - 1) * MMA_SIZE): compute Cfore(0, 0) += Afore(0, 0) * Bfore(0, 0)
   DSPLIB_UTIL_SE0AloadSE1BloadComputeC<dataType, 1>(filterLoopSize * MMA_SIZE);

   // swap Cfore(0, 0) to Cback(0, 0)
   // swap Bback(0, 1) to Bfore(0, 1)
   // load Afore(0, 1)
   // load Bback(0, 2)
   // compute Cfore(0, 1) = Afore(0, 1) * Bfore(0, 1)
   // swap operation OP0 to OP1
   // compute Cfore(0, 1) += Afore(0, 1) * Bfore(0, 1)
   // transfer Cback(0, 0) to Xfore(0, 0)
   // store Xfore(0, 0)
   DSPLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore<dataType, 1>(batchSize * dataLoopSize * filterLoopSize * MMA_SIZE,
                                                              (dataType *) pOutLocal);

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __HWACLOSE(0);

   return DSPLIB_SUCCESS;
}

/*
 * MMA execution variant - float specialization
 */
#if !defined(__C7X_MMA_2_256F__)
template <>
DSPLIB_STATUS DSPLIB_fir_MMA_exec_ci<float>(DSPLIB_kernelHandle handle,
                                            void *restrict pIn,
                                            void *restrict pFilter,
                                            void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              batchSize    = pKerPrivArgs->initArgs.batchSize;
   int32_t              filterSize   = pKerPrivArgs->initArgs.filterSize;

   float *pInLocal     = (float *) pIn;
   float *pFilterLocal = (float *) pFilter;
   float *pOutLocal    = (float *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_fir_MMA_exec_ci<float>\n");
#endif

   typedef typename c7x::make_full_vector<float>::type vec;

   uint32_t Q        = pKerPrivArgs->initArgs.Q;
   int32_t  MMA_SIZE = pKerPrivArgs->initArgs.MMA_SIZE;

   __HWA_CONFIG_REG_v1 mmaConfigReg = *((__HWA_CONFIG_REG_v1 *) ((uint8_t *) pBlock + MMA_CONFIGREG_OFFSET));
   __HWA_OFFSET_REG    mmaOffsetReg = *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock + MMA_OFFSETREG_OFFSET));

   se0Params            = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET));
   se1Params            = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET));
   sa0Params            = *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET));
   int32_t filterOffset = *((int32_t *) ((uint8_t *) pBlock + MMA_FILTER_OFFSET));

   int32_t filterLoopSize = pKerPrivArgs->filterLoopSize;
   int32_t dataLoopSize   = pKerPrivArgs->dataLoopSize;

   /* Set filter pointer to last element after zero padding */
   pFilterLocal = &pFilterLocal[filterOffset + filterSize - 1];

   float QFactor  = (1U << Q);
   vec   vQFactor = (vec) QFactor;

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pFilterLocal, se1Params);
   __SA0_OPEN(sa0Params);
   __HWAOPEN(mmaConfigReg, mmaOffsetReg, __MMA_OPEN_FSM_RESET);

   __HWAADV();

   DSPLIB_UTIL_Q_SE1Bload<1>(MMA_SIZE, vQFactor);

   DSPLIB_UTIL_Q_SE0AloadSE1BloadComputeC<1>(filterLoopSize * MMA_SIZE, vQFactor);

   DSPLIB_UTIL_Q_SE0AloadSE1BloadComputeCSA0CstoreUnroll3(batchSize * dataLoopSize * filterLoopSize * MMA_SIZE,
                                                          vQFactor, pOutLocal);

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __HWACLOSE(0);

   return DSPLIB_SUCCESS;
}

#else
template DSPLIB_STATUS DSPLIB_fir_MMA_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn,
                                                     void *restrict pFilter,
                                                     void *restrict pOut);

#endif // #if !defined(__C7X_MMA_2_256F__)

/* Template instantiations for MMA execution variant */
template DSPLIB_STATUS DSPLIB_fir_MMA_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn,
                                                       void *restrict pFilter,
                                                       void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_MMA_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn,
                                                       void *restrict pFilter,
                                                       void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_MMA_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn,
                                                      void *restrict pFilter,
                                                      void *restrict pOut);

/* Template instantiations for the three C7x execution variants */
template DSPLIB_STATUS DSPLIB_fir_smallFirSS_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pFilter,
                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallFirSS_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                              void *restrict pIn,
                                                              void *restrict pFilter,
                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallFirSS_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                              void *restrict pIn,
                                                              void *restrict pFilter,
                                                              void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallFirSS_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                             void *restrict pIn,
                                                             void *restrict pFilter,
                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallFir_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                          void *restrict pIn,
                                                          void *restrict pFilter,
                                                          void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallFir_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pFilter,
                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallFir_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pFilter,
                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallFir_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn,
                                                           void *restrict pFilter,
                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallMediumFir_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                void *restrict pIn,
                                                                void *restrict pFilter,
                                                                void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallMediumFir_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                                  void *restrict pIn,
                                                                  void *restrict pFilter,
                                                                  void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallMediumFir_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                                  void *restrict pIn,
                                                                  void *restrict pFilter,
                                                                  void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_smallMediumFir_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                                 void *restrict pIn,
                                                                 void *restrict pFilter,
                                                                 void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_mediumFir_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn,
                                                           void *restrict pFilter,
                                                           void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_mediumFir_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                             void *restrict pIn,
                                                             void *restrict pFilter,
                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_mediumFir_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                             void *restrict pIn,
                                                             void *restrict pFilter,
                                                             void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_mediumFir_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pFilter,
                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_largeFir_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                          void *restrict pIn,
                                                          void *restrict pFilter,
                                                          void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_largeFir_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pFilter,
                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_largeFir_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pFilter,
                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_largeFir_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn,
                                                           void *restrict pFilter,
                                                           void *restrict pOut);
