// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_small_priv.h"

#define MAX_ITERATION_COUNT 30

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */

DSPLIB_STATUS DSPLIB_svd_small_init_cn(DSPLIB_kernelHandle              handle,
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

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */
template <typename dataType>
int DSPF_sp_convert_to_bidiag_cn(const int Nrows,
                                 const int Ncols,
                                 dataType *U,
                                 dataType *V,
                                 dataType *diag,
                                 dataType *superdiag,
                                 const int colUStride,
                                 const int colVStride,
                                 uint32_t  enableReducedForm)
{
   int      i, j, k;
   dataType s, s2, si, scale, half_norm_squared;

   /* Householder processing */
   s     = 0;
   scale = 0;

   for (i = 0; i < Ncols; i++) {
      superdiag[i] = scale * s;
      /* process columns */
      scale = 0;

      for (j = i; j < Nrows; j++) {
         scale += fabs(U[i + j * colUStride]);
      }

      if (scale > 0) {
         s2 = 0;
         for (j = i; j < Nrows; j++) {
            U[i + j * colUStride] = U[i + j * colUStride] / scale;
            s2 += U[i + j * colUStride] * U[i + j * colUStride];
         }
         if (U[i + i * colUStride] < 0) {
            s = sqrt(s2);
         }
         else {
            s = -sqrt(s2);
         }
         half_norm_squared = U[i + i * colUStride] * s - s2;
         U[i + i * colUStride] -= s;

         for (j = i + 1; j < Ncols; j++) {

            si = 0;
            for (k = i; k < Nrows; k++) {
               si += U[i + k * colUStride] * U[j + k * colUStride];
            }
            si = si / half_norm_squared;
            for (k = i; k < Nrows; k++) {
               U[j + k * colUStride] += si * U[i + k * colUStride];
            }
         }
      } /* if (scale>0) */

      for (j = i; j < Nrows; j++) {
         U[i + j * colUStride] *= scale;
      }
      diag[i] = s * scale;
      /* process rows */
      s     = 0;
      scale = 0;

      if ((i != Ncols - 1)) {
         for (j = i + 1; j < Ncols; j++) {
            scale += fabs(U[j + i * colUStride]);
         }

         if (scale > 0) {
            s2 = 0;
            for (j = i + 1; j < Ncols; j++) {
               U[j + i * colUStride] = U[j + i * colUStride] / scale;
               s2 += U[j + i * colUStride] * U[j + i * colUStride];
            }
            j--;
            if (U[j + i * colUStride] < 0) {
               s = sqrt(s2);
            }
            else {
               s = -sqrt(s2);
            }
            half_norm_squared = U[i + 1 + i * colUStride] * s - s2;
            U[i + 1 + i * colUStride] -= s;

            for (k = i + 1; k < Ncols; k++) {
               superdiag[k] = U[k + i * colUStride] / half_norm_squared;
            }
            /* if (i < Nrows - 1) */
            {
               for (j = i + 1; j < Nrows; j++) {
                  si = 0;
                  for (k = i + 1; k < Ncols; k++) {
                     si += U[k + i * colUStride] * U[k + j * colUStride];
                  }
                  for (k = i + 1; k < Ncols; k++) {
                     U[k + j * colUStride] += si * superdiag[k];
                  }
               }
            }
         } /* if (scale>0) */

         for (k = i + 1; k < Ncols; k++) {
            U[k + i * colUStride] *= scale;
         }
      } /* if ((i<Nrows)&&(i!=Ncols-1)) */

   } /* for (i=0;i<Ncols;i++) */

   /* update V */
   V[(Ncols - 1) + (Ncols - 1) * colVStride] = 1;
   s                                         = superdiag[Ncols - 1];
   for (i = Ncols - 2; i >= 0; i--) {
      if (s != 0) {
         for (j = i + 1; j < Ncols; j++) {
            V[i + j * colVStride] = U[j + i * colUStride] / (U[i + 1 + i * colUStride] * s);
         }

         for (j = i + 1; j < Ncols; j++) {
            si = 0;
            for (k = i + 1; k < Ncols; k++) {
               si += U[k + i * colUStride] * V[j + k * colVStride];
            }
            for (k = i + 1; k < Ncols; k++) {
               V[j + k * colVStride] += si * V[i + k * colVStride];
            }
         }
      } /* if (s!=0) */

      for (j = i + 1; j < Ncols; j++) {

         V[j + i * colVStride] = 0;
         V[i + j * colVStride] = 0;
      }
      V[i + i * colVStride] = 1;
      s                     = superdiag[i];
   } /* for (i=Ncols-2;i>=0;i--) */

   if (enableReducedForm == 0u) {
      /* expand U to from Nrows x Ncols to */
      /*                  Nrows x Nrows    */
      if (Nrows > Ncols) {
         for (i = Nrows - 1; i >= 0; i--) {
            for (j = Nrows - 1; j >= 0; j--) {
               if (j <= Ncols - 1) {
                  U[j + i * colUStride] = U[j + i * colUStride];
               }
               else {
                  U[j + i * colUStride] = 0;
               }
            }
         }
      }

      /* update U */
      for (i = Ncols - 1; i >= 0; i--) {
         s = diag[i];
         for (j = i + 1; j < Ncols; j++) {
            U[j + i * colUStride] = 0;
         }

         if (s != 0) {

            for (j = i + 1; j < Nrows; j++) {

               si = 0;
               for (k = i + 1; k < Nrows; k++) {
                  si += U[i + k * colUStride] * U[j + k * colUStride];
               }
               si = si / (U[i + i * colUStride] * s);
               for (k = i; k < Nrows; k++) {
                  U[j + k * colUStride] += si * U[i + k * colUStride];
               }
            }

            /* initial U */
            if (i == Ncols - 1) {
               for (j = i; j < Nrows; j++) {
                  for (k = Nrows - 1; k >= i + 1; k--) {
                     U[k + j * colUStride] =
                         U[i + j * colUStride] * U[i + k * colUStride] / (U[i + i * colUStride] * s);
                     if (j == k) {
                        U[k + j * colUStride] += 1;
                     }
                  }
               }
            }
            for (j = i; j < Nrows; j++) {
               U[i + j * colUStride] = U[i + j * colUStride] / s;
            }
         } /* if (s!=0) */
         else {
            if (i == Ncols - 1) {
               for (k = 1; k <= Nrows - Ncols; k++) {
                  U[i + k + (i + k) * colUStride] = 1;
               }
            }
            for (j = i; j < Nrows; j++) {
               U[i + j * colUStride] = 0;
            }
         } /* if (s!=0) */
         U[i + i * colUStride] += 1;
      } /* for (i=Ncols-1;i>=0;i--) */
   }

   else {
      /* update U */
      for (i = Ncols - 1; i >= 0; i--) {
         s = diag[i];
         for (j = i + 1; j < Ncols; j++) {
            U[j + i * colUStride] = 0;
         }
         if (s != 0) {
            for (j = i + 1; j < Ncols; j++) {
               si = 0;
               for (k = i + 1; k < Nrows; k++) {
                  si += U[i + k * colUStride] * U[j + k * colUStride];
               }
               si = si / (U[i + i * colUStride] * s);
               for (k = i; k < Nrows; k++) {
                  U[j + k * colUStride] += si * U[i + k * colUStride];
               }
            }
            for (j = i; j < Nrows; j++) {
               U[i + j * colUStride] = U[i + j * colUStride] / s;
            }
         }
         else { /* if (s!=0) */
            for (j = i; j < Nrows; j++) {
               U[i + j * colUStride] = 0;
            }
         } /* if (s!=0) */
         U[i + i * colUStride] += 1;
      } /* for (i=Ncols-1;i>=0;i--) */
   }

   return 0;
}
// template int DSPF_sp_convert_to_bidiag_cn<float>(const int Nrows,
//                                                  const int Ncols,
//                                                  float    *U,
//                                                  float    *V,
//                                                  float    *diag,
//                                                  float    *superdiag,
//                                                  const int colUStride,
//                                                  const int colVStride,
template int DSPF_sp_convert_to_bidiag_cn<double>(const int Nrows,
                                                  const int Ncols,
                                                  double   *U,
                                                  double   *V,
                                                  double   *diag,
                                                  double   *superdiag,
                                                  const int colUStride,
                                                  const int colVStride,
                                                  uint32_t  enableReducedForm);

template <typename dataType>
int DSPF_sp_bidiag_to_diag_cn(const int Nrows,
                              const int Ncols,
                              dataType *U,
                              dataType *V,
                              dataType *diag,
                              dataType *superdiag,
                              const int colUStride,
                              const int colVStride,
                              uint32_t  enableReducedForm)
{

   int      row, i, k, m, rotation_test, iter, total_iter;
   dataType x, y, z, epsilon;
   dataType c, s, f, g, h;

   iter       = 0;
   total_iter = 0;

   /* ------------------------------------------------------------------- */
   /* find max in col                                                     */
   /* ------------------------------------------------------------------- */
   x = 0;
   for (i = 0; i < Ncols; i++) {
      y = fabs(diag[i]) + fabs(superdiag[i]);
      if (x < y) {
         x = y;
      }
   }
   // else {
   epsilon = DBL_EPSILON * x;

   for (k = Ncols - 1; k >= 0; k--) {
      total_iter += iter;
      iter = 0;
      while (true) {
         rotation_test = 1;

         /* Coverage issue fixes: superdiag[0] is always 0. Hence loop modified to m > 0.  */
         //        break;
         //        break;

         for (m = k; m > 0; m--) {
            if (fabs(superdiag[m]) <= epsilon) {
               rotation_test = 0;
               break; /* goto "test f convergence" part */
            }
            if (fabs(diag[m - 1]) <= epsilon) {
               break; /* goto "cancellation" part */
            }
         } /* for (m=k;m>=0;m--) */

         if (m == 0) {
            rotation_test = 0;
         }

         if (rotation_test) { /* "cancellation" part */
            c = 0;
            s = 1;
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
               g       = diag[i];
               h       = sqrt(f * f + g * g);
               diag[i] = h;
               c       = g / h;
               s       = -f / h;

               if (enableReducedForm == 0u) {
                  for (row = 0; row < Nrows; row++) {
                     y                           = U[m - 1 + row * colUStride];
                     z                           = U[i + row * colUStride];
                     U[m - 1 + row * colUStride] = y * c + z * s;
                     U[i + row * colUStride]     = -y * s + z * c;
                  }
               }
               else {
                  for (row = 0; row < Nrows; row++) {
                     y                           = U[m - 1 + row * colUStride];
                     z                           = U[i + row * colUStride];
                     U[m - 1 + row * colUStride] = y * c + z * s;
                     U[i + row * colUStride]     = -y * s + z * c;
                  }
               }

            } /* for (i=m;i<=k;i++) */
         }    /* if (rotation_test) */

         z = diag[k];
         if (m == k) { /* "test f convergence" part */
            if (z < 0) {
               diag[k] = -z;
               for (row = 0; row < Ncols; row++) {
                  V[k + row * colVStride] = -V[k + row * colVStride];
               }
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
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2 * h * y);
            g = sqrt(f * f + 1);
            if (f < 0) {
               g = -g;
            }
            f = ((x - z) * (x + z) + h * (y / (f + g) - h)) / x;
            /* next QR transformation */
            c = 1;
            s = 1;
            for (i = m + 1; i <= k; i++) {
               g                = superdiag[i];
               y                = diag[i];
               h                = s * g;
               g                = g * c;
               z                = sqrt(f * f + h * h);
               superdiag[i - 1] = z;
               c                = f / z;
               s                = h / z;
               f                = x * c + g * s;
               g                = -x * s + g * c;
               h                = y * s;
               y                = c * y;

               for (row = 0; row < Ncols; row++) {
                  x                           = V[i - 1 + row * colVStride];
                  z                           = V[i + row * colVStride];
                  V[i - 1 + row * colVStride] = x * c + z * s;
                  V[i + row * colVStride]     = -x * s + z * c;
               }
               z           = sqrt(f * f + h * h);
               diag[i - 1] = z;
#if !defined(ENABLE_LDRA_COVERAGE)
               /* This part of code test value of z in order to avoid the "inf" result */
               if (z != 0) {
                  c = f / z;
                  s = h / z;
               }
#else
               c = f / z;
               s = h / z;
#endif
               f = c * g + s * y;
               x = -s * g + c * y;

               if (enableReducedForm == 0u) {
                  for (row = 0; row < Nrows; row++) {
                     y                           = U[i - 1 + row * colUStride];
                     z                           = U[i + row * colUStride];
                     U[i - 1 + row * colUStride] = c * y + s * z;
                     U[i + row * colUStride]     = -s * y + c * z;
                  }
               }
               else {
                  for (row = 0; row < Nrows; row++) {
                     y                           = U[i - 1 + row * colUStride];
                     z                           = U[i + row * colUStride];
                     U[i - 1 + row * colUStride] = c * y + s * z;
                     U[i + row * colUStride]     = -s * y + c * z;
                  }
               }
            } /* for (i=m+1;i<=k;i++) */
            superdiag[m] = 0;
            superdiag[k] = f;
            diag[k]      = x;
         } /* if (m==k) */
      }    /* while (1==1) */
   }       /* for (k=Ncols-1:k>=0;k--) */

   return total_iter;
}
template int DSPF_sp_bidiag_to_diag_cn<float>(const int Nrows,
                                              const int Ncols,
                                              float    *U,
                                              float    *V,
                                              float    *diag,
                                              float    *superdiag,
                                              const int colUStride,
                                              const int colVStride,
                                              uint32_t  enableReducedForm);
