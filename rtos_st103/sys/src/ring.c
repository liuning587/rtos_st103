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
#include <string.h>
#include <ring.h>

#define cn_limit_uint32     0xffffffff

//----�������λ�����----------------------------------------------------------
//����: �������λ���������ʼ����ʹ���������֮ǰ���û�Ӧ�ö��建�����ڴ���
//      ���������ݽṹ��
//����: ring,Ŀ�껷�λ������ṹָ��
//      buf,��������ʼ��ַ
//      len,����������.��λ���ֽ���
//����: ��
//-----------------------------------------------------------------------------
void ring_init(struct ring_buf *ring, uint8_t *buf, uint32_t len)
{
    ring->buf = buf;
    ring->max_len = len;
    ring->offset_write = 0;
    ring->offset_read  = 0;
    ring->len = 0;
}

//----��黺��������-----------------------------------------------------------
//����: ���ػ���������
//����: ring,Ŀ�����Ի������ṹָ��
//����: ���������������ǵ���ring_initʱʹ�õ�len������
//-----------------------------------------------------------------------------
uint32_t ring_capacity(struct ring_buf *ring)
{
    return ring->max_len;
}

//----�����ֽڳص�ַ-----------------------------------------------------------
//����: ��ѯ���������ֽڳص�ַ�������ַ���û�����ring_initʱʹ�õĻ�������ַ��
//����: ring,Ŀ�����Ի�����ָ��.
//����: ���λ��������ֽڳص�ַ
//˵��: ����������������û�������ַ������
//      1��Ҫ�ͷŻ������ڴ棬Ӧ�ó������ʹ�õ���ring_initʱʹ�õ�ָ�룬�����
//         ָ�붪ʧ�����ñ�����ȡ�ظ�ָ�롣
//      2���û����Լ����ַ��ʸû��������ⲻ��һ�����ǵ�ѡ�����ƻ�ģ�������
//         �����ɣ���ʱ��ʹ����Ӧ����ȫ�����Լ��ڸ�ʲô!
//-----------------------------------------------------------------------------
uint8_t *ring_get_buf(struct ring_buf *ring)
{
    return ring->buf;
}

//----���λ�����д��-----------------------------------------------------------
//����: ���λ�����д�����ɸ��ֽ�,����ʵ��д���������,���ƶ�дָ��,�������
//      ������û���㹻�Ŀռ�,��ʵ��ʣ��ռ�д��
//����: ring,Ŀ�껷�λ������ṹָ��
//      buffer,��д�������ָ��
//      len,��д������ݳ���.��λ���ֽ���
//����: ʵ��д����ֽ���,������������㹻�Ŀռ�,=len
//-----------------------------------------------------------------------------
uint32_t    ring_write(struct ring_buf *ring,uint8_t *buffer,uint32_t len)
{
    uint32_t    wr_len;
    uint32_t    partial;
    wr_len = ring->max_len - ring->len;
    if(wr_len == 0)
        return 0;
    if(wr_len > len)
        wr_len = len;
    if((ring->offset_write + wr_len) > ring->max_len)
    {   //���ݷ�������
        partial = ring->max_len - ring->offset_write;
        memcpy(&ring->buf[ring->offset_write],buffer,partial);    //д��һ����
        memcpy( ring->buf,&buffer[partial],wr_len - partial);    //д�ڶ�����
        ring->offset_write = wr_len - partial;
    }else
    {   //����������
        memcpy( &ring->buf[ring->offset_write],buffer,wr_len);
        ring->offset_write +=wr_len;
    }
    ring->len += wr_len;
    return wr_len;
}

//----�ӻ��λ�������-----------------------------------------------------------
//����: �ӻ��λ������������ɸ��ֽ�,����ʵ�ʶ�����������,�����ƶ���ָ�����
//      �����������ݲ��㣬��ʵ��������ȡ��
//����: ring,Ŀ�껷�λ������ṹָ��
//      buffer,�������ݵĻ�����ָ��
//      len,�����������ݳ���.��λ���ֽ���
//����: ʵ�ʶ������ֽ���,������������㹻������,=len
//------------------------------------------------------------------------------
uint32_t    ring_read(struct ring_buf *ring,uint8_t *buffer,uint32_t len)
{
    uint32_t    wr_len;
    wr_len = (ring->len < len)? ring->len : len;
    if((ring->offset_read + wr_len) > ring->max_len)
    {   //���ݷ�������
        uint32_t  partial;
        partial =ring->max_len - ring->offset_read;
        memcpy( buffer,&ring->buf[ring->offset_read],partial);    //д��һ����
        memcpy( &buffer[partial],ring->buf,wr_len - partial);    //д�ڶ�����
        ring->offset_read = wr_len - partial;
    }else
    {   //����������
        memcpy( buffer,&ring->buf[ring->offset_read],wr_len);
        ring->offset_read += wr_len;
    }
    ring->len -= wr_len;
    return wr_len;
}

