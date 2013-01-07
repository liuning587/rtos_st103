/**
 ******************************************************************************
 * @file       lcd.h
 * @brief      API include file of gui.h.
 * @details    This file including all API functions's declare of gui.h.
 *
 ******************************************************************************
 */
#ifndef __LCD_H__
#define __LCD_H__
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <types.h>


typedef struct _lcd_msg_t
{
	uint8_t font;
	int32_t line;
	int32_t x;
    const uint8_t *pcontent;
    uint8_t iny	;
}lcd_msg_t;

extern void usr_lcd_init(void);

#endif /* __LCD_H__ */
