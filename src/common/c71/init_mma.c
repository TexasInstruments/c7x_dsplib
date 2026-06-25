// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "init_mma.h"
#include "../DSPLIB_types.h"

/* -------------------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not being         */
/*  addressed so as not  to lose portability across different platforms.      */
/* -------------------------------------------------------------------------- */
#ifdef WIN32
#define ASSIGN(param, value) value
#else
#define ASSIGN(param, value) .param = value
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void init_mma(void)
{
#if !defined(_HOST_BUILD)
   __HWA_CONFIG_REG_v1 mmaConfig = __gen_HWA_CONFIG_REG_v1();
   __HWA_OFFSET_REG    mmaOffset = offsetRegStruct_zeros;
   __HWAOPEN(mmaConfig, mmaOffset, __MMA_OPEN_FSM_RESET);
   __HWACLOSE(0);
#endif
}

#ifdef __cplusplus
}
#endif // extern "C"
