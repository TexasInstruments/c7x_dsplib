// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_small_diag.h"
#include "DSPLIB_svd_small_priv.h"
#include "DSPLIB_svd_small_singular_sort.h"
#include "DSPLIB_svd_small_u_process.h"
#include "DSPLIB_svd_small_v_process.h"

#define MAX_ITERATION_COUNT 30

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */

template <typename dataType>
DSPLIB_STATUS DSPLIB_svd_small_init_ci(DSPLIB_kernelHandle              handle,
                                       const DSPLIB_bufParams2D_t      *bufParamsIn,
                                       const DSPLIB_bufParams2D_t      *bufParamsU,
                                       const DSPLIB_bufParams2D_t      *bufParamsV,
                                       const DSPLIB_bufParams1D_t      *bufParamsDiag,
                                       const DSPLIB_bufParams1D_t      *bufParamsSuperDiag,
                                       const DSPLIB_svd_small_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

// template DSPLIB_STATUS DSPLIB_svd_small_init_ci<float>(DSPLIB_kernelHandle              handle,
//                                                        const DSPLIB_bufParams2D_t      *bufParamsIn,
//                                                        const DSPLIB_bufParams2D_t      *bufParamsU,
//                                                        const DSPLIB_bufParams2D_t      *bufParamsV,
//                                                        const DSPLIB_bufParams1D_t      *bufParamsDiag,
//                                                        const DSPLIB_bufParams1D_t      *bufParamsSuperDiag,
//                                                        const DSPLIB_svd_small_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_svd_small_init_ci<double>(DSPLIB_kernelHandle              handle,
                                                        const DSPLIB_bufParams2D_t      *bufParamsIn,
                                                        const DSPLIB_bufParams2D_t      *bufParamsU,
                                                        const DSPLIB_bufParams2D_t      *bufParamsV,
                                                        const DSPLIB_bufParams1D_t      *bufParamsDiag,
                                                        const DSPLIB_bufParams1D_t      *bufParamsSuperDiag,
                                                        const DSPLIB_svd_small_InitArgs *pKerInitArgs);

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */
#if (__C7X_VEC_SIZE_BITS__ == 512)

