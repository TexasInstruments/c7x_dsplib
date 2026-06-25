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
   xdc.meta:        > L2SRAM, type = COPY
}

#else // not FLAT_MEM

-heap  0x70000   // 448 kB
-stack 0x4000    //  16 kB
--cinit_compression=off
--args 0x1000
--diag_suppress=10068 // "no matching section"

MEMORY
{
#if SOC == j784s4
  MSMC3SRAM_CINIT (RWX)   : org = 0x70000000, len = 0x000100
  MSMC3SRAM (RWX)         : org = 0x70000100, len = 0x1fff00
  MSMC3SRAM_STATIC   (RWX): org = 0x70200000, len = 0x200000
  MSMC3SRAM_DATACN   (RWX): org = 0x70400000, len = 0x300000
  MSMC3SRAM_PT       (RWX): org = 0x70700000, len = 0x100000
  MSMCSRAM (RWX)          : org = 0x68000000, len = 0x300000
#else
  MSMCSRAM_CINIT (RWX)  : org = 0x70000000, len = 0x000100
  MSMCSRAM (RWX)        : org = 0x70000100, len = 0x3fff00
  EXTMEM_STATIC   (RWX): org = 0x80000000, len = 0x200000
  EXTMEM_DATACN   (RWX): org = 0x80200000, len = 0x400000
  EXTMEM   (RWX): org = 0x80600000, len = 0x400000
  EXTMEMPAGE (RWX): org = 0x80A00000, len = 0x200000
#endif // SOC
  L2SRAM   (RWX): org = 0x64800000, len = 0x080000   // for J7
  L1DSRAM  (RWX): org = 0x64E00000, len = 0x004000   // 16kB, for J7

}

SECTIONS
{
  .sram_start START(_sram_start) > L2SRAM NOINIT

  .kernel: {
    *.obj (.text:optimized) { SIZE(_kernel_size) }
  } > L2SRAM

  .kernel_data SIZE(_data_size)
  .l1dmemory        > L1DSRAM
  .l2dmemory        > L2SRAM
#if SOC == j784s4
  .text:            > MSMC3SRAM
  .text:touch:      > MSMC3SRAM
  .text:_c_int00:   > MSMC3SRAM_CINIT
  .neardata:        > MSMC3SRAM
  .rodata:          > MSMC3SRAM
  .bss:             > MSMC3SRAM
  .init_array:      > MSMC3SRAM
  .far:             > MSMC3SRAM
  .fardata:         > MSMC3SRAM
  .neardata         > MSMC3SRAM
  .rodata           > MSMC3SRAM
  .data:            > MSMC3SRAM

  .data:pte:pte_lvl1_0000000000000000 > MSMC3SRAM_PT
  .data:pte:pte_lvl0 > MSMC3SRAM_PT
  .data:pte:pte_lvl2_40000000 > MSMC3SRAM_PT
  .data:pte:pte_lvl2_80000000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64800000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64E00000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64D00000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64D20000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64D40000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64D60000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64D80000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64DA0000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64DC0000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_64DE0000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_70000000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_70200000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_70400000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_70600000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_80000000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_80200000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_80400000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_80600000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_80800000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_80A00000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_80C00000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_80E00000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_81000000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_81200000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_81400000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_81600000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_81800000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_81A00000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_81C00000 > MSMC3SRAM_PT
  .data:pte:pte_lvl3_81E00000 > MSMC3SRAM_PT

  .switch:          > MSMC3SRAM
  .stack:           > MSMC3SRAM
  .args:            > MSMC3SRAM align = 0x4, fill = 0 {_argsize = 0x200; }
  .sysmem:          > L2SRAM
  .cinit:           > MSMC3SRAM
  .const:           > MSMC3SRAM START(const_start) SIZE(const_size)
  .pinit:           > MSMC3SRAM
  .cio:             > MSMC3SRAM
  .stack:           > MSMC3SRAM
  .ddrData          > MSMC3SRAM_DATACN
  .staticData       > MSMC3SRAM_STATIC
  .msmcData         > MSMCSRAM
   xdc.meta:        > MSMC3SRAM, type = COPY
#else
  .text:            > MSMCSRAM
  .text:touch:      > MSMCSRAM
  .text:_c_int00:   > MSMCSRAM_CINIT
  .neardata:        > MSMCSRAM
  .rodata:          > MSMCSRAM
  .bss:             > MSMCSRAM
  .init_array:      > MSMCSRAM
  .far:             > MSMCSRAM
  .fardata:         > MSMCSRAM
  .neardata         > MSMCSRAM
  .rodata           > MSMCSRAM
  .data:            > MSMCSRAM
  .data:pte:pte_lvl1_0000000000000000 > EXTMEMPAGE
  .data:pte:pte_lvl0 > EXTMEMPAGE
  .data:pte:pte_lvl2_40000000 > EXTMEMPAGE
  .data:pte:pte_lvl2_80000000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64800000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64E00000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64D00000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64D20000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64D40000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64D60000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64D80000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64DA0000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64DC0000 > EXTMEMPAGE
  .data:pte:pte_lvl3_64DE0000 > EXTMEMPAGE
  .data:pte:pte_lvl3_70000000 > EXTMEMPAGE
  .data:pte:pte_lvl3_70200000 > EXTMEMPAGE
  .data:pte:pte_lvl3_70400000 > EXTMEMPAGE
  .data:pte:pte_lvl3_70600000 > EXTMEMPAGE
  .data:pte:pte_lvl3_80000000 > EXTMEMPAGE
  .data:pte:pte_lvl3_80200000 > EXTMEMPAGE
  .data:pte:pte_lvl3_80400000 > EXTMEMPAGE
  .data:pte:pte_lvl3_80600000 > EXTMEMPAGE
  .data:pte:pte_lvl3_80800000 > EXTMEMPAGE
  .data:pte:pte_lvl3_80A00000 > EXTMEMPAGE
  .data:pte:pte_lvl3_80C00000 > EXTMEMPAGE
  .data:pte:pte_lvl3_80E00000 > EXTMEMPAGE
  .data:pte:pte_lvl3_81000000 > EXTMEMPAGE
  .data:pte:pte_lvl3_81200000 > EXTMEMPAGE
  .data:pte:pte_lvl3_81400000 > EXTMEMPAGE
  .data:pte:pte_lvl3_81600000 > EXTMEMPAGE
  .data:pte:pte_lvl3_81800000 > EXTMEMPAGE
  .data:pte:pte_lvl3_81A00000 > EXTMEMPAGE
  .data:pte:pte_lvl3_81C00000 > EXTMEMPAGE
  .data:pte:pte_lvl3_81E00000 > EXTMEMPAGE

  .switch:          > MSMCSRAM
  .stack:           > MSMCSRAM
  .args:            > MSMCSRAM align = 0x4, fill = 0 {_argsize = 0x200; }
  .sysmem:          > L2SRAM
  .cinit:           > MSMCSRAM
  .const:           > MSMCSRAM START(const_start) SIZE(const_size)
  .pinit:           > MSMCSRAM
  .cio:             > MSMCSRAM
  .stack:           > MSMCSRAM
  .ddrData          > EXTMEM_DATACN
  .staticData       > EXTMEM_STATIC
  .msmcData         > MSMCSRAM
   xdc.meta:        > MSMCSRAM, type = COPY
#endif

}
#endif
