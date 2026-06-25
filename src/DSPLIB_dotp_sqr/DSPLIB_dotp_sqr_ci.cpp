// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_dotp_sqr_priv.h"
#include <float.h>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)

// Generic initialization
template <typename dataType>
DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci(DSPLIB_kernelHandle             handle,
                                      const DSPLIB_bufParams1D_t     *bufParamsIn,
                                      const DSPLIB_bufParams1D_t     *bufParamsOut,
                                      const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;

   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;

   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   int32_t  blockSize = pKerPrivArgs->blockSize;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

#if DSPLIB_DEBUGPRINT
   int32_t eleCount = c7x::element_count_of<vec>::value;
   printf("Enter eleCount %d\n", eleCount);
#endif

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch the input                      */
   /**********************************************************************/
   se0Params = __gen_SE_TEMPLATE_v1();

   se0Params.ICNT0   = blockSize;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.DIMFMT  = __SE_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;

   return status;
}

// int8_t initialization promote to int16_t
template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci<int8_t>(DSPLIB_kernelHandle             handle,
                                              const DSPLIB_bufParams1D_t     *bufParamsIn,
                                              const DSPLIB_bufParams1D_t     *bufParamsOut,
                                              const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;

   __SE_PROMOTE SE_PROMOTE;

   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;

   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   int32_t  blockSize = pKerPrivArgs->blockSize;

   SE_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
   SE_ELETYPE = c7x::se_eletype<c7x::char_vec>::value;
   SE_PROMOTE = __SE_PROMOTE_2X_SIGNEXT;
#if DSPLIB_DEBUGPRINT
   int32_t eleCount = c7x::element_count_of<char_vec>::value;
   printf("Enter eleCount %d\n", eleCount);
#endif

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch the input                      */
   /**********************************************************************/
   se0Params = __gen_SE_TEMPLATE_v1();

   se0Params.ICNT0   = blockSize;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.DIMFMT  = __SE_DIMFMT_1D;
   se0Params.PROMOTE = SE_PROMOTE;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci<int16_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci<uint16_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                         const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci<int32_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci<uint32_t>(DSPLIB_kernelHandle             handle,
                                                         const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                         const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                         const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci<int8_t>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                       const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci<uint8_t>(DSPLIB_kernelHandle             handle,
                                                        const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                        const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                        const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci<float>(DSPLIB_kernelHandle             handle,
                                                      const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                      const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_dotp_sqr_init_ci<double>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams1D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t     *bufParamsOut,
                                                       const DSPLIB_dotp_sqr_InitArgs *pKerInitArgs);

// This function performs horizontal add of the output vector.
// It is used for float and double datat teyps.
// The __horizontal_add() intrinsic is used to perform horizontal add on all other datatypes.

#pragma FUNC_ALWAYS_INLINE
static inline float DSPLIB_horiAdd(c7x::float_vec vector)
{
   float sum = 0;

   vector.lo()      = vector.hi() + vector.lo();
   vector.lo().lo() = vector.lo().hi() + vector.lo().lo();
// #if __C7X_VEC_SIZE_BYTES__ == 64
#if defined(__C7100__) || defined(__C7120__)
   vector.lo().lo().lo() = vector.lo().lo().hi() + vector.lo().lo().lo();
#endif
   sum = (float) vector.s[0] + (float) vector.s[1];

   return sum;
}

#pragma FUNC_ALWAYS_INLINE
static inline double DSPLIB_horiAdd(c7x::double_vec vector)
{
   double sum = 0;

   vector.lo() = vector.hi() + vector.lo();

// #if __C7X_VEC_SIZE_BYTES__ == 64
#if defined(__C7100__) || defined(__C7120__)
   vector.lo().lo() = vector.lo().hi() + vector.lo().lo();
#endif

   sum = (double) vector.s[0] + (double) vector.s[1];

   return sum;
}

/**********************************************************************/
/*         Execute for datatypes float and double                     */
/**********************************************************************/

// This is the generic implementation of exec_ci. It is used for float and double.
// Other datatypes have their own explicet implementation.
template <typename dataType>
DSPLIB_STATUS
DSPLIB_dotp_sqr_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;

   dataType *restrict pInLocal1 = (dataType *) pIn1;
   dataType *restrict pInLocal2 = (dataType *) pIn2;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   vec out_dotp;
   vec out_ab_dotp;
   vec out_cd_dotp;
   vec out_ef_dotp;
   vec out_gh_dotp;
   out_dotp    = (vec) 0.0;
   out_ab_dotp = (vec) 0.0;
   out_cd_dotp = (vec) 0.0;
   out_ef_dotp = (vec) 0.0;
   out_gh_dotp = (vec) 0.0;
   dataType result_dotp;

   vec out_sqrAdd;
   vec out_ab_sqrAdd;
   vec out_cd_sqrAdd;
   vec out_ef_sqrAdd;
   vec out_gh_sqrAdd;
   out_sqrAdd    = (vec) 0.0;
   out_ab_sqrAdd = (vec) 0.0;
   out_cd_sqrAdd = (vec) 0.0;
   out_ef_sqrAdd = (vec) 0.0;
   out_gh_sqrAdd = (vec) 0.0;
   dataType result_sqrAdd;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount * 4) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      vec b = c7x::strm_eng<1, vec>::get_adv();

      out_ab_dotp += a * b;

      out_ab_sqrAdd += b * b;

      vec c = c7x::strm_eng<0, vec>::get_adv();
      vec d = c7x::strm_eng<1, vec>::get_adv();

      out_cd_dotp += c * d;

      out_cd_sqrAdd += d * d;

      vec e = c7x::strm_eng<0, vec>::get_adv();
      vec f = c7x::strm_eng<1, vec>::get_adv();

      out_ef_dotp += e * f;

      out_ef_sqrAdd += f * f;

      vec g = c7x::strm_eng<0, vec>::get_adv();
      vec h = c7x::strm_eng<1, vec>::get_adv();

      out_gh_dotp += g * h;

      out_gh_sqrAdd += h * h;
   }

   out_dotp = out_ab_dotp + out_cd_dotp + out_ef_dotp + out_gh_dotp;

   out_sqrAdd = out_ab_sqrAdd + out_cd_sqrAdd + out_ef_sqrAdd + out_gh_sqrAdd;

   result_dotp   = DSPLIB_horiAdd(out_dotp);
   result_sqrAdd = DSPLIB_horiAdd(out_sqrAdd);

   *pOutLocal   = result_dotp;
   *++pOutLocal = result_sqrAdd;

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype int8_t                          */
/**********************************************************************/
// The input datatype of int8_t is promoted to int16_t using the streaming engine.
// The dotp_sqr is then implemented as int16_t.
// When completed, the output of int64_t is casted down to int32_t.
template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                              void *restrict pIn1,
                                              void *restrict pIn2,
                                              void *restrict pOut)
{
   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;

   int16_t *restrict pInLocal1 = (int16_t *) pIn1;
   int16_t *restrict pInLocal2 = (int16_t *) pIn2;
   int32_t *restrict pOutLocal = (int32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<int16_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   typedef typename c7x::make_full_vector<int64_t>::type vec_out;

   vec_out out_dotp;
   out_dotp            = (vec_out) 0;
   int32_t result_dotp = 0;

   vec_out out_sqrAdd;
   out_sqrAdd            = (vec_out) 0;
   int32_t result_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      vec b = c7x::strm_eng<1, vec>::get_adv();

      out_dotp += __vdotp4hd_vvv(a, b);

      out_sqrAdd += __vdotp4hd_vvv(b, b);
   }

   // use intrensic for horizontal add
   // cast the int64_t output of __horizontal_add to int32_t.
   result_dotp   = (int32_t) __horizontal_add(out_dotp);
   result_sqrAdd = (int32_t) __horizontal_add(out_sqrAdd);

   *pOutLocal   = result_dotp;
   *++pOutLocal = result_sqrAdd;

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype uint8_t                         */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<uint8_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn1,
                                               void *restrict pIn2,
                                               void *restrict pOut)
{
   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;

   uint8_t *restrict pInLocal1  = (uint8_t *) pIn1;
   uint8_t *restrict pInLocal2  = (uint8_t *) pIn2;
   uint32_t *restrict pOutLocal = (uint32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<uint8_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   typedef typename c7x::make_full_vector<uint32_t>::type vec_out;

   vec_out out_dotp;
   out_dotp             = (vec_out) 0;
   uint32_t result_dotp = 0;

   vec_out out_sqrAdd;
   out_sqrAdd             = (vec_out) 0;
   uint32_t result_sqrAdd = 0;

   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      vec b = c7x::strm_eng<1, vec>::get_adv();

      out_dotp += __vdotp4ubw_vvv(a, b);

      out_sqrAdd += __vdotp4ubw_vvv(b, b);
   }
   // use intrensic for horizontal add
   // cast the uint64_t output of __horizontal_add to uint32_t.

   result_dotp   = (uint32_t) __horizontal_add(out_dotp);
   result_sqrAdd = (uint32_t) __horizontal_add(out_sqrAdd);

   *pOutLocal   = result_dotp;
   *++pOutLocal = result_sqrAdd;

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype int16_t                         */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn1,
                                               void *restrict pIn2,
                                               void *restrict pOut)
{
   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;

   int16_t *restrict pInLocal1 = (int16_t *) pIn1;
   int16_t *restrict pInLocal2 = (int16_t *) pIn2;
   int64_t *restrict pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<int16_t>::type vec; // short16
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   typedef typename c7x::make_full_vector<int64_t>::type vec_out;

   vec_out out_dotp;
   out_dotp            = (vec_out) 0;
   int64_t result_dotp = 0;

   vec_out out_sqrAdd;
   out_sqrAdd            = (vec_out) 0;
   int64_t result_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      vec b = c7x::strm_eng<1, vec>::get_adv();

      out_dotp += __vdotp4hd_vvv(a, b);

      out_sqrAdd += __vdotp4hd_vvv(b, b);
   }

   // use intrensic for horizontal add
   result_dotp   = __horizontal_add(out_dotp);
   result_sqrAdd = __horizontal_add(out_sqrAdd);

   *pOutLocal   = result_dotp;
   *++pOutLocal = result_sqrAdd;

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype uint16_t                        */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<uint16_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn1,
                                                void *restrict pIn2,
                                                void *restrict pOut)
{
   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;

   uint16_t *restrict pInLocal1 = (uint16_t *) pIn1;
   uint16_t *restrict pInLocal2 = (uint16_t *) pIn2;
   uint64_t *restrict pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<uint16_t>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   typedef typename c7x::make_full_vector<uint64_t>::type vec_out;

   vec_out out_dotp;
   out_dotp             = (vec_out) 0;
   uint64_t result_dotp = 0;

   vec_out out_sqrAdd;
   out_sqrAdd             = (vec_out) 0;
   uint64_t result_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      vec b = c7x::strm_eng<1, vec>::get_adv();

      out_dotp += __vdotp4uhd_vvv(a, b);

      out_sqrAdd += __vdotp4uhd_vvv(b, b);
   }

   // use intrensic for horizontal add
   result_dotp   = __horizontal_add(out_dotp);
   result_sqrAdd = __horizontal_add(out_sqrAdd);

   *pOutLocal   = result_dotp;
   *++pOutLocal = result_sqrAdd;

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype int32_t                         */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn1,
                                               void *restrict pIn2,
                                               void *restrict pOut)
{
   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;

   int32_t *restrict pInLocal1 = (int32_t *) pIn1;
   int32_t *restrict pInLocal2 = (int32_t *) pIn2;
   int64_t *restrict pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<int32_t>::type vec; // short16
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   typedef typename c7x::make_full_vector<int64_t>::type vec_out;

   vec_out out_dotp;
   vec_out out0_dotp;
   vec_out out1_dotp;
   out_dotp            = (vec_out) 0;
   int64_t result_dotp = 0;

   vec_out out_sqrAdd;
   vec_out out0_sqrAdd;
   vec_out out1_sqrAdd;
   out_sqrAdd            = (vec_out) 0;
   int64_t result_sqrAdd = 0;

   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      vec b = c7x::strm_eng<1, vec>::get_adv();

      __vmpywd_vvw(a, b, out0_dotp, out1_dotp);
      out_dotp += (out0_dotp + out1_dotp);

      __vmpywd_vvw(b, b, out0_sqrAdd, out1_sqrAdd);
      out_sqrAdd += (out0_sqrAdd + out1_sqrAdd);
   }

   // use intrensic for horizontal add
   result_dotp   = __horizontal_add(out_dotp);
   result_sqrAdd = __horizontal_add(out_sqrAdd);

   // print resutls

   *pOutLocal   = result_dotp;
   *++pOutLocal = result_sqrAdd;

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype uint32_t                        */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<uint32_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn1,
                                                void *restrict pIn2,
                                                void *restrict pOut)
{
   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;

   uint32_t *restrict pInLocal1 = (uint32_t *) pIn1;
   uint32_t *restrict pInLocal2 = (uint32_t *) pIn2;
   uint64_t *restrict pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<uint32_t>::type vec; // short16
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d\n", blockSize);
#endif

   typedef typename c7x::make_full_vector<uint64_t>::type vec_out;

   vec_out out_dotp;
   vec_out out0_dotp;
   vec_out out1_dotp;
   out_dotp             = (vec_out) 0;
   uint64_t result_dotp = 0;

   vec_out out_sqrAdd;
   vec_out out0_sqrAdd;
   vec_out out1_sqrAdd;
   out_sqrAdd             = (vec_out) 0;
   uint64_t result_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      vec b = c7x::strm_eng<1, vec>::get_adv();

      __vmpyuwd_vvw(a, b, out0_dotp, out1_dotp);
      out_dotp += (out0_dotp + out1_dotp);

      __vmpyuwd_vvw(b, b, out0_sqrAdd, out1_sqrAdd);
      out_sqrAdd += (out0_sqrAdd + out1_sqrAdd);
   }

   // use intrensic for horizontal add
   result_dotp   = __horizontal_add(out_dotp);
   result_sqrAdd = __horizontal_add(out_sqrAdd);

   *pOutLocal   = result_dotp;
   *++pOutLocal = result_sqrAdd;

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn1,
                                                      void *restrict pIn2,
                                                      void *restrict pOut);

template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn1,
                                                       void *restrict pIn2,
                                                       void *restrict pOut);

template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<int8_t>(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn1,
                                                       void *restrict pIn2,
                                                       void *restrict pOut);

template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<uint8_t>(DSPLIB_kernelHandle handle,
                                                        void *restrict pIn1,
                                                        void *restrict pIn2,
                                                        void *restrict pOut);

template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                        void *restrict pIn1,
                                                        void *restrict pIn2,
                                                        void *restrict pOut);

template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<uint16_t>(DSPLIB_kernelHandle handle,
                                                         void *restrict pIn1,
                                                         void *restrict pIn2,
                                                         void *restrict pOut);

template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                        void *restrict pIn1,
                                                        void *restrict pIn2,
                                                        void *restrict pOut);

template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_ci<uint32_t>(DSPLIB_kernelHandle handle,
                                                         void *restrict pIn1,
                                                         void *restrict pIn2,
                                                         void *restrict pOut);
