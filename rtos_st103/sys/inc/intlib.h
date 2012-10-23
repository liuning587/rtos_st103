/**
 ******************************************************************************
 * @file       intlib.h
 * @version    V0.0.1
 * @brief      CM3中断处理.
 * @details    This file including all API functions's implement of intlib.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#ifndef _INTLIB_H_
#define _INTLIB_H_

/*-----------------------------------------------------------------------------
 Section: Includes
 ---------------------------------------------------------------------------*/
#include <types.h>
#include <cpu_os.h>
/*------------------------------------------------------------------------------
Section: Macro Definitions
------------------------------------------------------------------------------*/
#define INT_COUNT   103u    /**< 支持的中断个数 */

/*------------------------------------------------------------------------------
Section: Globals
------------------------------------------------------------------------------*/
/* None */

/*------------------------------------------------------------------------------
Section: Function Prototypes
------------------------------------------------------------------------------*/

extern status_t intConnect(uint32_t int_num, VOIDFUNCPTR routine, uint32_t parameter);
extern status_t intDisconnect(uint32_t int_num);
extern status_t intPrioSet(uint32_t int_num, uint8_t prio);
extern void intLock(void);
extern void intUnlock(void);
extern status_t intEnable(uint32_t int_num);
extern status_t intDisable(uint32_t int_num);
extern void intLibInit(void);

void resetRoutine(void);
/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/

#endif /* _INTLIB_H_ */

/*------------------------------End of intLib.h------------------------------*/
