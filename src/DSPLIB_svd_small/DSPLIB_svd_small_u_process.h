// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_svd_small_priv.h"

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */

#if (__C7X_VEC_SIZE_BITS__ == 512)

/* Following horizontal addition are used for DSPLIB_svd_small */
template <typename V, typename W> inline void c7x_horizontal_add_6_elems(V inVec, W *horizontalSum);
template <> inline void                       c7x_horizontal_add_6_elems(c7x::double_vec inVec, double *horizontalSum)
{
   double s0      = inVec.s[0];
   double s1      = inVec.s[1];
   double s2      = inVec.s[2];
   double s3      = inVec.s[3];
   double s4      = inVec.s[4];
   double s5      = inVec.s[5];
   double ss0     = s0 + s1;
   double ss1     = s2 + s3;
   double ss2     = s4 + s5;
   double sss0    = ss0 + ss1 + ss2;
   *horizontalSum = sss0;
}

template <typename V, typename W> inline void c7x_horizontal_add_5_elems(V inVec, W *horizontalSum);
template <> inline void                       c7x_horizontal_add_5_elems(c7x::double_vec inVec, double *horizontalSum)
{
   double s0      = inVec.s[0];
   double s1      = inVec.s[1];
   double s2      = inVec.s[2];
   double s3      = inVec.s[3];
   double s4      = inVec.s[4];
   double ss0     = s0 + s1;
   double ss1     = s2 + s3 + s4;
   double sss0    = ss0 + ss1;
   *horizontalSum = sss0;
}

template <typename V, typename W> inline void c7x_horizontal_add_4_elems(V inVec, W *horizontalSum);
template <> inline void                       c7x_horizontal_add_4_elems(c7x::double_vec inVec, double *horizontalSum)
{
   double s0      = inVec.s[0];
   double s1      = inVec.s[1];
   double s2      = inVec.s[2];
   double s3      = inVec.s[3];
   double ss0     = s0 + s1;
   double ss1     = s2 + s3;
   double sss0    = ss0 + ss1;
   *horizontalSum = sss0;
}

template <typename V, typename W> inline void c7x_horizontal_add_3_elems(V inVec, W *horizontalSum);
template <> inline void                       c7x_horizontal_add_3_elems(c7x::double_vec inVec, double *horizontalSum)
{
   double s0      = inVec.s[0];
   double s1      = inVec.s[1];
   double s2      = inVec.s[2];
   double ss0     = s0 + s1;
   double ss1     = s2;
   double sss0    = ss0 + ss1;
   *horizontalSum = sss0;
}

template <typename V, typename W> inline void c7x_horizontal_add_2_elems(V inVec, W *horizontalSum);
template <> inline void                       c7x_horizontal_add_2_elems(c7x::double_vec inVec, double *horizontalSum)
{
   double s0      = inVec.s[0];
   double s1      = inVec.s[1];
   double ss0     = s0 + s1;
   *horizontalSum = ss0;
}

template <typename X, typename Y> inline Y vec_put(uint8_t idx, X val, Y vecIn);

template <> inline c7x::float_vec vec_put<float, c7x::float_vec>(uint8_t idx, float val, c7x::float_vec vecIn)
{
   return __vputw_rkv(val, idx, vecIn);
}

template <> inline c7x::double_vec vec_put<double, c7x::double_vec>(uint8_t idx, double val, c7x::double_vec vecIn)
{
   return __vputd_dkv(val, idx, vecIn);
}

#if (__C7X_VEC_SIZE_BITS__ == 512)
template <typename X> inline X    gen_u_row_process_mask();
template <> inline c7x::float_vec gen_u_row_process_mask<c7x::float_vec>()
{
   c7x::float_vec ret_mask = c7x::float_vec(0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
   return ret_mask;
}
template <> inline c7x::double_vec gen_u_row_process_mask<c7x::double_vec>()
{
   c7x::double_vec ret_mask = c7x::double_vec(0, 1, 1, 1, 1, 1, 1, 1);
   return ret_mask;
}

template <typename X> inline X    gen_u_row_process_rev_mask();
template <> inline c7x::float_vec gen_u_row_process_rev_mask<c7x::float_vec>()
{
   c7x::float_vec ret_mask = c7x::float_vec(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   return ret_mask;
}
template <> inline c7x::double_vec gen_u_row_process_rev_mask<c7x::double_vec>()
{
   c7x::double_vec ret_mask = c7x::double_vec(1, 0, 0, 0, 0, 0, 0, 0);
   return ret_mask;
}

#endif

#define U_PROC_SKIP_VLOAD 0

#if U_PROC_SKIP_VLOAD
c7x::double_vec vec_in0_u_proc = (c7x::double_vec) 0.0;
c7x::double_vec vec_in1_u_proc = (c7x::double_vec) 0.0;
c7x::double_vec vec_in2_u_proc = (c7x::double_vec) 0.0;
c7x::double_vec vec_in3_u_proc = (c7x::double_vec) 0.0;
c7x::double_vec vec_in4_u_proc = (c7x::double_vec) 0.0;
c7x::double_vec vec_in5_u_proc = (c7x::double_vec) 0.0;
#endif

template <typename dataType>
static inline void u_process_0th_iter(dataType      *U,
                                      const int      colUStride,
                                      dataType      *diag,
                                      dataType      *superdiag,
                                      dataType      *scale,
                                      dataType      *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx);
template <>
inline void u_process_0th_iter<float>(float         *U,
                                      const int      colUStride,
                                      float         *diag,
                                      float         *superdiag,
                                      float         *scale,
                                      float         *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx)
{
   return;
}
template <>
inline void u_process_0th_iter<double>(double        *U,
                                       const int      colUStride,
                                       double        *diag,
                                       double        *superdiag,
                                       double        *scale,
                                       double        *s,
                                       const uint32_t num_cols,
                                       const uint32_t diag_idx)
{
   double s2 = 0, half_norm_squared = 0;
   /* -------------------------------------------------- */
   /* For i = 0 */
   /* -------------------------------------------------- */
   {
      /* section 1- 54 */
      typedef typename c7x::make_full_vector<double>::type vec;

      __vpred pred_col_ele = __mask_long(num_cols);

#if U_PROC_SKIP_VLOAD
      vec vec_in0 = vec_in0_u_proc;
      vec vec_in1 = vec_in1_u_proc;
      vec vec_in2 = vec_in2_u_proc;
      vec vec_in3 = vec_in3_u_proc;
      vec vec_in4 = vec_in4_u_proc;
      vec vec_in5 = vec_in5_u_proc;
#else
      vec vec_in0 = __vload_pred(pred_col_ele, (vec *) (&U[0 + 0 * colUStride]));
      vec vec_in1 = __vload_pred(pred_col_ele, (vec *) (&U[0 + 1 * colUStride]));
      vec vec_in2 = __vload_pred(pred_col_ele, (vec *) (&U[0 + 2 * colUStride]));
      vec vec_in3 = __vload_pred(pred_col_ele, (vec *) (&U[0 + 3 * colUStride]));
      vec vec_in4 = __vload_pred(pred_col_ele, (vec *) (&U[0 + 4 * colUStride]));
      vec vec_in5 = __vload_pred(pred_col_ele, (vec *) (&U[0 + 5 * colUStride]));
      vec vec_in6 = __vload_pred(pred_col_ele, (vec *) (&U[0 + 6 * colUStride]));

#endif

      superdiag[diag_idx] = (*scale) * (*s);
      *scale              = 0;

      /* -------------------------------------------------- */
      /* U COLUMN PROCESS FOR i = 0 */
      /* -------------------------------------------------- */

      /* Accumulate i = 0 th column absolute values to get 'scale' */
      double in0_s0 = vec_in0.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in0), 0u));
      double in1_s0 = vec_in1.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in1), 0u));
      double in2_s0 = vec_in2.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in2), 0u));
      double in3_s0 = vec_in3.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in3), 0u));
      double in4_s0 = vec_in4.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in4), 0u));
      double in5_s0 = vec_in5.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in5), 0u));
      double in6_s0 = vec_in6.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in5), 0u));

      /* Store the first columns values in another vector */
      vec vec_first_col = (vec) 0.0;
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in0_s0), 0, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in1_s0), 1, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in2_s0), 2, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in3_s0), 3, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in4_s0), 4, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in5_s0), 5, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in6_s0), 6, c7x::as_long_vec(vec_first_col)));

      (*scale) += __abs(in0_s0);
      (*scale) += __abs(in1_s0);
      (*scale) += __abs(in2_s0);
      (*scale) += __abs(in3_s0);
      (*scale) += __abs(in4_s0);
      (*scale) += __abs(in5_s0);
      (*scale) += __abs(in6_s0);

      /* section 2- 132 */

      if ((*scale) > 0) {
         s2 = 0;
         /* normalize the 'vec_first_col' */
         vec_first_col        = vec_first_col * (vec) getRecip((*scale)); // / scale;
         vec vec_first_col_sq = vec_first_col * vec_first_col;
         c7x_horizontal_add(vec_first_col_sq, &s2);

         *s = getSqrt(s2);

         if (vec_first_col.s[0] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_col.s[0] * (*s) - s2;
         vec inv_hnsq      = (vec) getRecip(half_norm_squared);
         vec_first_col.s[0] -= (*s);

         vec vec_si = (vec) 0;

         vec_si += (vec_first_col.s[0] * vec_in0);
         vec_si += (vec_first_col.s[1] * vec_in1);
         vec_si += (vec_first_col.s[2] * vec_in2);
         vec_si += (vec_first_col.s[3] * vec_in3);
         vec_si += (vec_first_col.s[4] * vec_in4);
         vec_si += (vec_first_col.s[5] * vec_in5);
         vec_si += (vec_first_col.s[6] * vec_in6);

         vec_si = vec_si * inv_hnsq; //(vec) getRecip(half_norm_squared); // / half_norm_squared;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];
         vec_in4 += vec_si * vec_first_col.s[4];
         vec_in5 += vec_si * vec_first_col.s[5];
         vec_in6 += vec_si * vec_first_col.s[6];
      }
      /* section 3- 69 */
      vec_first_col *= (*scale);
      vec_in0.s[0] = vec_first_col.s[0];
      vec_in1.s[0] = vec_first_col.s[1];
      vec_in2.s[0] = vec_first_col.s[2];
      vec_in3.s[0] = vec_first_col.s[3];
      vec_in4.s[0] = vec_first_col.s[4];
      vec_in5.s[0] = vec_first_col.s[5];
      vec_in6.s[0] = vec_first_col.s[6];

      diag[diag_idx] = (*s) * (*scale);

      *s     = 0;
      *scale = 0;

      vec vec_row_mask = gen_u_row_process_mask<vec>();

      vec vec_first_row = vec_row_mask * vec_in0;
      c7x_horizontal_add_6_elems(__abs(vec_first_row), scale);

      /* section 4- 142 */

      if ((*scale) > 0) {
         s2                   = 0;
         vec_first_row        = vec_first_row * (vec) getRecip((*scale)); // / scale;
         vec vec_first_row_sq = vec_first_row * vec_first_row;
         c7x_horizontal_add_6_elems(vec_first_row_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_row.s[(num_cols - 1)] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_row.s[1] * (*s) - s2;
         vec_first_row.s[1] -= (*s);

         vec vec_superdiag_temp = vec_first_row * (vec) getRecip(half_norm_squared); //  / half_norm_squared;

         vec vec_si = (vec) 0;

         vec vec_temp1 = vec_first_row * vec_in1;
         vec vec_temp2 = vec_first_row * vec_in2;
         vec vec_temp3 = vec_first_row * vec_in3;
         vec vec_temp4 = vec_first_row * vec_in4;
         vec vec_temp5 = vec_first_row * vec_in5;
         vec vec_temp6 = vec_first_row * vec_in6;

         c7x_horizontal_add_6_elems(vec_temp1, &vec_si.s[1]);
         c7x_horizontal_add_6_elems(vec_temp2, &vec_si.s[2]);
         c7x_horizontal_add_6_elems(vec_temp3, &vec_si.s[3]);
         c7x_horizontal_add_6_elems(vec_temp4, &vec_si.s[4]);
         c7x_horizontal_add_6_elems(vec_temp5, &vec_si.s[5]);
         c7x_horizontal_add_6_elems(vec_temp6, &vec_si.s[6]);

         vec_in1 += vec_si.s[1] * vec_superdiag_temp;
         vec_in2 += vec_si.s[2] * vec_superdiag_temp;
         vec_in3 += vec_si.s[3] * vec_superdiag_temp;
         vec_in4 += vec_si.s[4] * vec_superdiag_temp;
         vec_in5 += vec_si.s[5] * vec_superdiag_temp;
         vec_in6 += vec_si.s[6] * vec_superdiag_temp;

         vec vec_row_rev_mask = gen_u_row_process_rev_mask<vec>();
         vec_in0 *= vec_row_rev_mask;
         vec_in0 += vec_first_row * (*scale);
      }

#if U_PROC_SKIP_VLOAD
      __vstore_pred(pred_col_ele, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      U[0 + 1 * colUStride] = vec_in1.s[0];
      U[0 + 2 * colUStride] = vec_in2.s[0];
      U[0 + 3 * colUStride] = vec_in3.s[0];
      U[0 + 4 * colUStride] = vec_in4.s[0];
      U[0 + 5 * colUStride] = vec_in5.s[0];
      vec_in0_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in1), 64u));
      vec_in1_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in2), 64u));
      vec_in2_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in3), 64u));
      vec_in3_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in4), 64u));
      vec_in4_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in5), 64u));

