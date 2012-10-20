#include <stdio.h>
#include <board.h>

extern void sys_start(void);
extern void sysHwInit0(void);

/*
 * 入口函数
 * 1. CPU相关初始化
 * 2. 系统滴答初始化
 * 3.
 * 4. 创建root任务
 */
int32_t main(void)
{
    sysHwInit0();

    //printf("111app start!\n");
    sys_start();        /* never return */

    return 0;
}
