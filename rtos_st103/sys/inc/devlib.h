/**
 ******************************************************************************
 * @file       devlib.c
 * @version    V1.1.4
 * @brief      �豸��������
 * @details    This file including all API functions's implement of dps.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#ifndef __DEVLIB_H__
#define __DEVLIB_H__

#include <types.h>

#define MAX_DEVICE_NAME     (5u)    /**< ����豸������ */
#define MAX_DEVICE_NUM      (5u)    /**< ����豸�� */
#define MAX_OPEN_NUM        (5u)    /**< ���ͬʱ���豸�� */


typedef struct fileopt
{
    status_t  (*init)   (struct fileopt* dev);
    status_t  (*release)(struct fileopt* dev);
    int32_t   (*open)   (struct fileopt* dev, uint16_t oflag);
    status_t  (*close)  (struct fileopt* dev);
    size_t    (*read)   (struct fileopt* dev, int32_t pos, void *buffer, size_t size);
    size_t    (*write)  (struct fileopt* dev, int32_t pos, const void *buffer, size_t size);
    int32_t   (*ioctl)  (struct fileopt* dev, uint32_t cmd, void *args);
    status_t  (*lock)   (struct fileopt* dev);   /**< ��д�� */
    status_t  (*unlock) (struct fileopt* dev);   /**< ��д�� */
} fileopt_t;

typedef struct device
{
    struct device*  next;
    char_t name[MAX_DEVICE_NAME];   /**< �豸�� */
    int32_t offset;                 /**< ��дƫ�Ƶ�ַ */
    int32_t usrs;                   /**< �豸�򿪴��� */
    struct fileopt fileopt;         /**< �豸�������� */
} device_t;

extern status_t devlib_init(void);
extern status_t dev_create(const char_t* pname, const fileopt_t* pfileopt);
extern status_t dev_release(const char_t* pname);
extern int32_t dev_open(const char_t* pname, int32_t flags);
extern int32_t dev_read(int32_t fd, void* buf, int32_t count);
extern int32_t dev_write(int32_t fd, const void* buf, int32_t count);
extern int32_t dev_ioctl(int32_t fd, uint32_t cmd, void *args);
extern int32_t dev_close(int32_t fd);

#endif /* __DEVLIB_H__ */
