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
#include <devlib.h>
#if 0
typedef struct
{
    device_t*  next;
    int8_t name[MAX_DEVICE_NAME];   /**< �豸�� */
    fileopt_t fileopt;              /**< �豸�������� */
} device_t;

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

/**
 ******************************************************************************
 * @brief
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
dev_create(int8_t* pdevname, fileopt_t* pfileopt)
{
    if ((pdevname == NULL) || (pfileopt == NULL))
    {
        return ERROR;
    }
}
#endif
