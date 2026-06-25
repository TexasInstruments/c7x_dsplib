// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#if defined(FLAT_MEM)
-heap  0x7C00000
-stack 0x20000
--args 0x1000

--diag_suppress=10068 // "no matching section"
--cinit_compression=off

MEMORY
{
#if defined(RTL_TEST)
  L2SRAM (RWX)  : org = 0x64E00000, len = 0xB200000
  /* Temporary place holder for L1D*/
  L1DSRAM  (RWX): org = 0xA0000000, len = 0x004000
#else
  L2SRAM (RWX)  : org = 0x64E00000, len = 0xF800000
  L2SRAMAUX   (RWX): org = 0x74600000, len = 0x100000
  /* Temporary place holder for L1D*/
  L1DSRAM  (RWX): org = 0xA0000000, len = 0x004000
#endif
  EXTMEM (RWX)  : org = 0x80000000, len = 0x800000
}

SECTIONS
{
  .sram_start START(_sram_start) > L2SRAM NOINIT

  .kernel: {
    *.obj (.text:optimized) { SIZE(_kernel_size) }
  } > L2SRAM

  .kernel_data SIZE(_data_size)

  .text:            > L2SRAM
  .text:touch:      > L2SRAM
  .neardata:        > L2SRAM
  .rodata:          > L2SRAM
  .bss:             > L2SRAM
  .init_array:      > L2SRAM
  .far:             > L2SRAM
  .fardata:         > L2SRAM
  .neardata         > L2SRAM
  .rodata           > L2SRAM
  .data:            > L2SRAM
  .switch:          > L2SRAM
  .stack:           > L2SRAM
  .args:            > L2SRAM align = 0x4, fill = 0 {_argsize = 0x200; }
  .sysmem:          > L2SRAM
  .cinit:           > L2SRAM
  .const:           > L2SRAM START(const_start) SIZE(const_size)
  .pinit:           > L2SRAM
  .cio:             > L2SRAM
  .stack:           > L2SRAM
  .staticData       > L2SRAM
  .msmcData         > L2SRAM
  .ddrData          > L2SRAM
  .l1dmemory        > L1DSRAM
  .l2dmemory        > L2SRAM
  .l2sramaux        > L2SRAMAUX
   xdc.meta:        > L2SRAM, type = COPY
}

#else // not FLAT_MEM

-heap  0xF0000   // 756 kB
-stack 0x4000    //  16 kB
--cinit_compression=off
--args 0x1000
--diag_suppress=10068 // "no matching section"

MEMORY
{
  L2SRAM_CINIT (RWX)  : org = 0x7E000000, len = 0x000100 //for 256byte init
  L2SRAM (RWX)        : org = 0x7E000100, len = 0x0fff00 //for 1MBytes  EL2
  L2SRAMAUX   (RWX): org = 0x7F000000, len = 0x040000   // for 256 KBytes AM62A
  L1DSRAM  (RWX): org = 0x64E00000, len = 0x004000   // 16kB, for J7

/*  EXTMEM_STATIC   (RWX): org = 0x80000000, len = 0x200000 */
  EXTMEM_STATIC   (RWX): org = 0x80000000, len = 0x2000000
  EXTMEM_DATACN   (RWX): org = 0x82000000, len = 0x200000
  EXTMEM   (RWX): org = 0x82200000, len = 0x200000
  /* rem EXTMEM   (RWX): org = 0x80600000, len = 0x400000
   * rem EXTMEMPAGE (RWX): org = 0x80A00000, len = 0x200000 */
}

SECTIONS
{
  .sram_start START(_sram_start) > L2SRAM NOINIT

//  .kernel: {
//    *.obj (.text:optimized) { SIZE(_kernel_size) }
//  } > EXTMEM

//  .kernel_data SIZE(_data_size)
  .l1dmemory        > L2SRAM
  .l2dmemory        > L2SRAM
  .text:            > EXTMEM
  .text:touch:      > EXTMEM
  .text:_c_int00:   > L2SRAM_CINIT
  .neardata:        > EXTMEM
  .rodata:          > EXTMEM
  .bss:             > EXTMEM
  .init_array:      > EXTMEM
  .far:             > EXTMEM
  .fardata:         > EXTMEM
  .neardata         > EXTMEM
  .rodata           > EXTMEM
  .data:            > EXTMEM
  .switch:          > EXTMEM
  .stack:           > L2SRAM
  .args:            > EXTMEM align = 0x4, fill = 0 {_argsize = 0x200; }
  .sysmem:          > L2SRAM
  .cinit:           > EXTMEM
  .const:           > L2SRAM START(const_start) SIZE(const_size)
  .pinit:           > L2SRAM
  .cio:             > L2SRAM
  .stack:           > EXTMEM
  .ddrData          > EXTMEM_DATACN
  .staticData       > EXTMEM_STATIC
  .l2sramaux        > L2SRAMAUX
  xdc.meta:        > L2SRAM, type = COPY
}
#endif
