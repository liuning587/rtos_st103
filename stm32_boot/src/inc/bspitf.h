/**
 ******************************************************************************
 * @file       bspitf.h
 * @version    V0.0.1
 * @brief      API C source file of bspitf.
 * @details    This file including all API functions's declare of bspitf.
 * @copy       Copyrigth(C), 2012-2015.
 *
 ******************************************************************************
 */
#ifndef BSPITF_H
#define BSPITF_H

/*----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
/* None */

/*----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
/* Ӳ����ʼ�� */
IMPORT void hw_init(void);

/* ϵͳ��ʱ�� */
IMPORT void systick_open(void);
IMPORT void systick_stop(void);

/* ι�� */
IMPORT void watchdog_feed(void);

/* DataFlash���� */
IMPORT void dflash_read(uint8 * pData,
                  uint32 address,
                  uint32 size);
IMPORT void dflash_write(const uint8 * pData,
                   uint32 address,
                   uint32 size);
IMPORT void dflash_erase(void);

/* Ƭ��Flash���� */
IMPORT void iflash_init(void);
IMPORT boolean iflash_erase(uint32 startAddress,
                      uint32 size);
IMPORT boolean iflash_read(uint8 *pData,
                     uint32 address,
                     uint32 length);
IMPORT boolean iflash_write(uint32 destAddr,
       uint8 *pSrcData,
       uint32 length);

/* ���ڴ���  */
IMPORT void uart_send(const uint8 c);
IMPORT void uart_receive(uint8 *pData,
                   uint32 size);
IMPORT boolean uart_try_receive(uint8 *pData);

#endif /* BSPITF_H */
