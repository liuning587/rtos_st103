#include <maths.h>
#include <sys_gpio.h>
#include <stm32f1lib.h>

/*IO_MAP的定义*/
typedef struct
{
    uint8_t  iono;
    uint8_t  mode;
    uint32_t gpiobase;
    uint32_t pinno;
} io_map_t;

/*输入IO的对应关系表*/
static const io_map_t the_inout_iomap[] =
{
    {IO_LED0, IO_OUTPUT1, GPIOD_BASE, GPIO_Pin_0},
    {IO_LED1, IO_OUTPUT1, GPIOD_BASE, GPIO_Pin_0},
    {IO_LED2, IO_OUTPUT1, GPIOD_BASE, GPIO_Pin_0},
    {IO_LED3, IO_OUTPUT1, GPIOD_BASE, GPIO_Pin_0},
    {IO_LED4, IO_OUTPUT1, GPIOD_BASE, GPIO_Pin_0},
    {IO_LED5, IO_OUTPUT1, GPIOD_BASE, GPIO_Pin_0},
    {IO_LED6, IO_OUTPUT1, GPIOD_BASE, GPIO_Pin_0},
};
static status_t
bsp_gpio_cfg(int32_t io_no, int32_t value)
{
    uint32_t i = 0;

    for(i = 0; i < ARRAY_SIZE(the_inout_iomap); i++)
    {
        if(io_no == the_inout_iomap[i].iono)
        {
            switch (value)
            {
                case IO_INPUT:

                    break;
                case IO_OUTPUT0:


                    break;
                case IO_OUTPUT1:


                    break;
                default:
                    return ERROR;
            }
        }
    }

    return ERROR;
}

static status_t
bsp_gpio_write(int32_t io_no, int32_t value)
{
    uint32_t i = 0;

    for(i = 0; i < ARRAY_SIZE(the_inout_iomap); i++)
    {
        if(io_no == the_inout_iomap[i].iono)
        {
            if(value == 1)
            {

            }
            else
            {

            }
            return  OK;
        }
    }

    return ERROR;
}
static status_t
bsp_gpio_read (int32_t io_no)
{
    uint32_t i = 0;

    for(i = 0; i < sizeof(the_inout_iomap)/sizeof(io_map_t); i++)
    {
        if(io_no == the_inout_iomap[i].iono)
        {
            return (status_t)GPIOPinRead(the_inout_iomap[i].gpiobase, the_inout_iomap[i].pinno);
        }
    }

    return ERROR;
}
static void
bsp_gpio_init(void)
{
    int32_t i;

    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    for (i = 0; i < ARRAY_SIZE(the_inout_iomap); i++)
    {
        if (the_inout_iomap[i].mode == IO_INPUT)
        {
            //GPIODirModeSet(the_inout_iomap[i].gpiobase, the_inout_iomap[i].pinno, GPIO_DIR_MODE_IN);
        }
        else
        {
            //GPIODirModeSet(the_inout_iomap[i].gpiobase, the_inout_iomap[i].pinno, GPIO_DIR_MODE_OUT);
            //GPIOPadConfigSet(the_inout_iomap[i].gpiobase, the_inout_iomap[i].pinno, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        }
    }

    sys_gpio_bspInstall(bsp_gpio_cfg, bsp_gpio_read, bsp_gpio_write);

}