#else
      /* section 5- 8 */
      __vstore_pred(pred_col_ele, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      __vstore_pred(pred_col_ele, (vec *) (&U[0 + 1 * colUStride]), vec_in1);
      __vstore_pred(pred_col_ele, (vec *) (&U[0 + 2 * colUStride]), vec_in2);
      __vstore_pred(pred_col_ele, (vec *) (&U[0 + 3 * colUStride]), vec_in3);
      __vstore_pred(pred_col_ele, (vec *) (&U[0 + 4 * colUStride]), vec_in4);
      __vstore_pred(pred_col_ele, (vec *) (&U[0 + 5 * colUStride]), vec_in5);
      __vstore_pred(pred_col_ele, (vec *) (&U[0 + 6 * colUStride]), vec_in6);
#endif
   }

   return;
}

template <typename dataType>
static inline void u_process_1st_iter(dataType      *U,
                                      const int      colUStride,
                                      dataType      *diag,
                                      dataType      *superdiag,
                                      dataType      *scale,
                                      dataType      *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx);
template <>
inline void u_process_1st_iter<float>(float         *U,
                                      const int      colUStride,
                                      float         *diag,
                                      float         *superdiag,
                                      float         *scale,
                                      float         *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx)
{
   return;
}
template <>
inline void u_process_1st_iter<double>(double        *U,
                                       const int      colUStride,
                                       double        *diag,
                                       double        *superdiag,
                                       double        *scale,
                                       double        *s,
                                       const uint32_t num_cols,
                                       const uint32_t diag_idx)
{
   double s2 = 0, half_norm_squared = 0;
   /* -------------------------------------------------- */
   /* For i = 0 */
   /* -------------------------------------------------- */
   {
      /* section 1- 54 */
      typedef typename c7x::make_full_vector<double>::type vec;

      __vpred pred_6_elem = __mask_long(num_cols);

#if U_PROC_SKIP_VLOAD
      vec vec_in0 = vec_in0_u_proc;
      vec vec_in1 = vec_in1_u_proc;
      vec vec_in2 = vec_in2_u_proc;
      vec vec_in3 = vec_in3_u_proc;
      vec vec_in4 = vec_in4_u_proc;
      vec vec_in5 = vec_in5_u_proc;
#else
      vec vec_in0 = __vload_pred(pred_6_elem, (vec *) (&U[0 + 0 * colUStride]));
      vec vec_in1 = __vload_pred(pred_6_elem, (vec *) (&U[0 + 1 * colUStride]));
      vec vec_in2 = __vload_pred(pred_6_elem, (vec *) (&U[0 + 2 * colUStride]));
      vec vec_in3 = __vload_pred(pred_6_elem, (vec *) (&U[0 + 3 * colUStride]));
      vec vec_in4 = __vload_pred(pred_6_elem, (vec *) (&U[0 + 4 * colUStride]));
      vec vec_in5 = __vload_pred(pred_6_elem, (vec *) (&U[0 + 5 * colUStride]));
#endif

      superdiag[diag_idx] = (*scale) * (*s);
      *scale              = 0;

      /* -------------------------------------------------- */
      /* U COLUMN PROCESS FOR i = 0 */
      /* -------------------------------------------------- */

      /* Accumulate i = 0 th column absolute values to get 'scale' */
      double in0_s0 = vec_in0.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in0), 0u));
      double in1_s0 = vec_in1.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in1), 0u));
      double in2_s0 = vec_in2.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in2), 0u));
      double in3_s0 = vec_in3.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in3), 0u));
      double in4_s0 = vec_in4.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in4), 0u));
      double in5_s0 = vec_in5.s[0]; // __as_double(__vgetd_vrd(c7x::as_long_vec(vec_in5), 0u));

      /* Store the first columns values in another vector */
      vec vec_first_col = (vec) 0.0;
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in0_s0), 0, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in1_s0), 1, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in2_s0), 2, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in3_s0), 3, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in4_s0), 4, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in5_s0), 5, c7x::as_long_vec(vec_first_col)));

      (*scale) += __abs(in0_s0);
      (*scale) += __abs(in1_s0);
      (*scale) += __abs(in2_s0);
      (*scale) += __abs(in3_s0);
      (*scale) += __abs(in4_s0);
      (*scale) += __abs(in5_s0);

      /* section 2- 132 */

      if ((*scale) > 0) {
         s2 = 0;
         /* normalize the 'vec_first_col' */
         vec_first_col        = vec_first_col * (vec) getRecip((*scale)); // / scale;
         vec vec_first_col_sq = vec_first_col * vec_first_col;
         c7x_horizontal_add_6_elems(vec_first_col_sq, &s2);

         *s = getSqrt(s2);

         if (vec_first_col.s[0] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_col.s[0] * (*s) - s2;
         vec inv_hnsq      = (vec) getRecip(half_norm_squared);
         vec_first_col.s[0] -= (*s);

         vec vec_si = (vec) 0;

         vec_si += (vec_first_col.s[0] * vec_in0);
         vec_si += (vec_first_col.s[1] * vec_in1);
         vec_si += (vec_first_col.s[2] * vec_in2);
         vec_si += (vec_first_col.s[3] * vec_in3);
         vec_si += (vec_first_col.s[4] * vec_in4);
         vec_si += (vec_first_col.s[5] * vec_in5);

         vec_si = vec_si * inv_hnsq; //(vec) getRecip(half_norm_squared); // / half_norm_squared;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];
         vec_in4 += vec_si * vec_first_col.s[4];
         vec_in5 += vec_si * vec_first_col.s[5];
      }
      /* section 3- 69 */
      vec_first_col *= (*scale);
      vec_in0.s[0] = vec_first_col.s[0];
      vec_in1.s[0] = vec_first_col.s[1];
      vec_in2.s[0] = vec_first_col.s[2];
      vec_in3.s[0] = vec_first_col.s[3];
      vec_in4.s[0] = vec_first_col.s[4];
      vec_in5.s[0] = vec_first_col.s[5];

      diag[diag_idx] = (*s) * (*scale);

      *s     = 0;
      *scale = 0;

      vec vec_row_mask = gen_u_row_process_mask<vec>();

      vec vec_first_row = vec_row_mask * vec_in0;
      c7x_horizontal_add_6_elems(__abs(vec_first_row), scale);

      /* section 4- 142 */

      if ((*scale) > 0) {
         s2                   = 0;
         vec_first_row        = vec_first_row * (vec) getRecip((*scale)); // / scale;
         vec vec_first_row_sq = vec_first_row * vec_first_row;
         c7x_horizontal_add_6_elems(vec_first_row_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_row.s[(num_cols - 1)] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_row.s[1] * (*s) - s2;
         vec_first_row.s[1] -= (*s);

         vec vec_superdiag_temp = vec_first_row * (vec) getRecip(half_norm_squared); //  / half_norm_squared;

         vec vec_si = (vec) 0;

         vec vec_temp1 = vec_first_row * vec_in1;
         vec vec_temp2 = vec_first_row * vec_in2;
         vec vec_temp3 = vec_first_row * vec_in3;
         vec vec_temp4 = vec_first_row * vec_in4;
         vec vec_temp5 = vec_first_row * vec_in5;

         c7x_horizontal_add_6_elems(vec_temp1, &vec_si.s[1]);
         c7x_horizontal_add_6_elems(vec_temp2, &vec_si.s[2]);
         c7x_horizontal_add_6_elems(vec_temp3, &vec_si.s[3]);
         c7x_horizontal_add_6_elems(vec_temp4, &vec_si.s[4]);
         c7x_horizontal_add_6_elems(vec_temp5, &vec_si.s[5]);

         vec_in1 += vec_si.s[1] * vec_superdiag_temp;
         vec_in2 += vec_si.s[2] * vec_superdiag_temp;
         vec_in3 += vec_si.s[3] * vec_superdiag_temp;
         vec_in4 += vec_si.s[4] * vec_superdiag_temp;
         vec_in5 += vec_si.s[5] * vec_superdiag_temp;

         vec vec_row_rev_mask = gen_u_row_process_rev_mask<vec>();
         vec_in0 *= vec_row_rev_mask;
         vec_in0 += vec_first_row * (*scale);
      }

#if U_PROC_SKIP_VLOAD
      __vstore_pred(pred_6_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      U[0 + 1 * colUStride] = vec_in1.s[0];
      U[0 + 2 * colUStride] = vec_in2.s[0];
      U[0 + 3 * colUStride] = vec_in3.s[0];
      U[0 + 4 * colUStride] = vec_in4.s[0];
      U[0 + 5 * colUStride] = vec_in5.s[0];
      vec_in0_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in1), 64u));
      vec_in1_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in2), 64u));
      vec_in2_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in3), 64u));
      vec_in3_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in4), 64u));
      vec_in4_u_proc        = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in5), 64u));

