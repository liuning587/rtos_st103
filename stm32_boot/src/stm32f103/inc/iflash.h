/*******************************************************************************
 *
 * Copyrigth(C), 2002-2005,Sanxing Tech,Co.,Ltd.
 * Filename: iflash.h
 * Version: V1.0
 * Description: 片内flash处理.
 *
 modification history
---------------------
* $Log: iflash.h,v $
* Revision 1.1  2011-05-06 01:01:08  jghuang
* lm1911 bootloader调试通过
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
* iflash_erase - 擦除将写入的区域
*
* INPUTS: 
*   startAddress : 擦除起始地址
*   size         : 擦除大小
*
* RETURNS:
*   boolean : true - 操作成功，false - 操作失败
*
*******************************************************************************/
extern boolean iflash_erase(uint32 startAddress, uint32 size);

/*******************************************************************************
*
* iflash_write - 写入
*
* INPUTS: 
*   destAddr : 写入地址
*   pSrcData : 待写入数据
*   length   : 写入长度
*
* RETURNS:
*   boolean : true - 操作成功，false - 操作失败
*
*******************************************************************************/
extern boolean iflash_write(uint32 destAddr, uint8 *pSrcData, uint32 length);

/*******************************************************************************
*
* iflash_read - 读取
*
* INPUTS: 
*   pData    : 读取缓冲
*   address  : 读取地址
*   length   : 读取长度
*
* RETURNS:
*   boolean : true - 操作成功，false - 操作失败
*
*******************************************************************************/
extern boolean iflash_read(uint8 *pData, uint32 address, uint32 length);

/*******************************************************************************
*
* iflash_sizeGet - 获取flash大小
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
