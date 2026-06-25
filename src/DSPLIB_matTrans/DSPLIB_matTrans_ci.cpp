// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_matTrans_priv.h"

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

/**********************************************************************/
/* INITIALIZATION                                                     */
/**********************************************************************/
template <typename dataType>
inline void matTrans_init_64bit(DSPLIB_kernelHandle         handle,
                                const DSPLIB_bufParams2D_t *bufParamsIn,
                                const DSPLIB_bufParams2D_t *bufParamsOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function to init SE for 64-bit data");

   DSPLIB_matTrans_PrivArgs                              *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;
   uint8_t                                               *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                                               widthIn      = pKerPrivArgs->widthIn;
   uint32_t                                               heightIn     = pKerPrivArgs->heightIn;
   int32_t                                                strideIn     = bufParamsIn->stride_y;
   int32_t                                                strideOut    = bufParamsOut->stride_y;
   int32_t                                                dataSize     = sizeof(dataType);
   __SE_TEMPLATE_v1                                       se0Params    = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1                                       sa0Params    = __gen_SA_TEMPLATE_v1();
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;
   int32_t                                                iter       = (heightIn + eleCount - 1) / eleCount;

   se0Params.ICNT0     = widthIn;
   se0Params.ICNT1     = (heightIn > (uint32_t) eleCount) ? eleCount : heightIn;
   se0Params.DIM1      = strideIn / dataSize;
   se0Params.ICNT2     = iter;
   se0Params.DIM2      = (strideIn / dataSize) * eleCount;
   se0Params.DIMFMT    = __SE_DIMFMT_3D;
   se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   se0Params.ELETYPE   = SE_ELETYPE;
   se0Params.VECLEN    = SE_VECLEN;

   sa0Params.ICNT0         = eleCount;
   sa0Params.ICNT1         = widthIn;
   sa0Params.DIM1          = strideOut / dataSize;
   sa0Params.ICNT2         = iter;
   sa0Params.DIM2          = eleCount;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DIMFMT        = __SA_DIMFMT_3D;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1_WIDTH = heightIn;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET))    = iter;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function to init SE for 64-bit data");
}

template <typename dataType>
inline void matTrans_init_32bit(DSPLIB_kernelHandle         handle,
                                const DSPLIB_bufParams2D_t *bufParamsIn,
                                const DSPLIB_bufParams2D_t *bufParamsOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function to init SE for 32-bit data");

   DSPLIB_matTrans_PrivArgs                              *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;
   uint8_t                                               *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                                               widthIn      = pKerPrivArgs->widthIn;
   uint32_t                                               heightIn     = pKerPrivArgs->heightIn;
   int32_t                                                strideIn     = bufParamsIn->stride_y;
   int32_t                                                strideOut    = bufParamsOut->stride_y;
   int32_t                                                dataSize     = sizeof(dataType);
   __SE_TEMPLATE_v1                                       se0Params    = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1                                       sa0Params    = __gen_SA_TEMPLATE_v1();
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;
   int32_t                                                iter       = (heightIn + eleCount - 1) / eleCount;

   se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   se0Params.ICNT0     = widthIn;
   se0Params.ICNT1     = (heightIn > (uint32_t) eleCount) ? eleCount : heightIn;
   se0Params.DIM1      = strideIn / dataSize;
   se0Params.ICNT2     = iter;
   se0Params.DIM2      = (strideIn / dataSize) * eleCount;
   se0Params.DIMFMT    = __SE_DIMFMT_3D;
   se0Params.ELETYPE   = SE_ELETYPE;
   se0Params.VECLEN    = SE_VECLEN;

   sa0Params.ICNT0         = eleCount;
   sa0Params.ICNT1         = widthIn;
   sa0Params.DIM1          = strideOut / dataSize;
   sa0Params.ICNT2         = iter;
   sa0Params.DIM2          = eleCount;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DIMFMT        = __SA_DIMFMT_3D;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1_WIDTH = heightIn;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET))    = iter;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function to init SE for 32-bit data");
}

