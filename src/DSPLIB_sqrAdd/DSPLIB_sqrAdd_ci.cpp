// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_sqrAdd_priv.h"
#include <float.h>

// Generic initialization
template <typename dataType>
DSPLIB_STATUS DSPLIB_sqrAdd_init_ci(DSPLIB_kernelHandle           handle,
                                    const DSPLIB_bufParams1D_t   *bufParamsIn,
                                    const DSPLIB_bufParams1D_t   *bufParamsOut,
                                    const DSPLIB_sqrAdd_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS           status       = DSPLIB_SUCCESS;
   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   SE_VECLEN                                                       = c7x::se_veclen<vec>::value;
   SE_ELETYPE                                                      = c7x::se_eletype<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   int32_t blockCount     = (blockSize % eleCount == 0) ? (blockSize / eleCount) : (blockSize / eleCount) + 1;
   int32_t blockCountHalf = (blockCount % 2 == 0) ? blockCount / 2 : blockCount / 2 + 1;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch the input                      */
   /**********************************************************************/
   se0Params               = __gen_SE_TEMPLATE_v1();
   uint32_t icnt0Param     = (blockSize < eleCount) ? blockSize : eleCount;
   se0Params.ICNT0         = icnt0Param;
   se0Params.ICNT1         = blockCountHalf;
   se0Params.DIM1          = eleCount * 2;
   se0Params.ELETYPE       = SE_ELETYPE;
   se0Params.VECLEN        = SE_VECLEN;
   se0Params.DECDIM1_WIDTH = blockSize;
   se0Params.DECDIM1       = __SE_DECDIM_DIM1;
   se0Params.DIMFMT        = __SE_DIMFMT_2D;

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch the input                      */
   /**********************************************************************/
   se1Params               = __gen_SE_TEMPLATE_v1();
   icnt0Param              = ((blockSize - eleCount) < eleCount) ? (blockSize - eleCount) : eleCount;
   se1Params.ICNT0         = icnt0Param;
   se1Params.ICNT1         = blockCountHalf;
   se1Params.DIM1          = eleCount * 2;
   se1Params.ELETYPE       = SE_ELETYPE;
   se1Params.VECLEN        = SE_VECLEN;
   se1Params.DECDIM1_WIDTH = blockSize - eleCount;
   se1Params.DECDIM1       = __SE_DECDIM_DIM1;
   se1Params.DIMFMT        = __SE_DIMFMT_2D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;

   return status;
}

