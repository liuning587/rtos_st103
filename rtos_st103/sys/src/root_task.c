#include <stdio.h>
#include <types.h>
#include <sched.h>
#include <ucos_ii.h>

#include <shell.h>
#include <exc.h>

extern void usrAppInit(void);
extern void OS_CPU_SysTickInit();
extern uint32_t SysCtlClockGet(void);

static void root_task(void *p_arg);

#define  ROOTSTACKSIZE  (200)      /* ����root task��ջ��С */
static  uint32_t rootstack[ROOTSTACKSIZE / 4];

void sys_start(void)
{
    OSInit();
    /*���������ʱ�ӽ��ĳ�ʼ��*/
    taskSpawn("root", 0, rootstack, ROOTSTACKSIZE,(OSFUNCPTR)root_task, 0);

    OSStart();
}

static void
root_task(void *p_arg)
{
    //printf version
    printf("....STM32F103 APP START...\n");
    //��ʼ��ϵͳ����
    OS_CPU_SysTickInit( (SysCtlClockGet() / OS_TICKS_PER_SEC));

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();
#endif
    shellInit();
    usrAppInit();
    excInit();
}
