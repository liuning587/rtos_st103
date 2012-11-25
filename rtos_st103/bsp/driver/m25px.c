/************************************************************************************
 * drivers/mtd/m25px.c
 * Driver for SPI-based M25P1 (128Kbit), M25P64 (64Mbit), and M25P128 (128Mbit) FLASH
 *
 *   Copyright (C) 2009-2010 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ************************************************************************************/

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include <types.h>
#include <errno.h>
#include <spi.h>
#include <ioctl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <flash.h>

/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

#define M25P_M25P64_PSEC_64K     /*M25P64 per sector 64K*/
#undef  M25P_M25P64_PSEC_4K      /*M25P64 per sector 4K*/

/* Indentification register values */

#define M25P_MANUFACTURER         0xef
#define M25P_MEMORY_TYPE          0x40
#define M25P_M25P1_CAPACITY       0x11 /* 1 M-bit */
#define M25P_M25P32_CAPACITY      0x16 /* 32 M-bit */
#define M25P_M25P64_CAPACITY      0x17 /* 64 M-bit */
#define M25P_M25P128_CAPACITY     0x18 /* 128 M-bit */
#define M25P_M25P256_CAPACITY     0x19 /* 256 M-bit */

#define MX25_MANUFACTURER         0xc2
#define MX25_MEMORY_TYPE          0x20

#define S25F128P_MANUFACTURER     0x01
#define S25F128P_MEMORY_TYPE      0x20
#define S25F256P_MEMORY_TYPE      0x02

/*  M25P1 capacity is 131,072 bytes:
 *  (4 sectors) * (32,768 bytes per sector)
 *  (512 pages) * (256 bytes per page)
 */

#define M25P_M25P1_SECTOR_SHIFT  15    /* Sector size 1 << 15 = 65,536 */
#define M25P_M25P1_NSECTORS      4
#define M25P_M25P1_PAGE_SHIFT    8     /* Page size 1 << 8 = 256 */
#define M25P_M25P1_NPAGES        512

/*  M25P64 capacity is 8,338,608 bytes:
 *  (128 sectors) * (65,536 bytes per sector)
 *  (32768 pages) * (256 bytes per page)
 */

#ifdef M25P_M25P64_PSEC_64K
#define M25P_M25P64_SECTOR_SHIFT  16    /* Sector size 1 << 16 = 65,536 */
#define M25P_M25P64_NSECTORS      128
#define M25P_M25P64_PAGE_SHIFT    8     /* Page size 1 << 8 = 256 */
#define M25P_M25P64_NPAGES        32768
#else
#define M25P_M25P64_SECTOR_SHIFT  12    /* Sector size 1 << 12 = 4,096 */
#define M25P_M25P64_NSECTORS      2048
#define M25P_M25P64_PAGE_SHIFT    8     /* Page size 1 << 8 = 256 */
#define M25P_M25P64_NPAGES        32768
#endif

/*  M25P128 capacity is 16,777,216 bytes:
 *  (64 sectors) * (262,144 bytes per sector)
 *  (65536 pages) * (256 bytes per page)
 */

#define M25P_M25P128_SECTOR_SHIFT 18    /* Sector size 1 << 18 = 262,144 */
#define M25P_M25P128_NSECTORS     64
#define M25P_M25P128_PAGE_SHIFT   8     /* Page size 1 << 8 = 256 */
#define M25P_M25P128_NPAGES       65536

