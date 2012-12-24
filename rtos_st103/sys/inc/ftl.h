/**
 ******************************************************************************
 * @file       ftl.h
 * @version    V0.0.1
 * @brief      sched.
 * @details    This file including all API functions's implement of ftl.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */

#ifndef __FTL_H__
#define __FTL_H__
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <errno.h>

/*-----------------------------------------------------------------------------
Section: Macro Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
extern status_t ftlInit(void);
extern status_t ftlRead(uint32_t virtualAddr, uint8_t * buf, int32_t len);
extern status_t ftlWrite(uint32_t virtualAddr, uint8_t * from, int32_t len);
extern status_t ftlSet(uint32_t virtualAddr, uint8_t value, int32_t len);
status_t ftlCmp(uint32_t virtualAddr, uint8_t* buf, int32_t len, bool_e * same);
extern FLStatus deleteSector(uint16_t sectorNo, int32_t noOfSectors);
extern unsigned long ftl_get_sector_count(void);

#endif /* __FTL_H__ */

/*------------------------------End of ftl.h---------------------------------*/
