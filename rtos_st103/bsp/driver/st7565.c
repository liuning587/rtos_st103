/**
 ******************************************************************************
 * @file       st7565.c
 * @brief      LCD驱动
 * @details    This file including all API functions's implement of dps.
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <sched.h>
#include <sys_gpio.h>
#include <gui.h>
#include "spi.h"

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
/**
 * LCD的背光
 */
#define LCD_EL_ON()     ((void)sys_gpio_write(IO_LCD_AK, 1u))  /**< 背光打开*/
#define LCD_EL_OFF()    ((void)sys_gpio_write(IO_LCD_AK, 0u))  /**< 背光关闭*/

/**
 * LCD复位管脚
 */
#define LCD_RESET_ON()  ((void)sys_gpio_write(IO_LCD_RST, 0u)) /**< 复位拉低*/
#define LCD_RESET_OFF() ((void)sys_gpio_write(IO_LCD_RST, 1u)) /**< 复位拉高*/

/**
 * LCD地址线
 */
#define LCD_A0_H()      ((void)sys_gpio_write(IO_LCD_A0, 1u))  /**< 数据模式*/
#define LCD_A0_L()      ((void)sys_gpio_write(IO_LCD_A0, 0u))  /**< 指令模式*/

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
typedef struct
{
    spi_opt_t* pdev;
    SEM_ID lcd_sem;
} st7565_dev_t;

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 -----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Global Variables
 -----------------------------------------------------------------------------*/
const uint8_t the_logo[8][16] =
{
    {0x00,0x80,0x60,0xF8,0x07,0x40,0x20,0x18,0x0F,0x08,0xC8,0x08,0x08,0x28,0x18,0x00},
    {0x01,0x00,0x00,0xFF,0x00,0x10,0x0C,0x03,0x40,0x80,0x7F,0x00,0x01,0x06,0x18,0x00},/*"你",0*/

    {0x10,0x10,0xF0,0x1F,0x10,0xF0,0x00,0x80,0x82,0x82,0xE2,0x92,0x8A,0x86,0x80,0x00},
    {0x40,0x22,0x15,0x08,0x16,0x61,0x00,0x00,0x40,0x80,0x7F,0x00,0x00,0x00,0x00,0x00},/*"好",1*/

    {0x20,0x20,0x20,0xFE,0x20,0x20,0xFF,0x20,0x20,0x20,0xFF,0x20,0x20,0x20,0x20,0x00},
    {0x00,0x00,0x00,0x7F,0x40,0x40,0x47,0x44,0x44,0x44,0x47,0x40,0x40,0x40,0x00,0x00},/*"世",2*/

    {0x00,0x00,0x00,0xFE,0x92,0x92,0x92,0xFE,0x92,0x92,0x92,0xFE,0x00,0x00,0x00,0x00},
    {0x08,0x08,0x04,0x84,0x62,0x1E,0x01,0x00,0x01,0xFE,0x02,0x04,0x04,0x08,0x08,0x00},/*"界",3*/
};


/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
static st7565_dev_t the_st7565;


/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 -----------------------------------------------------------------------------*/
static void
lcd_command(uint8_t data);
static void
lcd_data(uint8_t data);
static void
lcd_disp_hz16(int32_t line,
              int32_t x,
              int32_t fontnum,
              const uint8_t *pdata);
static void
lcd_disp_asc16(int32_t line,
               int32_t x,
               int32_t fontnum,
               const uint8_t *pdata);

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      lcd power on .
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    打开LCD电源并且点亮背光
 *
 * @note
 ******************************************************************************
 */
void
lcd_poweron(void)
{
    LCD_RESET_ON();
    taskDelay(10u);

    LCD_RESET_OFF();
    taskDelay(10u);
    LCD_EL_ON();
}

/**
 ******************************************************************************
 * @brief      发送命令数据给LCD.
 * @param[in]  uint8 f_uidata 需要发送的指令
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
lcd_command(uint8_t data)
{
    the_st7565.pdev->select(the_st7565.pdev, TRUE);
    LCD_A0_L();
    the_st7565.pdev->send(the_st7565.pdev, data);
    the_st7565.pdev->select(the_st7565.pdev, FALSE);
}

/**
 ******************************************************************************
 * @brief      发送数据信息给LCD.
 * @param[in]  uint8 data 需要发送的数据
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
lcd_data(uint8_t data)
{
    the_st7565.pdev->select(the_st7565.pdev, TRUE);
    LCD_A0_H();
    the_st7565.pdev->send(the_st7565.pdev, data);
    the_st7565.pdev->select(the_st7565.pdev, FALSE);
}

/**
 ******************************************************************************
 * @brief      lcd初始化.
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
lcd_init(void)
{
    the_st7565.lcd_sem = semBCreate((uint16_t) 1);
    the_st7565.pdev = spi_init(E_SPI_LCD_PORT);
    lcd_poweron();
    semTake(the_st7565.lcd_sem, WAIT_FOREVER);
    lcd_command((uint8_t) 0xa2); /* 1/9 bias */
    lcd_command((uint8_t) 0xe2);
    lcd_command((uint8_t) 0xa0); /* ADC select , Normal */
    lcd_command((uint8_t) 0xc8); /* Common output reverse */
    lcd_command((uint8_t) 0xa6); /* normal display 1=on */
    lcd_command((uint8_t) 0x2c);
    lcd_command((uint8_t) 0x2e);
    lcd_command((uint8_t) 0x2f); /* V/C off, V/R off, V/F on */

    lcd_command((uint8_t) 0xf8); /***4 booster***/
    lcd_command((uint8_t) 0x00);
    lcd_command((uint8_t) 0x26); /* internal resistor ratio */
    lcd_command((uint8_t) 0x81); /* electronic volume mode set */
    lcd_command((uint8_t) 0x12); /* electronic volume */
    lcd_command((uint8_t) 0x60); /* display start first line */
    lcd_command((uint8_t) 0xaf); /* display on */
    semGive(the_st7565.lcd_sem);
    lcd_clearscreen();
}


