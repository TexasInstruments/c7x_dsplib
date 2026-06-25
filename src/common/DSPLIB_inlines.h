// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_MMA_INLINES_H
#define COMMON_MMA_INLINES_H

#include "DSPLIB_types.h"
#include <float.h>  // for max float, double values
#include <limits.h> // for min, max integer values

#if defined(__C7100__)
#include "c7100/DSPLIB_inlines.h"
#endif

#if defined(__C7120__)
#include "c7120/DSPLIB_inlines.h"
#endif

#if defined(__C7502__)
#include "c7502/DSPLIB_inlines.h"
#endif

#if defined(__C7504__)
#include "c7504/DSPLIB_inlines.h"
#endif

#if defined(__C7508__)
#include "c7508/DSPLIB_inlines.h"
#endif

#if defined(__C7524__)
#include "c7524/DSPLIB_inlines.h"
#endif

/******************************************************************************/

#endif
