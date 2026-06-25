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

template <typename dataType>
inline void sort_singular_vals(dataType *diag,
                               dataType *U,
                               dataType *V,
                               const int Nrows,
                               const int Ncols,
                               const int colUStride,
                               const int colVStride);

template <>
inline void sort_singular_vals<float>(float    *diag,
                                      float    *U,
                                      float    *V,
                                      const int Nrows,
                                      const int Ncols,
                                      const int colUStride,
                                      const int colVStride)
{
   return;
}

template <>
inline void sort_singular_vals<double>(double   *singular_values,
                                       double   *U,
                                       double   *V,
                                       const int Nrows,
                                       const int Ncols,
                                       const int colUStride,
                                       const int colVStride)
{

   typedef typename c7x::make_full_vector<double>::type vec_dp;
   typedef typename c7x::make_full_vector<float>::type  vec_sp;

   __vpred pred_diag_elems = __mask_long((uint32_t) Ncols);

   vec_dp vec_diag = __vload_pred(pred_diag_elems, (vec_dp *) singular_values);

   vec_sp vec_diag_temp = __double_to_float(vec_diag);

   vec_sp test_vec_1 = c7x::as_float_vec(__shift_left_full(c7x::as_ulong_vec(vec_diag_temp), 32u));

   vec_sp test_vec_2 = vec_diag_temp + test_vec_1;

   c7x::uchar_vec vec_rearrange = __sort_desc_perm(test_vec_2);

   vec_rearrange = c7x::as_uchar_vec(__swap(c7x::as_uint_vec(vec_rearrange)));

   vec_sp vec_sorted_temp = __sort_desc(test_vec_2);
   vec_dp vec_sorted_sv   = __low_float_to_double(vec_sorted_temp);
   __vstore_pred(pred_diag_elems, (vec_dp *) singular_values, vec_sorted_sv);

   /* ---------------------------------------------------------------------------- */
   /* REARRANGE V MATRIX  */
   /* ---------------------------------------------------------------------------- */
   vec_dp vec_v_in0 = __vload_pred(pred_diag_elems, (vec_dp *) (V + 0 * colVStride));
   vec_dp vec_v_in1 = __vload_pred(pred_diag_elems, (vec_dp *) (V + 1 * colVStride));
   vec_dp vec_v_in2 = __vload_pred(pred_diag_elems, (vec_dp *) (V + 2 * colVStride));
   vec_dp vec_v_in3 = __vload_pred(pred_diag_elems, (vec_dp *) (V + 3 * colVStride));
   vec_dp vec_v_in4 = __vload_pred(pred_diag_elems, (vec_dp *) (V + 4 * colVStride));
   vec_dp vec_v_in5 = __vload_pred(pred_diag_elems, (vec_dp *) (V + 5 * colVStride));

   vec_v_in0 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_v_in0)));
   vec_v_in1 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_v_in1)));
   vec_v_in2 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_v_in2)));
   vec_v_in3 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_v_in3)));
   vec_v_in4 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_v_in4)));
   vec_v_in5 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_v_in5)));

   __vstore_pred(pred_diag_elems, (vec_dp *) (V + 0 * colVStride), vec_v_in0);
   __vstore_pred(pred_diag_elems, (vec_dp *) (V + 1 * colVStride), vec_v_in1);
   __vstore_pred(pred_diag_elems, (vec_dp *) (V + 2 * colVStride), vec_v_in2);
   __vstore_pred(pred_diag_elems, (vec_dp *) (V + 3 * colVStride), vec_v_in3);
   __vstore_pred(pred_diag_elems, (vec_dp *) (V + 4 * colVStride), vec_v_in4);
   __vstore_pred(pred_diag_elems, (vec_dp *) (V + 5 * colVStride), vec_v_in5);

   /* ---------------------------------------------------------------------------- */
   /* REARRANGE U MATRIX  */
   /* ---------------------------------------------------------------------------- */
   vec_dp vec_u_in0 = __vload_pred(pred_diag_elems, (vec_dp *) (U + 0 * colUStride));
   vec_dp vec_u_in1 = __vload_pred(pred_diag_elems, (vec_dp *) (U + 1 * colUStride));
   vec_dp vec_u_in2 = __vload_pred(pred_diag_elems, (vec_dp *) (U + 2 * colUStride));
   vec_dp vec_u_in3 = __vload_pred(pred_diag_elems, (vec_dp *) (U + 3 * colUStride));
   vec_dp vec_u_in4 = __vload_pred(pred_diag_elems, (vec_dp *) (U + 4 * colUStride));
   vec_dp vec_u_in5 = __vload_pred(pred_diag_elems, (vec_dp *) (U + 5 * colUStride));

   vec_u_in0 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_u_in0)));
   vec_u_in1 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_u_in1)));
   vec_u_in2 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_u_in2)));
   vec_u_in3 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_u_in3)));
   vec_u_in4 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_u_in4)));
   vec_u_in5 = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_u_in5)));

   __vstore_pred(pred_diag_elems, (vec_dp *) (U + 0 * colUStride), vec_u_in0);
   __vstore_pred(pred_diag_elems, (vec_dp *) (U + 1 * colUStride), vec_u_in1);
   __vstore_pred(pred_diag_elems, (vec_dp *) (U + 2 * colUStride), vec_u_in2);
   __vstore_pred(pred_diag_elems, (vec_dp *) (U + 3 * colUStride), vec_u_in3);
   __vstore_pred(pred_diag_elems, (vec_dp *) (U + 4 * colUStride), vec_u_in4);
   __vstore_pred(pred_diag_elems, (vec_dp *) (U + 5 * colUStride), vec_u_in5);

   if (Nrows == 7) {
      vec_dp vec_u_in6 = __vload_pred(pred_diag_elems, (vec_dp *) (U + 6 * colUStride));
      vec_u_in6        = c7x::as_double_vec(__permute(vec_rearrange, c7x::as_uchar_vec(vec_u_in6)));
      __vstore_pred(pred_diag_elems, (vec_dp *) (U + 6 * colUStride), vec_u_in6);
   }

   return;
}

#endif /* #if (__C7X_VEC_SIZE_BITS__ == 512) */