//----��黺����������----------------------------------------------------------
//����: ���ָ���Ļ��λ������е�������,�����ֽ���.
//����: ring,Ŀ�껷�λ�����ָ��.
//����: �������е�������
//------------------------------------------------------------------------------
uint32_t    ring_check(struct ring_buf *ring)
{
    return ring->len;
}

//----��黺�����Ƿ��-----------------------------------------------------------
//����: ���ָ���Ļ��λ��������Ƿ��Ѿ���.
//����: ring,Ŀ�껷�λ�����ָ��.
//����: ���򷵻�TRUE,�ǿշ���FALSE
//------------------------------------------------------------------------------
bool_e   ring_if_empty(struct ring_buf *ring)
{
    return (ring->len ==0)? TRUE:FALSE;
}

//----��黺�����Ƿ���-----------------------------------------------------------
//����: ���ָ���Ļ��λ��������Ƿ��Ѿ���.
//����: ring,Ŀ�껷�λ�����ָ��.
//����: ���򷵻�TRUE,��������FALSE
//------------------------------------------------------------------------------
bool_e   ring_if_full(struct ring_buf *ring)
{
    return (ring->len == ring->max_len) ? TRUE:FALSE;
}

//----��ջ��λ�����-------------------------------------------------------------
//����: �������������������
//����: ring,Ŀ�껷�λ�����ָ��.
//����: ��
//------------------------------------------------------------------------------
void    ring_flush(struct ring_buf *ring)
{
    ring->len = 0;
    ring->offset_write = 0;
    ring->offset_read = 0;
}

//----�ͷ���������---------------------------------------------------------------
//����: �Ӷ�ָ�뿪ʼ,�ͷŵ�ָ����С������,�൱���ƶ���len���ֽ�
//����: ring,Ŀ�껷�λ�����ָ��.
//      len,�ͷŵ���������
//����: ʵ���ͷŵ�������
//------------------------------------------------------------------------------
uint32_t ring_dumb_read(struct ring_buf *ring,uint32_t len)
{
    uint32_t    result;
    result = (ring->len < len)? ring->len : len;
    if((ring->offset_read + result) > ring->max_len)
    {   //���ݷ�������
        ring->offset_read = result + ring->offset_read - ring->max_len;
    }else
    {   //����������
        ring->offset_read += result;
    }
    ring->len -= result;
    return result;
}

//----�˻���������-------------------------------------------------------------
//����: ��������ring_dumb_read���������෴���ѻ�����ָ���˻�len�ֽڣ�����˻ص�
//      ���ȳ����������Ŀ��г��ȣ���ȡ���������г��ȡ��൱�ڰѻ��������Ѿ�����
//      �����ݷ��ػ�����������û�ж��������ӡ�ringģ�鲢��У���˻صĲ����Ƿ����
//      ԭ�������ݡ�
//����: ring,Ŀ�껷�λ�����ָ��.
//      len,�˻ص���������
//����: ʵ���˻ص�������
//-----------------------------------------------------------------------------
uint32_t ring_recede_read(struct ring_buf *ring,uint32_t len)
{
    uint32_t    result;
    if((ring->max_len - ring->len) > len)   //���г��ȴ������˻صĳ���
        result = len;
    else
        result = ring->max_len - ring->len;
    if(ring->offset_read < result)
    {   //���ݷ�������
        ring->offset_read = ring->max_len - (result - ring->offset_read);
    }else
    {   //����������
        ring->offset_read -= result;
    }
    ring->len += result;
    return result;
}

//----ȡ����������-------------------------------------------------------------
//����: ȡ���Ѿ�д�����Ի��������������ݣ��������û��д��һ����
//����: ring,Ŀ�껷�λ�����ָ��.
//      len,�˻ص���������
//����: ʵ���˻ص�������
//-----------------------------------------------------------------------------
uint32_t ring_skip_tail(struct ring_buf *ring,uint32_t size)
{
    uint32_t result;
    if(ring->len > size)
    {
        result = size;
        if(ring->offset_write < result)
        {   //���ݷ�������
            ring->offset_write = ring->max_len - (result - ring->offset_write);
        }else
        {   //����������
            ring->offset_read -= result;
        }
        ring->len -=result;
    }else
    {
        result = ring->len;
        ring_flush(ring);
    }
    return result;
}

//----�����ַ�------------------------------------------------------------------
//����: ��ring��ǰ��λ�ÿ�ʼ�����ַ�c��λ��
//����: ring,Ŀ�껷�λ�����ָ��
//      c,����ҵ��ַ�
//����: c���ֵ�λ��,���û�г����򷵻� cn_limit_uint32
//------------------------------------------------------------------------------
uint32_t ring_search_ch(struct ring_buf *ring, char c)
{
    uint32_t    i;
    uint8_t *buf = ring->buf;
    if(ring->offset_read > ring->offset_write)
    {   //�������л���
        for(i=ring->offset_read;i<ring->max_len;i++)
        {
            if(buf[i] == c)
                return (i - ring->offset_read);
        }
        for(i=0;i<ring->offset_write;i++)
            if(buf[i] == c)
                return (i - ring->offset_read + ring->max_len);
    }else
    {   //������û�л���
        for(i = ring->offset_read; i < ring->offset_write; i++)
            if(buf[i] == c)
                return (i - ring->offset_read);
    }
    return cn_limit_uint32;
}