template <typename dataType>
static inline int DSPF_sp_convert_to_bidiag_ci(const int Nrows,
                                               const int Ncols,
                                               dataType *U,
                                               dataType *V,
                                               dataType *diag,
                                               dataType *superdiag,
                                               const int colUStride,
                                               const int colVStride,
                                               uint32_t  enableReducedForm)
{
   // dataType si;
   dataType s, scale;

   /* Householder processing */
   s     = 0;
   scale = 0;

   if (Nrows == 6) {
      u_process_1st_iter(&U[0 + 0 * colUStride], colUStride, diag, superdiag, &scale, &s, 6, 0);
      u_process_2nd_iter(&U[1 + 1 * colUStride], colUStride, diag, superdiag, &scale, &s, 5, 1);
      u_process_3rd_iter(&U[2 + 2 * colUStride], colUStride, diag, superdiag, &scale, &s, 4, 2);
      u_process_4th_iter(&U[3 + 3 * colUStride], colUStride, diag, superdiag, &scale, &s, 3, 3);
      u_process_5th_iter(&U[4 + 4 * colUStride], colUStride, diag, superdiag, &scale, &s, 2, 4);
      u_process_6th_iter(&U[5 + 5 * colUStride], colUStride, diag, superdiag, &scale, &s, 1, 5);
   }
   else {
      u_process_0th_iter(&U[0 + 0 * colUStride], colUStride, diag, superdiag, &scale, &s, 6, 0);
      u_process_1st_iter(&U[1 + 1 * colUStride], colUStride, diag, superdiag, &scale, &s, 5, 1);
      u_process_2nd_iter(&U[2 + 2 * colUStride], colUStride, diag, superdiag, &scale, &s, 4, 2);
      u_process_3rd_iter(&U[3 + 3 * colUStride], colUStride, diag, superdiag, &scale, &s, 3, 3);
      u_process_4th_iter(&U[4 + 4 * colUStride], colUStride, diag, superdiag, &scale, &s, 2, 4);
      u_process_5th_iter(&U[5 + 5 * colUStride], colUStride, diag, superdiag, &scale, &s, 1, 5);
   }

   /* update V */
   int diag_index_V = (Ncols - 1) + (Ncols - 1) * colVStride;
   V[diag_index_V]  = 1;
   s                = superdiag[Ncols - 1];
   int diag_index_U = (Ncols - 2) + (Ncols - 2) * colUStride;
   diag_index_V     = (Ncols - 2) + (Ncols - 2) * colVStride;
   v_process_1st_iter(&U[diag_index_U], &V[diag_index_V], colUStride, colVStride, &s);

   V[diag_index_V] = 1;
   s               = superdiag[Ncols - 2];
   diag_index_U    = (Ncols - 3) + (Ncols - 3) * colUStride;
   diag_index_V    = (Ncols - 3) + (Ncols - 3) * colVStride;
   v_process_2nd_iter(&U[diag_index_U], &V[diag_index_V], colUStride, colVStride, &s);

   V[diag_index_V] = 1;
   s               = superdiag[Ncols - 3];
   diag_index_U    = (Ncols - 4) + (Ncols - 4) * colUStride;
   diag_index_V    = (Ncols - 4) + (Ncols - 4) * colVStride;
   v_process_3rd_iter(&U[diag_index_U], &V[diag_index_V], colUStride, colVStride, &s);

   V[diag_index_V] = 1;
   s               = superdiag[Ncols - 4];
   diag_index_U    = (Ncols - 5) + (Ncols - 5) * colUStride;
   diag_index_V    = (Ncols - 5) + (Ncols - 5) * colVStride;
   v_process_4th_iter(&U[diag_index_U], &V[diag_index_V], colUStride, colVStride, &s);

   V[diag_index_V] = 1;
   s               = superdiag[Ncols - 5];
   diag_index_U    = (Ncols - 6) + (Ncols - 6) * colUStride;
   diag_index_V    = (Ncols - 6) + (Ncols - 6) * colVStride;
   v_process_5th_iter(&U[diag_index_U], &V[diag_index_V], colUStride, colVStride, &s);

   if (Nrows == 6) {
      dataType *U_diag = &U[(Ncols - 1) + (Ncols - 1) * colUStride];
      s                = diag[Ncols - 1];
      u_update_1st_iter(U_diag, colUStride, diag, s);

      U_diag = &U[(Ncols - 2) + (Ncols - 2) * colUStride];
      s      = diag[Ncols - 2];
      u_update_2nd_iter(U_diag, colUStride, diag, s);

      U_diag = &U[(Ncols - 3) + (Ncols - 3) * colUStride];
      s      = diag[Ncols - 3];
      u_update_3rd_iter(U_diag, colUStride, diag, s);

      U_diag = &U[(Ncols - 4) + (Ncols - 4) * colUStride];
      s      = diag[Ncols - 4];
      u_update_4th_iter(U_diag, colUStride, diag, s);

      U_diag = &U[(Ncols - 5) + (Ncols - 5) * colUStride];
      s      = diag[Ncols - 5];
      u_update_5th_iter(U_diag, colUStride, diag, s);

      U_diag = &U[(Ncols - 6) + (Ncols - 6) * colUStride];
      s      = diag[Ncols - 6];
      u_update_6th_iter(U_diag, colUStride, diag, s);
   }
   else {
      if (enableReducedForm == 0) {
         dataType *U_diag = &U[(Ncols - 1) + (Ncols - 1) * colUStride];
         s                = diag[Ncols - 1];
         u_update_6X7_R_1st_iter(U_diag, colUStride, diag, s, 2U);

         U_diag = &U[(Ncols - 2) + (Ncols - 2) * colUStride];
         s      = diag[Ncols - 2];
         u_update_6X7_NR_2nd_iter(U_diag, colUStride, diag, s, 3U);

         U_diag = &U[(Ncols - 3) + (Ncols - 3) * colUStride];
         s      = diag[Ncols - 3];
         u_update_6X7_NR_3rd_iter(U_diag, colUStride, diag, s, 4U);

         U_diag = &U[(Ncols - 4) + (Ncols - 4) * colUStride];
         s      = diag[Ncols - 4];
         u_update_6X7_NR_4th_iter(U_diag, colUStride, diag, s, 5U);

         U_diag = &U[(Ncols - 5) + (Ncols - 5) * colUStride];
         s      = diag[Ncols - 5];
         u_update_6X7_NR_5th_iter(U_diag, colUStride, diag, s, 6U);

         U_diag = &U[(Ncols - 6) + (Ncols - 6) * colUStride];
         s      = diag[Ncols - 6];
         u_update_6X7_NR_6th_iter(U_diag, colUStride, diag, s, 7U);
      }
      else {
         dataType *U_diag = &U[(Ncols - 1) + (Ncols - 1) * colUStride];
         s                = diag[Ncols - 1];
         u_update_6X7_NR_1st_iter(U_diag, colUStride, diag, s, 1U);

         U_diag = &U[(Ncols - 2) + (Ncols - 2) * colUStride];
         s      = diag[Ncols - 2];
         u_update_6X7_NR_2nd_iter(U_diag, colUStride, diag, s, 2U);

         U_diag = &U[(Ncols - 3) + (Ncols - 3) * colUStride];
         s      = diag[Ncols - 3];
         u_update_6X7_NR_3rd_iter(U_diag, colUStride, diag, s, 3U);

         U_diag = &U[(Ncols - 4) + (Ncols - 4) * colUStride];
         s      = diag[Ncols - 4];
         u_update_6X7_NR_4th_iter(U_diag, colUStride, diag, s, 4U);

         U_diag = &U[(Ncols - 5) + (Ncols - 5) * colUStride];
         s      = diag[Ncols - 5];
         u_update_6X7_NR_5th_iter(U_diag, colUStride, diag, s, 5U);

         U_diag = &U[(Ncols - 6) + (Ncols - 6) * colUStride];
         s      = diag[Ncols - 6];
         u_update_6X7_NR_6th_iter(U_diag, colUStride, diag, s, 6U);
      }
   }

   return 0;
}
// template int DSPF_sp_convert_to_bidiag_ci<float>(const int Nrows,
//                                                  const int Ncols,
//                                                  float    *U,
//                                                  float    *V,
//                                                  float    *diag,
//                                                  float    *superdiag,
//                                                  const int colUStride,
//                                                  const int colVStride,
template int DSPF_sp_convert_to_bidiag_ci<double>(const int Nrows,
                                                  const int Ncols,
                                                  double   *U,
                                                  double   *V,
                                                  double   *diag,
                                                  double   *superdiag,
                                                  const int colUStride,
                                                  const int colVStride,
                                                  uint32_t  enableReducedForm);

