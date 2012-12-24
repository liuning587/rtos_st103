/**
 ******************************************************************************
 * @file       devlib.c
 * @version    V1.1.4
 * @brief      �豸�������� todo: 1. �ڵ���������ٽ��� 2. �ظ��򿪹ر�
 * @details    This file including all API functions's implement of dps.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#include <types.h>
#include <stdio.h>
#include <string.h>
#include <devlib.h>

#ifdef Dprintf
#undef Dprintf
#endif
#define Dprintf(x...)

static device_t* pfree = NULL;
static device_t* phead = NULL;
static device_t the_devs[MAX_DEVICE_NUM];
static device_t* the_opend_devs[MAX_OPEN_NUM];

/**
 ******************************************************************************
 * @brief Ѱ�ҿ��о���ռ�
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static int32_t
find_free_fd(device_t* pnode)
{
    for (int32_t i = 0; i < MAX_OPEN_NUM; i++)
    {
        if (the_opend_devs[i] == NULL)
        {
            the_opend_devs[i] = pnode;
            Dprintf("find free fd node\n");
            return i + 1;   /* ע�ⷵ�ؾ����1��ֹ��� */
        }
    }
    Dprintf("can not find free fd node\n");

    return -1;
}

/**
 ******************************************************************************
 * @brief      �豸���ĳ�ʼ��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
status_t
devlib_init(void)
{
    uint32_t i;
    static bool_e is_initd = FALSE;

    if (is_initd == TRUE)
    	return OK;
    is_initd = TRUE;
    memset(the_opend_devs, 0x00, sizeof(the_opend_devs));
    memset(the_devs, 0x00, sizeof(the_devs));
    for (i = 0; i < MAX_DEVICE_NUM - 1; i++)
    {
        the_devs[i].next = &the_devs[i + 1];
    }
    the_devs[i].next = NULL;

    pfree = &the_devs[0];
    phead = &the_devs[0];

    Dprintf("init OK\n");

    return OK;
}

/**
 ******************************************************************************
 * @brief Ѱ����ע����豸 (�����Ʋ���)
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static device_t*
find_dev_by_name(const char_t* pname)
{
    device_t* pnode = phead;
    while (pnode != NULL)
    {
        if (strncmp(pnode->name, pname, sizeof(pnode->name)) == 0)
        {
            return pnode;
        }
        pnode = pnode->next;
    }
    return NULL;
}

/**
 ******************************************************************************
 * @brief �����豸
 * @param[in]  *pname
 * @param[in]  *pfileopt
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
status_t
dev_create(const char_t* pname, const fileopt_t* pfileopt)
{
    device_t* new = NULL;

    if ((pname == NULL) || (pfileopt == NULL) || (pfree == NULL))
    {
        return ERROR;
    }

    /* ��ֹ�ظ�ע�� */
    if (find_dev_by_name(pname) != NULL)
    {
        return ERROR;
    }
    new = pfree;
    pfree = new->next;

    strncpy(new->name, pname, sizeof(new->name));
    memcpy(&new->fileopt, pfileopt, sizeof(fileopt_t));
    if (new->fileopt.init != NULL)
    {
        if (new->fileopt.init(&new->fileopt) != OK)
        {
            /* ��ִ�г�ʼ������ʧ��,���ͷŽڵ� */
            pfree = new;
            new = NULL;
            return ERROR;
        }
    }

    return OK;
}

/**
 ******************************************************************************
 * @brief �豸�ͷ�
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
status_t
dev_release(const char_t* pname)
{
    device_t* pnode = phead;

    if ((pname == NULL) || (phead == NULL))
    {
        return -1;
    }

    /* �ж��豸�Ƿ���ʹ�� */
    for (int32_t i = 0; i < MAX_OPEN_NUM; i++)
    {
        if (strncmp(the_opend_devs[i]->name, pname, sizeof(the_opend_devs[i]->name)) == 0)
        {
            Dprintf("dev:%s is using\n", pname);
            return ERROR;
        }
    }

    /* �����жϵ�һ���ڵ� */
    if (strncmp(pnode->name, pname, sizeof(pnode->name)) == 0)
    {
        phead = phead->next;
        pnode->next = pfree->next;
        pfree->next = pnode;
        if (pnode->fileopt.release != NULL)
        	return pnode->fileopt.release(&pnode->fileopt);
        return OK;
    }

    device_t* pnode_last = pnode;
    pnode = pnode->next;
    while (pnode != NULL)
    {
        if (strncmp(pnode->name, pname, sizeof(pnode->name)) == 0)
        {
            /* �ҵ���Ӧ�ڵ� */
            pnode_last->next = pnode->next;
            pnode->next = pfree->next;
            pfree->next = pnode;
            if (pnode->fileopt.release != NULL)
            	return pnode->fileopt.release(&pnode->fileopt);
            return OK;
        }
        pnode_last = pnode;
        pnode = pnode->next;
    }
    Dprintf("can not find name:%s", pname);

    return ERROR;
}

