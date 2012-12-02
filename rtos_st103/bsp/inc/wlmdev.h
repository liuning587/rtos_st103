/**
 ******************************************************************************
 * @file       wlmdev.h
 * @brief      API include file of wlmdev.h.
 * @details    This file including all API functions's declare of wlmdev.h.
 *
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <types.h>
#include <sched.h>

/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
#define WLM_IS_CC1101            (1u)
#define WLM_MAX_TRANSMISSION_SIZE   (20u)   //设置单包传输20字节

/*-----------------------------------------------------------------------------
Section: Type Definitions
----------------------------- ------------------------------------------------*/


/*-----------------------------------------------------------------------------
Section: Globals
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Function Prototypes
-----------------------------------------------------------------------------*/
extern status_t wlm_init(uint8_t addr);
extern int32_t wlm_send(uint8_t addr, const uint8_t* pbuf, int32_t len);
extern int32_t wlm_recv(uint8_t addr, uint8_t* pbuf, int32_t len);

/*--------------------------End of wlmdev.h----------------------------*/
