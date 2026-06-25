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

// set_epsilon
template <typename dataType> inline dataType set_epsilon(dataType *diag, dataType *superdiag);
template <> inline float                     set_epsilon<float>(float *diag, float *superdiag) { return FLT_EPSILON; }

template <> inline double set_epsilon<double>(double *diag, double *superdiag)
{

   typedef typename c7x::make_full_vector<double>::type vec;
   __vpred                                              pred_6_elem   = __mask_long(6U);
   vec                                                  vec_diag      = __vload_pred(pred_6_elem, (vec *) diag);
   vec                                                  vec_superdiag = __vload_pred(pred_6_elem, (vec *) superdiag);
   vec                                                  total_vec     = __abs(vec_diag) + __abs(vec_superdiag);

   double max_val = c7x_horizontal_max_fp<double, vec>(total_vec);
   return (DBL_EPSILON * max_val);
}

template <typename dataType>
inline int32_t
rotation_test_check(dataType *diag, dataType *superdiag, int32_t *m, const dataType epsilon, const uint32_t k);
template <>
inline int32_t
rotation_test_check<float>(float *diag, float *superdiag, int32_t *m, const float epsilon, const uint32_t k)
{
   return FLT_EPSILON;
}

template <>
inline int32_t
rotation_test_check<double>(double *diag, double *superdiag, int32_t *m, const double epsilon, const uint32_t k)
{

   typedef typename c7x::make_full_vector<double>::type vec;

   vec fill_max_val = (vec) std::numeric_limits<double>::max();
   vec vec_epsilon  = (vec) epsilon;

   __vpred pred_k_elem   = __mask_long(k);
   __vpred pred_k_1_elem = __mask_long((k - 1U));

   vec vec_superdiag = __vload_pred(pred_k_elem, (vec *) superdiag);
   vec vec_diag      = __vload_pred(pred_k_1_elem, (vec *) diag);

   vec_superdiag = __select(pred_k_elem, vec_superdiag, fill_max_val);
   vec_diag      = __select(pred_k_1_elem, vec_diag, fill_max_val);

   __vpred pred_superdiag = __cmp_le_pred(__abs(vec_superdiag), vec_epsilon);
   __vpred pred_diag      = __cmp_le_pred(__abs(vec_diag), vec_epsilon);

   uint64_t reg_superdiag = _mvpd(pred_superdiag);
   uint64_t reg_diag      = _mvpd(pred_diag);

   uint64_t leftmost_bit_SD = __leftmost_bit_detect_one(reg_superdiag);
   uint64_t leftmost_bit_D  = __leftmost_bit_detect_one(reg_diag);

   int32_t m_superdiag = (int32_t) (63 - (int) leftmost_bit_SD);
   int32_t m_diag      = (int32_t) (63 - (int) leftmost_bit_D);

   __vpred pred_m_compare = __cmp_ge_pred(m_superdiag, m_diag);

   *m                    = __select(pred_m_compare, m_superdiag, m_diag);
   int32_t rotation_test = __select(pred_m_compare, 0, 1);

   return rotation_test;
}

template <typename T>
static inline void diag_superdiag_sqrt(T *diag, T *superdiag, const int32_t start_idx, const uint32_t length);

template <>
inline void diag_superdiag_sqrt<float>(float *diag, float *superdiag, const int32_t start_idx, const uint32_t length)
{
   return;
}

template <>
inline void diag_superdiag_sqrt<double>(double *diag, double *superdiag, const int32_t start_idx, const uint32_t length)
{
   typedef typename c7x::make_full_vector<double>::type vec;

   __vpred pred_elems = __mask_long(length);

   vec vec_diag      = __vload_pred(pred_elems, (vec *) (diag + start_idx));
   vec vec_superdiag = __vload_pred(pred_elems, (vec *) (superdiag + start_idx));

   vec_diag      = getSqrt<vec>(vec_diag);
   vec_superdiag = getSqrt<vec>(vec_superdiag);

   __vstore_pred(pred_elems, (vec *) (diag + start_idx), vec_diag);
   __vstore_pred(pred_elems, (vec *) (superdiag + start_idx), vec_superdiag);

   return;
}

