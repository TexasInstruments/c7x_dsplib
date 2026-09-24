// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_diag_priv.h"
#include <float.h>

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)

// Diagonal elements gathered per SE fetch: transpose mode (<= 16 rows) for 32-bit and wider elements
template <typename dataType> static inline int32_t DSPLIB_diag_rows()
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t eleCount = c7x::element_count_of<vec>::value;
   int32_t maxRows  = (eleCount > 16) ? 16 : eleCount;
   return (sizeof(dataType) < 4) ? 1 : maxRows;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_diag_init_ci(DSPLIB_kernelHandle         handle,
                                  const DSPLIB_bufParams2D_t *bufParamsIn,
                                  const DSPLIB_bufParams1D_t *bufParamsOut,
                                  const DSPLIB_diag_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   DSPLIB_diag_PrivArgs *pKerPrivArgs = (DSPLIB_diag_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   uint32_t order  = pKerPrivArgs->order;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch the input                      */
   /**********************************************************************/

   // Transpose mode gathers 'rows' diagonal elements per fetch (one per matrix row, stride order+1).
   // The row count is limited by the vector width and by the SE transpose maximum of 16 rows.
   // 8-/16-bit real types would need decimation + promotion in transpose mode, so they keep the
   // one-element-per-fetch stream (rows = 1).
   int32_t rows = DSPLIB_diag_rows<dataType>();
   int32_t iter = (order + rows - 1) / rows;

   se0Params = __gen_SE_TEMPLATE_v1();

   se0Params.ICNT0   = 1;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   if (rows > 1) {
      se0Params.TRANSPOSE     = (sizeof(dataType) == 4) ? __SE_TRANSPOSE_32BIT : __SE_TRANSPOSE_64BIT;
      se0Params.ICNT1         = rows;
      se0Params.DIM1          = order + 1;
      se0Params.ICNT2         = iter;
      se0Params.DIM2          = rows * (order + 1);
      se0Params.DECDIM1       = __SE_DECDIM_DIM2;
      se0Params.DECDIM1_WIDTH = order * (order + 1);
      se0Params.DIMFMT        = __SE_DIMFMT_3D;
   }
   else {
      se0Params.ICNT1  = order;
      se0Params.DIM1   = order + 1;
      se0Params.DIMFMT = __SE_DIMFMT_2D;
   }

   /**********************************************************************/
   /* Prepare SA template to store output                                */
   /**********************************************************************/
   sa0Params = __gen_SA_TEMPLATE_v1();

   sa0Params.ICNT0  = rows;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.ICNT1  = iter;
   sa0Params.DIM1   = rows;
   if (rows > 1) {
      sa0Params.DECDIM1       = __SA_DECDIM_DIM1;
      sa0Params.DECDIM1_WIDTH = order;
   }
   sa0Params.DIMFMT = __SA_DIMFMT_2D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

// Diagonal elements gathered per SE fetch for a given data type (used by init and perfEst)
int32_t DSPLIB_diag_getRowsPerFetch(int32_t dataType)
{
   switch (dataType) {
   case DSPLIB_FLOAT32:
      return DSPLIB_diag_rows<float>();
   case DSPLIB_FLOAT64:
      return DSPLIB_diag_rows<double>();
   case DSPLIB_INT8:
      return DSPLIB_diag_rows<int8_t>();
   case DSPLIB_UINT8:
      return DSPLIB_diag_rows<uint8_t>();
   case DSPLIB_INT16:
      return DSPLIB_diag_rows<int16_t>();
   case DSPLIB_UINT16:
      return DSPLIB_diag_rows<uint16_t>();
   case DSPLIB_INT32:
      return DSPLIB_diag_rows<int32_t>();
   case DSPLIB_UINT32:
      return DSPLIB_diag_rows<uint32_t>();
   case DSPLIB_CFLOAT32:
      return DSPLIB_diag_rows<cfloat>();
   case DSPLIB_CINT16:
      return DSPLIB_diag_rows<cshort>();
   case DSPLIB_CINT32:
      return DSPLIB_diag_rows<cint>();
   default:
      return 1;
   }
}

void DSPLIB_diag_perfEst(DSPLIB_kernelHandle handle, uint64_t *archCycles, uint64_t *estCycles)
{
   DSPLIB_diag_PrivArgs *pKerPrivArgs = (DSPLIB_diag_PrivArgs *) handle;

   uint64_t rows  = (uint64_t) pKerPrivArgs->rowsPerFetch;
   uint64_t nVecs = ((uint64_t) pKerPrivArgs->order + rows - 1) / rows;
   *archCycles    = 46 + nVecs;
   *estCycles     = *archCycles;
}

template DSPLIB_STATUS DSPLIB_diag_init_ci<int16_t>(DSPLIB_kernelHandle         handle,
                                                    const DSPLIB_bufParams2D_t *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t *bufParamsOut,
                                                    const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<uint16_t>(DSPLIB_kernelHandle         handle,
                                                     const DSPLIB_bufParams2D_t *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t *bufParamsOut,
                                                     const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<int32_t>(DSPLIB_kernelHandle         handle,
                                                    const DSPLIB_bufParams2D_t *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t *bufParamsOut,
                                                    const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<uint32_t>(DSPLIB_kernelHandle         handle,
                                                     const DSPLIB_bufParams2D_t *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t *bufParamsOut,
                                                     const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<int8_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsOut,
                                                   const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<uint8_t>(DSPLIB_kernelHandle         handle,
                                                    const DSPLIB_bufParams2D_t *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t *bufParamsOut,
                                                    const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<float>(DSPLIB_kernelHandle         handle,
                                                  const DSPLIB_bufParams2D_t *bufParamsIn,
                                                  const DSPLIB_bufParams1D_t *bufParamsOut,
                                                  const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<double>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsOut,
                                                   const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<cfloat>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsOut,
                                                   const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<cshort>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsOut,
                                                   const DSPLIB_diag_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_diag_init_ci<cint>(DSPLIB_kernelHandle         handle,
                                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                                 const DSPLIB_bufParams1D_t *bufParamsOut,
                                                 const DSPLIB_diag_InitArgs *pKerInitArgs);
template <typename dataType, int32_t dataIn>
DSPLIB_STATUS DSPLIB_diag_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_diag_PrivArgs *pKerPrivArgs = (DSPLIB_diag_PrivArgs *) handle;
   uint32_t              order        = pKerPrivArgs->order;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_diag_exec_ci\n");

#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;

#if DSPLIB_DEBUGPRINT
   int32_t eleCount = c7x::element_count_of<vec>::value;
   printf("Enter eleCount %d\n", eleCount);
#endif

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal, se0Params);

   // Output samples
   __SA0_OPEN(sa0Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT order %d\n", order);
#endif

   int32_t rows = DSPLIB_diag_rows<dataType>();
   int32_t iter = (order + rows - 1) / rows;

   for (int32_t counter = 0; counter < iter; counter++) {
      vec a = c7x::strm_eng<0, vec>::get_adv();

      __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
      __vstore_pred(tmp, VB1, a);
#if DSPLIB_DEBUGPRINT
      DSPLIB_debugPrintVector(a);
      // DSPLIB_debugPrintVector(*VB1);
#endif
   }
   __SE0_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS
DSPLIB_diag_exec_ci<float, DSPLIB_FLOAT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<double, DSPLIB_FLOAT64>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<int8_t, DSPLIB_INT8>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<uint8_t, DSPLIB_UINT8>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<uint16_t, DSPLIB_UINT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<int32_t, DSPLIB_INT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<uint32_t, DSPLIB_UINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<cfloat, DSPLIB_CFLOAT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<cshort, DSPLIB_CINT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_diag_exec_ci<cint, DSPLIB_CINT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