template <typename dataType>
inline void matTrans_init_16bit(DSPLIB_kernelHandle         handle,
                                const DSPLIB_bufParams2D_t *bufParamsIn,
                                const DSPLIB_bufParams2D_t *bufParamsOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function to init SE for 16-bit data");

   DSPLIB_matTrans_PrivArgs                              *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;
   uint8_t                                               *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                                               widthIn      = pKerPrivArgs->widthIn;
   uint32_t                                               heightIn     = pKerPrivArgs->heightIn;
   int32_t                                                strideIn     = bufParamsIn->stride_y;
   int32_t                                                strideOut    = bufParamsOut->stride_y;
   int32_t                                                dataSize     = sizeof(dataType);
   __SE_TEMPLATE_v1                                       se0Params    = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1                                       sa0Params    = __gen_SA_TEMPLATE_v1();
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount    = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE  = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN   = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN   = c7x::sa_veclen<vec>::value;
   int32_t                                                iter        = (heightIn + eleCount - 1) / eleCount;
   uint32_t                                               paddedWidth = ((widthIn + 15) / 16) * 16;

   se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   se0Params.ICNT0     = 16;
   se0Params.ICNT1     = (heightIn > (uint32_t) (eleCount / 2)) ? (eleCount / 2) : heightIn;
   se0Params.DIM1      = strideIn / dataSize;
   se0Params.ICNT2     = 2;
   se0Params.DIM2      = (strideIn / dataSize) * eleCount / 2;
   se0Params.ICNT3     = (widthIn + 15) / 16;
   se0Params.DIM3      = 16;
   se0Params.ICNT4     = iter;
   se0Params.DIM4      = (strideIn / dataSize) * eleCount;
   se0Params.DIMFMT    = __SE_DIMFMT_5D;
   se0Params.ELETYPE   = SE_ELETYPE;
   se0Params.VECLEN    = SE_VECLEN;

   sa0Params.ICNT0         = eleCount;
   sa0Params.ICNT1         = paddedWidth;
   sa0Params.DIM1          = strideOut / dataSize;
   sa0Params.ICNT2         = iter;
   sa0Params.DIM2          = eleCount;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DIMFMT        = __SA_DIMFMT_3D;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1_WIDTH = heightIn;
   sa0Params.DECDIM2       = __SA_DECDIM_DIM1;
   sa0Params.DECDIM2_WIDTH = sa0Params.DIM1 * widthIn;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET))    = iter;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function to init SE for 16-bit data");
}

