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
#include <zk.h>
#include <lcd.h>

static uint32_t led_fd = -1;
extern MSG_Q_ID lcd_id;
static lcd_msg_t lcd_msg= {
		.font = FONT_SIZE_16,
		.line = 1,
		.x = 0,
		.pcontent = (uint8_t*)"测试任务通讯  ",
		.iny = 0u,
};
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
#if 0
        printf("zcp_det = %d,", sys_gpio_read(IO_ZCP_DET));
        taskDelay(50);
#endif
        if(msgQSend(lcd_id, &lcd_msg)!= 0)
        {
        	printf("msgQSend error\n");
        	return ;
        }
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

