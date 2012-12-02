/**
 ******************************************************************************
 * @file       debug.h
 * @brief      API include file of debug.h.
 * @details    This file including all API functions's declare of debug.h.
 * @copy       Copyrigth(C), 2008-2012,Sanxing Electric Co.,Ltd.
 *
 ******************************************************************************
 */
#ifndef __DEBUG_H__
#define __DEBUG_H__
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <types.h>

/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Type Definitions
-----------------------------------------------------------------------------*/

#define Dprintf(x...) \
{    \
    (void)printf("\'%s\'L[%d] %s()=>", __FILE__, __LINE__, __FUNCTION__); \
    (void)printf(x);    \
}


#ifndef SYS_ASSERT
#define SYS_ASSERT(assertion) \
        {if (assertion) sysAssert(__FUNCTION__, __LINE__);}
#endif

#ifndef SYS_ERROR
#define SYS_ERROR(expression)\
        {if (expression) { \
            sysError(__FUNCTION__, __LINE__);}\
        }
#endif
/*-----------------------------------------------------------------------------
Section: Globals
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Function Prototypes
-----------------------------------------------------------------------------*/
extern void
printbuffer(char_t* format,
            uint8_t* buffer,
            int32_t len);

extern void sysAssert(const char* funcname,int line);
extern void sysError(const char* funcname,int line);

#endif /* __DEBUG_H__ */
/*--------------------------End of debug.h----------------------------*/
