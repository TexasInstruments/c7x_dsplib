// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "invalidate_tlb.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void invalidate_tlb()
{
#if !defined(_HOST_BUILD)
   DSPLIB_TEST_c7xMmuTlbInvAll();
#endif
}

#ifdef __cplusplus
}
#endif // extern "C"
