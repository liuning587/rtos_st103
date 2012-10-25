/**
 ******************************************************************************
 * @file       flash.c
 * @version    V0.0.1
 * @brief      sched.
 * @details    This file including all API functions's implement of flash.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
/*------------------------------------------------------------------------------ 
Section: Includes 
------------------------------------------------------------------------------*/ 


#include <flash.h>
//#include <spi.h>
#include <ioctl.h>
#include <string.h>
#include <stdio.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
/* NONE */ 
 
/*-----------------------------------------------------------------------------
 Section: Private Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */ 
 
/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/

/** 在BSP中申明一个mtd初始化函数数组，由FlFlash 初始化函数执行 */
extern MTD_TBL_ENTRY mtdDevTbl[];
 
/*-----------------------------------------------------------------------------
 Section: Private Variables
 ----------------------------------------------------------------------------*/
static FLFlash s_FlFlash;
static struct mtd_dev_s* mtdDev[MAX_FLASH_CHIP_NUM];
 
/*-----------------------------------------------------------------------------
 Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      read data from the physical address
 * @param[in]  FLFlash *flash: Pointer identifying flash object
 * @param[in]  uint32 address: physical address to read
 * @param[out] void * buf : where to receive data
 * @param[in]  int32_t len : length of data
 *
 * @retval     flOK on success,  otherwise when an error happened.
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static FLStatus
flash_read(FLFlash *flash, uint32_t address, void *buffer, int32_t len)
{
    if (address+len > flash->chipSize) return flBadLength;

    int32_t i = 0;
    while(address > mtdDev[i]->chipsize) {
        address -= mtdDev[i]->chipsize;
        i++;
        if ((i >= MAX_FLASH_CHIP_NUM) || (mtdDev[i] == NULL))
            return flBadLength;
    }

    return  mtdDev[i]->read(mtdDev[i], address,  buffer,len);
}

/**
 ******************************************************************************
 * @brief      write data to the physical address
 * @param[in]  FLFlash *flash: Pointer identifying flash object
 * @param[in]  uint32 address: physical address to write
 * @param[out] void * from : Pointer of data where to write
 * @param[in]  int32_t len : length of data
 *
 * @retval     FLStatus: flOK on success,  otherwise when an error happened.
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static FLStatus
flash_write(FLFlash *flash, uint32_t address,void *buffer,int32_t len)
{
    if (address+len > flash->chipSize) return flBadLength;
  
    int32_t i = 0;
    while(address > mtdDev[i]->chipsize){
        address -= mtdDev[i]->chipsize;
        i++;
        if ((i >= MAX_FLASH_CHIP_NUM)||(mtdDev[i] == NULL))
            return flBadLength;
    }

    return  mtdDev[i]->write(mtdDev[i], address,  buffer,len);

}

/**
 ******************************************************************************
 * @brief      erase contiguous block
 * @param[in]  flash       : Pointer identifying flash object
 * @param[in]  startblock  : First block no. to erase
 * @param[in]  nblocks     : No. of block to erase
 *
 * @retval     FLStatus: flOK on success,  otherwise when an error happened.
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static FLStatus
flash_erase(FLFlash *flash, int32_t startblock, int32_t nblocks)
{
    int32_t i = 0;
    while(startblock > (mtdDev[i]->chipsize / mtdDev[i]->erasesize)){
        startblock -= mtdDev[i]->chipsize / mtdDev[i]->erasesize;
        i++;
        if ((i >= MAX_FLASH_CHIP_NUM)||(mtdDev[i] == NULL))
            return flBadLength;
    }
    return  mtdDev[i]->erase(mtdDev[i], startblock, nblocks);
}

/**
 ******************************************************************************
 * @brief      physical erase all of flash block
 * @param[in]  flash       : Pointer identifying flash object
 * @param[out] None
 * @retval     FLStatus: flOK on success,  otherwise when an error happened.
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static FLStatus
flash_eraseAll(FLFlash *flash)
{
    for (int32_t i=0; i < flash->noOfChips; i++)
    {
        mtdDev[i]->ioctl(mtdDev[i], MTDIOC_BULKERASE, 0);
    }
    return flOK;
}

/*------------------------------------------------------------------------------ 
Section: Public Function  
------------------------------------------------------------------------------*/ 
/**
 ******************************************************************************
 * @brief      initialize the flash object,to support the multi of flash chips
 * @param[in]  None
 * @param[out] None
 * @retval     return the FLFlash object if sucessed!
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern FLFlash*
flash_init(void)
{
    MTD_TBL_ENTRY* pDevTbl;
    int32_t count;

    static uint8_t firstRun = 1;
    if (firstRun)
    {
        memset(&s_FlFlash, 0, sizeof(FLFlash));

        /* Initialize all the available flash devices. */
        for (count = 0, pDevTbl = mtdDevTbl;
            (pDevTbl->mtdLoadFunc != MTD_TBL_END) && (count < MAX_FLASH_CHIP_NUM);
             pDevTbl++, count++)
        {
            mtdDev[count] = (struct mtd_dev_s*)pDevTbl->mtdLoadFunc(pDevTbl->param1,pDevTbl->param2);
            if (mtdDev[count] == NULL) return NULL;
            s_FlFlash.chipSize += mtdDev[count]->chipsize;
            if (s_FlFlash.erasableBlockSize == 0)
            {
                s_FlFlash.erasableBlockSize = mtdDev[count]->erasesize;
            }
            else
            {
                if (s_FlFlash.erasableBlockSize != mtdDev[count]->erasesize){
                    printf("the flash erase block isn't equal! \r\n");
                    return NULL;
                 }
            }
        }
        s_FlFlash.noOfChips = count;
        s_FlFlash.read = flash_read;
        s_FlFlash.write= flash_write;
        s_FlFlash.erase= flash_erase;
        s_FlFlash.eraseAll= flash_eraseAll;
        firstRun = 0;
   }

   return &s_FlFlash;
}

/*----------------------------End of FlFlash.c-------------------------------*/
