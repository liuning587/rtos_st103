#include <sched.h>
#include <types.h>
#include <leds.h>
#include <lcd.h>


void usrAppInit(void)
{
    printf("����ʱ�� %s %s\n", __DATE__, __TIME__);
	leds_init();
	usr_lcd_init();
}
