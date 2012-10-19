/**
 ******************************************************************************
 * @file       update.c
 * @version    V0.0.1
 * @brief      API C source file of update.
 * @details    This file including all API functions's implement of update.
 * @copy       Copyrigth(C), 2012-2015.
 *
 ******************************************************************************
 */
 
/*------------------------------------------------------------------------------
Section: Includes
------------------------------------------------------------------------------*/
#include <string.h>
#include <lib.h>
#include <cfg.h>
#include <bspitf.h>

/*------------------------------------------------------------------------------
Section: Macro Definitions 
------------------------------------------------------------------------------*/
/* 命令码定义  */
/* 0x20 测试连接  */
#define COMMAND_PING             0x20
/* 0x21 开始下载  */
#define COMMAND_DOWNLOAD         0x21
/* 0x23 获取状态  */
#define COMMAND_GET_STATUS       0x23
/* 0x24 发送数据 */
#define COMMAND_SEND_DATA        0x24
/*0x25 复位 */
#define COMMAND_RESET            0x25
/* 应答码 */
/* 0xCC 接收成功 */
#define COMMAND_ACK              0xCC
/* 0x33 接收失败 */
#define COMMAND_NAK              0x33

/* 状态码 */
/* 0x40 成功 */
#define COMMAND_RET_SUCCESS      0x40
/* 0x41 未知命令 */
#define COMMAND_RET_UNKNOWN_CMD  0x41
/* 0x42 非法命令 */
#define COMMAND_RET_INVALID_CMD  0x42
/* 0x43 非法参数 */
#define COMMAND_RET_INVALID_PARA 0x43
/* 0x44 FLASH操作失败 */
#define COMMAND_RET_FLASH_FAIL   0x44
/* 0x45 缓冲区不足 */
#define COMMAND_RET_BUFFER_FAIL  0x45

/* DataFlash信息 */
/* 信息所在地址 */
#define UPDATE_CFG_START_ADDR    0x0
/* 信息标识 */
#define UPDATE_FLAG              "s103"

/*------------------------------------------------------------------------------
Section: Private Type Definitions 
------------------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)    //设定为1字节对齐

typedef struct {
    uint8  flag[4];             /* if "sxdq", Update sucess */
    uint32 addr;                /* 内容存储地址 */
    uint32 len;                 /* 数据内容长度 */
} T_UPDATE_INFO;

#pragma pack(pop)

extern uint32 get_systime(void);
extern boolean uart_tryReceive(uint8 *pData);
void reset(void);
/*------------------------------------------------------------------------------
Section: Private Function Prototypes
------------------------------------------------------------------------------*/
LOCAL boolean esc_key_detect(void);
LOCAL boolean send_hello(void);
LOCAL boolean receive_packet(uint8 *cmd, uint8 *data, uint8 *size);
LOCAL boolean send_status(uint8 s);
LOCAL void send_ack(void);
LOCAL void send_nak(void);
LOCAL boolean check_sum(const uint8 cmd, const uint8 *data,
                     const uint8 size, const uint8 sum);
LOCAL void send_packet(const uint8 *packet, uint16 size);

/*------------------------------------------------------------------------------
Section: Private Variables
------------------------------------------------------------------------------*/
/* 确认包 */
LOCAL const uint8 PACKET_ACK[] = {0, 0, COMMAND_ACK};
/* 否认包 */
LOCAL const uint8 PACKET_NAK[] = {0, 0, COMMAND_NAK};

/*------------------------------------------------------------------------------
Section: Function Definitions
------------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      df_update
 * @param[in]  None
 * @param[out] None
 * @retval     true-成功、false-失败
 *
 * @details    从串口进行升级
 *
 * @note 
 ******************************************************************************
 */
