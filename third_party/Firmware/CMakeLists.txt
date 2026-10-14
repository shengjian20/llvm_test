# =============================================================================
# GD32F4xx 固件库 CMake 模块
#
# 由工程根目录的 CMakeLists.txt 通过 add_subdirectory(third_party/Firmware) 引入。
#
# 对外提供:
#   目标        : gd32f4xx         (静态库, 含 CMSIS 系统文件 + 全部标准外设库源文件)
#                 gd32f4xx-startup (INTERFACE: 启动文件源码 + 链接脚本 -T 选项,
#                                   应用目标链接它即自动带上启动与链接配置)
#   公共 include: CMSIS 头文件、标准外设库头文件、以及 gd32f4xx_libopt.h 所在目录
#   公共宏      : GD32_DEVICE (由 GD32_PART 推导, 如 GD32F470)
#
# 器件选择 (在 add_subdirectory 之前或 -D 传入):
#   GD32_PART          器件型号短格式, 如 GD32F470VG (必填, 无默认值)
#                      结构: GD32F4<家族两位><引脚字母><容量字母>, 共 10 字符
#                      容量: E=512K  G=1M  I=2M  K=3M
#                      (封装后缀不支持传入, 解析以倒数第二位为容量字母)
#
# 启动文件 / 链接脚本 (纯 C 方案; 官方旧版 .S 启动与 *_flash.ld 已不被本模块引用):
#   1. 显式覆盖: GD32_STARTUP_FILE / GD32_LD_SCRIPT (完整路径)
#   2. 默认: Source/GCC/startup_gd32f4xx.c
#      (单文件, 弱定义/向量表的分组与 gd32f4xx.h 的 IRQn_Type 枚举对称:
#       共享主体 + #if 450/470 → 405/425 → 407/427 三段家族块)
#      + Ld/gd32f4xx_flash_pico.ld.S 模板经 C 预处理器生成的 .ld
#        (注入 -D<GD32_DEVICE> 与 -DGD32F470xG 式容量组合宏,
#         模板内置 Flash/RAM 容量总表)
#
# 可覆盖变量 (在 add_subdirectory 之前或 -D 传入):
#   GD32_PART          器件型号(到容量字母, 封装后缀可省略)          (必填, 无默认值)
#   GD32_DEVICE        编译宏名              (默认按 GD32_PART 推导)
#   GD32_STARTUP_FILE  显式指定启动文件      (默认自动推导)
#   GD32_LD_SCRIPT     显式指定链接脚本      (默认自动推导)
#   GD32_LIBOPT_DIR    gd32f4xx_libopt.h 目录 (默认 ${PROJECT_SOURCE_DIR}/include)
#
# 为什么启动文件走 INTERFACE 目标而非静态库:
#   启动文件中的向量表/Reset_Handler 没有任何外部符号引用者, 若编译进 .a,
#   链接器永远不会把这个归档成员拉出来; 必须作为源码直接参与应用链接。
# =============================================================================

# ---------------- 固件库路径 ----------------
set(FW_ROOT      "${CMAKE_CURRENT_SOURCE_DIR}")
set(FW_CMSIS     "${FW_ROOT}/CMSIS")
set(FW_CMSIS_DEV "${FW_CMSIS}/GD/GD32F4xx")
set(FW_STDPERIPH "${FW_ROOT}/GD32F4xx_standard_peripheral")
set(GD32_GCC_DIR "${FW_CMSIS_DEV}/Source/GCC")

# ---------------- 器件型号解析 (截断 + 替换, 不用正则) ----------------
# GD32_PART 必填 (无默认值): 由使用方在 add_subdirectory 之前显式给出,
# 这里是防御性检查 (直接引入本模块而未经根 CMakeLists 的场景)
if(NOT DEFINED GD32_PART)
    message(FATAL_ERROR
        "GD32F4xx 固件库模块: 未定义 GD32_PART (器件型号(到容量字母, 封装后缀可省略), 如 GD32F470VG)。\n"
        "应由工程根目录在 add_subdirectory 之前设置, 或 cmake -DGD32_PART=... 传入")
endif()

# 1. 校验: 长度与前缀 (最短合法形式 GD32F470VG = 10 字符)
string(LENGTH "${GD32_PART}" _part_len)
if(_part_len LESS 10)
    message(FATAL_ERROR
        "无法解析 GD32_PART='${GD32_PART}'\n"
        "期望形如 GD32F470VG (至少 10 字符): GD32F4 + 家族两位 + 引脚字母 + 容量字母")
endif()
string(SUBSTRING "${GD32_PART}" 0 6 _prefix)
if(NOT _prefix STREQUAL "GD32F4")
    message(FATAL_ERROR
        "无法解析 GD32_PART='${GD32_PART}'\n"
        "前缀必须是 GD32F4")
endif()

# 2. family: 直接取第 7-8 位 (前缀 GD32F4 固定 6 字符之后)
string(SUBSTRING "${GD32_PART}" 6 2 GD32_FAMILY)

# 3. size: 替换掉前 9 位 (前缀 GD32F4 + 家族 + 引脚), 剩余部分即容量字母
#    (恰好是倒数第二位; 若带封装后缀如 VGT6, 剩余为 "GT6" 会被下方校验拒绝)
string(SUBSTRING "${GD32_PART}" 0 9 _head)
string(REPLACE "${_head}" "" _size_rest "${GD32_PART}")
string(LENGTH "${_size_rest}" _size_len)
if(NOT _size_len EQUAL 1)
    message(FATAL_ERROR
        "无法解析 GD32_PART='${GD32_PART}'\n"
        "只支持到容量字母的短格式 (如 GD32F470VG), 封装后缀请省略")
