/*;********************************************************************************************************
;                                               excALib.s
;                                         exception handle routine
;
;                                          All Rights Reserved
;
;                                           ARM Cortex-M3 Port
;
; File      : excALib.s
; Version   : V1.0
; By        :
;
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : IAR EWARM
;********************************************************************************************************/

/*;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************/

.cpu cortex-m3
.fpu softvfp
.syntax unified
.thumb
.text
    .extern  OSPrioCur
    .extern  OSPrioHighRdy
    .extern  OSTCBCur
    .extern  OSTCBHighRdy

    .extern  excExcHandle

    .global  EXC_ENTER_HARD_FAULT                                     /*; Functions declared in this file*/
    .global  EXC_ENTER_MEM_FAULT
    .global  EXC_ENTER_BUS_FAULT
    .global  EXC_ENTER_USAGE_FAULT



/*;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;*********************************************************************************************************

        RSEG CODE:CODE:NOROOT(2)
        THUMB

*/

/*;********************************************************************************************************
;                                         HANDLE HARD FAULT EXCEPTION
;                                     void EXC_ENTER_HARD_FAULT(void)
;
; Note(s) : 1) 本函数用于处理硬件FAULT异常，函数地址放置于中断向量表编号3的位置中。
;              PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;*********************************************************************************************************/
.thumb_func
EXC_ENTER_HARD_FAULT:
    MOV	R1,#3
	B	excEnterCommon
.thumb_func
EXC_ENTER_MEM_FAULT:
    MOV	R1,#4
	B	excEnterCommon
.thumb_func
EXC_ENTER_BUS_FAULT:
    MOV	R1,#5
	B	excEnterCommon
.thumb_func
EXC_ENTER_USAGE_FAULT:
    MOV	R1,#6

.thumb_func
excEnterCommon:
    CPSID   F
    TST     LR, #0x4
    ITE     EQ
    MRSEQ   R0, MSP
    MRSNE   R0, PSP

    SUBS    R0, R0, #0x20
    STM     R0, {R4-R11}

    BL      excExcHandle

    LDR     R0, =OSPrioCur                                      /*; OSPrioCur = OSPrioHighRdy;*/
    LDR     R1, =OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =OSTCBCur                                       /*; OSTCBCur  = OSTCBHighRdy;*/
    LDR     R1, =OSTCBHighRdy
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                                            /*; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;*/
    LDM     R0, {R4-R11}                                        /*; Restore r4-11 from new process stack*/
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                                             /*; Load PSP with new process SP*/
   /*;ORR     LR, LR, #0x04                                       ; Ensure exception return uses process stack*/
    LDR     LR, =0xFFFFFFFD
    CPSIE   F
    BX      LR                                                  /*; Exception return will restore remaining context*/

    .END
