/**
 ******************************************************************************
 * @file       ftllite.c
 * @version    V0.0.1
 * @brief      flash ftl.
 * @details    This file including all API functions's implement of ftllite.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */

/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <ftl.h>
#include <flash.h>
//#include <config.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
/* custom define */
#ifndef FLASH_CHIP_SIZE
#define FLASH_CHIP_SIZE         (0x800000l)         /**< flash尺寸8M */
#endif

#define FLASH_CHIP_NUM          (1u)                /**< flash数量 */
#define FLASH_ERASE_BLOCK_SIZE  0x10000l            /**< 擦除基本单位大小64k */
#define UNIT_SIZE               0x10000
#define UNIT_SIZE_BITS          16
#define MAX_UNIT_COUNT          ((FLASH_CHIP_SIZE*FLASH_CHIP_NUM)>>UNIT_SIZE_BITS)

#define BOOT_IMAGE_LEN          0                   /**< 预留给使用 */
#define PERCENT_USE             98                  /**< 最大使用百分比 */
#define NO_OF_SPARE_UNITS       1
//#define VM_ADDRESS_LIMIT      0x10000   /* 64K 内存直接访问寻址 */
#define VM_ADDRESS_LIMIT        0

/* Implementation constants  */
#define SECTOR_SIZE_BITS        9                   /**< 512 bytes */
#define SECTOR_SIZE             (1 << SECTOR_SIZE_BITS)
#define SECTOR_OFFSET_MASK      (SECTOR_SIZE - 1)

#define PAGE_SIZE_BITS          (SECTOR_SIZE_BITS + (SECTOR_SIZE_BITS - 2))
#define PAGE_SIZE               (1L << PAGE_SIZE_BITS)
#define MAX_PAGE_COUNT          ((FLASH_CHIP_SIZE*FLASH_CHIP_NUM)>>PAGE_SIZE_BITS)

#define UNASSIGNED_SECTOR       0xffff
#define UNASSIGNED_ADDRESS      0xffffffffl
#define DELETED_ADDRESS         0
#define    DELETED_SECTOR       0
#define UNASSIGNED_UNIT_NO      0xffff
#define MARKED_FOR_ERASE        0x7fff


/* Structure of data on a unit */
#define FREE_SECTOR             0xffffffffl
#define GARBAGE_SECTOR          0
#define ALLOCATED_SECTOR        0xfffffffel
#define FORMAT_SECTOR           0x30
#define DATA_SECTOR             0x40
#define REPLACEMENT_PAGE        0x60
#define BAD_SECTOR              0x70
#define FREE_UNIT              -0x400    /* Indicates a transfer unit */

#define ADDRESSES_PER_SECTOR    (SECTOR_SIZE / sizeof(uint32_t))

#define cannotWriteOver(newContents, oldContents)        \
                                ((newContents) & ~(oldContents))


#define HEAP_SIZE                        \
        (MAX_PAGE_COUNT+(VM_ADDRESS_LIMIT>>SECTOR_SIZE_BITS))*sizeof(LogicalSectorNo)  \
        +   MAX_UNIT_COUNT * (sizeof(Unit) + sizeof(UnitPtr))


#define dummyUnit               ((const UnitHeader *) 0)  /**< for offset calculations */
#define logicalUnitNoOffset     ((char *) &dummyUnit->logicalUnitNo -    \
                                     (char *) dummyUnit)
#define eraseCountOffset        ((char *) &dummyUnit->eraseCount -    \
                                     (char *) dummyUnit)

#define buffer                  (*vol.volBuffer)
/** Virtual map cache (shares memory with buffer) */
#define mapCache                ((uint32_t *) buffer.data)


/** Unit header buffer (shares memory with buffer) */
#define uh                      ((UnitHeader *) buffer.data)

/** Transfer sector buffer (shares memory with buffer) */
#define sectorCopy              ((uint32_t *) buffer.data)

#define MapBuf                  ((uint32_t *)vol.mapBuffer.data)

#define vol                     (*pVol)


#define DEBUG_PRINT             printf
#undef DEBUG_PRINT

/*-----------------------------------------------------------------------------
 Section: Private Type Definitions
 ----------------------------------------------------------------------------*/

typedef int32_t     FLBoolean;
typedef uint16_t    SectorNo;           /**< MAX Sector count is 0xffff */
typedef int32_t     LogicalAddress;     /**< Byte address of media in logical unit no. order. */
typedef int32_t     VirtualAddress;     /**< Byte address of media as specified
                                             by Virtual Map. */
typedef SectorNo    LogicalSectorNo;    /**< A logical sector no. is given
                                             by dividing its logical address by
                                             the sector size */
typedef SectorNo    VirtualSectorNo;    /**< A virtual sector no. is such that
                                             the first page is no. 0, the 2nd
                                             is 1 etc.
                                             The virtual sector no. is given
                                             by dividing its virtual address by
                                             the sector size and adding the
                                             number of pages (result always
                                             positive). */
typedef uint16_t      UnitNo;


typedef struct {
    uint8_t       data[SECTOR_SIZE];      /**< sector buffer */
    SectorNo      sectorNo;               /**< current sector in buffer */
    void *        owner;                  /**< owner of buffer */
    FLBoolean     dirty;                  /**< sector in buffer was changed */
    FLBoolean     checkPoint;             /**< sector in buffer must be flushed */
} FLBuffer;

typedef struct tUintHeader{
    char_t        formatPattern[15];
    uint8_t       noOfTransferUnits;      /**< no. of transfer units */
    uint32_t      wearLevelingInfo;
    uint16_t      logicalUnitNo;
    uint8_t       log2SectorSize;
    uint8_t       log2UnitSize;
    uint16_t      firstPhysicalEUN;       /**< units reserved for boot image */
    uint16_t      noOfUnits;              /**< no. of formatted units */
    uint32_t      virtualMediumSize;      /**< virtual size of volume */
    uint32_t      directAddressingMemory; /**< directly addressable memory */
    uint16_t      noOfPages;              /**< no. of virtual pages */
    uint8_t       flags;
    uint32_t      BAMoffset;
    uint32_t      eraseCount;
    uint8_t       reserved[12];
} UnitHeader;

typedef struct {
    uint8_t       data[SECTOR_SIZE + sizeof(UnitHeader)]; /**< sector buffer */
    SectorNo      mappedSectorNo;
    void *        mappedSector;
    uint32_t      mappedSectorAddress;
    FLBoolean     remapped;               /**< set to TRUE whenever the mapbuf is moved */
} FLMapBuf;

typedef struct {
    int16_t       noOfFreeSectors;
    int16_t       noOfGarbageSectors;
} Unit;

typedef Unit *UnitPtr;

typedef struct tTLrec {
    FLBoolean        badFormat;               /**< true if FTL format is bad */

    VirtualSectorNo   totalFreeSectors;       /**< Free sectors on volume */
    SectorNo          virtualSectors;         /**< size of virtual volume */
    uint32_t          unitSizeBits;           /**< log2 of unit size */
    uint32_t          erasableBlockSizeBits;  /**< log2 of erasable block size */
    UnitNo            noOfUnits;
    UnitNo            noOfTransferUnits;
    UnitNo            firstPhysicalEUN;
    int32_t           noOfPages;
    VirtualSectorNo   directAddressingSectors;/**< no. of directly addressable sectors */
    VirtualAddress    directAddressingMemory; /**< end of directly addressable memory */
    uint32_t          unitOffsetMask;         /**< = 1 << unitSizeBits - 1 */
    uint32_t          bamOffset;
    uint32_t          sectorsPerUnit;
    uint32_t          unitHeaderSectors;      /**< sectors used by unit header */

    Unit *            physicalUnits;          /**< unit table by physical no. */
    Unit **           logicalUnits;           /**< unit table by logical no. */
    Unit *            transferUnit;           /**< The active transfer unit */
    LogicalSectorNo * pageTable;              /**< page translation table */
                                              /**< directly addressable sectors */
    LogicalSectorNo   replacementPageAddress;
    VirtualSectorNo   replacementPageNo;


    unsigned long     currWearLevelingInfo;
    unsigned long     maxEraseCount;

    FLBuffer *        volBuffer;              /**< Define a sector buffer */

    FLMapBuf          mapBuffer;

    FLFlash*          flash;

    char_t            heap[HEAP_SIZE];

} Flare;



/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Private Variables
 ----------------------------------------------------------------------------*/
static uint8_t FORMAT_PATTERN[15] = {0x13, 3, 'C', 'I', 'S',
                 0x46, 57, 0, 'F', 'T', 'L', '1', '0', '0', 0};
static SEM_ID semFtlRW;
static char_t bufFtlRw[SECTOR_SIZE];
static FLBuffer s_volBuffers;
static Flare s_flare;
/*-----------------------------------------------------------------------------
 Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
extern void sysFeedDog();

#define checkStatus(exp)      { FLStatus status = (exp); \
                                if (status != flOK)     \
                                    return status; }

/*-----------------------------------------------------------------------------
 Section: Public Function
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief   从flash中读取数据到ftl缓存
 * @param[in]   Flare vol           : Flare描述
 * @param[in]   uint32_t address    : 起始地址
 * @param[in]   uint32_t length     : 长度
 * @retval      缓存首地址
 *
 * @details
 *
 * @note
 * 注意：程序中的vol已经宏定义了
 * #define vol (*pVol)
 ******************************************************************************
 */
