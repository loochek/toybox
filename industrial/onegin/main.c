#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>

#include "sort.h"
#include "string_lib.h"
#include "lerror.h"

/** \file */

/// компаратор строк
static int comp(const void *a_ptr, const void *b_ptr)
{
    string_entry_t a = *(string_entry_t*)a_ptr;
    string_entry_t b = *(string_entry_t*)b_ptr;
    return custom_strcmp(a.begin, b.begin, 1);
}

/// перевернутый компаратор
static int comp_rev(const void *a_ptr, const void *b_ptr)
{
    string_entry_t a = *(string_entry_t*)a_ptr;
    string_entry_t b = *(string_entry_t*)b_ptr;
    // т.к. .end указывает на нуль-терминатор
    return custom_strcmp(a.end - 1, b.end - 1, -1);
}

#ifndef TEST
int main(const int argc, const char* argv[])
#else
int dummy(const int argc, const char* argv[])
#endif
{
    setlocale(LC_ALL, "ru_RU.CP1251");

    const char* input_file_name  = NULL;
    const char* output_file_name = NULL;

    if (argc == 3)
    {
        input_file_name  = argv[1];
        output_file_name = argv[2];
    }
    else
    {
        printf("Usage:\n"
               "onegin <input file name> <output file name>\n");
        return 0;
    }

    string_index_t *index = create_index_from_file(input_file_name);
    if (index == NULL)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Can't create index\n");
        return -1;
    }
    //скопируем индекс, чтобы сохранить его изначальное состояние
    string_index_t *index_orig = copy_index(index);
    if (index_orig == NULL)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Can't copy index\n");
        return -1;
    }

    FILE *fout = fopen(output_file_name, "w");
    if (fout == NULL)
    {
        fprintf(stderr, "Unable to open output file\n");   
        return -1;
    }

    // сортируем и выводим
    // обычная сортировка
    qsort_custom(index->string_entries, index->str_cnt, sizeof(string_entry_t), comp);
    if (write_by_index(fout, index) != 0)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Output error\n");
        return -1;
    }

    // перевернутая
    qsort_custom(index->string_entries, index->str_cnt, sizeof(string_entry_t), comp_rev);
    if (write_by_index(fout, index) != 0)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Output error\n");
        return -1;
    }

    // оригинал текста
    if (write_by_index(fout, index_orig) != 0)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Output error\n");
        return -1;
    }

    if (fclose(fout) != 0)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Output error\n");
        return -1;
    }

    // освобождаем память
    destroy_index(index);
    destroy_index(index_orig);
    return 0;
}