// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef C71_DSPLIB_INLINES_H
#define C71_DSPLIB_INLINES_H

#include "../DSPLIB_types.h"
#if defined(DSPLIB_DEBUGPRINT)
#include "DSPLIB_debug.h"
#endif
#include <float.h>  // for max float, double values
#include <limits.h> // for min, max integer values

/*******************************************************************************
 *
 * Inlined functions
 *
 ******************************************************************************/

/******************************************************************************/
#include "c7x_inlines.h"

#include "DSPLIB_UTIL_SE0AloadSE1BloadComputeC.h"
#include "DSPLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore.h"
#include "DSPLIB_UTIL_SE1Bload.h"
#include "DSPLIB_UTIL_SE1BloadSA0Cstore.h"

#include "DSPLIB_UTIL_Q_SE0AloadSE1BloadComputeC.h"
#include "DSPLIB_UTIL_Q_SE0AloadSE1BloadComputeCSA0CstoreUnroll3.h"
#include "DSPLIB_UTIL_Q_SE1Bload.h"

#endif