/* Instructions */
/*      Command        Value      N Description             Addr Dummy Data */
#define M25P_WREN      0x06    /* 1 Write Enable              0   0     0 */
#define M25P_WRDI      0x04    /* 1 Write Disable             0   0     0 */
#define M25P_RDID      0x9f    /* 1 Read Identification       0   0     1-3 */
#define M25P_RDSR      0x05    /* 1 Read Status Register      0   0     >=1 */
#define M25P_WRSR      0x01    /* 1 Write Status Register     0   0     1 */
#define M25P_READ      0x03    /* 1 Read Data Bytes           3   0     >=1 */
#define M25P_FAST_READ 0x0b    /* 1 Higher speed read         3   1     >=1 */
#define M25P_PP        0x02    /* 1 Page Program              3   0     1-256 */
#ifdef M25P_M25P64_PSEC_64K
#define M25P_SE        0xD8    /* 1 Sector Erase              3   0     0 */
#else
#define M25P_SE        0x20    /* 1 Sector Erase              3   0     0 */
#endif
#define M25P_BE        0xc7    /* 1 Bulk Erase                0   0     0 */
#define M25P_RES       0xab    /* 2 Read Electronic Signature 0   3     >=1 */
/* Used for Spansion flashes only. */
#define OPCODE_BRWR     0x17    /* Bank register write */

/* NOTE 1: Both parts, NOTE 2: M25P64 only */

/* Status register bit definitions */

#define M25P_SR_WIP            (1 << 0)                /* Bit 0: Write in progress bit */
#define M25P_SR_WEL            (1 << 1)                /* Bit 1: Write enable latch bit */
#define M25P_SR_BP_SHIFT       (2)                     /* Bits 2-4: Block protect bits */
#define M25P_SR_BP_MASK        (7 << M25P_SR_BP_SHIFT)
#  define M25P_SR_BP_NONE      (0 << M25P_SR_BP_SHIFT) /* Unprotected */
#  define M25P_SR_BP_UPPER64th (1 << M25P_SR_BP_SHIFT) /* Upper 64th */
#  define M25P_SR_BP_UPPER32nd (2 << M25P_SR_BP_SHIFT) /* Upper 32nd */
#  define M25P_SR_BP_UPPER16th (3 << M25P_SR_BP_SHIFT) /* Upper 16th */
#  define M25P_SR_BP_UPPER8th  (4 << M25P_SR_BP_SHIFT) /* Upper 8th */
#  define M25P_SR_BP_UPPERQTR  (5 << M25P_SR_BP_SHIFT) /* Upper quarter */
#  define M25P_SR_BP_UPPERHALF (6 << M25P_SR_BP_SHIFT) /* Upper half */
#  define M25P_SR_BP_ALL       (7 << M25P_SR_BP_SHIFT) /* All sectors */
#define M25P_SR_SRWD           (1 << 7)                /* Bit 7: Status register write protect */

#define M25P_DUMMY     0xa5

#define SPI_SELECT(d,s)     ((d)->select(d,s))
#define SPI_SEND(d,wd)      ((d)->send(d, wd))
//#define SPI_SNDBLOCK(d,b,l) ((d)->sndblock(d,b,l))
//#define SPI_RECVBLOCK(d,b,l) ((d)->recvblock(d,b,l))
/************************************************************************************
 * Private Types
 ************************************************************************************/

/* This type represents the state of the MTD device.  The struct mtd_dev_s
 * must appear at the beginning of the definition so that you can freely
 * cast between pointers to struct mtd_dev_s and struct m25p_dev_s.
 */

struct m25p_dev_s
{
    struct mtd_dev_s mtd; /* MTD interface */
    spi_opt_t* dev; /* Saved SPI interface instance */
    SEM_ID lock;
    uint8_t sectorshift; /* 16 or 18 */
    uint8_t pageshift; /* 8 */
    uint16_t nsectors; /* 128 or 64 */
    uint32_t npages; /* 32,768 or 65,536 */
};

struct mtd_dev_s*
m25p_initialize(int spiport, int chipselect);

static struct m25p_dev_s m25pDev[MAX_FLASH_CHIP_NUM];
static int noOfM25p = 0;

/*------------------------------------------------------------------------------
 Section: Global Variables
 ------------------------------------------------------------------------------*/

MTD_TBL_ENTRY mtdDevTbl[] =
    {

        { 0, m25p_initialize, 0, 0 },

        { 0, MTD_TBL_END, 0, 0 },

    };

/************************************************************************************
 * Private Function Prototypes
 ************************************************************************************/