STATUS
uart_update(void) {
    uint32 startAddress = 0,
           transferSize = 0;
    uint8 size;
    uint8 status;
    uint8 cmd,
		  data[MAX_BUF_SIZE];

    print("Press ESC to upgrade from serial port ...\n\r");

    /* 检测是否要从串口进行升级 */
    if (!esc_key_detect())
        return (OK);

    print("Receiving data ...");

    /* 过滤多余按键或串口切换产生的错误字节 */
    while (!send_hello()) {
    }

    startAddress = 0xffffffff;

    status = COMMAND_RET_SUCCESS;
    while (true) {
        /* 如果未正确接收报文，放弃处理 */
        if (!receive_packet(&cmd, data, &size)) {
            send_nak();
            continue;
        }

        /* 根据命令字进行处理 */
        switch (cmd) {
			/* 测试连接 */
            case COMMAND_PING:
                status = COMMAND_RET_SUCCESS;

                /* 返回确认包 */
                send_ack();

                break;

            /* 开始下载 */
            case COMMAND_DOWNLOAD:
                status = COMMAND_RET_SUCCESS;
                do {
                    /* 检查报文长度 */
                    if (size != 8) {
                        status = COMMAND_RET_INVALID_CMD;

                        break;
                    }

                    /**
                     * 起始地址
                     */
                    startAddress = (data[0] << 24)
                                 | (data[1] << 16)
                                 | (data[2] <<  8)
                                 |  data[3];
                    /* 起始地址必须为0，由各BootLoader自行决定写入位置 */
                    if (startAddress != 0u)
                    {
                        status = COMMAND_RET_INVALID_CMD;
                        break;
                    }
                    startAddress = APP_START_ADDRESS;
                    /**
                     * 下载字节数
                     */
                    transferSize = (data[4] << 24)
                                 | (data[5] << 16)
                                 | (data[6] <<  8)
                                 |  data[7];

                    /**
                     * 初始化Flash
                     */
                    iflash_init();

                    /**
                     * 擦除将要更新的空间
                     */
                    if (!iflash_erase(startAddress, transferSize))
                        status = COMMAND_RET_FLASH_FAIL;
                } while(false);

                if (status != COMMAND_RET_SUCCESS)
                    transferSize = 0;

                /**
                 * 确认报文已得到处理
                 */
                send_ack();

                break;

            /**
             * 获取状态
             */
            case COMMAND_GET_STATUS:
                /**
                 * 返回确认包
                 */
                send_ack();

                /**
                 * 返回当前状态
                 */
                send_status(status);

                break;
            /**
             * 数据
             */
            case COMMAND_SEND_DATA:
                status = COMMAND_RET_SUCCESS;

                /**
                 * 注意:后台程序应保证size按字对齐，否则写入结果无法预料
                 * 检查是否还有数据未写入
                 */
                if (transferSize >= size) {
                    if (!iflash_write(startAddress, data, size))
                        status = COMMAND_RET_FLASH_FAIL;
                    else {
                        transferSize -= size;
                        startAddress += size;
                    }
                }
                else status = COMMAND_RET_INVALID_PARA;

                /**
                 * 返回确认包
                 */
                send_ack();

                break;
            /**
             * 复位
             */
            case COMMAND_RESET:
                /**
                 * 返回确认包
                 */
                send_ack();

                // 复位
                reset();

                // 死循环
                while (true) {
                    ;
                }
                break;
            default: // 未知命令
                // 返回确认包
                send_ack();

                // 状态设为未知命令
                status = COMMAND_RET_UNKNOWN_CMD;
                break;
        }
    }
    return OK;
}


/**
 ******************************************************************************
 * @brief      esc_key_detect
 * @param[in]  None
 * @param[out] None
 * @retval     true-成功、false-失败
 *
 * @details    按键检测
 *
 * @note
 ******************************************************************************
 */
LOCAL boolean esc_key_detect(void) {
    uint8 key;
    boolean result = false;

    while(true) {        
        // 如果超过3秒，则结束等待
        if (3 < get_systime())
            break;
        
        // 检测是否已按下"ESC"键        
        if (uart_tryReceive(&key) && (27 == key)) {
            result = true;
            break;
        }
    }
    print("\r\n");

    return (result);
}

/**
 ******************************************************************************
 * @brief      send_ack
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    发送接收确认包,只说明数据包合法并已被处理，而不意味着业务流程上也正确
 *
 * @note
 ******************************************************************************
 */
void send_ack(void) {
    send_packet(PACKET_ACK, ARRAY_SIZE(PACKET_ACK));
}

/**
 ******************************************************************************
 * @brief      send_nak
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    返回接收否认包
 *
 * @note
 ******************************************************************************
 */
