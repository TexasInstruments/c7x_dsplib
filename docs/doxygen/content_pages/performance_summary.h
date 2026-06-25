// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#if defined(__C7100__)
/**
 \page performance_summary Performance Summary

Performance data was obtained on the J721E EVM. EVM warm cycle obtained by
profiling the kernel's compute code execution after a cold run of the same code.
Please refer to the kernel's documentation for more information about the parameters
in the tables shown below.

<BR><HR>

\section DSPLIB DSPLIB kernels
This section contains tables that depict the expected performance numbers for the DSPLIB kernels.

\subsection DSPLIB_grouped DSPLIB Vector Operations
\htmlinclude ./html/DSPLIB_performance_c7100_migrated.html

\subsection DSPLIB_fir DSPLIB_fir
\htmlinclude ./html/dsplib_fir_performance_c7100.html

\subsection DSPLIB_cascadeBiquad DSPLIB_cascadeBiquad
\htmlinclude ./html/dsplib_cascadeBiquad_performance_c7100.html

\subsection DSPLIB_w_vec DSPLIB_w_vec
\htmlinclude ./html/DSPLIB_performance_c7100_w_vec.html

\subsection DSPLIB_bexp DSPLIB_bexp
\htmlinclude ./html/DSPLIB_bexp_performance_c7100.html

\subsection DSPLIB_blk_eswap DSPLIB_blk_eswap
\htmlinclude ./html/DSPLIB_blk_eswap_performance_c7100.html

\subsection DSPLIB_blk_move DSPLIB_blk_move
\htmlinclude ./html/DSPLIB_blk_move_performance_c7100.html

\subsection DSPLIB_fltoq15 DSPLIB_fltoq15
\htmlinclude ./html/DSPLIB_fltoq15_performance_c7100.html

\subsection DSPLIB_q15tofl DSPLIB_q15tofl
\htmlinclude ./html/DSPLIB_q15tofl_performance_c7100.html

\subsection DSPLIB_minerror DSPLIB_minerror
\htmlinclude ./html/DSPLIB_minerror_performance_c7100.html

\subsection DSPLIB_matMul DSPLIB_matMul
C  = AxB, where C, A, and B are M×N, M×K, and K×N matrices, respectively

\htmlinclude ./html/matmul_C7100.html

\subsection DSPLIB_matMul_fixed DSPLIB_matMul_fixed
C  = AxB, where C, A, and B are M×N, M×K, and K×N matrices, respectively
\htmlinclude ./html/DSPLIB_matMul_fixed_performance_c7100.html

\subsection DSPLIB_matTrans DSPLIB_matTrans
\htmlinclude ./html/DSPLIB_matTrans_performance_c7100.html

\subsection DSPLIB_mat_submat_copy DSPLIB_mat_submat_copy
If dir = 0, copies from X(M,N) to Y(nRows,nCols) starting from the index (stRow,stCol).
If dir = 1, copies from Y(nRows,nCols) to X(M,N) into the index (stRow,stCol).
\htmlinclude ./html/DSPLIB_mat_submat_copy_performance_c7100.html

\subsection DSPLIB_cholesky DSPLIB_cholesky
\htmlinclude ./html/DSPLIB_cholesky_performance_c7100.html

\subsection DSPLIB_cholesky_inplace DSPLIB_cholesky_inplace
\htmlinclude ./html/DSPLIB_cholesky_inplace_performance_c7100.html

\subsection DSPLIB_cholesky_solver DSPLIB_cholesky_solver
\htmlinclude ./html/DSPLIB_cholesky_solver_performance_c7100.html

\subsection DSPLIB_svd DSPLIB_svd
\htmlinclude ./html/DSPLIB_svd_performance_c7100.html

\subsection DSPLIB_qrd DSPLIB_qrd
\htmlinclude ./html/DSPLIB_qrd_performance_c7100.html

\subsection DSPLIB_qrd_solver DSPLIB_qrd_solver
\htmlinclude ./html/DSPLIB_qrd_solver_performance_c7100.html

\subsection DSPLIB_qrd_inverse DSPLIB_qrd_inverse
\htmlinclude ./html/DSPLIB_qrd_inverse_c7100.html

\subsection DSPLIB_lud DSPLIB_lud
\htmlinclude ./html/DSPLIB_lud_performance_c7100.html

\subsection DSPLIB_lud_solver DSPLIB_lud_solver
\htmlinclude ./html/DSPLIB_lud_solver_performance_c7100.html

\subsection DSPLIB_lud_inverse DSPLIB_lud_inverse
\htmlinclude ./html/DSPLIB_lud_inverse_performance_c7100.html

\subsection DSPLIB_dotp_sqr DSPLIB_dotp_sqr
\htmlinclude ./html/DSPLIB_performance_c7100_dotp_sqr.html

\subsection DSPLIB_dotprod DSPLIB_dotprod
\htmlinclude ./html/DSPLIB_performance_c7100_dotprod.html

\subsection DSPLIB_negate DSPLIB_negate
\htmlinclude ./html/DSPLIB_performance_c7100_negate.html

\subsection DSPLIB_recip DSPLIB_recip
\htmlinclude ./html/DSPLIB_performance_c7100_recip.html

\subsection DSPLIB_sqrAdd DSPLIB_sqrAdd
\htmlinclude ./html/DSPLIB_performance_c7100_sqrAdd.html

\subsection DSPLIB_svd_small DSPLIB_svd_small
\htmlinclude ./html/DSPLIB_svd_small_performance_c7100.html

<BR>
*/