#else
      /* section 5- 8 */
      __vstore_pred(pred_6_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      __vstore_pred(pred_6_elem, (vec *) (&U[0 + 1 * colUStride]), vec_in1);
      __vstore_pred(pred_6_elem, (vec *) (&U[0 + 2 * colUStride]), vec_in2);
      __vstore_pred(pred_6_elem, (vec *) (&U[0 + 3 * colUStride]), vec_in3);
      __vstore_pred(pred_6_elem, (vec *) (&U[0 + 4 * colUStride]), vec_in4);
      __vstore_pred(pred_6_elem, (vec *) (&U[0 + 5 * colUStride]), vec_in5);
#endif
   }

   return;
}

template <typename dataType>
static inline void u_process_2nd_iter(dataType      *U,
                                      const int      colUStride,
                                      dataType      *diag,
                                      dataType      *superdiag,
                                      dataType      *scale,
                                      dataType      *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx);
template <>
inline void u_process_2nd_iter<float>(float         *U,
                                      const int      colUStride,
                                      float         *diag,
                                      float         *superdiag,
                                      float         *scale,
                                      float         *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx)
{
   return;
}
template <>
inline void u_process_2nd_iter<double>(double        *U,
                                       const int      colUStride,
                                       double        *diag,
                                       double        *superdiag,
                                       double        *scale,
                                       double        *s,
                                       const uint32_t num_cols,
                                       const uint32_t diag_idx)
{
   double s2 = 0, half_norm_squared = 0;
   /* -------------------------------------------------- */
   /* For i = 1 */
   /* -------------------------------------------------- */
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      __vpred pred_5_elem = __mask_long(num_cols);

#if U_PROC_SKIP_VLOAD
      vec vec_in0 = vec_in0_u_proc;
      vec vec_in1 = vec_in1_u_proc;
      vec vec_in2 = vec_in2_u_proc;
      vec vec_in3 = vec_in3_u_proc;
      vec vec_in4 = vec_in4_u_proc;
#else
      vec vec_in0 = __vload_pred(pred_5_elem, (vec *) (&U[0 + 0 * colUStride]));
      vec vec_in1 = __vload_pred(pred_5_elem, (vec *) (&U[0 + 1 * colUStride]));
      vec vec_in2 = __vload_pred(pred_5_elem, (vec *) (&U[0 + 2 * colUStride]));
      vec vec_in3 = __vload_pred(pred_5_elem, (vec *) (&U[0 + 3 * colUStride]));
      vec vec_in4 = __vload_pred(pred_5_elem, (vec *) (&U[0 + 4 * colUStride]));
#endif

      superdiag[diag_idx] = (*scale) * (*s);
      *scale              = 0;

      /* -------------------------------------------------- */
      /* U COLUMN PROCESS FOR i = 0 */
      /* -------------------------------------------------- */

      /* Accumulate i = 0 th column absolute values to get 'scale' */
      double in0_s0 = vec_in0.s[0];
      double in1_s0 = vec_in1.s[0];
      double in2_s0 = vec_in2.s[0];
      double in3_s0 = vec_in3.s[0];
      double in4_s0 = vec_in4.s[0];

      /* Store the first columns values in another vector */
      vec vec_first_col = (vec) 0.0;
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in0_s0), 0, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in1_s0), 1, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in2_s0), 2, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in3_s0), 3, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in4_s0), 4, c7x::as_long_vec(vec_first_col)));

      (*scale) += __abs(in0_s0);
      (*scale) += __abs(in1_s0);
      (*scale) += __abs(in2_s0);
      (*scale) += __abs(in3_s0);
      (*scale) += __abs(in4_s0);

      if ((*scale) > 0) {
         s2 = 0;
         /* normalize the 'vec_first_col' */
         vec_first_col        = vec_first_col * (vec) getRecip((*scale)); // / scale;
         vec vec_first_col_sq = vec_first_col * vec_first_col;
         c7x_horizontal_add_5_elems(vec_first_col_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_col.s[0] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_col.s[0] * (*s) - s2;
         vec_first_col.s[0] -= (*s);

         vec vec_si = (vec) 0;

         vec_si += (vec_first_col.s[0] * vec_in0);
         vec_si += (vec_first_col.s[1] * vec_in1);
         vec_si += (vec_first_col.s[2] * vec_in2);
         vec_si += (vec_first_col.s[3] * vec_in3);
         vec_si += (vec_first_col.s[4] * vec_in4);

         vec_si = vec_si * (vec) getRecip(half_norm_squared); // / half_norm_squared;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];
         vec_in4 += vec_si * vec_first_col.s[4];
      }
      vec_first_col *= (*scale);
      vec_in0.s[0] = vec_first_col.s[0];
      vec_in1.s[0] = vec_first_col.s[1];
      vec_in2.s[0] = vec_first_col.s[2];
      vec_in3.s[0] = vec_first_col.s[3];
      vec_in4.s[0] = vec_first_col.s[4];

      diag[diag_idx] = (*s) * (*scale);

      *s     = 0;
      *scale = 0;

      vec vec_row_mask = gen_u_row_process_mask<vec>();

      vec vec_first_row = vec_row_mask * vec_in0;
      c7x_horizontal_add_5_elems(__abs(vec_first_row), scale);

      if ((*scale) > 0) {
         s2                   = 0;
         vec_first_row        = vec_first_row * (vec) getRecip((*scale)); // / scale;
         vec vec_first_row_sq = vec_first_row * vec_first_row;
         c7x_horizontal_add_5_elems(vec_first_row_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_row.s[(num_cols - 1)] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_row.s[1] * (*s) - s2;
         vec_first_row.s[1] -= (*s);

         vec vec_superdiag_temp = vec_first_row * (vec) getRecip(half_norm_squared); //  / half_norm_squared;

         vec vec_si = (vec) 0;

         vec vec_temp1 = vec_first_row * vec_in1;
         vec vec_temp2 = vec_first_row * vec_in2;
         vec vec_temp3 = vec_first_row * vec_in3;
         vec vec_temp4 = vec_first_row * vec_in4;

         c7x_horizontal_add_5_elems(vec_temp1, &vec_si.s[1]);
         c7x_horizontal_add_5_elems(vec_temp2, &vec_si.s[2]);
         c7x_horizontal_add_5_elems(vec_temp3, &vec_si.s[3]);
         c7x_horizontal_add_5_elems(vec_temp4, &vec_si.s[4]);

         vec_in1 += vec_si.s[1] * vec_superdiag_temp;
         vec_in2 += vec_si.s[2] * vec_superdiag_temp;
         vec_in3 += vec_si.s[3] * vec_superdiag_temp;
         vec_in4 += vec_si.s[4] * vec_superdiag_temp;

         vec vec_row_rev_mask = gen_u_row_process_rev_mask<vec>();
         vec_in0 *= vec_row_rev_mask;
         vec_in0 += vec_first_row * (*scale);
      }

#if U_PROC_SKIP_VLOAD
      __vstore_pred(pred_5_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      U[0 + 1 * colUStride] = vec_in1.s[0];
      U[0 + 2 * colUStride] = vec_in2.s[0];
      U[0 + 3 * colUStride] = vec_in3.s[0];
      U[0 + 4 * colUStride] = vec_in4.s[0];

      vec_in0_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in1), 64u));
      vec_in1_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in2), 64u));
      vec_in2_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in3), 64u));
      vec_in3_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in4), 64u));
