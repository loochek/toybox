#include <stdlib.h>

/** \file */

typedef enum
{
    BUILTIN_CP1251,
    BUILTIN_CP866,
    CSTDLIB_CURRENT_LOCALE
} codepage_t;

extern codepage_t global_cp_switch;

/**
 * \brief свой strcmp для соответствия ТЗ
 * 
 * step - шаг сравнения
*/
int custom_strcmp(const char *a, const char *b, int step);

typedef int (*cmp_func_t)(const void*, const void*);

/// своя сортировка, полностью совместима с объявлением qsort
void bubble_sort(void *data, size_t elem_count, size_t elem_size, cmp_func_t cmp_func);

/// своя реализация быстрой сортировки, полностью совместима с объявлением qsort
void qsort_custom(void *data, size_t elem_count, size_t elem_size, cmp_func_t cmp_func);