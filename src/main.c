/*!
    \file    main.c
    \brief   GD32F470VGT6 最简点灯程序

    功能: 让连接在 LED 引脚上的发光二极管以 500ms 的周期闪烁。

    时钟: 使用固件库默认的 system_gd32f4xx.c 配置
          (GD32F470 默认走 25MHz 外部晶振 HXTAL, PLL 倍频到 240MHz)。
          若你的板子没有 25MHz 晶振, 请修改:
          third_party/.../CMSIS/GD/GD32F4xx/Source/system_gd32f4xx.c
          中 "select a system clock" 处的宏选择。

    LED : 默认使用 PE3, 若与你的板子不符, 只需修改下面三个宏即可。
*/

#include "gd32f4xx.h"
#include <math.h>
#include <stdio.h>
#include <uchar.h>

/* ---------------- 板级配置: 按实际硬件修改 ---------------- */
#define LED_GPIO_PORT   GPIOD               /* LED 所在端口 */
#define LED_GPIO_PIN    GPIO_PIN_3          /* LED 所在引脚 */
#define LED_RCU_CLOCK   RCU_GPIOD           /* 该端口对应的 RCU 时钟 */
#define LED_TOGGLE_MS   500U                /* 亮/灭 各持续时间 (毫秒) */

/* 由 SysTick 每 1ms 自增一次的毫秒计数 */
static volatile uint32_t s_ms_tick = 0U;

/*!
    \brief      SysTick 中断服务函数 (覆盖启动文件中的弱定义)
*/
void SysTick_Handler(void)
{
    s_ms_tick++;
}

/*!
    \brief      基于 SysTick 的毫秒级阻塞延时
    \param[in]  ms: 延时毫秒数
*/
static void delay_ms(uint32_t ms)
{
    uint32_t start = s_ms_tick;
    while((s_ms_tick - start) < ms) {
        /* 忙等待 */
    }
}

/*!
    \brief      初始化 LED 引脚为推挽输出
*/
static void led_init(void)
{
    /* 使能 LED 端口时钟 */
    rcu_periph_clock_enable(LED_RCU_CLOCK);

    /* 配置为输出模式, 无上下拉 */
    gpio_mode_set(LED_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GPIO_PIN);

    /* 配置为推挽输出, 速度 50MHz */
    gpio_output_options_set(LED_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_GPIO_PIN);

    /* 默认熄灭 */
    gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);
}

/*!
    \brief      主函数
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* 芯片级初始化: 系统时钟 (IRC16M -> 240MHz PLL) 与向量表偏移 */
    SystemInit();
    
    led_init();

    /* 配置 SysTick 为 1ms 中断一次 */
    SysTick_Config(SystemCoreClock / 1000U);

    while(1) {
        gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);      /* 点亮 LED */
        delay_ms(LED_TOGGLE_MS);
        gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);    /* 熄灭 LED */
        delay_ms(LED_TOGGLE_MS);
    }
}
