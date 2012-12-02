/***********************************************************************************
    Filename: cc1101.h
    cc1101寄存器定义头文件(与cc1101相同)
    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#ifndef CC1101_H
#define CC1101_H

#include <wlmdev.h>

#define CC1101_MAX_TRANSMISSION_SIZE   (64u)    //设置单包传输20字节
#define CC1101_RBUFS                   (8u)     //设置接收缓存包个数


extern status_t cc1101_init(uint8_t addr);
extern int32_t cc1101_send(uint8_t addr, const uint8_t* pbuf, int32_t len);
extern int32_t cc1101_recv(uint8_t addr, uint8_t* pbuf, int32_t len, uint32_t time_out);

/**********************************************************************************/
#endif
