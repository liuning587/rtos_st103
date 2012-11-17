/**
 ******************************************************************************
 * @file       devlib.c
 * @version    V1.1.4
 * @brief      设备驱动操作
 * @details    This file including all API functions's implement of dps.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#ifndef __DEVLIB_H__
#define __DEVLIB_H__

#include <types.h>

#define MAX_DEVICE_NAME     (5u)    /**< 最大设备名长度 */
#define MAX_DEVICE_NUM      (5u)    /**< 最大设备数 */
#define MAX_OPEN_NUM        (5u)    /**< 最大同时打开设备数 */


typedef struct fileopt
{
    status_t  (*init)   (struct fileopt* dev);
    status_t  (*release)(struct fileopt* dev);
    int32_t   (*open)   (struct fileopt* dev, uint16_t oflag);
    status_t  (*close)  (struct fileopt* dev);
    size_t    (*read)   (struct fileopt* dev, int32_t pos, void *buffer, size_t size);
    size_t    (*write)  (struct fileopt* dev, int32_t pos, const void *buffer, size_t size);
    int32_t   (*ioctl)  (struct fileopt* dev, uint32_t cmd, void *args);
    status_t  (*lock)   (struct fileopt* dev);   /**< 读写锁 */
    status_t  (*unlock) (struct fileopt* dev);   /**< 读写锁 */
} fileopt_t;

typedef struct device
{
    struct device*  next;
    char_t name[MAX_DEVICE_NAME];   /**< 设备名 */
    int32_t offset;                 /**< 读写偏移地址 */
    int32_t usrs;                   /**< 设备打开次数 */
    struct fileopt fileopt;         /**< 设备操作方法 */
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