#else
      __vstore_pred(pred_5_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      __vstore_pred(pred_5_elem, (vec *) (&U[0 + 1 * colUStride]), vec_in1);
      __vstore_pred(pred_5_elem, (vec *) (&U[0 + 2 * colUStride]), vec_in2);
      __vstore_pred(pred_5_elem, (vec *) (&U[0 + 3 * colUStride]), vec_in3);
      __vstore_pred(pred_5_elem, (vec *) (&U[0 + 4 * colUStride]), vec_in4);
#endif
   }
   return;
}

template <typename dataType>
static inline void u_process_3rd_iter(dataType      *U,
                                      const int      colUStride,
                                      dataType      *diag,
                                      dataType      *superdiag,
                                      dataType      *scale,
                                      dataType      *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx);
template <>
inline void u_process_3rd_iter<float>(float         *U,
                                      const int      colUStride,
                                      float         *diag,
                                      float         *superdiag,
                                      float         *scale,
                                      float         *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx)
{
   return;
}
template <>
inline void u_process_3rd_iter<double>(double        *U,
                                       const int      colUStride,
                                       double        *diag,
                                       double        *superdiag,
                                       double        *scale,
                                       double        *s,
                                       const uint32_t num_cols,
                                       const uint32_t diag_idx)
{
   double s2 = 0, half_norm_squared = 0;
   /* -------------------------------------------------- */
   /* For i = 2 */
   /* -------------------------------------------------- */
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      __vpred pred_4_elem = __mask_long(num_cols);

#if U_PROC_SKIP_VLOAD
      vec vec_in0 = vec_in0_u_proc;
      vec vec_in1 = vec_in1_u_proc;
      vec vec_in2 = vec_in2_u_proc;
      vec vec_in3 = vec_in3_u_proc;
#else
      vec vec_in0 = __vload_pred(pred_4_elem, (vec *) (&U[0 + 0 * colUStride]));
      vec vec_in1 = __vload_pred(pred_4_elem, (vec *) (&U[0 + 1 * colUStride]));
      vec vec_in2 = __vload_pred(pred_4_elem, (vec *) (&U[0 + 2 * colUStride]));
      vec vec_in3 = __vload_pred(pred_4_elem, (vec *) (&U[0 + 3 * colUStride]));
#endif

      superdiag[diag_idx] = (*scale) * (*s);
      *scale              = 0;

      /* -------------------------------------------------- */
      /* U COLUMN PROCESS FOR i = 0 */
      /* -------------------------------------------------- */

      /* Accumulate i = 0 th column absolute values to get 'scale' */
      double in0_s0 = vec_in0.s[0];
      double in1_s0 = vec_in1.s[0];
      double in2_s0 = vec_in2.s[0];
      double in3_s0 = vec_in3.s[0];

      /* Store the first columns values in another vector */
      vec vec_first_col = (vec) 0.0;
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in0_s0), 0, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in1_s0), 1, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in2_s0), 2, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in3_s0), 3, c7x::as_long_vec(vec_first_col)));

      (*scale) += __abs(in0_s0);
      (*scale) += __abs(in1_s0);
      (*scale) += __abs(in2_s0);
      (*scale) += __abs(in3_s0);

      if ((*scale) > 0) {
         s2 = 0;
         /* normalize the 'vec_first_col' */
         vec_first_col        = vec_first_col * (vec) getRecip((*scale)); // / scale;
         vec vec_first_col_sq = vec_first_col * vec_first_col;
         c7x_horizontal_add_4_elems(vec_first_col_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_col.s[0] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_col.s[0] * (*s) - s2;
         vec_first_col.s[0] -= (*s);

         vec vec_si = (vec) 0;

         vec_si += (vec_first_col.s[0] * vec_in0);
         vec_si += (vec_first_col.s[1] * vec_in1);
         vec_si += (vec_first_col.s[2] * vec_in2);
         vec_si += (vec_first_col.s[3] * vec_in3);

         vec_si = vec_si * (vec) getRecip(half_norm_squared); // / half_norm_squared;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];
      }
      vec_first_col *= (*scale);
      vec_in0.s[0] = vec_first_col.s[0];
      vec_in1.s[0] = vec_first_col.s[1];
      vec_in2.s[0] = vec_first_col.s[2];
      vec_in3.s[0] = vec_first_col.s[3];

      diag[diag_idx] = (*s) * (*scale);

      *s     = 0;
      *scale = 0;

      vec vec_row_mask = gen_u_row_process_mask<vec>();

      vec vec_first_row = vec_row_mask * vec_in0;
      c7x_horizontal_add_4_elems(__abs(vec_first_row), scale);

      if ((*scale) > 0) {
         s2                   = 0;
         vec_first_row        = vec_first_row * (vec) getRecip((*scale)); // / scale;
         vec vec_first_row_sq = vec_first_row * vec_first_row;
         c7x_horizontal_add_4_elems(vec_first_row_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_row.s[(num_cols - 1)] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_row.s[1] * (*s) - s2;
         vec_first_row.s[1] -= (*s);

         vec vec_superdiag_temp = vec_first_row * (vec) getRecip(half_norm_squared); //  / half_norm_squared;

         vec vec_si = (vec) 0;

         vec vec_temp1 = vec_first_row * vec_in1;
         vec vec_temp2 = vec_first_row * vec_in2;
         vec vec_temp3 = vec_first_row * vec_in3;

         c7x_horizontal_add_4_elems(vec_temp1, &vec_si.s[1]);
         c7x_horizontal_add_4_elems(vec_temp2, &vec_si.s[2]);
         c7x_horizontal_add_4_elems(vec_temp3, &vec_si.s[3]);

         vec_in1 += vec_si.s[1] * vec_superdiag_temp;
         vec_in2 += vec_si.s[2] * vec_superdiag_temp;
         vec_in3 += vec_si.s[3] * vec_superdiag_temp;

         vec vec_row_rev_mask = gen_u_row_process_rev_mask<vec>();
         vec_in0 *= vec_row_rev_mask;
         vec_in0 += vec_first_row * (*scale);
      }

#if U_PROC_SKIP_VLOAD
      __vstore_pred(pred_4_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      U[0 + 1 * colUStride] = vec_in1.s[0];
      U[0 + 2 * colUStride] = vec_in2.s[0];
      U[0 + 3 * colUStride] = vec_in3.s[0];

      vec_in0_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in1), 64u));
      vec_in1_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in2), 64u));
      vec_in2_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in3), 64u));
#else
      __vstore_pred(pred_4_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      __vstore_pred(pred_4_elem, (vec *) (&U[0 + 1 * colUStride]), vec_in1);
      __vstore_pred(pred_4_elem, (vec *) (&U[0 + 2 * colUStride]), vec_in2);
      __vstore_pred(pred_4_elem, (vec *) (&U[0 + 3 * colUStride]), vec_in3);
#endif
   }
   return;
}