/* Helpers */

static void
m25p_lock(struct m25p_dev_s *priv);
static inline void
m25p_unlock(struct m25p_dev_s *priv);
static inline int
m25p_readid(struct m25p_dev_s *priv);
static void
m25p_waitwritecomplete(struct m25p_dev_s *priv);
static void
m25p_writeenable(struct m25p_dev_s *priv);
static inline void
m25p_sectorerase(struct m25p_dev_s *priv, uint32_t offset);
static inline int
m25p_bulkerase(struct m25p_dev_s *priv);
//static inline void m25p_pagewrite(struct m25p_dev_s *priv,  const uint8 *buffer,
//                                 uint32 offset);
//static void m25p_wpenable(struct m25p_dev_s *priv);

/* MTD driver methods */

/************************************************************************************
 * Name: m25p_lock
 ************************************************************************************/

static void
m25p_lock(struct m25p_dev_s *priv)
{
    semTake(priv->lock, WAIT_FOREVER);
}

/************************************************************************************
 * Name: m25p_unlock
 ************************************************************************************/

static inline void
m25p_unlock(struct m25p_dev_s *priv)
{
    semGive(priv->lock);
}

/************************************************************************************
 * Name: m25p_readid
 ************************************************************************************/

static inline int
m25p_readid(struct m25p_dev_s *priv)
{
    uint16_t manufacturer;
    uint16_t memory;
    uint16_t capacity;

    /* Lock the SPI bus, configure the bus, and select this FLASH part. */

    m25p_lock(priv);
    SPI_SELECT(priv->dev, TRUE);

    /* Send the "Read ID (RDID)" command and read the first three ID bytes */

    (void) SPI_SEND(priv->dev, M25P_RDID);
    manufacturer = SPI_SEND(priv->dev, M25P_DUMMY);
    memory = SPI_SEND(priv->dev, M25P_DUMMY);
    capacity = SPI_SEND(priv->dev, M25P_DUMMY);

    /* Deselect the FLASH and unlock the bus */
    SPI_SELECT(priv->dev, FALSE);
    m25p_unlock(priv);

    printf("\rmanufacturer: %02x memory: %02x capacity: %02x\r\n", manufacturer,
            memory, capacity);

    /* Check for a valid manufacturer and memory type */

    if (manufacturer == M25P_MANUFACTURER && memory == M25P_MEMORY_TYPE)
    {
        /* Okay.. is it a FLASH capacity that we understand? */

        if (capacity == M25P_M25P1_CAPACITY)
        {
            /* Save the FLASH geometry */

            priv->sectorshift = M25P_M25P1_SECTOR_SHIFT;
            priv->nsectors = M25P_M25P1_NSECTORS;
            priv->pageshift = M25P_M25P1_PAGE_SHIFT;
            priv->npages = M25P_M25P1_NPAGES;
            return OK;
        }
        else if (capacity == M25P_M25P64_CAPACITY)
        {
            /* Save the FLASH geometry */

            priv->sectorshift = M25P_M25P64_SECTOR_SHIFT;
            priv->nsectors = M25P_M25P64_NSECTORS;
            priv->pageshift = M25P_M25P64_PAGE_SHIFT;
            priv->npages = M25P_M25P64_NPAGES;
            return OK;
        }
        else if (capacity == M25P_M25P128_CAPACITY)
        {
            /* Save the FLASH geometry */

            priv->sectorshift = M25P_M25P128_SECTOR_SHIFT;
            priv->nsectors = M25P_M25P128_NSECTORS;
            priv->pageshift = M25P_M25P128_PAGE_SHIFT;
            priv->npages = M25P_M25P128_NPAGES;
            return OK;
        }
    }
    if (manufacturer == S25F128P_MANUFACTURER && memory == S25F128P_MEMORY_TYPE)
    {
        if (capacity == M25P_M25P128_CAPACITY)
        {
            /* Save the FLASH geometry */

            priv->sectorshift = M25P_M25P64_SECTOR_SHIFT;
            priv->nsectors = M25P_M25P64_NSECTORS * 2;
            priv->pageshift = M25P_M25P64_PAGE_SHIFT;
            priv->npages = M25P_M25P64_NPAGES * 2;

            return OK;

        }
    }
    if (manufacturer == S25F128P_MANUFACTURER && memory == S25F256P_MEMORY_TYPE)
    {
        if (capacity == M25P_M25P256_CAPACITY)
        {
            /* Save the FLASH geometry */

            priv->sectorshift = M25P_M25P64_SECTOR_SHIFT;
            priv->nsectors = M25P_M25P64_NSECTORS * 4;
            priv->pageshift = M25P_M25P64_PAGE_SHIFT;
            priv->npages = M25P_M25P64_NPAGES * 4;

            return OK;

        }
    }
    if (manufacturer == MX25_MANUFACTURER && memory == MX25_MEMORY_TYPE)
    {
        if (capacity == M25P_M25P64_CAPACITY)
        {
            /* Save the FLASH geometry */
            priv->sectorshift = M25P_M25P64_SECTOR_SHIFT;
            priv->nsectors = M25P_M25P64_NSECTORS;
            priv->pageshift = M25P_M25P64_PAGE_SHIFT;
            priv->npages = M25P_M25P64_NPAGES;
            return OK;
        }
        if (capacity == M25P_M25P32_CAPACITY)
        {
            /* Save the FLASH geometry */
            priv->sectorshift = M25P_M25P64_SECTOR_SHIFT;
            priv->nsectors = M25P_M25P64_NSECTORS / 2;
            priv->pageshift = M25P_M25P64_PAGE_SHIFT;
            priv->npages = M25P_M25P64_NPAGES / 2;
            return OK;
        }
    }

    return -ENODEV;
}

