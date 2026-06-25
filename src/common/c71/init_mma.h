// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef INIT_MMA_H_
#define INIT_MMA_H_

#include <c7x.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * JIRA: Document Update / DOCU-113
 * After power-on, a short instruction sequence running on the C7x can initi * alize the internal MMA state before the
 first execution of normal MMA ope * ration. Only one execution of the sequence is required.

 * The sequence generates a valid HWA_CONFIG and HWA_OFFSET value, loads it  * into the MMA, then clears the sticky
 error codes.

 */

void init_mma(void);
#ifdef __cplusplus
}
#endif // extern "C"

#endif