template <typename dataType>
inline void matTrans_init_8bit(DSPLIB_kernelHandle         handle,
                               const DSPLIB_bufParams2D_t *bufParamsIn,
                               const DSPLIB_bufParams2D_t *bufParamsOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function to init SE for 8-bit data");

   DSPLIB_matTrans_PrivArgs                              *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;
   uint8_t                                               *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                                               widthIn      = pKerPrivArgs->widthIn;
   uint32_t                                               heightIn     = pKerPrivArgs->heightIn;
   int32_t                                                strideIn     = bufParamsIn->stride_y;
   int32_t                                                strideOut    = bufParamsOut->stride_y;
   int32_t                                                dataSize     = sizeof(dataType);
   int32_t                                                iter         = 0;
   __SE_TEMPLATE_v1                                       se0Params    = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1                                       sa0Params    = __gen_SA_TEMPLATE_v1();
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;

#if __C7X_VEC_SIZE_BITS__ == 512
   int32_t outEleCount = eleCount / 2;
#else
   int32_t                         outEleCount = eleCount;
#endif

   iter = (heightIn + outEleCount - 1) / outEleCount;

   se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   se0Params.ICNT0     = widthIn;
   se0Params.ICNT1     = 16;
   se0Params.DIM1      = strideIn;
   se0Params.ICNT2     = iter;
   se0Params.DIM2      = strideIn * outEleCount;
   se0Params.DIMFMT    = __SE_DIMFMT_3D;
   se0Params.ELETYPE   = SE_ELETYPE;
   se0Params.VECLEN    = SE_VECLEN;

   sa0Params.ICNT0         = outEleCount;
   sa0Params.ICNT1         = widthIn;
   sa0Params.DIM1          = strideOut / dataSize;
   sa0Params.ICNT2         = iter;
   sa0Params.DIM2          = outEleCount;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DIMFMT        = __SA_DIMFMT_3D;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1_WIDTH = heightIn;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET))    = iter;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function to init SE for 8-bit data");
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<float>(DSPLIB_kernelHandle            handle,
                                             const DSPLIB_bufParams2D_t    *bufParamsIn,
                                             const DSPLIB_bufParams2D_t    *bufParamsOut,
                                             const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_32bit<float>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<double>(DSPLIB_kernelHandle            handle,
                                              const DSPLIB_bufParams2D_t    *bufParamsIn,
                                              const DSPLIB_bufParams2D_t    *bufParamsOut,
                                              const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_64bit<double>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<int8_t>(DSPLIB_kernelHandle            handle,
                                              const DSPLIB_bufParams2D_t    *bufParamsIn,
                                              const DSPLIB_bufParams2D_t    *bufParamsOut,
                                              const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_8bit<int8_t>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<uint8_t>(DSPLIB_kernelHandle            handle,
                                               const DSPLIB_bufParams2D_t    *bufParamsIn,
                                               const DSPLIB_bufParams2D_t    *bufParamsOut,
                                               const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_8bit<uint8_t>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<int16_t>(DSPLIB_kernelHandle            handle,
                                               const DSPLIB_bufParams2D_t    *bufParamsIn,
                                               const DSPLIB_bufParams2D_t    *bufParamsOut,
                                               const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_16bit<int16_t>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<uint16_t>(DSPLIB_kernelHandle            handle,
                                                const DSPLIB_bufParams2D_t    *bufParamsIn,
                                                const DSPLIB_bufParams2D_t    *bufParamsOut,
                                                const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_16bit<uint16_t>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<int32_t>(DSPLIB_kernelHandle            handle,
                                               const DSPLIB_bufParams2D_t    *bufParamsIn,
                                               const DSPLIB_bufParams2D_t    *bufParamsOut,
                                               const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_32bit<int32_t>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<uint32_t>(DSPLIB_kernelHandle            handle,
                                                const DSPLIB_bufParams2D_t    *bufParamsIn,
                                                const DSPLIB_bufParams2D_t    *bufParamsOut,
                                                const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_32bit<uint32_t>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<int64_t>(DSPLIB_kernelHandle            handle,
                                               const DSPLIB_bufParams2D_t    *bufParamsIn,
                                               const DSPLIB_bufParams2D_t    *bufParamsOut,
                                               const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_64bit<int64_t>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_init_ci<uint64_t>(DSPLIB_kernelHandle            handle,
                                                const DSPLIB_bufParams2D_t    *bufParamsIn,
                                                const DSPLIB_bufParams2D_t    *bufParamsOut,
                                                const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_init_64bit<uint64_t>(handle, bufParamsIn, bufParamsOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/* IMPLEMENTATION                                                     */
/**********************************************************************/
template <typename dataType>
inline void matTrans_compute_64_32bit(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for Transpose 64/32-bit data");

   DSPLIB_matTrans_PrivArgs *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;
   uint32_t                  widthIn      = pKerPrivArgs->widthIn;
   __SE_TEMPLATE_v1          se0Params;
   __SA_TEMPLATE_v1          sa0Params;
   uint8_t                  *pBlock = pKerPrivArgs->bufPblock;
   se0Params                        = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params                        = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   int32_t iter                     = *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET));
   dataType *restrict pInLocal      = (dataType *) pIn;
   dataType *restrict pOutLocal     = (dataType *) pOut;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                loopCount = iter * widthIn;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p loopCount: %d\n", pInLocal, pOutLocal, loopCount);

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   for (int32_t i = 0; i < loopCount; i++) {
      vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();

      __vpred predTemp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, loadVec1);
   }
   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for Transpose 64/32-bit data");
}

template <typename dataType>
inline void matTrans_compute_16bit(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for Transpose 16-bit data");

   DSPLIB_matTrans_PrivArgs *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;
   uint32_t                  widthIn      = pKerPrivArgs->widthIn;
   __SE_TEMPLATE_v1          se0Params;
   __SA_TEMPLATE_v1          sa0Params;
   uint8_t                  *pBlock = pKerPrivArgs->bufPblock;

   se0Params                    = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params                    = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   int32_t iter                 = *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET));
   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;
   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t loopCount = iter * ((widthIn + 15) / 16);

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p loopCount: %d\n", pInLocal, pOutLocal, loopCount);

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);
   for (int i = 0; i < loopCount; i++) {
      vec loadVec1  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec2  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec3  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec4  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec5  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec6  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec7  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec8  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec9  = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec10 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec11 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec12 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec13 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec14 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec15 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec16 = c7x::strm_eng<0, vec>::get_adv();

      vec     checkEven1 = __pack_consec_low(loadVec9, loadVec1);
      __vpred predTemp   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreVec  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkEven1);

      vec checkOdd1 = __pack_consec_high(loadVec9, loadVec1);
      predTemp      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec     = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkOdd1);

      vec checkEven2 = __pack_consec_low(loadVec10, loadVec2);
      predTemp       = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec      = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkEven2);

      vec checkOdd2 = __pack_consec_high(loadVec10, loadVec2);
      predTemp      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec     = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkOdd2);

      vec checkEven3 = __pack_consec_low(loadVec11, loadVec3);
      predTemp       = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec      = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkEven3);

      vec checkOdd3 = __pack_consec_high(loadVec11, loadVec3);
      predTemp      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec     = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkOdd3);

      vec checkEven4 = __pack_consec_low(loadVec12, loadVec4);
      predTemp       = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec      = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkEven4);

      vec checkOdd4 = __pack_consec_high(loadVec12, loadVec4);
      predTemp      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec     = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkOdd4);

      vec checkEven5 = __pack_consec_low(loadVec13, loadVec5);
      predTemp       = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec      = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkEven5);

      vec checkOdd5 = __pack_consec_high(loadVec13, loadVec5);
      predTemp      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec     = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkOdd5);

      vec checkEven6 = __pack_consec_low(loadVec14, loadVec6);
      predTemp       = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec      = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkEven6);

      vec checkOdd6 = __pack_consec_high(loadVec14, loadVec6);
      predTemp      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec     = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkOdd6);

      vec checkEven7 = __pack_consec_low(loadVec15, loadVec7);
      predTemp       = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec      = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkEven7);

      vec checkOdd7 = __pack_consec_high(loadVec15, loadVec7);
      predTemp      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec     = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkOdd7);

      vec checkEven8 = __pack_consec_low(loadVec16, loadVec8);
      predTemp       = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec      = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkEven8);

      vec checkOdd8 = __pack_consec_high(loadVec16, loadVec8);
      predTemp      = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec     = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(predTemp, pStoreVec, checkOdd8);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for Transpose 16-bit data");
}

