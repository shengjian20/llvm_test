/*!
    \file    startup_gd32f4xx.c
    \brief   GD32F4xx 家族通用纯 C 启动文件 (分组与 gd32f4xx.h 的 IRQn_Type 枚举对称)

    一份源文件覆盖 GD32F405/407/425/427/450/470 全部家族:
      - 弱定义与向量表按 gd32f4xx.h 中 IRQn_Type 枚举的分组方式组织:
            共享主体       : 内核异常 + IRQ 0-47
            #if 450/470   : IRQ 48-90  (EXMC..IPA, 全量外设)
            #if 405/425   : IRQ 49-81  (无 EXMC/ENET, 对应槽位保留 0)
            #if 407/427   : IRQ 48-81  (含 EXMC/ENET)
        三个 #if 块的顺序与枚举中的出现顺序一致; 每次编译只激活一个块
      - 向量表与官方 GCC 汇编启动 startup_gd32f<家族>.S 逐项一致
      - 中断处理函数以弱定义提供, 可被应用层同名函数覆盖

    启动流程:
      上电复位
        -> Cortex-M 硬件从向量表 (.vectors) 读取初始 MSP 与复位入口
        -> Reset_Handler (本文件): 尾调用 _start
        -> _start (picolibc crt0): 使能 FPU / 初始化 TLS / 复制 .data /
           清零 .bss / __libc_init_array / 调用 main
        -> main()
           (SystemInit 时钟等芯片级初始化由应用在 main 开头自行调用)

    需搭配 *_flash_pico.ld 链接脚本 (提供 _sp 与 picolibc crt0 符号)。
*/

#include <stdint.h>

/* 器件宏防呆 (与 gd32f4xx.h 相同的检查方式) */
#if !defined (GD32F450)  && !defined (GD32F405) && !defined (GD32F407) && \
    !defined (GD32F470)  && !defined (GD32F425) && !defined (GD32F427)
#error "startup_gd32f4xx.c: 未定义器件宏, 需要 GD32F405/407/425/427/450/470 之一 (CMake GD32_PART 自动提供)"
#endif

/* picolibc crt0 提供的 C 运行时入口 */
extern void _start(void);

/* 链接脚本提供的栈顶地址 */
extern uint32_t _sp;

/* 链接脚本提供的 .tcmram 段位置 (见 gd32f4xx_flash_pico.ld.S) */
extern uint32_t __tcmram_source;   /* Flash 中的初始数据 (LMA) */
extern uint32_t __tcmram_start;    /* TCMRAM 中的运行地址 (VMA) */
extern uint32_t __tcmram_end;

/*!
    \brief      默认中断处理函数 (死循环)
*/
void Default_Handler(void)
{
    for(;;) {
    }
}

#define GD32_WEAK_ISR(name)  void name(void) __attribute__((weak, alias("Default_Handler")))

/* ---------------- 中断弱定义 (分组与 IRQn_Type 枚举对称) ---------------- */

