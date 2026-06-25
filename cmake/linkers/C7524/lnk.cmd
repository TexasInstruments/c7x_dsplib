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
#else
  L2SRAM (RWX)  : org = 0x64E00000, len = 0xF800000
  L2SRAMAUX   (RWX): org = 0x74600000, len = 0x100000
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
  .l2dmemory        > L2SRAM
  .l2sramaux        > L2SRAMAUX
   xdc.meta:        > L2SRAM, type = COPY
}

#else // not FLAT_MEM

-heap  0xF0000   // 756 kB
-stack 0x4000    //  -stack 0x4000  for 16 kB
--cinit_compression=off
--args 0x1000
--diag_suppress=10068 // "no matching section"

MEMORY
{
#if SOC == j722s
  L2SRAM_CINIT (RWX)  : org = 0x7E000000, len = 0x000100 //for 256byte init     c7x_0 = 7E000000, c7x_1 = 7E200000
  L2SRAM (RWX)        : org = 0x7E000100, len = 0x1fff00 //for 2MBytes  EL2
  L2SRAMAUX   (RWX): org = 0x7F000000, len = 0x040000   // for 256 KBytes J7AEN c7x_0 = 7F000000, c7x_1 = 7F800000
#elif SOC == AM275 
  L2SRAM_CINIT (RWX)  : org = 0x7E000000, len = 0x000100 //for 256byte init
  L2SRAM (RWX)        : org = 0x7E000100, len = 0x1fff00 //for 2MBytes  EL2
  L2SRAMAUX      (RWX): org = 0x7E200000, len = 0x040000   // for 256 KBytes AM275
  HYPERRAM       (RW) : org = 0x68000000, len = 0x4000000   // 64 MB HyperRAM
#endif

#if defined(VMIN_DDR_MEMORY_RANGE)
  EXTMEM_STATIC   (RWX): org = 0xa0000000, len = 0x200000
  EXTMEM_DATACN   (RWX): org = 0xa0200000, len = 0x200000
  EXTMEM          (RWX): org = 0xa0400000, len = 0x200000
#else  
  EXTMEM_STATIC   (RWX): org = 0x80000000, len = 0x200000
  EXTMEM_DATACN   (RWX): org = 0x80200000, len = 0x200000
  EXTMEM          (RWX): org = 0x80400000, len = 0x200000
#endif // VMIN_DDR_MEMORY_RANGE
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
#if SOC == AM275 
  .staticData       > HYPERRAM
#else
  .staticData       > EXTMEM
#endif
  .l2sramaux        > L2SRAMAUX
  .coeffMemory      > EXTMEM
  xdc.meta:        > L2SRAM, type = COPY
}
#endif
