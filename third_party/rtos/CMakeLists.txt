# =============================================================================
# third_party/rtos — RTOS 集成入口
#
# 当前仅集成 RT-Thread v5.3.0 (仅内核适配)。
# 用法: 根工程 -DRTOS_RTTHREAD=ON (或取消根 CMakeLists 注释)
#       target_link_libraries(<app> PRIVATE RTOS::rtthread)
#
# 历史: 曾并行集成 FreeRTOS/ThreadX/µC-OS-III 并做四选一互斥,
# 现收敛为仅 RT-Thread (见提交历史 b54f61ca 之前的四包存档)。
# =============================================================================

set(_rtos_enabled "")
if(RTOS_RTTHREAD)
    list(APPEND _rtos_enabled "rt-thread")
endif()

if(RTOS_RTTHREAD)
    add_subdirectory(rt-thread)
endif()
