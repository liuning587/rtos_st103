/**
 ******************************************************************************
 * @file       exArchLib.c
 * @version    V0.0.1
 * @brief      CM3异常处理.
 * @details    This file including all API functions's implement of exc.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sched.h>
#include <ucos_ii.h>
#include <types.h>
#include <exc.h>
/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define HARD_FALUT_FSR  0xE000ED2C
#define MEM_FALUT_FSR   0xE000ED28
#define BUS_FALUT_FSR   0xE000ED29
#define USGE_FALUT_FSR  0xE000ED2A

#define MEM_FALUT_FAR   0xE000ED34
#define BUS_FALUT_FAR   0xE000ED38

/*-----------------------------------------------------------------------------
 Section: Private Type Definitions
 ----------------------------------------------------------------------------*/
typedef struct
{
    uint32_t  vecAddr;      /**< exception vector */
    char_t *  excMsg;       /**< exception message */
} exc_msg_tbl_t;


typedef struct
{
    uint32_t    valid;      /**< indicators that following fields are valid */
    uint32_t    vecAddr;    /**< exception vector address */
    uint32_t    fsr;        /**< fault status register*/
    uint32_t    faddr;      /**< fault address register */
    uint32_t    sp;
    uint32_t    taskid;
    uint32_t    regs[16];
    time_t      time;       /**< occur time */
} exc_info_t;
/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
exc_info_t    excInfo;
FUNCPTR     _func_excPanicHook;  /* 异常处理钩子函数，在异常中断中执行 */
FUNCPTR     _func_excTaskHook;   /* 异常处理钩子函数，在异常任务中执行，可用于在文件中保存现场信息 */

extern FUNCPTR     _func_excJobAdd;
extern void bsp_reboot(void);

/*-----------------------------------------------------------------------------
 Section: Private Variables
 ----------------------------------------------------------------------------*/
static void excInfoShow(exc_info_t info);
static const exc_msg_tbl_t excMsgTbl [] = {
    {1, "RESET"},
    {2, "NMI"},
    {3, "HARD FAULT"},
    {4, "MEM MANAGE FAULT"},
    {5, "BUS FAULT"},
    {6, "USAGE FAULT"},
    {0, NULL}
    };

/*-----------------------------------------------------------------------------
 Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      异常信息保存
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
excInfoSave(void)
{
    uint32_t i;

    if 	(excInfo.valid != 1)
    {
        return;
    }
    for ( i = 0; excMsgTbl[i].vecAddr != excInfo.vecAddr; i++)
	{
        if (excMsgTbl[i].excMsg == NULL)
        {
            return;
        }
    }
    if (excMsgTbl[i].excMsg != NULL)
    {
        printf ("\r\n%s\r\n", excMsgTbl[i].excMsg);
    }
    else
    {
        printf ("\r\nTrap to uninitialized vector no %d.\r\n",excInfo.vecAddr);
		return;
    }

    // 打印信息
    excInfoShow(excInfo);

    if (_func_excTaskHook != NULL)         /* panic hook? */
    {
        (*_func_excTaskHook) (&excInfo);
    }

	excInfo.valid = 0;
	excInfo.faddr = 0;
}

/**
 ******************************************************************************
 * @brief      打印异常信息
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
excInfoShow(exc_info_t info)
{
	printf("exception fault state regs value: 0x%ux \r\n",info.fsr);
	if (info.faddr != 0)
	{
	    printf("exception occur address: 0x%08x \r\n",info.faddr);
	}

	printf("exception occur regs : \r\n");
	printf("    r0  =%08x    r1  =%08x    r2  =%08x    r3  =%08x \r\n",
		info.regs[8],info.regs[9],info.regs[10],info.regs[11]);
	printf("    r4  =%08x    r5  =%08x    r6  =%08x    r7  =%08x \r\n",
		info.regs[0],info.regs[1],info.regs[2],info.regs[3]);
	printf("    r8  =%08x    r9  =%08x    r10 =%08x    r11 =%08x \r\n",
		info.regs[4],info.regs[5],info.regs[6],info.regs[7]);
	printf("    sp  =%08x    lr  =%08x    pc  =%08x    xpsr=%08x \r\n",
		info.sp,info.regs[13],info.regs[14],info.regs[15]);

	printf("exception occur in task: %d \r\n",info.taskid);

#if 0
    struct tm daytime = *localtime(&info.time);
    printf("exception occur time: %04d-%02d-%02d %02d:%02d:%02d\r\n",
        daytime.tm_year+ 1900, daytime.tm_mon + 1, daytime.tm_mday,
        daytime.tm_hour, daytime.tm_min, daytime.tm_sec);
#endif
}

/**
 ******************************************************************************
 * @brief      excExcHandle - 异常处理
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
excExcHandle(void* pRegs, uint32_t excNo)
{
    excInfo.valid = 1;
    excInfo.vecAddr = excNo;
    memcpy(&excInfo.regs,pRegs,64);
	excInfo.sp =(uint32_t) pRegs;
	excInfo.taskid = OSPrioCur;
    switch(excNo){
        case 3:
            excInfo.fsr = *(uint32_t*)(HARD_FALUT_FSR);
            break;
        case 4:
            excInfo.fsr = (*(uint32_t*)(MEM_FALUT_FSR))&0xff;
            excInfo.faddr = *(uint32_t*)(MEM_FALUT_FAR);
            break;
        case 5:
            excInfo.fsr = ((*(uint32_t*)(MEM_FALUT_FSR))>>8)&0xff;
            excInfo.faddr = *(uint32_t*)(BUS_FALUT_FAR);
            break;
        case 6:
            excInfo.fsr = ((*(uint32_t*)(MEM_FALUT_FSR))>>16)&0xffff;
            break;
    }

    excInfo.time = time(NULL);

    if((OSRunning == OS_FALSE) || (OSIntNesting > 0 ))
    {

        if (_func_excPanicHook != NULL)         /* panic hook? */
        {
            (*_func_excPanicHook) (&excInfo);
        }
        bsp_reboot();
        return;
    }

    OSTaskSuspend(OSPrioCur);

    /* 输出到excTask 打印或存储信息*/
    if (_func_excJobAdd != NULL)
        _func_excJobAdd((VOIDFUNCPTR) excInfoSave, 0, 0, 0,0,0,0);
    //bsp_reboot();
    return;
}
/*-------------------------End of excArchLib.c-------------------------------*/