/************************************************************************************
 * Name: m25p_waitwritecomplete
 ************************************************************************************/

static void
m25p_waitwritecomplete(struct m25p_dev_s *priv)
{
    /* Select this FLASH part */
    SPI_SELECT(priv->dev, TRUE);

    /* Send "Read Status Register (RDSR)" command */
    (void) SPI_SEND(priv->dev, M25P_RDSR);

    /* Loop as long as the memory is busy with a write cycle */
    while (((SPI_SEND(priv->dev, M25P_DUMMY)) & M25P_SR_WIP) != 0)
    {
        /* Send a dummy byte to generate the clock needed to shift out the status */
        //taskDelay(1);
    }

    /* Deselect the FLASH */
    SPI_SELECT(priv->dev, FALSE);
}

/************************************************************************************
 * Name:  m25p_writeenable
 ************************************************************************************/
static void
m25p_writeenable(struct m25p_dev_s *priv)
{
    /* Select this FLASH part */
    SPI_SELECT(priv->dev, TRUE);

    /* Send "Write Enable (WREN)" command */
    (void) SPI_SEND(priv->dev, M25P_WREN);

    /* Deselect the FLASH */
    SPI_SELECT(priv->dev, FALSE);
}

/************************************************************************************
 * Name:  m25p_sectorerase
 ************************************************************************************/
static inline void
m25p_sectorerase(struct m25p_dev_s *priv, uint32_t sector)
{
    uint32_t offset = sector << priv->sectorshift;

    /* Wait for any preceding write to complete.  We could simplify things by
     * perform this wait at the end of each write operation (rather than at
     * the beginning of ALL operations), but have the wait first will slightly
     * improve performance.
     */
    m25p_waitwritecomplete(priv);

    /* Send write enable instruction */
    m25p_writeenable(priv);

    /* Select this FLASH part */
    SPI_SELECT(priv->dev, TRUE);

    /* Send the "Sector Erase (SE)" instruction */
    (void) SPI_SEND(priv->dev, M25P_SE);

    /* Send the sector offset high byte first.  For all of the supported
     * parts, the sector number is completely contained in the first byte
     * and the values used in the following two bytes don't really matter.
     */
#if (FLASH_CHIP_SIZE > 0x1000000L)
    (void)SPI_SEND(priv->dev, (offset >> 24) & 0xff);
#endif
    (void) SPI_SEND(priv->dev, (offset >> 16) & 0xff);
    (void) SPI_SEND(priv->dev, (offset >> 8) & 0xff);
    (void) SPI_SEND(priv->dev, offset & 0xff);

    /* Deselect the FLASH */
    SPI_SELECT(priv->dev, FALSE);
}

