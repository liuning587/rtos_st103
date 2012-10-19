/*******************************************************************************
 *
 * Copyrigth(C), 2002-2005,Sanxing Tech,Co.,Ltd.
 * Filename: iflash.h
 * Version: V1.0
 * Description: Ƭ��flash����.
 *
 modification history
---------------------
* $Log: iflash.h,v $
* Revision 1.1  2011-05-06 01:01:08  jghuang
* lm1911 bootloader����ͨ��
*
*
*******************************************************************************/
#ifndef __IFLASH_H__
#define __IFLASH_H__

#ifdef __cplusplus            /* Maintain C++ compatibility */
extern "C"
{
#endif /* __cplusplus */

/*------------------------------------------------------------------------------
Section: Includes
------------------------------------------------------------------------------*/
#include <types.h>

/*------------------------------------------------------------------------------
Section: Function Prototypes
------------------------------------------------------------------------------*/
/*******************************************************************************
*
* iflash_erase - ������д�������
*
* INPUTS: 
*   startAddress : ������ʼ��ַ
*   size         : ������С
*
* RETURNS:
*   boolean : true - �����ɹ���false - ����ʧ��
*
*******************************************************************************/
extern boolean iflash_erase(uint32 startAddress, uint32 size);

/*******************************************************************************
*
* iflash_write - д��
*
* INPUTS: 
*   destAddr : д���ַ
*   pSrcData : ��д������
*   length   : д�볤��
*
* RETURNS:
*   boolean : true - �����ɹ���false - ����ʧ��
*
*******************************************************************************/
extern boolean iflash_write(uint32 destAddr, uint8 *pSrcData, uint32 length);

/*******************************************************************************
*
* iflash_read - ��ȡ
*
* INPUTS: 
*   pData    : ��ȡ����
*   address  : ��ȡ��ַ
*   length   : ��ȡ����
*
* RETURNS:
*   boolean : true - �����ɹ���false - ����ʧ��
*
*******************************************************************************/
extern boolean iflash_read(uint8 *pData, uint32 address, uint32 length);

/*******************************************************************************
*
* iflash_sizeGet - ��ȡflash��С
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
extern uint32 iflash_sizeGet(void);

#ifdef __cplusplus /* Maintain C++ compatibility */
} 
#endif /* __cplusplus */

#endif /*__IFLASH_H__ */
/*-------------------------------End of iflash.h------------------------------*/
