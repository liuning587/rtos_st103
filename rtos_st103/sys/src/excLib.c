/**
 ******************************************************************************
 * @file       excLib.c
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
#include <types.h>
#include <sched.h>
#include <exc.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define EXC_MAX_ARGS	6		/* max args to task level call */


/*-----------------------------------------------------------------------------
 Section: Private Type Definitions
 ----------------------------------------------------------------------------*/
typedef struct
{
    VOIDFUNCPTR func;               /**< pointer to function to invoke */
    int32_t     arg [EXC_MAX_ARGS]; /**< args for function */
} exc_msg_t;

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
FUNCPTR     _func_excJobAdd;

/*-----------------------------------------------------------------------------
 Section: Private Variables
 ----------------------------------------------------------------------------*/
static uint8_t exc_pri = 1;                      /*异常中断任务的优先级*/
static uint32_t excTaskStack[480 / 4];           /*异常中断任务的堆栈*/
static uint32_t excstack_size = 480;

static SEM_ID excSemId;
static exc_msg_t msg;

/*-----------------------------------------------------------------------------
  Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      异常处理任务执行体
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
excTask(void)

{
    while (1)
	{
		semTake(excSemId, WAIT_FOREVER);

		(* msg.func) (msg.arg[0], msg.arg[1], msg.arg[2],
			  msg.arg[3], msg.arg[4], msg.arg[5]);
	}
 }

/**
 ******************************************************************************
 * @brief   excJobAdd - request a task-level function call from interrupt level
 *          发生异常是调用
 * @note
 *  This routine allows interrupt level code to request a function call
 *  to be made by excTask at task-level.
 ******************************************************************************
 */
status_t
excJobAdd (VOIDFUNCPTR func,
        int32_t arg1,
        int32_t arg2,
        int32_t arg3,
        int32_t arg4,
        int32_t arg5,
        int32_t arg6)
{
    msg.func = func;
    msg.arg[0] = arg1;
    msg.arg[1] = arg2;
    msg.arg[2] = arg3;
    msg.arg[3] = arg4;
    msg.arg[4] = arg5;
    msg.arg[5] = arg6;
    semGive(excSemId);

    return OK;
}

/**
 ******************************************************************************
 * @brief      异常任务初始化
 * @param[in]  None
 * @param[out] None
 * @retval     OK   : 任务创建成功
 * @retval     ERROR: 任务创建失败
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
status_t
excInit(void)

{
	_func_excJobAdd = excJobAdd;

    excSemId = semBCreate(0);
    int32_t excTaskId =  taskSpawn("tExcTask", exc_pri, excTaskStack, excstack_size, (OSFUNCPTR)excTask, 0);

    return (excTaskId == exc_pri ? OK :ERROR );
}

/*-----------------------------End of excLib.c-------------------------------*/