/************************************************************************************
 * Name:  m25p_bulkerase
 ************************************************************************************/

static inline int
m25p_bulkerase(struct m25p_dev_s *priv)
{

    /* Wait for any preceding write to complete.  We could simplify things by
     * perform this wait at the end of each write operation (rather than at
     * the beginning of ALL operations), but have the wait first will slightly
     * improve performance.
     */

    m25p_waitwritecomplete(priv);

    /* Send write enable instruction */

    m25p_writeenable(priv);

    /* Select this FLASH part */

    SPI_SELECT(priv->dev, TRUE);

    /* Send the "Bulk Erase (BE)" instruction */

    (void) SPI_SEND(priv->dev, M25P_BE);

    /* Deselect the FLASH */

    SPI_SELECT(priv->dev, FALSE);
    return OK;
}

/************************************************************************************
 * Name:  m25p_pagewrite
 ************************************************************************************/
#if 0
static inline void m25p_pagewrite(struct m25p_dev_s *priv, const uint8_t *buffer,
        uint32_t page)
{
    uint32_t offset = page << priv->pageshift;

    /* Wait for any preceding write to complete.  We could simplify things by
     * perform this wait at the end of each write operation (rather than at
     * the beginning of ALL operations), but have the wait first will slightly
     * improve performance.
     */
    m25p_waitwritecomplete(priv);

    /* Enable the write access to the FLASH */
    m25p_writeenable(priv);

    /* Select this FLASH part */
    SPI_SELECT(priv->dev, TRUE);

    /* Send "Page Program (PP)" command */
    (void)SPI_SEND(priv->dev, M25P_PP);

    /* Send the page offset high byte first. */
    (void)SPI_SEND(priv->dev, (offset >> 16) & 0xff);
    (void)SPI_SEND(priv->dev, (offset >> 8) & 0xff);
    (void)SPI_SEND(priv->dev, offset & 0xff);

    /* Then write the specified number of bytes */
    SPI_SNDBLOCK(priv->dev, buffer, 1 << priv->pageshift);

    /* Deselect the FLASH: Chip Select high */
    SPI_SELECT(priv->dev, FALSE);
}

#endif
/************************************************************************************
 * Name:  m25p_page_offsetwrite
 ************************************************************************************/
static inline void
m25p_page_offsetwrite(struct m25p_dev_s *priv, const uint8_t *buffer,
        uint16_t len, uint32_t page, uint8_t offsetin)
{
    uint32_t offset = (page << priv->pageshift) + offsetin;

    /*如果写的字节超过了该页最大长度则返回错误*/
    if ((1 << priv->pageshift) < offsetin + len)
        return;

    /* Wait for any preceding write to complete.  We could simplify things by
     * perform this wait at the end of each write operation (rather than at
     * the beginning of ALL operations), but have the wait first will slightly
     * improve performance.
     */
    m25p_waitwritecomplete(priv);

    /* Enable the write access to the FLASH */
    m25p_writeenable(priv);

    /* Select this FLASH part */
    SPI_SELECT(priv->dev, TRUE);

    /* Send "Page Program (PP)" command */
    (void) SPI_SEND(priv->dev, M25P_PP);

    /* Send the page offset high byte first. */
#if (FLASH_CHIP_SIZE > 0x1000000L)
    (void)SPI_SEND(priv->dev, (offset >> 24) & 0xff);
#endif
    (void) SPI_SEND(priv->dev, (offset >> 16) & 0xff);
    (void) SPI_SEND(priv->dev, (offset >> 8) & 0xff);
    (void) SPI_SEND(priv->dev, offset & 0xff);

    /* Then write the specified number of bytes */
    //SPI_SNDBLOCK(priv->dev, buffer, len);
    for (uint16_t i = 0; i < len; i++)
    {
        (void) SPI_SEND(priv->dev, buffer[i]);
    }


    /* Deselect the FLASH: Chip Select high */
    SPI_SELECT(priv->dev, FALSE);
}

