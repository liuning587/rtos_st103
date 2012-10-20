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
    IO_LED4,            /* LED2        */
    IO_LED5,            /* LED3        */
    IO_LED6,            /* LED3        */
    IO_BUTTON,          /* USER BOTTON */
    IO_MAX_COUNTS
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
