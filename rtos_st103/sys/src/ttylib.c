/**
 ******************************************************************************
 * @file       ttylib.c
 * @version    V1.1.4
 * @brief      API C source file of ttylib.c
 *             Created on: 2012-9-25
 *             Author: Administrator
 * @details    This file including all API functions's implement of dps.
 *
 ******************************************************************************
 */

/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <ttylib.h>
#include <string.h>
#include <maths.h>


/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#define TYBUFSIZE                       256     /*TTY�Ļ����С*/
#define TYNUM                           3       /*TTY�豸�Ĵ�С*/

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 Section: static Variable
 ----------------------------------------------------------------------------*/
static const int32_t ty_num = TYNUM;            /*TTY������*/
static const int32_t ty_bufsize = TYBUFSIZE;    /*ÿ��TTY��BUF��С*/
static TY_DEV ty_devs[TYNUM];                   /*TTY�豸*/
static uint8_t rbuf[TYNUM * TYBUFSIZE];         /*����TTY���ջ���*/
static uint8_t wbuf[TYNUM * TYBUFSIZE];         /*����TTY�ķ��ͻ���*/
/*-----------------------------------------------------------------------------
 Section: static Function Prototypes
 ----------------------------------------------------------------------------*/
static status_t tyITx(uint32_t ttyno, uint8_t *pChar);
static status_t tyIRd(uint32_t ttyno, uint8_t inchar);

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/

/******************************************************************************
*
* ttyDevCreate -
*
* INPUTS:
*   uint8_t *name: name to use for this device
*     SIO_CHAN *  pSioChan:  pointer to core driver structure
*   uint32_t rdBufSize : read buffer size, in bytes
*     uint32_t wrtBufSize :write buffer size, in bytes
* RETURNS:   OK, or ERROR if the driver is not installed,
*     or the device already exists.
*
******************************************************************************/
status_t
ttyDrv(void)
{
    memset(ty_devs, 0, ty_num * sizeof(TY_DEV));
    memset(rbuf, 0, ty_num * ty_bufsize);
    memset(wbuf, 0, ty_num * ty_bufsize);

    return OK;
}

/******************************************************************************
*
* ttyDevCreate -
*
* INPUTS:
*   uint8_t *name: name to use for this device
*     SIO_CHAN *  pSioChan:  pointer to core driver structure
*   uint32_t rdBufSize : read buffer size, in bytes
*     uint32_t wrtBufSize :write buffer size, in bytes
* RETURNS:   OK, or ERROR if the driver is not installed,
*     or the device already exists.
*
******************************************************************************/
status_t
ttyDevCreate(uint8_t *name, SIO_CHAN *pChan)
{
    uint8_t i = 0;
    // ��λ��δ������dev
    while ((0 != ty_devs[i].fd) && (i < ty_num))
    {
        i++;
    }
    if (i >= ty_num)
    {
        return ERROR;
    }

    memcpy(ty_devs[i].name, name, 5);
    ty_devs[i].fd = i + 1;
    ty_devs[i].pSioChan = pChan;
    ty_devs[i].rdSyncSem = semBCreate(1);
    ty_devs[i].wrtSyncSem = semBCreate(1);
    ty_devs[i].rdBuf.bufSize = ty_bufsize;
    ty_devs[i].wrtBuf.bufSize = ty_bufsize;
    ty_devs[i].rdBuf.buf = &rbuf[0] + i * ty_bufsize;
    ty_devs[i].wrtBuf.buf = &wbuf[0] + i * ty_bufsize;
    pChan->pDrvFuncs->getTxChar = tyITx;
    pChan->pDrvFuncs->putRcvChar = tyIRd;

    return OK;

}

