#include <stdio.h>
#include <board.h>

extern void sys_start(void);
extern void sysHwInit0(void);

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

    //printf("111app start!\n");
    sys_start();        /* never return */

    return 0;
}
