/**
 ******************************************************************************
 * @file       devlib.c
 * @version    V1.1.4
 * @brief      设备驱动操作 todo: 1. 节点操作进入临界区 2. 重复打开关闭
 * @details    This file including all API functions's implement of dps.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#include <types.h>
#include <stdio.h>
#include <string.h>
#include <devlib.h>

static device_t* pfree = NULL;
static device_t* phead = NULL;
static device_t the_devs[MAX_DEVICE_NUM];
static device_t* the_opend_devs[MAX_OPEN_NUM];

/**
 ******************************************************************************
 * @brief 寻找空闲句柄空间
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
            return i + 1;   /* 注意返回句柄加1防止句柄 */
        }
    }
    Dprintf("can not find free fd node\n");

    return -1;
}

/**
 ******************************************************************************
 * @brief      设备核心初始化
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
 * @brief 寻找已注册的设备 (按名称查找)
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
        if (strncpy(pnode->name, pname, sizeof(pnode->name)) == 0)
        {
            return pnode;
        }
        pnode++;
    }
    return NULL;
}

/**
 ******************************************************************************
 * @brief 创建设备
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
dev_create(const char_t* pname, fileopt_t* pfileopt)
{
    device_t* new = NULL;

    if ((pname == NULL) || (pfileopt == NULL) || (pfree == NULL))
    {
        return ERROR;
    }

    /* 防止重复注册 */
    if (find_dev_by_name(pname) != NULL)
    {
        return ERROR;
    }
    new = pfree;
    pfree = new->next;

    strncpy(new->name, pname, sizeof(new->name));
    memcpy(new, pfileopt, sizeof(fileopt_t));
    if (new->fileopt.init == NULL)
    {
        if (new->fileopt.init(&new->fileopt) != OK)
        {
            /* 若执行初始化方法失败,则释放节点 */
            pfree = new;
            new = NULL;
            return ERROR;
        }
    }

    return OK;
}

/**
 ******************************************************************************
 * @brief 设备释放
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

    /* 判断设备是否在使用 */
    for (int32_t i = 0; i < MAX_OPEN_NUM; i++)
    {
        if (strncpy(the_opend_devs[i]->name, pname, sizeof(the_opend_devs[i]->name)) == 0)
        {
            Dprintf("dev:%s is using\n", pname);
            return ERROR;
        }
    }

    /* 首先判断第一个节点 */
    if (strncmp(pnode->name, pname, sizeof(pnode->name)) == 0)
    {
        phead = phead->next;
        pnode->next = pfree;
        pfree->next = pnode;
        return pnode->fileopt.release(&pnode->fileopt);
    }

    device_t* pnode_last = pnode;
    pnode = pnode->next;
    while (pnode != NULL)
    {
        if (strncmp(pnode->name, pname, sizeof(pnode->name)) == 0)
        {
            /* 找到对应节点 */
            pnode_last->next = pnode->next;
            pnode->next = pfree;
            pfree->next = pnode;
            return pnode->fileopt.release(&pnode->fileopt);
        }
        pnode_last = pnode;
        pnode++;
    }
    Dprintf("can not find name:%s", pname);

    return ERROR;
}

/**
 ******************************************************************************
 * @brief 设备打开
 * @param[in]  *pname   : 设备名
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
    device_t* pnode = pfree;

    if (pname == NULL)
    {
        return -1;
    }

    while (pnode != NULL)
    {
        if (strncmp(pnode->name, pname, sizeof(pnode->name)) == 0)
        {
            /* 找到对应节点 */
            return find_free_fd(pnode);
        }
        pnode++;    /* next node */
    }
    return -1;  /* can not find pname */
}

/**
 ******************************************************************************
 * @brief 判断句柄是否合法
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
 * @brief 设备读
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
    int32_t realfd = fd - 1;    /* 这里取得真实的fd */

    if (FALSE == is_fd_valid(realfd))
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
 * @brief 设备写
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
    int32_t realfd = fd - 1;    /* 这里取得真实的fd */

    if (FALSE == is_fd_valid(realfd))
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
 * @brief 设备ioctl
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
    int32_t realfd = fd - 1;    /* 这里取得真实的fd */
    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }
    //ioctl 不上锁吗？
    return the_opend_devs[realfd]->fileopt.ioctl(&the_opend_devs[realfd]->fileopt,
            cmd,
            args);
}

/**
 ******************************************************************************
 * @brief 设备关闭
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
    int32_t realfd = fd - 1;    /* 这里取得真实的fd */

    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }
    if (OK != the_opend_devs[realfd]->fileopt.close(&the_opend_devs[realfd]->fileopt))
    {
        return -1;
    }
    the_opend_devs[realfd] = NULL;  /* release fd */

    return 1;
}
