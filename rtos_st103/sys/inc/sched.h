#ifndef _SCHED_H_
#define _SCHED_H_


/*-----------------------------------------------------------------------------
 Section: Includes
-----------------------------------------------------------------------------*/

#include "types.h"
#include "stdio.h"
//#include "time.h"


/*------------------------------------------------------------------------------
Section: Macro Definitions
------------------------------------------------------------------------------*/
//#define CLOCK_REALTIME	0x1	/* system wide realtime clock */


/*------------------------------------------------------------------------------
Section: Type Definitions
------------------------------------------------------------------------------*/
//typedef unsigned long time_t;
typedef void * SEM_ID;
typedef void * MSG_Q_ID;
typedef void        (*OSFUNCPTR) (void *);

typedef struct clock
{
    uint32_t tickBase;			/* vxTicks when time set */
    struct timespec timeBase;		/* time set */
} CLOCK;

#define WAIT_FOREVER  ((uint32_t)0)
/*------------------------------------------------------------------------------
Section: Globals
------------------------------------------------------------------------------*/

/*
********************************************************************************
*                                TASK INTERFACE
********************************************************************************
*/
/*
* This function spawns a task
*/
extern int32_t taskSpawn
    (
    char_t *  name,             /* name of new task */
    uint8_t     priority,         /* priority of new task */
    uint32_t    *pbos,           /* a pointer to the task's bottom of stack */
    uint32_t  stackSize,        /* size (bytes) of stack needed plus name */
    OSFUNCPTR entryPt,        /* entry point32_t of new task */
    int32_t     arg               /* task args to pass to func */
    );


/*
* This function allows you to delete a task
*/
extern  status_t taskDelete
    (
    uint8_t tid                   /* task ID of task to delete */
    );

/*
* This function is called to suspend a task
*/
extern  status_t taskSuspend
    (
    uint8_t tid                   /* task ID of task to suspend */
    );

/*
* This function is called to resume a previously suspended task
*/
extern  status_t taskResume
    (
    uint8_t tid                   /* task ID of task to resume */
    );

/*
* This function allows you to change the priority of a task dynamically
*/
extern  status_t taskPrioritySet
    (
    uint8_t tid,                  /* task ID */
    uint8_t newPriority           /* new priority */
    );
/*
* This function is used to prevent rescheduling to take place.
*/
extern  void taskLock(void);

/*
* This function is used to re-allow rescheduling.
*/
extern  void taskUnlock(void);

/*
* This function is called to delay execution of the currently running task until the
* specified number of system ticks expires.
*/
extern  void taskDelay
    (
    uint32_t ticks                 /* number of ticks to delay task */
    );

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
    );
/*
* This function waits for a mutual exclusion semaphore.
*/
extern  status_t semDelete
    (
    SEM_ID semId              /* semaphore ID to take */
    );

/*
* This function waits for a mutual exclusion semaphore.
*/
extern  status_t semTake
    (
    SEM_ID semId,             /* semaphore ID to take */
    uint32_t    timeout         /* timeout in ticks */
    );
/*
* This function signals a semaphore.
*/
extern  status_t semGive
    (
    SEM_ID semId              /* semaphore ID to give */
    );

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
    );

/*
* This function sends a message to a queue.
*/
extern  status_t msgQSend
    (
    MSG_Q_ID  msgQId,/* a pointer to the event control block associated with the desired queue */
    void      *pmsg  /* a pointer to the message to send */
    );
/*
* This function waits for a message to be sent to a queue
*/
extern  status_t msgQReceive
    (
    MSG_Q_ID  msgQId,/* a pointer to the event control block associated with the desired queue */
    int32_t    timeout,  /* ticks to wait */
    void **pmsg       /* a pointer to the message to recv */
    );
/*
* This function gets the number of messages queued to a message queue
*/
extern  int32_t msgQNumMsgs
    (
    MSG_Q_ID msgQId /* a pointer to the event control block associated with the desired queue */
    );

/*
* This function is used to flush the contents of the message queue.
*/
extern  uint8_t msgQFlush
    (
    MSG_Q_ID msgQId /* a pointer to the event control block associated with the desired queue */
    );
/*
********************************************************************************
*                                TIME INTERFACE
********************************************************************************
*/
/*******************************************************************************
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

int32_t clock_gettime
    (
    int32_t clock_id,		    /* clock ID (always CLOCK_REALTIME) */
    struct timespec * tp	/* where to store current time */
    );

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

int32_t clock_settime
    (
    int32_t clock_id,		/* clock ID (always CLOCK_REALTIME) */
    const struct timespec * tp	/* time to set */
    );

extern char_t *
time_ntoa(const time_t stime);

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
#if 0
time_t time
    (
    time_t *timer	/* calendar time in seconds */
    );
#endif
/*
* This function is used by your application to obtain the current value of the 32-bit
* counter which keeps track of the number of clock ticks.
*/
extern  uint32_t tickGet(void);


extern uint32_t sysClkRateGet();

extern void timer_start(void);
extern void timer_stop(uint8_t *msg);

//extern struct tm * localtime(const time_t *);
//extern time_t mktime(struct tm *);
extern double difftime(time_t, time_t);
extern time_t time(time_t *timer);

#define TIMER_START      timer_start()
#define TIMER_STOP(x)    timer_stop(x)


#ifdef __cplusplus      /* Maintain C++ compatibility */
}
#endif /* __cplusplus */

#endif /* _SX_SYS_H_ */

/*-------------------------------End of sched.h--------------------------------*/