/* 共享: 内核异常 + IRQ 0-47 */
GD32_WEAK_ISR(NMI_Handler);
GD32_WEAK_ISR(HardFault_Handler);
GD32_WEAK_ISR(MemManage_Handler);
GD32_WEAK_ISR(BusFault_Handler);
GD32_WEAK_ISR(UsageFault_Handler);
GD32_WEAK_ISR(SVC_Handler);
GD32_WEAK_ISR(DebugMon_Handler);
GD32_WEAK_ISR(PendSV_Handler);
GD32_WEAK_ISR(SysTick_Handler);
GD32_WEAK_ISR(WWDGT_IRQHandler);
GD32_WEAK_ISR(LVD_IRQHandler);
GD32_WEAK_ISR(TAMPER_STAMP_IRQHandler);
GD32_WEAK_ISR(RTC_WKUP_IRQHandler);
GD32_WEAK_ISR(FMC_IRQHandler);
GD32_WEAK_ISR(RCU_CTC_IRQHandler);
GD32_WEAK_ISR(EXTI0_IRQHandler);
GD32_WEAK_ISR(EXTI1_IRQHandler);
GD32_WEAK_ISR(EXTI2_IRQHandler);
GD32_WEAK_ISR(EXTI3_IRQHandler);
GD32_WEAK_ISR(EXTI4_IRQHandler);
GD32_WEAK_ISR(DMA0_Channel0_IRQHandler);
GD32_WEAK_ISR(DMA0_Channel1_IRQHandler);
GD32_WEAK_ISR(DMA0_Channel2_IRQHandler);
GD32_WEAK_ISR(DMA0_Channel3_IRQHandler);
GD32_WEAK_ISR(DMA0_Channel4_IRQHandler);
GD32_WEAK_ISR(DMA0_Channel5_IRQHandler);
GD32_WEAK_ISR(DMA0_Channel6_IRQHandler);
GD32_WEAK_ISR(ADC_IRQHandler);
GD32_WEAK_ISR(CAN0_TX_IRQHandler);
GD32_WEAK_ISR(CAN0_RX0_IRQHandler);
GD32_WEAK_ISR(CAN0_RX1_IRQHandler);
GD32_WEAK_ISR(CAN0_EWMC_IRQHandler);
GD32_WEAK_ISR(EXTI5_9_IRQHandler);
GD32_WEAK_ISR(TIMER0_BRK_TIMER8_IRQHandler);
GD32_WEAK_ISR(TIMER0_UP_TIMER9_IRQHandler);
GD32_WEAK_ISR(TIMER0_TRG_CMT_TIMER10_IRQHandler);
GD32_WEAK_ISR(TIMER0_Channel_IRQHandler);
GD32_WEAK_ISR(TIMER1_IRQHandler);
GD32_WEAK_ISR(TIMER2_IRQHandler);
GD32_WEAK_ISR(TIMER3_IRQHandler);
GD32_WEAK_ISR(I2C0_EV_IRQHandler);
GD32_WEAK_ISR(I2C0_ER_IRQHandler);
GD32_WEAK_ISR(I2C1_EV_IRQHandler);
GD32_WEAK_ISR(I2C1_ER_IRQHandler);
GD32_WEAK_ISR(SPI0_IRQHandler);
GD32_WEAK_ISR(SPI1_IRQHandler);
GD32_WEAK_ISR(USART0_IRQHandler);
GD32_WEAK_ISR(USART1_IRQHandler);
GD32_WEAK_ISR(USART2_IRQHandler);
GD32_WEAK_ISR(EXTI10_15_IRQHandler);
GD32_WEAK_ISR(RTC_Alarm_IRQHandler);
GD32_WEAK_ISR(USBFS_WKUP_IRQHandler);
GD32_WEAK_ISR(TIMER7_BRK_TIMER11_IRQHandler);
GD32_WEAK_ISR(TIMER7_UP_TIMER12_IRQHandler);
GD32_WEAK_ISR(TIMER7_TRG_CMT_TIMER13_IRQHandler);
GD32_WEAK_ISR(TIMER7_Channel_IRQHandler);
GD32_WEAK_ISR(DMA0_Channel7_IRQHandler);

#if defined (GD32F450) || defined (GD32F470)
/* GD32F450/470: IRQ 48-90 (EXMC..IPA) */
GD32_WEAK_ISR(EXMC_IRQHandler);
GD32_WEAK_ISR(SDIO_IRQHandler);
GD32_WEAK_ISR(TIMER4_IRQHandler);
GD32_WEAK_ISR(SPI2_IRQHandler);
GD32_WEAK_ISR(UART3_IRQHandler);
GD32_WEAK_ISR(UART4_IRQHandler);
GD32_WEAK_ISR(TIMER5_DAC_IRQHandler);
GD32_WEAK_ISR(TIMER6_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel0_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel1_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel2_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel3_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel4_IRQHandler);
GD32_WEAK_ISR(ENET_IRQHandler);
GD32_WEAK_ISR(ENET_WKUP_IRQHandler);
GD32_WEAK_ISR(CAN1_TX_IRQHandler);
GD32_WEAK_ISR(CAN1_RX0_IRQHandler);
GD32_WEAK_ISR(CAN1_RX1_IRQHandler);
GD32_WEAK_ISR(CAN1_EWMC_IRQHandler);
GD32_WEAK_ISR(USBFS_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel5_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel6_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel7_IRQHandler);
GD32_WEAK_ISR(USART5_IRQHandler);
GD32_WEAK_ISR(I2C2_EV_IRQHandler);
GD32_WEAK_ISR(I2C2_ER_IRQHandler);
GD32_WEAK_ISR(USBHS_EP1_Out_IRQHandler);
GD32_WEAK_ISR(USBHS_EP1_In_IRQHandler);
GD32_WEAK_ISR(USBHS_WKUP_IRQHandler);
GD32_WEAK_ISR(USBHS_IRQHandler);
GD32_WEAK_ISR(DCI_IRQHandler);
GD32_WEAK_ISR(TRNG_IRQHandler);
GD32_WEAK_ISR(FPU_IRQHandler);
GD32_WEAK_ISR(UART6_IRQHandler);
GD32_WEAK_ISR(UART7_IRQHandler);
GD32_WEAK_ISR(SPI3_IRQHandler);
GD32_WEAK_ISR(SPI4_IRQHandler);
GD32_WEAK_ISR(SPI5_IRQHandler);
GD32_WEAK_ISR(TLI_IRQHandler);
GD32_WEAK_ISR(TLI_ER_IRQHandler);
GD32_WEAK_ISR(IPA_IRQHandler);
#endif /* GD32F450 and GD32F470 */