/**
 ******************************************************************************
 * @brief      lcd的寄存器初始化.
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern void
lcd_reg(void)
{

    semTake(the_st7565.lcd_sem, WAIT_FOREVER);
    lcd_command((uint8_t) 0xe2);/*reset*/
    //taskDelay(100);
    lcd_command((uint8_t) 0xa2); /* 1/9 bias */
    lcd_command((uint8_t) 0xe2);
    lcd_command((uint8_t) 0xa0); /* ADC select , Normal */
    lcd_command((uint8_t) 0xc8); /* Common output reverse */
    lcd_command((uint8_t) 0xa6); /* normal display 1=on */
    //lcd_command((uint8_t) 0x2c);
    //lcd_command((uint8_t) 0x2e);
    lcd_command((uint8_t) 0x2f); /* V/C off, V/R off, V/F on */

    lcd_command((uint8_t) 0xf8); /***4 booster***/
    lcd_command((uint8_t) 0x00);
    lcd_command((uint8_t) 0x26); /* internal resistor ratio */
    lcd_command((uint8_t) 0x81); /* electronic volume mode set */
    lcd_command((uint8_t) 0x12); /* electronic volume */
    lcd_command((uint8_t) 0x60); /* display start first line */
    lcd_command((uint8_t) 0xaf); /* display on */
    semGive(the_st7565.lcd_sem);
}
/**
 ******************************************************************************
 * @brief      清屏处理.
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern void
lcd_clearscreen(void)
{
    int32_t i;
    int32_t j;
    semTake(the_st7565.lcd_sem, WAIT_FOREVER);
    for (i = 0; i < 8; i++)
    {
        lcd_command((uint8_t) 0x60);
        lcd_command((uint8_t) (0xB0 + (uint32_t)i)); /*set page address*/
        lcd_command((uint8_t) 0x10); /*set seg address*/
        lcd_command((uint8_t) 0x00);
        for (j = 0; j < 132; j++)
        {
            lcd_data((uint8_t) 0x00);
        }
    }
    semGive(the_st7565.lcd_sem);
}

/**
 ******************************************************************************
 * @brief      点亮背光.
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern void
lcd_backligth(int32_t light)
{
    if (light == 1)
    {
        LCD_EL_ON();
    }
    else
    {
        LCD_EL_OFF();
    }
}

/**
 ******************************************************************************
 * @brief      显示LOGO   三星电气字样.
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
lcd_logo(void)
{
    int32_t i;
    lcd_command((uint8_t) 0x60);
    lcd_command((uint8_t) (0xB0 + 1));
    lcd_command((uint8_t) 0x10);
    lcd_command((uint8_t) (0x00 + 4));
    for (i = 0; i < 120; i++)
    {
        lcd_data((uint8_t) 0x28);
    }

    lcd_disp_hz16(1, 32, 4, (uint8_t *) the_logo);

    lcd_command((uint8_t) (0xB0 + 4));
    lcd_command((uint8_t) 0x10);
    lcd_command((uint8_t) (0x00 + 4));
    for (i = 0; i < 120; i++)
    {
        lcd_data((uint8_t) 0x50);
    }
}

/**
 *******************************************************************************
 * @brief      显示16*16的汉字.
 * @param[in]  int32_t line         行数 ，共四行   0-3
 * @param[in]  int32_t x            X坐标，0-128
 * @param[in]  int32_t fontnum      汉字个数
 * @param[in]  const uint8 *pdata 具体内容
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *******************************************************************************
 */
