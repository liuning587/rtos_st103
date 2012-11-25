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
		return RES_ERROR;

	case MMC :
	    return sd_initialize();
#if 0
        if (SD_Init() == SD_RESPONSE_NO_ERROR)
            return RES_OK;
        else
            return RES_ERROR;
#endif
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

		return RES_ERROR;

	case MMC :
	    return sd_read(buff, sector, count);
#if 0
	    //printf("read sector:%d count:%d\n", sector, count);
	    if (SD_ReadMultiBlocks(buff, sector * 512, 512, count) == SD_RESPONSE_NO_ERROR)
	        return RES_OK;
	    else
	       return RES_ERROR;
#endif
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
		return RES_ERROR;

	case MMC :
        return sd_write(buff, sector, count);
#if 0
        //printf("write sector:%d count:%d\n", sector, count);
        if (SD_WriteMultiBlocks((uint8_t*)buff, sector * 512, 512, count) == SD_RESPONSE_NO_ERROR)
            return RES_OK;
        else
           return RES_ERROR;
#endif
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
		// pre-process here

		//result = FTL_disk_ioctl(ctrl, buff);

		// post-process here

		return RES_ERROR;

	case MMC :
        return sd_ioctl(ctrl, buff);
		// pre-process here
	    //return RES_OK;
		// post-process here

	}
	return RES_PARERR;
}
#endif