#if defined (GD32F405) || defined (GD32F425)
/* GD32F405/425: IRQ 49-81 (无 EXMC/ENET) */
GD32_WEAK_ISR(SDIO_IRQHandler);
GD32_WEAK_ISR(TIMER4_IRQHandler);
GD32_WEAK_ISR(SPI2_IRQHandler);
GD32_WEAK_ISR(UART3_IRQHandler);
GD32_WEAK_ISR(UART4_IRQHandler);
GD32_WEAK_ISR(TIMER5_DAC_IRQHandler);
GD32_WEAK_ISR(TIMER6_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel0_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel1_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel2_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel3_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel4_IRQHandler);
GD32_WEAK_ISR(CAN1_TX_IRQHandler);
GD32_WEAK_ISR(CAN1_RX0_IRQHandler);
GD32_WEAK_ISR(CAN1_RX1_IRQHandler);
GD32_WEAK_ISR(CAN1_EWMC_IRQHandler);
GD32_WEAK_ISR(USBFS_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel5_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel6_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel7_IRQHandler);
GD32_WEAK_ISR(USART5_IRQHandler);
GD32_WEAK_ISR(I2C2_EV_IRQHandler);
GD32_WEAK_ISR(I2C2_ER_IRQHandler);
GD32_WEAK_ISR(USBHS_EP1_Out_IRQHandler);
GD32_WEAK_ISR(USBHS_EP1_In_IRQHandler);
GD32_WEAK_ISR(USBHS_WKUP_IRQHandler);
GD32_WEAK_ISR(USBHS_IRQHandler);
GD32_WEAK_ISR(DCI_IRQHandler);
GD32_WEAK_ISR(TRNG_IRQHandler);
GD32_WEAK_ISR(FPU_IRQHandler);
#endif /* GD32F405 and GD32F425 */

#if defined (GD32F407) || defined (GD32F427)
/* GD32F407/427: IRQ 48-81 (含 EXMC/ENET) */
GD32_WEAK_ISR(EXMC_IRQHandler);
GD32_WEAK_ISR(SDIO_IRQHandler);
GD32_WEAK_ISR(TIMER4_IRQHandler);
GD32_WEAK_ISR(SPI2_IRQHandler);
GD32_WEAK_ISR(UART3_IRQHandler);
GD32_WEAK_ISR(UART4_IRQHandler);
GD32_WEAK_ISR(TIMER5_DAC_IRQHandler);
GD32_WEAK_ISR(TIMER6_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel0_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel1_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel2_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel3_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel4_IRQHandler);
GD32_WEAK_ISR(ENET_IRQHandler);
GD32_WEAK_ISR(ENET_WKUP_IRQHandler);
GD32_WEAK_ISR(CAN1_TX_IRQHandler);
GD32_WEAK_ISR(CAN1_RX0_IRQHandler);
GD32_WEAK_ISR(CAN1_RX1_IRQHandler);
GD32_WEAK_ISR(CAN1_EWMC_IRQHandler);
GD32_WEAK_ISR(USBFS_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel5_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel6_IRQHandler);
GD32_WEAK_ISR(DMA1_Channel7_IRQHandler);
GD32_WEAK_ISR(USART5_IRQHandler);
GD32_WEAK_ISR(I2C2_EV_IRQHandler);
GD32_WEAK_ISR(I2C2_ER_IRQHandler);
GD32_WEAK_ISR(USBHS_EP1_Out_IRQHandler);
GD32_WEAK_ISR(USBHS_EP1_In_IRQHandler);
GD32_WEAK_ISR(USBHS_WKUP_IRQHandler);
GD32_WEAK_ISR(USBHS_IRQHandler);
GD32_WEAK_ISR(DCI_IRQHandler);
GD32_WEAK_ISR(TRNG_IRQHandler);
GD32_WEAK_ISR(FPU_IRQHandler);
#endif /* GD32F407 and GD32F427 */