template <typename dataType>
static inline int DSPF_sp_bidiag_to_diag_ci(const int Nrows,
                                            const int Ncols,
                                            dataType *U,
                                            dataType *V,
                                            dataType *diag,
                                            dataType *superdiag,
                                            const int colUStride,
                                            const int colVStride,
                                            uint32_t  enableReducedForm)
{
   int      m = 0;
   int      i, k, rotation_test, iter, total_iter;
   dataType x, y, z, epsilon;
   dataType c, s, f, g, h;

   iter       = 0;
   total_iter = 0;
   /* ------------------------------------------------------------------- */
   /* find max in col                                                     */
   /* ------------------------------------------------------------------- */
   typedef typename c7x::make_full_vector<double>::type vec;

   epsilon = set_epsilon(diag, superdiag);

   for (k = Ncols - 1; k >= 0; k--) {
      total_iter += iter;
      iter = 0;
      while (true) {
         rotation_test = 1;
         rotation_test = rotation_test_check(diag, superdiag, &m, epsilon, (uint32_t) (k + 1));

         if (rotation_test) {
            c              = 0;
            s              = 1;
            __vpred pred_Z = __mask_long((uint32_t) Nrows);
            for (i = m; i <= k; i++) {
               vec *ptr_Uy = (vec *) &U[(m - 1) * colVStride];
               vec *ptr_Uz = (vec *) &U[i * colVStride];
               //  vec  vec_Uy = *ptr_Uy;
               //  vec  vec_Uz = *ptr_Uz;
               vec vec_Uy = __vload_pred(pred_Z, ptr_Uy);
               vec vec_Uz = __vload_pred(pred_Z, ptr_Uz);

               f            = s * superdiag[i];
               superdiag[i] = c * superdiag[i];
#if !defined(ENABLE_LDRA_COVERAGE)
               /* This part of code checks for "test f convergence" part condition
                  Ref. Singular Value Decomposition and Least Squares Solutions. G. H. Golub et al
                  We use these conditions in order to calculate correct results if and when they occur */
               if (__abs(f) <= epsilon) {
                  break;
               }
#endif

               g             = diag[i];
               double f_sq   = f * f;
               double g_sq   = g * g;
               double f_g_sq = f_sq + g_sq;
               double h_rsq  = getRecipSqrt(f_g_sq);
               h             = f_g_sq * h_rsq; // sqrt(f * f + g * g);
               diag[i]       = h;
               c             = g * h_rsq;    // g / h;
               s             = (-f) * h_rsq; //-f / h;

               vec temp_Uy = vec_Uy * c + vec_Uz * s;
               vec temp_Uz = -vec_Uy * s + vec_Uz * c;
               //  *ptr_Uy     = temp_Uy;
               //  *ptr_Uz     = temp_Uz;
               __vstore_pred(pred_Z, ptr_Uy, temp_Uy);
               __vstore_pred(pred_Z, ptr_Uz, temp_Uz);
            } /* for (i=m;i<=k;i++) */
         }    /* if (rotation_test) */

         z = diag[k];
         if (m == k) {
            if (z < 0) {
               diag[k]         = -z;
               __vpred pred_Z  = __mask_long(6u);
               vec    *p_vec_Z = (vec *) (&V[k * colVStride]);

               vec vec_Z = __vload_pred(pred_Z, p_vec_Z);
               __vstore_pred(pred_Z, p_vec_Z, -vec_Z);
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
            dataType z_sq, x_sq, x_recip, y_sq, g_sq, h_sq, h_y, h_y_2, f_g_recip, f_sq, f_h_sq;

            z_sq = z * z;

            x       = diag[m];
            x_sq    = x * x;
            x_recip = getRecip(x);

            y    = diag[k - 1];
            y_sq = y * y;

            g    = superdiag[k - 1];
            g_sq = g * g;

            h    = superdiag[k];
            h_sq = h * h;

            h_y   = h * y;
            h_y_2 = h_y + h_y;

            f = ((y_sq - z_sq) + (g_sq - h_sq)) * getRecip((h_y_2));

            g = getSqrt(f * f + 1);
            if (f < 0) {
               g = -g;
            }

            f_g_recip = getRecip((f + g));
            f         = ((x_sq - z_sq) + h * y * f_g_recip - h_sq) * x_recip;

            /* next QR transformation */
            c              = 1;
            s              = 1;
            __vpred pred_Z = __mask_long((uint32_t) Nrows);

            for (i = m + 1; i <= k; i++) {
               vec *ptr_Vx = (vec *) &V[(i - 1) * colVStride];
               vec *ptr_Vz = (vec *) &V[i * colVStride];
               //  vec  vec_Vx = *ptr_Vx;
               //  vec  vec_Vz = *ptr_Vz;
               vec vec_Vx = __vload_pred(pred_Z, ptr_Vx);
               vec vec_Vz = __vload_pred(pred_Z, ptr_Vz);

               vec *ptr_Uy = (vec *) &U[(i - 1) * colUStride];
               vec *ptr_Uz = (vec *) &U[i * colUStride];
               //  vec  vec_Uy = *ptr_Uy;
               //  vec  vec_Uz = *ptr_Uz;
               vec vec_Uy = __vload_pred(pred_Z, ptr_Uy);
               vec vec_Uz = __vload_pred(pred_Z, ptr_Uz);

               f_sq             = f * f;
               g                = superdiag[i];
               y                = diag[i];
               h                = s * g;
               h_sq             = h * h;
               f_h_sq           = f_sq + h_sq;
               g                = g * c;
               z                = getRecipSqrt(f_h_sq);
               superdiag[i - 1] = z * f_h_sq;

               c           = f * z;
               s           = h * z;
               vec temp_Vx = vec_Vx * c + vec_Vz * s;
               vec temp_Vz = -vec_Vx * s + vec_Vz * c;
               //  *ptr_Vx     = temp_Vx;
               //  *ptr_Vz     = temp_Vz;
               __vstore_pred(pred_Z, ptr_Vx, temp_Vx);
               __vstore_pred(pred_Z, ptr_Vz, temp_Vz);

               f      = x * c + g * s;
               f_sq   = f * f;
               g      = -x * s + g * c;
               h      = y * s;
               h_sq   = h * h;
               f_h_sq = f_sq + h_sq;
               y      = c * y;

               z           = getRecipSqrt(f_h_sq);
               diag[i - 1] = z * f_h_sq;

               c = f * z;
               s = h * z;
               //  cnt++;
               vec temp_Uy = vec_Uy * c + vec_Uz * s;
               vec temp_Uz = -vec_Uy * s + vec_Uz * c;
               //  *ptr_Uy     = temp_Uy;
               //  *ptr_Uz     = temp_Uz;
               __vstore_pred(pred_Z, ptr_Uy, temp_Uy);
               __vstore_pred(pred_Z, ptr_Uz, temp_Uz);

               f = c * g + s * y;
               x = -s * g + c * y;

            } /* for (i=m+1;i<=k;i++) */
            superdiag[m] = 0;
            superdiag[k] = f;
            diag[k]      = x;
         } /* if (m==k) */
      }    /* while (1==1) */
   }       /* for (k=Ncols-1:k>=0;k--) */

   return total_iter;
}
template int DSPF_sp_bidiag_to_diag_ci<float>(const int Nrows,
                                              const int Ncols,
                                              float    *U,
                                              float    *V,
                                              float    *diag,
                                              float    *superdiag,
                                              const int colUStride,
                                              const int colVStride,
                                              uint32_t  enableReducedForm);
template int DSPF_sp_bidiag_to_diag_ci<double>(const int Nrows,
                                               const int Ncols,
                                               double   *U,
                                               double   *V,
                                               double   *diag,
                                               double   *superdiag,
                                               const int colUStride,
                                               const int colVStride,
                                               uint32_t  enableReducedForm);

template <typename dataType>
static inline int DSPF_sp_sort_singular_values_ci(const int Nrows,
                                                  const int Ncols,
                                                  dataType *U,
                                                  dataType *V,
                                                  dataType *singular_values,
                                                  const int colUStride,
                                                  const int colVStride,
                                                  uint32_t  enableReducedForm)
{

   sort_singular_vals(singular_values, U, V, Nrows, Ncols, colUStride, colVStride);

   return 0;
}
// template int DSPF_sp_sort_singular_values_ci<float>(const int Nrows,
//                                                     const int Ncols,
//                                                     float    *U,
//                                                     float    *V,
//                                                     float    *singular_values,
//                                                     const int colUStride,
//                                                     const int colVStride,
template int DSPF_sp_sort_singular_values_ci<double>(const int Nrows,
                                                     const int Ncols,
                                                     double   *U,
                                                     double   *V,
                                                     double   *singular_values,
                                                     const int colUStride,
                                                     const int colVStride,
                                                     uint32_t  enableReducedForm);

template <typename dataType>
static inline int DSPF_sp_svd_ci(DSPLIB_svd_small_PrivArgs *pKerPrivArgs,
                                 const int                  Nrows,
                                 const int                  Ncols,
                                 dataType                  *A,
                                 dataType                  *U,
                                 dataType                  *V,
                                 dataType                  *U1,
                                 dataType                  *diag,
                                 dataType                  *superdiag,
                                 const int32_t              strideIn,
                                 const int32_t              strideU,
                                 const int32_t              strideV,
                                 uint32_t                   enableReducedForm)
{
   DSPLIB_DEBUGPRINTFN(0, "Entering function pA: %p\n", A);

   int row, col, Nrows1, Ncols1, status;
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
   int32_t colVStride = strideV / dataSize;
   int32_t colAStride = strideIn / dataSize;

   if (Nrows >= Ncols) {
      /* Copy A to U */
      for (row = 0; row < Nrows1; row++) {
         for (col = 0; col < Ncols1; col++) {
            U[col + row * colUStride] = A[col + row * colAStride];
         }
      }
   }
   else {
      /* Copy A' to U */
      for (row = 0; row < Nrows1; row++) {
         for (col = 0; col < Ncols1; col++) {
            U[col + row * colUStride] = A[row + col * colAStride];
         }
      }
   }

   /* ------------------------------------------------------------------- */
   /* convert A to bidiagonal matrix using Householder reflections        */
   /* ------------------------------------------------------------------- */
   DSPF_sp_convert_to_bidiag_ci<dataType>(Nrows1, Ncols1, U, V, diag, superdiag, colUStride, colVStride,
                                          enableReducedForm);

   /* ------------------------------------------------------------------- */
   /* convert bidiagonal to diagonal using Givens rotations               */
   /* ------------------------------------------------------------------- */

   transpose_vec_mat(V, Ncols, Ncols, colVStride);
   transpose_vec_mat(U, Ncols, Ncols, colUStride);
   status = DSPF_sp_bidiag_to_diag_ci<dataType>(Nrows1, Ncols1, U, V, diag, superdiag, colUStride, colVStride,
                                                enableReducedForm);

   transpose_vec_mat(V, Ncols, Ncols, colVStride);
   transpose_vec_mat(U, Ncols, Ncols, colUStride);

   /* ------------------------------------------------------------------- */
   /* sort singular values in descending order                            */
   /* ------------------------------------------------------------------- */
   DSPF_sp_sort_singular_values_ci<dataType>(Nrows1, Ncols1, U, V, diag, colUStride, colVStride, enableReducedForm);

   /* ------------------------------------------------------------------- */
   /* switch U and V                                                      */
   /* ------------------------------------------------------------------- */
   if (Ncols > Nrows) {
      if (enableReducedForm == 0u) {
         memcpy(U1, V, sizeof(dataType) * Nrows * colVStride);
         memcpy(V, U, sizeof(dataType) * Ncols * colUStride);
         memcpy(U, U1, sizeof(dataType) * Nrows * colUStride);
      }
      else {
         memcpy(U1, V, sizeof(dataType) * Ncols * colVStride);
         memcpy(V, U, sizeof(dataType) * Ncols * colUStride);
         memcpy(U, U1, sizeof(dataType) * Nrows * colUStride);
      }
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with status: %d\n", status);

   return status;
}
// template int DSPF_sp_svd_ci<float>(DSPLIB_svd_small_PrivArgs *pKerPrivArgs,
//                                    const int                  Nrows,
//                                    const int                  Ncols,
//                                    float                     *A,
//                                    float                     *U,
//                                    float                     *V,
//                                    float                     *U1,
//                                    float                     *diag,
//                                    float                     *superdiag,
//                                    const int32_t              strideIn,
//                                    const int32_t              strideU,
//                                    const int32_t              strideV,
template int DSPF_sp_svd_ci<double>(DSPLIB_svd_small_PrivArgs *pKerPrivArgs,
                                    const int                  Nrows,
                                    const int                  Ncols,
                                    double                    *A,
                                    double                    *U,
                                    double                    *V,
                                    double                    *U1,
                                    double                    *diag,
                                    double                    *superdiag,
                                    const int32_t              strideIn,
                                    const int32_t              strideU,
                                    const int32_t              strideV,
                                    uint32_t                   enableReducedForm);

#endif /* #if (__C7X_VEC_SIZE_BITS__ == 512) */

template <typename dataType>
DSPLIB_STATUS DSPLIB_svd_small_exec_ci(DSPLIB_kernelHandle handle,
                                       void *restrict pA,
                                       void *restrict pU,
                                       void *restrict pV,
                                       void *restrict pDiag,
                                       void *restrict pSuperDiag,
                                       void *restrict pU1)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if (__C7X_VEC_SIZE_BITS__ == 512)
   DSPLIB_svd_small_PrivArgs *pKerPrivArgs      = (DSPLIB_svd_small_PrivArgs *) handle;
   uint32_t                   heightIn          = pKerPrivArgs->heightIn;
   uint32_t                   widthIn           = pKerPrivArgs->widthIn;
   int32_t                    strideIn          = pKerPrivArgs->strideIn;
   int32_t                    strideU           = pKerPrivArgs->strideU;
   int32_t                    strideV           = pKerPrivArgs->strideV;
   uint32_t                   enableReducedForm = pKerPrivArgs->enableReducedForm;

   /* Typecast void pointers to respective data type */
   dataType *pALocal         = (dataType *) pA;
   dataType *pULocal         = (dataType *) pU;
   dataType *pVLocal         = (dataType *) pV;
   dataType *pDiagLocal      = (dataType *) pDiag;
   dataType *pSuperDiagLocal = (dataType *) pSuperDiag;
   dataType *pU1Local        = (dataType *) pU1;

   DSPLIB_DEBUGPRINTFN(0, "pALocal: %p pOutLocal: %p widthIn: %d heightIn: %d\n", pALocal, pULocal, widthIn, heightIn);

#if !defined(ENABLE_LDRA_COVERAGE)
   int svd_status =
       DSPF_sp_svd_ci<dataType>(pKerPrivArgs, heightIn, widthIn, pALocal, pULocal, pVLocal, pU1Local, pDiagLocal,
                                pSuperDiagLocal, strideIn, strideU, strideV, enableReducedForm);

   if (svd_status < 0) {
      status = DSPLIB_ERR_FAILURE;
   }
#else
   DSPF_sp_svd_ci<dataType>(pKerPrivArgs, heightIn, widthIn, pALocal, pULocal, pVLocal, pU1Local, pDiagLocal,
                            pSuperDiagLocal, strideIn, strideU, strideV, enableReducedForm);

#endif
#else
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "The code is only implemented for __C7X_VEC_SIZE_BITS__ == 512 ");
   status = DSPLIB_ERR_NOT_IMPLEMENTED;

#endif

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

// template DSPLIB_STATUS DSPLIB_svd_small_exec_ci<float>(DSPLIB_kernelHandle handle,
//                                                        void *restrict pA,
//                                                        void *restrict pU,
//                                                        void *restrict pV,
//                                                        void *restrict pDiag,
//                                                        void *restrict pSuperDiag,
//                                                        void *restrict pU1);

template DSPLIB_STATUS DSPLIB_svd_small_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                        void *restrict pA,
                                                        void *restrict pU,
                                                        void *restrict pV,
                                                        void *restrict pDiag,
                                                        void *restrict pSuperDiag,
                                                        void *restrict pU1);
