/**
 ******************************************************************************
 * @file       iflash.c
 * @version    V0.0.1
 * @brief      API C source file of iflash.
 * @details    Ƭ��flash����.
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
boolean iflash_read(uint8 *pData, uint32 address, uint32 length) {
	uint32 i;
    for (i = 0; i < length; i ++) {
        *pData++ = (*((uint8 *) address++));
    }
    return (true);
}

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
boolean iflash_write(uint32 destAddr, uint8 *pSrcData, uint32 length) {
    uint32 i;

    for (i = 0; i < length; i += 4) {
        // д��4�ֽ�
        if (!programWord(destAddr + i, *(uint32 *)(pSrcData + i)))
            return (false);
        // У��д���Ƿ�ɹ�
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


