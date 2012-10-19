/**
 ******************************************************************************
 * @file       stm32f2xx.c
 * @version    V0.0.1
 * @brief      API C source file of iflash.
 * @details
 * @copy       Copyrigth(C), 2012-2015.
 *
 ******************************************************************************
 */
/*------------------------------------------------------------------------------
Section: Includes
------------------------------------------------------------------------------*/
#include <stm32f10x.h>
#include <uart.h>
#include <system_stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <cfg.h>

/*------------------------------------------------------------------------------
Section: Macro Definitions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Section: Type Definitions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Section: Function Definitions
------------------------------------------------------------------------------*/
/**
******************************************************************************
* @brief      hw_init
* @param[in]  None
* @param[out] None
* @retval     None
*
* @details    ”≤º˛≥ı ºªØ
*
* @note
******************************************************************************
*/
extern void hw_init(void) {
    SystemInit();  

    /* Enable GPIO clock */
    RCC->APB2ENR |= RCC_APB2Periph_AFIO;
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA;

    /* Enable UART clock */
    RCC->APB1ENR |= RCC_APB1Periph_USART2;

    // ≈‰÷√UART
    uart_init();


}
typedef struct {
  vu32 CTRL;
  vu32 LOAD;
  vu32 VAL;
  vuc32 CALIB;
} SysTick_TypeDef;

#define SysTick_LOAD_RELOAD_Msk  (0xFFFFFFul << SysTick_LOAD_RELOAD_Pos)
#define SysTick_LOAD_RELOAD_Pos  0
#define SysTick                  ((SysTick_TypeDef *)SysTick_BASE)
#define SysTick_BASE        (SCS_BASE +  0x0010)                      /*!< SysTick Base Address              */
#define SCS_BASE            (0xE000E000)                              /*!< System Control Space Base Address */
#define SysTick_CTRL_TICKINT_Msk           (1ul << SysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */
#define SysTick_CTRL_ENABLE_Msk            (1ul << SysTick_CTRL_ENABLE_Pos)               /*!< SysTick CTRL: ENABLE Mask */
#define SysTick_CTRL_TICKINT_Pos            1                                             /*!< SysTick CTRL: TICKINT Position */

#define SysTick_CTRL_ENABLE_Pos             0                                             /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_CLKSOURCE_Pos          2                                             /*!< SysTick CTRL: CLKSOURCE Position */
#define SCB_AIRCR_VECTKEY_Msk              (0xFFFFul << SCB_AIRCR_VECTKEY_Pos)            /*!< SCB AIRCR: VECTKEY Mask */
#define SCB_AIRCR_PRIGROUP_Pos              8
#define SCB_AIRCR_PRIGROUP_Msk             (7ul << SCB_AIRCR_PRIGROUP_Pos)                /*!< SCB AIRCR: PRIGROUP Mask */
#define SCB_AIRCR_VECTKEY_Pos              16
#define SysTick_CTRL_CLKSOURCE_Msk         (1ul << SysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */
#define SCB_BASE            (SCS_BASE +  0x0D00)                      /*!< System Control Block Base Address */

#define CTRL_TICKINT_Set    ((u32)0x00000002)
#define SCB                 ((SCB_Type *)           SCB_BASE)         /*!< SCB configuration struct          */

typedef struct {
  __IO uint32_t ISER[8];                      /*!< Offset: 0x000  Interrupt Set Enable Register           */
       uint32_t RESERVED0[24];
  __IO uint32_t ICER[8];                      /*!< Offset: 0x080  Interrupt Clear Enable Register         */
       uint32_t RSERVED1[24];
  __IO uint32_t ISPR[8];                      /*!< Offset: 0x100  Interrupt Set Pending Register          */
       uint32_t RESERVED2[24];
  __IO uint32_t ICPR[8];                      /*!< Offset: 0x180  Interrupt Clear Pending Register        */
       uint32_t RESERVED3[24];
  __IO uint32_t IABR[8];                      /*!< Offset: 0x200  Interrupt Active bit Register           */
       uint32_t RESERVED4[56];
  __IO uint8_t  IP[240];                      /*!< Offset: 0x300  Interrupt Priority Register (8Bit wide) */
       uint32_t RESERVED5[644];
  __O  uint32_t STIR;                         /*!< Offset: 0xE00  Software Trigger Interrupt Register     */
}  NVIC_Type;
#define NVIC                ((NVIC_Type *)          NVIC_BASE)        /*!< NVIC configuration struct         */
#define NVIC_BASE           (SCS_BASE +  0x0100)                      /*!< NVIC Base Address                 */

static void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
  if(IRQn < 0) {
    SCB->SHP[((uint32_t)(IRQn) & 0xF)-4] = ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff); 
  } else {
    NVIC->IP[(uint32_t)(IRQn)] = ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff);    
  }
}

void systick_open(void) {
    // = max HCLK/8
    SysTick->LOAD  = ((SystemCoreClock / 1000) & SysTick_LOAD_RELOAD_Msk) - 1;
    NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    SysTick->VAL   = 0;                                          
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
    return;
}

void systick_stop(void) {
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