template <typename dataType>
static inline void u_process_4th_iter(dataType      *U,
                                      const int      colUStride,
                                      dataType      *diag,
                                      dataType      *superdiag,
                                      dataType      *scale,
                                      dataType      *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx);
template <>
inline void u_process_4th_iter<float>(float         *U,
                                      const int      colUStride,
                                      float         *diag,
                                      float         *superdiag,
                                      float         *scale,
                                      float         *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx)
{
   return;
}
template <>
inline void u_process_4th_iter<double>(double        *U,
                                       const int      colUStride,
                                       double        *diag,
                                       double        *superdiag,
                                       double        *scale,
                                       double        *s,
                                       const uint32_t num_cols,
                                       const uint32_t diag_idx)
{
   double s2 = 0, half_norm_squared = 0;
   /* -------------------------------------------------- */
   /* For i = 3 */
   /* -------------------------------------------------- */
   {
      typedef typename c7x::make_full_vector<double>::type vec;
      __vpred                                              pred_3_elem = __mask_long(num_cols);

#if U_PROC_SKIP_VLOAD
      vec vec_in0 = vec_in0_u_proc;
      vec vec_in1 = vec_in1_u_proc;
      vec vec_in2 = vec_in2_u_proc;
#else
      vec vec_in0 = __vload_pred(pred_3_elem, (vec *) (&U[0 + 0 * colUStride]));
      vec vec_in1 = __vload_pred(pred_3_elem, (vec *) (&U[0 + 1 * colUStride]));
      vec vec_in2 = __vload_pred(pred_3_elem, (vec *) (&U[0 + 2 * colUStride]));
#endif
      superdiag[diag_idx] = (*scale) * (*s);
      *scale              = 0;

      /* -------------------------------------------------- */
      /* U COLUMN PROCESS FOR i = 0 */
      /* -------------------------------------------------- */

      /* Accumulate i = 0 th column absolute values to get 'scale' */
      double in0_s0 = vec_in0.s[0];
      double in1_s0 = vec_in1.s[0];
      double in2_s0 = vec_in2.s[0];

      /* Store the first columns values in another vector */
      vec vec_first_col = (vec) 0.0;
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in0_s0), 0, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in1_s0), 1, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in2_s0), 2, c7x::as_long_vec(vec_first_col)));

      (*scale) += __abs(in0_s0);
      (*scale) += __abs(in1_s0);
      (*scale) += __abs(in2_s0);

      if ((*scale) > 0) {
         s2 = 0;
         /* normalize the 'vec_first_col' */
         vec_first_col        = vec_first_col * (vec) getRecip((*scale)); // / scale;
         vec vec_first_col_sq = vec_first_col * vec_first_col;
         c7x_horizontal_add_3_elems(vec_first_col_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_col.s[0] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_col.s[0] * (*s) - s2;
         vec_first_col.s[0] -= (*s);

         vec vec_si = (vec) 0;

         vec_si += (vec_first_col.s[0] * vec_in0);
         vec_si += (vec_first_col.s[1] * vec_in1);
         vec_si += (vec_first_col.s[2] * vec_in2);

         vec_si = vec_si * (vec) getRecip(half_norm_squared); // / half_norm_squared;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
      }
      vec_first_col *= (*scale);
      vec_in0.s[0] = vec_first_col.s[0];
      vec_in1.s[0] = vec_first_col.s[1];
      vec_in2.s[0] = vec_first_col.s[2];

      diag[diag_idx] = (*s) * (*scale);

      *s     = 0;
      *scale = 0;

      vec vec_row_mask = gen_u_row_process_mask<vec>();

      vec vec_first_row = vec_row_mask * vec_in0;
      c7x_horizontal_add_3_elems(__abs(vec_first_row), scale);

      if ((*scale) > 0) {
         s2                   = 0;
         vec_first_row        = vec_first_row * (vec) getRecip((*scale)); // / scale;
         vec vec_first_row_sq = vec_first_row * vec_first_row;
         c7x_horizontal_add_3_elems(vec_first_row_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_row.s[(num_cols - 1)] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_row.s[1] * (*s) - s2;
         vec_first_row.s[1] -= (*s);

         vec vec_superdiag_temp = vec_first_row * (vec) getRecip(half_norm_squared); //  / half_norm_squared;

         vec vec_si = (vec) 0;

         vec vec_temp1 = vec_first_row * vec_in1;
         vec vec_temp2 = vec_first_row * vec_in2;

         c7x_horizontal_add_3_elems(vec_temp1, &vec_si.s[1]);
         c7x_horizontal_add_3_elems(vec_temp2, &vec_si.s[2]);

         vec_in1 += vec_si.s[1] * vec_superdiag_temp;
         vec_in2 += vec_si.s[2] * vec_superdiag_temp;

         vec vec_row_rev_mask = gen_u_row_process_rev_mask<vec>();
         vec_in0 *= vec_row_rev_mask;
         vec_in0 += vec_first_row * (*scale);
      }

#if U_PROC_SKIP_VLOAD
      __vstore_pred(pred_3_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      U[0 + 1 * colUStride] = vec_in1.s[0];
      U[0 + 2 * colUStride] = vec_in2.s[0];

      vec_in0_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in1), 64u));
      vec_in1_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in2), 64u));
#else
      __vstore_pred(pred_3_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      __vstore_pred(pred_3_elem, (vec *) (&U[0 + 1 * colUStride]), vec_in1);
      __vstore_pred(pred_3_elem, (vec *) (&U[0 + 2 * colUStride]), vec_in2);
#endif
   }
   return;
}

template <typename dataType>
static inline void u_process_5th_iter(dataType      *U,
                                      const int      colUStride,
                                      dataType      *diag,
                                      dataType      *superdiag,
                                      dataType      *scale,
                                      dataType      *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx);