endif()
set(GD32_SIZE_LETTER "${_size_rest}")

# 4. 校验: 家族与容量必须属于启动文件宏分支/链接脚本容量表覆盖范围
set(_valid_families 05 07 25 27 50 70)
set(_valid_sizes    E G I K)
list(FIND _valid_families "${GD32_FAMILY}" _idx)
if(_idx EQUAL -1)
    message(FATAL_ERROR "不支持的 GD32F4 家族: F4${GD32_FAMILY} "
        "(仅支持 05/07/25/27/50/70)")
endif()
list(FIND _valid_sizes "${GD32_SIZE_LETTER}" _idx)
if(_idx EQUAL -1)
    message(FATAL_ERROR "不支持的容量字母: ${GD32_SIZE_LETTER} "
        "(仅支持 E=512K / G=1M / I=2M / K=3M)")
endif()

# 编译宏名 (如 GD32F470)
if(NOT DEFINED GD32_DEVICE)
    set(GD32_DEVICE "GD32F4${GD32_FAMILY}")
endif()

# ---------------- 启动文件解析 ----------------
if(NOT DEFINED GD32_STARTUP_FILE)
    # 家族通用纯 C 启动 (单文件 startup_gd32f4xx.c, 依赖 -D<GD32_DEVICE>
    # 器件宏做条件编译, 分组与 gd32f4xx.h 的 IRQn_Type 枚举对称)
    set(GD32_STARTUP_FILE "${GD32_GCC_DIR}/startup_gd32f4xx.c")
endif()
if(NOT EXISTS "${GD32_STARTUP_FILE}")
    message(FATAL_ERROR "启动文件不存在: ${GD32_STARTUP_FILE}")
endif()

# ---------------- 链接脚本: 模板预处理生成 (U-Boot/Linux 的 .ld.S 方案) ----------------
if(NOT DEFINED GD32_LD_SCRIPT)
    # 单个容量组合宏: 命名沿用 GD 官方 ld 的 x 记法 (gd32f470xG_flash.ld 同源),
    # 模板内用它做 #if 选择; 家族宏 (GD32F470 等) 一并注入便于将来扩展
    set(_size_macro "GD32F4${GD32_FAMILY}x${GD32_SIZE_LETTER}")

    # ---- C 预处理器展开模板 → 生成最终 .ld ----
    set(GD32_LD_TEMPLATE "${GD32_GCC_DIR}/Ld/gd32f4xx_flash_pico.ld.S")
    set(GD32_LD_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/gd32_${GD32_PART}_flash_pico.ld")
    execute_process(
        COMMAND "${CMAKE_C_COMPILER}" -E -P -C -x c
                "-D${GD32_DEVICE}"
                "-D${_size_macro}"
                "${GD32_LD_TEMPLATE}"
                "-o" "${GD32_LD_SCRIPT}"
        RESULT_VARIABLE _ld_cpp_result
    )
    if(NOT _ld_cpp_result EQUAL 0)
        message(FATAL_ERROR "预处理链接脚本模板失败: ${GD32_LD_TEMPLATE}")
    endif()
    # 模板被修改时自动触发重新 configure
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${GD32_LD_TEMPLATE}")

    message(STATUS "GD32 链接脚本  : ${GD32_LD_SCRIPT} (模板生成: 宏 -D${_size_macro})")
else()
    message(STATUS "GD32 链接脚本  : ${GD32_LD_SCRIPT} (显式指定)")
endif()

message(STATUS "GD32 器件      : ${GD32_PART} (宏 ${GD32_DEVICE}, Flash 容量档 ${GD32_SIZE_LETTER})")
message(STATUS "GD32 启动文件  : ${GD32_STARTUP_FILE}")

# ---------------- 固件库静态目标 ----------------
file(GLOB GD32_STDPERIPH_SRC CONFIGURE_DEPENDS "${FW_STDPERIPH}/Source/*.c")

add_library(gd32f4xx STATIC
    ${FW_CMSIS_DEV}/Source/system_gd32f4xx.c
    ${GD32_STDPERIPH_SRC}
)

target_include_directories(gd32f4xx PUBLIC
    ${FW_CMSIS}
    ${FW_CMSIS_DEV}/Include
    ${FW_STDPERIPH}/Include
    ${GD32_LIBOPT_DIR}
)

target_compile_definitions(gd32f4xx PUBLIC ${GD32_DEVICE})
target_compile_options(gd32f4xx PRIVATE -ffunction-sections -fdata-sections)

# ---------------- 启动/链接接口目标 ----------------
# 应用目标 target_link_libraries(... gd32f4xx-startup) 即自动获得:
#   - 启动文件作为源码参与编译 (INTERFACE source)
#   - 链接脚本 -T 选项
add_library(gd32f4xx-startup INTERFACE)
target_sources(gd32f4xx-startup INTERFACE "${GD32_STARTUP_FILE}")
target_link_options(gd32f4xx-startup INTERFACE "-T${GD32_LD_SCRIPT}")
# 生成的 .ld 是链接输入的依赖: 模板变化 → 重新生成 → 触发重新链接
# (没有这一行, 仅模板变化时 CMake 会认为目标无需重链, 产物使用旧脚本)
set_target_properties(gd32f4xx-startup PROPERTIES
    INTERFACE_LINK_DEPENDS "${GD32_LD_SCRIPT}")
