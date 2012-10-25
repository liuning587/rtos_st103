/**
 ******************************************************************************
 * @file       flash.h
 * @version    V0.0.1
 * @brief      sched.
 * @details    This file including all API functions's implement of flash.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#ifndef __FLASH_H__
#define __FLASH_H__
/*------------------------------------------------------------------------------ 
Section: Includes 
------------------------------------------------------------------------------*/ 
#include <types.h>
#include <errno.h>
/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/

                        /* +----------------------------+
                           |OFS: 0K       LEN: 8M       |
                           |              FTL管理                    |
                           +----------------------------+ */

/** FLASH最小擦除block大小64K */
#define FLASH_BLOCK_SIZE            (64u * 1024u)

/** FLASH扇区大小(读写的最小单位) */
#define FLASH_SECTOR_SIZE           (512u)

/** FLASH最大数量 */
#define MAX_FLASH_CHIP_NUM          (1u)

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
struct mtd_dev_s
{
  uint32_t chipsize;
  uint32_t erasesize;
  uint32_t noofblock;

  FLStatus (*erase)(struct mtd_dev_s *dev, int32_t startblock, int32_t nblocks);
  FLStatus (*write)(struct mtd_dev_s *dev, uint32_t address, void *buffer, int32_t len);
  FLStatus (*read)(struct mtd_dev_s *dev, uint32_t address, void *buffer, int32_t len);
  int32_t (*ioctl)( struct mtd_dev_s *dev, int32_t cmd, unsigned long arg);
};

/* Flash array identification structure */

typedef struct tFlash FLFlash;      /**< Forward definition */

struct tFlash
{
    uint32_t erasableBlockSize;     /**< Smallest physically erasable size */
    uint32_t    chipSize;           /**< chip size */
    int32_t    noOfChips;           /**< no of chips in array */

    FLStatus (*erase)(FLFlash *flash, int32_t startblock, int32_t nblocks);
    FLStatus (*write)(FLFlash *flash, uint32_t address, void *buffer, int32_t len);
    FLStatus (*read)(FLFlash *flash, uint32_t address, void *buffer, int32_t len);
    FLStatus (*eraseAll)(FLFlash *flash);
} ;
  


typedef struct flash_tbl_entry
{
    int32_t unit;                   /**< This device's unit # */
    /**
     *  The Load function.
     */
    struct mtd_dev_s * (*mtdLoadFunc) (int32_t, int32_t);
    int32_t param1;
    int32_t param2;
                                    /**< Has this been processed? */
} MTD_TBL_ENTRY;
    
#define MTD_TBL_END NULL

 
/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
extern FLFlash* flash_init(void);

#endif /* __FLASH_H__ */

/*----------------------------End of FlFlash.h-------------------------------*/
