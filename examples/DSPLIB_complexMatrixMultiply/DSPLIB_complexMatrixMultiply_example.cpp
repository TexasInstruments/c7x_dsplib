// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

// Include the DSPLIB kernel header
#include "../../src/DSPLIB_complexMatrixMultiply/DSPLIB_complexMatrixMultiply.h"

// Helper to print a complex matrix (real, imag interleaved)
void printComplexMatrix(const char* name, const int16_t* mat, size_t rows, size_t cols) {
    printf("%s = [\n", name);
    for (size_t i = 0; i < rows; ++i) {
        printf("  ");
        for (size_t j = 0; j < cols; ++j) {
            size_t idx = i * (cols * 2) + j * 2;
            int16_t real = mat[idx];
            int16_t imag = mat[idx + 1];
            printf("(%+d%+di) ", real, imag);
        }
        printf("\n");
    }
    printf("]\n");
}

int main() {
    const size_t M = 5, K = 5, N = 5;
    const size_t nBatch = 1;

    int16_t A[M * K * 2];   // input matrix A
    int16_t B[K * N * 2];   // input matrix B
    int16_t C[M * N * 2];   // output matrix C

    // Initialise A as identity (real=1 on diagonal)
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < K; ++j) {
            size_t idx = i * (K * 2) + j * 2;
            A[idx]     = (i == j) ? 1 : 0; // real part
            A[idx + 1] = 0;                  // imag part
        }
    }

    for (size_t i = 0; i < K; ++i) {
        for (size_t j = 0; j < N; ++j) {
            size_t idx = i * (N * 2) + j * 2;
            if (i == j) {
                B[idx] = i + 1;                            // real part
                B[idx + 1] = 0;                            // imag part
            } else {
                B[idx] = 0;
                B[idx + 1] = 0;
            }
        }
    }
    B[2] = 11;
    B[4] = 22;
    B[14] = 44;
    B[26] = 55;

    // Zero the output buffer
    for (size_t i = 0; i < M * N * 2; ++i) {
        C[i] = 0;
    }

    // Buffer parameters
    DSPLIB_data_type_e dataType = DSPLIB_INT16;
    DSPLIB_bufParams3D_t bufParamsInA = { .data_type = dataType, .dim_x = K, .dim_y = M, .stride_y = (int32_t) K * DSPLIB_sizeof(dataType) * 2, .dim_z = nBatch, .stride_z = M * K };
    DSPLIB_bufParams3D_t bufParamsInB = { .data_type = dataType, .dim_x = N, .dim_y = K, .stride_y = (int32_t) N * DSPLIB_sizeof(dataType) * 2, .dim_z = nBatch, .stride_z = K * N };
    DSPLIB_bufParams3D_t bufParamsOut = { .data_type = dataType, .dim_x = N, .dim_y = M, .stride_y = (int32_t) N * DSPLIB_sizeof(dataType) * 2, .dim_z = nBatch, .stride_z = M * N };

    // Init args – only shift/scale needed for this demo
    DSPLIB_complexMatrixMultiply_InitArgs initArgs = { .funcStyle = DSPLIB_FUNCTION_OPTIMIZED, .params = { .shift = 0, .scaleValue = 1}};

    // Allocate the kernel handle based on the size query
    int32_t handleSize = DSPLIB_complexMatrixMultiply_getHandleSize(&initArgs);
    void* handle = malloc(handleSize);
    if (!handle) {
        printf("Failed to allocate handle\n");
        return 1;
    }

    // Initialise the kernel
    DSPLIB_STATUS status = DSPLIB_complexMatrixMultiply_init(handle, &bufParamsInA, &bufParamsInB, &bufParamsOut, &initArgs);
    if (status != DSPLIB_SUCCESS) {
        printf("Init failed %d\n", status);
        free(handle);
        return 1;
    }

    // Execute the multiplication
    status = DSPLIB_complexMatrixMultiply_exec(handle, A, B, C);
    if (status != DSPLIB_SUCCESS) {
        printf("Exec failed %d\n", status);
        free(handle);
        return 1;
    }

    // Print the matrices
    printComplexMatrix("A", A, M, K);
    printComplexMatrix("B", B, K, N);
    printComplexMatrix("C (A*B)", C, M, N);

    free(handle);
    // Expected result is a diagonal matrix [2,0,0; 0,3,0; 0,0,4]
    return 0;
}
