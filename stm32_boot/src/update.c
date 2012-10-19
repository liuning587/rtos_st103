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
/* �����붨��  */
/* 0x20 ��������  */
#define COMMAND_PING             0x20
/* 0x21 ��ʼ����  */
#define COMMAND_DOWNLOAD         0x21
/* 0x23 ��ȡ״̬  */
#define COMMAND_GET_STATUS       0x23
/* 0x24 �������� */
#define COMMAND_SEND_DATA        0x24
/*0x25 ��λ */
#define COMMAND_RESET            0x25
/* Ӧ���� */
/* 0xCC ���ճɹ� */
#define COMMAND_ACK              0xCC
/* 0x33 ����ʧ�� */
#define COMMAND_NAK              0x33

/* ״̬�� */
/* 0x40 �ɹ� */
#define COMMAND_RET_SUCCESS      0x40
/* 0x41 δ֪���� */
#define COMMAND_RET_UNKNOWN_CMD  0x41
/* 0x42 �Ƿ����� */
#define COMMAND_RET_INVALID_CMD  0x42
/* 0x43 �Ƿ����� */
#define COMMAND_RET_INVALID_PARA 0x43
/* 0x44 FLASH����ʧ�� */
#define COMMAND_RET_FLASH_FAIL   0x44
/* 0x45 ���������� */
#define COMMAND_RET_BUFFER_FAIL  0x45

/* DataFlash��Ϣ */
/* ��Ϣ���ڵ�ַ */
#define UPDATE_CFG_START_ADDR    0x0
/* ��Ϣ��ʶ */
#define UPDATE_FLAG              "s103"

/*------------------------------------------------------------------------------
Section: Private Type Definitions 
------------------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)    //�趨Ϊ1�ֽڶ���

typedef struct {
    uint8  flag[4];             /* if "sxdq", Update sucess */
    uint32 addr;                /* ���ݴ洢��ַ */
    uint32 len;                 /* �������ݳ��� */
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
/* ȷ�ϰ� */
LOCAL const uint8 PACKET_ACK[] = {0, 0, COMMAND_ACK};
/* ���ϰ� */
LOCAL const uint8 PACKET_NAK[] = {0, 0, COMMAND_NAK};

