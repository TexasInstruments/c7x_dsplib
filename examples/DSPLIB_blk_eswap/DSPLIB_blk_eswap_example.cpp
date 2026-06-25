// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <stdint.h>

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/
uint16_t inUint16[]  = {0x2BAE, 0xC99A, 0xCC07, 0xAB58, 0x8EBF, 0xDC49, 0x24FE, 0x777D, 0x5333, 0xB4EB, 0x30BC,
                        0xE018, 0xDC78, 0xBE20, 0x3085, 0x4754, 0x72D3, 0x6528, 0x061B, 0xFDAC, 0xBA84, 0xE738,
                        0x364E, 0xA0BA, 0x4A94, 0xEF9A, 0xA2F1, 0x3EFC, 0x0163, 0xA7C8, 0xE1F1, 0x8E1D};
uint16_t outUint16[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/*
uint8_t refOutUint16[] = {0xAE2B, 0x9AC9, 0x07CC, 0x58AB, 0xBF8E, 0x49DC, 0xFE24, 0x7D77,
                          0x3353, 0xEBB4, 0xBC30, 0x18E0, 0x78DC, 0x20BE, 0x8530, 0x5447,
                          0xD372, 0x2865, 0x1B06, 0xACFD, 0x84BA, 0x38E7, 0x4E36, 0xBAA0,
                          0x944A, 0x9AEF, 0xF1A2, 0xFC3E, 0x6301, 0xC8A7, 0xF1E1, 0x1D8E};
 */

uint32_t inUint32[] = {0x000DE689, 0x0626078F, 0x9B017C9A, 0x82393BA1, 0xE0EC7D15, 0x2B81285F, 0xA6D26E3C, 0xE5CF6D0E,
                       0xE2ECBCA2, 0xC4DC9ADC, 0x199E9AF6, 0xAD38A746, 0x3E81DD44, 0xA9BCE1B4, 0x4B46A38D, 0x7650FA90,
                       0x90F64837, 0x7B5013D2, 0x670DD9AE, 0x84B16365, 0x307196A7, 0xCF74F114, 0x72416E15, 0xD358C18E,
                       0xD2415819, 0x0FB244BD, 0xE15366AA, 0xEA105D4B, 0xA9572B62, 0xC6EA34C3, 0x31EE95A0, 0xEA6C348E};

uint32_t outUint32[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/*
uint32_t refOutUint32[] = {0x89E60D00, 0x8F072606, 0x9A7C019B, 0xA13B3982,
                         0x157DECE0, 0x5F28812B, 0x3C6ED2A6, 0x0E6DCFE5,
                         0xA2BCECE2, 0xDC9ADCC4, 0xF69A9E19, 0x46A738AD,
                         0x44DD813E, 0xB4E1BCA9, 0x8DA3464B, 0x90FA5076,
                         0x3748F690, 0xD213507B, 0xAED90D67, 0x6563B184,
                         0xA7967130, 0x14F174CF, 0x156E4172, 0x8EC158D3,
                         0x195841D2, 0xBD44B20F, 0xAA6653E1, 0x4B5D10EA,
                         0x622B57A9, 0xC334EAC6, 0xA095EE31, 0x8E346CEA};
 */

uint64_t inUint64[]  = {0xF68D190C087F8A70, 0x62E6B9D4C8807796, 0x41D462A153E11933, 0xB3BC1A704D28E357,
                        0xA8FCE3DDF5E61AE3, 0xAF661C5D2EBCCE9A, 0x96C320BED3ECC716, 0x14DBF7937A5A37ED,
                        0xF8E511A0F0326EEB, 0x20B20332E4B99FE2, 0x6FBDB9F23FCF69BB, 0x0919393ABA12A648,
                        0xD6F9D7171B3C5F96, 0x487D419E00D13895, 0x8AF03FEDC26E37E9, 0x1C91CF5DBDB6AE05,
                        0x8D708AFBE290EB19, 0x98FA4CFB3D423BF2, 0x05C03CED9B3E6474, 0xEB3C2A152CDB5B8F,
                        0x3301ECC170551CDE, 0xC9E9805A9429CAA6, 0xC0B78D8B6DA5CAD3, 0x0E1CA4A82A8EEB69,
                        0x85E7C65AE12765BD, 0x62B1AFE7131D6F41, 0x00E5C1D0E712E7C2, 0xF7204DC112C4E16A,
                        0xDA9CB9C49BE5B02E, 0x0B218841A240AE16, 0x0263375612E5D550, 0x34743AB5814FC9D1};
uint64_t outUint64[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/*
uint64_t refOutUint64[] = {0x708A7F080C198DF6, 0x967780C8D4B9E662, 0x3319E153A162D441, 0x57E3284D701ABCB3,
                         0xE31AE6F5DDE3FCA8, 0x9ACEBC2E5D1C66AF, 0x16C7ECD3BE20C396, 0xED375A7A93F7DB14,
                         0xEB6E32F0A011E5F8, 0xE29FB9E43203B220, 0xBB69CF3FF2B9BD6F, 0x48A612BA3A391909,
                         0x965F3C1B17D7F9D6, 0x9538D1009E417D48, 0xE9376EC2ED3FF08A, 0x05AEB6BD5DCF911C,
                         0x19EB90E2FB8A708D, 0xF23B423DFB4CFA98, 0x74643E9BED3CC005, 0x8F5BDB2C152A3CEB,
                         0xDE1C5570C1EC0133, 0xA6CA29945A80E9C9, 0xD3CAA56D8B8DB7C0, 0x69EB8E2AA8A41C0E,
                         0xBD6527E15AC6E785, 0x416F1D13E7AFB162, 0xC2E712E7D0C1E500, 0x6AE1C412C14D20F7,
                         0x2EB0E59BC4B99CDA, 0x16AE40A24188210B, 0x50D5E51256376302, 0xD1C94F81B53A7434};
 */
int32_t size = 32;

int main(void)
{
   DSPLIB_blk_eswap_InitArgs kerInitArgs;
   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t             handleSize = DSPLIB_blk_eswap_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   /* Example to block endian swap uint16_t data type*/
   bufParamsIn.data_type = DSPLIB_UINT16;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_UINT16;
   bufParamsOut.dim_x     = size;

   int32_t status = DSPLIB_blk_eswap_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_blk_eswap_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_blk_eswap_exec_checkParams(handle, inUint16, outUint16);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_blk_eswap_exec(handle, inUint16, outUint16);
         }
      }
   }
   printf("Block endian swap 16bit integer data type\n");
   printf("%s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%04X -> %04X\n", inUint16[i], outUint16[i]);
   }
   printf("\n");

   /* Example to block endian swap int32_t data type*/
   bufParamsIn.data_type = DSPLIB_UINT32;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_UINT32;
   bufParamsOut.dim_x     = size;

   status = DSPLIB_blk_eswap_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_blk_eswap_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_blk_eswap_exec_checkParams(handle, inUint32, outUint32);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_blk_eswap_exec(handle, inUint32, outUint32);
         }
      }
   }
   printf("Block endian swap 32bit integer data type\n");
   printf("%s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%08X -> %08X\n", inUint32[i], outUint32[i]);
   }
   printf("\n");

   /* Example to block endian swap int64_t data type*/
   bufParamsIn.data_type = DSPLIB_UINT64;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_UINT64;
   bufParamsOut.dim_x     = size;

   status = DSPLIB_blk_eswap_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_blk_eswap_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_blk_eswap_exec_checkParams(handle, inUint64, outUint64);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_blk_eswap_exec(handle, inUint64, outUint64);
         }
      }
   }
   printf("Block endian swap 64bit integer data type\n");
   printf("%s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%016lX -> %016lX\n", inUint64[i], outUint64[i]);
   }
   printf("\n");

   return 0;
}
