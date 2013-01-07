#include <sched.h>
#include <types.h>
#include <leds.h>
#include <lcd.h>
#include <devlib.h>
#include <wlmdev.h>
#include <bsp_adc.h>
#include <bsp_key.h>
#include <interrupt.h>

#define SLAVE

extern void dev_zero_init(void);

struct timespec mytime;

void usrAppInit(void)
{
#if 0
	int32_t fd = -1 ;
	uint8_t val = 0;
	uint8_t rec_val = 0;
	time_t fisr_time = 0, second_time = 0, period = 0;
#endif
    printf("编译时间 %s %s\n", __DATE__, __TIME__);

    devlib_init();

    isr_init();

    //led task init
	leds_init();
	//lcd task init
	usr_lcd_init();
    //key task init
	key_dev_init();//TODO ,放到任务初始化中
	//zero cross task init
	dev_zero_init();
    //ad task init
	ad_init();//TODO，放到任务初始化中


#if 0
	wlm_init(0x13);
	while(1)
	{
		dev_read(fd ,&val ,1);
		//TODO,时间不对
		fisr_time = time(NULL);
		clock_gettime (1, &mytime);
		printf("fisr time  = %d \n", (int)mytime.tv_nsec);
		if(val == 1)
		{
		    wlm_send(0x12, &val, 1);
		    wlm_recv(0x12, &rec_val, 1, 0);
		    if(rec_val != 0)
		    {
		    	second_time = time(NULL);
		    	clock_gettime (1, &mytime);
		    	printf("second time  = %d \n", (int)mytime.tv_nsec);
		    }
		    printf("wlm send %x", val);
		}
		period = second_time - fisr_time;
		printf("time period = %d \n", (int)period);
		if(period <= 0)
		{
			printf("get time interval error \n");
		}
	}

	wlm_init(0x12);
	while(1)
	{
        wlm_recv(0x13, &rec_val, 1, 0);
		if(rec_val == 1)
		{
			//主机那边已经有过零动作，检测本机是否有过零动作
			dev_read(fd ,&val ,1);
			if(val == 1)
			{
				wlm_send(0x13, &rec_val, 1);
			}

		}
	}

//	dev_close(fd);
//	dev_release("zero");
#endif
}
