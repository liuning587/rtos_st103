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
#include <intlib.h>
#include <stm32f1lib.h>
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

#if 0
    //系统IO初始化
    sysGpioInit();

    //系统串口初始化
    sysSerialHwInit();
    //系统ADC3初始化
    ADC3_CH10_DMA_Config();
    ADC_SoftwareStartConv(ADC3);
#endif
    intUnlock();

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
