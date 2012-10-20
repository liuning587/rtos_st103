/*------------------------------------------------------------------------------
Section: Includes
------------------------------------------------------------------------------*/
#include <intlib.h>
#include <ucos_ii.h>

/*------------------------------------------------------------------------------
Section: Macro Definitions
------------------------------------------------------------------------------*/
//#define ROUTINE(n) n < intcount ? routine##n : dummy
#define ROUTINE(n) routine##n 

/*------------------------------------------------------------------------------
Section: Globals
------------------------------------------------------------------------------*/
/* None */
extern unsigned long _sidata;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
/*------------------------------------------------------------------------------
Section: Constant Definitions
------------------------------------------------------------------------------*/
/* None */

/*------------------------------------------------------------------------------
Section: Type Definitions
------------------------------------------------------------------------------*/

/*FOR INTLIB*/
#define INT_COUNT 103
INT_RTN intRtnTbl[INT_COUNT - 15];                     /* �ж�ʵ�ֺ�����ڱ� */
uint32_t intcount = INT_COUNT;                           /* ֧�ֵ��жϸ���*/

/*------------------------------------------------------------------------------
Section: Local Variable
------------------------------------------------------------------------------*/



extern void  EXC_ENTER_HARD_FAULT(void);                                
extern void  EXC_ENTER_MEM_FAULT(void);
extern void  EXC_ENTER_BUS_FAULT(void);
extern void  EXC_ENTER_USAGE_FAULT(void);
extern int32_t main(void);
extern uint32_t intcount;
/*******************************************************************************
*
* resetRoutine - ��λ���� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
//static void resetRoutine(void);

/*******************************************************************************
*
* NMIRoutine - NMI�жϴ���
*
* INPUTS: *   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void NMIRoutine(void);

#if 0
/*******************************************************************************
*
* faultRoutine -  Ӳ�������жϴ��� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void faultRoutine(void);

/*******************************************************************************
*
* memFaultRoutine -  �洢�����������
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void memFaultRoutine(void);

/*******************************************************************************
*
* busFaultRoutine -  ���ߴ����� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void busFaultRoutine(void);

/*******************************************************************************
*
* usageFaultRoutine -  �÷������� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void usageFaultRoutine(void);


#endif
/*******************************************************************************
*
* intHandler -   ���ݴ�����жϺ��ҵ���������Ӧ��Routine
*
* INPUTS: 
*   int_num : �жϺ�
*
* RETURNS:
*   void
*
*******************************************************************************/
static void intHandler (uint32_t int_num);

/* �����ж� */
void	routine16       (void)	{ intHandler(	16	); }
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

void	routine60	(void)	{ intHandler(	60	); }
void	routine61	(void)	{ intHandler(	61	); }
void	routine62	(void)	{ intHandler(	62	); }
void	routine63	(void)	{ intHandler(	63	); }
void	routine64	(void)	{ intHandler(	64	); }
void	routine65	(void)	{ intHandler(	65	); }
void	routine66	(void)	{ intHandler(	66	); }
void	routine67	(void)	{ intHandler(	67	); }
void	routine68	(void)	{ intHandler(	68	); }
void	routine69	(void)	{ intHandler(	69	); }
void	routine70	(void)	{ intHandler(	70	); }
void	routine71	(void)	{ intHandler(	71	); }
void	routine72	(void)	{ intHandler(	72	); }
void	routine73	(void)	{ intHandler(	73	); }
void	routine74	(void)	{ intHandler(	74	); }
void	routine75	(void)	{ intHandler(	75	); }
void	routine76	(void)	{ intHandler(	76	); }
void	routine77	(void)	{ intHandler(	77	); }
void	routine78	(void)	{ intHandler(	78	); }
void	routine79	(void)	{ intHandler(	79	); }
void	routine80	(void)	{ intHandler(	80	); }
void	routine81	(void)	{ intHandler(	81	); }
void	routine82	(void)	{ intHandler(	82	); }
void	routine83	(void)	{ intHandler(	83	); }
void	routine84	(void)	{ intHandler(	84	); }
void	routine85	(void)	{ intHandler(	85	); }
void	routine86	(void)	{ intHandler(	86	); }
void	routine87	(void)	{ intHandler(	87	); }
void	routine88	(void)	{ intHandler(	88	); }

