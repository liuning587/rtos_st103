#include <sched.h>
#include <types.h>
#include <leds.h>
#include <lcd.h>


void usrAppInit(void)
{
	leds_init();
	usr_lcd_init();
}
