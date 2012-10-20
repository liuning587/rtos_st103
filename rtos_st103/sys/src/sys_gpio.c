/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <sys_gpio.h>
#include <sched.h>
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
/******************************************************************************
*
* sys_gpio_read - ����IO�������ģʽ
*
* INPUTS:
*  int32_t f_iIoNo : IO���
*
* RETURNS:
*  int: ���صĵ�ǰIO״̬��1����0����1��ʾ��ǰ������Ч��d
*
******************************************************************************/
extern int32_t sys_gpio_cfg(int32_t f_iIoNo, uint32_t f_imode)
{
    if (IO_NO_SUPPORT == f_iIoNo) return ERROR;;
    if (NULL != __bsp_gpio_cfg) return __bsp_gpio_cfg(f_iIoNo, f_imode);
    return ERROR;

}

/******************************************************************************
*
* sys_gpio_read - ��ȡָ��IO�ĵ�ǰ״̬
*
* INPUTS:
*  int32_t f_iIoNo : IO���
*
* RETURNS:
*  int: ���صĵ�ǰIO״̬��1����0����1��ʾ��ǰ������Ч��
*
******************************************************************************/
extern int32_t sys_gpio_read(int32_t f_iIoNo)
{
    if (IO_NO_SUPPORT == f_iIoNo) return ERROR;
    if (NULL != __bsp_gpio_read) return __bsp_gpio_read(f_iIoNo);
    return ERROR;
}

/******************************************************************************
*
* sys_gpio_write - ����ָ��IO�ĵ�ǰ״̬
*
* INPUTS:
*   int32_t f_iIoNo:   IO���
*   int32_t f_IoState:    ��Ҫ���õ�IO״̬
* RETURNS:
*  status_t: ָʾ�Ƿ����óɹ�
*
******************************************************************************/
extern status_t sys_gpio_write(int32_t f_iIoNo,int32_t f_IoState)
{
    if (IO_NO_SUPPORT == f_iIoNo) return ERROR;
    if (NULL != __bsp_gpio_write) return __bsp_gpio_write(f_iIoNo, f_IoState);
    return ERROR;
}

/******************************************************************************
*
* sys_gpio_bspInstall - initialize the bsp .
*
* INPUTS:
*   void.
*
* RETURNS:
*   status_t: On success, returns OK. A 0 is returned on error.
*
******************************************************************************/
extern void sys_gpio_bspInstall(
        FUNCPTR bsp_gpio_cfg,
        FUNCPTR bsp_gpio_read,
        FUNCPTR bsp_gpio_write)
{
    __bsp_gpio_cfg = bsp_gpio_cfg;
    __bsp_gpio_read = bsp_gpio_read;
    __bsp_gpio_write = bsp_gpio_write;
}


/*******************************End of gpio.c ********************************/
