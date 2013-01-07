/**
 ******************************************************************************
 * @file       sys_gpio.c
 * @version    V0.0.1
 * @brief      gpio模块.
 * @details    This file including all API functions's implement of gpio.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#ifndef __SYS_GPIO_H__
#define __SYS_GPIO_H__
#include <types.h>

/* IO不同类型状态，也许需要根据不同板子用宏定义做相应调整 */
#define E_LED_ON                        1    /* LED点亮              */
#define E_LED_OFF                       0    /* LED熄灭              */
#define E_POWER_OFF                     1    /* 电源关闭             */
#define E_POWER_ON                      0    /* 电源打开             */

/* 输入输出IO */
typedef enum
{
    IO_LED0 = 0,        /* LED0        */
    IO_LED1,            /* LED1        */
    IO_LED2,            /* LED2        */
    IO_LED3,            /* LED3        */
    IO_KEY0,            /* LED2        */
    IO_KEY1,            /* LED3        */
    IO_KEY2,            /* LED3        */
    IO_KEY3,          /* USER BOTTON */
    IO_LCD_AK,
    IO_LCD_RST,         /* LCD RST PIN */
    IO_LCD_CS,
    IO_LCD_CLK,
    IO_LCD_A0,
    IO_LCD_MOSI,
    IO_FLASH_CS,    /* flash片选 */
    IO_SD_CS,       /* SD片选 */
    IO_WLM_CS,       /* 无线片选 */
    IO_WLM_GDO2,
    IO_WLM_GDO0,
    IO_WLM_RXEN,    /* L:发射状态（开功放）H:接收状态/(关功放) */
    IO_MAX_COUNTS,
    IO_ZCP_DET,     /*过零检测*/
} gpio_out_e;

#define IO_NO_SUPPORT                 (-1) /* 无效IO               */

/* 定义GPIO默认模式 */
#define IO_INPUT                        0   /* 输入模式 */
#define IO_OUTPUT0                      1   /* 输出0 */
#define IO_OUTPUT1                      2   /* 输出1 */
#define IO_INT                          3   /* 中断 */

/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
extern int32_t sys_gpio_cfg(int32_t f_iIoNo, uint32_t f_imode);

extern int32_t sys_gpio_read(int32_t f_iIoNo);

extern status_t sys_gpio_write(int32_t f_iIoNo, int32_t f_IoState);

extern void sys_gpio_bspInstall(
        FUNCPTR bsp_gpio_cfg,
        FUNCPTR bsp_gpio_read,
        FUNCPTR bsp_gpio_write);




#endif /* __GPIO_H__ */

/*------------------------------End of gpio.h -------------------------------*/

