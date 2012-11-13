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
#if 0
#define MAX_DEVICE_NAME     (5u)    /**< ����豸������ */
#define MAX_DEVICE_NUM      (5u)    /**< ����豸�� */

typedef struct
{
    status_t  (*init)   (device_t dev);
    status_t  (*open)   (device_t dev, uint16_t oflag);
    status_t  (*close)  (device_t dev);
    size_t    (*read)   (device_t dev, off_t pos, void *buffer, size_t size);
    size_t    (*write)  (device_t dev, off_t pos, const void *buffer, size_t size);
    status_t  (*ioctl)  (device_t dev, uint8_t cmd, void *args);
} fileopt_t;

#endif


#endif /* __DEVLIB_H__ */
