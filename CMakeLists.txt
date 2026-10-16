cmake_minimum_required(VERSION 3.20)

# ---------------- 工具链选择 (首次 configure 时生效) ----------------
# 两种指定方式 (优先级从高到低):
#   1. -DCMAKE_TOOLCHAIN_FILE=<文件>   直接指定任意工具链文件
#   2. -DTOOLCHAIN=atfe|gnu            在预置工具链中选择 (默认 atfe)
#      atfe : ATfE (LLVM/clang + picolibc)      → cmake/llvm.cmake
#      gnu  : GNU Arm Embedded (gcc + picolibc) → cmake/gnu.cmake
# 注意: 工具链在 project() 之后不可更换, 换工具链请使用新的构建目录,
# 或先清空 build/ 再重新 configure
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(NOT DEFINED TOOLCHAIN)
        set(TOOLCHAIN "atfe")
    endif()
    if(TOOLCHAIN STREQUAL "atfe")
        set(_toolchain_file "cmake/llvm.cmake")
    elseif(TOOLCHAIN STREQUAL "gnu")
        set(_toolchain_file "cmake/gnu.cmake")
    else()
        message(FATAL_ERROR
            "未知工具链 TOOLCHAIN='${TOOLCHAIN}'\n"
            "可选值: atfe (ATfE/clang, 默认) | gnu (GNU/gcc)")
    endif()
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${_toolchain_file}"
        CACHE FILEPATH "交叉编译工具链文件")
endif()

project(gd32f470_blink C ASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)   # 可选：标准不可用则报错
set(CMAKE_C_EXTENSIONS ON)          # 默认就是 ON，生成 -std=gnu23

# 生成 build/compile_commands.json 供 clangd 等工具使用 (见 .clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------- 引入 GD32F4xx 固件库模块 ----------------
# 模块位于 third_party/Firmware/CMakeLists.txt, 对外提供:
#   gd32f4xx         静态库 (CMSIS + 标准外设库)
#   gd32f4xx-startup INTERFACE: 启动文件 + 链接脚本 -T (随器件自动选择)
# 必须显式指定器件型号(到容量字母, 封装后缀可省略) (无默认值, 防止误选芯片):
#   方式一: 在本文件 add_subdirectory 之前 set(GD32_PART "GD32F470VG")
#   方式二: cmake -DGD32_PART=GD32F470VG ...
if(NOT DEFINED GD32_PART)
    message(FATAL_ERROR
        "未指定 GD32_PART (器件型号(到容量字母, 封装后缀可省略), 如 GD32F470VG)。\n"
        "请在本文件 add_subdirectory(third_party/Firmware) 之前添加:\n"
        "    set(GD32_PART \"GD32F470VG\")\n"
        "或在 cmake 命令行传入: cmake -DGD32_PART=GD32F470VG ...")
endif()
add_subdirectory(third_party/Firmware)

# ---------------- RT-Thread (可选; 仅内核适配) ----------------
# 启用: -DRTOS_RTTHREAD=ON 或取消下行注释; 目标名 RTOS::rtthread
# set(RTOS_RTTHREAD ON)
add_subdirectory(third_party/rtos)

# ---------------- 应用目标 ----------------
add_executable(${PROJECT_NAME}
    src/main.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections -g -O2)

# 仅对工程自身的源码开启告警, 避免固件库告警刷屏
set_source_files_properties(src/main.c PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra")

# 链接固件库 (其 PUBLIC include / 宏会自动传递给本目标)
# gd32f4xx-startup : 启动文件源码 + 链接脚本 -T (由 GD32_PART 自动选择)
# crt0             : picolibc 的 C 运行时启动 (提供 _start, 由启动文件调用)
target_link_libraries(${PROJECT_NAME} PRIVATE gd32f4xx gd32f4xx-startup crt0)

target_link_options(${PROJECT_NAME} PRIVATE
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
)

# ---------------- 生成 hex / bin 并打印尺寸 ----------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Generating ${PROJECT_NAME}.hex / ${PROJECT_NAME}.bin"
)
