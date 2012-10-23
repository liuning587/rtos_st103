/**
 ******************************************************************************
 * @file       sys_gpio.c
 * @version    V0.0.1
 * @brief      gpioģ��.
 * @details    This file including all API functions's implement of gpio.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <sched.h>
#include <sys_gpio.h>
/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#ifndef IO_LOW
#define IO_LOW 0
#endif

#ifndef IO_HIGH
#define IO_HIGH 1
#endif

/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
static FUNCPTR __bsp_gpio_cfg = NULL;
static FUNCPTR __bsp_gpio_read = NULL;
static FUNCPTR __bsp_gpio_write = NULL;

/*-----------------------------------------------------------------------------
 Section: Local Variable
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      sys_gpio_read - ����IO�������ģʽ
 * @param[in]  int32_t iono  : IO���
 * @param[in]  uint32_t mode : ģʽ
 * @retval     OK    ���óɹ�
 * @retval     ERROR ����ʧ��
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern int32_t
sys_gpio_cfg(int32_t iono, uint32_t mode)
{
    if (IO_NO_SUPPORT == iono) return ERROR;;
    if (NULL != __bsp_gpio_cfg) return __bsp_gpio_cfg(iono, mode);
    return ERROR;

}

/**
 ******************************************************************************
 * @brief      sys_gpio_read - ��ȡָ��IO�ĵ�ǰ״̬
 * @param[in]  int32_t iono  : IO���
 * @param[out] None
 * @retval     int: ���صĵ�ǰIO״̬��1����0����1��ʾ��ǰ������Ч��
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern int32_t sys_gpio_read(int32_t iono)
{
    if (IO_NO_SUPPORT == iono) return ERROR;
    if (NULL != __bsp_gpio_read) return __bsp_gpio_read(iono);
    return ERROR;
}

/**
 ******************************************************************************
 * @brief      sys_gpio_write - ����ָ��IO�ĵ�ǰ״̬
 * @param[in]  int32_t iono     : IO���
 * @param[in]  int32_t iostate  : ״̬
 * @retval     OK    ���óɹ�
 * @retval     ERROR ����ʧ��
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern status_t sys_gpio_write(int32_t iono,int32_t state)
{
    if (IO_NO_SUPPORT == iono) return ERROR;
    if (NULL != __bsp_gpio_write) return __bsp_gpio_write(iono, state);
    return ERROR;
}

/**
 ******************************************************************************
 * @brief      sys_gpio_bspInstall - initialize the bsp
 * @param[in]  FUNCPTR bsp_gpio_cfg
 * @param[in]  FUNCPTR bsp_gpio_read
 * @param[in]  FUNCPTR bsp_gpio_write
 * @retval     status_t: On success, returns OK. A 0 is returned on error.
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern void sys_gpio_bspInstall(
        FUNCPTR bsp_gpio_cfg,
        FUNCPTR bsp_gpio_read,
        FUNCPTR bsp_gpio_write)
{
    __bsp_gpio_cfg = bsp_gpio_cfg;
    __bsp_gpio_read = bsp_gpio_read;
    __bsp_gpio_write = bsp_gpio_write;
}

/*------------------------------End of gpio.c -------------------------------*/