template <>
inline void u_process_5th_iter<float>(float         *U,
                                      const int      colUStride,
                                      float         *diag,
                                      float         *superdiag,
                                      float         *scale,
                                      float         *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx)
{
   return;
}
template <>
inline void u_process_5th_iter<double>(double        *U,
                                       const int      colUStride,
                                       double        *diag,
                                       double        *superdiag,
                                       double        *scale,
                                       double        *s,
                                       const uint32_t num_cols,
                                       const uint32_t diag_idx)
{
   double s2 = 0, half_norm_squared = 0;
   /* -------------------------------------------------- */
   /* For i = 4 */
   /* -------------------------------------------------- */

   {
      typedef typename c7x::make_full_vector<double>::type vec;

      __vpred pred_2_elem = __mask_long(num_cols);

#if U_PROC_SKIP_VLOAD
      vec vec_in0 = vec_in0_u_proc;
      vec vec_in1 = vec_in1_u_proc;
#else
      vec vec_in0 = __vload_pred(pred_2_elem, (vec *) (&U[0 + 0 * colUStride]));
      vec vec_in1 = __vload_pred(pred_2_elem, (vec *) (&U[0 + 1 * colUStride]));
#endif
      superdiag[diag_idx] = (*scale) * (*s);
      *scale              = 0;

      /* -------------------------------------------------- */
      /* U COLUMN PROCESS FOR i = 0 */
      /* -------------------------------------------------- */

      /* Accumulate i = 0 th column absolute values to get 'scale' */
      double in0_s0 = vec_in0.s[0];
      double in1_s0 = vec_in1.s[0];

      /* Store the first columns values in another vector */
      vec vec_first_col = (vec) 0.0;
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in0_s0), 0, c7x::as_long_vec(vec_first_col)));
      vec_first_col     = c7x::reinterpret<vec>(__vputd_dkv(__as_long(in1_s0), 1, c7x::as_long_vec(vec_first_col)));

      (*scale) += __abs(in0_s0);
      (*scale) += __abs(in1_s0);

      if ((*scale) > 0) {
         s2 = 0;
         /* normalize the 'vec_first_col' */
         vec_first_col        = vec_first_col * (vec) getRecip((*scale)); // / scale;
         vec vec_first_col_sq = vec_first_col * vec_first_col;
         c7x_horizontal_add_2_elems(vec_first_col_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_col.s[0] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_col.s[0] * (*s) - s2;
         vec_first_col.s[0] -= (*s);

         vec vec_si = (vec) 0;

         vec_si += (vec_first_col.s[0] * vec_in0);
         vec_si += (vec_first_col.s[1] * vec_in1);

         vec_si = vec_si * (vec) getRecip(half_norm_squared); // / half_norm_squared;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
      }
      vec_first_col *= (*scale);
      vec_in0.s[0] = vec_first_col.s[0];
      vec_in1.s[0] = vec_first_col.s[1];

      diag[diag_idx] = (*s) * (*scale);

      *s     = 0;
      *scale = 0;

      vec vec_row_mask = gen_u_row_process_mask<vec>();

      vec vec_first_row = vec_row_mask * vec_in0;
      c7x_horizontal_add_2_elems(__abs(vec_first_row), scale);

      if ((*scale) > 0) {
         s2                   = 0;
         vec_first_row        = vec_first_row * (vec) getRecip((*scale)); // / scale;
         vec vec_first_row_sq = vec_first_row * vec_first_row;
         c7x_horizontal_add_2_elems(vec_first_row_sq, &s2);

         *s = getSqrt(s2);
         if (vec_first_row.s[(num_cols - 1)] >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_row.s[1] * (*s) - s2;
         vec_first_row.s[1] -= (*s);

         vec vec_superdiag_temp = vec_first_row * (vec) getRecip(half_norm_squared); //  / half_norm_squared;

         vec vec_si = (vec) 0;

         vec vec_temp1 = vec_first_row * vec_in1;

         c7x_horizontal_add_2_elems(vec_temp1, &vec_si.s[1]);

         vec_in1 += vec_si.s[1] * vec_superdiag_temp;

         vec vec_row_rev_mask = gen_u_row_process_rev_mask<vec>();
         vec_in0 *= vec_row_rev_mask;
         vec_in0 += vec_first_row * (*scale);
      }

#if U_PROC_SKIP_VLOAD
      __vstore_pred(pred_2_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      __vstore_pred(pred_2_elem, (vec *) (&U[0 + 1 * colUStride]), vec_in1);

      vec_in0_u_proc = c7x::as_double_vec(__shift_right_full(c7x::as_long_vec(vec_in1), 64u));
#else
      __vstore_pred(pred_2_elem, (vec *) (&U[0 + 0 * colUStride]), vec_in0);
      __vstore_pred(pred_2_elem, (vec *) (&U[0 + 1 * colUStride]), vec_in1);
#endif
   }
   return;
}

template <typename dataType>
static inline void u_process_6th_iter(dataType      *U,
                                      const int      colUStride,
                                      dataType      *diag,
                                      dataType      *superdiag,
                                      dataType      *scale,
                                      dataType      *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx);
template <>
inline void u_process_6th_iter<float>(float         *U,
                                      const int      colUStride,
                                      float         *diag,
                                      float         *superdiag,
                                      float         *scale,
                                      float         *s,
                                      const uint32_t num_cols,
                                      const uint32_t diag_idx)
{
   return;
}
template <>
inline void u_process_6th_iter<double>(double        *U,
                                       const int      colUStride,
                                       double        *diag,
                                       double        *superdiag,
                                       double        *scale,
                                       double        *s,
                                       const uint32_t num_cols,
                                       const uint32_t diag_idx)
{
   double s2 = 0, half_norm_squared = 0;
   /* -------------------------------------------------- */
   /* For i = 5 */
   /* -------------------------------------------------- */
   {
      double vec_in0 = U[0 + 0 * colUStride];

      superdiag[5] = (*scale) * (*s);
      *scale       = 0;

      /* -------------------------------------------------- */
      /* U COLUMN PROCESS FOR i = 0 */
      /* -------------------------------------------------- */

      /* Accumulate i = 0 th column absolute values to get 'scale' */
      double in0_s0 = vec_in0;

      /* Store the first columns values in another vector */
      double vec_first_col = in0_s0;

      (*scale) += __abs(in0_s0);

      if ((*scale) > 0.0) {
         s2 = 0.0;
         /* normalize the 'vec_first_col' */
         vec_first_col           = vec_first_col * (double) getRecip((*scale)); // / scale;
         double vec_first_col_sq = vec_first_col * vec_first_col;
         s2                      = vec_first_col_sq;

         *s = getSqrt(s2);
         if (vec_first_col >= 0) {
            *s = -(*s);
         }

         half_norm_squared = vec_first_col * (*s) - s2;
         vec_first_col -= (*s);

         double vec_si = (double) 0.0;

         vec_si += (vec_first_col * vec_in0);

         vec_si = vec_si * (double) getRecip(half_norm_squared); // / half_norm_squared;

         vec_in0 += vec_si * vec_first_col;
      }
      vec_first_col *= (*scale);
      vec_in0 = vec_first_col;

      diag[5] = (*s) * (*scale);

      *s     = 0.0;
      *scale = 0.0;

      U[0 + 0 * colUStride] = vec_in0;
   }
   return;
}

/* ********************************************************************************** */
/* ********************************************************************************** */
/* *************************   U UPDATE PROCESS FOR 6X6  **************************** */
/* ********************************************************************************** */
/* ********************************************************************************** */

#if (__C7X_VEC_SIZE_BITS__ == 512)
template <typename X> static inline X gen_plusOneVec();

template <> inline c7x::float_vec gen_plusOneVec<c7x::float_vec>()
{
   c7x::float_vec ret_vec = c7x::float_vec(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   return ret_vec;
}

template <> inline c7x::double_vec gen_plusOneVec<c7x::double_vec>()
{
   c7x::double_vec ret_vec = c7x::double_vec(1, 0, 0, 0, 0, 0, 0, 0);
   return ret_vec;
}
#elif (__C7X_VEC_SIZE_BITS__ == 256)
template <typename X> static inline X gen_plusOneVec();

template <> inline c7x::float_vec gen_plusOneVec<c7x::float_vec>()
{
   c7x::float_vec ret_vec = c7x::float_vec(1, 0, 0, 0, 0, 0, 0, 0);
   return ret_vec;
}

template <> inline c7x::double_vec gen_plusOneVec<c7x::double_vec>()
{
   c7x::double_vec ret_vec = c7x::double_vec(1, 0, 0, 0);
   return ret_vec;
}

#endif

template <typename dataType>
static inline void      u_update_1st_iter(dataType *U, const int colUStride, dataType *diag, dataType s);
template <> inline void u_update_1st_iter<float>(float *U, const int colUStride, float *diag, float s) { return; }
template <> inline void u_update_1st_iter<double>(double *U, const int colUStride, double *diag, double s)
{
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      __vpred pred_1_elem        = __create_vpred(0x00000000000000FFU);
      vec     vec_in0            = (vec) 0;
      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      if (s != 0) {
         vec_in0 = __vload_pred(pred_1_elem, (vec *) U);
         vec_in0 = vec_in0 * (vec) getRecip(s) + vec_mask_first_row;
         __vstore_pred(pred_1_elem, (vec *) U, vec_in0);
      }
      else {
         __vstore_pred(pred_1_elem, (vec *) U, (vec_in0 + vec_mask_first_row));
      }
   }
   return;
}

template <typename dataType>
static inline void      u_update_2nd_iter(dataType *U, const int colUStride, dataType *diag, dataType s);
template <> inline void u_update_2nd_iter<float>(float *U, const int colUStride, float *diag, float s) { return; }
template <> inline void u_update_2nd_iter<double>(double *U, const int colUStride, double *diag, double s)
{
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_2_elem        = __create_vpred(0x000000000000FFFFU);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_2_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec vec_in1        = __vload_pred(pred_2_elem, (vec *) &U[0 + 1 * colUStride]);
         vec_first_col.s[1] = vec_in1.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];

         vec_first_col = vec_first_col * (vec) getRecip(s);

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_2_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_2_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_2_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
      }
   }
   return;
}

template <typename dataType>
static inline void      u_update_3rd_iter(dataType *U, const int colUStride, dataType *diag, dataType s);
template <> inline void u_update_3rd_iter<float>(float *U, const int colUStride, float *diag, float s) { return; }
template <> inline void u_update_3rd_iter<double>(double *U, const int colUStride, double *diag, double s)
{
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_3_elem        = __create_vpred(0x0000000000FFFFFFU);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_3_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec vec_in1 = __vload_pred(pred_3_elem, (vec *) &U[0 + 1 * colUStride]);
         vec vec_in2 = __vload_pred(pred_3_elem, (vec *) &U[0 + 2 * colUStride]);

         vec_first_col.s[1] = vec_in1.s[0];
         vec_first_col.s[2] = vec_in2.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;
         vec_si += vec_first_col.s[2] * vec_in2;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];

         vec_first_col = vec_first_col * (vec) getRecip(s);

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];
         vec_in2.s[0] = vec_first_col.s[2];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_3_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_3_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
         __vstore_pred(pred_3_elem, (vec *) &U[0 + 2 * colUStride], vec_in2);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_3_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
         U[0 + 2 * colUStride] = 0.0;
      }
   }
   return;
}

template <typename dataType>
static inline void      u_update_4th_iter(dataType *U, const int colUStride, dataType *diag, dataType s);
template <> inline void u_update_4th_iter<float>(float *U, const int colUStride, float *diag, float s) { return; }
template <> inline void u_update_4th_iter<double>(double *U, const int colUStride, double *diag, double s)
{
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_4_elem        = __create_vpred(0x00000000FFFFFFFFU);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec vec_in1 = __vload_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride]);
         vec vec_in2 = __vload_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride]);
         vec vec_in3 = __vload_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride]);

         vec_first_col.s[1] = vec_in1.s[0];
         vec_first_col.s[2] = vec_in2.s[0];
         vec_first_col.s[3] = vec_in3.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;
         vec_si += vec_first_col.s[2] * vec_in2;
         vec_si += vec_first_col.s[3] * vec_in3;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];

         vec_first_col = vec_first_col * (vec) getRecip(s);

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];
         vec_in2.s[0] = vec_first_col.s[2];
         vec_in3.s[0] = vec_first_col.s[3];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride], vec_in2);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride], vec_in3);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
         U[0 + 2 * colUStride] = 0.0;
         U[0 + 3 * colUStride] = 0.0;
      }
   }
   return;
}