#if (__C7X_VEC_SIZE_BITS__ == 512)
const c7x::uchar_vec vperm_linear_mask = c7x::uchar_vec(0,
                                                        1,
                                                        2,
                                                        3,
                                                        4,
                                                        5,
                                                        6,
                                                        7,
                                                        8,
                                                        9,
                                                        10,
                                                        11,
                                                        12,
                                                        13,
                                                        14,
                                                        15,
                                                        16,
                                                        17,
                                                        18,
                                                        19,
                                                        20,
                                                        21,
                                                        22,
                                                        23,
                                                        24,
                                                        25,
                                                        26,
                                                        27,
                                                        28,
                                                        29,
                                                        30,
                                                        31,
                                                        32,
                                                        33,
                                                        34,
                                                        35,
                                                        36,
                                                        37,
                                                        38,
                                                        39,
                                                        40,
                                                        41,
                                                        42,
                                                        43,
                                                        44,
                                                        45,
                                                        46,
                                                        47,
                                                        48,
                                                        49,
                                                        50,
                                                        51,
                                                        52,
                                                        53,
                                                        54,
                                                        55,
                                                        56,
                                                        57,
                                                        58,
                                                        59,
                                                        60,
                                                        61,
                                                        62,
                                                        63);
#elif (__C7X_VEC_SIZE_BITS__ == 256)
const c7x::uchar_vec vperm_linear_mask = c7x::uchar_vec(0,
                                                        1,
                                                        2,
                                                        3,
                                                        4,
                                                        5,
                                                        6,
                                                        7,
                                                        8,
                                                        9,
                                                        10,
                                                        11,
                                                        12,
                                                        13,
                                                        14,
                                                        15,
                                                        16,
                                                        17,
                                                        18,
                                                        19,
                                                        20,
                                                        21,
                                                        22,
                                                        23,
                                                        24,
                                                        25,
                                                        26,
                                                        27,
                                                        28,
                                                        29,
                                                        30,
                                                        31);
#else
#error "Invalid Target"
#endif

template <typename dataType>
static inline void transpose_vec_mat(dataType *in_mat, const int Nrows, const int Ncols, const int colStride);

template <> inline void transpose_vec_mat<float>(float *in_mat, const int Nrows, const int Ncols, const int colStride)
{
   return;
}

