/**
 ******************************************************************************
 * @file       intlib.c
 * @version    V0.0.1
 * @brief      CM3�жϴ���.
 * @details    This file including all API functions's implement of intlib.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <intlib.h>
#include <ucos_ii.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define ROUTINE(n) routine##n 

/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
extern unsigned long _sidata;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
extern void  EXC_ENTER_HARD_FAULT(void);
extern void  EXC_ENTER_MEM_FAULT(void);
extern void  EXC_ENTER_BUS_FAULT(void);
extern void  EXC_ENTER_USAGE_FAULT(void);
extern int32_t main(void);

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* None */

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
typedef struct IntRtn
{
    VOIDFUNCPTR routine;    /**< interrupt handler */
    int32_t     parameter;  /**< parameter of the handler */
} INT_RTN;

/** �ж�����������ջ����ַ��ISR��ַ */
typedef union
{
    VOIDFUNCPTR routine;     /**< �жϴ�������ַ */
    void *msp;               /**< ջ����ַ         */
} INTVECT_ITEM;
/*---------------------------------------------------------------------------*/
/* FOR INTLIB */
static INT_RTN intRtnTbl[INT_COUNT - 15];              /* �ж�ʵ�ֺ�����ڱ� */

/*-----------------------------------------------------------------------------
 Section: Local Variable
 ----------------------------------------------------------------------------*/
static void NMIRoutine(void);
static void dummy(void);
static void intHandler (uint32_t int_num);

/* �����ж� */
void	routine16   (void)	{ intHandler(	16	); }
void	routine17	(void)	{ intHandler(	17	); }
void	routine18	(void)	{ intHandler(	18	); }
void	routine19	(void)	{ intHandler(	19	); }
void	routine20	(void)	{ intHandler(	20	); }
void	routine21	(void)	{ intHandler(	21	); }
void	routine22	(void)	{ intHandler(	22	); }
void	routine23	(void)	{ intHandler(	23	); }
void	routine24	(void)	{ intHandler(	24	); }
void	routine25	(void)	{ intHandler(	25	); }
void	routine26	(void)	{ intHandler(	26	); }
void	routine27	(void)	{ intHandler(	27	); }
void	routine28	(void)	{ intHandler(	28	); }
void	routine29	(void)	{ intHandler(	29	); }
void	routine30	(void)	{ intHandler(	30	); }
void	routine31	(void)	{ intHandler(	31	); }
void	routine32	(void)	{ intHandler(	32	); }
void	routine33	(void)	{ intHandler(	33	); }
void	routine34	(void)	{ intHandler(	34	); }
void	routine35	(void)	{ intHandler(	35	); }
void	routine36	(void)	{ intHandler(	36	); }
void	routine37	(void)	{ intHandler(	37	); }
void	routine38	(void)	{ intHandler(	38	); }
void	routine39	(void)	{ intHandler(	39	); }
void	routine40	(void)	{ intHandler(	40	); }
void	routine41	(void)	{ intHandler(	41	); }
void	routine42	(void)	{ intHandler(	42	); }
void	routine43	(void)	{ intHandler(	43	); }
void	routine44	(void)	{ intHandler(	44	); }
void	routine45	(void)	{ intHandler(	45	); }
void	routine46	(void)	{ intHandler(	46	); }
void	routine47	(void)	{ intHandler(	47	); }
void	routine48	(void)	{ intHandler(	48	); }
void	routine49	(void)	{ intHandler(	49	); }
void	routine50	(void)	{ intHandler(	50	); }
void	routine51	(void)	{ intHandler(	51	); }
void	routine52	(void)	{ intHandler(	52	); }
void	routine53	(void)	{ intHandler(	53	); }
void	routine54	(void)	{ intHandler(	54	); }
void	routine55	(void)	{ intHandler(	55	); }
void	routine56	(void)	{ intHandler(	56	); }
void	routine57	(void)	{ intHandler(	57	); }
void	routine58	(void)	{ intHandler(	58	); }
void	routine59	(void)	{ intHandler(	59	); }