/******************************************************************************
*
* ttyIoctl -
*
* INPUTS:
*   uint32_t fd :
*     int32_t request:  CMD
*   uint32_t arg:
* RETURNS:   OK, or ERROR if the ttyioctl is error
*
******************************************************************************/
status_t
ttyIoctl(uint32_t fd, uint32_t request, uint32_t arg)
{
    if (fd > ty_num || fd < 1)
    {
        return ERROR;
    }
    if (ty_devs[fd-1].fd == 0)
    {
        return ERROR;
    }

    if(request == SIO_FIOFLUSH)
    {
        semTake(ty_devs[fd-1].rdSyncSem, WAIT_FOREVER);
        ty_devs[fd-1].rdBuf.pFromBuf = ty_devs[fd-1].rdBuf.pToBuf;
        semGive(ty_devs[fd-1].rdSyncSem);
        return OK;
    }
    if (request == SIO_FIONREAD)
    {
        uint32_t tobuf = ty_devs[fd-1].rdBuf.pToBuf;
        semTake (ty_devs[fd-1].rdSyncSem, WAIT_FOREVER );
        //δ��β�����
        if(ty_devs[fd-1].rdBuf.pFromBuf <= tobuf )
        {
          *((uint32_t*)arg) = tobuf - ty_devs[fd-1].rdBuf.pFromBuf;
        }
        //��β�����
        else
        {
         *((uint32_t*)arg)  = tobuf + ty_devs[fd-1].rdBuf.bufSize - ty_devs[fd-1].rdBuf.pFromBuf;
        }
        semGive(ty_devs[fd-1].rdSyncSem);
        return OK;
    }

    return ty_devs[fd-1].pSioChan->pDrvFuncs->ioctl(ty_devs[fd-1].pSioChan, request, (void*)arg);

}

/******************************************************************************
*
* ttyRead -������
*
* INPUTS:
*   uint32_t fd :    ���
*     uint8_t *buffer:      ���صĻ�����
*   uint32_t maxbytes:   ��ȡ������ֽ���
* RETURNS:   ʵ�ʷ��ص��ֽ���
*
******************************************************************************/
uint32_t
ttyRead(uint32_t fd, uint8_t *buffer, uint32_t maxbytes)
{
    uint32_t bytenum = 0;
    uint32_t i = 0, tobuf = 0;

    if (fd > ty_num || fd < 1)
    {
        return 0;
    }
    if (ty_devs[fd - 1].fd == 0)
    {
        return 0;
    }

    semTake(ty_devs[fd - 1].rdSyncSem, WAIT_FOREVER);
    tobuf = ty_devs[fd - 1].rdBuf.pToBuf;
    //δ��β�����
    if (ty_devs[fd - 1].rdBuf.pFromBuf <= tobuf)
    {
        bytenum = tobuf - ty_devs[fd - 1].rdBuf.pFromBuf;
    }
    //��β�����
    else
    {
        bytenum = tobuf + ty_devs[fd - 1].rdBuf.bufSize
                - ty_devs[fd - 1].rdBuf.pFromBuf;
    }
    for (i = 0; i < MIN(bytenum, maxbytes); i++)
    {
        *buffer = ty_devs[fd - 1].rdBuf.buf[ty_devs[fd - 1].rdBuf.pFromBuf];
        buffer++;
        ty_devs[fd - 1].rdBuf.pFromBuf++;
        if (ty_devs[fd - 1].rdBuf.pFromBuf >= ty_devs[fd - 1].rdBuf.bufSize)
        {
            ty_devs[fd - 1].rdBuf.pFromBuf = 0;
        }
    }
    semGive(ty_devs[fd - 1].rdSyncSem);
    return MIN(bytenum,maxbytes);

}

/******************************************************************************
*
* ttyWrite -д����
*
* INPUTS:
*   uint32_t fd :    ���
*     uint8_t *buffer:      д��Ļ�����
*   uint32_t nbytes:   д����ֽ���
* RETURNS:   ʵ��д����ֽ���
*
 ******************************************************************************/
uint32_t
ttyWrite(uint32_t fd, uint8_t *buffer, uint32_t nbytes)
{
    uint32_t bytenum = 0;
    uint32_t i = 0, frombuf = 0;
    uint32_t sendnum = nbytes;
    if (fd > ty_num || fd < 1)
    {
        return 0;
    }
    if (ty_devs[fd - 1].fd == 0)
    {
        return 0;
    }

    semTake(ty_devs[fd - 1].wrtSyncSem, WAIT_FOREVER);

    while (nbytes != 0)
    {
        frombuf = ty_devs[fd - 1].wrtBuf.pFromBuf;
        //δ��β�����
        if (ty_devs[fd - 1].wrtBuf.pToBuf < frombuf)
        {
            bytenum = frombuf - ty_devs[fd - 1].wrtBuf.pToBuf - 1;
        }
        //��β�����
        else
        {
            bytenum = frombuf + ty_devs[fd - 1].wrtBuf.bufSize - 1
                    - ty_devs[fd - 1].wrtBuf.pToBuf;
        }
        for (i = 0; i < MIN(bytenum,nbytes); i++)
        {
            ty_devs[fd - 1].wrtBuf.buf[ty_devs[fd - 1].wrtBuf.pToBuf] = *buffer;
            buffer++;
            ty_devs[fd - 1].wrtBuf.pToBuf++;
            if (ty_devs[fd - 1].wrtBuf.pToBuf >= ty_devs[fd - 1].wrtBuf.bufSize)
            {
                ty_devs[fd - 1].wrtBuf.pToBuf = 0;
            }
        }
        nbytes -= MIN(bytenum,nbytes);
        ty_devs[fd - 1].pSioChan->pDrvFuncs->txStartup(ty_devs[fd - 1].pSioChan);
        if (nbytes != 0)
        {
            taskDelay(1);
        }
    }

    semGive(ty_devs[fd - 1].wrtSyncSem);
    return sendnum;

}


