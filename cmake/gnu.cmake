# =============================================================================
# GD32F4xx 交叉编译工具链文件 — GNU Arm Embedded (arm-none-eabi-gcc) + picolibc
#
# 测试用途: 与 cmake/llvm.cmake (ATfE/clang + picolibc) 平行, 用于对比
# 编译效果。通过 --specs=picolibc.specs 启用工具链内置的 picolibc。
# 工具链根目录可用缓存变量 GNU_ARM_ROOT 或环境变量 GNU_ARM_ROOT 覆盖。
#
# 使用:
#   cmake -S . -B build-gnu \
#       -DCMAKE_TOOLCHAIN_FILE=cmake/gnu.cmake \
#       -DGD32_PART=GD32F470VG
#
# 与 llvm.cmake (ATfE) 的差异:
#   - 无 --target: gcc 前缀即目标三元组
#   - 链接走 gcc 驱动的 GNU ld, 无 -fuse-ld=lld
#   - picolibc 经 --specs=picolibc.specs 启用 (multilib 精确匹配
#     thumb/v7e-m+fp/hard), 无需手动 -lcrt0: specs 的 *startfile 段
#     自动注入对应 multilib 的 crt0.o (符号契约与 ATfE 的 libcrt0.a
#     完全一致: __data_start/__bss_size/__tls_base + _start)
# =============================================================================

if(DEFINED ENV{GNU_ARM_ROOT})
    set(GNU_ARM_ROOT "$ENV{GNU_ARM_ROOT}")
else()
    set(GNU_ARM_ROOT "/opt/arm-gnu-toolchain-15.3.rel2-x86_64-arm-none-eabi")
endif()
set(GNU_BIN "${GNU_ARM_ROOT}/bin")

if(NOT EXISTS "${GNU_BIN}/arm-none-eabi-gcc")
    message(FATAL_ERROR "未找到 GNU 工具链: ${GNU_BIN}/arm-none-eabi-gcc\n"
        "请确认安装路径, 或用 -DGNU_ARM_ROOT=<路径> / 环境变量 GNU_ARM_ROOT 指定")
endif()

# 确认 picolibc 可用 (specs 文件存在)
file(GLOB GNU_PICOLIBC_SPECS
    "${GNU_ARM_ROOT}/lib/gcc/arm-none-eabi/*/picolibc.specs")
if(NOT GNU_PICOLIBC_SPECS)
    message(FATAL_ERROR "未找到 picolibc.specs (在 ${GNU_ARM_ROOT}/lib/gcc/arm-none-eabi/*/ 下)")
endif()

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 各工具 (gcc 驱动链接, CMAKE_LINKER 仅记录 ld 位置)
set(CMAKE_C_COMPILER   "${GNU_BIN}/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${GNU_BIN}/arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "${GNU_BIN}/arm-none-eabi-gcc")
set(CMAKE_AR           "${GNU_BIN}/arm-none-eabi-ar")
set(CMAKE_RANLIB       "${GNU_BIN}/arm-none-eabi-ranlib")
set(CMAKE_OBJCOPY      "${GNU_BIN}/arm-none-eabi-objcopy")
set(CMAKE_OBJDUMP      "${GNU_BIN}/arm-none-eabi-objdump")
set(CMAKE_SIZE         "${GNU_BIN}/arm-none-eabi-size")
set(CMAKE_LINKER       "${GNU_BIN}/arm-none-eabi-ld")

# 目标三元组由编译器可执行文件名决定, 无需 --target
set(TARGET_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard --specs=picolibc.specs")
set(CMAKE_C_FLAGS_INIT   "${TARGET_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${TARGET_FLAGS}")

# 链接: 不能用 -nostartfiles —— picolibc 的 _start 在 crt0.o 里, 由
# specs 的 *startfile 段自动注入, 排除它会导致 undefined reference to _start。
# 应用自管启动 (Reset_Handler 调 _start) 与 ATfE 方案一致;
# 传了自定义 -T 时 specs 不会再注入它自带的 picolibc.ld
set(CMAKE_EXE_LINKER_FLAGS_INIT "")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
