/**
 ******************************************************************************
 * @file       uart.c
 * @version    V0.0.1
 * @brief      API C source file of uart.
 * @details    串口控制.
 * @copy       Copyrigth(C), 2012-2015.
 *
 ******************************************************************************
 */
/*------------------------------------------------------------------------------
Section: Includes
------------------------------------------------------------------------------*/
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_gpio.h>
#include <cfg.h>
#include <lib.h>
#include <uart.h>

/*------------------------------------------------------------------------------
Section: Macro Definitions
------------------------------------------------------------------------------*/
#ifdef BOARD_VERSON_0_1
#define UART_BASE                  ((USART_TypeDef *) USART2_BASE)
#define USART_IT_RXNE              ((uint16_t)0x0525)
#define GPIO_PinSource2            ((uint8_t)0x02)
#define GPIO_PinSource3            ((uint8_t)0x03)
#define GPIO_AF_USART2             ((uint8_t)0x07)  /* USART2 Alternate Function mapping */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#endif
#ifdef BOARD_VERSON_0_2
#define UART_BASE                  ((USART_TypeDef *) USART1_BASE)
#define USART_IT_RXNE              ((uint16_t)0x0525)
#define GPIO_PinSource9            ((uint8_t)0x09)
#define GPIO_PinSource10           ((uint8_t)0x0a)
#define GPIO_AF_USART1             ((uint8_t)0x07)  /* USART1 Alternate Function mapping */
#endif

#define UART_CONFIG_WLEN_8      0x00000060  // 8 bit data
#define UART_CONFIG_STOP_ONE    0x00000000  // One stop bit
#define UART_CONFIG_PAR_NONE    0x00000000  // No parity
#define UART_CONFIG_STOP_MASK   0x00000008  // Mask for extracting stop bits
#define UART_CONFIG_PAR_MASK    0x00000086  // Mask for extracting parity
#define UART_CONFIG_STOP_TWO    0x00000008  // Two stop bits

#define USART_StopBits_1                     ((uint16_t)0x0000)
#define USART_StopBits_2                     ((uint16_t)0x2000)

#define CR1_CLEAR_MASK            ((uint16_t)(USART_CR1_M | USART_CR1_PCE | \
                                                  USART_CR1_PS | USART_CR1_TE | \
                                                  USART_CR1_RE))
#define USART_WordLength_8b                  ((uint16_t)0x0000)
#define USART_Parity_No                      ((uint16_t)0x0000)
#define USART_Parity_Even                    ((uint16_t)0x0400)
#define USART_Parity_Odd                     ((uint16_t)0x0600) 
#define USART_Mode_Rx                        ((uint16_t)0x0004)
#define USART_Mode_Tx                        ((uint16_t)0x0008)
#define USART_HardwareFlowControl_None       ((uint16_t)0x0000)
#define IT_MASK                   ((uint16_t)0x001F)
#define CR3_CLEAR_MASK            ((uint16_t)(USART_CR3_RTSE | USART_CR3_CTSE))

/*------------------------------------------------------------------------------
Section: Type Definitions
------------------------------------------------------------------------------*/
/* NONE */

/*------------------------------------------------------------------------------
Section: Function Definitions
------------------------------------------------------------------------------*/
/*******************************************************************************
*
* uart_init - uart初始化
*
*
* INPUTS: 
*   void.
*
* RETURNS:
*   void.
*
*******************************************************************************/
void uart_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
#ifdef BOARD_VERSON_0_1
    // UART IO口线设置

    /* Configure USART Tx as alternate function push-pull  */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef BOARD_VERSON_0_2
    // UART IO口线设置

    /* Configure USART0 Tx as alternate function push-pull  */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART0 Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_ClockInitTypeDef USART_ClockInitStructure;
    USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;

    USART_ClockInit(UART_BASE, &USART_ClockInitStructure);
    USART_Init(UART_BASE, &USART_InitStructure);
    UART_BASE->CR1 |= USART_CR1_UE;
}

/*******************************************************************************
*
* uart_send - 发送指定长度的数据
*
* INPUTS: 
*   const uint8 c : 要发送的字符
*
* RETURNS:
*   void.
*
*******************************************************************************/
void uart_send(const uint8 c) {
    USART_SendData(UART_BASE, c);
    // 等待完成发送
    while (USART_GetFlagStatus(UART_BASE, USART_FLAG_TXE) == RESET) {
    }    
    /*
    while ((UART_BASE->SR & USART_SR_TXE) == 0) {
    }
    UART_BASE->DR = (uint16)(c & 0xFF);
    */
}

/*******************************************************************************
*
* uart_receive - 接收指定长度的数据
*
* INPUTS: 
*   pData : 接收缓冲
*   size  : 接收长度
*
* RETURNS:
*   void
*
*******************************************************************************/
void uart_receive(uint8 *pData, uint32 size) {
    while (size--) {
        // 如果没有数据，则等待
        while (!uart_tryReceive(pData)) {
        }
        pData++;
    }
}

/*******************************************************************************
*
* uart_tryReceive - 尝试接收1个字符
*
* INPUTS: 
*   pData : 接收缓冲
*
* RETURNS:
*   boolean : true-有数据 false-无数据.
*
*******************************************************************************/
boolean hasData() {
    if ((UART_BASE->SR & USART_SR_RXNE) != 0) 
        return true;
    else return false;
}

boolean uart_tryReceive(uint8 *pData) {
    /*
    if ((UART_BASE->SR & USART_SR_RXNE) != 0) {
        *pData = (uint8) (UART_BASE->DR & (uint16_t)0x01FF);
        return (true);
    } else return (false);
    */
    if (USART_GetFlagStatus(UART_BASE, USART_FLAG_RXNE) != RESET) {
        *pData = USART_ReceiveData(UART_BASE);
        return (true);
    } else return (false);
}

/*******************************************************************************
*
* putchar - 往串口输出一个字符
*
* INPUTS: 
*   const uint8 c : 要发送的字符
*
* RETURNS:
*   void.
*
*******************************************************************************/
void putchar(const uint8 c) {
    uart_send(c);
}

/*--------------------------------End of uart.c-------------------------------*/