//----�����ַ�����--------------------------------------------------------------
//����: ��ring��ǰ��λ�ÿ�ʼ�����ַ����е�λ��,�ַ����в���0����,����ָ������
//����: ring,Ŀ�껷�λ�����ָ��
//      string,����ҵ��ַ�����
//      str_len,�ַ����г���
//����: string���ֵ�λ�����offset_read��ƫ����,���û�г��ַ��� cn_limit_uint32
//��ע: ������ܿ��ܱȽϳ���,�����ڱ�дʱע�����ٶ��Ż�,��ȴʹ����������.
//------------------------------------------------------------------------------
uint32_t ring_search_str(struct ring_buf *ring, char *string,uint32_t str_len)
{
    uint32_t i,j;
    bool_e next;
    uint8_t *buf;
    uint32_t  end,start;

    if(ring->len < str_len)
        return cn_limit_uint32;
    buf = ring->buf;
    if(ring->offset_read <= ring->offset_write)
    {   //������û�л���
        for(i=ring->offset_read;i <= ring->offset_write - str_len;i++)
        {
            next = FALSE;
            for(j=0;j < str_len;j++)
            {
                if(buf[i+j] != string[j])
                    next = TRUE;
            }
            if(next == FALSE)
            {
                return (i - ring->offset_read);
            }
        }
    }else
    {   //�������л���
        //�ȴ������Ʋ���,��������������ĩ��ϲ���
        for(i=ring->offset_read;i <= (ring->max_len - str_len);i++)
        {
            next = FALSE;
            for(j=0;j < str_len;j++)
            {
                if(buf[i+j] != string[j])
                    next = TRUE;    //������ֲ���,���ʾҪ������һ��ѭ��
            }
            if(next == FALSE)
            {   //��ʾ��һ��ѭ�����Ѿ�����ƥ��Ĵ�,
                return (i - ring->offset_read);
            }
        }
        //�ٴ���������ĩ��ϲ����Լ����Ʋ���
        if(ring->offset_write >= str_len)
        {   //�������л��Ʋ��ֵĳ��ȳ������ҳ���,ĩ�˺��׶˷ֱ����
            //�Ȳ��һ�����ĩ�˲���
            for(;i < ring->max_len;i++)
            {
                next = FALSE;
                //string�ֳ�������,end���ַ��ڻ�����ĩ��,start���ַ��ڻ�������
                end = ring->max_len - i;
                start = str_len - end;
                for(j=0;j<end;j++)
                {   //�ȱȽϻ�����ĩ�˲���
                    if(buf[i+j] != string[j])
                        next = TRUE;
                }
                if(next == FALSE)
                {   //������ĩ�˲���ȫ��ƥ��,��Ҫ���׶��Ƿ�Ҳƥ��
                    for(j=0;j<start;j++)
                        if(buf[j] != string[start+j])
                            next = TRUE;
                }
                if(next == FALSE)
                {   //��ʾ��һ��ѭ�����Ѿ�����ƥ��Ĵ�,
                    return (i - ring->offset_read);
                }
            }
            //�ٲ����׶˲���
            for(i=0;i < (ring->offset_write - str_len);i++)
            {
                next = FALSE;
                for(j=0;j < str_len;j++)
                {
                    if(buf[i+j] != string[j])
                        next = TRUE;    //������ֲ���,���ʾҪ������һ��ѭ��
                }
                if(next == FALSE)
                {   //��ʾ��һ��ѭ�����Ѿ�����ƥ��Ĵ�,
                    return (i - ring->offset_read);
                }
            }
        }else
        {   //�������л��Ʋ��ֳ���û�г���str_len,ֻ��Ҫ����ĩ�˼���.
            next = FALSE;
            for(;i < (ring->offset_read + ring->len - str_len);i++)
            {
                uint32_t  end,start;
                next = FALSE;
                //string�ֳ�������,end���ַ��ڻ�����ĩ��,start���ַ��ڻ�������
                end = ring->max_len - i;
                start = str_len - end;
                for(j=0;j<end;j++)
                {   //�ȱȽϻ�����ĩ�˲���
                    if(buf[i+j] != string[j])
                        next = TRUE;
                }
                if(next == FALSE)
                {   //������ĩ�˲���ȫ��ƥ��,��Ҫ���׶��Ƿ�Ҳƥ��
                    for(j=0;j<start;j++)
                        if(buf[j] != string[start+j])
                            next = TRUE;
                }
                if(next == FALSE)
                {   //��ʾ��һ��ѭ�����Ѿ�����ƥ��Ĵ�,
                    return (i - ring->offset_read);
                }
            }
        }
    }
    return cn_limit_uint32;
}
