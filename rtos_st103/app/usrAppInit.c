#include <sched.h>
#include <types.h>
#include <stm32f1lib.h>
#include <bsp_gpio.h>
#include <sys_gpio.h>

void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == BSP_SUCCESS)
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }

  /* Enable GPIOA, GPIOB and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO, ENABLE);
}

uint8_t led_test(void)
{
	status_t key_val = 0;
	RCC_Configuration();
	sys_gpio_cfg(IO_LED0,IO_OUTPUT0);
	sys_gpio_write(IO_LED0,E_LED_ON);

	sys_gpio_cfg(IO_LED1,IO_OUTPUT0);
	sys_gpio_write(IO_LED1,E_LED_ON);

	sys_gpio_cfg(IO_LCD_AK,IO_OUTPUT0);
	sys_gpio_write(IO_LCD_AK,E_LED_OFF);

	sys_gpio_cfg(IO_KEY0,IO_INPUT);
	key_val = sys_gpio_read(IO_KEY0);

	printf("%d", key_val);
	return 0;
}

void usrAppInit(void)
{
//	printf("float 1.234=%f\n", 1.234);
	led_test();
    //task_leds_start();
}
