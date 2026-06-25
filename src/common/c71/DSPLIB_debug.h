// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef C71_DSPLIB_DEBUG_H_
#define C71_DSPLIB_DEBUG_H_ 1

#include <float.h>  // for max float, double values
#include <limits.h> // for min, max integer values

#include "../DSPLIB_bufParams.h"
#include "../DSPLIB_types.h"

#include "DSPLIB_debugPrintVector.h"

/*******************************************************************************
 *
 * Function prototypes
 *
 ******************************************************************************/

#ifdef __cplusplus

extern "C" {
#endif /* __cplusplus */

void DSPLIB_debugPrintMatrix1D(void *matrix, const DSPLIB_bufParams1D_t *params);
void DSPLIB_debugPrintMatrix(void *matrix, const DSPLIB_bufParams2D_t *params);
void DSPLIB_debugPrintMatrix3D(void *matrix, const DSPLIB_bufParams3D_t *params);

void DSPLIB_debugPrint3DVReg(void *matrix, const DSPLIB_bufParams3D_t *params);
void DSPLIB_debugPrintBufParams1D(const DSPLIB_bufParams1D_t *params);
void DSPLIB_debugPrintBufParams(const DSPLIB_bufParams2D_t *params);
void DSPLIB_debugPrintBufParams3D(const DSPLIB_bufParams3D_t *params);

void DSPLIB_debugPrintMMAReg(const int32_t regNumber);

/******************************************************************************/

//#if defined(_HOST_BUILD)
//   // x86 GCC assembly code for printing the stack pointer
//   // this version defines the function in asm, which avoid all compiler
//   warnings
//   // if this stops working, a simpler version is commented below
//   unsigned long *get_stack_ptr(void);
//   __asm__(
//          "get_stack_ptr:\n"
//          "mov %rsp, %rax\n"
//          "ret\n"
//          );
//
//   static inline unsigned long *get_stack_ptr(void) {
//      __asm__(
//              "mov %rsp, %rax"
//              );
//#define DSPLIB_DEBUGPRINT_STACK_PTR DSPLIB_DEBUGPRINTFN(1, "Stack Pointer:
//%p\n", get_stack_ptr()) #else #define DSPLIB_DEBUGPRINT_STACK_PTR #endif  //
//#if defined(_HOST_BUILD)

//   static inline void DSPLIB_debugPrintStackPtr(void){
//#if defined(_HOST_BUILD)
//      unsigned long *stack_ptr = get_stack_ptr();
//#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

// C++ linkage only
#ifdef __cplusplus

static inline uint8_t DSPLIB_firstElement(c7x::uchar_vec vector) { return *((uint8_t *) (&vector)); }

static inline int8_t DSPLIB_firstElement(c7x::char_vec vector) { return *((int8_t *) (&vector)); }

static inline uint16_t DSPLIB_firstElement(c7x::ushort_vec vector) { return *((uint16_t *) (&vector)); }

static inline int16_t DSPLIB_firstElement(c7x::short_vec vector) { return *((int16_t *) (&vector)); }

#endif // #ifdef __cplusplus

#endif /* C71_DSPLIB_DEBUG_H_ */
