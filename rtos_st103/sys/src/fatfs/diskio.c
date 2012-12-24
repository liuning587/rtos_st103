/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2012        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "../../driver/spi_sd.h"
#include "ftl.h"

/* Definitions of physical drive number for each media */
#define FTL		1
#define MMC		0

extern DSTATUS sd_initialize (void);
extern DSTATUS sd_status (void);
extern DRESULT sd_read (
        BYTE *buff,         /* Pointer to the data buffer to store read data */
        DWORD sector,       /* Start sector number (LBA) */
        BYTE count          /* Sector count (1..128) */
    );
extern DRESULT sd_write (
        const BYTE *buff,   /* Pointer to the data to be written */
        DWORD sector,       /* Start sector number (LBA) */
        BYTE count          /* Sector count (1..128) */
    );
extern DRESULT sd_ioctl (
        BYTE ctrl,      /* Control code */
        void *buff      /* Buffer to send/receive control data */
    );
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	switch (drv) {
	case FTL :
	    if (ftlInit() == OK)
	    {
	        return RES_OK;
	    }
	    else
	    {
	        return RES_ERROR;
	    }

	case MMC :
	    return sd_initialize();

	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	switch (drv) {
	case FTL :
		return RES_OK;

	case MMC :
		return sd_status();
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* DFTL buffer to store read dFTL */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
	switch (drv) {
	case FTL :
	    if (ftlRead(sector * 512, buff, count * 512) == OK)
	    {
	        return RES_OK;
	    }
	    else
	    {
	        return RES_ERROR;
	    }

	case MMC :
	    return sd_read(buff, sector, count);
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* DFTL to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
	switch (drv) {
	case FTL :
        if (ftlWrite(sector * 512, (uint8_t* )buff, count * 512) == OK)
        {
            return RES_OK;
        }
        else
        {
            return RES_ERROR;
        }

	case MMC :
        return sd_write(buff, sector, count);
	}
	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control dFTL */
)
{
	switch (drv) {
	case FTL :
	    switch (ctrl)
	    {
	        case CTRL_SYNC :        /* Make sure that no pending write process */
	            return RES_OK;

	        case GET_SECTOR_COUNT : /* Get number of sectors on the disk (DWORD) */
                *(DWORD*)buff = (DWORD)ftl_get_sector_count();
                return RES_OK;

	        case GET_BLOCK_SIZE :   /* Get erase block size in unit of sector (DWORD) */
	            *(DWORD*)buff = (DWORD)512u;
	            return RES_OK;

	        default:
	            return RES_PARERR;
	    }
		return RES_ERROR;

	case MMC :
        return sd_ioctl(ctrl, buff);
	}

	return RES_PARERR;
}
#endif
