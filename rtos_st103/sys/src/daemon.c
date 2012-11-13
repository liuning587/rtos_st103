/**
 ******************************************************************************
 * @file       daemon.c
 * @version    V1.1.4
 * @brief      API C source file of daemon.c
 *             Created on: 2012-8-30
 *             Author: liuning
 * @details    This file including all API functions's implement of dps.
 * @copy       Copyrigth(C), 2008-2012.
 *
 ******************************************************************************
 */
#include <sched.h>
#include <ucos_ii.h>
#include <types.h>
#include <string.h>


#define OS_TASK_DAEMON_PRIO              3               /* SHELL�̵߳����ȼ�*/
#define OS_TASK_DAEMON_STK_SIZE          1024*8          /* SHELL�̵߳Ķ�ջ��С*/
uint32_t the_daemon_stack[OS_TASK_DAEMON_STK_SIZE/4];          /*SHELL�̵߳Ķ�ջ*/

#ifndef DM_MAX_TASK_NUM
#define DM_MAX_TASK_NUM    (10u)        /* ֧�ֵ������������*/
#endif

#ifndef DM_MAX_NAME_LEN
#define DM_MAX_NAME_LEN     (8u)        /* �����������󳤶� */
#endif

#ifndef DM_MAX_CHECK_TIME
#define DM_MAX_CHECK_TIME  (300u)       /* ι����ʱ����  */
#endif


typedef struct
{
    uint32_t count;                 /**< ������ */
    char_t name[DM_MAX_NAME_LEN];
} soft_dog_t;

static void daemon_loop(void);

static soft_dog_t the_dogs[DM_MAX_TASK_NUM];
static uint8_t the_reboot_flag = 0;

void
daemonInit(void)
{
    int32_t i;

    for (i = 0; i < DM_MAX_TASK_NUM; i++)
    {
        the_dogs[i].count = -1;
        memset(the_dogs[i].name, 0x00, sizeof(the_dogs[i].name));
    }

    //TODO:����򿪹�

    taskSpawn("DAEMON",
            OS_TASK_DAEMON_PRIO,
            the_daemon_stack,
            OS_TASK_DAEMON_STK_SIZE,
            (OSFUNCPTR)daemon_loop,
            0);

    return ;
}


static void
printf_daemon_info()
{

}

/**
 ******************************************************************************
 * @brief      .
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t
daemon_reg(char_t name)
{
    int32_t i;
    OS_CPU_SR cpu_sr;

    for (i = 0; i < DM_MAX_TASK_NUM; i++)
    {
        if (the_dogs[i].count == -1)
        {
            OS_ENTER_CRITICAL();
            the_dogs[i].count = DM_MAX_CHECK_TIME;
            OS_EXIT_CRITICAL();
            return i;
        }
    }

    return -1;
}

/**
 ******************************************************************************
 * @brief
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t
daemon_feed(uint32_t id)
{
    OS_CPU_SR cpu_sr;

    if (id >= DM_MAX_TASK_NUM)
    {
        return -1;
    }

    OS_ENTER_CRITICAL();
    the_dogs[id].count = DM_MAX_CHECK_TIME;
    OS_EXIT_CRITICAL();

    return 0;
}

/**
 ******************************************************************************
 * @brief      ��λ
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
daemon_reboot(void)
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();
    the_reboot_flag = 1u;
    OS_EXIT_CRITICAL();
}

/**
 ******************************************************************************
 * @brief     �ػ�����ִ����
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
daemon_loop(void)
{
    int32_t i;
    OS_CPU_SR cpu_sr;

    while (1)
    {
        taskDelay(OS_TICKS_PER_SEC);

        OS_ENTER_CRITICAL();
        if (the_reboot_flag == 1u)
        {
            /* ���� */
        }
        OS_EXIT_CRITICAL();
        for (i = 0; i < DM_MAX_TASK_NUM; i++)
        {
            if (the_dogs[i].count == 0)
            {
                /* ι����ʱ��Ҫ���� */
            }
            else if (the_dogs[i].count > 0)
            {
                OS_ENTER_CRITICAL();
                the_dogs[i].count--;
                OS_EXIT_CRITICAL();
            }
        }
    }
}

