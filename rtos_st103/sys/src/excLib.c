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
/* NONE */

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
typedef struct				/* EXC_MSG */
    {
    VOIDFUNCPTR	func;			/* pointer to function to invoke */
    int32_t     arg [EXC_MAX_ARGS];	/* args for function */
    } EXC_MSG;

static EXC_MSG msg;

/*------------------------------------------------------------------------------
Section: Private Function Prototypes
------------------------------------------------------------------------------*/
void excTask(void)

{
    while (1)
	{
		semTake(excSemId, WAIT_FOREVER);

		(* msg.func) (msg.arg[0], msg.arg[1], msg.arg[2],
			  msg.arg[3], msg.arg[4], msg.arg[5]);
	}
 }

/*------------------------------------------------------------------------------
Section: Public Function
------------------------------------------------------------------------------*/
/*******************************************************************************
*
* excJobAdd - request a task-level function call from interrupt level
*
* This routine allows interrupt level code to request a function call
* to be made by excTask at task-level.
*
* NOMANUAL
*/

status_t excJobAdd (VOIDFUNCPTR func, int arg1,int arg2,int arg3,int arg4,int arg5,int arg6)
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

status_t excInit(void)

{
	_func_excJobAdd = excJobAdd;

    excSemId = semBCreate(0);
    int32_t excTaskId =  taskSpawn("tExcTask", exc_pri, excTaskStack, excstack_size, (OSFUNCPTR)excTask, 0);

    return (excTaskId == exc_pri ? OK :ERROR );
}

/*------------------------------End of excLib.c---------------------------------*/