static void*
flashMap(Flare vol, uint32_t address, uint32_t length)
{
    if (length > SECTOR_SIZE + sizeof(UnitHeader)) return NULL;
    vol.flash->read(vol.flash, address, MapBuf, length);
    vol.mapBuffer.remapped = TRUE;
    return (void *)MapBuf;
}

/**
 ******************************************************************************
 * @brief      取单元物理页地址
 * @param[in]  vol        : Pointer identifying drive
 * @param[in]  unit       : unit pointer
 * @retval     physical address of unit
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint32_t
physicalBase(Flare vol, const Unit *unit)
{
    return (uint32_t)(unit - vol.physicalUnits) << vol.unitSizeBits;
}

/**
 ******************************************************************************
 * @brief       逻辑扇区地址转换为物理扇区地址
 * @param[in]  vol        : Pointer identifying drive
 * @param[in]  address    : logical sector no.
 *
 * @retval     uint32    : physical address of sector
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint32_t
logical2Physical(Flare vol, LogicalSectorNo address)
{
    return physicalBase(&vol,
            vol.logicalUnits[address >> (vol.unitSizeBits - SECTOR_SIZE_BITS)])
          | (((uint32_t) address << SECTOR_SIZE_BITS) & vol.unitOffsetMask);
}

/**
 ******************************************************************************
 * @brief   读取一个逻辑扇区至缓存,返回缓存首地址
 * @param[in]  vol        : Pointer identifying drive
 * @param[in]  address    : logical sector no.
 * @retval     Pointer to sector on Flash
 *
 * @details
 *  Maps a logical sector and returns pointer to physical Flash location.
 * @note
 ******************************************************************************
 */
static void *
mapLogical(Flare vol, LogicalSectorNo address)
{
    return flashMap(&vol, logical2Physical(&vol, address), SECTOR_SIZE);
}

/**
 ******************************************************************************
 * @brief 获取扇区映射表数据
 * @param[in]  vol          : Pointer identifying drive
 * @param[in]  sectorNo     : BAM entry no
 *
 * @retval     Offset of BAM entry in unit
 *
 * @details Returns unit offset of given BAM entry
 *
 * @note
 ******************************************************************************
 */
static int
allocEntryOffset(Flare vol, int sectorNo)
{
    return (int)(vol.bamOffset + sizeof(VirtualAddress) * sectorNo);
}

/**
 ******************************************************************************
 * @brief 读取物理单元头信息
 * @param[in]  vol              : Pointer identifying drive
 * @param[in]  unit             : Unit to map header
 * @param[out] blockAllocMap    : (optional) Pointer to mapped BAM
 *
 * @retval     Pointer to mapped unit header
 *
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static UnitHeader*
mapUnitHeader(Flare vol,
        const Unit *unit,
        uint32_t  **blockAllocMap)
{
    UnitHeader  *unitHeader;
    int length = sizeof(UnitHeader);

    if (blockAllocMap != NULL)
        length = allocEntryOffset(&vol, vol.sectorsPerUnit);
    unitHeader = (UnitHeader *)flashMap(&vol, physicalBase(&vol, unit), length);
    if (blockAllocMap != NULL)
        *blockAllocMap = (uint32_t *)((char *)unitHeader + allocEntryOffset(&vol, 0));

    return unitHeader;
}

/**
 ******************************************************************************
 * @brief 设置虚拟页映射缓存
 * @param[in]  vol        : Pointer identifying drive
 * @param[out] pageNo     : Page no. to copy to map cache
 * @retval     None
 *
 * @details
 *
 * @note
 *  Sets up map cache sector to contents of specified Virtual Map page
 ******************************************************************************
 */
static void
setupMapCache(Flare vol, VirtualSectorNo pageNo)
{
  vol.flash->read(vol.flash,
          logical2Physical(&vol, vol.pageTable[pageNo]),
          mapCache,
          SECTOR_SIZE);
  if (pageNo == vol.replacementPageNo) {
    uint32_t* replacementPage = (uint32_t*)mapLogical(&vol, vol.replacementPageAddress);

    for (int i = 0; (unsigned)i < ADDRESSES_PER_SECTOR; i++) {
      if (mapCache[i] == DELETED_ADDRESS)
          mapCache[i] = replacementPage[i];
    }
  }
  buffer.sectorNo = pageNo;
  buffer.owner = &vol;
}

/**
 ******************************************************************************
 * @brief      虚拟扇区号 转换为 逻辑扇区号
 * @param[in]  vol        : Pointer identifying drive
 * @param[in]  sectorNo   : Virtual sector no.
 * @retval     逻辑扇区号
 *
 * @details
 *
 * @note
 *  Translates virtual sector no. to logical sector no.
 ******************************************************************************
 */
static LogicalSectorNo
virtual2Logical(Flare vol, VirtualSectorNo sectorNo)
{
    LogicalAddress virtualMapEntry;

    if (sectorNo < vol.directAddressingSectors)
        return vol.pageTable[sectorNo];
    else {
        unsigned pageNo;
        int sectorInPage;

        sectorNo -= vol.noOfPages;
        pageNo = (int) (sectorNo >> (PAGE_SIZE_BITS - SECTOR_SIZE_BITS));
        sectorInPage = (int) (sectorNo) % ADDRESSES_PER_SECTOR;

        if (buffer.sectorNo != pageNo || buffer.owner != &vol)
            setupMapCache(&vol, pageNo);
        virtualMapEntry = mapCache[sectorInPage];

        return (LogicalSectorNo) (virtualMapEntry >> SECTOR_SIZE_BITS);
    }
}

/**
 ******************************************************************************
 * @brief      校验FTL单元头信息
 * @param[in]  unitHeader    : Pointer to unit header
 *
 * @retval     TRUE if header is correct. FALSE if not.
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static FLBoolean
verifyFormat(UnitHeader *unitHeader)
{

  return memcmp(unitHeader->formatPattern + 2,
         FORMAT_PATTERN + 2,
         sizeof unitHeader->formatPattern - 2) == 0;
}

/**
 ******************************************************************************
 * @brief      格式化物理单元
 * @param[in]  vol        : Pointer identifying drive
 * @param[in]  unit       : Unit to format
 * @retval     FLStatus   : 0 on success, failed otherwise
 *
 * @details
 *
 * @note
 * Formats a unit by erasing it and writing a unit header.
 ******************************************************************************
 */
static FLStatus formatUnit(Flare vol,  Unit *unit)
{
    unsigned unitHeaderLength = allocEntryOffset(&vol, vol.unitHeaderSectors);
    uint32_t eraseCount=0;

    /* 读取擦除次数 */
    vol.flash->read(vol.flash,
            physicalBase(&vol,unit) + eraseCountOffset,
            &eraseCount,
            4);

    if (eraseCount == 0xffffffffl) eraseCount = 0;

    unit->noOfFreeSectors = FREE_UNIT;    /* 记为空闲单元 */
    unit->noOfGarbageSectors = 0;

    /* 擦除 */
    checkStatus(vol.flash->erase(vol.flash,
            (int) (physicalBase(&vol,unit) >> vol.erasableBlockSizeBits),
            1 << (vol.unitSizeBits - vol.erasableBlockSizeBits)));


    /* We will copy the unit header as far as the format entries of the BAM
     from another unit (logical unit 0) */

    buffer.sectorNo = UNASSIGNED_SECTOR;    /* Invalidate map cache so we can
                                               use it as a buffer */
    if (vol.logicalUnits[vol.firstPhysicalEUN]) {
        vol.flash->read(vol.flash,
               physicalBase(&vol, vol.logicalUnits[vol.firstPhysicalEUN]),
               uh,
               unitHeaderLength);
    }

    uh->wearLevelingInfo = ++vol.currWearLevelingInfo;
    uh->eraseCount = eraseCount + 1;    /* 擦除次数++ */
    uh->logicalUnitNo = UNASSIGNED_UNIT_NO;

    /* 写入物理单元头信息 */
    checkStatus(vol.flash->write(vol.flash,
              physicalBase(&vol,unit),
              uh,
              unitHeaderLength));

    return flOK;
}

/**
 ******************************************************************************
 * @brief      挂载一个物理单元
 * @param[in]  vol        : Pointer identifying drive
 * @param[in] unit        : Unit to mount
 * @retval     FLStatus    : 0 on success, failed otherwise
 *
 * @details
 *
 * @note
 * Performs mount scan for a single unit
 ******************************************************************************
 */
