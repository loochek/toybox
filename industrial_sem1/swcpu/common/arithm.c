#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "headers/lerror.h"
#include "headers/arithm.h"

double str_to_num(const char str[])
{
    if (str == NULL)
    {
        __lerrno = LERR_NAN;
        return 0;
    }
    __lerrno = LERR_NO_ERROR;
    double num = 0;
    int cnt = 0;
    sscanf(str, "%lg%n", &num, &cnt);
    if (cnt != strlen(str))
    {
        LERR(LERR_NAN, "Not a number");
        return 0;
    }
    return num;
}

void num_to_str(double num, char str[])
{
    sprintf(str, "%lg", num);
}