template <>
DSPLIB_STATUS DSPLIB_sqrAdd_init_ci<int8_t>(DSPLIB_kernelHandle           handle,
                                            const DSPLIB_bufParams1D_t   *bufParamsIn,
                                            const DSPLIB_bufParams1D_t   *bufParamsOut,
                                            const DSPLIB_sqrAdd_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS           status       = DSPLIB_SUCCESS;
   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;

   __SE_PROMOTE SE_PROMOTE;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   int32_t eleCount = c7x::element_count_of<c7x::short_vec>::value;

   SE_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
   SE_ELETYPE = c7x::se_eletype<c7x::char_vec>::value;
   SE_PROMOTE = __SE_PROMOTE_2X_SIGNEXT;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint32_t blockCount     = (blockSize % eleCount == 0) ? (blockSize / eleCount) : (blockSize / eleCount) + 1;
   uint32_t blockCountHalf = (blockCount % 2 == 0) ? blockCount / 2 : blockCount / 2 + 1;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch the input                      */
   /**********************************************************************/
   se0Params               = __gen_SE_TEMPLATE_v1();
   uint32_t icnt0Param     = (blockSize < eleCount) ? blockSize : eleCount;
   se0Params.ICNT0         = icnt0Param;
   se0Params.ICNT1         = blockCountHalf;
   se0Params.DIM1          = eleCount * 2;
   se0Params.ELETYPE       = SE_ELETYPE;
   se0Params.VECLEN        = SE_VECLEN;
   se0Params.DECDIM1_WIDTH = blockSize;
   se0Params.DECDIM1       = __SE_DECDIM_DIM1;
   se0Params.DIMFMT        = __SE_DIMFMT_2D;
   se0Params.PROMOTE       = SE_PROMOTE;

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch the input                      */
   /**********************************************************************/
   se1Params               = __gen_SE_TEMPLATE_v1();
   icnt0Param              = ((blockSize - eleCount) < eleCount) ? (blockSize - eleCount) : eleCount;
   se1Params.ICNT0         = icnt0Param;
   se1Params.ICNT1         = blockCountHalf;
   se1Params.DIM1          = eleCount * 2;
   se1Params.ELETYPE       = SE_ELETYPE;
   se1Params.VECLEN        = SE_VECLEN;
   se1Params.DECDIM1_WIDTH = blockSize - eleCount;
   se1Params.DECDIM1       = __SE_DECDIM_DIM1;
   se1Params.DIMFMT        = __SE_DIMFMT_2D;
   se1Params.PROMOTE       = SE_PROMOTE;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_sqrAdd_init_ci<int16_t>(DSPLIB_kernelHandle           handle,
                                                      const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                      const DSPLIB_sqrAdd_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_sqrAdd_init_ci<uint16_t>(DSPLIB_kernelHandle           handle,
                                                       const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                       const DSPLIB_sqrAdd_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_sqrAdd_init_ci<int32_t>(DSPLIB_kernelHandle           handle,
                                                      const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                      const DSPLIB_sqrAdd_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_sqrAdd_init_ci<uint32_t>(DSPLIB_kernelHandle           handle,
                                                       const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                       const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                       const DSPLIB_sqrAdd_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_sqrAdd_init_ci<int8_t>(DSPLIB_kernelHandle           handle,
                                                     const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                     const DSPLIB_sqrAdd_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_sqrAdd_init_ci<uint8_t>(DSPLIB_kernelHandle           handle,
                                                      const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                      const DSPLIB_sqrAdd_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_sqrAdd_init_ci<float>(DSPLIB_kernelHandle           handle,
                                                    const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                    const DSPLIB_sqrAdd_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_sqrAdd_init_ci<double>(DSPLIB_kernelHandle           handle,
                                                     const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                     const DSPLIB_sqrAdd_InitArgs *pKerInitArgs);

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
DSPLIB_STATUS DSPLIB_sqrAdd_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   dataType *restrict pInLocal1 = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   dataType *restrict pInLocal2 = pInLocal1 + eleCount;
#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se1Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d pInLocal1 %p pInLocal2 %p\n", blockSize, pInLocal1, pInLocal2);
#endif

   vec outa, outb, outc, outd, oute, outf, outg, outh, outab, outcd, outef, outgh, out;

   outa  = (vec) 0.0;
   outb  = (vec) 0.0;
   outc  = (vec) 0.0;
   outd  = (vec) 0.0;
   oute  = (vec) 0.0;
   outf  = (vec) 0.0;
   outg  = (vec) 0.0;
   outh  = (vec) 0.0;
   outab = (vec) 0.0;
   outcd = (vec) 0.0;
   outef = (vec) 0.0;
   outgh = (vec) 0.0;
   out   = (vec) 0.0;

   dataType result;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount * 8) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      // a.print();
      vec b = c7x::strm_eng<1, vec>::get_adv();
      // b.print();
      outa += a * a;
      outb += b * b;
#if DSPLIB_DEBUGPRINT
#endif
      vec c = c7x::strm_eng<0, vec>::get_adv();
      // c.print();
      vec d = c7x::strm_eng<1, vec>::get_adv();
      // d.print();
      outc += c * c;
      outd += d * d;

      vec e = c7x::strm_eng<0, vec>::get_adv();
      // e.print();
      vec f = c7x::strm_eng<1, vec>::get_adv();
      // f.print();
      oute += e * e;
      outf += f * f;

      vec g = c7x::strm_eng<0, vec>::get_adv();
      // g.print();
      vec h = c7x::strm_eng<1, vec>::get_adv();
      // h.print();

      outg += g * g;
      outh += h * h;
   }

   outab = outa + outb;
   outcd = outc + outd;
   outef = oute + outf;
   outgh = outg + outh;
   out   = outab + outcd + outef + outgh;

   // out.print();
#if DSPLIB_DEBUGPRINT
   DSPLIB_debugPrintVector(out);
#endif
   result = DSPLIB_horiAdd(out);

   *pOutLocal = result;
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_ci result %lf\n", result);
#endif

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype int8_t                          */
/**********************************************************************/
// The input datatype of int8_t is promoted to int16_t using the streaming engine.
// The dotprod is then implemented as int16_t.
// When completed, the output of int64_t is casted down to int32_t.
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   int8_t *restrict pInLocal1  = (int8_t *) pIn;
   int32_t *restrict pOutLocal = (int32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<int16_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;

   int8_t *restrict pInLocal2 = pInLocal1 + eleCount;

#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se1Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d pInLocal1 %p pInLocal2 %p\n", blockSize, pInLocal1, pInLocal2);
#endif

   typedef typename c7x::make_full_vector<int64_t>::type vec_out;

   vec_out outa;
   vec_out outb;
   outa = (vec_out) 0;
   outb = (vec_out) 0;

   int64_t result = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount * 2) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      // a.print();
      vec b = c7x::strm_eng<1, vec>::get_adv();
      // b.print();

      outa += __vdotp4hd_vvv(a, a);
      outb += __vdotp4hd_vvv(b, b);

      // a.print();

      // outa.print();

      // b.print();

      // outb.print();

#if DSPLIB_DEBUGPRINT
#endif
   }

   outa = outa + outb;
   // out.print();
#if DSPLIB_DEBUGPRINT
#endif
   // use intrensic for horizontal add
   // cast the int64_t output of __horizontal_add to int32_t.
   result = (int32_t) __horizontal_add(outa);

   *pOutLocal = result;
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_ci result %lf\n", result);
#endif

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype uint8_t                         */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_ci<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   uint8_t *restrict pInLocal1  = (uint8_t *) pIn;
   uint32_t *restrict pOutLocal = (uint32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<uint8_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;

   uint8_t *restrict pInLocal2 = pInLocal1 + eleCount;
#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se1Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d pInLocal1 %p pInLocal2 %p\n", blockSize, pInLocal1, pInLocal2);
#endif

   typedef typename c7x::make_full_vector<uint32_t>::type vec_out;

   vec_out outa;
   vec_out outb;

   outa            = (vec_out) 0;
   outb            = (vec_out) 0;
   uint64_t result = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount * 2) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      // a.print();
      vec b = c7x::strm_eng<1, vec>::get_adv();
      // b.print();

      outa += __vdotp4ubw_vvv(a, a);
      outb += __vdotp4ubw_vvv(b, b);

      // a.print();

      // outa.print();

      // b.print();

      // outb.print();

#if DSPLIB_DEBUGPRINT
#endif
   }

   outa = outa + outb;

   // out.print();
#if DSPLIB_DEBUGPRINT
#endif
   // use intrensic for horizontal add
   // cast the uint64_t output of __horizontal_add to uint32_t.
   result = (uint32_t) __horizontal_add(outa);

   *pOutLocal = result;
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_ci result %lf\n", result);
#endif

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype int16_t                         */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   int16_t *restrict pInLocal1 = (int16_t *) pIn;
   int64_t *restrict pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<int16_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;

   int16_t *restrict pInLocal2 = pInLocal1 + eleCount;
#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se1Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d pInLocal1 %p pInLocal2 %p\n", blockSize, pInLocal1, pInLocal2);
#endif

   typedef typename c7x::make_full_vector<int64_t>::type vec_out;

   vec_out outa;
   vec_out outb;
   outa = (vec_out) 0;
   outb = (vec_out) 0;

   int64_t result = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount * 2) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      // a.print();
      vec b = c7x::strm_eng<1, vec>::get_adv();
      // b.print();

      outa += __vdotp4hd_vvv(a, a);
      outb += __vdotp4hd_vvv(b, b);

#if DSPLIB_DEBUGPRINT
#endif
   }

   outa = outa + outb;
   // out.print();
#if DSPLIB_DEBUGPRINT
#endif
   // use intrensic for horizontal add
   result = __horizontal_add(outa);

   *pOutLocal = result;
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_ci result %lf\n", result);
#endif

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype uint16_t                        */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   uint16_t *restrict pInLocal1 = (uint16_t *) pIn;
   uint64_t *restrict pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<uint16_t>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   uint16_t *restrict pInLocal2 = pInLocal1 + eleCount;
#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se1Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d pInLocal1 %p pInLocal2 %p\n", blockSize, pInLocal1, pInLocal2);
#endif

   typedef typename c7x::make_full_vector<uint64_t>::type vec_out;

   vec_out outa;
   vec_out outb;

   outa            = (vec_out) 0;
   outb            = (vec_out) 0;
   uint64_t result = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount * 2) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      // a.print();
      vec b = c7x::strm_eng<1, vec>::get_adv();
      // b.print();

      outa += __vdotp4uhd_vvv(a, a);
      outb += __vdotp4uhd_vvv(b, b);

#if DSPLIB_DEBUGPRINT
#endif
   }

   outa = outa + outb;

   // out.print();
