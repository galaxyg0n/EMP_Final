/***************** Source File ******************/
/*
 * small_sprints.c
 *
 * Created on: 20. apr. 2025
 * Author: Imported library
 */

/***************** Includes *********************/
#include <stdarg.h>

/*****************  Functions ***********/
static void printchar(char **str, int c)
/************************************
*Input   : Pointer to string pointer, character to print
*Output  : None
*Function: Outputs a character either to string or to putchar
************************************/
{
    extern int putchar(int c);

    if (str) {
        **str = c;
        ++(*str);
    }
    else (void)putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO  2

static int prints(char **out, const char *string, int width, int pad)
/************************************
*Input   : Pointer to output string pointer, input string,
*          minimum width, padding flags
*Output  : Number of characters printed
*Function: Prints a string with optional padding
************************************/
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
            printchar(out, padchar);
            ++pc;
        }
    }
    for ( ; *string ; ++string) {
        printchar(out, *string);
        ++pc;
    }
    for ( ; width > 0; --width) {
        printchar(out, padchar);
        ++pc;
    }

    return pc;
}

#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
/************************************
*Input   : Output string pointer, integer to print,
*          base, signed flag, width, padding, letter base
*Output  : Number of characters printed
*Function: Prints an integer number in given base with formatting
************************************/
{
    char print_buf[PRINT_BUF_LEN];
    register char *s;
    register int t, neg = 0, pc = 0;
    register unsigned int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints(out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN - 1;
    *s = '\0';

    while (u) {
        t = u % b;
        if (t >= 10)
            t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    }

    if (neg) {
        if (width && (pad & PAD_ZERO)) {
            printchar(out, '-');
            ++pc;
            --width;
        } else {
            *--s = '-';
        }
    }

    return pc + prints(out, s, width, pad);
}

static int print(char **out, const char *format, va_list args)
/************************************
*Input   : Output string pointer, format string, va_list args
*Output  : Number of characters printed
*Function: Core printf-like function supporting limited formats
************************************/
{
    register int width, pad;
    register int pc = 0;
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
            for (; *format >= '0' && *format <= '9'; ++format) {
                width *= 10;
                width += *format - '0';
            }
            if (*format == 's') {
                char *s;
                s = (char *)va_arg(args, int);
                pc += prints(out, s ? s : "(null)", width, pad);
                continue;
            }
            if (*format == 'd') {
                pc += printi(out, va_arg(args, int), 10, 1, width, pad, 'a');
                continue;
            }
            if (*format == 'x') {
                pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'X') {
                pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'A');
                continue;
            }
            if (*format == 'u') {
                pc += printi(out, va_arg(args, int), 10, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'c') {
                scr[0] = (char)va_arg(args, int);
                scr[1] = '\0';
                pc += prints(out, scr, width, pad);
                continue;
            }
        } else {
        out:
            printchar(out, *format);
            ++pc;
        }
    }
    if (out) **out = '\0';
    va_end(args);
    return pc;
}


int snprintf(char *buf, unsigned int count, const char *format, ...)
/************************************
*Input   : Buffer to write to, max length, format string, variable args
*Output  : Number of characters written (excluding null terminator)
*Function: Writes formatted output to buffer (limited printf)
************************************/
{
    va_list args;

    (void)count; // Unused param

    va_start(args, format);
    return print(&buf, format, args);
}