#elif defined(__C7120__)

/**
 \page performance_summary Performance Summary

Performance data was obtained on the J721S2 EVM. EVM warm cycle obtained by
profiling the kernel's compute code execution after a cold run of the same code.
Please refer to the kernel's documentation for more information about the parameters
in the tables shown below.

<BR><HR>

\section DSPLIB DSPLIB kernels
This section contains tables that depict the expected performance numbers for the DSPLIB kernels.

\subsection DSPLIB_grouped DSPLIB Vector Operations
\htmlinclude ./html/DSPLIB_performance_c7120_migrated.html

\subsection DSPLIB_fir DSPLIB_fir
\htmlinclude ./html/dsplib_fir_performance_c7120.html

\subsection DSPLIB_cascadeBiquad DSPLIB_cascadeBiquad
\htmlinclude ./html/dsplib_cascadeBiquad_performance_c7120.html

\subsection DSPLIB_w_vec DSPLIB_w_vec
\htmlinclude ./html/DSPLIB_performance_c7120_w_vec.html

\subsection DSPLIB_bexp DSPLIB_bexp
\htmlinclude ./html/DSPLIB_bexp_performance_c7120.html

\subsection DSPLIB_blk_eswap DSPLIB_blk_eswap
\htmlinclude ./html/DSPLIB_blk_eswap_performance_c7120.html

\subsection DSPLIB_blk_move DSPLIB_blk_move
\htmlinclude ./html/DSPLIB_blk_move_performance_c7120.html

\subsection DSPLIB_fltoq15 DSPLIB_fltoq15
\htmlinclude ./html/DSPLIB_fltoq15_performance_c7120.html

\subsection DSPLIB_q15tofl DSPLIB_q15tofl
\htmlinclude ./html/DSPLIB_q15tofl_performance_c7120.html

\subsection DSPLIB_minerror DSPLIB_minerror
\htmlinclude ./html/DSPLIB_minerror_performance_c7120.html

\subsection DSPLIB_matMul DSPLIB_matMul
C  = AxB, where C, A, and B are M×N, M×K, and K×N matrices, respectively

\htmlinclude ./html/matMul_C7120.html

\subsection DSPLIB_matMul_fixed DSPLIB_matMul_fixed
C  = AxB, where C, A, and B are M×N, M×K, and K×N matrices, respectively
\htmlinclude ./html/DSPLIB_matMul_fixed_performance_c7120.html

\subsection DSPLIB_matTrans DSPLIB_matTrans
\htmlinclude ./html/DSPLIB_matTrans_performance_c7120.html

\subsection DSPLIB_mat_submat_copy DSPLIB_mat_submat_copy
If dir = 0, copies from X(M,N) to Y(nRows,nCols) starting from the index (stRow,stCol).
If dir = 1, copies from Y(nRows,nCols) to X(M,N) into the index (stRow,stCol).
\htmlinclude ./html/DSPLIB_mat_submat_copy_performance_c7120.html

\subsection DSPLIB_cholesky DSPLIB_cholesky
\htmlinclude ./html/DSPLIB_cholesky_performance_c7120.html

\subsection DSPLIB_cholesky_inplace DSPLIB_cholesky_inplace
\htmlinclude ./html/DSPLIB_cholesky_inplace_performance_c7120.html

\subsection DSPLIB_cholesky_solver DSPLIB_cholesky_solver
\htmlinclude ./html/DSPLIB_cholesky_solver_performance_c7120.html

\subsection DSPLIB_svd DSPLIB_svd
\htmlinclude ./html/DSPLIB_svd_performance_c7120.html

\subsection DSPLIB_qrd DSPLIB_qrd
\htmlinclude ./html/DSPLIB_qrd_performance_c7120.html

\subsection DSPLIB_qrd_solver DSPLIB_qrd_solver
\htmlinclude ./html/DSPLIB_qrd_solver_performance_c7120.html

\subsection DSPLIB_qrd_inverse DSPLIB_qrd_inverse
\htmlinclude ./html/DSPLIB_qrd_inverse_c7120.html

\subsection DSPLIB_lud DSPLIB_lud
\htmlinclude ./html/DSPLIB_lud_performance_c7120.html

\subsection DSPLIB_lud_solver DSPLIB_lud_solver
\htmlinclude ./html/DSPLIB_lud_solver_performance_c7120.html

\subsection DSPLIB_lud_inverse DSPLIB_lud_inverse
\htmlinclude ./html/DSPLIB_lud_inverse_performance_c7120.html

\subsection DSPLIB_dotp_sqr DSPLIB_dotp_sqr
\htmlinclude ./html/DSPLIB_performance_c7120_dotp_sqr.html

\subsection DSPLIB_dotprod DSPLIB_dotprod
\htmlinclude ./html/DSPLIB_performance_c7120_dotprod.html

\subsection DSPLIB_negate DSPLIB_negate
\htmlinclude ./html/DSPLIB_performance_c7120_negate.html

\subsection DSPLIB_recip DSPLIB_recip
\htmlinclude ./html/DSPLIB_performance_c7120_recip.html

\subsection DSPLIB_sqrAdd DSPLIB_sqrAdd
\htmlinclude ./html/DSPLIB_performance_c7120_sqrAdd.html

\subsection DSPLIB_svd_small DSPLIB_svd_small
\htmlinclude ./html/DSPLIB_svd_small_performance_c7120.html

<BR>

*/

