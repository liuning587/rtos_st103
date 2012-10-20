/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <ctype.h>
#include <string.h>
#include <ucos_ii.h>
#include <maths.h>
#include <sched.h>
#include <time.h>
#include <shell.h>

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* None */

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* None */

/*-----------------------------------------------------------------------------
Section: Local Variable
-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Section: Local Function Prototypes
------------------------------------------------------------------------------*/

uint32_t do_os_info (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    float32_t v = OSVersion() / 100.0;
    // ����汾��Ϣ
    printf("Micrium uC/OS-II %d.%d", (int32_t)v, (int32_t)(v * 100) % 100);
    // ���CPUʹ����
    printf(" CPU Usage %d%%\r\n", (uint32_t)OSCPUUsage);

    return 1;
}

SHELL_CMD(
    osv,    CFG_MAXARGS,        do_os_info,
    "osv \r\t\t\t\t Print32_t OS version info\r\n"
);

uint32_t do_task_info (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    OS_TCB *tcb;
    OS_STK_DATA stkData;
    uint8_t str[20];

    // �������
    printf("%-16s %3s %-10s %8s %20s %10s\r\n", "NAME", "PRI", "STATUS", "SP", "USED/SIZE", "DELAY");
    printf("---------------- --- ---------- -------- -------------------- ----------\r\n");
    tcb = OSTCBList;
    while (NULL != tcb) {
        // ���������
        printf("%-16s", tcb->OSTCBTaskName);
        // ����������ȼ�
        printf(" %3u", tcb->OSTCBPrio);
        // �������״̬
        switch (tcb->OSTCBStat) {
        case OS_STAT_RDY    : printf(" %-10s", "READY");      break;
        case OS_STAT_SUSPEND: printf(" %-10s", "SUSPEND");    break;
        case OS_STAT_SEM    : printf(" %-10s", "PEND SEM");   break;
        case OS_STAT_MBOX   : printf(" %-10s", "PEND MBOX");  break;
        case OS_STAT_Q      : printf(" %-10s", "PEND QUEUE"); break;
        case OS_STAT_MUTEX  : printf(" %-10s", "PEND MUTEX"); break;
        case OS_STAT_FLAG   : printf(" %-10s", "PEND FLAG");  break;
        case OS_STAT_MULTI  : printf(" %-10s", "PEND MULTI"); break;
        default             : printf(" %-10s", "UNKNOW");     break;
        }
        // ���ջ����ַ
        printf(" %8X", (uint32_t)tcb->OSTCBStkPtr);
        // ���ջ������ʹ�����
        if (0u != tcb->OSTCBStkSize) {
            OSTaskStkChk(tcb->OSTCBPrio, &stkData);
            memset(str, 0, sizeof(str));

            sprintf((char_t *)str, "%d/%d(%3d%%)", stkData.OSUsed, tcb->OSTCBStkSize * 4, (stkData.OSUsed * 100) / (tcb->OSTCBStkSize * 4));
            printf(" %20s", str);
        }
        else printf(" %20s", "UNKNOW");
        // �������ȴ�������
        printf(" %10d\r\n", tcb->OSTCBDly);

        tcb = tcb->OSTCBNext;
    }
    return 1;
}

SHELL_CMD(
    i,  CFG_MAXARGS,        do_task_info,
    "i \r\t\t\t\t Summary of tasks' TCBs\r\n"
);

extern void bsp_reboot(void);
uint32_t do_reboot(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    printf("\n");
    bsp_reboot();
    return 0;
}

SHELL_CMD(
    reboot, CFG_MAXARGS,        do_reboot,
    "reboot \r\t\t\t\t Reboot \n"
);