extern unsigned long cstack_top; /* Defined by the linker */

/* �ж������� */
__attribute__((section(".isr_vector")))
const INTVECT_ITEM __vector_table[] = {
    (void (*)(void))(&cstack_top),      /* ջ��ָ��           */
    resetRoutine,                       /* ��λ�ж�           */
    NMIRoutine,                         /* NMI�ж�            */
#if 0
   // faultRoutine,                         /* Ӳ�������ж�       */
   // memFaultRoutine,                      /* �洢����������ж� */
   // busFaultRoutine,                      /* ���ߴ����ж�       */
   // usageFaultRoutine,                    /* �÷������ж�       */
#else
    EXC_ENTER_HARD_FAULT,
    EXC_ENTER_MEM_FAULT,
    EXC_ENTER_BUS_FAULT,
    EXC_ENTER_USAGE_FAULT,
#endif
    dummy,                                  /* �����ж�...       */
    dummy,
    dummy,
    dummy,
    dummy,
    dummy,
    dummy,
    OS_CPU_PendSVHandler,                   /* ��COS             */
    OS_CPU_SysTickHandler,                  /* ��COS             */
    ROUTINE(16 ),
    ROUTINE(17 ),
    ROUTINE(18 ),
    ROUTINE(19 ),
    ROUTINE(20 ),
    ROUTINE(21 ),
    ROUTINE(22 ),
    ROUTINE(23 ),
    ROUTINE(24 ),
    ROUTINE(25 ),
    ROUTINE(26 ),
    ROUTINE(27 ),
    ROUTINE(28 ),
    ROUTINE(29 ),
    ROUTINE(30 ),
    ROUTINE(31 ),
    ROUTINE(32 ),
    ROUTINE(33 ),
    ROUTINE(34 ),
    ROUTINE(35 ),
    ROUTINE(36 ),
    ROUTINE(37 ),
    ROUTINE(38 ),
    ROUTINE(39 ),
    ROUTINE(40 ),
    ROUTINE(41 ),
    ROUTINE(42 ),
    ROUTINE(43 ),
    ROUTINE(44 ),
    ROUTINE(45 ),
    ROUTINE(46 ),
    ROUTINE(47 ),
    ROUTINE(48 ),
    ROUTINE(49 ),
    ROUTINE(50 ),
    ROUTINE(51 ),
    ROUTINE(52 ),
    ROUTINE(53 ),
    ROUTINE(54 ),
    ROUTINE(55 ),
    ROUTINE(56 ),
    ROUTINE(57 ),
    ROUTINE(58 ),
    ROUTINE(59 ),
};