#elif defined(__C7504__)

/**
 \page performance_summary Performance Summary

Performance data was obtained on the AM62A EVM. EVM warm cycle obtained by
profiling the kernel's compute code execution after a cold run of the same code.
Please refer to the kernel's documentation for more information about the parameters
in the tables shown below.

<BR><HR>

\section DSPLIB DSPLIB kernels
This section contains tables that depict the expected performance numbers for the DSPLIB kernels.

\subsection DSPLIB_grouped DSPLIB Vector Operations
\htmlinclude ./html/DSPLIB_performance_c7504_migrated.html

\subsection DSPLIB_fir DSPLIB_fir
\htmlinclude ./html/DSPLIB_fir_performance_c7504.html

\subsection DSPLIB_cascadeBiquad DSPLIB_cascadeBiquad
\f$ \text{Cycles/Biquad} = \frac{\text{EVM Warm Cycles}}{\text{dataSize} \times \text{numChannels} \times
\text{numStages}} \f$ \htmlinclude ./html/DSPLIB_cascadeBiquad_performance_c7504.html

\subsection DSPLIB_w_vec DSPLIB_w_vec
\htmlinclude ./html/DSPLIB_performance_c7504_w_vec.html

\subsection DSPLIB_bexp DSPLIB_bexp
\htmlinclude ./html/DSPLIB_bexp_performance_c7504.html

\subsection DSPLIB_blk_eswap DSPLIB_blk_eswap
\htmlinclude ./html/DSPLIB_blk_eswap_performance_c7504.html

\subsection DSPLIB_blk_move DSPLIB_blk_move
\htmlinclude ./html/DSPLIB_blk_move_performance_c7504.html

\subsection DSPLIB_fltoq15 DSPLIB_fltoq15
\htmlinclude ./html/DSPLIB_fltoq15_performance_c7504.html

\subsection DSPLIB_q15tofl DSPLIB_q15tofl
\htmlinclude ./html/DSPLIB_q15tofl_performance_c7504.html

\subsection DSPLIB_minerror DSPLIB_minerror
\htmlinclude ./html/DSPLIB_minerror_performance_c7504.html

\subsection DSPLIB_matMul DSPLIB_matMul
C  = AxB, where C, A, and B are M×N, M×K, and K×N matrices, respectively

\htmlinclude ./html/matMul_C7504.html

\subsection DSPLIB_matMul_fixed DSPLIB_matMul_fixed
C  = AxB, where C, A, and B are M×N, M×K, and K×N matrices, respectively
\htmlinclude ./html/DSPLIB_matMul_fixed_performance_c7504.html

\subsection DSPLIB_matTrans DSPLIB_matTrans
\htmlinclude ./html/DSPLIB_matTrans_performance_c7504.html

\subsection DSPLIB_mat_submat_copy DSPLIB_mat_submat_copy
If dir = 0, copies from X(M,N) to Y(nRows,nCols) starting from the index (stRow,stCol).
If dir = 1, copies from Y(nRows,nCols) to X(M,N) into the index (stRow,stCol).
\htmlinclude ./html/DSPLIB_mat_submat_copy_performance_c7504.html

\subsection DSPLIB_cholesky DSPLIB_cholesky
\htmlinclude ./html/DSPLIB_cholesky_performance_c7504.html

\subsection DSPLIB_cholesky_inplace DSPLIB_cholesky_inplace
\htmlinclude ./html/DSPLIB_cholesky_inplace_performance_c7504.html

\subsection DSPLIB_cholesky_solver DSPLIB_cholesky_solver
\htmlinclude ./html/DSPLIB_cholesky_solver_performance_c7504.html

\subsection DSPLIB_svd DSPLIB_svd
\htmlinclude ./html/DSPLIB_svd_performance_c7504.html

\subsection DSPLIB_qrd DSPLIB_qrd
\htmlinclude ./html/DSPLIB_qrd_performance_c7504.html

\subsection DSPLIB_qrd_solver DSPLIB_qrd_solver
\htmlinclude ./html/DSPLIB_qrd_solver_performance_c7504.html

\subsection DSPLIB_qrd_inverse DSPLIB_qrd_inverse
\htmlinclude ./html/DSPLIB_qrd_inverse_performance_c7504.html

\subsection DSPLIB_lud DSPLIB_lud
\htmlinclude ./html/DSPLIB_lud_performance_c7504.html

\subsection DSPLIB_lud_solver DSPLIB_lud_solver
\htmlinclude ./html/DSPLIB_lud_solver_performance_c7504.html

\subsection DSPLIB_lud_inverse DSPLIB_lud_inverse
\htmlinclude ./html/DSPLIB_lud_inverse_performance_c7504.html

\subsection DSPLIB_dotp_sqr DSPLIB_dotp_sqr
\htmlinclude ./html/DSPLIB_performance_c7504_dotp_sqr.html

\subsection DSPLIB_dotprod DSPLIB_dotprod
\htmlinclude ./html/DSPLIB_performance_c7504_dotprod.html

\subsection DSPLIB_negate DSPLIB_negate
\htmlinclude ./html/DSPLIB_performance_c7504_negate.html

\subsection DSPLIB_recip DSPLIB_recip
\htmlinclude ./html/DSPLIB_performance_c7504_recip.html

\subsection DSPLIB_sqrAdd DSPLIB_sqrAdd
\htmlinclude ./html/DSPLIB_performance_c7504_sqrAdd.html

\subsection DSPLIB_addNCh DSPLIB_addNCh
\htmlinclude ./html/DSPLIB_addNCh_performance_c7504.html

\subsection DSPLIB_blkCopyConst2D DSPLIB_blkCopyConst2D
\htmlinclude ./html/DSPLIB_blkCopyConst2D_performance_c7504.html

\subsection DSPLIB_deinterleave DSPLIB_deinterleave
\htmlinclude ./html/DSPLIB_deinterleave_performance_c7504.html

\subsection DSPLIB_interleave DSPLIB_interleave
\htmlinclude ./html/DSPLIB_interleave_performance_c7504.html

\subsection DSPLIB_realImagToComplex DSPLIB_realImagToComplex
\htmlinclude ./html/DSPLIB_realImagToComplex_performance_c7504.html

<BR>

*/

