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
                           |OFS: 0K       LEN: 512k     |
                           |              字库                           |
                           +----------------------------+
                           |OFS: 0K       LEN: 8M       |
                           |              FTL管理                    |
                           +----------------------------+ */

/** FLASH最小擦除block大小64K */
#define FLASH_BLOCK_SIZE            (64u * 1024u)

/** FLASH扇区大小(读写的最小单位) */
#define FLASH_SECTOR_SIZE           (512u)

/** FLASH最大数量 */
#define MAX_FLASH_CHIP_NUM          (1u)

/** flash尺寸4M */
#define FLASH_CHIP_SIZE             (0x400000l)

#define FLASH_FTL_OFFSET            (0x80000)

/**< 汉字库16*16起始偏移 */
#define ZK_OFFSET_16_16             (0)

/**< 汉字库16*16大小264K */
#define ZK_SIZE_16_16               (264 * 1024)

/**< 字符库16*8起始偏移 */
#define ZK_OFFSET_16_8              (ZK_OFFSET_16_16 + ZK_SIZE_16_16)

/**< 汉字库16*8大小4K */
#define ZK_SIZE_16_8                (4 * 1024)

/**< 字符库12*8起始偏移 */
#define ZK_OFFSET_12_8              (ZK_OFFSET_16_8 + ZK_SIZE_16_8)

/**< 字符库12*8大小4k */
#define ZK_SIZE_12_8                (4 * 1024)

/**< 字符库12*12起始偏移 */
#define ZK_OFFSET_12_12             (ZK_OFFSET_12_8 + ZK_SIZE_12_8)

/**< 字符库12*12大小200k */
#define ZK_SIZE_12_12               (200 * 1024)
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
