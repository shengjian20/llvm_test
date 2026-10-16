# =============================================================================
# RT-Thread v5.3.0 内核适配 (独立 CMake 包)
#
# 用法: 根工程 set(RTOS_RTTHREAD ON) + add_subdirectory(third_party/rtos)
#       target_link_libraries(<app> PRIVATE RTOS::rtthread)
#
# 内容范围 (仅内核):
#   - src/ 内核通用源 (调度/IPC/时钟/内存/对象)
#   - libcpu/arm/common + libcpu/arm/cortex-m4 (context_gcc.S + cpuport.c)
#   - 配置头: <本目录>/rtconfig.h
# 不含: bsp/ components/ examples/ documentation/ finsh shell (外围不适配)
#
# 符号对接: context_gcc.S 直接实现 PendSV_Handler / SysTick_Handler,
#   与工程 startup_gd32f4xx.c 的弱符号向量表对接, 无需胶水。
# =============================================================================

set(RTTHREAD_SRC "${CMAKE_CURRENT_LIST_DIR}")

set(RTTHREAD_KERNEL_SRC
    ${RTTHREAD_SRC}/src/clock.c
    ${RTTHREAD_SRC}/src/components.c
    ${RTTHREAD_SRC}/src/cpu_up.c
    ${RTTHREAD_SRC}/src/defunct.c
    ${RTTHREAD_SRC}/src/idle.c
    ${RTTHREAD_SRC}/src/ipc.c
    ${RTTHREAD_SRC}/src/irq.c
    ${RTTHREAD_SRC}/src/kservice.c
    ${RTTHREAD_SRC}/src/klibc/kerrno.c
    ${RTTHREAD_SRC}/src/klibc/kstdio.c
    ${RTTHREAD_SRC}/src/klibc/kstring.c
    ${RTTHREAD_SRC}/src/klibc/rt_vsnprintf_std.c
    ${RTTHREAD_SRC}/src/klibc/rt_vsscanf.c
    ${RTTHREAD_SRC}/src/mem.c
    ${RTTHREAD_SRC}/src/memheap.c
    ${RTTHREAD_SRC}/src/mempool.c
    ${RTTHREAD_SRC}/src/object.c
    # 单核配置: scheduler_up.c (UP 调度器); scheduler_mp.c 属 SMP 内核,
    # 未定义 RT_USING_SMP 时不参与编译
    ${RTTHREAD_SRC}/src/scheduler_up.c
    ${RTTHREAD_SRC}/src/thread.c
    ${RTTHREAD_SRC}/src/timer.c
)

add_library(rtos_rtthread STATIC
    ${RTTHREAD_KERNEL_SRC}
    ${RTTHREAD_SRC}/libcpu/arm/common/div0.c
    ${RTTHREAD_SRC}/libcpu/arm/cortex-m4/context_gcc.S
    ${RTTHREAD_SRC}/libcpu/arm/cortex-m4/cpuport.c
)

target_include_directories(rtos_rtthread PUBLIC
    ${RTTHREAD_SRC}/include
    ${RTTHREAD_SRC}/libcpu/arm/common
    # rtconfig.h 所在目录 (RT-Thread 要求 RT_USING_ 配置在此)
    ${CMAKE_CURRENT_LIST_DIR}
)

# rt-thread 内核要求: RT_USING_XXX 等宏来自 rtconfig.h (不要在命令行重复定义)
target_compile_definitions(rtos_rtthread PRIVATE
    # 内核源文件编译时必须定义 (rtsched.h 等内部 API 的声明由它解锁)
    __RT_KERNEL_SOURCE__
)

target_compile_options(rtos_rtthread PRIVATE -ffunction-sections -fdata-sections)

add_library(RTOS::rtthread ALIAS rtos_rtthread)