static FLStatus
mountUnit(Flare vol, Unit *unit)
{
    unsigned i;
    LogicalSectorNo sectorAddress;
    uint32_t *blockAllocMap;

    UnitHeader *unitHeader = mapUnitHeader(&vol, unit, &blockAllocMap);

    UnitNo logicalUnitNo = unitHeader->logicalUnitNo;

    unit->noOfGarbageSectors = 0;
    unit->noOfFreeSectors = FREE_UNIT;

    if (!verifyFormat(unitHeader) ||            /* 单元格式无效 */
      ((logicalUnitNo != UNASSIGNED_UNIT_NO) &&
       ((logicalUnitNo >= vol.noOfUnits) ||
        (logicalUnitNo < vol.firstPhysicalEUN) ||
        vol.logicalUnits[logicalUnitNo]))) {
        if (vol.transferUnit == NULL)
            vol.transferUnit = unit;
        return flBadFormat;
    }

    if (logicalUnitNo == UNASSIGNED_UNIT_NO) {
        vol.transferUnit = unit;
        return flOK;        /* this is a transfer unit */
    }

    /* 更新磨损级别 */
    if (unitHeader->wearLevelingInfo > vol.currWearLevelingInfo &&
      unitHeader->wearLevelingInfo != 0xffffffffl)
        vol.currWearLevelingInfo = unitHeader->wearLevelingInfo;

    /* 更新最大擦除次数 */
    if (unitHeader->eraseCount > vol.maxEraseCount &&
      unitHeader->eraseCount != 0xffffffffl)
        vol.maxEraseCount = unitHeader->eraseCount;

    /* count sectors and setup virtual map */
    sectorAddress =
     ((LogicalSectorNo) logicalUnitNo << (vol.unitSizeBits - SECTOR_SIZE_BITS));
    unit->noOfFreeSectors = 0;
    for (i = 0; i < vol.sectorsPerUnit; i++, sectorAddress++) {
        VirtualAddress allocMapEntry = blockAllocMap[i];

        if (allocMapEntry == GARBAGE_SECTOR || (unsigned long)allocMapEntry == ALLOCATED_SECTOR)
            unit->noOfGarbageSectors++; /* 统计垃圾扇区 */
        else if ((unsigned long)allocMapEntry == FREE_SECTOR) {
            unit->noOfFreeSectors++;  /* 统计空闲扇区 */
            vol.totalFreeSectors++;
        }
        else if (allocMapEntry < vol.directAddressingMemory) { /* 直接访问地址 */
            char signature = (short) (allocMapEntry) & SECTOR_OFFSET_MASK;
            if (signature == DATA_SECTOR || signature == REPLACEMENT_PAGE) {
                int pageNo = (int) (allocMapEntry >> SECTOR_SIZE_BITS) + vol.noOfPages;
                if (pageNo >= 0)
                    if (signature == DATA_SECTOR)
                        vol.pageTable[pageNo] = sectorAddress;
                    else {
                        vol.replacementPageAddress = sectorAddress;
                        vol.replacementPageNo = pageNo;
                    }
            }
        }
    }

    /* Place the logical mapping of the unit */
    vol.mapBuffer.mappedSectorNo = UNASSIGNED_SECTOR;
    vol.logicalUnits[logicalUnitNo] = unit;

    return flOK;
}

/**
 ******************************************************************************
 * @brief      分配一个逻辑单元
 * @param[in]  vol        : Pointer identifying drive
 * @param[in]  unit       : Unit to assign
 * @param[in]  logicalUnitNo : 逻辑单元号
 *
 * @retval     FLStatus    : 0 on success, failed otherwise
 *
 * @details
 *
 * @note
 * Assigns a logical unit no. to a unit
 * 实质上是讲逻辑单元号写到物理单元头信息中
 ******************************************************************************
 */
static FLStatus
assignUnit(Flare vol, Unit *unit, UnitNo logicalUnitNo)
{
    uint16_t unitNoToWrite = logicalUnitNo;

    return vol.flash->write(vol.flash,
            physicalBase(&vol, unit) + logicalUnitNoOffset,
            &unitNoToWrite,
            sizeof unitNoToWrite);
}

/**
 ******************************************************************************
 * @brief 找出磨损最少的单元
 * @param[in]  vol        : Pointer identifying drive
 * @param[in]  leastUsed  : Whether most garbage space is the criterion
 * @retval     Best unit to transfer
 *
 * @details
 *
 * @note
 * Find best candidate for unit transfer, usually on the basis of which
 * unit has the most garbage space. A lower wear-leveling info serves
 * as a tie-breaker. If 'leastUsed' is NOT specified, then the least
 * wear-leveling info is the only criterion.
 ******************************************************************************
 */
static UnitNo
bestUnitToTransfer(Flare vol, FLBoolean leastUsed)
{
    UnitNo i;

    int mostGarbageSectors = 1;
    unsigned long int leastWearLevelingInfo = 0xffffffffl;
    UnitNo bestUnitSoFar = UNASSIGNED_UNIT_NO;

    for (i = 0; i < vol.noOfUnits; i++) {
        Unit *unit = vol.logicalUnits[i];
        if (unit && (!leastUsed || (unit->noOfGarbageSectors >= mostGarbageSectors))) {
            UnitHeader  *unitHeader = mapUnitHeader(&vol, unit, NULL);
            if ((leastUsed && (unit->noOfGarbageSectors > mostGarbageSectors)) ||
                ((unitHeader->wearLevelingInfo) < leastWearLevelingInfo)) {
                    mostGarbageSectors = unit->noOfGarbageSectors;
                    leastWearLevelingInfo = (unitHeader->wearLevelingInfo);
                    bestUnitSoFar = i;
            }
        }
    }

    return bestUnitSoFar;
}

/**
 ******************************************************************************
 * @brief 将fromUnitNo拷贝至toUnit(只拷贝有效的物理扇区)
 * @param[in]  vol        : Pointer identifying drive
 * @param[in]  toUnit     : Target transfer unit
 * @param[in]  fromUnitNo : Source logical unit no.
 * @retval     FLStatus   : 0 on success, failed otherwise
 *
 * @details
 *
 * @note
 * Performs a unit transfer from a selected unit to a tranfer unit.
 * A side effect is to invalidate the map cache (reused as buffer).
 ******************************************************************************
 */
static FLStatus
unitTransfer(Flare vol, Unit *toUnit, UnitNo fromUnitNo)
{
    unsigned i;
    Unit *fromUnit = vol.logicalUnits[fromUnitNo];

    UnitHeader  *transferUnitHeader = mapUnitHeader(&vol, toUnit, NULL);
    /* 若之前toUnit格式失败,则现在格式化 */
    if (!verifyFormat(transferUnitHeader) ||
            transferUnitHeader->logicalUnitNo != UNASSIGNED_UNIT_NO)
        /* previous formatting failed or did not complete.         */
        checkStatus(formatUnit(&vol, toUnit));

    /* Should the transfer not complete, the unit is marked to be erased */
    checkStatus(assignUnit(&vol, toUnit, MARKED_FOR_ERASE));    /* 标记为擦除 */

    /* copy the block allocation table and the good sectors */
    /* 拷贝有用的扇区和映射表 */
    for (i = 0; i < vol.sectorsPerUnit;) {
        int j;

        FLBoolean needToWrite = FALSE;
        int firstOffset = allocEntryOffset(&vol, i);

        /* Up to 128 bytes of the BAM are processed per loop */
        int nEntries = (128 - (firstOffset & 127)) / sizeof(VirtualAddress);

        /* We are going to use the Virtual Map cache as our sector buffer in the */
        /* transfer, so let's better invalidate the cache first.           */
        buffer.sectorNo = UNASSIGNED_SECTOR;

        /* Read some of the BAM */
        vol.flash->read(vol.flash,
               physicalBase(&vol,fromUnit) + firstOffset,
               sectorCopy,
               nEntries * sizeof(VirtualAddress));

        /* Convert garbage entries to free entries */
        for (j = 0; j < nEntries && i+j < vol.sectorsPerUnit; j++) {
            unsigned bamSignature = (unsigned) sectorCopy[j] & SECTOR_OFFSET_MASK;
            if (bamSignature == DATA_SECTOR || bamSignature == REPLACEMENT_PAGE)
                needToWrite = TRUE;
            else if (bamSignature != FORMAT_SECTOR)
                sectorCopy[j]=FREE_SECTOR;
        }

        if (needToWrite) {
            FLStatus status;

            /* Write new BAM, and copy sectors that need to be copied */
            status = vol.flash->write(vol.flash,
                  physicalBase(&vol, toUnit) + firstOffset,
                  sectorCopy,
                  nEntries * sizeof(VirtualAddress));
            if (status != flOK) {
                return status;
            }

            for (j = 0; j < nEntries && i+j < vol.sectorsPerUnit; j++) {
                unsigned bamSignature = (unsigned) sectorCopy[j] & SECTOR_OFFSET_MASK;
                if (bamSignature == DATA_SECTOR || bamSignature == REPLACEMENT_PAGE) { /* a good sector */
                    uint32_t sectorOffset = (uint32_t) (i+j) << SECTOR_SIZE_BITS;

                    vol.flash->read(vol.flash,
                            physicalBase(&vol, fromUnit) + sectorOffset,
                            sectorCopy,
                            SECTOR_SIZE);
                    status = vol.flash->write(vol.flash,
                            physicalBase(&vol, toUnit) + sectorOffset,
                            sectorCopy,
                            SECTOR_SIZE);
                    if (status != flOK) {
                        return status;
                    }
                    vol.flash->read(vol.flash,
                            physicalBase(&vol, fromUnit) + firstOffset,
                            sectorCopy,
                            nEntries * sizeof(VirtualAddress));
                }
            }
        }

        i += nEntries;
    }

    /* Write the new logical unit no. */
    checkStatus(assignUnit(&vol, toUnit, fromUnitNo));  /* 写逻辑单元号 */

    /* Mount the new unit in place of old one */
    vol.logicalUnits[fromUnitNo] = NULL;
    if (mountUnit(&vol,toUnit) == flOK) {
        vol.totalFreeSectors -= fromUnit->noOfFreeSectors;  /* 更新空闲扇区 */

        /* Finally, format the source unit (the new transfer unit) */
        vol.transferUnit = fromUnit;
        formatUnit(&vol, fromUnit);    /* nothing we can or should do if this fails */
    }
    else {        /* Something went wrong */
        vol.logicalUnits[fromUnitNo] = fromUnit;    /* reinstate original unit */
        return flGeneralFailure;
    }

    return flOK;
}


