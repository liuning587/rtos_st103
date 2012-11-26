/**
 ******************************************************************************
 * @file       daemon.c
 * @version    V1.1.4
 * @brief      API C source file of daemon.c
 *             Created on: 2012-11-26
 *             Author: daixuyi
 * @details    This file including all API functions's implement of dps.
 * &copy       Copyrigth(C), 2008-2012.
 *
 ******************************************************************************
 */
#include <sched.h>
#include <ucos_ii.h>
#include <types.h>
#include <string.h>
#include <shell.h>

#define DM_MAX_TASK_NUM         (10u)       /**< �������ע������ */
#define DM_MAX_NAME_LEN         (8u)
#define DM_MAX_CHECK_TIME       (10u)       //ι�����ʱ��(s)
#define OS_TASK_DAEMON_PRIO     (8u)
#define OS_TASK_DAEMON_STK_SIZE (512u)

typedef struct soft_dog
{
    int32_t count;                 /**< ������ */
    char_t name[DM_MAX_NAME_LEN];
    struct soft_dog* next;
} soft_dog_t;

static soft_dog_t the_soft_dog[DM_MAX_TASK_NUM];
static soft_dog_t *phead = NULL;
static soft_dog_t *pfree = NULL;
static uint32_t the_fd = -1;
static uint8_t daemon_pri = OS_TASK_DAEMON_PRIO;                 /*DAEMON�̵߳����ȼ�*/
static uint32_t daemonstack_size = OS_TASK_DAEMON_STK_SIZE;      /*DAEMON�̵߳Ķ�ջ��С*/
static uint32_t daemonstack[OS_TASK_DAEMON_STK_SIZE / 4];          /*DAEMON�̵߳Ķ�ջ*/

static uint32_t
soft_dog_init(void)
{
	OS_CPU_SR cpu_sr;
	uint32_t i;

	OS_ENTER_CRITICAL();
	memset(the_soft_dog, 0x00, sizeof(the_soft_dog[DM_MAX_TASK_NUM]));
	for(i = 0; i < DM_MAX_TASK_NUM - 1; i++)
	{
		the_soft_dog[i].next = &the_soft_dog[i + 1];
	}
	the_soft_dog[i].next = NULL;
	phead = &the_soft_dog[0];
	pfree = &the_soft_dog[0];
	OS_EXIT_CRITICAL();

	return 0;
}

static soft_dog_t*
find_by_name(char_t* name)
{
	OS_CPU_SR cpu_sr;
    soft_dog_t* pnode = phead;

	OS_ENTER_CRITICAL();
	while(pnode != NULL)
	{
		if(strncmp(pnode->name, name, DM_MAX_NAME_LEN) == 0)
		{

	        OS_EXIT_CRITICAL();
			return pnode;
		}
		pnode = pnode->next;
	}
    OS_EXIT_CRITICAL();

	return NULL;
}

/**
 ******************************************************************************
 * @brief      ע�ᵽ�ػ�����
 * @param[in]  task name
 * @param[out] fd
 * @retval     fd
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
uint32_t
regist_to_daemon(char_t *name)
{
	OS_CPU_SR cpu_sr;
	soft_dog_t *new = NULL;
	uint32_t fd;
	fd = the_fd;

	if(find_by_name(name) != NULL)
	{
		printf("err %s registered\n", name);
		return -1;
	}
	if(fd ==-1 || fd < DM_MAX_TASK_NUM )
	{
		fd ++;
	}
	else
	{
		printf("registered too much\n");
		return -1;//�ĳ�ͳһ�Ĵ����ʶ
	}

	OS_ENTER_CRITICAL();
	new = pfree;
	pfree = new->next;
	OS_EXIT_CRITICAL();
	the_fd = fd;

	new->count = DM_MAX_CHECK_TIME;
	strncpy(new->name, name, DM_MAX_NAME_LEN);

	return fd;
}

uint32_t
feed_dog(uint32_t task_fd)
{
	OS_CPU_SR cpu_sr;
	soft_dog_t* pnode = phead;

	while(pnode != NULL)
	{
		OS_ENTER_CRITICAL();
		pnode->count = DM_MAX_CHECK_TIME;
		pnode = pnode->next;
		OS_EXIT_CRITICAL();
	}
	return 0;
}

/**
 ******************************************************************************
 * @brief      �ػ�������ѭ��
 * @param[in]  none
 * @param[out] none
 * @retval     none
 *
 * @details   �ػ�����ÿһ���Ӹ�����ע������Ľ��̵ļ�������1
 *
 * @note
 ******************************************************************************
 */
static void daemon_loop(void)
{
	OS_CPU_SR cpu_sr;
	soft_dog_t* pnode = phead;
	while(1)
	{
		taskDelay(OS_TICKS_PER_SEC);//1s
		while(pnode != NULL)
		{
			OS_ENTER_CRITICAL();
			if((pnode->count == 0) && (strlen(pnode->name) != 0))
			{
				printf("daemon reboot system...\n");
			}
			else if((pnode->count != 0)&&(strlen(pnode->name) != 0))
			{
				pnode->count --;
			}
			pnode = pnode->next;
			OS_EXIT_CRITICAL();
		}
		pnode = phead;
	}
}
void daemon_init()
{
	soft_dog_init();
    taskSpawn("DAEMON", daemon_pri, daemonstack, daemonstack_size, (OSFUNCPTR)daemon_loop, 0);
}

static void wdt_show(void)
{
	soft_dog_t* pnode = phead;
	printf("Name");
	printf("\r\t\t\t");
	printf("RemainCounts\n");
	while((pnode != NULL) && (strlen(pnode->name) != 0))
	{
		printf("%s", pnode->name);
		printf("\r\t\t\t");
		printf("%d\n", pnode->count);
		pnode = pnode->next;

	}
}
uint32_t do_wdt_show(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
	wdt_show();
    return 0;
}

SHELL_CMD(
    wdtshow, CFG_MAXARGS,        do_wdt_show,
    "wdtshow \r\t\t\t\t wdt show \n"
);

