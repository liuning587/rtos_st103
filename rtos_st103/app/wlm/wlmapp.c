/**
 ******************************************************************************
 * @file      wlm.c
 * @brief     C Source file of wlm.c.
 * @details   This file including all API functions's 
 *            implement of wlm.c.	
 *
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <wlmapp.h>
#include <wlmdev.h>
#include <shell.h>
#include <ttylib.h>
#include <debug.h>

/*-----------------------------------------------------------------------------
Section: Type Definitions
-----------------------------------------------------------------------------*/
#define MY_WLM_ADDR1    (0x12)  //µØÖ·1
#define MY_WLM_ADDR2    (0x13)  //µØÖ·2

/*-----------------------------------------------------------------------------
Section: Constant Definitions
-----------------------------------------------------------------------------*/
/* NONE */ 

/*-----------------------------------------------------------------------------
Section: Global Variables
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Section: Local Variables
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Local Function Prototypes
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Function Definitions
-----------------------------------------------------------------------------*/
status_t
wlmapp_init(void)
{
    if (OK != wlm_init(MY_WLM_ADDR1))
    {
        printf("wlm init ERR!\n");
        return ERROR;
    }
    printf("wlm init OK!\n");

    return OK;
}


uint32_t do_wlmrecv(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    uint32_t i = 0;
    uint8_t recvbuf[20];

    if (OK != wlm_init(MY_WLM_ADDR1))
    {
        printf("wlm init ERR!\n");
        return ERROR;
    }
    printf("wlm init OK!\n");
    while (1)
    {
        uint32_t recvlen = 0u;
        memset(recvbuf, 0x00, sizeof(recvbuf));
        recvlen = wlm_recv(MY_WLM_ADDR2, recvbuf, sizeof(recvbuf));
        if (recvlen > 0u)
        {
            printf("I recv len:%d\n", recvlen);
            printbuffer("dat:", recvbuf, recvlen);
        }
        if (i++ % 10 == 0)
            printf("\n");
        printf("*");
        uint8_t c = 0;
        if (ttyRead(consoleFd, &c, 1) == 1)
        {
            if (c == 'q')
                break;
            c = 0;
        }
        taskDelay(10);
    }
    printf("\n");
    return 0;
}

SHELL_CMD(
    wlmrecv, CFG_MAXARGS,        do_wlmrecv,
    "wlmrecv \r\t\t\t\t do_wlmrecv\n"
);

uint32_t do_wlmsend(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    static uint8_t seed = 0;
    uint8_t sendbuf[20];

    if (OK != wlm_init(MY_WLM_ADDR2))
    {
        printf("wlm init ERR!\n");
        return ERROR;
    }
    printf("wlm init OK!\n");
    for (uint8_t i = 0; i < sizeof(sendbuf); i++)
    {
        sendbuf[i] = seed++;
    }
    taskDelay(50);
    wlm_send(MY_WLM_ADDR1, sendbuf, sizeof(sendbuf));
    printbuffer("I send buf", sendbuf, sizeof(sendbuf));

    return 0;
}

SHELL_CMD(
    wlmsend, CFG_MAXARGS,        do_wlmsend,
    "wlmsend \r\t\t\t\t do_wlmsend\n"
);
/*-----------------------------  wlmapp.c  ----------------------------------*/