#if DSPLIB_DEBUGPRINT
#endif
   // use intrensic for horizontal add
   result = __horizontal_add(outa);

   *pOutLocal = result;
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_ci result %lf\n", result);
#endif

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype int32_t                         */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   int32_t *restrict pInLocal1 = (int32_t *) pIn;
   int64_t *restrict pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<int32_t>::type vec;
   int32_t                                               eleCount = c7x::element_count_of<vec>::value;

   int32_t *restrict pInLocal2 = pInLocal1 + eleCount;
#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se1Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d pInLocal1 %p pInLocal2 %p\n", blockSize, pInLocal1, pInLocal2);
#endif

   typedef typename c7x::make_full_vector<int64_t>::type vec_out;

   vec_out out;
   vec_out outa0;
   vec_out outa1;
   vec_out outb0;
   vec_out outb1;

   out            = (vec_out) 0;
   int64_t result = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount * 2) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      // a.print();
      vec b = c7x::strm_eng<1, vec>::get_adv();
      // b.print();

      __vmpywd_vvw(a, a, outa0, outa1);
      __vmpywd_vvw(b, b, outb0, outb1);

      out += (outa0 + outa1 + outb0 + outb1);

#if DSPLIB_DEBUGPRINT
#endif
   }

   // out.print();