template <typename dataType>
inline void matTrans_compute_8bit(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering template function for Transpose 8-bit data");

   DSPLIB_matTrans_PrivArgs *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;
   uint32_t                  widthIn      = pKerPrivArgs->widthIn;
   int32_t                   strideIn     = pKerPrivArgs->strideIn;
   uint32_t                  dataSize     = sizeof(dataType);
   __SE_TEMPLATE_v1          se0Params;
   __SA_TEMPLATE_v1          sa0Params;
   uint8_t                  *pBlock = pKerPrivArgs->bufPblock;
   se0Params                        = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params                        = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   int32_t iter                     = *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET));
   dataType *restrict pInLocal      = (dataType *) pIn;
   dataType *restrict pOutLocal     = (dataType *) pOut;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount  = c7x::element_count_of<vec>::value;
   int32_t                                                loopCount = iter * widthIn / 4;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p loopCount: %d\n", pInLocal, pOutLocal, loopCount);

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);
#if (__C7X_VEC_SIZE_BITS__ == 256)
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * (eleCount / 2)), se0Params);
   for (int i = 0; i < loopCount; i++) {

      vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec2 = c7x::strm_eng<0, vec>::get_adv();

      vec loadVec3 = c7x::strm_eng<1, vec>::get_adv();
      vec loadVec4 = c7x::strm_eng<1, vec>::get_adv();

      vec iEven1 = __pack_consec_low(loadVec2, loadVec1);
      vec iOdd1  = __pack_consec_high(loadVec2, loadVec1);
      vec iEven2 = __pack_consec_low(loadVec4, loadVec3);
      vec iOdd2  = __pack_consec_high(loadVec4, loadVec3);

      __vpred        predTemp  = c7x::strm_agen<0, vec>::get_vpred();
      c7x::char_vec *pStoreVec = c7x::strm_agen<0, c7x::char_vec>::get_adv(pOutLocal);
      __vstore_pred_packl_2src(predTemp, pStoreVec, c7x::as_short_vec(iEven1), c7x::as_short_vec(iEven2));

      predTemp  = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, c7x::char_vec>::get_adv(pOutLocal);
      __vstore_pred_packl_2src(predTemp, pStoreVec, c7x::as_short_vec(iOdd1), c7x::as_short_vec(iOdd2));

      predTemp  = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, c7x::char_vec>::get_adv(pOutLocal);
      __vstore_pred_packh_2src(predTemp, pStoreVec, c7x::as_short_vec(iEven1), c7x::as_short_vec(iEven2));

      predTemp  = c7x::strm_agen<0, vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, c7x::char_vec>::get_adv(pOutLocal);
      __vstore_pred_packh_2src(predTemp, pStoreVec, c7x::as_short_vec(iOdd1), c7x::as_short_vec(iOdd2));
   }

