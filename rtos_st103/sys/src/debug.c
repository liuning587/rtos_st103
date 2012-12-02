/**
 ******************************************************************************
 * @file      debug.c
 * @brief     C Source file of debug.c.
 * @details   This file including all API functions's 
 *            implement of debug.c.	
 *
 * @copy      Copyrigth(C), 2008-2012,Sanxing Electric Co.,Ltd.
 */
 
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <intlib.h>
#include <debug.h>

/*-----------------------------------------------------------------------------
Section: Type Definitions
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Constant Definitions
-----------------------------------------------------------------------------*/
/* NONE */ 

/*-----------------------------------------------------------------------------
Section: Global Variables
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Local Variables
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Local Function Prototypes
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Function Definitions
-----------------------------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief      系统断言调试输出.
 * @param[in]  funcname : 函数名称
 * @param[in]  line          : 代码行数
 *
 * @details
 *      通过SYS_ASSERT宏进行调用，在调试端口输出断言信息，并调用
 *      事件记录的钩子函数。该函数将终止原有任务的继续执行。
 *      SYS_ASSERT(expression);
 * @note
 *******************************************************************************
 */
extern void sysAssert(const char* funcname, int line)
{
    printf("SYS_ASSERT: \n");
    printf("occurred in function:%s,line:%d \n", funcname, line);
    //printf("taskname: %s \n", taskName(NULL));
    intLock();
    while(1);
}

/**
 *******************************************************************************
 * @brief      系统错误调试输出.
 * @param[in]  funcname : 函数名称
 * @param[in]  line     : 代码行数
 *
 * @details
 *      通过SYS_ERROR宏进行调用，在调试端口输出错误信息，并调用
 *      事件记录的钩子函数。该函数不会终止原有任务的继续执行。
 *      SYS_ERROR( expression);
 * @note
 *******************************************************************************
 */
extern void sysError(const char* funcname, int line)
{
    printf("SYS_ERROR: \n");
    printf("occurred in function:%s,line:%d \n", funcname, line);
    //printf("taskname: %s \n", taskName(NULL));
}

extern void
printbuffer(char_t* format,
            uint8_t* buffer,
            int32_t len)
{
    (void)printf(format);
    for (int32_t i = 0; i < len; i++)
    {
        (void)printf("%02X ", *(buffer + i));
    }
    (void)printf("\r\n");
}

/*----------------------------debug.c--------------------------------*/
