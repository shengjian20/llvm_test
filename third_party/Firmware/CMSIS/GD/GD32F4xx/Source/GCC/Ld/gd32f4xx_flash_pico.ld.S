/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
 * Copyright (c) 2026, GigaDevice Semiconductor Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Based on GigaDevice gd32f470xG_flash.ld (picolibc/ATfE 改造版, 全家族共享).
 */

#if defined(GD32F405xE) || defined(GD32F407xE)
  #define PICO_FLASH_KB 512
  #define PICO_RAM_KB   128
#elif defined(GD32F405xG) || defined(GD32F407xG)
  #define PICO_FLASH_KB 1024
  #define PICO_RAM_KB   128
#elif defined(GD32F405xK) || defined(GD32F407xK)
  #define PICO_FLASH_KB 3072
  #define PICO_RAM_KB   128
#elif defined(GD32F425xE) || defined(GD32F427xE) || defined(GD32F450xE) || defined(GD32F470xE)
  #define PICO_FLASH_KB 512
  #define PICO_RAM_KB   192
#elif defined(GD32F425xG) || defined(GD32F427xG) || defined(GD32F450xG)
  #define PICO_FLASH_KB 1024
  #define PICO_RAM_KB   192
#elif defined(GD32F425xK) || defined(GD32F427xK) || defined(GD32F450xK) || defined(GD32F470xK)
  #define PICO_FLASH_KB 3072
  #define PICO_RAM_KB   192
#elif defined(GD32F450xI)
  #define PICO_FLASH_KB 2048
  #define PICO_RAM_KB   448
#elif defined(GD32F470xG)
  #define PICO_FLASH_KB 1024
  #define PICO_RAM_KB   448
#elif defined(GD32F470xI)
  #define PICO_FLASH_KB 2048
  #define PICO_RAM_KB   704
#else
  #error "gd32f4xx_flash_pico.ld.S: 未定义器件容量宏 (应为 GD32F470xG 这类, 由 CMake 依据 GD32_PART 注入)"
#endif

MEMORY
{
  FLASH    (rx): ORIGIN = 0x08000000, LENGTH = PICO_FLASH_KB * 1K
  RAM      (xrw): ORIGIN = 0x20000000, LENGTH = PICO_RAM_KB * 1K
  TCMRAM   (xrw): ORIGIN = 0x10000000, LENGTH = 64K
}

ENTRY(_start)

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 2K;
  __heap_size = DEFINED(__heap_size) ? __heap_size : 1K;

  .vectors :
  {
    . = ALIGN(4);
    KEEP(*(.vectors))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
    _etext = .;
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .ARM.attributes :
  {
   *(.ARM.attributes)
  }
  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  /* ---- ATfE 21+ picolibc: __libc_init_array 遍历的范围 ---- */
  PROVIDE_HIDDEN(__bothinit_array_start = __preinit_array_start);
  PROVIDE_HIDDEN(__bothinit_array_end   = __init_array_end);

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array*))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  __data_source = LOADADDR(.data);   /* .data 在 Flash 中的拷贝源 (LMA) */
  .data :
  {
    . = ALIGN(4);
    __data_start = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    __data_end = .;
  } > RAM AT> FLASH
  __data_size = __data_end - __data_start;

  .bss (NOLOAD):
  {
    . = ALIGN(4);
    __bss_start = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end = .;
  } > RAM
  __bss_size = __bss_end - __bss_start;
  __tls_base = __bss_end;            /* TLS 未使用, 仅提供一个合法的 RAM 地址 */
  __arm32_tls_tcb_offset = 8;        /* libc.a 的 _set_tls (crt0 调用) 需要, 不可删除 */

  /* heap and stack (NOLOAD: 不占用 Flash, 不写入 bin) */
  .heap_stack (NOLOAD):
  {
    . = ALIGN(8);
    PROVIDE ( end = __bss_end );
    PROVIDE ( _end = __bss_end );
    . = . + __heap_size;
    PROVIDE( _heap_end = . );
    . = . + __stack_size;
    PROVIDE( _sp = . );
    . = ALIGN(8);
  } > RAM

  __tcmram_source = LOADADDR(.tcmram);
  .tcmram :
  {
    . = ALIGN(4);
    __tcmram_start = .;
    *(.tcmram)
    *(.tcmram*)
    . = ALIGN(4);
    __tcmram_end = .;
  } > TCMRAM AT> FLASH
  __tcmram_size = __tcmram_end - __tcmram_start;
}