template int DSPF_sp_bidiag_to_diag_cn<double>(const int Nrows,
                                               const int Ncols,
                                               double   *U,
                                               double   *V,
                                               double   *diag,
                                               double   *superdiag,
                                               const int colUStride,
                                               const int colVStride,
                                               uint32_t  enableReducedForm);

template <typename dataType>
int DSPF_sp_sort_singular_values_cn(const int Nrows,
                                    const int Ncols,
                                    dataType *U,
                                    dataType *V,
                                    dataType *singular_values,
                                    const int colUStride,
                                    const int colVStride,
                                    uint32_t  enableReducedForm)
{

   int      i, j, row, max_index;
   dataType temp;
   for (i = 0; i < Ncols - 1; i++) {
      max_index = i;
      for (j = i + 1; j < Ncols; j++) {
         if (singular_values[j] > singular_values[max_index]) {
            max_index = j;
         }
      }
      if (max_index != i) {
         temp                       = singular_values[i];
         singular_values[i]         = singular_values[max_index];
         singular_values[max_index] = temp;
         if (enableReducedForm == 0u) {
            for (row = 0; row < Nrows; row++) {
               temp                            = U[max_index + row * colUStride];
               U[max_index + row * colUStride] = U[i + row * colUStride];
               U[i + row * colUStride]         = temp;
            }
         }
         else {
            for (row = 0; row < Nrows; row++) {
               temp                            = U[max_index + row * colUStride];
               U[max_index + row * colUStride] = U[i + row * colUStride];
               U[i + row * colUStride]         = temp;
            }
         }

         for (row = 0; row < Ncols; row++) {
            temp                            = V[max_index + row * colVStride];
            V[max_index + row * colVStride] = V[i + row * colVStride];
            V[i + row * colVStride]         = temp;
         }
      }
   }
   return 0;
}
// template int DSPF_sp_sort_singular_values_cn<float>(const int Nrows,
//                                                     const int Ncols,
//                                                     float    *U,
//                                                     float    *V,
//                                                     float    *singular_values,
//                                                     const int colUStride,
//                                                     const int colVStride,
template int DSPF_sp_sort_singular_values_cn<double>(const int Nrows,
                                                     const int Ncols,
                                                     double   *U,
                                                     double   *V,
                                                     double   *singular_values,
                                                     const int colUStride,
                                                     const int colVStride,
                                                     uint32_t  enableReducedForm);

