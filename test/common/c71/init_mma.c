// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

//*****************************************************************************
//  init_mma.c
//
//  Description
//      Initialize the MMA prior to processing
//
//*****************************************************************************

#include "init_mma.h"

#ifdef WIN32
#define ASSIGN(param, value) value
#else
#define ASSIGN(param, value) .param = value
#endif

#if defined(__C7100__) || defined(__C7120__)
const __HWA_OFFSET_REG zerosOffsetRegStruct = {
    ASSIGN(offset0, 0),  ASSIGN(offset1, 0),  ASSIGN(offset2, 0),  ASSIGN(offset3, 0),  ASSIGN(A_LUT_VAL_0, 0),
    ASSIGN(offset4, 0),  ASSIGN(offset5, 0),  ASSIGN(offset6, 0),  ASSIGN(offset7, 0),  ASSIGN(A_LUT_VAL_1, 0),
    ASSIGN(offset8, 0),  ASSIGN(offset9, 0),  ASSIGN(offset10, 0), ASSIGN(offset11, 0), ASSIGN(A_LUT_VAL_2, 0),
    ASSIGN(offset12, 0), ASSIGN(offset13, 0), ASSIGN(offset14, 0), ASSIGN(offset15, 0), ASSIGN(A_LUT_VAL_3, 0),
    ASSIGN(offset16, 0), ASSIGN(offset17, 0), ASSIGN(offset18, 0), ASSIGN(offset19, 0), ASSIGN(A_LUT_VAL_4, 0),
    ASSIGN(offset20, 0), ASSIGN(offset21, 0), ASSIGN(offset22, 0), ASSIGN(offset23, 0), ASSIGN(A_LUT_VAL_5, 0),
    ASSIGN(offset24, 0), ASSIGN(offset25, 0), ASSIGN(offset26, 0), ASSIGN(offset27, 0), ASSIGN(A_LUT_VAL_6, 0),
    ASSIGN(offset28, 0), ASSIGN(offset29, 0), ASSIGN(offset30, 0), ASSIGN(offset31, 0), ASSIGN(A_LUT_VAL_7, 0),
    ASSIGN(offset32, 0), ASSIGN(offset33, 0), ASSIGN(offset34, 0), ASSIGN(offset35, 0), ASSIGN(A_LUT_VAL_8, 0),
    ASSIGN(offset36, 0), ASSIGN(offset37, 0), ASSIGN(offset38, 0), ASSIGN(offset39, 0), ASSIGN(A_LUT_VAL_9, 0),
    ASSIGN(offset40, 0), ASSIGN(offset41, 0), ASSIGN(offset42, 0), ASSIGN(offset43, 0), ASSIGN(A_LUT_VAL_10, 0),
    ASSIGN(offset44, 0), ASSIGN(offset45, 0), ASSIGN(offset46, 0), ASSIGN(offset47, 0), ASSIGN(A_LUT_VAL_11, 0),
    ASSIGN(offset48, 0), ASSIGN(offset49, 0), ASSIGN(offset50, 0), ASSIGN(offset51, 0), ASSIGN(A_LUT_VAL_12, 0),
    ASSIGN(offset52, 0), ASSIGN(offset53, 0), ASSIGN(offset54, 0), ASSIGN(offset55, 0), ASSIGN(A_LUT_VAL_13, 0),
    ASSIGN(offset56, 0), ASSIGN(offset57, 0), ASSIGN(offset58, 0), ASSIGN(offset59, 0), ASSIGN(A_LUT_VAL_14, 0),
    ASSIGN(offset60, 0), ASSIGN(offset61, 0), ASSIGN(offset62, 0), ASSIGN(offset63, 0), ASSIGN(A_LUT_VAL_15, 0)};

#elif defined(__C7504__) || defined(__C7524__)

const __HWA_OFFSET_REG zerosOffsetRegStruct = {
    ASSIGN(offset0, 0),  ASSIGN(offset1, 0),  ASSIGN(offset2, 0),  ASSIGN(offset3, 0),  ASSIGN(A_LUT_VAL_0, 0),
    ASSIGN(offset4, 0),  ASSIGN(offset5, 0),  ASSIGN(offset6, 0),  ASSIGN(offset7, 0),  ASSIGN(A_LUT_VAL_1, 0),
    ASSIGN(offset8, 0),  ASSIGN(offset9, 0),  ASSIGN(offset10, 0), ASSIGN(offset11, 0), ASSIGN(A_LUT_VAL_2, 0),
    ASSIGN(offset12, 0), ASSIGN(offset13, 0), ASSIGN(offset14, 0), ASSIGN(offset15, 0), ASSIGN(A_LUT_VAL_3, 0),
    ASSIGN(offset16, 0), ASSIGN(offset17, 0), ASSIGN(offset18, 0), ASSIGN(offset19, 0), ASSIGN(A_LUT_VAL_4, 0),
    ASSIGN(offset20, 0), ASSIGN(offset21, 0), ASSIGN(offset22, 0), ASSIGN(offset23, 0), ASSIGN(A_LUT_VAL_5, 0),
    ASSIGN(offset24, 0), ASSIGN(offset25, 0), ASSIGN(offset26, 0), ASSIGN(offset27, 0), ASSIGN(A_LUT_VAL_6, 0),
    ASSIGN(offset28, 0), ASSIGN(offset29, 0), ASSIGN(offset30, 0), ASSIGN(offset31, 0), ASSIGN(A_LUT_VAL_7, 0),
};
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void init_mma()
{
#if !defined(_HOST_BUILD)
#if defined(EVM_TEST)
   // Is this still needed here, or does it get handled elsewhere in the system
   __HWA_CONFIG_REG_v1 mmaConfig = __gen_HWA_CONFIG_REG_v1();
   __HWA_OFFSET_REG    mmaOffset = zerosOffsetRegStruct;
   __HWAOPEN(mmaConfig, mmaOffset, __MMA_OPEN_FSM_RESET);
   __HWACLOSE(0);
#endif

#endif
}

#ifdef __cplusplus
}
#endif // extern "C"
