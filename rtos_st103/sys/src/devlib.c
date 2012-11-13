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
#include <devlib.h>
#if 0
typedef struct
{
    device_t*  next;
    int8_t name[MAX_DEVICE_NAME];   /**< 设备名 */
    fileopt_t fileopt;              /**< 设备操作方法 */
} device_t;

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
