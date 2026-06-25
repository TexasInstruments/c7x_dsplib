// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_priv.h"

/* *****************************************************************************
 *
 * DEFINES
 *
 ***************************************************************************** */

#define MAX_ITERATION_COUNT 30

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */

template <typename dataType>
DSPLIB_STATUS DSPLIB_svd_init_ci(DSPLIB_kernelHandle         handle,
                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                 const DSPLIB_bufParams2D_t *bufParamsU,
                                 const DSPLIB_bufParams2D_t *bufParamsV,
                                 const DSPLIB_bufParams1D_t *bufParamsDiag,
                                 const DSPLIB_bufParams1D_t *bufParamsSuperDiag,
                                 const DSPLIB_svdInitArgs   *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_bidiag_u_init_ci<dataType>(handle);
   DSPLIB_bidiag_v_init_ci<dataType>(handle);

   DSPLIB_bidiag_uFinal_init_ci<dataType>(handle);

   DSPLIB_diag_proc_init_ci<dataType>(handle);

   DSPLIB_singularSort_swap_init_ci<dataType>(handle);

   DSPLIB_svd_matTrans_init_ci<dataType>(handle, pKerInitArgs);

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_svd_init_ci<float>(DSPLIB_kernelHandle         handle,
                                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                                 const DSPLIB_bufParams2D_t *bufParamsU,
                                                 const DSPLIB_bufParams2D_t *bufParamsV,
                                                 const DSPLIB_bufParams1D_t *bufParamsDiag,
                                                 const DSPLIB_bufParams1D_t *bufParamsSuperDiag,
                                                 const DSPLIB_svdInitArgs   *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_svd_init_ci<double>(DSPLIB_kernelHandle         handle,
                                                  const DSPLIB_bufParams2D_t *bufParamsIn,
                                                  const DSPLIB_bufParams2D_t *bufParamsU,
                                                  const DSPLIB_bufParams2D_t *bufParamsV,
                                                  const DSPLIB_bufParams1D_t *bufParamsDiag,
                                                  const DSPLIB_bufParams1D_t *bufParamsSuperDiag,
                                                  const DSPLIB_svdInitArgs   *pKerInitArgs);

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */

template <typename dataType>
int DSPLIB_svd_convert_to_bidiag_ci(const int Nrows,
                                    const int Ncols,
                                    dataType *U,
                                    dataType *V,
                                    dataType *diag,
                                    dataType *superdiag,
                                    const int colUStride,
                                    const int colVStride,
                                    uint32_t  enableReducedForm,
                                    dataType *U1,
                                    uint8_t  *pBlock)
{
   int      i;
   dataType s, scale, half_norm_squared;

   /* Householder processing */
   s     = 0;
   scale = 0;
   for (i = 0; i < Ncols; i++) {
      superdiag[i] = scale * s;
      /* process columns */
      scale = DSPLIB_bidiag_uCol_halfnorm_ci<dataType>(&U[i + i * colUStride], Nrows - i, Ncols, colUStride,
                                                       &half_norm_squared, U1, &s, pBlock);
      U[i + i * colUStride] = U[i + i * colUStride] - (s * scale);
      if (Ncols - (i + 1) > 0 && scale > 0) {
         DSPLIB_bidiag_uCol_ci<dataType>(&U[i + i * colUStride], Nrows - i, Ncols - (i + 1), colUStride,
                                         half_norm_squared, U1, scale, pBlock);
      }
      diag[i] = s * scale;
      /* process rows */
      s     = 0;
      scale = 0;
      if ((i != Ncols - 1)) {
         scale = DSPLIB_bidiag_uRow_halfnorm_ci<dataType>(&U[(i + 1) + (i * colUStride)], Nrows - i, Ncols - (i + 1),
                                                          colUStride, &half_norm_squared, U1, &s, &superdiag[i + 1],
                                                          pBlock);
         if ((scale > 0)) {
            DSPLIB_bidiag_uRow_ci<dataType>(&U[(i + 1) + (i * colUStride)], Nrows - (i + 1), Ncols - (i + 1),
                                            colUStride, &superdiag[i + 1], U1, scale, pBlock);
         }
      } /* if ((i<Nrows)&&(i!=Ncols-1)) */
   }    /* for (i=0;i<Ncols;i++) */

   /* update V */
   V[(Ncols - 1) + (Ncols - 1) * colVStride] = 1;
   s                                         = superdiag[Ncols - 1];
   for (i = Ncols - 2; i >= 0; i--) {
      DSPLIB_bidiag_v_ci<dataType>(&V[i + (i) *colVStride], Ncols - i - 1, Ncols - i, colVStride, s, U1,
                                   &U[(i) + i * colUStride], colUStride, pBlock);
      s = superdiag[i];
   } /* for (i=Ncols-2;i>=0;i--) */
   if (enableReducedForm == 0u) {
      /* expand U to from Nrows x Ncols to */
      /*                  Nrows x Nrows    */
      if (Nrows > Ncols) {
         DSPLIB_bidiag_uFinal_expand_ci<dataType>(U, Nrows, Ncols, colUStride, pBlock);
      }
      {
         i = Ncols - 1;
         s = diag[i];
         if (Nrows - (i + 1) > 0) {

            DSPLIB_bidiag_uFinal_initalize_ci(&U[i + i * colUStride], Nrows - i, Nrows - (i + 1), colUStride, s, U1,
                                              pBlock);
         }
         DSPLIB_bidiag_uFinal_normalize_ci(&U[i + i * colUStride], Nrows - i, s, colUStride, pBlock);
         U[i + i * colUStride] += 1;
      }

      /* update U */
      for (i = Ncols - 2; i >= 0; i--) {
         s = diag[i];
         DSPLIB_bidiag_uFinal_ci(&U[i + i * colUStride], Nrows - i, Nrows - (i + 1), colUStride, s, U1, pBlock);
         DSPLIB_bidiag_uFinal_normalize_ci(&U[i + i * colUStride], Nrows - i, s, colUStride, pBlock);
         U[i + i * colUStride] += 1;
      } /* for (i=Ncols-1;i>=0;i--) */
   }    /* if (!enableReducedForm) */

   else /* if (enableReducedForm) */
   {
      /* update U */
      for (i = Ncols - 1; i >= 0; i--) {
         s = diag[i];
         if (i != Ncols - 1) {
            DSPLIB_bidiag_uFinal_ci(&U[i + i * colUStride], Nrows - i, Ncols - (i + 1), colUStride, s, U1, pBlock);
         }
         DSPLIB_bidiag_uFinal_normalize_ci(&U[i + i * colUStride], Nrows - i, s, colUStride, pBlock);

         U[i + i * colUStride] += 1;
      } /* for (i=Ncols-1;i>=0;i--) */
   }    /* if (enableReducedForm) */
   return 0;
}
template int DSPLIB_svd_convert_to_bidiag_ci<float>(const int Nrows,
                                                    const int Ncols,
                                                    float    *U,
                                                    float    *V,
                                                    float    *diag,
                                                    float    *superdiag,
                                                    const int colUStride,
                                                    const int colVStride,
                                                    uint32_t  enableReducedForm,
                                                    float    *U1,
                                                    uint8_t  *pBlock);
template int DSPLIB_svd_convert_to_bidiag_ci<double>(const int Nrows,
                                                     const int Ncols,
                                                     double   *U,
                                                     double   *V,
                                                     double   *diag,
                                                     double   *superdiag,
                                                     const int colUStride,
                                                     const int colVStride,
                                                     uint32_t  enableReducedForm,
                                                     double   *U1,
                                                     uint8_t  *pBlock);

template <typename dataType> static inline dataType getSqrt(dataType a)
{
   const dataType Half  = 0.5f;
   const dataType OneP5 = 1.5f;
   dataType       x, y;

   x = __recip_sqrt(a); /* compute square root reciprocal */

   // #pragma UNROLL(1) /* PRAGMA: do not unroll this loop */
   {
      x = x * (OneP5 - (a * x * x * Half));
      x = x * (OneP5 - (a * x * x * Half));
   }
   y = a * x;

   return (y);
}
template float  getSqrt<float>(float a);
template double getSqrt<double>(double a);

template <typename dataType> static inline dataType getRecipSqrt(dataType a)
{
   const dataType Half  = 0.5f;
   const dataType OneP5 = 1.5f;
   dataType       x;

   x = __recip_sqrt(a); // compute square root reciprocal

   // PRAGMA: do not unroll this loop
   // #pragma UNROLL(1)
   {
      x = x * (OneP5 - (a * x * x * Half));
      x = x * (OneP5 - (a * x * x * Half));
   }

   return x;
}
template float  getRecipSqrt<float>(float a);
template double getRecipSqrt<double>(double a);

template <typename dataType>
int DSPLIB_svd_bidiag_to_diag_ci(const int Nrows,
                                 const int Ncols,
                                 dataType *U,
                                 dataType *V,
                                 dataType *diag,
                                 dataType *superdiag,
                                 dataType *pTemp,
                                 const int colUStride,
                                 const int rowUStride,
                                 const int colVStride,
                                 const int rowVStride,
                                 uint32_t  enableReducedForm,
                                 uint8_t  *pBlock)
{
   int       i, k, rotation_test, iter, total_iter;
   int       m = 0;
   dataType  x, y, z, epsilon;
   dataType  c, s, f, g, h;
   dataType *cU = &pTemp[0 * Ncols];
   dataType *sU = &pTemp[1 * Ncols];
   dataType *cV = &pTemp[2 * Ncols];
   dataType *sV = &pTemp[3 * Ncols];
   iter         = 0;
   total_iter   = 0;
   /* ------------------------------------------------------------------- */
   /* find max in col                                                     */
   /* ------------------------------------------------------------------- */

   DSPLIB_diag_epsilon_ci(diag, superdiag, &epsilon, Ncols, pBlock);

   for (k = Ncols - 1; k >= 0; k--) {
      total_iter += iter;
      iter = 0;
      while (true) {
         rotation_test = 1;
         DSPLIB_diag_rotation_check_ci(&diag[k - 1], &superdiag[k], epsilon, &m, &rotation_test, (k + 1), pBlock);

         if (rotation_test) {
            int32_t loopCnt = 0;
            c               = 0;
            s               = 1;
            for (i = m; i <= k; i++) {
               f            = s * superdiag[i];
               superdiag[i] = c * superdiag[i];
#if !defined(ENABLE_LDRA_COVERAGE)
               /* This part of code checks for "test f convergence" part condition
                  Ref. Singular Value Decomposition and Least Squares Solutions. G. H. Golub et al
                  We use these conditions in order to calculate correct results if and when they occur */
               if (fabs(f) <= epsilon) {
                  break;
               }
#endif
               g               = diag[i];
               h               = getSqrt(f * f + g * g);
               diag[i]         = h;
               dataType recipH = getRecip(h);
               c               = g * recipH;
               s               = -f * recipH;
               cU[loopCnt]     = c;
               sU[loopCnt]     = s;
               loopCnt++;
            }

#if ORIGINAL_IMPL
            for (i = 0; i <= loopCnt - 1; i++) {
               for (row = 0; row < Nrows; row++) {
                  y                             = U[(m - 1) * rowUStride + row];
                  z                             = U[(i + m) * rowUStride + row];
                  U[(m - 1) * rowUStride + row] = y * cU[i] + z * sU[i];
                  U[(i + m) * rowUStride + row] = -y * sU[i] + z * cU[i];
               }
            } /* for (i=m;i<=k;i++) */
#else
            DSPLIB_diag_rotation_proc_ci<dataType>(&U[(m - 1) * rowUStride], m, loopCnt, Nrows, rowUStride, cU, sU,
                                                   pBlock);
#endif
         } /* if (rotation_test) */

         z = diag[k];
         if (m == k) { /* "test f convergence" part */
            if (z < 0) {
               diag[k] = -z;
               DSPLIB_diag_negate_v_ci(&V[k * rowVStride], Ncols, rowVStride, pBlock);
            } /* if (z>0) */
            break;
         } /* if (m==k) */
         else {
#if !defined(ENABLE_LDRA_COVERAGE)
            /* This part of code retricts the count of "test f splitting" part
               Ref. Singular Value Decomposition and Least Squares Solutions. G. H. Golub et al
               We use these conditions in order to break the while loop to avoid infinite loop */
            if (iter >= MAX_ITERATION_COUNT) {
               total_iter = -1;
               break;
            }
#endif
            iter++;
            x = diag[m];
            y = diag[k - 1];
            g = superdiag[k - 1];
            h = superdiag[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) * getRecip((2 * h * y));
            g = getSqrt(f * f + 1);
            if (f < 0) {
               g = -g;
            }
            f = ((x - z) * (x + z) + h * (y * getRecip((f + g)) - h)) * getRecip(x);

            /* next QR transformation */
            c = 1;
            s = 1;

            uint32_t loopCnt = (k - m);
            i                = m + 1;
            uint32_t cnt     = 0;
            dataType c1, s1, c2, s2, c3, s3, recipz;
            for (; cnt < loopCnt - 1; cnt = cnt + 2) {
               h                = s * superdiag[i];
               g                = superdiag[i] * c;
               z                = (f * f + h * h);
               superdiag[i - 1] = z;

               recipz = getRecipSqrt(z);

               /* 21 cycles*/
               c1          = f * recipz;
               s1          = h * recipz;
               f           = x * c1 + g * s1;
               g           = -x * s1 + g * c1;
               h           = diag[i] * s1;
               y           = c1 * diag[i];
               z           = (f * f + h * h);
               diag[i - 1] = z;

               c3      = c1;
               s3      = s1;
               cV[cnt] = c1;
               sV[cnt] = s1;
#if !defined(ENABLE_LDRA_COVERAGE)
               /* This part of code test value of z in order to avoid the "inf" result */
               if (z != 0) {
                  recipz = getRecipSqrt(z);
                  c3     = f * recipz;
                  s3     = h * recipz;
               }
#else
               recipz = getRecipSqrt(z);
               c3     = f * recipz;
               s3     = h * recipz;
#endif
               /* 22 cycle */
               f            = c3 * g + s3 * y;
               x            = -s3 * g + c3 * y;
               h            = s3 * superdiag[i + 1];
               g            = superdiag[i + 1] * c3;
               z            = (f * f + h * h);
               superdiag[i] = z;
               cU[cnt]      = c3;
               sU[cnt]      = s3;
               recipz       = getRecipSqrt(z);

               /* 21 cycle*/
               c2 = f * recipz;
               s2 = h * recipz;
               f  = x * c2 + g * s2;
               g  = -x * s2 + g * c2;
               h  = diag[i + 1] * s2;
               y  = c2 * diag[i + 1];

               z           = (f * f + h * h);
               diag[i]     = z;
               c           = c2;
               s           = s2;
               cV[cnt + 1] = c2;
               sV[cnt + 1] = s2;
#if !defined(ENABLE_LDRA_COVERAGE)
               /* This part of code test value of z in order to avoid the "inf" result */
               if (z != 0) {
                  /* 1 + 2 *19 + 19*/
                  recipz = getRecipSqrt(z);
                  c      = f * recipz;
                  s      = h * recipz;
               }

#else
               recipz = getRecipSqrt(z);
               c      = f * recipz;
               s      = h * recipz;
#endif
               f = c * g + s * y;
               x = -s * g + c * y;
               /* 11 cycle */
               cU[cnt + 1] = c;
               sU[cnt + 1] = s;
               i           = i + 2;
            } /* loopCnt */
            if (cnt < loopCnt) {
               g                = superdiag[i];
               y                = diag[i];
               h                = s * g;
               g                = g * c;
               z                = (f * f + h * h);
               recipz           = getRecipSqrt(z);
               superdiag[i - 1] = z;
               c                = f * recipz;
               s                = h * recipz;
               f                = x * c + g * s;
               g                = -x * s + g * c;
               h                = y * s;
               y                = c * y;
               cV[cnt]          = c;
               sV[cnt]          = s;
               z                = (f * f + h * h);
               recipz           = getRecipSqrt(z);
               diag[i - 1]      = z;
#if !defined(ENABLE_LDRA_COVERAGE)
               /* This part of code test value of z in order to avoid the "inf" result */
               if (z != 0) {
                  c = f * recipz;
                  s = h * recipz;
               }
#else
               c      = f * recipz;
               s      = h * recipz;
#endif
               cU[cnt] = c;
               sU[cnt] = s;
               f       = c * g + s * y;
               x       = -s * g + c * y;
            }
            DSPLIB_diag_sqrt_ci<dataType>(&superdiag[m], &diag[m], (k - m), pBlock);
            DSPLIB_diag_proc_ci(V, m, loopCnt + 1, Ncols, rowVStride, cV, sV, pBlock);
            DSPLIB_diag_proc_ci(U, m, loopCnt + 1, Nrows, rowUStride, cU, sU, pBlock);
            superdiag[m] = 0;
            superdiag[k] = f;
            diag[k]      = x;
         } /* if (m==k) */
      }    /* while (1==1) */
   }       /* for (k=Ncols-1:k>=0;k--) */

   return total_iter;
}
template int DSPLIB_svd_bidiag_to_diag_ci<float>(const int Nrows,
                                                 const int Ncols,
                                                 float    *U,
                                                 float    *V,
                                                 float    *diag,
                                                 float    *superdiag,
                                                 float    *pTemp,
                                                 const int colUStride,
                                                 const int rowUStride,
                                                 const int colVStride,
                                                 const int rowVStride,
                                                 uint32_t  enableReducedForm,
                                                 uint8_t  *pBlock);
template int DSPLIB_svd_bidiag_to_diag_ci<double>(const int Nrows,
                                                  const int Ncols,
                                                  double   *U,
                                                  double   *V,
                                                  double   *diag,
                                                  double   *superdiag,
                                                  double   *pTemp,
                                                  const int colUStride,
                                                  const int rowUStride,
                                                  const int colVStride,
                                                  const int rowVStride,
                                                  uint32_t  enableReducedForm,
                                                  uint8_t  *pBlock);

template <typename dataType>
int DSPLIB_svd_sort_singular_values_ci(const int Nrows,
                                       const int Ncols,
                                       dataType *U,
                                       dataType *U1,
                                       dataType *V,
                                       dataType *V1,
                                       dataType *singular_values,
                                       dataType *pScratch,
                                       const int colUStride,
                                       const int rowUStride,
                                       const int colVStride,
                                       const int rowVStride,
                                       uint32_t  enableReducedForm,
                                       uint8_t  *pBlock)
{
   int32_t  *maxIndArr      = (int32_t *) &pScratch[1 * Ncols];
   dataType *sortedSingular = &pScratch[2 * Ncols];
   /* -----------------------------------------------------------------------------------
      Sort the singular values in descending order and change the order of column vectors
      in U and V matrix. Here the input U and V matrix are in transposed for therefore
      rows vectors are ordered and the after the function call matrices are transposed back
    --------------------------------------------------------------------------------------*/

   DSPLIB_svd_blk_move_ci<dataType>(sortedSingular, singular_values, 1, Ncols, 0, 0, pBlock);
   /* Sort singular values in descending order and store the indices in maxIndArr buffer */
   DSPLIB_singularSort_index_ci(singular_values, sortedSingular, maxIndArr, Ncols, pBlock);

   /* Use the indices stored in maxIndArr buffer to reorder the rows of U' and V' and store in
      U1 and V1 buffer respectively*/
   DSPLIB_singularSort_swap_ci(U, Ncols, Nrows, rowUStride, maxIndArr, U1, pBlock);
   DSPLIB_singularSort_swap_ci(V, Ncols, Ncols, rowVStride, maxIndArr, V1, pBlock);

   /* Copy back reordered matrices from U1 and V1 to U and V respectively */
   DSPLIB_svd_blk_move_ci<dataType>(U, U1, Ncols, Nrows, colUStride, rowUStride, pBlock);
   DSPLIB_svd_blk_move_ci<dataType>(V, V1, Ncols, Ncols, colVStride, rowVStride, pBlock);

   return 0;
}
template int DSPLIB_svd_sort_singular_values_ci<float>(const int Nrows,
                                                       const int Ncols,
                                                       float    *U,
                                                       float    *U1,
                                                       float    *V,
                                                       float    *V1,
                                                       float    *singular_values,
                                                       float    *pScratch,
                                                       const int colUStride,
                                                       const int rowUStride,
                                                       const int colVStride,
                                                       const int rowVStride,
                                                       uint32_t  enableReducedForm,
                                                       uint8_t  *pBlock);
template int DSPLIB_svd_sort_singular_values_ci<double>(const int Nrows,
                                                        const int Ncols,
                                                        double   *U,
                                                        double   *U1,
                                                        double   *V,
                                                        double   *V1,
                                                        double   *singular_values,
                                                        double   *pScratch,
                                                        const int colUStride,
                                                        const int rowUStride,
                                                        const int colVStride,
                                                        const int rowVStride,
                                                        uint32_t  enableReducedForm,
                                                        uint8_t  *pBlock);

template <typename dataType>
DSPLIB_STATUS DSPLIB_svd_exec_ci(DSPLIB_kernelHandle handle,
                                 void *restrict pA,
                                 void *restrict pU,
                                 void *restrict pV,
                                 void *restrict pDiag,
                                 void *restrict pSuperDiag,
                                 void *restrict pU1,
                                 void *restrict pV1,
                                 void *restrict pScratch)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_svd_PrivArgs      *pKerPrivArgs          = (DSPLIB_svd_PrivArgs *) handle;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs1 = &pKerPrivArgs->pMatTransKerPrivArgs1;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs2 = &pKerPrivArgs->pMatTransKerPrivArgs2;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs3 = &pKerPrivArgs->pMatTransKerPrivArgs3;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs4 = &pKerPrivArgs->pMatTransKerPrivArgs4;
   DSPLIB_matTrans_PrivArgs *pMatTransKerPrivArgs5 = &pKerPrivArgs->pMatTransKerPrivArgs5;

   /* Typecast void pointers to respective data type */
   dataType *pALocal         = (dataType *) pA;
   dataType *pULocal         = (dataType *) pU;
   dataType *pVLocal         = (dataType *) pV;
   dataType *pDiagLocal      = (dataType *) pDiag;
   dataType *pSuperDiagLocal = (dataType *) pSuperDiag;
   dataType *pU1Local        = (dataType *) pU1;
   dataType *pV1Local        = (dataType *) pV1;
   dataType *pScratchLocal   = (dataType *) pScratch;

   DSPLIB_DEBUGPRINTFN(0, "pALocal: %p pOutLocal: %p\n", pALocal, pULocal);

   uint8_t *pBlock            = pKerPrivArgs->bufPblock;
   uint32_t Nrows             = pKerPrivArgs->heightIn;
   uint32_t Ncols             = pKerPrivArgs->widthIn;
   int32_t  strideIn          = pKerPrivArgs->strideIn;
   int32_t  strideU           = pKerPrivArgs->strideU;
   int32_t  strideURows       = pKerPrivArgs->strideURows;
   int32_t  strideV           = pKerPrivArgs->strideV;
   int32_t  strideVRows       = pKerPrivArgs->strideVRows;
   uint32_t enableReducedForm = pKerPrivArgs->enableReducedForm;

   int Nrows1 = 0;
   int Ncols1 = 0;

   /* ------------------------------------------------------------------- */
   /* copy A matrix to U                                                  */
   /* ------------------------------------------------------------------- */

   if (Nrows >= Ncols) {
      Nrows1 = Nrows;
      Ncols1 = Ncols;
   }
   else {
      Nrows1 = Ncols;
      Ncols1 = Nrows;
   }

   int32_t dataSize   = sizeof(dataType);
   int32_t colUStride = strideU / dataSize;
   int32_t rowUStride = strideURows / dataSize;
   int32_t colVStride = strideV / dataSize;
   int32_t rowVStride = strideVRows / dataSize;
   int32_t colAStride = strideIn / dataSize;
   if (Nrows >= Ncols) {
      /* Copy A to U */
      DSPLIB_svd_blk_move_ci<dataType>(pULocal, pALocal, Nrows1, Ncols1, colUStride, colAStride, pBlock);
   }
   else {
      /* Copy A' to U */
      DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs1, pALocal, pULocal);
   }

   /* ------------------------------------------------------------------- */
   /* convert A to bidiagonal matrix using Householder reflections        */
   /* ------------------------------------------------------------------- */
   DSPLIB_svd_convert_to_bidiag_ci<dataType>(Nrows1, Ncols1, pULocal, pV1Local, pDiagLocal, pSuperDiagLocal, colUStride,
                                             colVStride, enableReducedForm, pScratchLocal, pBlock);
   /* ------------------------------------------------------------------- */
   /* convert bidiagonal to diagonal using Givens rotations               */
   /* ------------------------------------------------------------------- */

   if (enableReducedForm == 0u) {
      DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs2, pULocal, pU1Local);
   }
   else {
      DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs3, pULocal, pU1Local);
   }

