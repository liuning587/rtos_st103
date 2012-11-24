/**
 ******************************************************************************
 * @file       gui.h
 * @brief      API include file of gui.h.
 * @details    This file including all API functions's declare of gui.h.
 *
 ******************************************************************************
 */
#ifndef __GUI_H__
#define __GUI_H__
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
/* NONE */

/*-----------------------------------------------------------------------------
Section: Globals
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Function Prototypes
-----------------------------------------------------------------------------*/
/*如下为直接操作LCD的接口*/
extern void
lcd_poweron(void);
extern void
lcd_init(void);
extern void
lcd_clearscreen(void);
extern void
lcd_backligth(int32_t light);
extern void
lcd_capacity(int32_t* width ,int32_t* height);
extern void
lcd_logo(void);
extern void
lcd_dispbmp(int32_t line, int32_t x, int32_t width, const uint8_t *data);
extern void

lcd_text(int8_t font,int32_t line, int32_t x,const int8_t *pcontent,int8_t iny);
extern void
lcd_point(int32_t x,int32_t y,int8_t color);
extern void
lcd_line(int32_t x,int32_t y,int8_t lenth);
extern void
lcd_row(int32_t x,int32_t y,int8_t h);
 extern void
 lcd_reg(void);

/* 如下为GUI的接口定义  */
 extern status_t
 gui_init(void);
#endif /* __GUI_H__ */
/*--------------------------End of gui.h----------------------------*/
