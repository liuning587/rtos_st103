/**
 ******************************************************************************
 * @file       shell.c
 * @version    V1.1.4
 * @brief      API C source file of shell.c
 *             Created on: 2012-4-9
 *             Author: Administrator
 * @details    This file including all API functions's implement of dps.
 *
 ******************************************************************************
 */
#include <shell.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

extern int32_t bsp_getchar(void);
extern void printchar(char c);  /* ���Ҫ��Ϊtty�豸 */

static uint8_t console_buffer[CFG_CBSIZE]; /* console I/O buffer   */
static const char_t erase_seq[] = "\b \b";
static const char_t *prompt = "->";

/*-----------------------------------------------------------------------------
 Section: static Function Prototypes
 ----------------------------------------------------------------------------*/
static bool_e readline(void);
static cmd_tbl_t *find_cmd(const char *cmd);
static int parse_line(char_t *line, char_t *argv[]);
static int run_command(char_t *cmd);

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/

/***************************************************************************
 * find command table entry for a command
 */
static cmd_tbl_t *find_cmd(const char_t *cmd)
{
    cmd_tbl_t *cmdtp;
    cmd_tbl_t *cmdtp_temp = &__shell_cmd_start; /*Init value */
    const char_t *p;
    int32_t len;
    int32_t n_found = 0;

    /*
     * Some commands allow length modifiers (like "cp.b");
     * compare command name only until first dot.
     */
    len = ((p = strchr(cmd, '.')) == NULL) ? strlen(cmd) : (p - cmd);

    for (cmdtp = &__shell_cmd_start; cmdtp != &__shell_cmd_end; cmdtp++)
    {
        if (strncmp(cmd, (char *) cmdtp->name, len) == 0)
        {
            if (len == strlen(cmdtp->name))
                return cmdtp; /* full match */

            cmdtp_temp = cmdtp; /* abbreviated command ? */
            n_found++;
        }
    }
    if (n_found == 1)
    {
        /* exactly one match */
        return cmdtp_temp;
    }

    return NULL; /* not found or ambiguous command */
}

static cmd_tbl_t *pmatch_cmd = NULL;

static int32_t match_cmd(char_t *buf, int32_t len)
{
    cmd_tbl_t *pcmd = NULL;
    int32_t i = 0;
    uint8_t flag = 0u;

    for (i = 0; i < len; i++)
    {
        if (buf[i] == ' ')
        {
            return -1;
        }
    }

    (pmatch_cmd != NULL) ? (pcmd = pmatch_cmd + 1) : (pmatch_cmd = pcmd = &__shell_cmd_start);

    do {
        if (pcmd >= &__shell_cmd_end)
        {
            pcmd = &__shell_cmd_start;
            flag = 1u;
        }
        if (strncmp(pcmd->name, buf, len) == 0)
        {
            pmatch_cmd = pcmd;
            strncpy(buf, pcmd->name, CFG_CBSIZE);
            strncat(buf, " ", CFG_CBSIZE);
            return strlen(buf);
        }
        if (flag == 1u)
        {
            flag = 0u;
        }
        else
        {
            pcmd++;
        }
    } while (pcmd != pmatch_cmd);

    return -2;
}

static bool_e readline(void)
{
    uint8_t c;
    uint8_t *p = console_buffer;
    int32_t n = 0;
    int32_t n_pos = 0;
    int32_t ret;

    // ��ӡ��ʾ��
    printf(prompt);
    //fflush(stdout);
    while (TRUE)
    {
        taskDelay(1);
        // �������
        if ((c = bsp_getchar()) == 0)
            continue;
        // ���������ַ�
        switch (c)
        {
        case '\r': // �س�
        case '\n': // ���У������������
            p[n] = '\0';
             printf ("\r\n");
            pmatch_cmd = NULL;
            return TRUE;

        case 0x03: // Ctrl + C
            // ��ջ�����
            console_buffer[0] = '\0';
            printf("\r\n");
            return FALSE;

        case 0x08: //�˸��
            if (0 >= n)
                continue;
            // ɾ�����һ���ַ�
            p[n] = '\0';
            //p--;
            n--;
            n_pos = n;
            pmatch_cmd = NULL;
            printf(erase_seq);
            continue;

        case 0x09: // TAB
            ret = match_cmd((char_t *)console_buffer, n_pos);
            n = ret > 0 ? ret : n;
            printf("\r");
            printf(prompt);
            printf("%s", console_buffer);
            printf("\033[0K"); // Clear from cursor to end of line
            continue;

        case 0x7F: // DEL
            continue;

        default: // �ɽ��ܵ��ַ�
            // Buffer full
            if (n >= CFG_CBSIZE - 2)
            {
                printf("\a");//������������ֽ�������
                continue;
            }
            printchar(c);
            // ���浽������
            p[n] = c;
            n++;
            n_pos = n;
            break;
        }
    }
    return TRUE;
}

/****************************************************************************
 * returns:
 *  1  - command executed, repeatable
 *  0  - command executed but not repeatable, interrupted commands are
 *       always considered not repeatable
 *  -1 - not executed (unrecognized, bootd recursion or too many args)
 *           (If cmd is NULL or "" or longer than CFG_CBSIZE-1 it is
 *           considered unrecognized)
 *
 * WARNING:
 *
 * We must create a temporary copy of the command since the command we get
 * may be the result from getenv(), which returns a pointer directly to
 * the environment data, which may change magicly when the command we run
 * creates or modifies environment variables (like "bootp" does).
 */
