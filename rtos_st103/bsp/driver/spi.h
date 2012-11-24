/**
 ******************************************************************************
 * @file       spi.h
 * @brief      SPI²Ù×÷
 * @details    This file including all API functions's implement of dps.
 *
 ******************************************************************************
 */
#ifndef __SPI_H__
#define __SPI_H__

#include <types.h>
#include <sched.h>
#include <devlib.h>

typedef enum
{
    E_SPI_LCD_PORT = 0,
    E_SPI_SD_PORT,
    E_SPI_FLASH_PORT,
    E_SPI_ZIGBEE_PORT,
} spi_port_e;

typedef struct spi_ops
{
    SEM_ID lock;
    void     (*select) (struct spi_ops *dev, bool_e selected);
    uint32_t (*ioctl) (struct spi_ops *dev, uint32_t cmd, void *args);
    void     (*send) (struct spi_ops *dev, uint8_t dat);
    void     (*sndblock) (struct spi_ops *dev, const uint8_t* pbuf, uint32_t len);
    void     (*recvblock) (struct spi_ops *dev, uint8_t* pbuf, uint32_t len);
} spi_opt_t;

extern spi_opt_t* spi_init(spi_port_e port);

#endif /* __SPI_H__ */