void	routine89	(void)	{ intHandler(	89	); }
void	routine90	(void)	{ intHandler(	90	); }
void	routine91	(void)	{ intHandler(	91	); }
void	routine92	(void)	{ intHandler(	92	); }
void	routine93	(void)	{ intHandler(	93	); }
void	routine94	(void)	{ intHandler(	94	); }
void	routine95	(void)	{ intHandler(	95	); }
void	routine96	(void)	{ intHandler(	96	); }
void	routine97	(void)	{ intHandler(	97	); }
void	routine98	(void)	{ intHandler(	98	); }
void	routine99	(void)	{ intHandler(	99	); }
void	routine100	(void)	{ intHandler(	100	); }
void	routine101(void)	{ intHandler(	101	); }
void	routine102(void)	{ intHandler(	102	); }
void	routine103	(void)	{ intHandler(	103	); }

/*******************************************************************************
*
* dummy -  �����ж� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void dummy(void);

//#define STACK_SIZE 1024
//static unsigned long cstack[STACK_SIZE];

 extern unsigned long cstack_top; /* Defined by the linker */

/* �ж������� */
__attribute__((section(".isr_vector")))
const INTVECT_ITEM __vector_table[] = {
    (void (*)(void))(&cstack_top),                                              /* ջ��ָ��           */
    resetRoutine,                                             /* ��λ�ж�           */
    NMIRoutine,                                               /* NMI�ж�            */
   // faultRoutine,                                           /* Ӳ�������ж�       */ 
   // memFaultRoutine,                                        /* �洢����������ж� */
   // busFaultRoutine,                                        /* ���ߴ����ж�       */
   // usageFaultRoutine,                                      /* �÷������ж�       */
#if 1
    EXC_ENTER_HARD_FAULT, 
    EXC_ENTER_MEM_FAULT,    
    EXC_ENTER_BUS_FAULT,
    EXC_ENTER_USAGE_FAULT,
#endif

    
    dummy,                                                  /* �����ж�...       */
    dummy,
    dummy,
    dummy,
    dummy,    
    dummy,                                                  
    dummy,
    OS_CPU_PendSVHandler,                                   /* ��COS             */
    OS_CPU_SysTickHandler,                                  /* ��COS             */
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
    
    ROUTINE(60 ),
    ROUTINE(61 ),
    ROUTINE(62 ),
    ROUTINE(63 ),
    ROUTINE(64 ),
    ROUTINE(65 ),
    ROUTINE(66 ),
    ROUTINE(67 ),
    ROUTINE(68 ),
    ROUTINE(69 ),
    ROUTINE(70 ),
    ROUTINE(71 ),
    ROUTINE(72 ),
    ROUTINE(73 ),
    ROUTINE(74 ),
    ROUTINE(75 ),
    ROUTINE(76 ),
    ROUTINE(77 ),
    ROUTINE(78 ),
    ROUTINE(79 ),
    ROUTINE(80 ),
    ROUTINE(81 ),
    ROUTINE(82 ),
    ROUTINE(83 ),
    ROUTINE(84 ),
    ROUTINE(85 ),
    ROUTINE(86 ),
    ROUTINE(87 ),
    ROUTINE(88 ),
    ROUTINE(89 ),
    ROUTINE(90 ),
    ROUTINE(91 ),
    ROUTINE(92 ),
    ROUTINE(93 ),
    ROUTINE(94 ),
    ROUTINE(95 ),
    ROUTINE(96 ),
    ROUTINE(97 ),
    ROUTINE(98 ),
    ROUTINE(99 ),
    ROUTINE(100),
    ROUTINE(101),
    ROUTINE(102),
    ROUTINE(103)

}; 

/*------------------------------------------------------------------------------
Section: Function Prototypes
------------------------------------------------------------------------------*/
/*******************************************************************************
*
* intConnect - ����ROUTINE
*
* INPUTS: 
*   int_num   : �жϺ�(16 ~ intcount)
*   routine   : �жϷ���
*   parameter : �жϲ���
*
* RETURNS:
*   status_t: �ɹ�-OK, ʧ��-ERROR
*
*******************************************************************************/
extern status_t intConnect(uint32_t int_num, VOIDFUNCPTR routine, uint32_t parameter) {
    if ((int_num < 16) || (int_num > intcount))
        return ERROR;

    intRtnTbl[int_num - 16].routine   = routine;
    intRtnTbl[int_num - 16].parameter = parameter;
    
    return OK;
}

/*******************************************************************************
*
* intDisconnect - ע���ж�ROUTINE
*
* INPUTS: 
*   int_num: �жϺ�(16 ~ intcount)
*
* RETURNS:
*   status_t: �ɹ�-OK, ʧ��-ERROR
*
*******************************************************************************/
extern status_t intDisconnect(uint32_t int_num) {
    if ((int_num < 16) || (int_num > intcount))
        return ERROR;     

    intRtnTbl[int_num - 16].routine = dummy;
    intRtnTbl[int_num - 16].parameter = NULL;
    
    return OK;
}