static int parse_line(char_t *line, char_t *argv[])
{
    int nargs = 0;

    while (nargs < CFG_MAXARGS)
    {

        /* skip any white space */
        while ((*line == ' ') || (*line == '\t'))
        {
            ++line;
        }

        if (*line == '\0')
        {
            /* end of line, no more args    */
            argv[nargs] = NULL;
            return (nargs);
        }

        argv[nargs++] = line; /* begin of argument string */

        /* find end of string */
        while (*line && (*line != ' ') && (*line != '\t'))
        {
            ++line;
        }

        if (*line == '\0')
        {
            /* end of line, no more args    */
            argv[nargs] = NULL;

            return (nargs);
        }

        *line++ = '\0'; /* terminate current arg     */
    }

    printf("** Too many args (max. %d) **\n", CFG_MAXARGS);
    return (nargs);
}

static int32_t run_command(char_t *cmd)
{
    cmd_tbl_t *cmdtp;
    char_t cmdbuf[CFG_CBSIZE]; /* working copy of cmd      */

    char_t *token; /* start of token in cmdbuf */
    char_t *sep; /* end of token (separator) in cmdbuf */
    char_t *str = cmdbuf;
    char_t *argv[CFG_MAXARGS + 1]; /* NULL terminated  */
    int32_t argc, inquotes;
    int32_t rc = 0;
    int32_t len = 0;

    len = 0;
    len = strlen(cmd);
    if (len >= CFG_CBSIZE)
    {
        printf("Command too long!\n\r");
        return -1;
    }
    memset(cmdbuf, 0, CFG_CBSIZE);
    memset(argv, 0, CFG_MAXARGS);
    strcpy(&cmdbuf[0], cmd);

    /* Process separators and check for invalid
     * repeatable commands
     */

    while (*str)
    {

        /*
         * Find separator, or string end
         * Allow simple escape of ';' by writing "\;"
         */
        for (inquotes = 0, sep = str; *sep; sep++)
        {
            if ((*sep == '\'') && (*(sep - 1) != '\\'))
                inquotes = !inquotes;

            if (!inquotes && (*sep == ';') && /* separator        */
                    (sep != str) && /* past string start    */
                    (*(sep - 1) != '\\')) /* and NOT escaped  */
                break;
        }

        /*
         * Limit the token to data between separators
         */
        token = str;
        if (*sep)
        {
            str = sep + 1; /* start of command for next pass */
            *sep = '\0';
        }
        else
            str = sep; /* no more commands for next pass */

        //printf("%s %s %s\r\n",console_buffer,argv[0],token);
        /* Extract arguments */
        if ((argc = parse_line(token, argv)) == 0)
        {
            rc = -1; /* no command at all */
            continue;
        }

        //printf("%s %s\r\n",console_buffer,argv[0]);

        /* Look up command in command table */
        if ((cmdtp = find_cmd((char*) argv[0])) == NULL)
        {
            printf("Unknown command '%s' - try 'help'\r\n", argv[0]);
            rc = -1; /* give up after bad command */
            continue;
        }

        /* found - check max args */
        if (argc > cmdtp->maxargs)
        {
            printf("Usage:\n%s\r\n", cmdtp->usage);
            rc = -1;
            continue;
        }

        /* OK - call function to do the command */
        if ((cmdtp->cmd)(cmdtp, argc, (const uint8_t **)argv) != 0)
        {
            rc = -1;
        }

    }

    return rc;
}
/*SHELL CMD FOR HELP*/
uint32_t help()
{
    uint8_t *usage;
    cmd_tbl_t *cmdtptemp;
    printf("\n");
    /* print short help (usage) */
    for (cmdtptemp = &__shell_cmd_start; cmdtptemp != &__shell_cmd_end;
            cmdtptemp++)
    {
        usage = (uint8_t *)cmdtptemp->name;
        if (usage == NULL)
            continue;
        printf((char*) usage);
        printf("\r\t\t\t");
        usage = (uint8_t *)cmdtptemp->usage;
        /* allow user abort */
        if (usage == NULL)
            continue;
        printf((char*) usage);
    }
    return 0;

}
/*SHELL CMD FOR SHELL*/
uint32_t do_help(cmd_tbl_t * cmdtp, uint32_t argc, uint8_t *argv[])
{
    return help();
}

SHELL_CMD( help, CFG_MAXARGS, help, "Print this list\r\n");
void shell_loop(void)
{
    while (1)
    {
        if (readline())
            run_command((char_t *) console_buffer);
    }
}

#include <sched.h>
/*FOR SHELL*/
#define OS_TASK_SHELL_PRIO              6               /* SHELL�̵߳����ȼ�*/
#define OS_TASK_SHELL_STK_SIZE          1024            /* SHELL�̵߳Ķ�ջ��С*/
uint32_t shellstack[OS_TASK_SHELL_STK_SIZE/4];          /*SHELL�̵߳Ķ�ջ*/

void shellInit()
{
    taskSpawn("SHELL", OS_TASK_SHELL_PRIO, shellstack, OS_TASK_SHELL_STK_SIZE, (OSFUNCPTR)shell_loop, 0);
}
