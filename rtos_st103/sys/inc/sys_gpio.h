/**
 ******************************************************************************
 * @file       sys_gpio.c
 * @version    V0.0.1
 * @brief      gpioģ��.
 * @details    This file including all API functions's implement of gpio.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#ifndef __SYS_GPIO_H__
#define __SYS_GPIO_H__
#include <types.h>

/* IO��ͬ����״̬��Ҳ����Ҫ���ݲ�ͬ�����ú궨������Ӧ���� */
#define E_LED_ON                        1    /* LED����              */
#define E_LED_OFF                       0    /* LEDϨ��              */
#define E_POWER_OFF                     1    /* ��Դ�ر�             */
#define E_POWER_ON                      0    /* ��Դ��             */

/* �������IO */
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
    IO_FLASH_CS,    /* flashƬѡ */
    IO_SD_CS,       /* SDƬѡ */
    IO_WLM_CS,       /* ����Ƭѡ */
    IO_WLM_GDO2,
    IO_WLM_GDO0,
    IO_WLM_RXEN,    /* L:����״̬�������ţ�H:����״̬/(�ع���) */
    IO_MAX_COUNTS,
    IO_ZCP_DET,     /*������*/
} gpio_out_e;

#define IO_NO_SUPPORT                 (-1) /* ��ЧIO               */

/* ����GPIOĬ��ģʽ */
#define IO_INPUT                        0   /* ����ģʽ */
#define IO_OUTPUT0                      1   /* ���0 */
#define IO_OUTPUT1                      2   /* ���1 */
#define IO_INT                          3   /* �ж� */

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

