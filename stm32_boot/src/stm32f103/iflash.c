/**
 ******************************************************************************
 * @file       iflash.c
 * @version    V0.0.1
 * @brief      API C source file of iflash.
 * @details    片内flash处理.
 * @copy       Copyrigth(C), 2012-2015.
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <cfg.h>
#include <iflash.h>
#include <stm32f10x.h>
#include <stm32f10x_flash.h>

#define BANK1_WRITE_START_ADDR  ((uint32_t)APP_START_ADDRESS)


/*-----------------------------------------------------------------------------
Section: Private Function Prototypes 
-----------------------------------------------------------------------------*/
LOCAL boolean programWord(uint32 address, uint32 data);

/*-----------------------------------------------------------------------------
Section: Function Definitions
-----------------------------------------------------------------------------*/
void iflash_init(void) {
    /* Unlock the Flash Bank1 Program Erase controller */
    FLASH_UnlockBank1();
    /* Clear All pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}

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
boolean iflash_erase(uint32 startAddress, uint32 size) {
    uint32_t EraseCounter = 0x00;
    uint32_t NbrOfPage = 0x00;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

    /* Define the number of page to be erased */
    NbrOfPage = (size + FLASH_PAGE_SIZE) / FLASH_PAGE_SIZE;
    /* Erase the FLASH pages */
    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
    }


    return (true);
}

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
boolean iflash_read(uint8 *pData, uint32 address, uint32 length) {
	uint32 i;
    for (i = 0; i < length; i ++) {
        *pData++ = (*((uint8 *) address++));
    }
    return (true);
}

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
boolean iflash_write(uint32 destAddr, uint8 *pSrcData, uint32 length) {
    uint32 i;

    for (i = 0; i < length; i += 4) {
        // 写入4字节
        if (!programWord(destAddr + i, *(uint32 *)(pSrcData + i)))
            return (false);
        // 校验写入是否成功
        if (*(uint32 *)(destAddr + i) != *(uint32 *)(pSrcData + i))
            return (false);
    }

    return (true);    
}


LOCAL boolean programWord(uint32 address, uint32 data) {
    boolean status = false;
    
    if (FLASH_COMPLETE ==  FLASH_ProgramWord(address, data))
        status = true;
    return status;
}

/*-------------------------------End of flash.c-------------------------------*/