#if !defined(ENABLE_LDRA_COVERAGE)
   int svd_status = DSPLIB_svd_bidiag_to_diag_ci<dataType>(Nrows1, Ncols1, pU1Local, pV1Local, pDiagLocal,
                                                           pSuperDiagLocal, pScratchLocal, colUStride, rowUStride,
                                                           colVStride, rowVStride, enableReducedForm, pBlock);
#else
   DSPLIB_svd_bidiag_to_diag_ci<dataType>(Nrows1, Ncols1, pU1Local, pV1Local, pDiagLocal, pSuperDiagLocal,
                                          pScratchLocal, colUStride, rowUStride, colVStride, rowVStride,
                                          enableReducedForm, pBlock);
#endif

   /* ------------------------------------------------------------------- */
   /* sort singular values in descending order                            */
   /* ------------------------------------------------------------------- */
   DSPLIB_svd_sort_singular_values_ci<dataType>(Nrows1, Ncols1, pU1Local, pULocal, pV1Local, pVLocal, pDiagLocal,
                                                pScratchLocal, colUStride, rowUStride, colVStride, rowVStride,
                                                enableReducedForm, pBlock);

   if (enableReducedForm == 0u) {
      DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs2, pU1Local, pULocal);
   }
   else {
      DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs4, pU1Local, pULocal);
   }
   DSPLIB_matTrans_exec_ci<dataType>(pMatTransKerPrivArgs5, pV1Local, pVLocal);

   /* ------------------------------------------------------------------- */
   /* switch U and V                                                      */
   /* ------------------------------------------------------------------- */
   if (Ncols > Nrows) {
      if (enableReducedForm == 0u) {
         DSPLIB_svd_blk_move_ci<dataType>(pU1Local, pVLocal, Nrows, Nrows, rowUStride, colVStride, pBlock);
         DSPLIB_svd_blk_move_ci<dataType>(pVLocal, pULocal, Ncols, Ncols, colVStride, colUStride, pBlock);
         DSPLIB_svd_blk_move_ci<dataType>(pULocal, pU1Local, Nrows, Nrows, colUStride, rowUStride, pBlock);
      }
      else {
         DSPLIB_svd_blk_move_ci<dataType>(pU1Local, pVLocal, Nrows, Nrows, rowUStride, colVStride, pBlock);
         DSPLIB_svd_blk_move_ci<dataType>(pVLocal, pULocal, Ncols, Nrows, colVStride, colUStride, pBlock);
         DSPLIB_svd_blk_move_ci<dataType>(pULocal, pU1Local, Nrows, Nrows, colUStride, rowUStride, pBlock);
      }
   }

#if !defined(ENABLE_LDRA_COVERAGE)
   if (svd_status < 0) {
      status = DSPLIB_ERR_FAILURE;
   }
#endif
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return status;
}

template DSPLIB_STATUS DSPLIB_svd_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                 void *restrict pA,
                                                 void *restrict pU,
                                                 void *restrict pV,
                                                 void *restrict pDiag,
                                                 void *restrict pSuperDiag,
                                                 void *restrict pU1,
                                                 void *restrict pV1,
                                                 void *restrict pScratch);

template DSPLIB_STATUS DSPLIB_svd_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                  void *restrict pA,
                                                  void *restrict pU,
                                                  void *restrict pV,
                                                  void *restrict pDiag,
                                                  void *restrict pSuperDiag,
                                                  void *restrict pU1,
                                                  void *restrict pV1,
                                                  void *restrict pScratch);
