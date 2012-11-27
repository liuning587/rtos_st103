/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <errno.h>
#include <stm32f1lib.h>
#include <uart.h>

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#define MAX_UART 2

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* None */

/*-----------------------------------------------------------------------------
 Section: static Variable
 ----------------------------------------------------------------------------*/
GPIO_TypeDef* COM_TX_PORT[MAX_UART] = {GPIOA, GPIOA};
GPIO_TypeDef* COM_RX_PORT[MAX_UART] = {GPIOA, GPIOA};

const uint8_t COM_TX_PIN_SOURCE[MAX_UART] = {GPIO_PinSource9, GPIO_PinSource2};

const uint8_t COM_RX_PIN_SOURCE[MAX_UART] = {GPIO_PinSource10, GPIO_PinSource3};

const uint32_t COM_USART_CLK[MAX_UART] = {RCC_APB2Periph_USART1, RCC_APB1Periph_USART2};

const uint32_t COM_TX_PORT_CLK[MAX_UART] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA};

const uint32_t COM_RX_PORT_CLK[MAX_UART] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA};


const uint16_t COM_TX_PIN[MAX_UART] = {GPIO_Pin_9, GPIO_Pin_2};

const uint16_t COM_RX_PIN[MAX_UART] = {GPIO_Pin_10, GPIO_Pin_3};

/*-----------------------------------------------------------------------------
 Section: static Function Prototypes
 ----------------------------------------------------------------------------*/

static int32_t  stm32f1xxUartTxStartup(SIO_CHAN* pSioChan);
static int32_t  stm32f1xxUartIoctl(SIO_CHAN *pSioChan, uint32_t request, void *arg);
static uint32_t stm32f1xxUartOptSet(stm32f1xx_chan_t * pChan, tty_param_t* pttyparam);
static status_t stm32f1xxUartOpen(stm32f1xx_chan_t* pChan );
static status_t stm32f1xxUartHup(stm32f1xx_chan_t* pChan);
static bool_e stm32f1xxUartTxIsNull(stm32f1xx_chan_t* pChan);

static SIO_DRV_FUNCS stm32f1xxUartDrvFuncs =
{
    stm32f1xxUartIoctl,
    stm32f1xxUartTxStartup,
    NULL,
    NULL
};



/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
void
stm32f1xxUartDevInit(stm32f1xx_chan_t * pChan)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    pChan->sio.pDrvFuncs = &stm32f1xxUartDrvFuncs;

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[pChan->ttyno] | COM_RX_PORT_CLK[pChan->ttyno] | RCC_APB2Periph_AFIO, ENABLE);

    /* Enable UART clock */
    if ((USART_TypeDef*)pChan[pChan->ttyno].baseregs == USART1)
    {
      RCC_APB2PeriphClockCmd(COM_USART_CLK[pChan->ttyno], ENABLE);
    }
    else
    {
      RCC_APB1PeriphClockCmd(COM_USART_CLK[pChan->ttyno], ENABLE);
    }

    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[pChan->ttyno];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(COM_TX_PORT[pChan->ttyno], &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[pChan->ttyno];
    GPIO_Init(COM_RX_PORT[pChan->ttyno], &GPIO_InitStructure);
}
#include <sys_gpio.h>
void
stm32f1xxUartInt(stm32f1xx_chan_t* pChan) /* channel generating the interrupt */
{
    uint8_t outchar;

    sys_gpio_write(IO_LED0, 1);
    if (USART_GetITStatus((USART_TypeDef*)(pChan->baseregs), USART_IT_RXNE) != RESET)
    {
        USART_SendData((USART_TypeDef*)(pChan->baseregs), '?'); //todo
        /* Read one byte from the receive data register */
        (pChan->sio.pDrvFuncs->putRcvChar)(pChan->ttyno,
                (uint8_t) (((USART_TypeDef*) (pChan->baseregs))->DR & 0xFF));

    }
    if (USART_GetITStatus((USART_TypeDef*)(pChan->baseregs), USART_IT_TC) != RESET)
    {
        if ((pChan->sio.pDrvFuncs->getTxChar)(pChan->ttyno, &outchar) != ERROR)
        {
            /* Transmit Data */
            USART_SendData((USART_TypeDef*)(pChan->baseregs), outchar);
        }
        else
        {
            /* Disable the USARTx transmit data register empty interrupt */
            USART_ITConfig((USART_TypeDef*) (pChan->baseregs), USART_IT_TC, DISABLE);
        }
    }
}

