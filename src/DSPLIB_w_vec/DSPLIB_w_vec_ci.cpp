// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_w_vec_priv.h"
#include <float.h>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)

template <typename dataType>
DSPLIB_STATUS DSPLIB_w_vec_init_ci(DSPLIB_kernelHandle          handle,
                                   const DSPLIB_bufParams1D_t  *bufParamsIn,
                                   const DSPLIB_bufParams1D_t  *bufParamsOut,
                                   const DSPLIB_w_vec_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   DSPLIB_w_vec_PrivArgs *pKerPrivArgs = (DSPLIB_w_vec_PrivArgs *) handle;

   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   uint32_t blockSize = pKerPrivArgs->blockSize;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   SE_VECLEN                                                       = c7x::se_veclen<vec>::value;
   SA_VECLEN                                                       = c7x::sa_veclen<vec>::value;
   SE_ELETYPE                                                      = c7x::se_eletype<vec>::value;

   // determine how many SIMD Widths SE1 has to fetch
   uint32_t icnt2Size     = blockSize / eleCount;
   uint32_t remBlocksSize = blockSize % eleCount;
   if (remBlocksSize) {
      icnt2Size++;
   }

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   // determine scalar of vector style weight input
   DSPLIB_w_vec_weightStyle weight_flag = pKerPrivArgs->initArgs.weightStyle;

   // if scalar (weight flag = 0) SE is 1D
   if (weight_flag == DSPLIB_SCALAR) {

      /**********************************************************************/
      /* Prepare streaming engine 0,1 to fetch the input1 and input 2       */
      /**********************************************************************/

      // se0 parameters
      se0Params = __gen_SE_TEMPLATE_v1();

      se0Params.ICNT0   = blockSize;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DIMFMT  = __SE_DIMFMT_1D;

      // se1 parameters same as se0
      se1Params = __gen_SE_TEMPLATE_v1();

      se1Params.ICNT0   = blockSize;
      se1Params.ELETYPE = SE_ELETYPE;
      se1Params.VECLEN  = SE_VECLEN;
      se1Params.DIMFMT  = __SE_DIMFMT_1D;
   }

   // if vector (weight flag = 1) SE is 2D
   else {

      /**********************************************************************/
      /* Prepare streaming engine 0 to fetch the weight input               */
      /**********************************************************************/
      se0Params = __gen_SE_TEMPLATE_v1();

      se0Params.ICNT0   = blockSize;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DIMFMT  = __SE_DIMFMT_1D;

      /**********************************************************************/
      /* Prepare streaming engine 1 to fetch input1 and input2              */
      /**********************************************************************/
      se1Params = __gen_SE_TEMPLATE_v1();

      se1Params.DIMFMT  = __SE_DIMFMT_3D;
      se1Params.ICNT0   = eleCount;
      se1Params.ELETYPE = SE_ELETYPE;
      se1Params.VECLEN  = SE_VECLEN;
      se1Params.ICNT1   = 2;
      se1Params.ICNT2   = icnt2Size;
      // se1Params.DIM1: set to ADDR(x2) - ADDR(x1) in exec
      se1Params.DIM2          = eleCount;
      se1Params.DECDIM1       = __SE_DECDIM_DIM2;
      se1Params.DECDIM1_WIDTH = blockSize;
   }

   /**********************************************************************/
   /* Prepare SA template to store output                                */
   /**********************************************************************/
   sa0Params = __gen_SA_TEMPLATE_v1();

   sa0Params.ICNT0  = blockSize;
   sa0Params.DIM1   = blockSize;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_w_vec_init_ci<int16_t>(DSPLIB_kernelHandle          handle,
                                                     const DSPLIB_bufParams1D_t  *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t  *bufParamsOut,
                                                     const DSPLIB_w_vec_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_w_vec_init_ci<uint16_t>(DSPLIB_kernelHandle          handle,
                                                      const DSPLIB_bufParams1D_t  *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t  *bufParamsOut,
                                                      const DSPLIB_w_vec_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_w_vec_init_ci<int32_t>(DSPLIB_kernelHandle          handle,
                                                     const DSPLIB_bufParams1D_t  *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t  *bufParamsOut,
                                                     const DSPLIB_w_vec_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_w_vec_init_ci<uint32_t>(DSPLIB_kernelHandle          handle,
                                                      const DSPLIB_bufParams1D_t  *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t  *bufParamsOut,
                                                      const DSPLIB_w_vec_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_w_vec_init_ci<int8_t>(DSPLIB_kernelHandle          handle,
                                                    const DSPLIB_bufParams1D_t  *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t  *bufParamsOut,
                                                    const DSPLIB_w_vec_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_w_vec_init_ci<uint8_t>(DSPLIB_kernelHandle          handle,
                                                     const DSPLIB_bufParams1D_t  *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t  *bufParamsOut,
                                                     const DSPLIB_w_vec_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_w_vec_init_ci<float>(DSPLIB_kernelHandle          handle,
                                                   const DSPLIB_bufParams1D_t  *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t  *bufParamsOut,
                                                   const DSPLIB_w_vec_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_w_vec_init_ci<double>(DSPLIB_kernelHandle          handle,
                                                    const DSPLIB_bufParams1D_t  *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t  *bufParamsOut,
                                                    const DSPLIB_w_vec_InitArgs *pKerInitArgs);

// Execution Function of W_VEC Kernel (both scalar and vector weight input)
template <typename dataType>
DSPLIB_STATUS DSPLIB_w_vec_exec_ci(DSPLIB_kernelHandle handle,
                                   void *restrict pIn1,
                                   void *restrict pIn2,
                                   void *restrict pM,
                                   void *restrict pOut)
{
   DSPLIB_STATUS          status       = DSPLIB_SUCCESS;
   DSPLIB_w_vec_PrivArgs *pKerPrivArgs = (DSPLIB_w_vec_PrivArgs *) handle;
   int32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // dataType *restrict pOutp     = (dataType *) pOut;
   dataType *restrict pInLocal1 = (dataType *) pIn1;
   dataType *restrict pInLocal2 = (dataType *) pIn2;
   dataType *restrict pMLocal   = (dataType *) pM;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_w_vec_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif
   uint8_t                 *pBlock     = pKerPrivArgs->bufPblock;
   DSPLIB_w_vec_weightStyle weightFlag = pKerPrivArgs->initArgs.weightStyle;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   // set se1 param DIM1 offset
   se1Params.DIM1 = ((dataType *) pM) - ((dataType *) pIn1);

   __SA0_OPEN(sa0Params);

   if (weightFlag == DSPLIB_SCALAR) {
      // Input samples
      __SE0_OPEN(pInLocal1, se0Params);
      __SE1_OPEN(pInLocal2, se0Params);

      // Output samples
      __SA0_OPEN(sa0Params);

#if DSPLIB_DEBUGPRINT
      printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

      vec out;

      // vec m = (vec)(*pMLocal);

      // vdup can't recognize float???
      // vec m = __duplicate(*pMlocal);
      vec m = __vload_dup(pMLocal);

      for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
         vec a = c7x::strm_eng<0, vec>::get_adv();
         vec b = c7x::strm_eng<1, vec>::get_adv();

         out = a * m + b;

         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);

         // out.print();

         __vstore_pred(tmp, VB1, out);
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();

      status = DSPLIB_SUCCESS;
   }

   else {

      // Input samples
      // need to use se1 for fetching both input1 and input2 vector
      __SE0_OPEN(pInLocal2, se0Params);
      __SE1_OPEN(pInLocal1, se1Params);

      // Output samples
      __SA0_OPEN(sa0Params);

#if DSPLIB_DEBUGPRINT
      printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

      vec out;
      for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
         vec a = c7x::strm_eng<1, vec>::get_adv();
         vec w = c7x::strm_eng<1, vec>::get_adv();
         vec b = c7x::strm_eng<0, vec>::get_adv();

         out = a * w + b;

         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);

         __vstore_pred(tmp, VB1, out);
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();

      status = DSPLIB_SUCCESS;
   }
   return status;
}

template DSPLIB_STATUS DSPLIB_w_vec_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn1,
                                                   void *restrict pIn2,
                                                   void *restrict pM,
                                                   void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                    void *restrict pIn1,
                                                    void *restrict pIn2,
                                                    void *restrict pM,
                                                    void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                    void *restrict pIn1,
                                                    void *restrict pIn2,
                                                    void *restrict pM,
                                                    void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_ci<uint8_t>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pM,
                                                     void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pM,
                                                     void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_ci<uint16_t>(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn1,
                                                      void *restrict pIn2,
                                                      void *restrict pM,
                                                      void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pM,
                                                     void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_ci<uint32_t>(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn1,
                                                      void *restrict pIn2,
                                                      void *restrict pM,
                                                      void *restrict pOut);