template <> inline void transpose_vec_mat<double>(double *in_mat, const int Nrows, const int Ncols, const int colStride)
{
   typedef typename c7x::make_full_vector<double>::type vec;

   __vpred pred_col_elems = __mask_long((uint32_t) Ncols);

   vec v1 = __vload_pred(pred_col_elems, (vec *) (in_mat + 0 * colStride));
   vec v2 = __vload_pred(pred_col_elems, (vec *) (in_mat + 1 * colStride));
   vec v3 = __vload_pred(pred_col_elems, (vec *) (in_mat + 2 * colStride));
   vec v4 = __vload_pred(pred_col_elems, (vec *) (in_mat + 3 * colStride));
   vec v5 = __vload_pred(pred_col_elems, (vec *) (in_mat + 4 * colStride));
   vec v6 = __vload_pred(pred_col_elems, (vec *) (in_mat + 5 * colStride));
   vec v7 = (vec) 0.0; // __vload_pred(pred_col_elems, (vec *)(in_mat + 6 * colStride));
   vec v8 = (vec) 0.0; // __vload_pred(pred_col_elems, (vec *)(in_mat + 7 * colStride));

   if (Ncols == 7) {
      v7 = __vload_pred(pred_col_elems, (vec *) (in_mat + 6 * colStride));
   }

   vec v1_s1 =
       c7x::as_double_vec(__permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v2), c7x::as_uchar_vec(v1)));
   vec v2_s1 =
       c7x::as_double_vec(__permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v2), c7x::as_uchar_vec(v1)));
   vec v3_s1 =
       c7x::as_double_vec(__permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v4), c7x::as_uchar_vec(v3)));
   vec v4_s1 =
       c7x::as_double_vec(__permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v4), c7x::as_uchar_vec(v3)));
   vec v5_s1 =
       c7x::as_double_vec(__permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v6), c7x::as_uchar_vec(v5)));
   vec v6_s1 =
       c7x::as_double_vec(__permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v6), c7x::as_uchar_vec(v5)));
   vec v7_s1 =
       c7x::as_double_vec(__permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v8), c7x::as_uchar_vec(v7)));
   vec v8_s1 =
       c7x::as_double_vec(__permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v8), c7x::as_uchar_vec(v7)));

   vec v1_s2 = c7x::as_double_vec(
       __permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v3_s1), c7x::as_uchar_vec(v1_s1)));
   vec v2_s2 = c7x::as_double_vec(
       __permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v4_s1), c7x::as_uchar_vec(v2_s1)));
   vec v3_s2 = c7x::as_double_vec(
       __permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v3_s1), c7x::as_uchar_vec(v1_s1)));
   vec v4_s2 = c7x::as_double_vec(
       __permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v4_s1), c7x::as_uchar_vec(v2_s1)));
   vec v5_s2 = c7x::as_double_vec(
       __permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v7_s1), c7x::as_uchar_vec(v5_s1)));
   vec v6_s2 = c7x::as_double_vec(
       __permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v8_s1), c7x::as_uchar_vec(v6_s1)));
   vec v7_s2 = c7x::as_double_vec(
       __permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v7_s1), c7x::as_uchar_vec(v5_s1)));
   vec v8_s2 = c7x::as_double_vec(
       __permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v8_s1), c7x::as_uchar_vec(v6_s1)));

   vec v1_s3 = c7x::as_double_vec(
       __permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v5_s2), c7x::as_uchar_vec(v1_s2)));
   vec v2_s3 = c7x::as_double_vec(
       __permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v6_s2), c7x::as_uchar_vec(v2_s2)));
   vec v3_s3 = c7x::as_double_vec(
       __permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v7_s2), c7x::as_uchar_vec(v3_s2)));
   vec v4_s3 = c7x::as_double_vec(
       __permute_even_even_long(vperm_linear_mask, c7x::as_uchar_vec(v8_s2), c7x::as_uchar_vec(v4_s2)));
   vec v5_s3 = c7x::as_double_vec(
       __permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v5_s2), c7x::as_uchar_vec(v1_s2)));
   vec v6_s3 = c7x::as_double_vec(
       __permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v6_s2), c7x::as_uchar_vec(v2_s2)));
   vec v7_s3 = c7x::as_double_vec(
       __permute_odd_odd_long(vperm_linear_mask, c7x::as_uchar_vec(v7_s2), c7x::as_uchar_vec(v3_s2)));
   //  vec v8_s3 = c7x::as_double_vec(

   __vstore_pred(pred_col_elems, (vec *) (in_mat + 0 * colStride), v1_s3);
   __vstore_pred(pred_col_elems, (vec *) (in_mat + 1 * colStride), v2_s3);
   __vstore_pred(pred_col_elems, (vec *) (in_mat + 2 * colStride), v3_s3);
   __vstore_pred(pred_col_elems, (vec *) (in_mat + 3 * colStride), v4_s3);
   __vstore_pred(pred_col_elems, (vec *) (in_mat + 4 * colStride), v5_s3);
   __vstore_pred(pred_col_elems, (vec *) (in_mat + 5 * colStride), v6_s3);

   if (Ncols == 7) {
      __vstore_pred(pred_col_elems, (vec *) (in_mat + 6 * colStride), v7_s3);
   }

   return;
}

#endif /* #if (__C7X_VEC_SIZE_BITS__ == 512) */
