/**
 ******************************************************************************
 * @file       syslib.c
 * @version    V1.1.4
 * @brief      API C source file of syslib.c
 *             Created on: 2012-10-20
 *             Author: Administrator
 * @details    This file including all API functions's implement of dps.
 * @copy       Copyrigth(C), 2008-2012,Sanxing Electric Co.,Ltd.
 *
 ******************************************************************************
 */
#include <maths.h>
#include <intlib.h>
#include <board.h>
#include <uart.h>
#include <ftl.h>
#include <bsp_gpio.h>

/*UART口的定义*/
typedef struct
{
    uint32_t base;
    uint32_t  ttyno;
    uint32_t  intid;

} uart_param_t;


static const uart_param_t uartParas[] =
{
    { USART1_BASE, 0, USART1_IRQn},
    { USART2_BASE, 1, USART2_IRQn},
    //{ USART3_BASE, 2, USART3_IRQn},
};
static stm32f1xx_chan_t stm32f1xxUartChan[NUM_TTY];

static void
sysSerialHwInit(void)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(stm32f1xxUartChan); i++)
    {
        stm32f1xxUartChan[i].baseregs = uartParas[i].base;
        stm32f1xxUartChan[i].ttyno = uartParas[i].ttyno;
        stm32f1xxUartDevInit(&stm32f1xxUartChan[i]);
    }
}
void sysSerialHwInit2(void)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(stm32f1xxUartChan); i++){
       intConnect(uartParas[i].intid, stm32f1xxUartInt, (uint32_t)&stm32f1xxUartChan[i]);
       intEnable(uartParas[i].intid);
    }
}

/* serial channel */
SIO_CHAN* sysSerialChanGet(int32_t channel)
{
    int32_t size = ARRAY_SIZE(stm32f1xxUartChan);

    if (channel < size)
    {
        return (SIO_CHAN *)&stm32f1xxUartChan[channel];
    }
    return NULL;
}
static void sysRccInit(void)
{
    RCC->APB2ENR |= RCC_APB2Periph_USART1;
    RCC->APB1ENR |= RCC_APB1Periph_USART2;
    RCC->APB1ENR |= RCC_APB1Periph_USART3;
}

/**
 ******************************************************************************
 * @brief      硬件初始化
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void sysHwInit0(void)
{
    intLock();

    SystemInit();

    intLibInit();

    //使能外设
    sysRccInit();

    //系统IO初始化
    bsp_gpio_init();

    //系统串口初始化
    sysSerialHwInit();

    intUnlock();

}

void sysHwInit(void)
{
    sysSerialHwInit2();
}

void sysHwInit2()
{
    if(ftlInit() != OK)
    {
        printf("ftlinit is error\n");
        while(1){};
    }
}

/**
 ******************************************************************************
 * @brief      bsp 串口字节输出
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void bsp_putchar(char_t c)
{
    //USART_SendData(USART1, (uint16_t)c);
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART1, (uint8_t) c);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
}
/**
 ******************************************************************************
 * @brief      bsp 串口字节输入
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t bsp_getchar(void)
{
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        return (int32_t)USART_ReceiveData(USART1);
    }
    return 0;
}

/**
 ******************************************************************************
 * @brief      获取系统时间
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
uint32_t SysCtlClockGet(void)
{
    return SystemCoreClock;
}
/**
 ******************************************************************************
 * @brief      cpu软复位
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void bsp_reboot(void)
{
    // 0x05FA: VECTKEY访问钥匙，需同时写入
    // 0x0004: 请求芯片产生一次复位
    *((uint32_t *)0xE000ED0C) = 0x05FA0004;
    return;
}