/*----------------------------------------------------------------------*/
/*                 g a r b a g e C o l l e c t            */
/*                                    */
/* Performs a unit transfer, selecting a unit to transfer and a        */
/* transfer unit.                                                       */
/*                                                                      */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus garbageCollect(Flare vol)
{
  FLStatus status;
  UnitNo fromUnitNo;

  if (vol.transferUnit == NULL)
    return flWriteProtect;    /* Cannot recover space without a spare unit */

  fromUnitNo = bestUnitToTransfer(&vol,(rand () & 0xff) >= 4);
  if (fromUnitNo == UNASSIGNED_UNIT_NO)
    return flGeneralFailure;    /* nothing to collect */

  /* Find a unit we can transfer to.                */
  status = unitTransfer(&vol,vol.transferUnit,fromUnitNo);
  if (status == flWriteFault) {
    int i;
    Unit *unit = vol.physicalUnits;

    for (i = 0; i < vol.noOfUnits; i++, unit++) {
      if (unit->noOfGarbageSectors == 0 && unit->noOfFreeSectors < 0) {
    if (unitTransfer(&vol,unit,fromUnitNo) == flOK)
      return flOK;    /* found a good one */
      }
    }
  }

  return status;
}



/*----------------------------------------------------------------------*/
/*                      d e f r a g m e n t                */
/*                                    */
/* Performs unit transfers to arrange a minimum number of writable    */
/* sectors.                                                             */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*    sectorsNeeded    : Minimum required sectors            */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus defragment(Flare vol, long  *sectorsNeeded)
{
  while ((long)(vol.totalFreeSectors) < *sectorsNeeded

     ) {
    if (vol.badFormat)
      return flBadFormat;


    checkStatus(garbageCollect(&vol));
  }


  *sectorsNeeded = vol.totalFreeSectors;

  return flOK;
}


/*----------------------------------------------------------------------*/
/*            b e s t U n i t T o A l l o c a t e            */
/*                                    */
/* Finds the best unit from which to allocate a sector. The unit    */
/* selected is the one with most free space.                */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*                                                                      */
/* Returns:                                                             */
/*    Best unit to allocate                        */
/*----------------------------------------------------------------------*/

static Unit *bestUnitToAllocate(Flare vol)
{
  int i;

  int mostFreeSectors = 0;
  Unit *bestUnitSoFar = NULL;

  for (i = 0; i < vol.noOfUnits; i++) {
    Unit *unit = vol.logicalUnits[i];

    if (unit && unit->noOfFreeSectors > mostFreeSectors) {
      mostFreeSectors = unit->noOfFreeSectors;
      bestUnitSoFar = unit;
    }
  }

  return bestUnitSoFar;
}


/*----------------------------------------------------------------------*/
/*               f i n d F r e e S e c t o r            */
/*                                    */
/* The allocation strategy goes this way:                               */
/*                                                                      */
/* We try to make consecutive virtual sectors physically consecutive if */
/* possible. If not, we prefer to have consecutive sectors on the same  */
/* unit at least. If all else fails, a sector is allocated on the unit  */
/* with most space available.                                           */
/*                                                                      */
/* The object of this strategy is to cluster related data (e.g. a file  */
/* data) in one unit, and to distribute unrelated data evenly among all */
/* units.                                */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*    sectorNo        : virtual sector no. that we want to allocate.    */
/*                                    */
/* Returns:                                                             */
/*    newAddress    : Allocated logical sector no.            */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus findFreeSector(Flare vol,
                 VirtualSectorNo sectorNo,
                 LogicalSectorNo *newAddress)
{
  unsigned iSector;
  uint32_t  *blockAllocMap;
  UnitHeader  *unitHeader;

  Unit *allocationUnit = NULL;

  LogicalSectorNo previousSectorAddress =
     sectorNo > 0 ? virtual2Logical(&vol,sectorNo - 1) : UNASSIGNED_SECTOR;
#ifdef DEBUG_PRINT
  DEBUG_PRINT("Debug: findFreeSector -> %d !!\n\t", sectorNo);
#endif
  if (previousSectorAddress != UNASSIGNED_SECTOR &&
      previousSectorAddress != DELETED_SECTOR) {
    allocationUnit =
    vol.logicalUnits[previousSectorAddress >> (vol.unitSizeBits - SECTOR_SIZE_BITS)];
    if (allocationUnit->noOfFreeSectors > 0) {
      unsigned int sectorIndex = ((unsigned) previousSectorAddress & (vol.sectorsPerUnit - 1)) + 1;
      uint32_t  *nextSectorAddress =
       (uint32_t  *) flashMap(&vol,
                     physicalBase(&vol,allocationUnit) +
                                          allocEntryOffset(&vol, sectorIndex),
                     sizeof(VirtualAddress));
      if (sectorIndex < vol.sectorsPerUnit && *nextSectorAddress == FREE_SECTOR) {
    /* can write sequentially */
    *newAddress = previousSectorAddress + 1;
    return flOK;
      }
    }
    else
      allocationUnit = NULL;    /* no space here, try elsewhere */
  }

  if (allocationUnit == NULL)
    allocationUnit = bestUnitToAllocate(&vol);
  if (allocationUnit == NULL)    /* No ? then all is lost */ {
#ifdef DEBUG_PRINT
      DEBUG_PRINT("Debug: findFreeSector -> Unable to find free sector!!\n\t");
#endif
    return flGeneralFailure;
  }
  unitHeader = mapUnitHeader(&vol,allocationUnit,&blockAllocMap);
  for (iSector = vol.unitHeaderSectors; iSector < vol.sectorsPerUnit; iSector++) {
    if (blockAllocMap[iSector] == FREE_SECTOR) {
      *newAddress = ((LogicalSectorNo) (unitHeader->logicalUnitNo) << (vol.unitSizeBits - SECTOR_SIZE_BITS)) +
            iSector;
      return flOK;
    }
  }
#ifdef DEBUG_PRINT
    DEBUG_PRINT("Debug: findFreeSector -> Problem marking allocation map!!\n\t");
#endif

  return flGeneralFailure;    /* what are we doing here ? */
}


/*----------------------------------------------------------------------*/
/*                   m a r k A l l o c M a p            */
/*                                    */
/* Writes a new value to a BAM entry.                    */
/*                                                                      */
/* This routine also updates the free & garbage sector counts.        */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*    sectorAddress    : Logical sector no. whose BAM entry to mark    */
/*    allocMapEntry    : New BAM entry value                */
/*    overwrite    : Whether we are overwriting some old value    */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus markAllocMap(Flare vol,
               LogicalSectorNo sectorAddress,
               VirtualAddress allocMapEntry,
               FLBoolean overwrite)
{
  UnitNo unitNo = (UnitNo) (sectorAddress >> (vol.unitSizeBits - SECTOR_SIZE_BITS));
  Unit *unit = vol.logicalUnits[unitNo];
  int sectorInUnit = (unsigned) sectorAddress & (vol.sectorsPerUnit - 1);
  uint32_t bamEntry;

  if (unitNo >= vol.noOfUnits - vol.noOfTransferUnits)
    return flGeneralFailure;

  if (allocMapEntry == GARBAGE_SECTOR)
    unit->noOfGarbageSectors++;
  else if (!overwrite) {
    unit->noOfFreeSectors--;
    vol.totalFreeSectors--;
  }

  bamEntry=allocMapEntry;

  return vol.flash->write(vol.flash,
            physicalBase(&vol,unit) + allocEntryOffset(&vol,sectorInUnit),
            &bamEntry,
            sizeof bamEntry);
}