static int32_t
stm32f1xxUartTxStartup(SIO_CHAN* pSioChan) /* channel to start */
{
    stm32f1xx_chan_t* pChan = (stm32f1xx_chan_t *)pSioChan;

    USART_ITConfig((USART_TypeDef*)(pChan->baseregs), USART_IT_TC, ENABLE);

    return OK;
}


static int32_t
stm32f1xxUartIoctl(SIO_CHAN* pSioChan, uint32_t request, void* someArg)
{
    stm32f1xx_chan_t* pChan = (stm32f1xx_chan_t*)pSioChan;
    tty_param_t* pttyparam = (tty_param_t*)someArg;

    switch (request)
    {
    case SIO_HW_OPTS_SET:
        switch(pttyparam->baudrate)
        {
        case 300:
        case 600:
        case 1200:
        case 2400:
        case 4800:
        case 9600:
        case 19200:
        case 38400:
        case 57600:
        case 115200:
        case 230400:
        case 460800:
            return (stm32f1xxUartOptSet(pChan, pttyparam));
        default:
            return (EIO);
        }
        break;

    case SIO_HUP:
        return stm32f1xxUartHup(pChan);

    case SIO_OPEN:
        return stm32f1xxUartOpen(pChan); /* always open */

    case SIO_TXISE:
        *((bool_e *)someArg) = stm32f1xxUartTxIsNull(pChan);
        return OK;

    default:
        return (ENOSYS);
    }
}

static status_t
stm32f1xxUartHup(stm32f1xx_chan_t* pChan)
{
    /* DISABLE the selected USART */
    USART_Cmd((USART_TypeDef*) (pChan->baseregs), DISABLE);
    USART_ITConfig((USART_TypeDef*) (pChan->baseregs), USART_IT_TXE, DISABLE);
    USART_ITConfig((USART_TypeDef*) (pChan->baseregs), USART_IT_RXNE, DISABLE);

    return OK;
}

static status_t
stm32f1xxUartOpen(stm32f1xx_chan_t* pChan)
{
    /* Enable the selected USART */
    USART_Cmd((USART_TypeDef*) (pChan->baseregs), ENABLE);
    USART_ITConfig((USART_TypeDef*)(pChan->baseregs), USART_IT_RXNE, ENABLE);

    return OK;
}


static uint32_t
stm32f1xxUartOptSet(stm32f1xx_chan_t * pChan, tty_param_t* pttyparam)
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = pttyparam->baudrate;
    USART_InitStructure.USART_WordLength = pttyparam->wordlength;
    USART_InitStructure.USART_StopBits = pttyparam->stopbits;
    USART_InitStructure.USART_Parity = pttyparam->stopbits;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


    USART_Cmd((USART_TypeDef*) (pChan->baseregs), DISABLE);
    USART_Init((USART_TypeDef*)(pChan->baseregs), &USART_InitStructure);
    //
    // Start the UART.
    //
    stm32f1xxUartOpen(pChan);

    return (OK);
}


static bool_e stm32f1xxUartTxIsNull
    (
    stm32f1xx_chan_t * pChan  /* pointer to channel */
    )
{
    if(USART_GetITStatus((USART_TypeDef*)(pChan->baseregs), USART_IT_TXE) == RESET)
        return TRUE;
    else
        return FALSE;
}


/***************************End of stm32f1xxuart.c ***************************/
