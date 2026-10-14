/*!
    \file    gd32f4xx_libopt.h
    \brief   GD32F4xx 标准外设库可选配置头文件

    该文件由 gd32f4xx.h 在定义了 USE_STDPERIPH_DRIVER 时自动包含,
    用于选择需要引入的外设驱动头文件。

    说明: 本工程实际编译的外设驱动只有 GPIO 与 RCU, 但 system_gd32f4xx.c
    在配置 240MHz 时钟时还会用到 PMU(高驱动模式) 与 MISC(向量表重定位),
    因此这里按固件库惯例引入 GD32F470 的全部标准外设头文件。
*/

#ifndef GD32F4XX_LIBOPT_H
#define GD32F4XX_LIBOPT_H

#if defined (GD32F450) || defined (GD32F405) || defined (GD32F407) || defined (GD32F470) || defined (GD32F425) || defined (GD32F427)
    #include "gd32f4xx_rcu.h"
    #include "gd32f4xx_adc.h"
    #include "gd32f4xx_can.h"
    #include "gd32f4xx_crc.h"
    #include "gd32f4xx_ctc.h"
    #include "gd32f4xx_dac.h"
    #include "gd32f4xx_dbg.h"
    #include "gd32f4xx_dci.h"
    #include "gd32f4xx_dma.h"
    #include "gd32f4xx_exti.h"
    #include "gd32f4xx_fmc.h"
    #include "gd32f4xx_fwdgt.h"
    #include "gd32f4xx_gpio.h"
    #include "gd32f4xx_syscfg.h"
    #include "gd32f4xx_i2c.h"
    #include "gd32f4xx_iref.h"
    #include "gd32f4xx_pmu.h"
    #include "gd32f4xx_rtc.h"
    #include "gd32f4xx_sdio.h"
    #include "gd32f4xx_spi.h"
    #include "gd32f4xx_timer.h"
    #include "gd32f4xx_trng.h"
    #include "gd32f4xx_usart.h"
    #include "gd32f4xx_wwdgt.h"
    #include "gd32f4xx_misc.h"
#endif

#if defined (GD32F450) || defined (GD32F470)
    #include "gd32f4xx_enet.h"
    #include "gd32f4xx_exmc.h"
    #include "gd32f4xx_ipa.h"
    #include "gd32f4xx_tli.h"
#endif

#endif /* GD32F4XX_LIBOPT_H */