/*----------------------------------------------------------------------*/
/*                d e l e t e L o g i c a l S e c t o r        */
/*                                    */
/* Marks a logical sector as deleted.                    */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*    sectorAddress    : Logical sector no. to delete            */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus deleteLogicalSector(Flare vol,  LogicalSectorNo sectorAddress)
{
  if (sectorAddress == UNASSIGNED_SECTOR ||
      sectorAddress == DELETED_SECTOR)
    return flOK;

  return markAllocMap(&vol,sectorAddress,GARBAGE_SECTOR,TRUE);
}


/* forward definition */
static FLStatus setVirtualMap(Flare vol,
                VirtualSectorNo sectorNo,
                LogicalSectorNo newAddress);


/*----------------------------------------------------------------------*/
/*               a l l o c a t e A n d W r i t e S e c t o r    */
/*                                    */
/* Allocates a sector or replacement page and (optionally) writes it.    */
/*                                                                      */
/* An allocated replacement page also becomes the active replacement     */
/* page.                                */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*    sectorNo    : Virtual sector no. to write            */
/*    fromAddress    : Address of sector data. If NULL, sector is    */
/*              not written.                    */
/*    replacementPage    : This is a replacement page sector.        */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus allocateAndWriteSector(Flare vol,
                     VirtualSectorNo sectorNo,
                     void  *fromAddress,
                     FLBoolean replacementPage)
{
  FLStatus status;
  LogicalSectorNo sectorAddress;
  VirtualAddress bamEntry =
    ((VirtualAddress) sectorNo - vol.noOfPages) << SECTOR_SIZE_BITS;
  long sectorsNeeded = 1;
#ifdef DEBUG_PRINT
  DEBUG_PRINT("Debug: calling defrgment routine!!\n\t");
#endif
  checkStatus(defragment(&vol,&sectorsNeeded));  /* Organize a free sector */

#ifdef DEBUG_PRINT
  DEBUG_PRINT("Debug: calling routine findFreeSector !!\n\t");
#endif
  checkStatus(findFreeSector(&vol,sectorNo,&sectorAddress));

  if (replacementPage)
    bamEntry |= REPLACEMENT_PAGE;
  else
    bamEntry |= DATA_SECTOR;

  status = markAllocMap(&vol,
            sectorAddress,
            sectorNo < vol.directAddressingSectors ?
              ALLOCATED_SECTOR : bamEntry,
            FALSE);

  if (status == flOK && fromAddress)
    status = vol.flash->write(vol.flash,
            logical2Physical(&vol,sectorAddress),
            fromAddress,
            SECTOR_SIZE);

  if (sectorNo < vol.directAddressingSectors && status == flOK)
    status = markAllocMap(&vol,
              sectorAddress,
              bamEntry,
              TRUE);

  if (status == flOK)
    if (replacementPage) {
      vol.replacementPageAddress = sectorAddress;
      vol.replacementPageNo = sectorNo;
    }
    else
      status = setVirtualMap(&vol,sectorNo,sectorAddress);

  if (status != flOK)
    status = markAllocMap(&vol,sectorAddress,GARBAGE_SECTOR,TRUE);

#ifdef DEBUG_PRINT
  if (status != flOK)
  DEBUG_PRINT("Debug: Bad status code at Allocate and Write sector !\n\t");
  DEBUG_PRINT("Debug: MarkAllocMap returned %d !\n", status);
#endif
  return status;
}


/*----------------------------------------------------------------------*/
/*               c l o s e R e p l a c e m e n t P a g e        */
/*                                    */
/* Closes the replacement page by merging it with the primary page.    */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus closeReplacementPage(Flare vol)
{
  FLStatus status;


  setupMapCache(&vol,vol.replacementPageNo);    /* read replacement page into map cache */
  status = vol.flash->write(vol.flash,
              logical2Physical(&vol,vol.replacementPageAddress),
              mapCache,SECTOR_SIZE);
  if (status != flOK) {
    /* Uh oh. Trouble. Let's replace this replacement page. */
    LogicalSectorNo prevReplacementPageAddress = vol.replacementPageAddress;

    checkStatus(allocateAndWriteSector(&vol,vol.replacementPageNo,mapCache,TRUE));
    checkStatus(deleteLogicalSector(&vol,prevReplacementPageAddress));
  }

  checkStatus(setVirtualMap(&vol,vol.replacementPageNo,vol.replacementPageAddress));
  checkStatus(markAllocMap(&vol,
               vol.replacementPageAddress,
               (((VirtualAddress) vol.replacementPageNo - vol.noOfPages)
                << SECTOR_SIZE_BITS) | DATA_SECTOR,
               TRUE));

  vol.replacementPageNo = UNASSIGNED_SECTOR;

  return flOK;
}


/*----------------------------------------------------------------------*/
/*                    s e t V i r t u a l M a p            */
/*                                    */
/* Changes an entry in the virtual map                    */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*    sectorNo    : Virtual sector no. whose entry is changed.    */
/*    newAddress    : Logical sector no. to assign in Virtual Map.    */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus setVirtualMap(Flare vol,
                VirtualSectorNo sectorNo,
                LogicalSectorNo newAddress)
{
  unsigned pageNo;
  int sectorInPage;
  uint32_t virtualMapEntryAddress;
  uint32_t addressToWrite;
  LogicalAddress oldAddress;
  LogicalSectorNo updatedPage;

  vol.mapBuffer.mappedSectorNo = UNASSIGNED_SECTOR;

  if (sectorNo < vol.directAddressingSectors) {
    checkStatus(deleteLogicalSector(&vol,vol.pageTable[sectorNo]));
    vol.pageTable[sectorNo] = newAddress;
    return flOK;
  }
  sectorNo -= vol.noOfPages;

  pageNo = sectorNo >> (PAGE_SIZE_BITS - SECTOR_SIZE_BITS);
  sectorInPage = (int) (sectorNo) % ADDRESSES_PER_SECTOR;
  updatedPage = vol.pageTable[pageNo];
  virtualMapEntryAddress = logical2Physical(&vol,updatedPage) +
                 sectorInPage * sizeof(LogicalAddress);
  oldAddress = *(uint32_t  *)
    flashMap(&vol,virtualMapEntryAddress,sizeof(LogicalAddress));

  if (oldAddress == DELETED_ADDRESS && vol.replacementPageNo == pageNo) {
    updatedPage = vol.replacementPageAddress;
    virtualMapEntryAddress = logical2Physical(&vol,updatedPage) +
                   sectorInPage * sizeof(LogicalAddress);
    oldAddress = *(uint32_t  *)
      flashMap(&vol,virtualMapEntryAddress,sizeof(LogicalAddress));
  }

  if (newAddress == DELETED_ADDRESS && ((unsigned long)oldAddress == UNASSIGNED_ADDRESS))
    return flOK;

  addressToWrite = (LogicalAddress) newAddress << SECTOR_SIZE_BITS;
  if (cannotWriteOver(addressToWrite,oldAddress)) {
    FLStatus status;

    if (pageNo != vol.replacementPageNo ||
    updatedPage == vol.replacementPageAddress) {
      if (vol.replacementPageNo != UNASSIGNED_SECTOR)
    checkStatus(closeReplacementPage(&vol));
      checkStatus(allocateAndWriteSector(&vol,pageNo,NULL,TRUE));
    }

    status = vol.flash->write(vol.flash,
            logical2Physical(&vol,vol.replacementPageAddress) +
                      sectorInPage * sizeof(LogicalAddress),
            &addressToWrite,
            sizeof addressToWrite);
    if (status != flOK) {
      closeReplacementPage(&vol);
                /* we may get a write-error because a
                   previous cache update did not complete. */
      return status;
    }
    addressToWrite = DELETED_ADDRESS;
    updatedPage = vol.pageTable[pageNo];
  }
  checkStatus(vol.flash->write(vol.flash,
             logical2Physical(&vol,updatedPage) +
                   sectorInPage * sizeof(LogicalAddress),
             &addressToWrite,
             sizeof(addressToWrite)));

  if (buffer.sectorNo == pageNo && buffer.owner == &vol)
    mapCache[sectorInPage] = (LogicalAddress) newAddress << SECTOR_SIZE_BITS;


  return deleteLogicalSector(&vol,(LogicalSectorNo) (oldAddress >> SECTOR_SIZE_BITS));
}


/*----------------------------------------------------------------------*/
/*               c h e c k F o r W r i t e I n p l a c e        */
/*                                    */
/* Checks possibility for writing Flash data inplace.            */
/*                                    */
/* Parameters:                                                          */
/*    newData        : New data to write.                */
/*    oldData        : Old data at this location.            */
/*                                                                      */
/* Returns:                                                             */
/*    < 0    =>    Writing inplace not possible            */
/*    >= 0    =>    Writing inplace is possible. Value indicates    */
/*            how many bytes at the start of data are        */
/*            identical and may be skipped.            */
/*----------------------------------------------------------------------*/

