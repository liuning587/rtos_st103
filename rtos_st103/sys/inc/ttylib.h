/**
 ******************************************************************************
 * @file       ttylib.h
 * @version    V0.0.1
 * @brief      TTY设备.
 * @details    This file including all API functions's implement of intlib.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#ifndef _TTYLIB_H_
#define _TTYLIB_H_


/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <sched.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define FIONREAD        1       /* get num chars available to read */
#define FIOFLUSH        2       /* flush any chars in buffers */
#define FIOOPTIONS      3       /* set options (FIOSETOPTIONS) */
#define FIOBAUDRATE     4       /* set serial baud rate */
#define FIODISKFORMAT   5       /* format disk */
#define FIODISKINIT     6       /* initialize disk directory */
#define FIOSEEK         7       /* set current file char position */
#define FIOWHERE        8       /* get current file char position */
#define FIODIRENTRY     9       /* return a directory entry (obsolete)*/
#define FIORENAME       10      /* rename a directory entry */
#define FIOREADYCHANGE  11      /* return TRUE if there has been a
                                   media change on the device */
#define FIONWRITE       12      /* get num chars still to be written */
#define FIODISKCHANGE   13      /* set a media change on the device */
#define FIOCANCEL       14      /* cancel read or write on the device */
#define FIOSQUEEZE      15      /* squeeze out empty holes in rt-11
                                 * file system */
#define FIONBIO         16      /* set non-blocking I/O; SOCKETS ONLY!*/
#define FIONMSGS        17      /* return num msgs in pipe */
#define FIOGETNAME      18      /* return file name in arg */
#define FIOGETOPTIONS   19      /* get options */
#define FIOSETOPTIONS   FIOOPTIONS  /* set options */
#define FIOISATTY       20      /* is a tty */
#define FIOSYNC         21      /* sync to disk */
#define FIOPROTOHOOK    22      /* specify protocol hook routine */
#define FIOPROTOARG     23      /* specify protocol argument */
#define FIORBUFSET      24      /* alter the size of read buffer  */
#define FIOWBUFSET      25      /* alter the size of write buffer */
#define FIORFLUSH       26      /* flush any chars in read buffers */
#define FIOWFLUSH       27      /* flush any chars in write buffers */
#define FIOSELECT       28      /* wake up process in select on I/O */
#define FIOUNSELECT     29      /* wake up process in select on I/O */
#define FIONFREE        30      /* get free byte count on device */
#define FIOMKDIR        31      /* create a directory */
#define FIORMDIR        32      /* remove a directory */
#define FIOLABELGET     33      /* get volume label */
#define FIOLABELSET     34      /* set volume label */
#define FIOATTRIBSET    35      /* set file attribute */
#define FIOCONTIG       36      /* allocate contiguous space */
#define FIOREADDIR      37      /* read a directory entry (POSIX) */
#define FIOFSTATGET     38      /* get file status_t info */
#define FIOUNMOUNT      39      /* unmount disk volume */
#define FIOSCSICOMMAND  40      /* issue a SCSI command */
#define FIONCONTIG      41      /* get size of max contig area on dev */
#define FIOTRUNC        42      /* truncate file to specified length */
#define FIOGETFL        43      /* get file mode, like fcntl(F_GETFL) */
#define FIOTIMESET      44      /* change times on a file for utime() */
#define FIOINODETONAME  45      /* given inode number, return filename*/
#define FIOFSTATFSGET   46      /* get file system status_t info */
#define FIOMOVE         47      /* move file, ala mv, (mv not rename) */

/* serial device I/O controls */
#define SIO_HW_OPTS_SET     0x1005u

#define SIO_MODE_SET        0x1007u
#define SIO_MODE_GET        0x1008u
#define SIO_AVAIL_MODES_GET 0x1009u
#define SIO_FIOFLUSH        0x1010u
#define SIO_FIONREAD        0x1011u
#define SIO_INCBUF          0x1012u


/* These are used to communicate open/close between layers */

#define SIO_OPEN        0x100Au
#define SIO_HUP         0x100Bu

#define SIO_TXISE       0X100Cu    /*TX FIFO IS EMPTY ?*/



/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/

typedef struct sio_drv_funcs SIO_DRV_FUNCS;

typedef struct sio_chan             /* a serial channel */
    {
    SIO_DRV_FUNCS  *pDrvFuncs;
    /* device data */
    } SIO_CHAN;



struct sio_drv_funcs                /* driver functions */
{
    int32_t (*ioctl)
            (
            SIO_CHAN *  pSioChan,
            uint32_t    cmd,
            void *      arg
            );

    int32_t (*txStartup)
            (
            SIO_CHAN *  pSioChan
            );

    int32_t (*putRcvChar)
            (
            uint32_t    ttyno,
            uint8_t     pchar
            );
    int32_t (*getTxChar)
            (
            uint32_t    ttyno,
            uint8_t *   pchar
            );
};



typedef struct      /* RING - ring buffer */
{
    int32_t pToBuf;     /* offset from start of buffer where to write next */
    int32_t pFromBuf;   /* offset from start of buffer where to read next */
    int32_t bufSize;    /* size of ring in bytes */
    uint8_t *buf;        /* pointer to start of buffer */
} RING;

typedef RING RING_ID;

typedef struct      /* TY_DEV  */
{
    uint32_t    fd;         /* file descriptor*/
    uint8_t     name[5];    /* device name */
    RING_ID     rdBuf;      /* ring buffer for read */
    RING_ID     wrtBuf;     /* ring buffer for write */
    SEM_ID      rdSyncSem;  /* reader synchronization semaphore */
    SEM_ID      wrtSyncSem; /* writer synchronization semaphore */
    SIO_CHAN *  pSioChan;
} TY_DEV;

#define WordLength_8b                  ((uint16_t)0x0000)
#define WordLength_9b                  ((uint16_t)0x1000)

#define StopBits_1                     ((uint16_t)0x0000)
#define StopBits_0_5                   ((uint16_t)0x1000)
#define StopBits_2                     ((uint16_t)0x2000)
#define StopBits_1_5                   ((uint16_t)0x3000)

#define Parity_No                      ((uint16_t)0x0000)
#define Parity_Even                    ((uint16_t)0x0400)
#define Parity_Odd                     ((uint16_t)0x0600)

typedef struct
{
    uint32_t baudrate;      /**< 波特率 */
    uint16_t wordlength;    /**< 数据位 */
    uint16_t stopbits;      /**< 停止位 */
    uint16_t parity;        /**< 校验位 */
} tty_param_t;
/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
extern status_t   ttyDrv (void);
extern status_t   ttyDevCreate (uint8_t *name, SIO_CHAN *pChan);
extern uint32_t   ttyRead (uint32_t fd, uint8_t *buffer, uint32_t maxbytes);
extern uint32_t   ttyWrite (uint32_t fd, uint8_t *buffer, uint32_t nbytes);
extern status_t   ttyIoctl (uint32_t fd, uint32_t request, uint32_t arg);
extern uint32_t   ttyOpen (uint8_t* name);
extern status_t   ttyClose (uint32_t fd);
extern TY_DEV*    ttyGet(int32_t i);

#endif /*_TTYLIB_H_ */

/*------------------------------End of ttylib.h------------------------------*/
