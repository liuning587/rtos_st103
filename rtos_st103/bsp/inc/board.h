/**
 ******************************************************************************
 * @file       board.h
 * @version    V1.1.4
 * @brief      API C source file of board.h
 *             Created on: 2012-10-20
 *             Author: Administrator
 * @details    This file including all API functions's implement of dps.
 * @copy       Copyrigth(C), 2008-2012,Sanxing Electric Co.,Ltd.
 *
 ******************************************************************************
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stm32f1lib.h>

#define NUM_TTY             (2u)    /* tty¸öÊý */
#define CONSOLE_TTY          0      /* ÖÕ¶ËTTY */

extern uint32_t consoleFd;      /* fd of initial console device */
extern uint8_t consoleName[5];  /* console device name, eg. "ttyc0" */


#endif /* BOARD_H_ */