static int checkForWriteInplace(long  *newData,
                long  *oldData)
{
  int i;

  int skipBytes = 0;
  FLBoolean stillSame = TRUE;

  for (i = SECTOR_SIZE / sizeof(*newData); i > 0; i--, newData++, oldData++) {
    if (cannotWriteOver(*newData,*oldData))
      return -1;
    if (stillSame && *newData == *oldData)
      skipBytes += sizeof *newData;
    else
      stillSame = FALSE;
  }

  return skipBytes;
}






/*----------------------------------------------------------------------*/
/*                        i n i t F T L                */
/*                                    */
/* Initializes essential volume data as a preparation for mount or    */
/* format.                                */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus initFTL(Flare vol)
{
  long int size = 1;

  memset(&vol,0,sizeof(Flare));

  for (vol.erasableBlockSizeBits = 0; size < FLASH_ERASE_BLOCK_SIZE;
       vol.erasableBlockSizeBits++, size <<= 1);

  vol.unitSizeBits = UNIT_SIZE_BITS;        /* At least 64 KB */
  vol.noOfUnits = MAX_UNIT_COUNT;
  vol.unitOffsetMask = (1L << vol.unitSizeBits) - 1;
  vol.sectorsPerUnit = 1 << (vol.unitSizeBits - SECTOR_SIZE_BITS);
  vol.bamOffset = sizeof(UnitHeader);
  vol.unitHeaderSectors = ((allocEntryOffset(&vol,vol.sectorsPerUnit) - 1) >>
                    SECTOR_SIZE_BITS) + 1;

  vol.transferUnit = NULL;
  vol.replacementPageNo = UNASSIGNED_SECTOR;
  vol.badFormat = TRUE;    /* until mount completes */
  vol.mapBuffer.mappedSectorNo = UNASSIGNED_SECTOR;

  vol.currWearLevelingInfo = 0;
  vol.maxEraseCount = 0;
  vol.mapBuffer.remapped = TRUE;


  return flOK;
}



/*----------------------------------------------------------------------*/
/*                      i n i t T a b l e s                */
/*                                    */
/* Allocates and initializes the dynamic volume table, including the    */
/* unit tables and secondary virtual map.                */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

static FLStatus initTables(Flare vol)
{
  VirtualSectorNo iSector;
  UnitNo iUnit;

  /* Allocate the conversion tables */

  char *heapPtr;

  heapPtr = vol.heap;
  vol.physicalUnits = (Unit *) heapPtr;
  heapPtr += vol.noOfUnits * sizeof(Unit);

  vol.logicalUnits = (UnitPtr *) heapPtr;
  heapPtr += vol.noOfUnits * sizeof(UnitPtr);

  vol.pageTable = (LogicalSectorNo *) heapPtr;
  heapPtr += vol.directAddressingSectors * sizeof(LogicalSectorNo);
  if (heapPtr > vol.heap + sizeof(vol.heap))
    return flNotEnoughMemory;

  vol.volBuffer = &s_volBuffers;


  buffer.sectorNo = UNASSIGNED_SECTOR;

  for (iSector = 0; iSector < vol.noOfPages; iSector++)
    vol.pageTable[iSector] = UNASSIGNED_SECTOR;

  for (iUnit = 0; iUnit < vol.noOfUnits; iUnit++)
    vol.logicalUnits[iUnit] = NULL;

  return flOK;
}





/*----------------------------------------------------------------------*/
/*                       m a p S e c t o r                */
/*                                    */
/* Maps and returns location of a given sector no.            */
/* NOTE: This function is used in place of a read-sector operation.    */
/*                                    */
/* A one-sector cache is maintained to save on map operations.        */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*    sectorNo    : Sector no. to read                */
/*    physAddress    : Optional pointer to receive sector address    */
/*                                                                      */
/* Returns:                                                             */
/*    Pointer to physical sector location. NULL returned if sector    */
/*    does not exist.                            */
/*----------------------------------------------------------------------*/

 void  *mapSector(Flare vol, SectorNo sectorNo, uint32_t *physAddress)
{
  if (sectorNo != vol.mapBuffer.mappedSectorNo || vol.mapBuffer.remapped) {
    LogicalSectorNo sectorAddress;

    if (sectorNo >= vol.virtualSectors)
      vol.mapBuffer.mappedSector = NULL;
    else {
      sectorAddress = virtual2Logical(&vol,sectorNo + vol.noOfPages);

      if (sectorAddress == UNASSIGNED_SECTOR || sectorAddress == DELETED_SECTOR)
    vol.mapBuffer.mappedSector = NULL;    /* no such sector */
      else {
    vol.mapBuffer.mappedSectorAddress = logical2Physical(&vol,sectorAddress);
    vol.mapBuffer.mappedSector = flashMap(&vol,
                     vol.mapBuffer.mappedSectorAddress,
                     SECTOR_SIZE);
      }
    }
    vol.mapBuffer.mappedSectorNo = sectorNo;
    vol.mapBuffer.remapped = FALSE;
  }

  if (physAddress)
    *physAddress = vol.mapBuffer.mappedSectorAddress;

  return vol.mapBuffer.mappedSector;
}


/*----------------------------------------------------------------------*/
/*                    w r i t e S e c t o r                */
/*                                    */
/* Writes a sector.                            */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*    sectorNo    : Sector no. to write                */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

FLStatus writeSector(Flare vol,  SectorNo sectorNo, void  *fromAddress)
{
  LogicalSectorNo oldSectorAddress;
  int skipBytes;
  FLStatus status;

  if (vol.badFormat)
    return flBadFormat;
  if (sectorNo >= vol.virtualSectors)
    return flSectorNotFound;

  sectorNo += vol.noOfPages;
  oldSectorAddress = virtual2Logical(&vol,sectorNo);

  if (oldSectorAddress != UNASSIGNED_SECTOR && oldSectorAddress != DELETED_SECTOR &&
      (skipBytes = checkForWriteInplace((long  *) fromAddress,
         (long  *) mapLogical(&vol,oldSectorAddress))) >= 0) {
    if (skipBytes < SECTOR_SIZE)
      status = vol.flash->write(vol.flash,
              logical2Physical(&vol,oldSectorAddress) + skipBytes,
              (char  *) fromAddress + skipBytes,
              SECTOR_SIZE - skipBytes);
    else
      status = flOK;        /* nothing to write */
  }
  else
    status = allocateAndWriteSector(&vol,sectorNo,fromAddress,FALSE);

  if (status == flWriteFault)        /* Automatic retry */
    status = allocateAndWriteSector(&vol,sectorNo,fromAddress,FALSE);

  return status;
}



/*----------------------------------------------------------------------*/
/*                   d e l e t e S e c t o r            */
/*                                    */
/* Marks contiguous sectors as deleted                    */
/*                                    */
/* Parameters:                                                          */
/*    vol        : Pointer identifying drive            */
/*    sectorNo    : First sector no. to delete            */
/*    noOfSectors    : No. of sectors to delete            */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

FLStatus deleteSector(uint16_t sectorNo, int32_t noOfSectors)
{
  int iSector;
  Flare vol = &s_flare;

  if (vol.badFormat)
    return flBadFormat;
  if (sectorNo + noOfSectors > vol.virtualSectors)
    return flSectorNotFound;

  sectorNo += vol.noOfPages;
  for (iSector = 0; iSector < noOfSectors; iSector++, sectorNo++)
    checkStatus(setVirtualMap(&vol,sectorNo,DELETED_SECTOR));

  return flOK;
}


/*----------------------------------------------------------------------*/
/*                     f o r m a t F T L                */
/*                                    */
/* Formats the Flash volume for FTL                    */
/*                                    */
/* Parameters:                                                          */
/*    volNo        : Volume no.                    */
/*    formatParams    : Address of FormatParams structure to use    */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

