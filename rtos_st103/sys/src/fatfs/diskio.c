/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2012        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "../../driver/spi_sd.h"
#include "ftl.h"

/* Definitions of physical drive number for each media */
#define FTL		1
#define MMC		0


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;

	switch (drv) {
	case FTL :
		return stat;

	case MMC :
        if (SD_Init() == SD_RESPONSE_NO_ERROR)
            return RES_OK;
        else
            return RES_ERROR;
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
	DSTATUS stat = RES_OK;

	switch (drv) {
	case FTL :

		return stat;

	case MMC :
		return stat;
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

		return RES_ERROR;

	case MMC :
	    //printf("read sector:%d count:%d\n", sector, count);
	    if (SD_ReadMultiBlocks(buff, sector * 512, 512, count) == SD_RESPONSE_NO_ERROR)
	        return RES_OK;
	    else
	       return RES_ERROR;
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
	DRESULT res;

	switch (drv) {
	case FTL :
		return res;

	case MMC :
        //printf("write sector:%d count:%d\n", sector, count);
        if (SD_WriteMultiBlocks((uint8_t*)buff, sector * 512, 512, count) == SD_RESPONSE_NO_ERROR)
            return RES_OK;
        else
           return RES_ERROR;
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
	DRESULT res;

	switch (drv) {
	case FTL :
		// pre-process here

		//result = FTL_disk_ioctl(ctrl, buff);

		// post-process here

		return res;

	case MMC :
		// pre-process here
	    return RES_OK;
		// post-process here

	}
	return RES_PARERR;
}
#endif