#elif defined(__C7524__)

/**
 \page performance_summary Performance Summary

Performance data was obtained on the J722S EVM. EVM warm cycle obtained by
profiling the kernel's compute code execution after a cold run of the same code.
Please refer to the kernel's documentation for more information about the parameters
in the tables shown below.

<BR><HR>

\section DSPLIB DSPLIB kernels
This section contains tables that depict the expected performance numbers for the DSPLIB kernels.

\subsection DSPLIB_grouped DSPLIB Vector Operations
\htmlinclude ./html/DSPLIB_performance_c7524_migrated.html

\subsection DSPLIB_fir DSPLIB_fir
\htmlinclude ./html/DSPLIB_fir_performance_c7524.html

\subsection DSPLIB_cascadeBiquad DSPLIB_cascadeBiquad
\f$ \text{Cycles/Biquad} = \frac{\text{EVM Warm Cycles}}{\text{dataSize} \times \text{numChannels} \times
\text{numStages}} \f$ \htmlinclude ./html/DSPLIB_cascadeBiquad_performance_c7524.html

\subsection DSPLIB_w_vec DSPLIB_w_vec
\htmlinclude ./html/DSPLIB_performance_c7524_w_vec.html

\subsection DSPLIB_bexp DSPLIB_bexp
\htmlinclude ./html/DSPLIB_bexp_performance_c7524.html

\subsection DSPLIB_blk_eswap DSPLIB_blk_eswap
\htmlinclude ./html/DSPLIB_blk_eswap_performance_c7524.html

\subsection DSPLIB_blk_move DSPLIB_blk_move
\htmlinclude ./html/DSPLIB_blk_move_performance_c7524.html

\subsection DSPLIB_fltoq15 DSPLIB_fltoq15
\htmlinclude ./html/DSPLIB_fltoq15_performance_c7524.html

\subsection DSPLIB_q15tofl DSPLIB_q15tofl
\htmlinclude ./html/DSPLIB_q15tofl_performance_c7524.html

\subsection DSPLIB_minerror DSPLIB_minerror
\htmlinclude ./html/DSPLIB_minerror_performance_c7524.html

\subsection DSPLIB_matMul DSPLIB_matMul
C  = AxB, where C, A, and B are M×N, M×K, and K×N matrices, respectively

\htmlinclude ./html/matMul_C7524.html

\subsection DSPLIB_matMul_fixed DSPLIB_matMul_fixed
C  = AxB, where C, A, and B are M×N, M×K, and K×N matrices, respectively
\htmlinclude ./html/DSPLIB_matMul_fixed_performance_c7524.html

\subsection DSPLIB_matTrans DSPLIB_matTrans
\htmlinclude ./html/DSPLIB_matTrans_performance_c7524.html

\subsection DSPLIB_mat_submat_copy DSPLIB_mat_submat_copy
If dir = 0, copies from X(M,N) to Y(nRows,nCols) starting from the index (stRow,stCol).
If dir = 1, copies from Y(nRows,nCols) to X(M,N) into the index (stRow,stCol).
\htmlinclude ./html/DSPLIB_mat_submat_copy_performance_c7524.html

\subsection DSPLIB_cholesky DSPLIB_cholesky
\htmlinclude ./html/DSPLIB_cholesky_performance_c7524.html

\subsection DSPLIB_cholesky_inplace DSPLIB_cholesky_inplace
\htmlinclude ./html/DSPLIB_cholesky_inplace_performance_c7524.html

\subsection DSPLIB_cholesky_solver DSPLIB_cholesky_solver
\htmlinclude ./html/DSPLIB_cholesky_solver_performance_c7524.html

\subsection DSPLIB_svd DSPLIB_svd
\htmlinclude ./html/DSPLIB_svd_performance_c7524.html

\subsection DSPLIB_qrd DSPLIB_qrd
\htmlinclude ./html/DSPLIB_qrd_performance_c7524.html

\subsection DSPLIB_qrd_solver DSPLIB_qrd_solver
\htmlinclude ./html/DSPLIB_qrd_solver_performance_c7524.html

\subsection DSPLIB_qrd_inverse DSPLIB_qrd_inverse
\htmlinclude ./html/DSPLIB_qrd_inverse_performance_c7524.html

\subsection DSPLIB_lud DSPLIB_lud
\htmlinclude ./html/DSPLIB_lud_performance_c7524.html

\subsection DSPLIB_lud_solver DSPLIB_lud_solver
\htmlinclude ./html/DSPLIB_lud_solver_performance_c7524.html

\subsection DSPLIB_lud_inverse DSPLIB_lud_inverse
\htmlinclude ./html/DSPLIB_lud_inverse_performance_c7524.html

\subsection DSPLIB_dotp_sqr DSPLIB_dotp_sqr
\htmlinclude ./html/DSPLIB_performance_c7524_dotp_sqr.html

\subsection DSPLIB_dotprod DSPLIB_dotprod
\htmlinclude ./html/DSPLIB_performance_c7524_dotprod.html

\subsection DSPLIB_negate DSPLIB_negate
\htmlinclude ./html/DSPLIB_performance_c7524_negate.html

\subsection DSPLIB_recip DSPLIB_recip
\htmlinclude ./html/DSPLIB_performance_c7524_recip.html

\subsection DSPLIB_sqrAdd DSPLIB_sqrAdd
\htmlinclude ./html/DSPLIB_performance_c7524_sqrAdd.html

\subsection DSPLIB_addNCh DSPLIB_addNCh
\htmlinclude ./html/DSPLIB_addNCh_performance_c7524.html

\subsection DSPLIB_blkCopyConst2D DSPLIB_blkCopyConst2D
\htmlinclude ./html/DSPLIB_blkCopyConst2D_performance_c7524.html

\subsection DSPLIB_deinterleave DSPLIB_deinterleave
\htmlinclude ./html/DSPLIB_deinterleave_performance_c7524.html

\subsection DSPLIB_interleave DSPLIB_interleave
\htmlinclude ./html/DSPLIB_interleave_performance_c7524.html

\subsection DSPLIB_realImagToComplex DSPLIB_realImagToComplex
\htmlinclude ./html/DSPLIB_realImagToComplex_performance_c7524.html

<BR>

*/

#endif

// clang-format on
