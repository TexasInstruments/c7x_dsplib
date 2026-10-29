# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0


import numpy as np


def norm32(a):
    """Redundant sign bits of a non-negative 32-bit value (C7x __norm)."""
    if a == 0:
        return 31
    return 31 - a.bit_length()


def sub_cond(b, a):
    """C7x __sub_cond on unsigned 32-bit operands."""
    if b >= a:
        return (((b - a) << 1) + 1) & 0xFFFFFFFF
    return (b << 1) & 0xFFFFFFFF


def recip_q15(x):
    """Fixed point reciprocal of a Q15 value, returns (exponent, fraction)."""
    a = int(x)
    neg = a < 0
    if neg:
        a = -a

    normal = norm32(a)
    a = (a << normal) & 0xFFFFFFFF
    exp = normal - 15

    b = 0x80000000
    for _ in range(15):
        b = sub_cond(b, a)

    frac = b & 0x7FFF
    if neg:
        frac = -frac

    return exp, frac


def recipProcess(src, storeStyle):
    """Compute the expected output matrix for a dimY x dimX input.

    float/double : dst[y][x] = 1 / src[y][x]
    int16_t      : each output row holds 2 * dimX values
                   DSPLIB_HSTACK_ST     -> | exp0 .. expN-1 | frac0 .. fracN-1 |
                   DSPLIB_INTERLEAVE_ST -> | exp0 | frac0 | exp1 | frac1 | ..  |
    """
    if np.issubdtype(src.dtype, np.floating):
        one = np.array(1, dtype=src.dtype)
        return (one / src).astype(src.dtype)

    dimY, dimX = src.shape
    dst = np.zeros((dimY, 2 * dimX), dtype="int16")
    for y in range(dimY):
        for x in range(dimX):
            exp, frac = recip_q15(src[y, x])
            if storeStyle == "DSPLIB_HSTACK_ST":
                dst[y, x] = exp
                dst[y, dimX + x] = frac
            else:
                dst[y, 2 * x] = exp
                dst[y, 2 * x + 1] = frac
    return dst
