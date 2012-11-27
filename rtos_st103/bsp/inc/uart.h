
#ifndef __UART_H__
#define __UART_H__

/*-----------------------------------------------------------------------------
 Section: Includes
 ---------------------------------------------------------------  -------------*/
#include <types.h>
#include <ttylib.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
typedef struct _STM32F2XX_CHAN
{
    /* must be first */
    SIO_CHAN        sio;        /*standard SIO_CHAN element */

    /* register addresses */
    uint32_t        baseregs;       /*UART Registers*/
    uint32_t        ttyno;

} stm32f1xx_chan_t;


/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
/* None */

/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
extern void stm32f1xxUartDevInit(stm32f1xx_chan_t* pChan);

extern void stm32f1xxUartInt
    (
            stm32f1xx_chan_t *    pChan /* channel generating the interrupt */
    );




#endif /* __UART_H__ */
/*-------------------------------end of uart.h-------------------------------*/