/*!
    \brief      复位处理函数 (C 实现)
*/
void Reset_Handler(void)
{
    /* 手动初始化 TCMRAM: crt0 只负责 .data/.bss, 不认识 .tcmram 段。
       语义同 .data: 把 Flash 中的初始镜像拷到 TCMRAM 运行地址。
       未使用 .tcmram 时段大小为 0, 循环零次直接跳过 */
    uint32_t *dst = &__tcmram_start;
    uint32_t *src = &__tcmram_source;
    while (dst < &__tcmram_end) {
        *dst++ = *src++;
    }

    /* 尾调用 picolibc crt0: 完成 C 运行时初始化并调用 main (不会返回) */
    _start();
}

/* ---------------- 中断向量表 (分组与 IRQn_Type 枚举对称) ---------------- */
__attribute__((section(".vectors"), used))
void (* const __gVectors[])(void) = {
    (void (*)(void))(uintptr_t)&_sp,   /* 0: 初始主栈指针 MSP */
    Reset_Handler,                     /* 1: 复位入口 */

    /* cortex-M4 exceptions (对应枚举 -14..-1) */
    NMI_Handler,                         /* 2 */
    HardFault_Handler,                   /* 3 */
    MemManage_Handler,                   /* 4 */
    BusFault_Handler,                    /* 5 */
    UsageFault_Handler,                  /* 6 */
    0,                                 /* 7: 保留 */
    0,                                 /* 8: 保留 */
    0,                                 /* 9: 保留 */
    0,                                 /* 10: 保留 */
    SVC_Handler,                         /* 11 */
    DebugMon_Handler,                    /* 12 */
    0,                                 /* 13: 保留 */
    PendSV_Handler,                      /* 14 */
    SysTick_Handler,                     /* 15 */

    /* IRQ 0-47: 全家族共享 (对应枚举共享部分) */
    WWDGT_IRQHandler,                    /* 16 */
    LVD_IRQHandler,                      /* 17 */
    TAMPER_STAMP_IRQHandler,             /* 18 */
    RTC_WKUP_IRQHandler,                 /* 19 */
    FMC_IRQHandler,                      /* 20 */
    RCU_CTC_IRQHandler,                  /* 21 */
    EXTI0_IRQHandler,                    /* 22 */
    EXTI1_IRQHandler,                    /* 23 */
    EXTI2_IRQHandler,                    /* 24 */
    EXTI3_IRQHandler,                    /* 25 */
    EXTI4_IRQHandler,                    /* 26 */
    DMA0_Channel0_IRQHandler,            /* 27 */
    DMA0_Channel1_IRQHandler,            /* 28 */
    DMA0_Channel2_IRQHandler,            /* 29 */
    DMA0_Channel3_IRQHandler,            /* 30 */
    DMA0_Channel4_IRQHandler,            /* 31 */
    DMA0_Channel5_IRQHandler,            /* 32 */
    DMA0_Channel6_IRQHandler,            /* 33 */
    ADC_IRQHandler,                      /* 34 */
    CAN0_TX_IRQHandler,                  /* 35 */
    CAN0_RX0_IRQHandler,                 /* 36 */
    CAN0_RX1_IRQHandler,                 /* 37 */
    CAN0_EWMC_IRQHandler,                /* 38 */
    EXTI5_9_IRQHandler,                  /* 39 */
    TIMER0_BRK_TIMER8_IRQHandler,        /* 40 */
    TIMER0_UP_TIMER9_IRQHandler,         /* 41 */
    TIMER0_TRG_CMT_TIMER10_IRQHandler,   /* 42 */
    TIMER0_Channel_IRQHandler,           /* 43 */
    TIMER1_IRQHandler,                   /* 44 */
    TIMER2_IRQHandler,                   /* 45 */
    TIMER3_IRQHandler,                   /* 46 */
    I2C0_EV_IRQHandler,                  /* 47 */
    I2C0_ER_IRQHandler,                  /* 48 */
    I2C1_EV_IRQHandler,                  /* 49 */
    I2C1_ER_IRQHandler,                  /* 50 */
    SPI0_IRQHandler,                     /* 51 */
    SPI1_IRQHandler,                     /* 52 */
    USART0_IRQHandler,                   /* 53 */
    USART1_IRQHandler,                   /* 54 */
    USART2_IRQHandler,                   /* 55 */
    EXTI10_15_IRQHandler,                /* 56 */
    RTC_Alarm_IRQHandler,                /* 57 */
    USBFS_WKUP_IRQHandler,               /* 58 */
    TIMER7_BRK_TIMER11_IRQHandler,       /* 59 */
    TIMER7_UP_TIMER12_IRQHandler,        /* 60 */
    TIMER7_TRG_CMT_TIMER13_IRQHandler,   /* 61 */
    TIMER7_Channel_IRQHandler,           /* 62 */
    DMA0_Channel7_IRQHandler,            /* 63 */

#if defined (GD32F450) || defined (GD32F470)
    /* GD32F450/470: IRQ 48-90 (EXMC..IPA) */
    EXMC_IRQHandler,                     /* 64 */
    SDIO_IRQHandler,                     /* 65 */
    TIMER4_IRQHandler,                   /* 66 */
    SPI2_IRQHandler,                     /* 67 */
    UART3_IRQHandler,                    /* 68 */
    UART4_IRQHandler,                    /* 69 */
    TIMER5_DAC_IRQHandler,               /* 70 */
    TIMER6_IRQHandler,                   /* 71 */
    DMA1_Channel0_IRQHandler,            /* 72 */
    DMA1_Channel1_IRQHandler,            /* 73 */
    DMA1_Channel2_IRQHandler,            /* 74 */
    DMA1_Channel3_IRQHandler,            /* 75 */
    DMA1_Channel4_IRQHandler,            /* 76 */
    ENET_IRQHandler,                     /* 77 */
    ENET_WKUP_IRQHandler,                /* 78 */
    CAN1_TX_IRQHandler,                  /* 79 */
    CAN1_RX0_IRQHandler,                 /* 80 */
    CAN1_RX1_IRQHandler,                 /* 81 */
    CAN1_EWMC_IRQHandler,                /* 82 */
    USBFS_IRQHandler,                    /* 83 */
    DMA1_Channel5_IRQHandler,            /* 84 */
    DMA1_Channel6_IRQHandler,            /* 85 */
    DMA1_Channel7_IRQHandler,            /* 86 */
    USART5_IRQHandler,                   /* 87 */
    I2C2_EV_IRQHandler,                  /* 88 */
    I2C2_ER_IRQHandler,                  /* 89 */
    USBHS_EP1_Out_IRQHandler,            /* 90 */
    USBHS_EP1_In_IRQHandler,             /* 91 */
    USBHS_WKUP_IRQHandler,               /* 92 */
    USBHS_IRQHandler,                    /* 93 */
    DCI_IRQHandler,                      /* 94 */
    0,                                 /* 95: 保留 */
    TRNG_IRQHandler,                     /* 96 */
    FPU_IRQHandler,                      /* 97 */
    UART6_IRQHandler,                    /* 98 */
    UART7_IRQHandler,                    /* 99 */
    SPI3_IRQHandler,                     /* 100 */
    SPI4_IRQHandler,                     /* 101 */
    SPI5_IRQHandler,                     /* 102 */
    0,                                 /* 103: 保留 */
    TLI_IRQHandler,                      /* 104 */
    TLI_ER_IRQHandler,                   /* 105 */
    IPA_IRQHandler,                      /* 106 */
#endif /* GD32F450 and GD32F470 */

#if defined (GD32F405) || defined (GD32F425)
    /* GD32F405/425: IRQ 49-81 (无 EXMC/ENET, 对应槽位保留 0) */
    0,                                 /* 64: 保留 */
    SDIO_IRQHandler,                     /* 65 */
    TIMER4_IRQHandler,                   /* 66 */
    SPI2_IRQHandler,                     /* 67 */
    UART3_IRQHandler,                    /* 68 */
    UART4_IRQHandler,                    /* 69 */
    TIMER5_DAC_IRQHandler,               /* 70 */
    TIMER6_IRQHandler,                   /* 71 */
    DMA1_Channel0_IRQHandler,            /* 72 */
    DMA1_Channel1_IRQHandler,            /* 73 */
    DMA1_Channel2_IRQHandler,            /* 74 */
    DMA1_Channel3_IRQHandler,            /* 75 */
    DMA1_Channel4_IRQHandler,            /* 76 */
    0,                                 /* 77: 保留 */
    0,                                 /* 78: 保留 */
    CAN1_TX_IRQHandler,                  /* 79 */
    CAN1_RX0_IRQHandler,                 /* 80 */
    CAN1_RX1_IRQHandler,                 /* 81 */
    CAN1_EWMC_IRQHandler,                /* 82 */
    USBFS_IRQHandler,                    /* 83 */
    DMA1_Channel5_IRQHandler,            /* 84 */
    DMA1_Channel6_IRQHandler,            /* 85 */
    DMA1_Channel7_IRQHandler,            /* 86 */
    USART5_IRQHandler,                   /* 87 */
    I2C2_EV_IRQHandler,                  /* 88 */
    I2C2_ER_IRQHandler,                  /* 89 */
    USBHS_EP1_Out_IRQHandler,            /* 90 */
    USBHS_EP1_In_IRQHandler,             /* 91 */
    USBHS_WKUP_IRQHandler,               /* 92 */
    USBHS_IRQHandler,                    /* 93 */
    DCI_IRQHandler,                      /* 94 */
    0,                                 /* 95: 保留 */
    TRNG_IRQHandler,                     /* 96 */
    FPU_IRQHandler,                      /* 97 */
#endif /* GD32F405 and GD32F425 */

#if defined (GD32F407) || defined (GD32F427)
    /* GD32F407/427: IRQ 48-81 (含 EXMC/ENET) */
    EXMC_IRQHandler,                     /* 64 */
    SDIO_IRQHandler,                     /* 65 */
    TIMER4_IRQHandler,                   /* 66 */
    SPI2_IRQHandler,                     /* 67 */
    UART3_IRQHandler,                    /* 68 */
    UART4_IRQHandler,                    /* 69 */
    TIMER5_DAC_IRQHandler,               /* 70 */
    TIMER6_IRQHandler,                   /* 71 */
    DMA1_Channel0_IRQHandler,            /* 72 */
    DMA1_Channel1_IRQHandler,            /* 73 */
    DMA1_Channel2_IRQHandler,            /* 74 */
    DMA1_Channel3_IRQHandler,            /* 75 */
    DMA1_Channel4_IRQHandler,            /* 76 */
    ENET_IRQHandler,                     /* 77 */
    ENET_WKUP_IRQHandler,                /* 78 */
    CAN1_TX_IRQHandler,                  /* 79 */
    CAN1_RX0_IRQHandler,                 /* 80 */
    CAN1_RX1_IRQHandler,                 /* 81 */
    CAN1_EWMC_IRQHandler,                /* 82 */
    USBFS_IRQHandler,                    /* 83 */
    DMA1_Channel5_IRQHandler,            /* 84 */
    DMA1_Channel6_IRQHandler,            /* 85 */
    DMA1_Channel7_IRQHandler,            /* 86 */
    USART5_IRQHandler,                   /* 87 */
    I2C2_EV_IRQHandler,                  /* 88 */
    I2C2_ER_IRQHandler,                  /* 89 */
    USBHS_EP1_Out_IRQHandler,            /* 90 */
    USBHS_EP1_In_IRQHandler,             /* 91 */
    USBHS_WKUP_IRQHandler,               /* 92 */
    USBHS_IRQHandler,                    /* 93 */
    DCI_IRQHandler,                      /* 94 */
    0,                                 /* 95: 保留 */
    TRNG_IRQHandler,                     /* 96 */
    FPU_IRQHandler,                      /* 97 */
#endif /* GD32F407 and GD32F427 */
};
