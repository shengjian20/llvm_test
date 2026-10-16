/*
 * rtconfig.h — RT-Thread v5.3.0 内核配置 (GD32F470VG / Cortex-M4F)
 *
 * 仅内核适配: 最小内核配置, 不含设备框架/components/finsh shell。
 * 命名对接: 本工程 startup_gd32f4xx.c 弱符号向量表 + rt-thread 的
 *   libcpu/arm/cortex-m4/context_gcc.S 导出 PendSV_Handler/SysTick_Handler
 *   (rt_hw_context_switch_* 挂在 PendSV), 与向量表直接对接。
 */

#ifndef __RTCONFIG_H__
#define __RTCONFIG_H__

/* ---- 基本配置 ---- */
#define RT_NAME_MAX                          8
#define RT_ALIGN_SIZE                        8
#define RT_THREAD_PRIORITY_MAX               32
#define RT_TICK_PER_SECOND                   1000
#define RT_CPUS_NR                           1   /* 单核 (Kconfig 默认, 内核源必需) */
#define RT_BACKTRACE_LEVEL_MAX_NR            32  /* 回溯深度上限 (Kconfig 默认) */

/* ---- 内核对象与 IPC ---- */
#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* ---- 内存管理: small mem (依赖堆框架, v5.3 起为堆上算法层) ---- */
#define RT_USING_HEAP
#define RT_USING_SMALL_MEM

/* ---- klibc (内核自带轻量库) + picolibc 双轨 ----
 * klibc 提供 rt_vsnprintf/rt_kprintf 等内核入口 (标准版实现):
 *   - RT_KLIBC_USING_VSNPRINTF_STANDARD: 完整格式化 (v5.3 Kconfig 默认)
 *   - 底层字符/内存操作 include <string.h>/<stdio.h> 由 picolibc 承接
 * RT_USING_LIBC: 声明使用标准 libc (picolibc), rtdef.h 走 UINTn_MAX 等库路径
 *
 * RT_KLIBC_USING_LIBC_*: 逐函数开关, 定义后 klibc 的 rt_memcpy/rt_strlen
 *   等直接转调 picolibc 同名实现 (kstring.c/kstdio.c 内按宏裁剪),
 *   内核不自带重复实现 —— 契合"字符串/内存原语由 picolibc 承接"的分工 */
#define RT_USING_LIBC
#define RT_KLIBC_USING_VSNPRINTF_STANDARD
#define RT_KLIBC_USING_VSNPRINTF_LONGLONG
#define RT_KLIBC_USING_LIBC_MEMSET
#define RT_KLIBC_USING_LIBC_MEMCPY
#define RT_KLIBC_USING_LIBC_MEMMOVE
#define RT_KLIBC_USING_LIBC_MEMCMP
#define RT_KLIBC_USING_LIBC_STRSTR
#define RT_KLIBC_USING_LIBC_STRNCPY
#define RT_KLIBC_USING_LIBC_STRCPY
#define RT_KLIBC_USING_LIBC_STRNCMP
#define RT_KLIBC_USING_LIBC_STRCMP
#define RT_KLIBC_USING_LIBC_STRLEN

/* ---- 软件定时器 ---- */
#define RT_USING_SOFT_TIMER

/* ---- 不启用 (内核适配范围外) ----
 * RT_USING_HEAP / RT_USING_DEVICE / RT_USING_CONSOLE / finsh /
 * components / dfs / lwIP 等, 需要时再逐个打开 */

/* ---- 调试 ---- */
#define RT_DEBUGING_COLOR
/* #define RT_DEBUGING_CONTEXT */

#endif /* __RTCONFIG_H__ */