/************************************************************************************
 * Name: m25p_erase
 ************************************************************************************/
static FLStatus
m25p_erase(struct mtd_dev_s *dev, int startblock, int nblocks)
{
    struct m25p_dev_s *priv = (struct m25p_dev_s *) dev;
    int blocksleft = nblocks;

    /* Lock access to the SPI bus until we complete the erase */

    m25p_lock(priv);

    while (blocksleft-- > 0)
    {
        /* Erase each sector */
        m25p_sectorerase(priv, startblock);
        startblock++;
    }
    m25p_unlock(priv);
    return flOK;
}

/************************************************************************************
 * Name: m25p_write
 ************************************************************************************/
static FLStatus
m25p_write(struct mtd_dev_s *dev, uint32_t address, void *buffer, int len)
{
    struct m25p_dev_s *priv = (struct m25p_dev_s *) dev;
    uint32_t offset = 0;
    uint32_t page = 0;
    uint32_t tmp = len;
    uint8_t* buf = (uint8_t*) buffer;
    /* Lock the SPI bus and write each page to FLASH */

    m25p_lock(priv);

    m25p_waitwritecomplete(priv);
    page = address / (1 << priv->pageshift);
    offset = address % (1 << priv->pageshift);

    //不跨页的操作,剩下的一页能写完
    if (((1 << priv->pageshift) - offset) >= len)
        m25p_page_offsetwrite(priv, buf, len, page, offset);

    else
    {
        //剩下一页写不完的情况
        m25p_page_offsetwrite(priv, buf, (1 << priv->pageshift) - offset, page,
                offset);
        tmp = tmp - ((1 << priv->pageshift) - offset);
        buf = buf + ((1 << priv->pageshift) - offset);
        page++;
        while (tmp > ((1 << priv->pageshift)))
        {

            m25p_page_offsetwrite(priv, buf, (1 << priv->pageshift), page, 0);
            page++;
            buf = buf + (1 << priv->pageshift);
            tmp = tmp - (1 << priv->pageshift);
        }
        //剩下最后一页
        m25p_page_offsetwrite(priv, buf, tmp, page, 0);

    }

    m25p_unlock(priv);

    return flOK;
}

/************************************************************************************
 * Name: m25p_read
 ************************************************************************************/
static FLStatus
m25p_read(struct mtd_dev_s *dev, uint32_t address, void *buffer, int nbytes)
{
    struct m25p_dev_s *priv = (struct m25p_dev_s *) dev;

    /* Wait for any preceding write to complete.  We could simplify things by
     * perform this wait at the end of each write operation (rather than at
     * the beginning of ALL operations), but have the wait first will slightly
     * improve performance.
     */

    /* Lock the SPI bus and select this FLASH part */
    m25p_lock(priv);

    m25p_waitwritecomplete(priv);
    SPI_SELECT(priv->dev, TRUE);

    /* Send "Read from Memory " instruction */
    (void) SPI_SEND(priv->dev, M25P_READ);

    /* Send the page offset high byte first. */
#if (FLASH_CHIP_SIZE > 0x1000000L)
    (void)SPI_SEND(priv->dev, (address >> 24) & 0xff);
#endif
    (void) SPI_SEND(priv->dev, (address >> 16) & 0xff);
    (void) SPI_SEND(priv->dev, (address >> 8) & 0xff);
    (void) SPI_SEND(priv->dev, address & 0xff);

    /* Then read all of the requested bytes */
    //SPI_RECVBLOCK(priv->dev, buffer, nbytes);
    uint8_t* pbuf = (uint8_t*)buffer;
    for (uint16_t i = 0; i < nbytes; i++)
    {
        pbuf[i] = SPI_SEND(priv->dev, M25P_DUMMY);
    }

    /* Deselect the FLASH and unlock the SPI bus */
    SPI_SELECT(priv->dev, FALSE);
    m25p_unlock(priv);

    return flOK;
}

