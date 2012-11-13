#include <stdio.h>
#include <board.h>

extern void sys_start(void);
extern void sysHwInit0(void);
const char_t* the_rtos_logo =
#if 0
        " \\|/\n"
        "--O-- The CM3 RTOS is starting...\n"
        " /|\\\n";
#else
    " _____    _____   _____   _____  \n"
    "|  _  \\  |_   _| /  _  \\ /  ___/ \n"
    "| |_| |    | |   | | | | | |___  \n"
    "|  _  /    | |   | | | | \\___  \\ \n"
    "| | \\ \\    | |   | |_| |  ___| | \n"
    "|_|  \\_\\   |_|   \\_____/ /_____/ \n";
#endif

/*
 * ��ں���
 * 1. CPU��س�ʼ��
 * 2. ϵͳ�δ��ʼ��
 * 3.
 * 4. ����root����
 */
int32_t main(void)
{
    sysHwInit0();

    printf(the_rtos_logo);
    sys_start();        /* never return */

    return 0;
}
