<div align="center">

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://www.ti.com/content/dam/ticom/images/identities/ti-brand/ti-logo-hz-1c-white.svg" width="300">
  <img alt="Texas Instruments Logo" src="https://www.ti.com/content/dam/ticom/images/identities/ti-brand/ti-hz-2c-pos-rgb.svg" width="300">
</picture>

# DSPLIB — C7<sup>™</sup> Digital Signal Processing Library

[Summary](#summary) | [Features](#features) | [Operators](#operators) | [Supported Devices](#supported-devices) | [Setup Instructions](#setup-instructions) | [Build Instructions](#build-instructions) | [Related Repos](#related-repos) | [Licensing](#licensing) | [Contributions](#contributions) | [Developer Resources](#developer-resources)

</div>

## Summary

DSPLIB is TI's optimized digital signal processing library for the **C7<sup>™</sup>+MMA** DSP architecture.
It provides a suite of kernels targeting the C7<sup>™</sup> vector and streaming engines (SE/SA) and the
Matrix Multiply Accelerator (MMA), enabling high-throughput signal processing and linear algebra
on TI SoCs such as AM62D and AM275.

Each kernel ships in two variants selected at init time via the `DSPLIB_FUNCTION_OPT` /
`DSPLIB_FUNCTION_NATC` enum in the kernel's `InitArgs`:

| Suffix | Description |
|--------|-------------|
| `_cn` | C-native reference (`NATC`) — portable, functionally correct, used for validation |
| `_ci` | C7<sup>™</sup> intrinsic (`OPT`) — hardware-optimized, targets SE/SA streaming engines |

Each kernel has a corresponding test driver (`_d.c`). Some kernels additionally include a Python
reference implementation under `test/DSPLIB_idat_gen/DSPLIB_<kernel>/` for test vector generation.


## Features

- **40+ kernels** spanning arithmetic, vector math, matrix operations, linear algebra, and signal processing
- **C7<sup>™</sup> streaming engine optimized** — leverages SE/SA hardware for peak throughput across all kernels
- **MMA acceleration** — `DSPLIB_fir` employs the Matrix Multiply Accelerator; all other kernels use C7<sup>™</sup> vector/streaming engines
- **Portable reference implementations** — `_cn` variants enable host-side validation and porting
- **CMake-based build** with presets for PC simulation and on-target deployment
- **Multi-type support** — `int8_t`, `int16_t`, `int32_t`, `uint8_t`, `uint16_t`, `uint32_t`, `float`, `double`


## Operators

### Arithmetic
| Kernel | Description |
|--------|-------------|
| `DSPLIB_add` | Element-wise addition |
| `DSPLIB_addConstant` | Add scalar constant to each element |
| `DSPLIB_addNCh` | Multi-channel element-wise addition |
| `DSPLIB_sub` | Element-wise subtraction |
| `DSPLIB_subConstant` | Subtract scalar constant from each element |
| `DSPLIB_mul` | Element-wise multiplication |
| `DSPLIB_mulConstant` | Multiply each element by scalar constant |
| `DSPLIB_negate` | Negate each element |
| `DSPLIB_sqr` | Square each element |
| `DSPLIB_sqrAdd` | Square-and-accumulate |
| `DSPLIB_recip` | Element-wise reciprocal |

### Vector
| Kernel | Description |
|--------|-------------|
| `DSPLIB_dotprod` | Dot product |
| `DSPLIB_dotp_sqr` | Dot product of squared elements |
| `DSPLIB_w_vec` | Weighted vector sum |
| `DSPLIB_bexp` | Block exponent (normalize) |

### Comparison & Search
| Kernel | Description |
|--------|-------------|
| `DSPLIB_max` | Maximum value |
| `DSPLIB_maxEvery` | Element-wise maximum of two vectors |
| `DSPLIB_maxIndex` | Index of the maximum value |
| `DSPLIB_min` | Minimum value |
| `DSPLIB_minEvery` | Element-wise minimum of two vectors |
| `DSPLIB_minIndex` | Index of the minimum value |
| `DSPLIB_minerror` | Minimum error search |

### Matrix
| Kernel | Description |
|--------|-------------|
| `DSPLIB_matMul` | Matrix multiply (floating-point) |
| `DSPLIB_matMul_fixed` | Matrix multiply (fixed-point) |
| `DSPLIB_matTrans` | Matrix transpose |
| `DSPLIB_mat_submat_copy` | Sub-matrix copy |
| `DSPLIB_blkCopy2D` | 2-D block copy |
| `DSPLIB_blkCopyConst2D` | 2-D block copy with constant fill |

### Linear Algebra
| Kernel | Description |
|--------|-------------|
| `DSPLIB_cholesky` | Cholesky decomposition |
| `DSPLIB_cholesky_inplace` | In-place Cholesky decomposition |
| `DSPLIB_cholesky_solver` | Solve via Cholesky |
| `DSPLIB_lud` | LU decomposition |
| `DSPLIB_lud_inv` | Matrix inversion via LU |
| `DSPLIB_lud_sol` | Solve via LU |
| `DSPLIB_qrd` | QR decomposition |
| `DSPLIB_qrd_inverse` | Matrix inversion via QR |
| `DSPLIB_qrd_solver` | Solve via QR |
| `DSPLIB_svd` | Singular Value Decomposition |
| `DSPLIB_svd_small` | SVD for small matrices |

### Signal Processing
| Kernel | Description |
|--------|-------------|
| `DSPLIB_fir` | FIR filter |
| `DSPLIB_cascadeBiquad` | Cascade biquad IIR filter (DF1, DF2, TDF2, TIF) |

### Data Conversion & Memory
| Kernel | Description |
|--------|-------------|
| `DSPLIB_fltoq15` | Float to Q15 conversion |
| `DSPLIB_q15tofl` | Q15 to float conversion |
| `DSPLIB_realImagToComplex` | Interleave real/imaginary to complex |
| `DSPLIB_interleave` | Interleave data channels |
| `DSPLIB_deinterleave` | Deinterleave data channels |
| `DSPLIB_blk_move` | Block memory move |
| `DSPLIB_blk_eswap` | Block endian swap |


## Supported Devices

| Device | C7<sup>™</sup> Core | MMA Version |
|--------|----------|-------------|
| AM62D | C7504 | MMAv2 |
| AM275 | C7524 | MMAv2f (adds 32-bit float support) |


## Setup Instructions

### Prerequisites

- **Linux** host (Ubuntu 20.04 or later recommended)
- **CMake** ≥ 3.21
- **TI C7000 Code Generation Tools** (`ti-cgt-c7000`) — install to `~/ti/`

  Download from [TI CGT C7000](https://www.ti.com/tool/C7000-CGT).
  Default expected path: `~/ti/ti-cgt-c7000_5.0.0.LTS/`

  Or set the environment variable before building:
  ```bash
  export CGT7X_ROOT=/path/to/ti-cgt-c7000_<version>
  ```


## Build Instructions

DSPLIB uses **CMake presets** to manage build configurations.

### Available Presets

| Preset | SoC | Platform |
|--------|-----|----------|
| `release-autotest-am62d-pc` | AM62D | PC simulation + tests |
| `release-autotest-am62d-target` | AM62D | On-device + tests |
| `release-autotest-am275-pc` | AM275 | PC simulation + tests |
| `release-autotest-am275-target` | AM275 | On-device + tests |
| `release-buildlib-am62d-pc` | AM62D | PC (library only) |
| `release-buildlib-am62d-target` | AM62D | On-device (library only) |
| `release-buildlib-am275-pc` | AM275 | PC (library only) |
| `release-buildlib-am275-target` | AM275 | On-device (library only) |

### Quick Start — PC simulation (AM62D)

```bash
# Configure
cmake -S . -B build --preset=release-autotest-am62d-pc

# Build
cmake --build build -- -j$(nproc)

# Build test binaries (ctest not supported; run binaries directly on target)
cmake --build build -- -j$(nproc)
```

### On-Target Build (AM62D)

```bash
cmake -S . -B build --preset=release-autotest-am62d-target
cmake --build build -- -j$(nproc)
```

### Library-Only Build

```bash
cmake -S . -B build --preset=release-buildlib-am62d-pc
cmake --build build -- -j$(nproc)
```


## Related Repos

- [MCU+ SDK](https://github.com/TexasInstruments/mcupsdk-core) — SoC drivers and middleware for AM2x/AM6x devices


## Licensing

This repository is licensed under the **Apache License, Version 2.0**.
See [LICENSE](LICENSE) for the full text.

All source files carry an SPDX `Apache-2.0` identifier.


## Contributions

This repository is not currently accepting community contributions.

Bug reports and feature requests are welcome via [TI E2E Community Forums](https://e2e.ti.com).


---

## Developer Resources

[TI E2E™ design support forums](https://e2e.ti.com)
