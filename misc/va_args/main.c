#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sum(int dummy, ...)
{
    va_list lst;
    va_start(lst, dummy);
    int summ = 0;
    for (;;)
    {
        int a = va_arg(lst, int);
        if (a == 0)
            break;
        summ += a;
    }
    va_end(lst);
    return summ;
}

void custom_printf(const char *fmt, ...)
{
    va_list lst;
    va_start(lst, fmt);
    for (const char* ptr = fmt; *ptr; ptr++)
    {
        if (*ptr == '%')
        {
            ptr++;
            if (*ptr == 'c')
                putchar(va_arg(lst, int));
            else if (*ptr == 'd')
            {
                char buf[30];
                int x = va_arg(lst, int);
                int sign = 0;
                if (x > 0)
                    sign = 1;
                else
                {
                    sign = -1;
                    x = -x;
                }
                int buf_ptr = 0;
                while (x > 0)
                {
                    int digit = x % 10;
                    x /= 10;
                    buf[buf_ptr] = '0' + digit;
                    buf_ptr++;
                }
                if (sign == -1)
                {
                    buf[buf_ptr] = '-';
                    buf_ptr++;
                }
                for (int i = buf_ptr; i >= 0; i--)
                    putchar(buf[i]);
            }
            else if (*ptr == 's')
            {
                const char* str = va_arg(lst, const char*);
                for (; *str; str++)
                    putchar(*str);
            }
        }
        else
            putchar(*ptr);
    }
    va_end(lst);
}

int main()
{
    custom_printf("hell%d\n%s\n%c\n", -12312, "literal", 'o');

    custom_printf("%d\n", sum(0, 1, 2, 3, 4, 0));
    custom_printf("%d\n", sum(0, 1, 2, 3, 4, -4, 0));

    //custom_printf("%s");
    return 0;
}