void send_nak(void) {
    send_packet(PACKET_NAK, ARRAY_SIZE(PACKET_NAK));
}

/**
 ******************************************************************************
 * @brief      send_hello
 * @param[in]  None
 * @param[out] None
 * @retval     true-已建立、 false-未建立
 *
 * @details    建立通道，过滤多余按键或串口切换产生的错误字节
 *
 * @note
 ******************************************************************************
 */
boolean send_hello(void) {
    uint8 data;

    // 读取帧长度字节
    do {
        uart_receive(&data, 1);
    } while (3 != data); // 如果返回帧长度不为3，则放弃

    // 读取帧校验字节
    uart_receive(&data, 1);
    if (data != 0x20) return (false);

    // 读取帧命令字
    uart_receive(&data, 1);
    if (data != 0x20) return (false);

    // 返回确认包
    send_ack();

    return (true);
}

/**
 ******************************************************************************
 * @brief      receive_packet
 * @param[in]  None
 * @param[out] uint8 *cmd   : 命令字
 * @param[out] uint8 *data  : 接收缓冲
 * @param[out] uint8 *size   : data长度
 * @retval     true-成功、 false-失败
 *
 * @details    接收一个数据包
 *
 * @note
 ******************************************************************************
 */
boolean receive_packet(uint8 *cmd, uint8 *data, uint8 *size) {
    uint8 sum;
    
    // 读取帧长度字节
    do {
        uart_receive(size, 1);
    } while (*size < 3); // 帧长度 >= 3
    
    // 读取帧校验字节
    uart_receive(&sum, 1);

    // 读取命令字
    uart_receive(cmd, 1);

    // 扣除长度、命令字及校验字节，还需读取的字节数
    *size -= 3;

    if (*size > 0)
        uart_receive(data, *size);
    
    // 如果帧校验错误，返回false
    if (!check_sum(*cmd, data, *size, sum))
        return (false);

    return (true);
}

/**
 ******************************************************************************
 * @brief      send_status
 * @param[in]  uint8 s: 状态
 * @param[out] None
 * @retval     发送结果，成功返回true，失败返回false
 *
 * @details    发送状态
 *
 * @note
 ******************************************************************************
 */
boolean send_status(uint8 s) {
    uint8 data;

    // 发送长度字节
    data = 3;
    uart_send(data);
    
    // 发送校验字节
    uart_send(s);

    // 发送状态字节
    uart_send(s);

    // 等待返回一个非0字节
    do {
        uart_receive((uint8 *)&data, 1);
    } while (0 == data);

    // 如果未返回ACK，则认为发送失败
    if (data == COMMAND_ACK)
        return (true);
    else return (false);
}

/**
 ******************************************************************************
 * @brief      send_packet
 * @param[in]  uint8 s: 状态
 * @param[out] None
 * @retval     true-成功、 false-失败
 *
 * @details	        发送数据
 *
 * @note
 ******************************************************************************
 */
LOCAL void send_packet(const uint8 *packet, uint16 size) {
	uint16 i;
    for (i = 0; i < size; i++) {
        uart_send(packet[i]);
    }
}

/**
 ******************************************************************************
 * @brief      check_sum
 * @param[in]  const uint8 cmd : 命令字
 * @param[in]  const uint8 *data : 数据指针
 * @param[in]  const uint8 size : 数据长度
 * @param[in]  const uint8 sum : 校验值
 *
 * @param[out] None
 * @retval     true-正确、flase-错误
 *
 * @details	        检查校验值
 *
 * @note
 ******************************************************************************
 */
LOCAL boolean check_sum(const uint8 cmd, const uint8 *data,
    const uint8 size, const uint8 sum) {
    uint8 s = cmd;
    uint8 i = size;
    
    if (i > 0) {
        while (i--) s += *data++;
    }

    return ((s & 0xff) == sum ? true : false);
}

/**
 ******************************************************************************
 * @brief      reset
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 *
 * @details	   reset
 *
 * @note将复位请求写入AIRCR(应用程序中断及复位控制寄存器)
 ******************************************************************************
 */
void reset(void) {
    // 0x05FA: VECTKEY访问钥匙，需同时写入
    // 0x0004: 请求芯片产生一次复位
    *((uint32 *)0xE000ED0C) = 0x05FA0004;
    return;
}