template <typename dataType>
static inline void      u_update_5th_iter(dataType *U, const int colUStride, dataType *diag, dataType s);
template <> inline void u_update_5th_iter<float>(float *U, const int colUStride, float *diag, float s) { return; }
template <> inline void u_update_5th_iter<double>(double *U, const int colUStride, double *diag, double s)
{
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_4_elem        = __create_vpred(0x000000FFFFFFFFFFU);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec vec_in1 = __vload_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride]);
         vec vec_in2 = __vload_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride]);
         vec vec_in3 = __vload_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride]);
         vec vec_in4 = __vload_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride]);

         vec_first_col.s[1] = vec_in1.s[0];
         vec_first_col.s[2] = vec_in2.s[0];
         vec_first_col.s[3] = vec_in3.s[0];
         vec_first_col.s[4] = vec_in4.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;
         vec_si += vec_first_col.s[2] * vec_in2;
         vec_si += vec_first_col.s[3] * vec_in3;
         vec_si += vec_first_col.s[4] * vec_in4;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];
         vec_in4 += vec_si * vec_first_col.s[4];

         vec_first_col = vec_first_col * (vec) getRecip(s);

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];
         vec_in2.s[0] = vec_first_col.s[2];
         vec_in3.s[0] = vec_first_col.s[3];
         vec_in4.s[0] = vec_first_col.s[4];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride], vec_in2);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride], vec_in3);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride], vec_in4);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
         U[0 + 2 * colUStride] = 0.0;
         U[0 + 3 * colUStride] = 0.0;
         U[0 + 4 * colUStride] = 0.0;
      }
   }
   return;
}

template <typename dataType>
static inline void      u_update_6th_iter(dataType *U, const int colUStride, dataType *diag, dataType s);
template <> inline void u_update_6th_iter<float>(float *U, const int colUStride, float *diag, float s) { return; }
template <> inline void u_update_6th_iter<double>(double *U, const int colUStride, double *diag, double s)
{

   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_4_elem        = __create_vpred(0x0000FFFFFFFFFFFFU);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec vec_in1 = __vload_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride]);
         vec vec_in2 = __vload_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride]);
         vec vec_in3 = __vload_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride]);
         vec vec_in4 = __vload_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride]);
         vec vec_in5 = __vload_pred(pred_4_elem, (vec *) &U[0 + 5 * colUStride]);

         vec_first_col.s[1] = vec_in1.s[0];
         vec_first_col.s[2] = vec_in2.s[0];
         vec_first_col.s[3] = vec_in3.s[0];
         vec_first_col.s[4] = vec_in4.s[0];
         vec_first_col.s[5] = vec_in5.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;
         vec_si += vec_first_col.s[2] * vec_in2;
         vec_si += vec_first_col.s[3] * vec_in3;
         vec_si += vec_first_col.s[4] * vec_in4;
         vec_si += vec_first_col.s[5] * vec_in5;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];
         vec_in4 += vec_si * vec_first_col.s[4];
         vec_in5 += vec_si * vec_first_col.s[5];

         vec_first_col = vec_first_col * (vec) getRecip(s);

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];
         vec_in2.s[0] = vec_first_col.s[2];
         vec_in3.s[0] = vec_first_col.s[3];
         vec_in4.s[0] = vec_first_col.s[4];
         vec_in5.s[0] = vec_first_col.s[5];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride], vec_in2);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride], vec_in3);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride], vec_in4);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 5 * colUStride], vec_in5);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
         U[0 + 2 * colUStride] = 0.0;
         U[0 + 3 * colUStride] = 0.0;
         U[0 + 4 * colUStride] = 0.0;
         U[0 + 5 * colUStride] = 0.0;
      }
   }
   return;
}

/* ********************************************************************************** */
/* ********************************************************************************** */
/* *************************   U UPDATE PROCESS FOR 6X7 REDUCE FORM ***************** */
/* ********************************************************************************** */
/* ********************************************************************************** */

template <typename dataType>
static inline void
u_update_6X7_NR_1st_iter(dataType *U, const int colUStride, dataType *diag, dataType s, const uint32_t col_elems);
template <>
inline void
u_update_6X7_NR_1st_iter<float>(float *U, const int colUStride, float *diag, float s, const uint32_t col_elems)
{
   return;
}
template <>
inline void
u_update_6X7_NR_1st_iter<double>(double *U, const int colUStride, double *diag, double s, const uint32_t col_elems)
{
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      __vpred pred_1_elem = __mask_long(col_elems);
      vec     vec_in0, vec_in1;
      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      if (s != 0) {
         vec recip_s = (vec) getRecip(s);
         vec_in0     = __vload_pred(pred_1_elem, (vec *) &U[0 + 0 * colUStride]);
         vec_in1     = __vload_pred(pred_1_elem, (vec *) &U[0 + 1 * colUStride]);

         vec_in0 = vec_in0 * recip_s + vec_mask_first_row;
         vec_in1 = vec_in1 * recip_s;

         __vstore_pred(pred_1_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_1_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
      }
      else {
         __vstore_pred(pred_1_elem, (vec *) U, (vec_in0 + vec_mask_first_row));
         U[0 + 1 * colUStride] = 0.0;
      }
   }
   return;
}

template <typename dataType>
static inline void
u_update_6X7_NR_2nd_iter(dataType *U, const int colUStride, dataType *diag, dataType s, const uint32_t col_elems);
template <>
inline void
u_update_6X7_NR_2nd_iter<float>(float *U, const int colUStride, float *diag, float s, const uint32_t col_elems)
{
   return;
}
template <>
inline void
u_update_6X7_NR_2nd_iter<double>(double *U, const int colUStride, double *diag, double s, const uint32_t col_elems)
{

   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_2_elem        = __mask_long(col_elems);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_2_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec recip_s = (vec) getRecip(s);
         vec vec_in1 = __vload_pred(pred_2_elem, (vec *) &U[0 + 1 * colUStride]);
         vec vec_in2 = __vload_pred(pred_2_elem, (vec *) &U[0 + 2 * colUStride]);

         vec_first_col.s[1] = vec_in1.s[0];
         vec_first_col.s[2] = vec_in2.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;
         vec_si += vec_first_col.s[2] * vec_in2;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];

         vec_first_col = vec_first_col * recip_s;

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];
         vec_in2.s[0] = vec_first_col.s[2];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_2_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_2_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
         __vstore_pred(pred_2_elem, (vec *) &U[0 + 2 * colUStride], vec_in2);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_2_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
         U[0 + 2 * colUStride] = 0.0;
      }
   }
   return;
}

template <typename dataType>
static inline void
u_update_6X7_NR_3rd_iter(dataType *U, const int colUStride, dataType *diag, dataType s, const uint32_t col_elems);
template <>
inline void
u_update_6X7_NR_3rd_iter<float>(float *U, const int colUStride, float *diag, float s, const uint32_t col_elems)
{
   return;
}
template <>
inline void
u_update_6X7_NR_3rd_iter<double>(double *U, const int colUStride, double *diag, double s, const uint32_t col_elems)
{
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_3_elem        = __mask_long(col_elems);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_3_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec recip_s = (vec) getRecip(s);
         vec vec_in1 = __vload_pred(pred_3_elem, (vec *) &U[0 + 1 * colUStride]);
         vec vec_in2 = __vload_pred(pred_3_elem, (vec *) &U[0 + 2 * colUStride]);
         vec vec_in3 = __vload_pred(pred_3_elem, (vec *) &U[0 + 3 * colUStride]);

         vec_first_col.s[1] = vec_in1.s[0];
         vec_first_col.s[2] = vec_in2.s[0];
         vec_first_col.s[3] = vec_in3.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;
         vec_si += vec_first_col.s[2] * vec_in2;
         vec_si += vec_first_col.s[3] * vec_in3;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];

         vec_first_col = vec_first_col * recip_s;

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];
         vec_in2.s[0] = vec_first_col.s[2];
         vec_in3.s[0] = vec_first_col.s[3];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_3_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_3_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
         __vstore_pred(pred_3_elem, (vec *) &U[0 + 2 * colUStride], vec_in2);
         __vstore_pred(pred_3_elem, (vec *) &U[0 + 3 * colUStride], vec_in3);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_3_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
         U[0 + 2 * colUStride] = 0.0;
         U[0 + 3 * colUStride] = 0.0;
      }
   }
   return;
}

