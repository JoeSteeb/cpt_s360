//Joseph Steeb 11751063
//8-31-2021

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>

char *ctable = "0123456789ABCDEF";
int BASE = 10;
typedef unsigned int u32;

int myprintf(char *fmt, ...);
int prints(char *ptr);
int printu(u32);
int rpu(u32 x);
int printo(u32 x);
int printx(u32 x);
int printd(int x);

int main(int argc, char *argv[], char *env[])
{
    myprintf("argc= %d", argc);

    myprintf("argv= \n");
    int i;
    for (i = 1; i < argc; i++)
        myprintf("%s, ", argv[i]);

    myprintf("env= \n");
    for (i = 0; env[i] != NULL; i++)
        myprintf("%s, \n", env[i]);

    myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n",
             'A', "this is a test", 100, 100, 100, -100);
}

//part 1

int rpu(u32 x)
{
    char c;
    if (x)
    {
        c = ctable[x % BASE];
        rpu(x / BASE);
        putchar(c);
    }
}

int printu(u32 x)
{
    (x == 0) ? putchar('0') : rpu(x);
    putchar(' ');
}

int prints(char *ptr)
{
    while (*ptr != 0)
    {
        putchar(*ptr);
        ptr++;
    }
}

int printd(int x)
{
    if (x < 0)
        putchar('-');
    printu(abs(x));
}

int printo(u32 x)
{
    BASE = 8;
    printu(x);
    BASE = 10;
}

int printx(u32 x)
{
    BASE = 16;
    printu(x);
    BASE = 10;
}

int myprintf(char *fmt, ...)
{
    char *cp = fmt;
    bool foundInit = false;
    va_list va;
    int p_count = 0;

    while (*cp != '\0')
    {
        if (*cp == '%')
            p_count++;
        *cp++;
    }

    cp = fmt;
    va_start(va, fmt);

    while (*cp != '\0')
    {
        if (*cp == '%')
        {
            cp++;
            if (*cp == 'c')
                putchar(va_arg(va, int));

            if (*cp == 's')
                prints(va_arg(va, char *));

            if (*cp == 'u')
                printu(va_arg(va, u32));

            if (*cp == 'd')
            {
                printd(va_arg(va, int));
            }

            if (*cp == 'o')
                printo(va_arg(va, u32));

            if (*cp == 'x')
            {
                printx(va_arg(va, u32));
            }

            cp++;
        }

        else
        {
            putchar(*cp);
            cp++;
        }
    }
}

//Part 2
