#include <stdlib.h>

/** \file */

/**
 * \brief свой strcmp для соответствия ТЗ
 * 
 * step - шаг сравнения
*/
int custom_strcmp(const char *a, const char *b, int step);

typedef int (*cmp_func_t)(const void*, const void*);

/// своя сортировка, полностью совместима с объявлением qsort
void bubble_sort(void *data, size_t elem_count, size_t elem_size, cmp_func_t cmp_func);