template <typename dataType>
static inline void
u_update_6X7_NR_4th_iter(dataType *U, const int colUStride, dataType *diag, dataType s, const uint32_t col_elems);
template <>
inline void
u_update_6X7_NR_4th_iter<float>(float *U, const int colUStride, float *diag, float s, const uint32_t col_elems)
{
   return;
}
template <>
inline void
u_update_6X7_NR_4th_iter<double>(double *U, const int colUStride, double *diag, double s, const uint32_t col_elems)
{
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_4_elem        = __mask_long(col_elems);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec recip_s = (vec) getRecip(s);
         vec vec_in1 = __vload_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride]);
         vec vec_in2 = __vload_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride]);
         vec vec_in3 = __vload_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride]);
         vec vec_in4 = __vload_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride]);

         vec_first_col.s[1] = vec_in1.s[0];
         vec_first_col.s[2] = vec_in2.s[0];
         vec_first_col.s[3] = vec_in3.s[0];
         vec_first_col.s[4] = vec_in4.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;
         vec_si += vec_first_col.s[2] * vec_in2;
         vec_si += vec_first_col.s[3] * vec_in3;
         vec_si += vec_first_col.s[4] * vec_in4;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];
         vec_in4 += vec_si * vec_first_col.s[4];

         vec_first_col = vec_first_col * recip_s;

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];
         vec_in2.s[0] = vec_first_col.s[2];
         vec_in3.s[0] = vec_first_col.s[3];
         vec_in4.s[0] = vec_first_col.s[4];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride], vec_in2);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride], vec_in3);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride], vec_in4);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
         U[0 + 2 * colUStride] = 0.0;
         U[0 + 3 * colUStride] = 0.0;
         U[0 + 4 * colUStride] = 0.0;
      }
   }

   return;
}

template <typename dataType>
static inline void
u_update_6X7_NR_5th_iter(dataType *U, const int colUStride, dataType *diag, dataType s, const uint32_t col_elems);
template <>
inline void
u_update_6X7_NR_5th_iter<float>(float *U, const int colUStride, float *diag, float s, const uint32_t col_elems)
{
   return;
}
template <>
inline void
u_update_6X7_NR_5th_iter<double>(double *U, const int colUStride, double *diag, double s, const uint32_t col_elems)
{

   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_4_elem        = __mask_long(col_elems);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec recip_s = (vec) getRecip(s);
         vec vec_in1 = __vload_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride]);
         vec vec_in2 = __vload_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride]);
         vec vec_in3 = __vload_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride]);
         vec vec_in4 = __vload_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride]);
         vec vec_in5 = __vload_pred(pred_4_elem, (vec *) &U[0 + 5 * colUStride]);

         vec_first_col.s[1] = vec_in1.s[0];
         vec_first_col.s[2] = vec_in2.s[0];
         vec_first_col.s[3] = vec_in3.s[0];
         vec_first_col.s[4] = vec_in4.s[0];
         vec_first_col.s[5] = vec_in5.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;
         vec_si += vec_first_col.s[2] * vec_in2;
         vec_si += vec_first_col.s[3] * vec_in3;
         vec_si += vec_first_col.s[4] * vec_in4;
         vec_si += vec_first_col.s[5] * vec_in5;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];
         vec_in4 += vec_si * vec_first_col.s[4];
         vec_in5 += vec_si * vec_first_col.s[5];

         vec_first_col = vec_first_col * recip_s;

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];
         vec_in2.s[0] = vec_first_col.s[2];
         vec_in3.s[0] = vec_first_col.s[3];
         vec_in4.s[0] = vec_first_col.s[4];
         vec_in5.s[0] = vec_first_col.s[5];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride], vec_in2);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride], vec_in3);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride], vec_in4);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 5 * colUStride], vec_in5);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
         U[0 + 2 * colUStride] = 0.0;
         U[0 + 3 * colUStride] = 0.0;
         U[0 + 4 * colUStride] = 0.0;
         U[0 + 5 * colUStride] = 0.0;
      }
   }
   return;
}

template <typename dataType>
static inline void
u_update_6X7_NR_6th_iter(dataType *U, const int colUStride, dataType *diag, dataType s, const uint32_t col_elems);
template <>
inline void
u_update_6X7_NR_6th_iter<float>(float *U, const int colUStride, float *diag, float s, const uint32_t col_elems)
{
   return;
}
template <>
inline void
u_update_6X7_NR_6th_iter<double>(double *U, const int colUStride, double *diag, double s, const uint32_t col_elems)
{
   {
      typedef typename c7x::make_full_vector<double>::type vec;

      vec     vec_mask_first_row = gen_plusOneVec<vec>();
      __vpred pred_4_elem        = __mask_long(col_elems);

      vec vec_first_col   = (vec) 0;
      vec vec_si          = (vec) 0.0;
      vec vec_in0         = __vload_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride]);
      vec_first_col.s[0]  = vec_in0.s[0];
      vec_in0             = vec_in0 * vec_mask_first_row;
      vec norm_factor     = (vec) (vec_in0.s[0] * s);
      vec inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         vec recip_s = (vec) getRecip(s);
         vec vec_in1 = __vload_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride]);
         vec vec_in2 = __vload_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride]);
         vec vec_in3 = __vload_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride]);
         vec vec_in4 = __vload_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride]);
         vec vec_in5 = __vload_pred(pred_4_elem, (vec *) &U[0 + 5 * colUStride]);
         vec vec_in6 = __vload_pred(pred_4_elem, (vec *) &U[0 + 6 * colUStride]);

         vec_first_col.s[1] = vec_in1.s[0];
         vec_first_col.s[2] = vec_in2.s[0];
         vec_first_col.s[3] = vec_in3.s[0];
         vec_first_col.s[4] = vec_in4.s[0];
         vec_first_col.s[5] = vec_in5.s[0];
         vec_first_col.s[6] = vec_in6.s[0];

         vec_si += vec_first_col.s[1] * vec_in1;
         vec_si += vec_first_col.s[2] * vec_in2;
         vec_si += vec_first_col.s[3] * vec_in3;
         vec_si += vec_first_col.s[4] * vec_in4;
         vec_si += vec_first_col.s[5] * vec_in5;
         vec_si += vec_first_col.s[6] * vec_in6;

         vec_si = vec_si * inv_norm_factor;

         vec_in0 += vec_si * vec_first_col.s[0];
         vec_in1 += vec_si * vec_first_col.s[1];
         vec_in2 += vec_si * vec_first_col.s[2];
         vec_in3 += vec_si * vec_first_col.s[3];
         vec_in4 += vec_si * vec_first_col.s[4];
         vec_in5 += vec_si * vec_first_col.s[5];
         vec_in6 += vec_si * vec_first_col.s[6];

         vec_first_col = vec_first_col * recip_s;

         vec_in0.s[0] = vec_first_col.s[0];
         vec_in1.s[0] = vec_first_col.s[1];
         vec_in2.s[0] = vec_first_col.s[2];
         vec_in3.s[0] = vec_first_col.s[3];
         vec_in4.s[0] = vec_first_col.s[4];
         vec_in5.s[0] = vec_first_col.s[5];
         vec_in6.s[0] = vec_first_col.s[6];

         vec_in0 += vec_mask_first_row;

         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 1 * colUStride], vec_in1);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 2 * colUStride], vec_in2);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 3 * colUStride], vec_in3);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 4 * colUStride], vec_in4);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 5 * colUStride], vec_in5);
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 6 * colUStride], vec_in6);
      }
      else {
         vec_in0 += vec_mask_first_row;
         __vstore_pred(pred_4_elem, (vec *) &U[0 + 0 * colUStride], vec_in0);
         U[0 + 1 * colUStride] = 0.0;
         U[0 + 2 * colUStride] = 0.0;
         U[0 + 3 * colUStride] = 0.0;
         U[0 + 4 * colUStride] = 0.0;
         U[0 + 5 * colUStride] = 0.0;
         U[0 + 6 * colUStride] = 0.0;
      }
   }
   return;
}

template <typename dataType>
static inline void
u_update_6X7_R_1st_iter(dataType *U, const int colUStride, dataType *diag, dataType s, const uint32_t col_elems);
template <>
inline void
u_update_6X7_R_1st_iter<float>(float *U, const int colUStride, float *diag, float s, const uint32_t col_elems)
{
   return;
}
template <>
inline void
u_update_6X7_R_1st_iter<double>(double *U, const int colUStride, double *diag, double s, const uint32_t col_elems)
{
   {

      double u_0_0 = U[0 + 0 * colUStride];
      double u_1_0 = U[0 + 1 * colUStride];

      double norm_factor     = (u_0_0 * s);
      double inv_norm_factor = getRecip(norm_factor);

      if (s != 0) {
         double si      = 0;
         double recip_s = getRecip(s);

         si = u_1_0 * U[1 + 1 * colUStride];
         si = si * inv_norm_factor;
         U[1 + 1 * colUStride] += si * u_1_0;

         U[1 + 0 * colUStride] = u_0_0 * u_1_0 * inv_norm_factor;
         U[1 + 1 * colUStride] = (u_1_0 * u_1_0 * inv_norm_factor) + 1;

         U[0 + 0 * colUStride] = (u_0_0 * recip_s) + 1;
         U[0 + 1 * colUStride] = u_1_0 * recip_s;
      }
      else {
         U[1 + 1 * colUStride] = 1;
         U[0 + 0 * colUStride] = 1;
         U[0 + 1 * colUStride] = 0;
      }
   }
   return;
}

#endif /* #if (__C7X_VEC_SIZE_BITS__ == 512) */
