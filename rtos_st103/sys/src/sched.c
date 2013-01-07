/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <time.h>
#include <string.h>
#include <sched.h>
#include <types.h>
#include <maths.h>
#include <ucos_ii.h>


/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define TV_NORMALIZE(a) \
    if ((a).tv_nsec >= BILLION) \
        { (a).tv_sec++; (a).tv_nsec -= BILLION; }   \
    else if ((a).tv_nsec < 0)   \
        { (a).tv_sec--; (a).tv_nsec += BILLION; }

#define TV_CONVERT_TO_SEC(a,b)  \
        do { \
        register uint32_t hz = sysClkRateGet(); \
    (a).tv_sec  = (time_t)((b) / hz);   \
    (a).tv_nsec = (long)(((b) % hz) * (BILLION / hz)); \
        } while (0)

#define TV_ADD(a,b) \
        (a).tv_sec += (b).tv_sec; (a).tv_nsec += (b).tv_nsec; TV_NORMALIZE(a)

#define BILLION         1000000000  /* 1000 million nanoseconds / second */

/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
static  uint16_t  OSTaskID = 65500;                /* task ID             */
static  struct clock _clockRealtime;


/*
 ******************************************************************************
 *                                TASK INTERFACE
 ******************************************************************************
 */
/*
 * This function spawns a task
 */