template <typename dataType>
int DSPF_sp_svd_cn(DSPLIB_svd_small_PrivArgs *pKerPrivArgs,
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
   DSPF_sp_convert_to_bidiag_cn<dataType>(Nrows1, Ncols1, U, V, diag, superdiag, colUStride, colVStride,
                                          enableReducedForm);

   /* ------------------------------------------------------------------- */
   /* convert bidiagonal to diagonal using Givens rotations               */
   /* ------------------------------------------------------------------- */
   status = DSPF_sp_bidiag_to_diag_cn<dataType>(Nrows1, Ncols1, U, V, diag, superdiag, colUStride, colVStride,
                                                enableReducedForm);

   /* ------------------------------------------------------------------- */
   /* sort singular values in descending order                            */
   /* ------------------------------------------------------------------- */
   DSPF_sp_sort_singular_values_cn<dataType>(Nrows1, Ncols1, U, V, diag, colUStride, colVStride, enableReducedForm);

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

// template int DSPF_sp_svd_cn<float>(DSPLIB_svd_small_PrivArgs *pKerPrivArgs,
//                                    const int            Nrows,
//                                    const int            Ncols,
//                                    float               *A,
//                                    float               *U,
//                                    float               *V,
//                                    float               *U1,
//                                    float               *V1,
//                                    float               *diag,
//                                    float               *superdiag,
//                                    const int32_t        strideIn,
//                                    const int32_t        strideU,
//                                    const int32_t        strideV,

template int DSPF_sp_svd_cn<double>(DSPLIB_svd_small_PrivArgs *pKerPrivArgs,
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

template <typename dataType>
DSPLIB_STATUS DSPLIB_svd_small_exec_cn(DSPLIB_kernelHandle handle,
                                       void *restrict pA,
                                       void *restrict pU,
                                       void *restrict pV,
                                       void *restrict pDiag,
                                       void *restrict pSuperDiag,
                                       void *restrict pU1)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

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
       DSPF_sp_svd_cn<dataType>(pKerPrivArgs, heightIn, widthIn, pALocal, pULocal, pVLocal, pU1Local, pDiagLocal,
                                pSuperDiagLocal, strideIn, strideU, strideV, enableReducedForm);
   if (svd_status < 0) {
      status = DSPLIB_ERR_FAILURE;
   }
#else
   DSPF_sp_svd_cn<dataType>(pKerPrivArgs, heightIn, widthIn, pALocal, pULocal, pVLocal, pU1Local, pDiagLocal,
                            pSuperDiagLocal, strideIn, strideU, strideV, enableReducedForm);
#endif
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

// template DSPLIB_STATUS DSPLIB_svd_small_exec_cn<float>(DSPLIB_kernelHandle handle,
//                                                  void *restrict pA,
//                                                  void *restrict pU,
//                                                  void *restrict pV,
//                                                  void *restrict pDiag,
//                                                  void *restrict pSuperDiag,
//                                                  void *restrict pU1);

template DSPLIB_STATUS DSPLIB_svd_small_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                        void *restrict pA,
                                                        void *restrict pU,
                                                        void *restrict pV,
                                                        void *restrict pDiag,
                                                        void *restrict pSuperDiag,
                                                        void *restrict pU1);
