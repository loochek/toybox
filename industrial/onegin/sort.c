#include <string.h>
#include "sort.h"

/** \file */

/// так как библиотечная не может в русские буквы
static int is_alpha(unsigned char c)
{
    return ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 192 && c <= 223) ||
        (c >= 224 && c <= 255)) ||
        (c == 184 || c == 168);
}

int custom_strcmp(const char *a, const char *b, int step)
{
    // сравниваем строки a и b без учета знаков препинания и пробелов
    // иначе говоря, рассмариваем только буквы a..z A..Z а..я А...Я

    while (*a != '\0' && *b != '\0')
    {
        // игнорируем не-буквы
        while (!is_alpha(*a) && *a != '\0')
            a += step;
        while (!is_alpha(*b) && *b != '\0')
            b += step;
        if (*a == 0 || *b == 0)
            break;

        if (*a > *b)
            return 1;
        else if (*a < *b)
            return -1;
        else
        {
            a += step;
            b += step;
        }
    }
    if (*a != '\0' && *b == '\0')
        return 1;
    else if (*a == '\0' && *b != '\0')
        return -1;
    else
        return 0;
}

void bubble_sort(void *data, size_t elem_count, size_t elem_size, cmp_func_t cmp_func)
{
    for (int i = elem_count; i >= 0; i--)
    {
        for (int j = 0; j < i - 1; j++)
        {
            char *first_addr = (char*)data + j * elem_size;
            char *second_addr = (char*)data + (j + 1) * elem_size;
            if (cmp_func(first_addr, second_addr) > 0)
            {
                #warning check size of tmp buffer!
                char tmp[16];
                memcpy(tmp, first_addr, elem_size);
                memcpy(first_addr, second_addr, elem_size);
                memcpy(second_addr, tmp, elem_size);
            }
        }
    }
}