/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      resetRoutine - ��λ�ж����
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
resetRoutine(void)
{
    unsigned long *pulSrc, *pulDest;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = &_sidata;
    for(pulDest = &_data; pulDest < &_edata; )
    {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    __asm("    ldr     r0, =_bss\n"
          "    ldr     r1, =_ebss\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop\n"
            );

    //
    // Call the application's entry point.
    //
    main();
}

/**
 ******************************************************************************
 * @brief      dummy -  �����ж����
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
dummy(void)
{
    return;
}

/**
 ******************************************************************************
 * @brief      NMIRoutine - NMI�ж����
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
NMIRoutine(void)
{
    while (TRUE)
    {
        ;
    }
}

/**
 ******************************************************************************
 * @brief      intHandler -   ���ݴ�����жϺ��ҵ���������Ӧ��ISR
 * @param[in]  id : �жϺ�
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
intHandler(uint32_t int_num)
{
    INT_RTN int_rtn = intRtnTbl[int_num - 16];

    OSIntEnter();

    if ((uint32_t)NULL != int_rtn.parameter)
    {
        int_rtn.routine(int_rtn.parameter);
    }
    else
    {
        int_rtn.routine();
    }

    OSIntExit();
}

/**
 ******************************************************************************
 * @brief      intConnect - ����ROUTINE
 * @param[in]  int_num   : �жϺ�(16 ~ INT_COUNT)
 * @param[in]  routine   : �жϷ���
 * @param[in]  parameter : �жϲ���
 *
 * @retval     �ɹ�-OK, ʧ��-ERROR
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern status_t
intConnect(uint32_t int_num,
         VOIDFUNCPTR routine,
         uint32_t parameter)
{
    if ((int_num < 16) || (int_num > INT_COUNT))
    {
        return ERROR;
    }

    intRtnTbl[int_num - 16].routine   = routine;
    intRtnTbl[int_num - 16].parameter = parameter;

    return OK;
}

/**
 ******************************************************************************
 * @brief      intDisconnect - ע���ж�ROUTINE
 * @param[in]  int_num: �жϺ�(16 ~ INT_COUNT)
 * @param[out] None
 *
 * @retval     �ɹ�-OK, ʧ��-ERROR
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern status_t
intDisconnect(uint32_t int_num)
{
    if ((int_num < 16) || (int_num > INT_COUNT))
    {
        return ERROR;
    }

    intRtnTbl[int_num - 16].routine = dummy;
    intRtnTbl[int_num - 16].parameter = (uint32_t)NULL;

    return OK;
}

/**
 ******************************************************************************
 * @brief      intPrioSet - �����ж����ȼ�
 * @param[in]  int_num   : �жϺ�(16 ~ INT_COUNT)
 * @param[in]  prio      : �ж����ȼ�
 *
 * @retval     �ɹ�-OK, ʧ��-ERROR
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern status_t
intPrioSet(uint32_t int_num, uint8_t prio)
{
    if ((int_num < 16) || (int_num > INT_COUNT))
    {
        return ERROR;
    }

    CPU_IntSrcPrioSet(int_num, prio);

    return OK;
}

/**
 ******************************************************************************
 * @brief      intEnable - ʹ��ָ�����ж�
 * @param[in]  int_num   : �жϺ�(16 ~ INT_COUNT)
 * @param[out] None
 *
 * @retval     �ɹ�-OK, ʧ��-ERROR
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern status_t
intEnable(uint32_t int_num)
{
    if ((int_num < 16) || (int_num > INT_COUNT))
    {
        return ERROR;
    }

    CPU_IntSrcEn(int_num);

    return OK;
}

 /**
 ******************************************************************************
 * @brief      intDisable - ����ָ�����ж�
 * @param[in]  int_num   : �жϺ�(16 ~ INT_COUNT - 1)
 * @param[out] None
 *
 * @retval     �ɹ�-OK, ʧ��-ERROR
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern status_t
intDisable(uint32_t int_num)
{
    if ((int_num < 16) || (int_num > INT_COUNT - 1))
    {
        return ERROR;
    }

    CPU_IntSrcDis(int_num);

    return OK;
}

/**
 ******************************************************************************
 * @brief      intLock - ���ж�
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern void
intLock(void)
{
    __asm(
            "CPSID   I\n"
            "CPSID   I\n"
         );
}

/**
 ******************************************************************************
 * @brief      intUnlock - ���ж�
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern void
intUnlock(void) {
    __asm(
            "CPSIE   I\n"
            "BX      LR\n"
         );
}

/**
 ******************************************************************************
 * @brief      intLibInit - �жϳ�ʼ��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern void
intLibInit(void)
{
    uint32_t int_num;

    // ��ʼ���жϱ�
    for (int_num = 16; int_num < INT_COUNT; int_num++)
    {
        intConnect(int_num, dummy, (uint32_t)NULL);
    }

    // ʹ��BusFault��memFault��usgFault
    CPU_REG_NVIC_SHCSR |= CPU_REG_NVIC_SHCSR_BUSFAULTENA
            | CPU_REG_NVIC_SHCSR_MEMFAULTENA | CPU_REG_NVIC_SHCSR_USGFAULTENA;
}

/*------------------------------End of intLib.c------------------------------*/