#elif (__C7X_VEC_SIZE_BITS__ == 512)
   typedef typename c7x::char_hvec vecOut;
   __SE1_OPEN(pInLocal + ((strideIn / dataSize) * eleCount / 4), se0Params);
   for (int i = 0; i < loopCount; i++) {
      vec loadVec1 = c7x::strm_eng<0, vec>::get_adv();
      vec loadVec2 = c7x::strm_eng<1, vec>::get_adv();

      vec iEven = __pack_consec_low(loadVec2, loadVec1);
      vec iOdd  = __pack_consec_high(loadVec2, loadVec1);

      __vpred predTemp  = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
      vecOut *pStoreVec = c7x::strm_agen<0, vecOut>::get_adv(pOutLocal);
      __vstore_pred_packl(predTemp, pStoreVec, c7x::as_short_vec(iEven));

      predTemp  = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vecOut>::get_adv(pOutLocal);
      __vstore_pred_packl(predTemp, pStoreVec, c7x::as_short_vec(iOdd));

      predTemp  = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vecOut>::get_adv(pOutLocal);
      __vstore_pred_packh(predTemp, pStoreVec, c7x::as_short_vec(iEven));

      predTemp  = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
      pStoreVec = c7x::strm_agen<0, vecOut>::get_adv(pOutLocal);
      __vstore_pred_packh(predTemp, pStoreVec, c7x::as_short_vec(iOdd));
   }

#endif
   __SE1_CLOSE();
   __SE0_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting template function for Transpose 8-bit data");
}

