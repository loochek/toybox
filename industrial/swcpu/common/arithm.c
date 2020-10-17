#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "headers/arithm.h"

int32_t str_to_num(const char str[])
{
    int32_t sign = 1;
    int32_t num = 0;
    size_t i = 0;
    if (str[0] == '-')
    {
        sign = -1;
        i++;
    }
    else if (str[0] == '+')
        i++;
    while (isdigit(str[i]))
    {
        num *= 10;
        num += str[i] - '0';
        i++;
    }
    if (str[i] != '.')
        return num * 1000 * sign;
    i++;
    int rcnt = 0;
    while (isdigit(str[i]) && rcnt <= 2)
    {
        num *= 10;
        num += str[i] - '0';
        i++;
        rcnt++;
    }
    if (3 - rcnt > 0)
        num *= 10 * (3 - rcnt);
    return num * sign;
}

void num_to_str(int32_t num, char str[])
{
    if (abs(num) % 1000 != 0)
        sprintf(str, "%d.%03d", num / 1000, abs(num) % 1000);
    else
        sprintf(str, "%d", num / 1000);
}