extern int32_t taskSpawn
    (
    char_t *    name,       /* name of new task */
    uint8_t     priority,   /* priority of new task */
    uint32_t    *pbos,      /* a pointer to the task's bottom of stack */
    uint32_t    stackSize,  /* size (bytes) of stack, need a multiple of 4 */
    OSFUNCPTR   entryPt,    /* entry point32_t of new task */
    int32_t     arg         /* task args to pass to func */
    )
{
    OS_STK *ptos;
    uint8_t err;

    if ((stackSize % 4) || ((uint32_t)pbos % 4))
    {
        printf("Create task error, the stack size or address is not 4 bytes alignment\r\n");
        return ERROR;
    }

    ptos = (OS_STK *)pbos + ((stackSize / 4) - 1);

    err = OSTaskCreateExt(entryPt,
                       (int32_t *)arg,
                       (OS_STK *)ptos,
                       priority,
                       OSTaskID,
                       (OS_STK *)pbos,
                       stackSize / 4,
                       (void *)0,                                 /* No TCB extension                     */
                       OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
    if (err != OS_ERR_NONE) {
        printf("Create task:%s error[%d]\r\n",name,err);
        return ERROR;
    }
    OSTaskNameSet(priority, (uint8_t *)name, (uint8_t *)&err);
    OSTaskID--;
    if (OSTaskID <= 10) return -1;
    return priority;
}

/*
 ******************************************************************************
 *                                TIME INTERFACE
 ******************************************************************************
 */
/******************************************************************************
*
* clockLibInit - initialize clock facility
*
* This routine initializes the POSIX timer/clock facilities.
* It should be called by usrRoot() in usrConfig.c before any other
* routines in this module.
*
* WARNING
* Non-POSIX.
*
* RETURNS:
* 0 (OK), or -1 (ERROR) on failure or already initialized or clock rate
* less than 1 Hz.
*
* NOMANUAL
*/

static int32_t clockLibInit (void)
{
    static bool_e libInstalled = FALSE;

    if (libInstalled)
        return (ERROR);

    if (sysClkRateGet () < 1)
        return (ERROR);

    libInstalled = TRUE;

    memset ((char*)&_clockRealtime, 0, sizeof (_clockRealtime));

    _clockRealtime.tickBase = OSTimeGet();

    return (OK);
}

/******************************************************************************
*
* clock_gettime - get the current time of the clock (POSIX)
*
* This routine gets the current value <tp> for the clock.
*
* INTERNAL
* The standard doesn't indicate when <tp> is NULL (an invalid address)
* whether errno should be EINVAL or EFAULT.
*
* RETURNS: 0 (OK), or -1 (ERROR) if <clock_id> is invalid or <tp> is NULL.
*
*/

extern int32_t clock_gettime
    (
    int32_t clock_id,       /* clock ID (always CLOCK_REALTIME) */
    struct timespec * tp    /* where to store current time */
    )
{
    uint32_t diffTicks;       /* system clock tick count */

    clockLibInit ();

    if (clock_id != CLOCK_REALTIME) {
        return (ERROR);
    }

    if (tp == NULL) {
        return (ERROR);
    }

    diffTicks = OSTimeGet() - _clockRealtime.tickBase;

    TV_CONVERT_TO_SEC(*tp, diffTicks);
    TV_ADD (*tp, _clockRealtime.timeBase);

    return (OK);
}

/*******************************************************************************
*
* clock_settime - set the clock to a specified time (POSIX)
*
* This routine sets the clock to the value <tp>, which should be a multiple
* of the clock resolution.  If <tp> is not a multiple of the resolution, it
* is truncated to the next smallest multiple of the resolution.
*
* RETURNS:
* 0 (OK), or -1 (ERROR) if <clock_id> is invalid, <tp> is outside the supported
* range, or the <tp> nanosecond value is less than 0 or equal to or greater than
* 1,000,000,000.
*
*/

extern int32_t clock_settime
    (
    int32_t clock_id,       /* clock ID (always CLOCK_REALTIME) */
    const struct timespec * tp  /* time to set */
    )
    {
    clockLibInit ();

    if (clock_id != CLOCK_REALTIME) {
        return (ERROR);
    }

    if (tp == NULL || tp->tv_nsec < 0 || tp->tv_nsec >= BILLION) {
        return (ERROR);
    }

    /* convert timespec to vxTicks XXX use new kernel time */

    _clockRealtime.tickBase = OSTimeGet();
    _clockRealtime.timeBase = *tp;

    return (OK);
    }

/****************************************************************************
*
* time - determine the current calendar time (ANSI)
*
* This routine returns the implementation's best approximation of current
* calendar time in seconds.  If <timer> is non-NULL, the return value is
* also copied to the location <timer> points to.
*
* INTERNAL
* Uses the POSIX clockLib functions.
* Does this return the number of seconds since the BOARD was booted?
*
* INCLUDE FILES: time.h
*
* RETURNS:
* The current calendar time in seconds, or ERROR (-1) if the calendar time
* is not available.
*
* SEE ALSO: clock_gettime()
*/
time_t time
    (
    time_t *timer   /* calendar time in seconds */
    )
{
    struct timespec tp;

    if (clock_gettime (CLOCK_REALTIME, &tp) == OK) {
        if (timer != NULL)
            *timer = (time_t) tp.tv_sec;
        return (time_t) (tp.tv_sec);
    } else
        return (time_t) (ERROR);
}

char_t *
time_ntoa(const time_t stime)
{
  static char str[22];
  struct tm daytime;
  daytime = *localtime(&stime);
  sprintf(str,"%04d-%02d-%02d  %02d:%02d:%02d",
    daytime.tm_year+ 1900, daytime.tm_mon + 1, daytime.tm_mday,
        daytime.tm_hour, daytime.tm_min, daytime.tm_sec);
  return str;
}


/*
* This function allows you to delete a task
*/
extern  status_t taskDelete
    (
    uint8_t tid                   /* task ID of task to delete */
    )
{
    return OSTaskDel(tid);
}

/*
* This function is called to suspend a task
*/
extern  status_t taskSuspend
    (
    uint8_t tid                   /* task ID of task to suspend */
    )
{
    return OSTaskSuspend(tid);
}

/*
* This function is called to resume a previously suspended task
*/
extern  status_t taskResume
    (
    uint8_t tid                   /* task ID of task to resume */
    )
{
    return OSTaskResume(tid);
}

/*
* This function allows you to change the priority of a task dynamically
*/
extern  status_t taskPrioritySet
    (
    uint8_t tid,                  /* task ID */
    uint8_t newPriority           /* new priority */
    )
{
    return OSTaskChangePrio(tid, newPriority);
}

/*
* This function is used to prevent rescheduling to take place.
*/
extern  void taskLock(void)
{
    OSSchedLock();
}

/*
* This function is used to re-allow rescheduling.
*/
extern  void taskUnlock(void)
{
    OSSchedUnlock();
}

/*
* This function is called to delay execution of the currently running task until the
* specified number of system ticks expires.
*/
extern  void taskDelay
    (
    uint32_t ticks                 /* number of ticks to delay task */
    )
{
    OSTimeDly(ticks);
}

/*
********************************************************************************
*                                SEMAPHORE INTERFACE
********************************************************************************
*/
/*
* This function creates a semaphore.
*/
extern  SEM_ID semBCreate
    (
    uint16_t  cnt  /* the initial value for the semaphore */
    )
{
    return OSSemCreate (cnt);
}
/*
* This function waits for a mutual exclusion semaphore.
*/
extern  status_t semDelete
    (
    SEM_ID semId              /* semaphore ID to take */
    )
{
    uint8_t status_err;

    OSSemDel(semId, OS_DEL_ALWAYS, &status_err);
    if (OS_NO_ERR == status_err) return OK;
    return ERROR;
}

/*
* This function waits for a mutual exclusion semaphore.
*/
extern  status_t semTake
    (
    SEM_ID semId,             /* semaphore ID to take */
    uint32_t timeout         /* timeout in ticks */
    )
{
    uint8_t status_err;

    OSSemPend (semId, timeout, &status_err);
    if (OS_NO_ERR == status_err) return OK;
//    printf("semTake error %d\r\n", status_err);
    return ERROR;
}
/*
* This function signals a semaphore.
*/
extern  status_t semGive
    (
    SEM_ID semId              /* semaphore ID to give */
    )
{

    uint8_t status_err = OSSemPost (semId);
    if (OS_NO_ERR == status_err) return OK;
    printf("semGive error[%d], pri:%d\r\n", status_err, OSTCBCur->OSTCBPrio);
    return ERROR;
}

/*
********************************************************************************
*                                MESSAGE INTERFACE
********************************************************************************
*/
/*
* This function creates a message queue if free event control blocks are available.
*/
extern  MSG_Q_ID msgQCreate
    (
    void    **start,/* a pointer to the base address of the message queue storage area */
    uint16_t  size  /* the number of elements in the storage area */
    )
{
    return OSQCreate (start, size);
}

/*
* This function sends a message to a queue.
*/
extern  status_t msgQSend
    (
    MSG_Q_ID  msgQId,/* a pointer to the event control block associated with the desired queue */
    void      *pmsg  /* a pointer to the message to send */
    )
{
     return OSQPost (msgQId, pmsg);
}

/*
* This function waits for a message to be sent to a queue
*/
extern  status_t msgQReceive
    (
    MSG_Q_ID  msgQId,/* a pointer to the event control block associated with the desired queue */
    int32_t   timeout,  /* ticks to wait */
    void **pmsg       /* a pointer to the message to recv */
    )
{
    uint8_t err;

    *pmsg = OSQPend (msgQId, timeout, &err);

    if (err == OS_NO_ERR) return OK;
    return ERROR;
}

/*
* This function gets the number of messages queued to a message queue
*/
extern int32_t msgQNumMsgs
    (
    MSG_Q_ID msgQId /* a pointer to the event control block associated with the desired queue */
    )
{
    OS_Q_DATA msgInfo;

    if (OS_NO_ERR == OSQQuery(msgQId, &msgInfo)) {
        return msgInfo.OSNMsgs;
    }
    return 0;
}

/*
* This function is used to flush the contents of the message queue.
*/
extern  uint8_t msgQFlush
    (
    MSG_Q_ID msgQId /* a pointer to the event control block associated with the desired queue */
    )
{
    return OSQFlush(msgQId);
}



/*
* This function is used by your application to obtain the current value of the 32-bit
* counter which keeps track of the number of clock ticks.
*/
extern  uint32_t tickGet(void)
{
    return OSTimeGet();
}



extern uint32_t sysClkRateGet()
{
 return OS_TICKS_PER_SEC;
}



/*-------------------------------End of sched.c--------------------------------*/
