/**
 ******************************************************************************
 * @file       spi.c
 * @brief      SPI操作
 * @details    This file including all API functions's implement of dps.
 *
 ******************************************************************************
 */
#include <sched.h>
#include <sys_gpio.h>
#include "spi.h"

static spi_opt_t the_spi_port[E_SPI_ZIGBEE_PORT] = {};

static status_t soft_spi_init(spi_opt_t* pspi);

spi_opt_t*
spi_init(spi_port_e port)
{
    static uint8_t is_init = 0u;

    switch (port)
    {
        case E_SPI_LCD_PORT:
            if ((is_init & (1u << port)) == 0u)
            {
                is_init |= (1u << port);
                soft_spi_init(&the_spi_port[port]);
            }
            break;

        case E_SPI_SD_PORT:
            if ((is_init & (1u << port)) == 0u)
            {
                is_init |= (1u << port);
            }
            break;

        case E_SPI_FLASH_PORT:
            if ((is_init & (1u << port)) == 0u)
            {
                is_init |= (1u << port);
            }
            break;

        case E_SPI_ZIGBEE_PORT:
            if ((is_init & (1u << port)) == 0u)
            {
                is_init |= (1u << port);
            }
            break;

        default:
            return NULL;
    }

    return &the_spi_port[port];
}

/**
 ******************************************************************************
 * @brief      模拟SPI延时
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
soft_delay(void)
{
    volatile uint32_t i;

    for (i = 0u; i < 1u; i++);  //todo: 延时
}
/**
 ******************************************************************************
 * @brief     模拟SPI片选
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
soft_spi_select(spi_opt_t *dev, bool_e selected)
{
   sys_gpio_write(IO_LCD_CS, (selected == TRUE) ? 0u : 1u);
}

/**
 ******************************************************************************
 * @brief      模拟SPI ioctl
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint32_t
soft_spi_ioctl(spi_opt_t *dev, uint32_t cmd, void *args)
{
    semTake(dev->lock, WAIT_FOREVER);

    semGive(dev->lock);
    return 0;
}

/**
 ******************************************************************************
 * @brief      模拟SPI发送单字节
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
soft_spi_send(spi_opt_t *dev, uint8_t dat)
{
    for (uint8_t i = 0u; i < 8u; i++)
    {
        sys_gpio_write(IO_LCD_CLK, 0u);
        sys_gpio_write(IO_LCD_MOSI, (dat & 0x80) ? 1u : 0u);
        dat <<= 1u;
        soft_delay();
        sys_gpio_write(IO_LCD_CLK, 1u);
    }
}


/**
 ******************************************************************************
 * @brief      模拟SPI发送
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
soft_spi_sendblock(spi_opt_t *dev, const uint8_t* pbuf, uint32_t len)
{
    semTake(dev->lock, WAIT_FOREVER);

    for (uint32_t i = 0; i < len; i++)
    {
        soft_spi_send(dev, pbuf[len]);
    }

    semGive(dev->lock);
}

/**
 ******************************************************************************
 * @brief    模拟SPI初始化
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *  CS      ->  PC1
 *  CLK     ->  PC2
 *  MOSI    ->  PC3
 ******************************************************************************
 */
static status_t
soft_spi_init(spi_opt_t* pspi)
{
    if (pspi->lock != 0)
    {
        return OK;  //已经初始化
    }
    pspi->lock = semBCreate(1);
    if (pspi->lock == NULL)
    {
        return ERROR;
    }
    //todo: IO初始化
    pspi->ioctl = soft_spi_ioctl;
    pspi->select = soft_spi_select;
    pspi->send = soft_spi_send;
    pspi->sndblock = soft_spi_sendblock;
    pspi->recvblock = NULL;

    sys_gpio_write(IO_LCD_CS, 1);
    sys_gpio_write(IO_LCD_CLK, 1);
    sys_gpio_write(IO_LCD_MOSI, 1);
    sys_gpio_write(IO_LCD_A0, 1);

    return OK;
}
