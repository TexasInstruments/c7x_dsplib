// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0


/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/
#include "DSPLIB_complexMatrixMultiply_priv.h"
#include "../common/c71/DSPLIB_inlines.h"
#include "../common/c71/init_mma.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define SE_PARAM_BASE (0x0000)

#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)

#define MMA_PARAM_BASE (SE_SA0_PARAM_OFFSET + SE_PARAM_SIZE)

#define MMA_CONFIGREG_OFFSET (MMA_PARAM_BASE)
#define MMA_OFFSETREG_OFFSET (MMA_CONFIGREG_OFFSET + sizeof(__HWA_CONFIG_REG_v1))

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DSPLIB_complexMatrixMultiply_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "DSPLIB_utility.h"

template <typename dType>
DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init_ci(DSPLIB_kernelHandle handle,
                                                       const DSPLIB_bufParams3D_t *bufParamsInA,
                                                       const DSPLIB_bufParams3D_t *bufParamsInB,
                                                       const DSPLIB_bufParams3D_t *bufParamsOut,
                                                       const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs)
{
   // clear sticky fields from parity checker
   init_mma();

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   // store parameters
   DSPLIB_complexMatrixMultiply_PrivArgs *pKerPrivArgs =
       (DSPLIB_complexMatrixMultiply_PrivArgs *)handle;

   typedef typename c7x::make_full_vector<dType>::type vec_t;

   // dataType dependent values
   uint32_t MMA_SIZE;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec_t>::value;
   __SE_VECLEN SE_VECLEN = c7x::se_veclen<vec_t>::value;

   uint32_t MMA_TYPE_BYTEWIDTH = sizeof(dType);
   __MMA_A_CONFIG_ATYPE ATYPE;
   __MMA_C_CONFIG_BTYPE BTYPE;
   __MMA_X_CONFIG_XTYPE XTYPE;
   __HWA_CONFIG_REG_v1 mmaConfig;

   // if/else structure removed by compiler as conditions are all known at compile time and only one is valid per template value
   if (std::is_same<dType, int32_t>::value)
   {
      MMA_SIZE = DSPLIB_MMA_SIZE_32_BIT;
      ATYPE = __MMA_A_CONFIG_ATYPE_INT32;
      BTYPE = __MMA_C_CONFIG_BTYPE_INT32;
      XTYPE = __MMA_X_CONFIG_XTYPE_INT32;

      // initialize the config to one of the common configurations
      mmaConfig = configRegisterStruct_i32s_i32s_o32s;
   }
   else if (std::is_same<dType, int16_t>::value)
   {
      MMA_SIZE = DSPLIB_MMA_SIZE_16_BIT;
      ATYPE = __MMA_A_CONFIG_ATYPE_INT16;
      BTYPE = __MMA_C_CONFIG_BTYPE_INT16;
      XTYPE = __MMA_X_CONFIG_XTYPE_INT16;

      // initialize the config to one of the common configurations
      mmaConfig = configRegisterStruct_i16s_i16s_o16s;
   }
   else
   {
      status = DSPLIB_ERR_INVALID_TYPE;
   }

   if (status != DSPLIB_SUCCESS)
   {
      return status;
   }

   /*************************************************************************/
   /* Matrix multiply cases                                                      */
   /*************************************************************************/
   int32_t M = DSPLIB_ceilingDiv(bufParamsInA->dim_y, MMA_SIZE);
   int32_t N = DSPLIB_ceilingDiv(bufParamsInB->dim_x, MMA_SIZE);
   int32_t K = DSPLIB_ceilingDiv(bufParamsInA->dim_x * 2, MMA_SIZE); // complex samples
   int32_t nBatch = pKerPrivArgs->nBatch;

   int32_t strideA = pKerPrivArgs->strideA;
   int32_t strideB = pKerPrivArgs->strideB;
   int32_t bStrideA = pKerPrivArgs->batchStrideA;
   int32_t bStrideB = pKerPrivArgs->batchStrideB;

   /*************************************************************************/
   /* Allocate SE/SA parameters and flags                                   */
   /*************************************************************************/
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   /*************************************************************************/
   /* Prepare streaming engine to fetch A matrix input                      */
   /*************************************************************************/
   se0Params.DIMFMT = __SE_DIMFMT_6D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN = SE_VECLEN;
   se0Params.DECDIM1 = __SE_DECDIM_DIM2;
   se0Params.DECDIM2 = __SE_DECDIM_DIM4;
   se0Params.DECDIM1SD = __SE_DECDIMSD_DIM0;
   se0Params.DECDIM2SD = __SE_DECDIMSD_DIM1;
   se0Params.DECDIM1_WIDTH = (uint32_t)bufParamsInA->dim_x * 2;
   se0Params.DECDIM2_WIDTH = (uint32_t)bufParamsInA->dim_y * strideA;

   se0Params.ICNT0 = (uint32_t)MMA_SIZE;
   se0Params.ICNT1 = (uint32_t)MMA_SIZE;
   se0Params.DIM1 = strideA;
   se0Params.ICNT2 = (uint32_t)K;
   se0Params.DIM2 = (int32_t)MMA_SIZE;
   se0Params.ICNT3 = (uint32_t)2 * N;
   se0Params.DIM3 = (int32_t)0;
   se0Params.ICNT4 = (uint32_t)M;
   se0Params.DIM4 = (int32_t)MMA_SIZE * strideA;
   se0Params.ICNT5 = nBatch;
   se0Params.DIM5 = bStrideA;

   /*************************************************************************/
   /* Prepare streaming engine to fetch B matrix input                      */
   /*************************************************************************/
   se1Params.DIMFMT = __SE_DIMFMT_6D;
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN = SE_VECLEN;
   se1Params.DECDIM1 = __SE_DECDIM_DIM3;
   se1Params.DECDIM2 = __SE_DECDIM_DIM2;
   se1Params.DECDIM1SD = __SE_DECDIMSD_DIM0;
   se1Params.DECDIM2SD = __SE_DECDIMSD_DIM1;
   se1Params.DECDIM2_WIDTH = (uint32_t)bufParamsInB->dim_y * strideB;
   se1Params.DECDIM1_WIDTH = (uint32_t)bufParamsInB->dim_x * 2;

   // The algorithm requires the B matrix to be loaded at half the rate of the A matrix
   const uint32_t complexBFactor = 2;

   se1Params.ICNT0 = (uint32_t)MMA_SIZE;
   se1Params.ICNT1 = (uint32_t)MMA_SIZE / complexBFactor;
   se1Params.DIM1 = (int32_t)strideB;
   se1Params.ICNT2 = (uint32_t)K;
   se1Params.DIM2 = (int32_t)MMA_SIZE * strideB / complexBFactor;
   se1Params.ICNT3 = (uint32_t)2 * N;
   se1Params.DIM3 = (int32_t)MMA_SIZE;
   se1Params.ICNT4 = (uint32_t)M;
   se1Params.DIM4 = (int32_t)0;
   se1Params.ICNT5 = nBatch;
   se1Params.DIM5 = bStrideB;

   /*************************************************************************/
   /* Prepare streaming address generator to store C matrix output           */
   /*************************************************************************/
   // SA flags for storing the C matrix
   sa0Params.VECLEN = c7x::sa_veclen<c7x::char_vec>::value;
   sa0Params.DIMFMT = __SA_DIMFMT_6D;
   sa0Params.DECDIM2 = __SA_DECDIM_DIM4;
   sa0Params.DECDIM3 = __SA_DECDIM_DIM3;
   sa0Params.DECDIM1SD = __SA_DECDIMSD_DIM0;
   sa0Params.DECDIM2SD = __SA_DECDIMSD_DIM1;
   sa0Params.DECDIM2_WIDTH = (uint32_t)bufParamsOut->dim_y * bufParamsOut->stride_y;
   sa0Params.DECDIM3_WIDTH = (uint32_t)bufParamsOut->dim_x * MMA_TYPE_BYTEWIDTH * 2;

   sa0Params.ICNT0 = (uint32_t)DSPLIB_BYTE_WIDTH;
   sa0Params.ICNT1 = (uint32_t)MMA_SIZE;
   sa0Params.DIM1 = (int32_t)bufParamsOut->stride_y;
   sa0Params.ICNT2 = (uint32_t)K;
   sa0Params.DIM2 = (int32_t)0;
   sa0Params.ICNT3 = (uint32_t)2 * N;
   sa0Params.DIM3 = (int32_t)DSPLIB_BYTE_WIDTH;
   sa0Params.ICNT4 = (uint32_t)M * 1;
   sa0Params.DIM4 = (int32_t)MMA_SIZE * bufParamsOut->stride_y;
   sa0Params.ICNT5 = nBatch;
   sa0Params.DIM5 = bufParamsOut->stride_z;

   /*************************************************************************/
   /* Prepare the MMA for C = A*B                                           */
   /*************************************************************************/

   // initialize the offset register
   __HWA_OFFSET_REG mmaOffset = offsetRegStruct_zeros;

   // update data dependent configuration parameters
   mmaConfig.A_ATYPE = ATYPE;
   mmaConfig.C_BTYPE = BTYPE;

   mmaConfig.C_OP1PER = (K - 1) * MMA_SIZE;
   mmaConfig.C_CRSWPER = K * MMA_SIZE;
   mmaConfig.C_CWSWPER = K * MMA_SIZE;
   mmaConfig.X_CSTART = 1;
   mmaConfig.X_CSWPER = K * MMA_SIZE;

   mmaConfig.X_XTYPE = XTYPE;
   mmaConfig.C_ATYPE = __MMA_C_CONFIG_ATYPE_SA; // CATYPE;

   mmaConfig.X_SAT = 0x1; // saturate
   if (std::is_same<dType, int16_t>::value)
   {
      mmaConfig.X_SHIFT = __MMA_X_CONFIG_SHIFT_ROW_SIGNED;
      mmaConfig.X_SCALE_SHIFT_CTRL = __MMA_X_CONFIG_SCALE_SHIFT_CTRL_ENABLE;
   }
   else
   {
      mmaConfig.X_SHIFT = pKerPrivArgs->pKerInitArgs.params.shift;
   }
   pKerPrivArgs->M = M;
   pKerPrivArgs->K = K;
   pKerPrivArgs->N = N;
   pKerPrivArgs->MMA_SIZE = MMA_SIZE;

#if DSPLIB_DEBUGPRINT
   printf("Init: M %d K %d N %d MMA_SIZE %d\n", M, K, N, MMA_SIZE);
#endif

   *(__SE_TEMPLATE_v1 *)((uint8_t *)pKerPrivArgs->bufPblock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *)((uint8_t *)pKerPrivArgs->bufPblock + SE_SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *)((uint8_t *)pKerPrivArgs->bufPblock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *((__HWA_CONFIG_REG_v1 *)((uint8_t *)pKerPrivArgs->bufPblock + MMA_CONFIGREG_OFFSET)) = mmaConfig;
   *((__HWA_OFFSET_REG *)((uint8_t *)pKerPrivArgs->bufPblock + MMA_OFFSETREG_OFFSET)) = mmaOffset;

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for DSPLIB_complexMatrixMultiply_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/
template DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                                         const DSPLIB_bufParams3D_t *bufParamsInA,
                                                                         const DSPLIB_bufParams3D_t *bufParamsInB,
                                                                         const DSPLIB_bufParams3D_t *bufParamsOut,
                                                                         const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_complexMatrixMultiply_init_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                                         const DSPLIB_bufParams3D_t *bufParamsInA,
                                                                         const DSPLIB_bufParams3D_t *bufParamsInB,
                                                                         const DSPLIB_bufParams3D_t *bufParamsOut,
                                                                         const DSPLIB_complexMatrixMultiply_InitArgs *pKerInitArgs);

void DSPLIB_complexMatrixMultiply_perfEst(
    DSPLIB_kernelHandle handle,
    const DSPLIB_bufParams3D_t *bufParamsInA,
    const DSPLIB_bufParams3D_t *bufParamsInB,
    uint64_t *archCycles,
    uint64_t *estCycles)
{
   DSPLIB_complexMatrixMultiply_PrivArgs *pKerPrivArgs =
       (DSPLIB_complexMatrixMultiply_PrivArgs *)handle;
   /* determine the mmaWidth based upon output data type */
   int32_t MMA_SIZE = pKerPrivArgs->MMA_SIZE;

   int32_t M = DSPLIB_ceilingDiv(bufParamsInA->dim_y, MMA_SIZE);
   int32_t K = DSPLIB_ceilingDiv(bufParamsInA->dim_x * 2, MMA_SIZE);
   int32_t N = DSPLIB_ceilingDiv(bufParamsInB->dim_x, MMA_SIZE);
   int32_t nBatch = bufParamsInB->dim_z;

   uint64_t startupCycles = 0;
   uint64_t teardownCycles = 0;
   uint64_t computeCycles = 0;

   startupCycles =
       10 + // Parameters
       5 +  // MMALIB_STATUS MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_exec
            // overhead
       5 +  // kernel function call
       5 +  // load SE1 params
       17 + // SEOPEN latency on SE1 (the first SE needed)
       5;   // SA0 and SA1 open cycles

   teardownCycles = 3 + // SE/SA/MMA closes
                    3;  // stack operations

   computeCycles += MMA_SIZE + 3;                  // BLOAD
   computeCycles += MMA_SIZE + 22;                 // // ALOAD, BLOAD, COMPUTE
   computeCycles += (uint64_t)nBatch * N * M * K * MMA_SIZE * 2 + 23; // ALOAD, BLOAD, COMPUTE, STORE

   *archCycles = startupCycles + computeCycles + teardownCycles;
   *estCycles = startupCycles + computeCycles + teardownCycles;

   return;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DSPLIB_complexMatrixMultiply_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/
template <typename dType, uint32_t MMA_SIZE>
DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_ci(DSPLIB_kernelHandle handle,
                                                       const void *restrict pInA,
                                                       const void *restrict pInB,
                                                       void *restrict pOut)
{
   // load private arguments into variables
   DSPLIB_complexMatrixMultiply_PrivArgs *pKerPrivArgs =
       (DSPLIB_complexMatrixMultiply_PrivArgs *)handle;

   int32_t M = pKerPrivArgs->M;
   int32_t K = pKerPrivArgs->K;
   int32_t N = pKerPrivArgs->N;
   int32_t nBatch = pKerPrivArgs->nBatch;
   // int32_t MMA_SIZE = pKerPrivArgs->MMA_SIZE;

   int32_t r;

   const int16_t shift = pKerPrivArgs->pKerInitArgs.params.shift;
   const int16_t scaleValue = pKerPrivArgs->pKerInitArgs.params.scaleValue;

#if DSPLIB_DEBUGPRINT
   printf("M %d K %d N %d MMA_SIZE %u\n", M, K, N, MMA_SIZE);
#endif

   /*************************************************************************/
   /* Allocate SE/SA parameters and flags                                   */
   /*************************************************************************/
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1(); // SE parameter vector
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1(); // SE parameter vector
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   se0Params = *(__SE_TEMPLATE_v1 *)((uint8_t *)pKerPrivArgs->bufPblock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *)((uint8_t *)pKerPrivArgs->bufPblock + SE_SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *)((uint8_t *)pKerPrivArgs->bufPblock + SE_SA0_PARAM_OFFSET);
   __HWA_CONFIG_REG_v1 mmaConfig = *((__HWA_CONFIG_REG_v1 *)((uint8_t *)pKerPrivArgs->bufPblock + MMA_CONFIGREG_OFFSET));
   __HWA_OFFSET_REG mmaOffset = *((__HWA_OFFSET_REG *)((uint8_t *)pKerPrivArgs->bufPblock + MMA_OFFSETREG_OFFSET));

   __SE0_OPEN(pInA, se0Params);
   __SE1_OPEN(pInB, se1Params);
   __SA0_OPEN(sa0Params);
   __HWAOPEN(mmaConfig, mmaOffset, __MMA_OPEN_FSM_RESET);
   __HWAADV();

   /*************************************************************************/
   /* Matrix multiply                                                       */
   /*************************************************************************/
   __mma_vec VA, VB, VC;

   c7x::uchar_vec scaleVec;
   c7x::uchar_vec shiftVec;

   scaleVec = __as_mma_vec(c7x::char_vec(scaleValue));
   shiftVec = __as_mma_vec(c7x::uchar_vec(shift));

   typedef typename std::conditional<std::is_same<dType, int16_t>::value, c7x::cshort_vec, c7x::cint_vec>::type cplxV;

   __HWA_LOAD_2REG(__as_mma_vec(scaleVec), __as_mma_vec(scaleVec),
                   __MMA_LOAD_2REG_SCALE_1_0);
   __HWAADV();
   __HWA_LOAD_2REG(__as_mma_vec(shiftVec), __as_mma_vec(shiftVec),
                   __MMA_LOAD_2REG_SHIFT_1_0);
   __HWAADV();

   for (r = 0; r < (int32_t)MMA_SIZE / 2; r++)
   {
      // 3 + trip_cnt * 2
      VB = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDB(VB);
      __HWAADV();
      __HWALDB(c7x::reinterpret<__mma_vec>(__crot270(c7x::reinterpret<cplxV>(VB))));
      __HWAADV();
   }

   for (r = 0; r < (int32_t)MMA_SIZE / 2; r++)
   {
      // 22 + trip_cnt * 2
      VA = c7x::strm_eng<0, __mma_vec>::get_adv();
      VB = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDAB(VA, VB);
      __HWAOPXFER(__MMA_A_LDA);
      VC = __HWARCV(0);
      __HWAADV();
      VA = c7x::strm_eng<0, __mma_vec>::get_adv();
      __HWALDAB(VA, c7x::reinterpret<__mma_vec>(__crot270(c7x::reinterpret<cplxV>(VB))));
      __HWAOPXFER(__MMA_A_LDA);
      VC = __HWARCV(0);
      __HWAADV();
   }

   for (r = 0; r < (int32_t)(nBatch * M * 2 * K * N * MMA_SIZE / 2); r++)
   {
      // 23 + trip_cnt * 2
      VA = c7x::strm_eng<0, __mma_vec>::get_adv();
      VB = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDAB(VA, VB);
      __HWAOPXFER(__MMA_A_LDA);
      VC = __HWARCV(0);
      __HWAADV();
      __vpred vpStore = __SA0_VPRED(__mma_vec);
      __mma_vec *ptr = __SA0ADV(__mma_vec, pOut);
      __vstore_pred(vpStore, ptr, VC);
      VA = c7x::strm_eng<0, __mma_vec>::get_adv();
      __HWALDAB(VA, c7x::reinterpret<__mma_vec>(__crot270(c7x::reinterpret<cplxV>(VB))));
      __HWAOPXFER(__MMA_A_LDA);
      VC = __HWARCV(0);
      __HWAADV();
      vpStore = __SA0_VPRED(__mma_vec);
      ptr = __SA0ADV(__mma_vec, pOut);
      __vstore_pred(vpStore, ptr, VC);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __HWACLOSE(0);

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_ci<int16_t, DSPLIB_MMA_SIZE_16_BIT>(DSPLIB_kernelHandle handle,
                                                                                                 const void *restrict pInA,
                                                                                                 const void *restrict pInB,
                                                                                                 void *restrict pOut);

template DSPLIB_STATUS DSPLIB_complexMatrixMultiply_exec_ci<int32_t, DSPLIB_MMA_SIZE_32_BIT>(DSPLIB_kernelHandle handle,
                                                                                                 const void *restrict pInA,
                                                                                                 const void *restrict pInB,
                                                                                                 void *restrict pOut);