#if 0
uint32_t do_mem_md (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    uint32_t i;
    uint8_t j;
    uint32_t adr, nunits, width;
    uint8_t linebytes, cnt, bytes[16];

    // ��������=2~4
    if ((argc < 2) || (argc > 4)) {
        printf("Usage:\r\n");
        printf("%s\r\n", cmdtp->usage);
        return 1;
    }
    // ȡ��ʼ��ַ
    if (0 == sscanf((char_t *)argv[1], "%i", &adr)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // ȡ��ʾ����
    if (argc > 2) {
        if (0 == sscanf((char_t *)argv[2], "%i", &nunits)) {
            printf ("Usage:\r\n");
            printf ("%s\r\n", cmdtp->usage);
            return 1;
        }
        // ������ʾ1��
        nunits = MAX(nunits, 1);
    } else nunits = 16 * 8; // Ĭ��Ϊ16 * 8

    // ȡ���ʿ��
    if (argc > 3) {
        if ((0 == sscanf((char_t *)argv[3], "%i", &width))
            || ((width != 1) && (width != 2) && (width != 4))) {
            printf ("Usage:\r\n");
            printf ("%s\r\n", cmdtp->usage);
            return 1;
        }
    } else width = 2; // Ĭ��Ϊ����

    linebytes = 0;
    for (i = 0; i < nunits;  i++) {
        // �����ַ
        if (0 == linebytes)
           printf("%08X: ", adr);
        // ��鵱ǰ��ַ�Ƿ������ȡ
#if 0
        if (!sysMemIsReadable(adr, width)) {
            printf("\r\nMemory Access Exception\r\n");
            return 1;
        }
#endif
        printf(" ");
        // ��ȡ����ӡhexֵ
        switch (width) {
        case 1: // ���ֽڶ�ȡ
            bytes[linebytes] = (*((uint8_t *)(adr)));
            printf("%02X", bytes[linebytes++]);
            adr++;
            break;

        case 2: // �����ֶ�ȡ
            (*((uint16_t *)&bytes[linebytes])) = (*((uint16_t *)(adr)));
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            adr += 2;
            break;

        case 4: // ���ֶ�ȡ
            (*((uint32_t *)&bytes[linebytes])) = (*((uint32_t *)(adr)));
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            adr += 4;
            break;

        }

        // ��ӡ�ַ�
        if (16 == linebytes) {
            printf("   *");
            for (j = 0; j < 16; j++) {
                printf("%c", isprint(bytes[j]) ? bytes[j] : '.');
            }
            printf("*\r\n");
            linebytes = 0;
        }
    }
    // ��ӡʣ���ַ�
    if (0 != linebytes) {
        cnt = 16 - linebytes;
        cnt = (cnt << 1) + (cnt >> (width == 1 ? 0 : width / 2));
        for (i = 0; i < cnt; i++)
            printf(" ");
        printf("   *");
        for (i = 0; i < linebytes; i++) {
           printf("%c", isprint(bytes[i]) ? bytes[i] : '.');
        }
        printf("*\r\n");
    }

    return 1;
}

SHELL_CMD(
    d,  CFG_MAXARGS,        do_mem_md,
    "d \r\t adr[nunits[width]] \r\t\t\t\t Display memory\n"
);


uint32_t do_mem_mm (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    uint32_t adr, value, width;

    // ��������=4
    if (argc != 4) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // ȡ��ʼ��ַ
    if (0 == sscanf((char_t *)argv[1], "%i", &adr)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // ȡд����
    if ((0 == sscanf((char_t *)argv[2], "%i", &width))
        || ((width != 1) && (width != 2) && (width != 4))) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // ȡֵ
    if (0 == sscanf((char_t *)argv[3], "%i", &value)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }
#if 0
    // ����Ƿ�����޸�
    if (!sysMemIsWriteable(adr, width)) {
        printf("\r\nMemory Access Exception\r\n");
        return 1;
    }
#endif
    switch (width) {
    case 1: // ���ֽ�д��
        (*((uint8_t *)(adr))) = value; break;
    case 2: // ������д��
        (*((uint16_t *)(adr))) = value; break;
    case 4: // ����д��
        (*((uint32_t *)(adr))) = value; break;
    }

    return 0;
}

SHELL_CMD(
    m,  CFG_MAXARGS,        do_mem_mm,
    "m \r\t\t\t\t Modify memory \r\t adr width value\n"

);

//extern void stats_display(void);
/*SHELL CMD FOR NET*/
uint32_t do_lwip(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    //stats_display();
    return 0;
}

SHELL_CMD(
    lwip,   CFG_MAXARGS,        do_lwip,
    "lwip \r\t\t\t\t Display lwip stats!\r\n"
);

//extern void reboot();
/*SHELL CMD FOR NET*/





uint32_t do_iow(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
       uint32_t io;
       uint32_t value;
       // ��������=2
       if ((argc != 3 )) {
           printf("Usage:\r\n");
           printf("%s\r\n", cmdtp->usage);
           return 1;
       }
       if (0 == sscanf((char_t *)argv[1], "%i", &io)) {
           printf ("Usage:\r\n");
           printf ("%s\r\n", cmdtp->usage);
           return 1;
       }
       if (0 == sscanf((char_t *)argv[2], "%i", &value)) {
           printf ("Usage:\r\n");
           printf ("%s\r\n", cmdtp->usage);
           return 1;
       }
       //gpio_write(io,value);

    return 0;
}

SHELL_CMD(
    iow,    CFG_MAXARGS,        do_iow,
    "iow \r\t io value \r\t\t\t\t Modify io's status \n"
);

uint32_t do_ior(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    uint32_t io;
    /*uint32_t value;*/
    // ��������=1
    if ((argc != 2 )) {
        printf("Usage:\r\n");
        printf("%s\r\n", cmdtp->usage);
        return 1;
    }
    if (0 == sscanf((char_t *)argv[1], "%i", &io)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

   // printf("value : %d\n",gpio_read(io));

    return 0;
}

SHELL_CMD(
    ior,    CFG_MAXARGS,        do_ior,
    "ior \r\t io \r\t\t\t\t Read io's status    \r\n"
);


uint32_t
do_deltask(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    uint32_t taskid;
    if (argc != 2)
    {
        printf("Usage:\r\n");
        printf("%s\r\n", cmdtp->usage);
        return 1;
    }
    if (0 == sscanf((char_t *) argv[1], "%i", &taskid))
    {
        printf("Usage:\r\n");
        printf("%s\r\n", cmdtp->usage);
        return 1;
    }
    if (255 <= sscanf((char_t *) argv[1], "%i", &taskid))
    {
        printf("Usage:\r\n");
        printf("%s\r\n", cmdtp->usage);
        return 1;
    }
    taskDelete((uint8_t)taskid);
    return(0);
}

SHELL_CMD(
    deltask,    CFG_MAXARGS,        do_deltask,
    "deltask \r\t taskid \r\t\t\t\t delete task\n"
);
#endif
uint32_t
do_show_osecb(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    int32_t i;
    int32_t num[6] = {0, 0, 0, 0, 0, 0};

    for(i = 0; i < OS_MAX_EVENTS; i++)
    {
        if(OSEventTbl[i].OSEventType <= OS_EVENT_TYPE_FLAG)
        {
            num[OSEventTbl[i].OSEventType]++;
        }
    }
    printf("OS EVENT control blocks info:\n");
    printf("TOTAL            %d\n", OS_MAX_EVENTS);
    printf("UNUSED           %d\n", num[OS_EVENT_TYPE_UNUSED]);
    printf("MBOX             %d\n", num[OS_EVENT_TYPE_MBOX]);
    printf("Q                %d\n", num[OS_EVENT_TYPE_Q]);
    printf("SEM              %d\n", num[OS_EVENT_TYPE_SEM]);
    printf("MUTEX            %d\n", num[OS_EVENT_TYPE_MUTEX]);
    printf("FLAG             %d\n", num[OS_EVENT_TYPE_FLAG]);


    return (0);
}
SHELL_CMD(
    osecb,    CFG_MAXARGS,        do_show_osecb,
    "osecb \r\t\t\t\t show os event control blocks info\n"
);

/*------------------------------------------------------------------------------
Section: Function Definitions
------------------------------------------------------------------------------*/


/******************************End of sxos.c ******************************/


