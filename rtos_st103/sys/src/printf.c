/**
 ******************************************************************************
 * @file       printf.c
 * @brief      Implementation of several stdio.h methods, such as printf(),
 *             sprintf() and so on. This reduces the memory footprint of the
 *             binary when using those methods, compared to the libc
 *             implementation.
 *
 ******************************************************************************
 */
#include <stdio.h>
#include <stdarg.h>
#include <types.h>

#ifdef putchar
    #undef putchar
#endif
int putchar(int c)
{
#if 0
extern uint32_t consoleFd;
#if 1
    if((c) == '\n'){
        char ch = '\r';
        ttyWrite(consoleFd, (uint8_t* )&ch, 1);
        ttyWrite(consoleFd, (uint8_t* )&c, 1);
    }
    else
#endif
        ttyWrite(consoleFd, (uint8_t* )&c, 1);
#endif
#if 1
extern void bsp_putchar(char_t c);
    if (c == '\n')
    {
        bsp_putchar('\r');
    }

    bsp_putchar((unsigned char)c);
#endif
    return 1;
}


signed int puts(const char *pStr)
{
    signed int num = 0;
    while (*pStr != 0) {

        putchar(*pStr++);
        num++;
    }
    char c ='\n';
    putchar(c);
    return num;
}

static void printchar(char **str, int c)
{
    if (str) {
        **str = c;
        ++(*str);
    }
    else (void)putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad)
{
    register int pc = 0, padchar = ' ';

    if (width > 0) {
        register int len = 0;
        register const char *ptr;
        for (ptr = string; *ptr; ++ptr) ++len;
        if (len >= width) width = 0;
        else width -= len;
        if (pad & PAD_ZERO) padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
        for ( ; width > 0; --width) {
            printchar (out, padchar);
            ++pc;
        }
    }
    for ( ; *string ; ++string) {
        printchar (out, *string);
        ++pc;
    }
    for ( ; width > 0; --width) {
        printchar (out, padchar);
        ++pc;
    }

    return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
    char print_buf[PRINT_BUF_LEN];
    register char *s;
    register int t, neg = 0, pc = 0;
    register unsigned int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints (out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    while (u) {
        t = u % b;
        if( t >= 10 )
            t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    }

    if (neg) {
        if( width && (pad & PAD_ZERO) ) {
            printchar (out, '-');
            ++pc;
            --width;
        }
        else {
            *--s = '-';
        }
    }

    return pc + prints (out, s, width, pad);
}

static int print(char **out, int *varg)
{
    register int width, pad;
    register int pc = 0;
    register char *format = (char *)(*varg++);
    char scr[2];

    for (; *format != 0; ++format) {
        if (*format == '%') {
            ++format;
            width = pad = 0;
            if (*format == '\0') break;
            if (*format == '%') goto out;
            if (*format == '-') {
                ++format;
                pad = PAD_RIGHT;
            }
            while (*format == '0') {
                ++format;
                pad |= PAD_ZERO;
            }
            for ( ; *format >= '0' && *format <= '9'; ++format) {
                width *= 10;
                width += *format - '0';
            }
            if( *format == 's' ) {
                register char *s = *((char **)varg++);
                pc += prints (out, s?s:"(null)", width, pad);
                continue;
            }
            if( *format == 'd' ) {
                pc += printi (out, *varg++, 10, 1, width, pad, 'a');
                continue;
            }
            if(*format == 'f') {
#if 1   //NEWLIB浮点打印,支持stm32 lm3s测试不过可能需要IQmath库
                char *cptr = (char *) varg++;  //lint !e740 !e826  convert to double pointer
                uint caddr = (uint) cptr ;
                if ((caddr & 0xF) != 0) {
                   cptr += 4 ;
                }
                double dbl = *(double *) cptr ;  //lint !e740 !e826  convert to double pointer
#else   //非NEWLIB
                double dbl = *(double *) varg++ ;  //lint !e740 !e826  convert to double pointer
#endif
                pc += printi (out, (int)dbl, 10, 1, width, pad, 'a');
                scr[0] = '.';
                scr[1] = '\0';
                pc += prints (out, scr, width, pad);
                pc += printi (out, (int)((int)(dbl*1000000.0) % 1000000), 10, 1, width, pad, 'a');
                varg++; //double需要加两次
                continue;
            }
            if( *format == 'x' ) {
                pc += printi (out, *varg++, 16, 0, width, pad, 'a');
                continue;
            }
            if( *format == 'X' ) {
                pc += printi (out, *varg++, 16, 0, width, pad, 'A');
                continue;
            }
            if( *format == 'u' ) {
                pc += printi (out, *varg++, 10, 0, width, pad, 'a');
                continue;
            }
            if( *format == 'c' ) {
                /* char are converted to int then pushed on the stack */
                scr[0] = *varg++;
                scr[1] = '\0';
                pc += prints (out, scr, width, pad);
                continue;
            }
        }
        else {
        out:
            printchar (out, *format);
            ++pc;
        }
    }
    if (out) **out = '\0';
    return pc;
}

/* assuming sizeof(void *) == sizeof(int) */

int printf(const char *format, ...)
{
    register int *varg = (int *)(&format);
    return print(0, varg);
}

int sprintf(char *out, const char *format, ...)
{
    register int *varg = (int *)(&format);
    return print(&out, varg);
}
/* --------------------------------- End Of File ----------------------------*/