/************************************************************************************
 * Name: m25p_ioctl
 ************************************************************************************/
static int
m25p_ioctl(struct mtd_dev_s *dev, int cmd, unsigned long arg)
{
    struct m25p_dev_s *priv = (struct m25p_dev_s *) dev;
    int ret = -EINVAL; /* Assume good command with bad parameters */

    switch (cmd)
    {
        case MTDIOC_BULKERASE:
            {
                /* Erase the entire device */

                m25p_lock(priv);
                ret = m25p_bulkerase(priv);
                m25p_unlock(priv);
            }
            break;

        case MTDIOC_XIPBASE:
        default:
            ret = -ENOTTY; /* Bad command */
            break;
    }
    return ret;
}

/************************************************************************************
 * Public Functions
 ************************************************************************************/

/************************************************************************************
 * Name: m25p_initialize
 *
 * Description:
 *   Create an initialize MTD device instance.  MTD devices are not registered
 *   in the file system, but are created as instances that can be bound to
 *   other functions (such as a block or character driver front end).
 *
 ************************************************************************************/

struct mtd_dev_s*
m25p_initialize(int spiport, int chipselect)
{
    struct m25p_dev_s *priv;

    if (noOfM25p >= MAX_FLASH_CHIP_NUM)
        return NULL;
    priv = &m25pDev[noOfM25p];

    if ((priv->lock = semBCreate(1)) == NULL)
    {
        return NULL;
    }
    priv->mtd.erase = m25p_erase;
    priv->mtd.write = m25p_write;
    priv->mtd.read = m25p_read;
    priv->mtd.ioctl = m25p_ioctl;
    priv->dev = spi_init(E_SPI_FLASH_PORT);

    /* Deselect the FLASH */
    SPI_SELECT(priv->dev, FALSE);

    /* Identify the FLASH chip and get its capacity */
    if (m25p_readid(priv) != OK)
    {
        return NULL;
    }

#if (FLASH_CHIP_SIZE > 0x1000000L)
    m25p_lock(priv->dev);
    SPI_SELECT(priv->dev, TRUE);
    (void)SPI_SEND(priv->dev, OPCODE_BRWR);
    (void)SPI_SEND(priv->dev, 0x80); /* 设置为4地址模式 */
    SPI_SELECT(priv->dev, FALSE);
    m25p_unlock(priv->dev);
#endif
    priv->mtd.erasesize = (1 << priv->sectorshift);
    priv->mtd.noofblock = priv->nsectors;
    priv->mtd.chipsize = priv->mtd.erasesize * priv->mtd.noofblock;
    noOfM25p++;
    //m25p_wpenable(priv);

    return (struct mtd_dev_s *) priv;
}

#if 0
static void
m25p_wpenable(struct m25p_dev_s *priv)
{

    /* Select this FLASH part */

    SPI_SELECT(priv->dev, TRUE);

    /* Send "Write Enable (WREN)" command */

    (void) SPI_SEND(priv->dev, 0x50);

    /* Deselect the FLASH */

    SPI_SELECT(priv->dev, FALSE);

    taskDelay(1);

    /* Select this FLASH part */

    SPI_SELECT(priv->dev, TRUE);

    /* Send "Write Enable (WREN)" command */

    (void) SPI_SEND(priv->dev, 0x01);

    (void) SPI_SEND(priv->dev, 0x00);

    (void) SPI_SEND(priv->dev, 0x00);

    /* Deselect the FLASH */

    SPI_SELECT(priv->dev, FALSE);

}

#endif