#if DSPLIB_DEBUGPRINT
#endif
   // use intrensic for horizontal add
   result = __horizontal_add(out);

   *pOutLocal = result;
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_ci result %lf\n", result);
#endif

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

/**********************************************************************/
/*               Execute for datatype uint32_t                        */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;

   uint32_t *restrict pInLocal1 = (uint32_t *) pIn;
   uint64_t *restrict pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<uint32_t>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   uint32_t *restrict pInLocal2 = pInLocal1 + eleCount;
#if DSPLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal1, se0Params);
   __SE1_OPEN(pInLocal2, se1Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT blockSize %d pInLocal1 %p pInLocal2 %p\n", blockSize, pInLocal1, pInLocal2);
#endif

   typedef typename c7x::make_full_vector<uint64_t>::type vec_out;

   vec_out out;
   vec_out outa0;
   vec_out outa1;
   vec_out outb0;
   vec_out outb1;

   out             = (vec_out) 0;
   uint64_t result = 0;
   for (int32_t counter = 0; counter < blockSize; counter += eleCount * 2) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      // a.print();
      vec b = c7x::strm_eng<1, vec>::get_adv();
      // b.print();

      __vmpyuwd_vvw(a, a, outa0, outa1);
      __vmpyuwd_vvw(b, b, outb0, outb1);

      out += (outa0 + outa1 + outb0 + outb1);

#if DSPLIB_DEBUGPRINT
#endif
   }

   // out.print();
#if DSPLIB_DEBUGPRINT
#endif
   // use intrensic for horizontal add
   result = __horizontal_add(out);

   *pOutLocal = result;
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_ci result %lf\n", result);
#endif

   __SE0_CLOSE();
   __SE1_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

// template DSPLIB_STATUS

// template DSPLIB_STATUS

template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_ci<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_ci<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_ci<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_ci<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
