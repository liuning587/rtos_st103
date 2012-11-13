/**
 ******************************************************************************
 * @file       cfg.h
 * @version    V0.0.1
 * @brief      cfg.
 * @details    ������.
 * @copy       Copyrigth(C), 2008-2012.
 *
 ******************************************************************************
 */
#ifndef CFG_H
#define CFG_H

/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
/* ʱ��Ƶ�� */
#define CRYSTAL_FREQ          8000000

/* ջ��С(��λΪ��) */
#define STACK_SIZE            512

/* ���ݻ�������С��ע��:���� <= 252 */
#define MAX_BUF_SIZE           252

/* Ƭ��FLASHҳ��С */
#define FLASH_PAGE_SIZE       0x00000800u

/* Ӧ�ó�����ʼ��ַ��4K */
#define APP_START_ADDRESS     0x08001000u

/* ���ڲ����� */
#define BAUDRATE         115200

/* ι������ */
#define FEED_DOG_CYCLE        1u

/* �汾 */
//#define BOARD_VERSON_0_1
#define BOARD_VERSON_0_2


#endif /*CFG_H */

/*---------------------------------End of cfg.h------------------------------*/
