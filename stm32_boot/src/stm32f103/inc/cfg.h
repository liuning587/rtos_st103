/**
 ******************************************************************************
 * @file       cfg.h
 * @version    V0.0.1
 * @brief      cfg.
 * @details    参数项.
 * @copy       Copyrigth(C), 2008-2012.
 *
 ******************************************************************************
 */
#ifndef CFG_H
#define CFG_H

/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
/* 时钟频率 */
#define CRYSTAL_FREQ          8000000

/* 栈大小(单位为字) */
#define STACK_SIZE            512

/* 数据缓冲区大小，注意:必须 <= 252 */
#define MAX_BUF_SIZE           252

/* 片内FLASH页大小 */
#define FLASH_PAGE_SIZE       0x00000800u

/* 应用程序起始地址，4K */
#define APP_START_ADDRESS     0x08001000u

/* 串口波特率 */
#define BAUDRATE         115200

/* 喂狗周期 */
#define FEED_DOG_CYCLE        1u

/* 版本 */
//#define BOARD_VERSON_0_1
#define BOARD_VERSON_0_2


#endif /*CFG_H */

/*---------------------------------End of cfg.h------------------------------*/
