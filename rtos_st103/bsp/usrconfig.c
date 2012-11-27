#include <stdio.h>
#include <string.h>
#include <board.h>
#include <ucos_ii.h>
#include <ttylib.h>
#include <shell.h>
#include <exc.h>
#include <daemon.h>

extern void sysHwInit0(void);
extern void sysHwInit2(void);
extern void sysHwInit(void);
extern uint32_t SysCtlClockGet(void);
extern SIO_CHAN* sysSerialChanGet(int32_t channel);
extern void usrAppInit(void);
const char_t* the_rtos_logo =
#if 0
        " \\|/\n"
        "--O-- The CM3 RTOS is starting...\n"
        " /|\\\n";
#else
    "\r\r\r _____    _____   _____    _____  \n"
    "|  _  \\  |_   _| /  _  \\ /  ___/ \n"
    "| |_| |    | |   | | | | | |___  \n"
    "|  _  /    | |   | | | | \\___  \\ \n"
    "| | \\ \\    | |   | |_| |  ___| | \n"
    "|_|  \\_\\   |_|   \\_____/ /_____/ \n";
#endif

#define  ROOTSTACKSIZE  (500)      /* ����root task��ջ��С */
static uint32_t rootstack[ROOTSTACKSIZE / 4];

uint32_t consoleFd;      /* fd of initial console device */
uint8_t consoleName[5];    /* console device name, eg. "ttyc0" */

static void
rootTask(void *p_arg)
{
    //printf version
    printf("....STM32F103 APP START...\n");
    //��ʼ��ϵͳ����
    OS_CPU_SysTickInit( (SysCtlClockGet() / OS_TICKS_PER_SEC));

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();
#endif

    ttyDrv();
    if (NUM_TTY > 0)
    {
        uint32_t ix;
        uint8_t tyName[5];
        for (ix = 0; ix < NUM_TTY; ix++)    /* create serial devices */
        {
            memcpy(tyName, "ttyc", 4);
            tyName[4] = (uint8_t) ix + '0';
            (void) ttyDevCreate(tyName, sysSerialChanGet(ix));

            if (ix == CONSOLE_TTY) /* init the tty console */
            {
                memcpy(consoleName, tyName, 5);
                consoleFd = ttyOpen(consoleName);
                tty_param_t param = { 115200, WordLength_8b, StopBits_1, Parity_No };
                ttyIoctl(consoleFd, SIO_HW_OPTS_SET, (uint32_t)&param);
            }
        }
    }
    sysHwInit2();
    shell_init();
    excInit();

    //�ػ�����
    daemon_init();
    //�û�����
    usrAppInit();
}

/*
 * ��ں���
 * 1. CPU��س�ʼ��
 * 2. ϵͳ�δ��ʼ��
 * 3.
 * 4. ����root����
 */
int32_t main(void)
{
    //���ж� ��ϵͳ��Ƶ
    //�ж��������ʼ��
    //IO��ʼ��TTY��ʼ��
    sysHwInit0();
    //
    //ϵͳ�ں�����
    //
    OSInit();

    //��TTY �� �ж�
    //���������
    sysHwInit();

    //���������ʱ�ӽ��ĳ�ʼ��
    taskSpawn("root",0,rootstack, ROOTSTACKSIZE,(OSFUNCPTR)rootTask,0);

    OSStart();

    return 0;
}
