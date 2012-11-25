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
static status_t spi1_init(spi_opt_t* pspi, spi_port_e port);

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
        case E_SPI_FLASH_PORT:
            if ((is_init & (1u << port)) == 0u)
            {
                is_init |= (1u << port);
                spi1_init(&the_spi_port[port], port);
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
static uint8_t
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
    return 0;
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

//============================================================================
#include <stm32f1lib.h>
/**
 ******************************************************************************
 * @brief       SPI1片选 (dataflash)
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
dataflash_spi_select(spi_opt_t *dev, bool_e selected)
{
    if (selected == TRUE)
    {
        semTake(dev->lock, WAIT_FOREVER);
        sys_gpio_write(IO_FLASH_CS, 0u);
    }
    else
    {
        sys_gpio_write(IO_FLASH_CS, 1u);
        semGive(dev->lock);
    }
}

/**
 ******************************************************************************
 * @brief      SPI1片选 (miniSD)
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
minisd_spi_select(spi_opt_t *dev, bool_e selected)
{
   if (selected == TRUE)
   {
       semTake(dev->lock, WAIT_FOREVER);
       sys_gpio_write(IO_SD_CS, 0u);
   }
   else
   {
       sys_gpio_write(IO_SD_CS, 1u);
       semGive(dev->lock);
   }
}

/**
 ******************************************************************************
 * @brief      SPI1发送单字节
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint8_t
spi1_sendbyte(spi_opt_t *dev, uint8_t data)
{
  /*!< Wait until the transmit buffer is empty */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
  {
  }

  /*!< Send the byte */
  SPI_I2S_SendData(SPI1, data);

  /*!< Wait to receive a byte*/
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}


/**
 ******************************************************************************
 * @brief      SPI1
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static status_t
spi1_init(spi_opt_t* pspi, spi_port_e port)
{
    static bool_e is_init = FALSE;
    static SEM_ID spi1_lock = NULL;

    if (is_init == FALSE)
    {
        if ((spi1_lock = semBCreate(1)) == NULL)
        {
            return ERROR;
        }
        is_init = TRUE;

        /* 初始化SPI IO口 */
        GPIO_InitTypeDef GPIO_InitStructure;

        /*!< SPI1 Periph clock enable */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

        /*!< Configure SPI1 pins: SCK */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /*!< Configure SPI1 pins: MOSI */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /*!< Configure SPI1 pins: MISO */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        SPI_InitTypeDef  SPI_InitStructure;


        /*!< Deselect the FLASH: Chip Select high */
        sys_gpio_write(IO_FLASH_CS, 1u);
        sys_gpio_write(IO_SD_CS, 1u);

        /*!< SPI configuration */
        SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
        SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;

        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
        SPI_InitStructure.SPI_CRCPolynomial = 7;
        SPI_Init(SPI1, &SPI_InitStructure);

        /*!< Enable the sFLASH_SPI  */
        SPI_Cmd(SPI1, ENABLE);
    }
    pspi->lock = spi1_lock;
    pspi->ioctl = NULL;
    pspi->send = spi1_sendbyte;
    pspi->sndblock = NULL;
    pspi->recvblock = NULL;
    if (port == E_SPI_SD_PORT)
    {
        printf("E_SPI_SD_PORT init ok\n");
        pspi->select =  minisd_spi_select;
        sys_gpio_write(IO_SD_CS, 1u);   //todo: 防止重复初始化
    }
    else if (port == E_SPI_FLASH_PORT)
    {
        pspi->select =  dataflash_spi_select;
        sys_gpio_write(IO_FLASH_CS, 1u);
    }

    return OK;
}

#if 0
/**
 ******************************************************************************
 * @brief      SPI2片选
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
spi2_select(spi_opt_t *dev, bool_e selected)
{
    sys_gpio_write(IO_WS_CS, (selected == TRUE) ? 0u : 1u);
}

/**
 ******************************************************************************
 * @brief      SPI2发送单字节
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint8_t
spi2_sendbyte(uint8_t data)
{
  /*!< Wait until the transmit buffer is empty */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
  {
  }

  /*!< Send the byte */
  SPI_I2S_SendData(SPI2, data);

  /*!< Wait to receive a byte*/
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}

static uint8_t
spi2_readbyte()
{
  uint8_t Data = 0;

  /*!< Wait until the transmit buffer is empty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
  {
  }
  /*!< Send the byte */
  SPI_I2S_SendData(SPI2, 0xFF);

  /*!< Wait until a data is received */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }
  /*!< Get the received data */
  Data = SPI_I2S_ReceiveData(SPI2);

  /*!< Return the shifted data */
  return Data;
}
/**
 ******************************************************************************
 * @brief      SPI2发送
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
spi2_sendblock(spi_opt_t *dev, const uint8_t* pbuf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        spi_sendbyte(SPI2, pbuf[len]);
    }
}

/**
 ******************************************************************************
 * @brief      SPI2发送
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
spi2_recvblock(spi_opt_t *dev, uint8_t* pbuf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        pbuf[i] = spi_readbyte(SPI2);
    }
}
#endif