/*******************************************************************************
*
* intPrioSet - �����ж����ȼ�
*
* INPUTS: 
*   int_num   : �жϺ�(16 ~ intcount)
*   prio : �ж����ȼ�
*
* RETURNS:
*   status_t: �ɹ�-OK, ʧ��-ERROR
*
*******************************************************************************/
extern status_t intPrioSet(uint32_t int_num, uint8_t prio) {
    if ((int_num < 16) || (int_num > intcount))
        return ERROR;         

    CPU_IntSrcPrioSet(int_num, prio);
    
    return OK;
}

/*******************************************************************************
*
* intEnable - ʹ��ָ�����ж�
*
* INPUTS: 
*   int_num   : �жϺ�(16 ~ intcount)  
*
* RETURNS:
*   status_t: �ɹ�-OK, ʧ��-ERROR
*
*******************************************************************************/
extern status_t intEnable(uint32_t int_num) {
    if ((int_num < 16) || (int_num > intcount))
        return ERROR;         

    CPU_IntSrcEn(int_num);
    
    return OK;
}

/*******************************************************************************
*
* intDisable - ����ָ�����ж�
*
* INPUTS: 
*   int_num   : �жϺ�(16 ~ intcount - 1)  
*
* RETURNS:
*   status_t: �ɹ�-OK, ʧ��-ERROR
*
*******************************************************************************/
extern status_t intDisable(uint32_t int_num) {
    if ((int_num < 16) || (int_num > intcount - 1))
        return ERROR;         

    CPU_IntSrcDis(int_num);
    
    return OK;
}

/*******************************************************************************
*
* intLock - ���ж�
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
extern void intLock(void) {
     __asm(
           "CPSID   I\n"
           "CPSID   I\n"
          );
}

/*******************************************************************************
*
* intUnlock - ���ж�
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
extern void intUnlock(void) {
     __asm(
           "CPSIE   I\n"
           "BX      LR\n"
          );
}

/*******************************************************************************
*
* dummy -  �����ж���� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void dummy(void) {
    return;
}

/*******************************************************************************
*
* intHandler -   ���ݴ�����жϺ��ҵ���������Ӧ��ISR
*
* INPUTS: 
*   id : �жϺ�
*
* RETURNS:
*   void
*
*******************************************************************************/
static void intHandler(uint32_t int_num)
{
    INT_RTN int_rtn = intRtnTbl[int_num - 16];

    OSIntEnter();

    if (NULL != int_rtn.parameter)
        int_rtn.routine(int_rtn.parameter);
    else 
        int_rtn.routine();

    OSIntExit(); 
}

/*******************************************************************************
*
* resetRoutine - ��λ�ж���� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
void resetRoutine(void)
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

/*******************************************************************************
*
* NMIRoutine - NMI�ж���� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void NMIRoutine(void) {
    while (TRUE) {
        ;
    }
}

#if 0
/*******************************************************************************
*
* faultRoutine -  Ӳ�������� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void faultRoutine(void) {
    while (TRUE) {
    }
}

/*******************************************************************************
*
* memFaultRoutine -  �洢�����������
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void memFaultRoutine(void) {
    while (TRUE) {
    }
}
  
/*******************************************************************************
*
* busFaultRoutine -  ���ߴ����� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void busFaultRoutine(void) {
    while (TRUE) {
    }
}

/*******************************************************************************
*
* usageFaultRoutine -  �÷������� 
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
static void usageFaultRoutine(void) {
    while (TRUE) {
    }
}

#endif

/*******************************************************************************
*
* intLibInit - �жϳ�ʼ��
*
* INPUTS: 
*   void
*
* RETURNS:
*   void
*
*******************************************************************************/
extern void intLibInit(void) {
    uint32_t  int_num;
    
    // ��ʼ���жϱ�
    for (int_num = 16; int_num < intcount; int_num++) {
        intConnect(int_num, dummy, NULL);
    }
    
    // ʹ��BusFault��memFault��usgFault
    CPU_REG_NVIC_SHCSR |= CPU_REG_NVIC_SHCSR_BUSFAULTENA | CPU_REG_NVIC_SHCSR_MEMFAULTENA | CPU_REG_NVIC_SHCSR_USGFAULTENA; 
}


/*------------------------------End of intLib.c-------------------------------*/