FLStatus formatFTL(FLFlash *flash)
{
  Flare vol = &s_flare;
  UnitNo iUnit;
  int iPage;
  SectorNo iSector;
  unsigned noOfBadUnits = 0;
  uint32_t *formatEntries;


  checkStatus(initFTL(&vol));
  vol.flash = flash;

  vol.firstPhysicalEUN =
      (UnitNo) (( BOOT_IMAGE_LEN - 1) >> vol.unitSizeBits) + 1;

  vol.noOfTransferUnits = NO_OF_SPARE_UNITS;
  if (vol.noOfUnits <= vol.firstPhysicalEUN + vol.noOfTransferUnits) {
#ifdef DEBUG_PRINT
      DEBUG_PRINT("Debug: Volume too small !!\n");
#endif
    return flVolumeTooSmall;
  }
  vol.virtualSectors = (unsigned long) (vol.noOfUnits - vol.firstPhysicalEUN - vol.noOfTransferUnits) *
           (vol.sectorsPerUnit - vol.unitHeaderSectors) *
           PERCENT_USE / 100;
  vol.noOfPages = (((long) vol.virtualSectors * SECTOR_SIZE - 1) >> PAGE_SIZE_BITS) + 1;
  /* take off size of virtual table, and one extra sector for sector writes */

  vol.virtualSectors -= (vol.noOfPages + 1);
  vol.directAddressingMemory = VM_ADDRESS_LIMIT;
  vol.directAddressingSectors = (SectorNo) (VM_ADDRESS_LIMIT / SECTOR_SIZE) +vol.noOfPages;

  checkStatus(initTables(&vol));

  memset(uh,0xff,SECTOR_SIZE);
  uh->noOfUnits = vol.noOfUnits - vol.firstPhysicalEUN;
  uh->firstPhysicalEUN = vol.firstPhysicalEUN;
  uh->noOfTransferUnits = (unsigned char) vol.noOfTransferUnits;
  memcpy(uh->formatPattern,FORMAT_PATTERN,sizeof uh->formatPattern);
  uh->log2SectorSize = SECTOR_SIZE_BITS;
  uh->log2UnitSize = vol.unitSizeBits;
  uh->directAddressingMemory = vol.directAddressingMemory;
  uh->flags = 0;
  uh->virtualMediumSize= (long) vol.virtualSectors * SECTOR_SIZE;
  uh->noOfPages = vol.noOfPages;

  uh->BAMoffset = vol.bamOffset;

  formatEntries = (uint32_t *) ((char *) uh + allocEntryOffset(&vol,0));
  for (iSector = 0; iSector < vol.unitHeaderSectors; iSector++)
     formatEntries[iSector]= FORMAT_SECTOR;
//格式化每个物理块，并依次赋予逻辑块号
  for (iUnit = vol.firstPhysicalEUN; iUnit < vol.noOfUnits; iUnit++) {
    FLStatus status;
    sysFeedDog();

    status = formatUnit(&vol,&vol.physicalUnits[iUnit]);
    if (status != flOK)
      status = formatUnit(&vol,&vol.physicalUnits[iUnit]);    /* Do it again */
    if (status == flWriteFault) {
      noOfBadUnits++;
      if (noOfBadUnits >= NO_OF_SPARE_UNITS) {
#ifdef DEBUG_PRINT
      DEBUG_PRINT("Too many bad units !! \n\t");
#endif
    return status;
      }
      else
    vol.transferUnit = &vol.physicalUnits[iUnit];
    }
    else if (status == flOK) {
      if (iUnit - noOfBadUnits < vol.noOfUnits - NO_OF_SPARE_UNITS) {
    checkStatus(assignUnit(&vol,
                   &vol.physicalUnits[iUnit],
                               (UnitNo)(iUnit - noOfBadUnits)));
        vol.physicalUnits[iUnit].noOfFreeSectors = vol.sectorsPerUnit - vol.unitHeaderSectors;
    vol.logicalUnits[iUnit - noOfBadUnits] = &vol.physicalUnits[iUnit];
      }
      else
//将最后一个块设为备用块
    vol.transferUnit = &vol.physicalUnits[iUnit];
    }
    else {
#ifdef DEBUG_PRINT
    DEBUG_PRINT("Should not have gotten here !!\n\t");
#endif
      return status;
    }


  }

  /* Allocate and write all page sectors */
  vol.totalFreeSectors = vol.noOfPages;    /* Fix for SPR 31147 */

  for (iPage = 0; iPage < vol.noOfPages; iPage++)
    checkStatus(allocateAndWriteSector(&vol,iPage,NULL,FALSE));

  return flOK;
}



/*----------------------------------------------------------------------*/
/*                     m o u n t F T L                */
/*                                    */
/* Mount FTL volume                            */
/*                                    */
/* Parameters:                                                          */
/*    volNo        : Volume no.                    */
/*    tl        : Where to store translation layer methods    */
/*      volForCallback    : Pointer to FLFlash structure for power on    */
/*              callback routine.                */
/*                                                                      */
/* Returns:                                                             */
/*    FLStatus    : 0 on success, failed otherwise        */
/*----------------------------------------------------------------------*/

FLStatus mountFTL(FLFlash *flash)
{
  Flare vol = &s_flare;
  UnitHeader unitHeader;
  UnitNo iUnit;
  int iPage;



  checkStatus(initFTL(&vol));
  vol.flash = flash;
  /* Find the first properly formatted unit */
  for (iUnit = 0; iUnit < vol.noOfUnits; iUnit++) {
    vol.flash->read(vol.flash,(uint32_t) iUnit << vol.unitSizeBits,
           &unitHeader, sizeof(UnitHeader));
    if (verifyFormat(&unitHeader)) {
      if (unitHeader.flags || unitHeader.log2SectorSize != SECTOR_SIZE_BITS)
        {printf("1 \n\t");
            return flBadFormat;
        }
      break;
    }
  }
  if (iUnit >= vol.noOfUnits)
    return flUnknownMedia;

  /* Get volume information from unit header */
  vol.noOfUnits = unitHeader.noOfUnits;
  vol.noOfTransferUnits = unitHeader.noOfTransferUnits;
  vol.firstPhysicalEUN = unitHeader.firstPhysicalEUN;
  vol.bamOffset = unitHeader.BAMoffset;
  vol.virtualSectors = (SectorNo) (unitHeader.virtualMediumSize >> SECTOR_SIZE_BITS);
  vol.noOfPages = unitHeader.noOfPages;
  vol.noOfUnits += vol.firstPhysicalEUN;
  vol.unitSizeBits = unitHeader.log2UnitSize;
  vol.directAddressingMemory = unitHeader.directAddressingMemory;
  vol.directAddressingSectors = vol.noOfPages +
          (SectorNo) (vol.directAddressingMemory >> SECTOR_SIZE_BITS);

  vol.unitOffsetMask = (1L << vol.unitSizeBits) - 1;
  vol.sectorsPerUnit = 1 << (vol.unitSizeBits - SECTOR_SIZE_BITS);
  vol.unitHeaderSectors = ((allocEntryOffset(&vol,vol.sectorsPerUnit) - 1) >>
                    SECTOR_SIZE_BITS) + 1;

  if (vol.noOfUnits <= vol.firstPhysicalEUN ||
       allocEntryOffset(&vol,vol.unitHeaderSectors) > SECTOR_SIZE ||
      (int)(vol.virtualSectors >> (PAGE_SIZE_BITS - SECTOR_SIZE_BITS)) > vol.noOfPages ||
      (int)(vol.virtualSectors >> (vol.unitSizeBits - SECTOR_SIZE_BITS)) > (vol.noOfUnits - vol.firstPhysicalEUN))
            {printf("2 \n\t");
            return flBadFormat;
        }

  checkStatus(initTables(&vol));

  vol.totalFreeSectors = 0;

  /* Mount all units */
  for (iUnit = vol.firstPhysicalEUN; iUnit < vol.noOfUnits; iUnit++)
    mountUnit(&vol,&vol.physicalUnits[iUnit]);

  /* Verify the conversion tables */
  vol.badFormat = FALSE;

  for (iUnit = vol.firstPhysicalEUN; iUnit < vol.noOfUnits - vol.noOfTransferUnits; iUnit++)
    if (vol.logicalUnits[iUnit] == NULL)
      vol.badFormat = TRUE;

  if (vol.replacementPageNo != UNASSIGNED_SECTOR &&
      vol.pageTable[vol.replacementPageNo] == UNASSIGNED_SECTOR) {
    /* A lonely replacement page. Mark it as a regular page (may fail   */
    /* because of write protection) and use it.                */
    markAllocMap(&vol,
         vol.replacementPageAddress,
         (((VirtualAddress) vol.replacementPageNo - vol.noOfPages)
                << SECTOR_SIZE_BITS) | DATA_SECTOR,
         TRUE);
    vol.pageTable[vol.replacementPageNo] = vol.replacementPageAddress;
    vol.replacementPageNo = UNASSIGNED_SECTOR;
  }

  for (iPage = 0; iPage < vol.noOfPages; iPage++)
    if (vol.pageTable[iPage] == UNASSIGNED_SECTOR)
      vol.badFormat = TRUE;

  return vol.badFormat ? flBadFormat : flOK;
}



/*******************************************************************************
*
* Function: ftlRead
*
* Description:
*    read data from the virtual address
*
* Parameters:
*    uint32_t virtualAddr: virtual address to read
*    void * buf : where to receive data
*    int len : length of data
*
* Return Value:
*    STATUS: OK on success, ERROR otherwise
*
******************************************************************************/
status_t ftlRead(uint32_t virtualAddr, uint8_t * buf, int32_t len)
{
    Flare vol = &s_flare;

    SectorNo sectorNo = virtualAddr>>SECTOR_SIZE_BITS;
    LogicalAddress offset = virtualAddr & SECTOR_OFFSET_MASK;
    int lenInSector = SECTOR_SIZE-offset;

    int noOfSectors =(len > lenInSector )?((len-lenInSector-1)>>SECTOR_SIZE_BITS)+2:1;

    if (sectorNo+noOfSectors-1 >= vol.virtualSectors)
        return ERROR;
    semTake(semFtlRW, WAIT_FOREVER);
    for(int i=0;i<noOfSectors;i++,sectorNo++){
        if (i == noOfSectors-1) lenInSector = len;
        if (mapSector(&vol,sectorNo,NULL) != NULL)
            memcpy(buf,(uint8_t*)vol.mapBuffer.mappedSector+offset,lenInSector);
        else
            memset(buf,0xFF,lenInSector);
        buf += lenInSector;
        offset = 0;
        len -= lenInSector;
        lenInSector = SECTOR_SIZE;


    }
    semGive(semFtlRW);
    return OK;

}

