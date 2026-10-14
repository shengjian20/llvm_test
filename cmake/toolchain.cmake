# CMake 交叉编译工具链文件
# 目标: GD32F470VGT6 (Cortex-M4F, hard-float)
# 工具链: Arm Toolchain for Embedded (ATfE, 基于 LLVM/clang + picolibc)
#
# 工具链根目录: 可用环境变量 ATFE_ROOT 覆盖, 否则使用下面的默认路径
if(DEFINED ENV{ATFE_ROOT})
    set(ATFE_ROOT "$ENV{ATFE_ROOT}")
else()
    set(ATFE_ROOT "/opt/ATfE-23.1.0-Linux-x86_64")
endif()

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 指定各工具使用绝对路径
set(CMAKE_C_COMPILER   "${ATFE_ROOT}/bin/clang")
set(CMAKE_CXX_COMPILER "${ATFE_ROOT}/bin/clang++")
set(CMAKE_ASM_COMPILER "${ATFE_ROOT}/bin/clang")
set(CMAKE_AR           "${ATFE_ROOT}/bin/llvm-ar")
set(CMAKE_RANLIB       "${ATFE_ROOT}/bin/llvm-ranlib")
set(CMAKE_OBJCOPY      "${ATFE_ROOT}/bin/llvm-objcopy")
set(CMAKE_OBJDUMP      "${ATFE_ROOT}/bin/llvm-objdump")
set(CMAKE_SIZE         "${ATFE_ROOT}/bin/llvm-size")
set(CMAKE_STRIP        "${ATFE_ROOT}/bin/llvm-strip")
set(CMAKE_LINKER       "${ATFE_ROOT}/bin/ld.lld")

# 目标三元组 (bare-metal ARM)
set(CMAKE_C_COMPILER_TARGET   arm-none-eabi)
set(CMAKE_CXX_COMPILER_TARGET arm-none-eabi)
set(CMAKE_ASM_COMPILER_TARGET arm-none-eabi)

# GD32F470VGT6: Cortex-M4, 单精度 FPU (fpv4-sp-d16), 硬件浮点 ABI
set(TARGET_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -gdwarf-4")
set(CMAKE_C_FLAGS_INIT   "${TARGET_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${TARGET_FLAGS}")

# 链接: 使用 lld; 不使用编译器自带的启动文件 (-nostartfiles),
# 启动代码由 GD 固件库提供; 具体的 -T 链接脚本在顶层 CMakeLists.txt 中指定
set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=lld -nostartfiles")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
