// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "enable_cache_mmu.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void enable_cache_mmu(uint64_t ttbr)
{
#if !defined(_HOST_BUILD)
   uint64_t tcr;
   uint64_t mar;
   uint64_t scr;

   tcr = 0x0000000000002a21U;
   scr = 0x80000000000000c1U;
   mar = 0x3D3D3D2915032A00U;

   DSPLIB_TEST_c7xSetTCR0(tcr);  // ECR785
   DSPLIB_TEST_c7xSetTBR0(ttbr); // ECR787
   DSPLIB_TEST_c7xSetMAR(mar);   // ECR789
   DSPLIB_TEST_c7xSetSCR(scr);   // ECR784  *** This register has to be last ***
#endif
}

#ifdef __cplusplus
}
#endif // extern "C"
