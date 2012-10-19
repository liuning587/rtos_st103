/**
 ******************************************************************************
 * @file       main.c
 * @version    V0.0.1
 * @brief      API C source file of main.
 * @details    ����������.
 * @copy       Copyrigth(C), 2012-2015.
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <lib.h>
#include <update.h>
#include <bspitf.h>
#include <cfg.h>

/*----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* �ж�����������ջ����ַ��ISR��ַ */
typedef union
{
    VOIDFUNCPTR routine;  /* �жϴ�������ַ */
    void* p_msp;          /* ջ����ַ */
} Intvect_item_t;
typedef void (*Function)(void);

IMPORT uint32 the_bss;
IMPORT uint32 the_ebss;
IMPORT uint32 the_sidata;
IMPORT uint32 the_data;
IMPORT uint32 the_edata;
extern void reset(void);
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
void jump_to_app(void);
void set_msp(uint32 topOfMainStack);

/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
LOCAL uint32 the_cstack[STACK_SIZE];

/* �ж������� */
__attribute__((section(".isr_vector")))
    const Intvect_item_t the_vector_table[] =
{
    {.p_msp = (uint32)the_cstack + sizeof(the_cstack) }, /* ջ��ָ�� */
    reset_routine, /* ��λ�ж�  */
    nmi_routine,   /* NMI�ж� */
    fault_routine, /* Ӳ������ */
    mem_fault_routine, /* �洢��������� */
    bus_fault_routine, /* ���ߴ��� */
    usage_fault_routine, /* �÷����� */
    dummy, /* ���� */
    dummy, /* ���� */
    dummy, /* ���� */
    dummy, /* ���� */
    dummy, /* ���� */
    dummy, /* ���� */
    dummy, /* ���� */
    dummy, /* ���� */
    systick_routine, /* ϵͳ��ʱ�� */
};
/*-----------------------------------------------------------------------------
 Section: Private Variables
 ----------------------------------------------------------------------------*/
LOCAL volatile uint32 the_run_time = 0u; /* ���������ۼ�����ʱ��(��) */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      main
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    ������
 *
 * @note
 ******************************************************************************
 */
void
boot_main(void)
{
    /* ��ʼ���豸 */
    hw_init();
    print("\033[H\033[J\r\n");
    print("*********************************************\r\n");
    print("*             GNU BootLoader v1.0           *\r\n");
    print("*           Copyright 2012 LiuNing.         *\r\n");
    print("*********************************************\r\n");

    /* ������ʱ�� */
    systick_open();

    /* ����ͨ���������� */
    if (OK != uart_update())
    {
        return;
    }

    /* �رն�ʱ�� */
    systick_stop();

    /* ��ת��Ӧ��ϵͳ */
    print("Enter application ...\r\n");
    // ����ջ��ָ��
    set_msp(*(volatile uint32 *) APP_START_ADDRESS);

    // ��ת
    jump_to_app();

    while (1)
    {
    }
}

/**
 ******************************************************************************
 * @brief      jump_to_app
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    ��ת��Ӧ��
 *
 * @note
 ******************************************************************************
 */
void
jump_to_app(void)
{
    __asm(
        "movw    r0, (0x08001000 & 0xffff)\n"
        "movt    r0, (0x08001000 >> 16)\n"
        "ldr     r0, [r0, 4]\n"
        "bx      r0"
    );
}

/**
 ******************************************************************************
 * @brief      set_msp
 * @param[in]  uint32 topOfMainStack: ջ��ָ��
 * @param[out] None
 * @retval     None
 *
 * @details    ���ö�ջָ��
 *
 * @note
 ******************************************************************************
 */
void
set_msp(uint32 topOfMainStack)
{
    __asm("msr msp, r0");
    __asm("bx lr");
}

/**
 ******************************************************************************
 * @brief      systick_routine
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    ��ʱ���жϴ���
 *
 * @note
 ******************************************************************************
 */
void
systick_routine(void)
{
    static uint32 ms = 0u; /* ������ */

    /* ÿ�����ж�һ�� */
    ms++;
    if (ms < 1000)
    {
        return;
    }
    ms = 0;

    /* ���������������ʱ��(10����)������ι����ʹ���Զ���λ */
    if (the_run_time >= (10u * 60u))
    {
        /* reboot */
        //print("reboot!\r\n");
        reset();
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
 * @details    �����ж����
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
 * @details    ����bss��
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
 * @details    ��λ�ж����
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
    boot_main();
}

/**
 ******************************************************************************
 * @brief      nmi_routine
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    NMI�ж����
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
 * @details    Ӳ��������
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
 * @details    �洢�����������
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
 * @details    ���ߴ�����
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
 * @details    �÷�������
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