/**
 ******************************************************************************
 * @brief �豸��
 * @param[in]  *pname   : �豸��
 * @param[in]   flags   : O_RDONLY | OWRONLY | O_RDWR
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t
dev_open(const char_t* pname, int32_t flags)
{
    device_t* pnode = phead;

    if (pname == NULL)
    {
        return -1;
    }

    while (pnode != pfree)
    {
        if (strncmp(pnode->name, pname, sizeof(pnode->name)) == 0)
        {
            /* �ҵ���Ӧ�ڵ� */
            int32_t fd = find_free_fd(pnode);

            if ((fd > 0) && (pnode->fileopt.open != NULL))
            {
                pnode->fileopt.open(&pnode->fileopt, flags);    /* ִ��open */
            }
            return fd;
        }
        pnode = pnode->next;    /* next node */
    }
    return -1;  /* can not find pname */
}

/**
 ******************************************************************************
 * @brief �жϾ���Ƿ�Ϸ�
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static bool_e
is_fd_valid(int32_t realfd)
{
    if (realfd >= MAX_OPEN_NUM)
    {
        Dprintf("fd is out of range!\n");
        return FALSE;
    }

    if (the_opend_devs[realfd] == NULL)
    {
        Dprintf("fd is not opend!\n");
        return FALSE;
    }

    return TRUE;
}

/**
 ******************************************************************************
 * @brief �豸��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t
dev_read(int32_t fd, void* buf, int32_t count)
{
    int32_t size = -1;
    int32_t realfd = fd - 1;    /* ����ȡ����ʵ��fd */

    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }

    if (the_opend_devs[realfd]->fileopt.read == NULL)
    {
        return -1;
    }

    if (the_opend_devs[realfd]->fileopt.lock != NULL)
    {
        if (OK != the_opend_devs[realfd]->fileopt.lock(&the_opend_devs[realfd]->fileopt))
        {
            return -1;
        }

    }

    size = the_opend_devs[realfd]->fileopt.read(&the_opend_devs[realfd]->fileopt,
            the_opend_devs[realfd]->offset,
            buf,
            count);
    if (the_opend_devs[realfd]->fileopt.unlock != NULL)
    {
        if (OK != the_opend_devs[realfd]->fileopt.unlock(&the_opend_devs[realfd]->fileopt))
        {
            return -1;
        }
    }
    return size;
}

/**
 ******************************************************************************
 * @brief �豸д
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t
dev_write(int32_t fd, const void* buf, int32_t count)
{
    int32_t size = -1;
    int32_t realfd = fd - 1;    /* ����ȡ����ʵ��fd */

    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }

    if (the_opend_devs[realfd]->fileopt.write == NULL)
    {
        return -1;
    }

    if (the_opend_devs[realfd]->fileopt.lock != NULL)
    {
        if (OK != the_opend_devs[realfd]->fileopt.lock(&the_opend_devs[realfd]->fileopt))
        {
            return -1;
        }

    }
    size = the_opend_devs[realfd]->fileopt.write(&the_opend_devs[realfd]->fileopt,
            the_opend_devs[realfd]->offset,
            buf,
            count);
    if (the_opend_devs[realfd]->fileopt.unlock != NULL)
    {
        if (OK != the_opend_devs[realfd]->fileopt.unlock(&the_opend_devs[realfd]->fileopt))
        {
            return -1;
        }
    }
    return size;
}

/**
 ******************************************************************************
 * @brief �豸ioctl
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t
dev_ioctl(int32_t fd, uint32_t cmd, void *args)
{
    int32_t realfd = fd - 1;    /* ����ȡ����ʵ��fd */
    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }

    if (the_opend_devs[realfd]->fileopt.ioctl == NULL)
    {
        return -1;
    }

    //ioctl ��������
    return the_opend_devs[realfd]->fileopt.ioctl(&the_opend_devs[realfd]->fileopt,
            cmd,
            args);
}

/**
 ******************************************************************************
 * @brief �豸�ر�
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t
dev_close(int32_t fd)
{
    int32_t realfd = fd - 1;    /* ����ȡ����ʵ��fd */

    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }

    if (the_opend_devs[realfd]->fileopt.close != NULL)
    {
        if (OK != the_opend_devs[realfd]->fileopt.close(&the_opend_devs[realfd]->fileopt))
        {
            return -1;
        }
    }
    the_opend_devs[realfd] = NULL;  /* release fd */

    return 1;
}
