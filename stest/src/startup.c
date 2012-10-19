/**
 ******************************************************************************
 * @file       main.c
 * @version    V0.0.1
 * @brief      API C source file of main.
 * @details    升级主程序.
 * @copy       Copyrigth(C), 2012-2015.
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>

/*----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* 中断向量表项，存放栈顶地址及ISR地址 */
typedef union
{
    VOIDFUNCPTR routine;  /* 中断处理函数地址 */
    void* p_msp;          /* 栈顶地址 */
} Intvect_item_t;
typedef void (*Function)(void);

IMPORT uint32 the_bss;
IMPORT uint32 the_ebss;
IMPORT uint32 the_sidata;
IMPORT uint32 the_data;
IMPORT uint32 the_edata;
/*-----------------------------------------------------------------------------
 Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
void reset_routine(void);
void nmi_routine(void);
void fault_routine(void);
void mem_fault_routine(void);
void bus_fault_routine(void);
void usage_fault_routine(void);
void dummy(void);
void systick_routine(void);

/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
/* 栈大小(单位为字) */
#define STACK_SIZE            1024
LOCAL uint32 the_cstack[STACK_SIZE];

/* 中断向量表 */
__attribute__((section(".isr_vector")))
    const Intvect_item_t the_vector_table[] =
{
    {.p_msp = (uint32)the_cstack + sizeof(the_cstack) }, /* 栈顶指针 */
    reset_routine, /* 复位中断  */
    nmi_routine,   /* NMI中断 */
    fault_routine, /* 硬件错误 */
    mem_fault_routine, /* 存储器管理错误 */
    bus_fault_routine, /* 总线错误 */
    usage_fault_routine, /* 用法错误 */
    dummy, /* 空闲 */
    dummy, /* 空闲 */
    dummy, /* 空闲 */
    dummy, /* 空闲 */
    dummy, /* 空闲 */
    dummy, /* 空闲 */
    dummy, /* 空闲 */
    dummy, /* 空闲 */
    systick_routine, /* 系统定时器 */
};
/*-----------------------------------------------------------------------------
 Section: Private Variables
 ----------------------------------------------------------------------------*/
LOCAL volatile uint32 the_run_time = 0u; /* 升级程序累计运行时间(秒) */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/




/**
 ******************************************************************************
 * @brief      systick_routine
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    定时器中断处理
 *
 * @note
 ******************************************************************************
 */
void
systick_routine(void)
{
    static uint32 ms = 0u; /* 毫秒数 */

    /* 每毫秒中断一次 */
    ms++;
    if (ms < 1000)
    {
        return;
    }
    ms = 0;

    /* 如果超过允许运行时间(10分钟)，则不再喂狗，使其自动复位 */
    if (the_run_time >= (10u * 60u))
    {
        /* reboot */
        //print("reboot!\r\n");
        return;
    }

    the_run_time++;

}

/**
 ******************************************************************************
 * @brief      dummy
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    空闲中断入口
 *
 * @note
 ******************************************************************************
 */
void
dummy(void)
{
    return;
}

/**
 ******************************************************************************
 * @brief      zero_bss
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    清零bss段
 *
 * @note
 ******************************************************************************
 */
void
zero_bss(void)
{
    __asm("    ldr     r0, =the_bss\n"
          "    ldr     r1, =the_ebss\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "    zero_loop:\n"
          "    cmp     r0, r1\n"
          "    it      lt\n"
          "    strlt   r2, [r0], #4\n"
          "    blt     zero_loop");
}

/**
 ******************************************************************************
 * @brief      reset_routine
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    复位中断入口
 *
 * @note
 ******************************************************************************
 */
void
reset_routine(void)
{
    uint32* pul_src;
    uint32* pul_dest;

    /* Copy the data segment initializers from flash to SRAM. */
    pul_src = &the_sidata;
    pul_dest = &the_data;
    while (pul_dest < &the_edata)
    {
        *pul_dest++ = *pul_src++;
    }

    /* Zero fill the bss segment. */
    zero_bss();

    /* Call the application's entry point. */
extern void app_main(void);
    app_main();
}

/**
 ******************************************************************************
 * @brief      nmi_routine
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    NMI中断入口
 *
 * @note
 ******************************************************************************
 */
void
nmi_routine(void) {
    while (true) {
    }
}

/**
 ******************************************************************************
 * @brief      fault_routine
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    硬件错误处理
 *
 * @note
 ******************************************************************************
 */
void
fault_routine(void)
{
    while (true)
    {
    }
}

/**
 ******************************************************************************
 * @brief      mem_fault_routine
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    存储器管理错误处理
 *
 * @note
 ******************************************************************************
 */
void
mem_fault_routine(void)
{
    while (true)
    {
    }
}

/**
 ******************************************************************************
 * @brief      bus_fault_routine
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    总线错误处理
 *
 * @note
 ******************************************************************************
 */
void
bus_fault_routine(void)
{
    while (true)
    {
    }
}

/**
 ******************************************************************************
 * @brief      usage_fault_routine
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    用法错误处理
 *
 * @note
 ******************************************************************************
 */
void
usage_fault_routine(void)
{
    while (true)
    {
    }
}

extern uint32
get_systime(void)
{
    return the_run_time;
}

/*--------------------------------End of main.c------------------------------*/