/*------------------------------------------------------------------------------
Section: Function Definitions
------------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      df_update
 * @param[in]  None
 * @param[out] None
 * @retval     true-�ɹ���false-ʧ��
 *
 * @details    �Ӵ��ڽ�������
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

    /* ����Ƿ�Ҫ�Ӵ��ڽ������� */
    if (!esc_key_detect())
        return (OK);

    print("Receiving data ...");

    /* ���˶��ఴ���򴮿��л������Ĵ����ֽ� */
    while (!send_hello()) {
    }

    startAddress = 0xffffffff;

    status = COMMAND_RET_SUCCESS;
    while (true) {
        /* ���δ��ȷ���ձ��ģ��������� */
        if (!receive_packet(&cmd, data, &size)) {
            send_nak();
            continue;
        }

        /* ���������ֽ��д��� */
        switch (cmd) {
			/* �������� */
            case COMMAND_PING:
                status = COMMAND_RET_SUCCESS;

                /* ����ȷ�ϰ� */
                send_ack();

                break;

            /* ��ʼ���� */
            case COMMAND_DOWNLOAD:
                status = COMMAND_RET_SUCCESS;
                do {
                    /* ��鱨�ĳ��� */
                    if (size != 8) {
                        status = COMMAND_RET_INVALID_CMD;

                        break;
                    }

                    /**
                     * ��ʼ��ַ
                     */
                    startAddress = (data[0] << 24)
                                 | (data[1] << 16)
                                 | (data[2] <<  8)
                                 |  data[3];
                    /* ��ʼ��ַ����Ϊ0���ɸ�BootLoader���о���д��λ�� */
                    if (startAddress != 0u)
                    {
                        status = COMMAND_RET_INVALID_CMD;
                        break;
                    }
                    startAddress = APP_START_ADDRESS;
                    /**
                     * �����ֽ���
                     */
                    transferSize = (data[4] << 24)
                                 | (data[5] << 16)
                                 | (data[6] <<  8)
                                 |  data[7];

                    /**
                     * ��ʼ��Flash
                     */
                    iflash_init();

                    /**
                     * ������Ҫ���µĿռ�
                     */
                    if (!iflash_erase(startAddress, transferSize))
                        status = COMMAND_RET_FLASH_FAIL;
                } while(false);

                if (status != COMMAND_RET_SUCCESS)
                    transferSize = 0;

                /**
                 * ȷ�ϱ����ѵõ�����
                 */
                send_ack();

                break;

            /**
             * ��ȡ״̬
             */
            case COMMAND_GET_STATUS:
                /**
                 * ����ȷ�ϰ�
                 */
                send_ack();

                /**
                 * ���ص�ǰ״̬
                 */
                send_status(status);

                break;
            /**
             * ����
             */
            case COMMAND_SEND_DATA:
                status = COMMAND_RET_SUCCESS;

                /**
                 * ע��:��̨����Ӧ��֤size���ֶ��룬����д�����޷�Ԥ��
                 * ����Ƿ�������δд��
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
                 * ����ȷ�ϰ�
                 */
                send_ack();

                break;
            /**
             * ��λ
             */
            case COMMAND_RESET:
                /**
                 * ����ȷ�ϰ�
                 */
                send_ack();

                // ��λ
                reset();

                // ��ѭ��
                while (true) {
                    ;
                }
                break;
            default: // δ֪����
                // ����ȷ�ϰ�
                send_ack();

                // ״̬��Ϊδ֪����
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
 * @retval     true-�ɹ���false-ʧ��
 *
 * @details    �������
 *
 * @note
 ******************************************************************************
 */
LOCAL boolean esc_key_detect(void) {
    uint8 key;
    boolean result = false;

    while(true) {        
        // �������3�룬������ȴ�
        if (3 < get_systime())
            break;
        
        // ����Ƿ��Ѱ���"ESC"��        
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
 * @details    ���ͽ���ȷ�ϰ�,ֻ˵�����ݰ��Ϸ����ѱ�����������ζ��ҵ��������Ҳ��ȷ
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
 * @details    ���ؽ��շ��ϰ�
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
 * @retval     true-�ѽ����� false-δ����
 *
 * @details    ����ͨ�������˶��ఴ���򴮿��л������Ĵ����ֽ�
 *
 * @note
 ******************************************************************************
 */
boolean send_hello(void) {
    uint8 data;

    // ��ȡ֡�����ֽ�
    do {
        uart_receive(&data, 1);
    } while (3 != data); // �������֡���Ȳ�Ϊ3�������

    // ��ȡ֡У���ֽ�
    uart_receive(&data, 1);
    if (data != 0x20) return (false);

    // ��ȡ֡������
    uart_receive(&data, 1);
    if (data != 0x20) return (false);

    // ����ȷ�ϰ�
    send_ack();

    return (true);
}

/**
 ******************************************************************************
 * @brief      receive_packet
 * @param[in]  None
 * @param[out] uint8 *cmd   : ������
 * @param[out] uint8 *data  : ���ջ���
 * @param[out] uint8 *size   : data����
 * @retval     true-�ɹ��� false-ʧ��
 *
 * @details    ����һ�����ݰ�
 *
 * @note
 ******************************************************************************
 */
boolean receive_packet(uint8 *cmd, uint8 *data, uint8 *size) {
    uint8 sum;
    
    // ��ȡ֡�����ֽ�
    do {
        uart_receive(size, 1);
    } while (*size < 3); // ֡���� >= 3
    
    // ��ȡ֡У���ֽ�
    uart_receive(&sum, 1);

    // ��ȡ������
    uart_receive(cmd, 1);

    // �۳����ȡ������ּ�У���ֽڣ������ȡ���ֽ���
    *size -= 3;

    if (*size > 0)
        uart_receive(data, *size);
    
    // ���֡У����󣬷���false
    if (!check_sum(*cmd, data, *size, sum))
        return (false);

    return (true);
}

/**
 ******************************************************************************
 * @brief      send_status
 * @param[in]  uint8 s: ״̬
 * @param[out] None
 * @retval     ���ͽ�����ɹ�����true��ʧ�ܷ���false
 *
 * @details    ����״̬
 *
 * @note
 ******************************************************************************
 */
boolean send_status(uint8 s) {
    uint8 data;

    // ���ͳ����ֽ�
    data = 3;
    uart_send(data);
    
    // ����У���ֽ�
    uart_send(s);

    // ����״̬�ֽ�
    uart_send(s);

    // �ȴ�����һ����0�ֽ�
    do {
        uart_receive((uint8 *)&data, 1);
    } while (0 == data);

    // ���δ����ACK������Ϊ����ʧ��
    if (data == COMMAND_ACK)
        return (true);
    else return (false);
}

/**
 ******************************************************************************
 * @brief      send_packet
 * @param[in]  uint8 s: ״̬
 * @param[out] None
 * @retval     true-�ɹ��� false-ʧ��
 *
 * @details	        ��������
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
 * @param[in]  const uint8 cmd : ������
 * @param[in]  const uint8 *data : ����ָ��
 * @param[in]  const uint8 size : ���ݳ���
 * @param[in]  const uint8 sum : У��ֵ
 *
 * @param[out] None
 * @retval     true-��ȷ��flase-����
 *
 * @details	        ���У��ֵ
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
 * @note����λ����д��AIRCR(Ӧ�ó����жϼ���λ���ƼĴ���)
 ******************************************************************************
 */
void reset(void) {
    // 0x05FA: VECTKEY����Կ�ף���ͬʱд��
    // 0x0004: ����оƬ����һ�θ�λ
    *((uint32 *)0xE000ED0C) = 0x05FA0004;
    return;
}


