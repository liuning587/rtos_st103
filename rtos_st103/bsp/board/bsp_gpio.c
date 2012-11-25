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
    {IO_LED0,     IO_OUTPUT1, GPIOA_BASE, GPIO_Pin_0},
    {IO_LED1,     IO_OUTPUT1, GPIOA_BASE, GPIO_Pin_1},
    {IO_LED2,     IO_OUTPUT1, GPIOB_BASE, GPIO_Pin_8},
    {IO_LED3,     IO_OUTPUT1, GPIOB_BASE, GPIO_Pin_9},
    {IO_KEY0,     IO_INPUT,   GPIOB_BASE, GPIO_Pin_0},
    {IO_KEY1,     IO_INPUT,   GPIOB_BASE, GPIO_Pin_1},
    {IO_KEY2,     IO_INPUT,   GPIOA_BASE, GPIO_Pin_8},
    {IO_KEY3,     IO_INPUT,   GPIOA_BASE, GPIO_Pin_13},
    {IO_LCD_AK,   IO_OUTPUT1, GPIOB_BASE, GPIO_Pin_5},
    {IO_LCD_A0,   IO_OUTPUT1, GPIOB_BASE, GPIO_Pin_6},
    {IO_LCD_RST,  IO_OUTPUT1, GPIOB_BASE, GPIO_Pin_7},
    {IO_LCD_CS,   IO_OUTPUT1, GPIOC_BASE, GPIO_Pin_1},
    {IO_LCD_CLK,  IO_OUTPUT1, GPIOC_BASE, GPIO_Pin_2},
    {IO_LCD_MOSI, IO_OUTPUT1, GPIOC_BASE, GPIO_Pin_3},
    {IO_FLASH_CS, IO_OUTPUT1, GPIOA_BASE, GPIO_Pin_4},
    {IO_SD_CS,    IO_OUTPUT1, GPIOC_BASE, GPIO_Pin_4},
};
static status_t
bsp_gpio_cfg(int32_t io_no, int32_t value)
{
    uint32_t i = 0;
    GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    for(i = 0; i < ARRAY_SIZE(the_inout_iomap); i++)
    {
        if(io_no == the_inout_iomap[i].iono)
        {
            switch (value)
            {
                case IO_INPUT:
                	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
                    break;
                case IO_OUTPUT0:
                	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
                    break;
                case IO_OUTPUT1:
                	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//开漏输出
                    break;
                default:
                    return ERROR;
            }
            GPIO_InitStructure.GPIO_Pin = the_inout_iomap[i].pinno;
            GPIO_Init((GPIO_TypeDef*)(the_inout_iomap[i].gpiobase), &GPIO_InitStructure);
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
            	GPIO_SetBits((GPIO_TypeDef *) (the_inout_iomap[i].gpiobase),\
            			the_inout_iomap[i].pinno);
            }
            else
            {
            	GPIO_ResetBits((GPIO_TypeDef *) (the_inout_iomap[i].gpiobase),\
            			the_inout_iomap[i].pinno);
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
            return (status_t)GPIO_ReadInputDataBit((GPIO_TypeDef *)(the_inout_iomap[i]).gpiobase, the_inout_iomap[i].pinno);
        }
    }

    return ERROR;
}

void
bsp_gpio_init(void)
{

    int32_t i;

    GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* Enable GPIOA, GPIOB and AFIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO, ENABLE);
    for (i = 0; i < ARRAY_SIZE(the_inout_iomap); i++)
    {
        if (the_inout_iomap[i].mode == IO_INPUT)
        {
        	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
        }
        else
        {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
        }

        GPIO_InitStructure.GPIO_Pin = the_inout_iomap[i].pinno;
        GPIO_Init((GPIO_TypeDef*)(the_inout_iomap[i].gpiobase), &GPIO_InitStructure);
    }

    sys_gpio_bspInstall(bsp_gpio_cfg, bsp_gpio_read, bsp_gpio_write);

}