static void
lcd_disp_hz16(int32_t line,
              int32_t x,
              int32_t fontnum,
              const uint8_t *pdata)
{
    int32_t i;
    int32_t j;
    int32_t k;
    semTake(the_st7565.lcd_sem, WAIT_FOREVER);
    lcd_command((uint8_t) 0x60);
    for (k = 0; k < fontnum; k++)
    {
        for (i = 0; i < 2; i++)
        {
            lcd_command((uint8_t) (0xB0u + (uint32_t)i + ((uint32_t)line * 2u)));
            lcd_command((uint8_t) (0x10u + ((((uint32_t)x +
                                ((uint32_t)k * 16u)) & 0xF0u) >> 4u)));
            lcd_command((uint8_t) (0x01u + (((uint32_t)x +
                                ((uint32_t)k * 16u)) & 0x0Fu)));
            for (j = 0; j < 16; j++)
            {
                lcd_data((uint8_t) (*(pdata + (((uint32_t)i +
                               ((uint32_t)k * 2u)) * 16u) + (uint32_t)j)));
            }
        }
    }
    semGive(the_st7565.lcd_sem);
}

/**
 *******************************************************************************
 * @brief      显示16*16的 ASC字符.
 * @param[in]  int32_t line         行数 ，共四行   0-3
 * @param[in]  int32_t x            X坐标，0-128
 * @param[in]  int32_t fontnum      个数
 * @param[in]  const uint8 *pdata 具体内容
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *******************************************************************************
 */
static void
lcd_disp_asc16(int32_t line,
               int32_t x,
               int32_t fontnum,
               const uint8_t *pdata)
{
    int32_t i;
    int32_t j;
    int32_t k;
    semTake(the_st7565.lcd_sem, WAIT_FOREVER);
    lcd_command((uint8_t) 0x60);
    for (k = 0; k < fontnum; k++)
    {
        for (i = 0; i < 2; i++)
        {
            lcd_command((uint8_t) (0xB0u + i + (line * 2u)));
            lcd_command((uint8_t) (0x10u + (((x + (k * 8u)) & 0xF0u) >> 4u)));
            lcd_command((uint8_t) (0x01u + ((x + (k * 8u)) & 0x0Fu)));
            for (j = 0; j < 8; j++)
            {
                lcd_data((uint8_t) (*(pdata + ((i + (k * 2u)) * 8u) + j)));
            }
        }
    }
    semGive(the_st7565.lcd_sem);
}

/**
 *******************************************************************************
 * @brief      获取LCD的容量  eg. 128*64  160*160.
 * @param[out] width 获取的LCD宽度
 * @param[out] height 获取的高度
 * @retval     None
 *
 * @details
 *
 * @note
 *******************************************************************************
 */
extern void
lcd_capacity(int32_t* width, int32_t* height)
{
    *width = 128;
    *height = 64;
}


/**
 *******************************************************************************
 * @brief      在LCD上画图片.
 * @param[in]  line    行序号，0~3,
 * @param[in]  x       横坐标
 * @param[in]  width   需要显示的宽度
 * @param[in]  *data   需要显示的图片数据
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *******************************************************************************
 */
void
lcd_dispbmp(int32_t line,
            int32_t x,
            int32_t width,
            const uint8_t *data)
{
    int32_t i;
    int32_t j;
    semTake(the_st7565.lcd_sem, WAIT_FOREVER);
    lcd_command(0x60);
    for (i = 0; i < 2; i++)
    {
        lcd_command((uint8_t)(0xB0 + i + (line * 2u)));
        lcd_command((uint8_t)(0x10 + (((uint32_t)x & 0xF0u) >> 4u)));
        lcd_command((uint8_t)(0x01 + ((uint32_t)x & 0x0Fu)));
        for (j = 0; j < width; j++)
        {
            lcd_data(*(data + (i * width) + j));
        }
    }
    semGive(the_st7565.lcd_sem);
}
#include <string.h>
#include <zk.h>
/**
 *******************************************************************************
 * @brief      在LCD上显示字符或则汉字.
 * @param[in]  line    行序号，0~3,
 * @param[in]  x       横坐标
 * @param[in]  *data   需要显示的图片数据
 * @param[in]  iny     0 字符不需要取反   1字符需要取反
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *******************************************************************************
 */
extern void
lcd_text(uint8_t font,
         int32_t line,
         int32_t x,
         const uint8_t *pcontent,
         uint8_t iny)
{
    uint8_t data[32];
    int32_t i = 0;
    int32_t j = 0;

    while (i < strlen((char_t*)pcontent))
    {

        if (pcontent[i] < 0x80)
        {/* ascii */
            zk_generate_data(font, pcontent + i, 1, data);
            if (iny == 1)
            {
                for (j = 0; j < 32; j++)
                    data[j] = (uint8_t)~data[j];
            }
            lcd_disp_asc16(line, x, 1, data);
            i++;
            x += 8;
        }
        else
        {
            zk_generate_data(font, pcontent + i, 2, data);
            if (iny == 1)
            {
                for (j = 0; j < 32; j++)
                    data[j] = (uint8_t)~data[j];
            }
            lcd_disp_hz16(line, x, 1, data);
            i += 2;
            x += 16;
        }
    }
}