void DSPLIB_matTrans_perfEst(DSPLIB_kernelHandle handle,
                             uint64_t           *archCycles,
                             uint64_t           *estCycles,
                             uint64_t            dataType,
                             uint32_t            dim_x)
{
   DSPLIB_matTrans_PrivArgs *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   uint64_t matTransStartupCycles   = 0;
   uint64_t matTransTeardownCycles  = 0;
   uint64_t matTransOperationCycles = 0;
   uint64_t matTransOverheadCycles  = 0;

   if ((dataType != DSPLIB_INT64) && (dataType != DSPLIB_INT32) && (dataType != DSPLIB_INT16) &&
       (dataType != DSPLIB_INT8) && (dataType != DSPLIB_UINT64) && (dataType != DSPLIB_UINT32) &&
       (dataType != DSPLIB_UINT16) && (dataType != DSPLIB_UINT8) && (dataType != DSPLIB_FLOAT32) &&
       (dataType != DSPLIB_FLOAT64)) {
      status = DSPLIB_ERR_INVALID_TYPE;
   }
   else if ((dataType == DSPLIB_UINT16 || dataType == DSPLIB_INT16) && (dim_x % 2 != 0)) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }
   else if ((dataType == DSPLIB_UINT8 || dataType == DSPLIB_INT8) && (dim_x % 4 != 0)) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }
   else if (dim_x == 0) {
      status = DSPLIB_ERR_INVALID_VALUE;
   }
   else {
      /* Do nothing */
   }

   if (status == DSPLIB_SUCCESS) {
      if ((dataType == DSPLIB_FLOAT32) || (dataType == DSPLIB_FLOAT64) || (dataType == DSPLIB_INT32) ||
          (dataType == DSPLIB_UINT32) || (dataType == DSPLIB_INT64) || (dataType == DSPLIB_UINT64)) {

         uint8_t *pBlock    = pKerPrivArgs->bufPblock;
         uint32_t widthIn   = pKerPrivArgs->widthIn;
         int32_t  iter      = *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET));
         int32_t  loopCount = iter * widthIn;

         matTransStartupCycles   = 27;
         matTransOperationCycles = 1 + loopCount * 2;
         matTransTeardownCycles  = 2;
      }
      else if (dataType == DSPLIB_INT8 || dataType == DSPLIB_UINT8) {

         uint8_t *pBlock    = pKerPrivArgs->bufPblock;
         uint32_t widthIn   = pKerPrivArgs->widthIn;
         int32_t  iter      = *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET));
         int32_t  loopCount = iter * widthIn / 4;

         matTransStartupCycles   = 26;
         matTransOperationCycles = 9 + loopCount * 4;
         matTransTeardownCycles  = 2;
      }

      else {

         uint8_t *pBlock    = pKerPrivArgs->bufPblock;
         int32_t  iter      = *(int32_t *) ((uint8_t *) pBlock + (2 * SE_SA0_PARAM_OFFSET));
         uint32_t widthIn   = pKerPrivArgs->widthIn;
         int32_t  loopCount = iter * widthIn / 2;

         matTransStartupCycles   = 28;
         matTransOperationCycles = 4 + loopCount * 2;
         matTransTeardownCycles  = 2;
      }
      matTransOverheadCycles += matTransStartupCycles + matTransTeardownCycles;
      *estCycles  = matTransOperationCycles + matTransOverheadCycles;
      *archCycles = matTransOperationCycles;
   }
   else {

      *estCycles  = 0;
      *archCycles = 0;
   }
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_64_32bit<float>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_64_32bit<double>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_8bit<int8_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_8bit<uint8_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_16bit<int16_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_16bit<uint16_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_64_32bit<int32_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_64_32bit<uint32_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_64_32bit<int64_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}

template <>
DSPLIB_STATUS DSPLIB_matTrans_exec_ci<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   matTrans_compute_64_32bit<uint64_t>(handle, pIn, pOut);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);

   return DSPLIB_SUCCESS;
}
