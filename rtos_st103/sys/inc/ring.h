//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//��Ȩ���� (C), 2004-2009,   lst.
//����ģ��: ������
//���ߣ�lst
//�汾��V1.0.0
//�ļ�����: �ṩ���λ���������
//����˵��:
//�޶���ʷ:
//2. ...
//1. ����: 2009-01-04
//   ����: lst
//   �°汾��: V1.0.0
//   �޸�˵��: ԭʼ�汾
//------------------------------------------------------
#ifndef __ring_h__
#define __ring_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct ring_buf
{
    uint32_t    len;            //�������е��ֽ���
    uint32_t    offset_read;    //��������ָ��,ָ����һ�ζ���λ��
    uint32_t    offset_write;   //������дָ��,ָ����һ��д��λ��
    uint32_t    max_len;        //��������󳤶�,Ԫ�ظ���.
    uint8_t     *buf;           //������ָ��,�û��Լ���֤�����ٵĻ������Ƿ����趨
                                //����һ��,djyos�������.
};

void ring_init(struct ring_buf *ring, uint8_t *buf, uint32_t len);
uint32_t ring_capacity(struct ring_buf *ring);
uint8_t *ring_get_buf(struct ring_buf *ring);
uint32_t    ring_write(struct ring_buf *ring,uint8_t *buffer,uint32_t len);
uint32_t    ring_read(struct ring_buf *ring,uint8_t *buffer,uint32_t len);
uint32_t    ring_check(struct ring_buf *ring);
bool_e   ring_if_empty(struct ring_buf *ring);
bool_e   ring_if_full(struct ring_buf *ring);
void    ring_flush(struct ring_buf *ring);
uint32_t ring_dumb_read(struct ring_buf *ring,uint32_t len);
uint32_t ring_recede_read(struct ring_buf *ring,uint32_t len);
uint32_t ring_skip_tail(struct ring_buf *ring,uint32_t size);
uint32_t ring_search_ch(struct ring_buf *ring, char c);
uint32_t ring_search_str(struct ring_buf *ring, char *string,uint32_t str_len);

#ifdef __cplusplus
}
#endif

#endif //__ring_h__