/*******************************************************************************
*
* Function: ftlWrite
*
* Description:
*    write data to the virtual address;
*
* Parameters:
*    uint32_t virtualAddr: virtual address to write
*    void * from : Pointer of data where to write
*    int len : length of data
*
* Return Value:
*    STATUS: OK on success, ERROR otherwise
*
******************************************************************************/
status_t ftlWrite(uint32_t virtualAddr, uint8_t * from, int32_t len)
{
    Flare vol = &s_flare;
    FLStatus status;

    SectorNo sectorNo = virtualAddr>>SECTOR_SIZE_BITS;
    LogicalAddress offset = virtualAddr & SECTOR_OFFSET_MASK;
    int lenInSector = SECTOR_SIZE-offset;

    int noOfSectors =(len > lenInSector )?((len-lenInSector-1)>>SECTOR_SIZE_BITS)+2:1;

    if (sectorNo+noOfSectors > vol.virtualSectors)
        return ERROR;

    semTake(semFtlRW,WAIT_FOREVER);

    for(int i=0;i<noOfSectors;i++,sectorNo++){
        if (lenInSector <= SECTOR_SIZE){
            if (mapSector(&vol,sectorNo,NULL) != NULL)
                memcpy(bufFtlRw,(char*)vol.mapBuffer.mappedSector,SECTOR_SIZE);
            else
                memset(bufFtlRw,0xFF,SECTOR_SIZE);

        }
        if (i == noOfSectors-1) lenInSector = len;
        memcpy(bufFtlRw+offset,from,lenInSector);
        status = writeSector(&vol,sectorNo,bufFtlRw);
        if (status != flOK ) break;
        from += lenInSector;
        len -= lenInSector;
        offset = 0;
        lenInSector = SECTOR_SIZE;

    }
    semGive(semFtlRW);
    return (status == flOK)?OK : ERROR;

}

/*******************************************************************************
*
* Function: ftlSet
*
* Description:
*    write a fix char to flash from the virtual address with the lenght bytes;
*
* Parameters:
*    uint32_t virtualAddr: virtual address to write
*    uint8 value : a fix char which write to flash
*    int len : length of data
*
* Return Value:
*    STATUS: OK on success, ERROR otherwise
*
******************************************************************************/
status_t ftlSet(uint32_t virtualAddr, uint8_t value,int32_t len)
{

    Flare vol = &s_flare;
    FLStatus status;

    SectorNo sectorNo = virtualAddr>>SECTOR_SIZE_BITS;
    LogicalAddress offset = virtualAddr & SECTOR_OFFSET_MASK;
    int lenInSector = SECTOR_SIZE-offset;

    int noOfSectors =(len > lenInSector )?((len-lenInSector-1)>>SECTOR_SIZE_BITS)+2:1;

    if (sectorNo+noOfSectors > vol.virtualSectors)
        return ERROR;
    semTake(semFtlRW,WAIT_FOREVER);
    for(int i=0;i<noOfSectors;i++,sectorNo++){
        if (lenInSector <= SECTOR_SIZE){
            if (mapSector(&vol,sectorNo,NULL) != NULL)
                memcpy(bufFtlRw,(char*)vol.mapBuffer.mappedSector,SECTOR_SIZE);
            else
                memset(bufFtlRw,0xFF,SECTOR_SIZE);

        }
        if (i == noOfSectors-1) lenInSector = len;
        memset(bufFtlRw+offset,value,lenInSector);
        status = writeSector(&vol,sectorNo,bufFtlRw);
        if (status != flOK ) break;
        len -= lenInSector;
        offset = 0;
        lenInSector = SECTOR_SIZE;

    }
    semGive(semFtlRW);
    return (status == flOK)?OK : ERROR;

}

/*******************************************************************************
*
* Function: ftlCmp
*
* Description:
*    compare buf and flash
*
* Parameters:
*    uint32_t virtualAddr: virtual address to read
*    void * buf : where to compare data
*    int len : length of data
*    boolean * same: compare result
*
* Return Value:
*    STATUS: OK on success, ERROR otherwise
*
******************************************************************************/
status_t ftlCmp(uint32_t virtualAddr, uint8_t * buf, int32_t len, bool_e* same)
{
    Flare vol = &s_flare;

    SectorNo sectorNo = virtualAddr >> SECTOR_SIZE_BITS;
    LogicalAddress offset = virtualAddr & SECTOR_OFFSET_MASK;
    int lenInSector = SECTOR_SIZE-offset;

    int noOfSectors = (len > lenInSector ) ? ((len - lenInSector - 1) >> SECTOR_SIZE_BITS) + 2 : 1;

    if (sectorNo+noOfSectors-1 >= vol.virtualSectors)
        return ERROR;
    semTake(semFtlRW, WAIT_FOREVER);

    *same = TRUE;
    for(int i = 0; (i < noOfSectors) && same; i++, sectorNo++){
        if (i == noOfSectors-1) lenInSector = len;
        if (mapSector(&vol, sectorNo, NULL) != NULL) {
            for (int j = 0; j < lenInSector; j++) {
                if (*buf++ != *((uint8_t *)vol.mapBuffer.mappedSector + offset + j)) {
                    *same = FALSE;
                    break;
                }
            }
        } else {
            for (int j = 0; j < lenInSector; j++) {
                if (*buf++ != 0xFF) {
                    *same = FALSE;
                    break;
                }
            }
        }
        offset = 0;
        len -= lenInSector;
        lenInSector = SECTOR_SIZE;
    }

    semGive(semFtlRW);
    return OK;
}

/*******************************************************************************
*
* Function: ftlInit
*
* Description:
*    initialize the ftl(Flash Translation Layer) system
*
* Parameters:
*    NONE
*
* Return Value:
*   STATUS: OK on success, ERROR otherwise
*
******************************************************************************/
status_t ftlInit()
{

    semFtlRW = semBCreate(1);
    FLStatus status;

//    struct spi_dev_s* spi_dev_flash;
//   spi_dev_flash = spiinitialize(0);
//    FLFlash* m25p = m25p_initialize(spi_dev_flash);

    FLFlash* flash = flash_init();
    if (flash == NULL){
        printf("\r\n dataflash init error! \r\n");
        return ERROR;

    }

    if ((status = mountFTL(flash)) != flOK){
        printf("mount failed! status= %d\r\n Formating flash ... \r\n",status);
        if ((status = formatFTL(flash)) != flOK ){
            printf("ftl formate failed!status = %d \r\n",status);
            return ERROR;
        }
        printf("Formate Done!................\r\n");
         if (mountFTL(flash) != flOK){
            printf("ftl Dev Create failed!\r\n");
            return ERROR;
        }
    }
    printf("ftl Dev mount success!\r\n");
    return OK;
}


/*******************************************************************************
*
* Function: ftlShow
*
* Description:
*    This routine prints ftl information.
*
* Parameters:
*    NONE
*
* Return Value:
*   NONE
*
******************************************************************************/
void ftlShow(void)
{
    Flare vol = &s_flare;
    printf ("FTL Version 1.0 :\r\n");
    printf ("    Flash physical size      : 0x%x \r\n",vol.flash->chipSize*FLASH_CHIP_NUM);
    printf ("    Flash virtual  size      : 0x%x \r\n",vol.virtualSectors<<SECTOR_SIZE_BITS);
    printf ("    Flash unit number        : %d   \r\n",vol.noOfUnits);
    printf ("    Flash page number        : %d   \r\n",vol.noOfPages);
    printf ("    Flash virtual sectors num: %d   \r\n",vol.virtualSectors);
    printf ("    Flash free sectors num   : %d   \r\n",vol.totalFreeSectors);
    printf ("    Flash transfer unit num  : %d   \r\n",vol.noOfTransferUnits);
    printf ("    Flash cur wear levering  : %d   \r\n",vol.currWearLevelingInfo);
    printf ("    Flash max erase count    : %d   \r\n",vol.maxEraseCount);
    printf (" -----------------------------------\r\n");
    printf ("    Flash chipSize           : 0x%x \r\n",vol.flash->chipSize);
    printf ("    Flash noOfChips          : %d   \r\n",vol.flash->noOfChips);
    printf ("    Flash erasableBlockSize  : 0x%x \r\n",vol.flash->erasableBlockSize);



}
unsigned long
ftlget_maxEraseCount(void)
{
    Flare vol = &s_flare;
    /*printf ("    Flash max erase count    : %d   \r\n",vol.maxEraseCount);*/
    return (vol.maxEraseCount);
}



/*----------------------------End of ftllite.c-------------------------------*/

