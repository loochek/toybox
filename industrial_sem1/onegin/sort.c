#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "sort.h"

/** \file */

codepage_t global_cp_switch = CSTDLIB_CURRENT_LOCALE;

static int is_alpha(unsigned char c);
static int partition(void *data, size_t elem_count, size_t elem_size, cmp_func_t cmp_func);
static void memswap(void *ptr1, void *ptr2, size_t bytes_count);

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
    return *a - *b;
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
                memswap(first_addr, second_addr, elem_size);
        }
    }
}

void qsort_custom(void *data, size_t elem_count, size_t elem_size, cmp_func_t cmp_func)
{
    if (elem_count < 2)
        return;
    int pivot_idx = partition(data, elem_count, elem_size, cmp_func);
    qsort_custom(data, pivot_idx + 1, elem_size, cmp_func);
    qsort_custom((char*)data + (pivot_idx + 1) * elem_size, elem_count - pivot_idx - 1, elem_size, cmp_func);
}

static int partition(void *data, size_t elem_count, size_t elem_size, cmp_func_t cmp_func)
{
    // выбираем опорный элемент
    void* pivot = calloc(1, elem_size);
    memcpy(pivot, (char*)data + (rand() % elem_count) * elem_size, elem_size);
    int i = 0, j = elem_count - 1;
    
    for (;;)
    {
        while (i < elem_count && cmp_func((char*)data + i * elem_size, pivot) < 0)
            i++;
        if (i == elem_count)
            i--;
        while (j >= 0 && cmp_func((char*)data + j * elem_size, pivot) > 0)
            j--;
        if (j == -1)
            j++;
        if (i >= j)
        {
            free(pivot);
            return j;
        }
        memswap((char*)data + i * elem_size, (char*)data + j * elem_size, elem_size);
        i++;
        j--;
    }   
}

static void memswap(void *ptr1, void *ptr2, size_t bytes_count)
{
    const int TMP_BUF_SIZE = 16;
    char tmp[TMP_BUF_SIZE];
    for (size_t i = 0; i < bytes_count; i += TMP_BUF_SIZE)
    {
        size_t copy_size = TMP_BUF_SIZE;
        if (bytes_count - i < TMP_BUF_SIZE)
            copy_size = bytes_count - i;

        memcpy(tmp, ptr1 + i, copy_size);
        memcpy(ptr1 + i, ptr2 + i, copy_size);
        memcpy(ptr2 + i, tmp, copy_size);
    }
}

/// является ли символ буквой в CP1251
static int is_alpha_1251(unsigned char c)
{
    return ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 192 && c <= 223) || // а..я
        (c >= 224 && c <= 255)) || // А..Я
        (c == 184 || c == 168); // ё Ё
}

/// является ли символ буквой в CP866
static int is_alpha_866(unsigned char c)
{
    return ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 128 && c <= 175) || // А..п
        (c >= 224 && c <= 241)); // р..ё
}

/// адаптер
static int is_alpha(unsigned char c)
{
    if (global_cp_switch == BUILTIN_CP1251)
        return is_alpha_1251(c);
    else if (global_cp_switch == BUILTIN_CP866)
        return is_alpha_866(c);
    return isalpha(c);
}