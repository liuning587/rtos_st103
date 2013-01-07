#include <types.h>
#include <sched.h>
#include <gui.h>
#include <lcd.h>
#include <zk.h>
#include <lcd.h>

#define  N_MESSAGES      10
static void *MsgGrp[N_MESSAGES] = {NULL};
MSG_Q_ID lcd_id = NULL;

//开一个任务进行lcd的维护
static void lcd_task(void)
{
    gui_init();
    taskDelay(100);
    lcd_clearscreen();

	lcd_id = msgQCreate(&MsgGrp[0], N_MESSAGES);

//    lcd_text(FONT_SIZE_16, 1, 0, (uint8_t*)"STM32F103 测试   ", 0u);

    while(1)
    {
        if(msgQReceive(lcd_id, 0, &MsgGrp[0]) == ERROR)
        {
        	printf("msgQReceive error\n");
        	return ;
        }
//        printf("lcd receive %s", MsgGrp[0]);
        lcd_text(((lcd_msg_t *)MsgGrp[0])->font,
        		((lcd_msg_t *)MsgGrp[0])->line,
        		((lcd_msg_t *)MsgGrp[0])->x,
        		((lcd_msg_t *)MsgGrp[0])->pcontent,
        		((lcd_msg_t *)MsgGrp[0])->iny);
    }
}
void
usr_lcd_init(void)
{
    static uint32_t shellstack[512 / 4];

    taskSpawn("lcd", 9, shellstack,
    		512, (OSFUNCPTR)lcd_task, 0);

}