/******************************************************************************
*
* ttyOpen -�򿪴���
*
* INPUTS:
*   uint8* name  �豸����
 *
 * RETURNS:   ���ؾ�� �����ʧ�ܷ��� 0
 *
 ******************************************************************************/
uint32_t
ttyOpen(uint8_t* name)
{
    uint32_t i;

    for (i = 0; i < ty_num; i++)
    {
        if (memcmp(ty_devs[i].name, name, 5) == 0)
        {
            if (ty_devs[i].pSioChan->pDrvFuncs->ioctl(ty_devs[i].pSioChan,
                    SIO_OPEN, 0) == OK)
            {
                return ty_devs[i].fd;
            }
            else
            {
                return 0;
            }
        }
    }

    return 0;

}

/******************************************************************************
*
* ttyClose -�رմ���
*
* INPUTS:
*   uint8* name  �豸����
*
* RETURNS:   ���ؾ�� �����ʧ�ܷ��� -1
*
 ******************************************************************************/
status_t
ttyClose(uint32_t fd)
{
    if (fd > ty_num || fd < 1)
    {
        return ERROR;
    }
    return ty_devs[fd - 1].pSioChan->pDrvFuncs->ioctl(ty_devs[fd - 1].pSioChan,
            SIO_HUP, 0);
}


/******************************************************************************
*
* tyITx -�жϼ�������
*
* INPUTS:
*   uint8* ttyno  tty ��
*     uint8_t *pChar
*
* RETURNS:  �Ƿ�ɹ�
*
******************************************************************************/
static int32_t
tyITx(uint32_t ttyno, uint8_t *pChar)
{
    if (ttyno >= ty_num)
    {
        return ERROR;
    }
    if (ty_devs[ttyno].fd == 0)
    {
        return ERROR;
    }
    if (ty_devs[ttyno].wrtBuf.pFromBuf == ty_devs[ttyno].wrtBuf.pToBuf)
    {
        return ERROR;
    }
    *pChar = ty_devs[ttyno].wrtBuf.buf[ty_devs[ttyno].wrtBuf.pFromBuf];
    ty_devs[ttyno].wrtBuf.pFromBuf++;
    if (ty_devs[ttyno].wrtBuf.pFromBuf >= ty_devs[ttyno].wrtBuf.bufSize)
    {
        ty_devs[ttyno].wrtBuf.pFromBuf = 0;
    }
    return OK;

}
/******************************************************************************
*
* tyIRd -�жϼ��������
*
* INPUTS:
*   uint8* ttyno  tty ��
*     uint8_t   inchar
*
* RETURNS:  �Ƿ�ɹ�
*
******************************************************************************/
static int32_t
tyIRd(uint32_t ttyno, uint8_t inchar)
{
    if (ttyno >= ty_num)
    {
        return ERROR;
    }
    if (ty_devs[ttyno].fd == 0)
    {
        return ERROR;
    }
    ty_devs[ttyno].rdBuf.buf[ty_devs[ttyno].rdBuf.pToBuf] = inchar;
    ty_devs[ttyno].rdBuf.pToBuf++;
    if (ty_devs[ttyno].rdBuf.pToBuf >= ty_devs[ttyno].rdBuf.bufSize)
    {
        ty_devs[ttyno].rdBuf.pToBuf = 0;
    }

    return OK;

}

/******************************************************************************
*
* ttyGet -��ȡTTY�豸
*
* INPUTS:
*   uint8* ttyno  tty ��
*
* RETURNS:  �Ƿ�ɹ�
*
******************************************************************************/
TY_DEV*
ttyGet(int32_t i)
{
    if (ty_devs[i].fd != 0)
    {
        return &ty_devs[i];
    }
    else
    {
        return NULL;
    }

}

/******************************End of ttylib.c ******************************/
