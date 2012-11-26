/*
 * leds.c
 *
 *  Created on: 2012-11-17
 *      Author: daixuyi
 */
#include <sched.h>
#include <types.h>
#include <sys_gpio.h>
#include <daemon.h>

static uint32_t led_fd = -1;
static void leds_loop(void)
{
	while (1)
	{
        sys_gpio_write(IO_LED0, E_LED_ON);
        taskDelay(50);
        sys_gpio_write(IO_LED1, E_LED_ON);
        taskDelay(50);
        sys_gpio_write(IO_LED2, E_LED_ON);
        taskDelay(50);
        sys_gpio_write(IO_LED3, E_LED_ON);
        taskDelay(50);
		//sys_gpio_write(IO_LCD_AK, E_LED_OFF);
		//taskDelay(50);
		sys_gpio_write(IO_LED0, E_LED_OFF);
		taskDelay(50);
		sys_gpio_write(IO_LED1, E_LED_OFF);
		taskDelay(50);
        sys_gpio_write(IO_LED2, E_LED_OFF);
        taskDelay(50);
        sys_gpio_write(IO_LED3, E_LED_OFF);
        taskDelay(50);
		//sys_gpio_write(IO_LCD_AK, E_LED_ON);
		//taskDelay(50);
        feed_dog(led_fd);
	}
}

void
leds_init(void)
{
    static uint32_t shellstack[128 / 4];

    taskSpawn("leds", 7, shellstack,
            128, (OSFUNCPTR)leds_loop, 0);

    led_fd = regist_to_daemon("leds");

}

