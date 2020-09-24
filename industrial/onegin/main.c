#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "sort.h"
#include "file_utils.h"
#include "lerror.h"

/** \file */

// Будем хранить указатели на начало и конец строк (для перевернутого компаратора) - их и будем сортировать
typedef struct
{
    const char *begin;
    const char *end;
}  string_entry_t;

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

/**
 * \brief заменяет \n и пробел в начале на \0, считает количество строк
 * 
 * минимальная строка - " ", иначе поведение неопределено
*/
static size_t parse_string(char *buf)
{
    // (сколько \n - столько и строк по определению)
    size_t str_cnt = 0;
    *buf = '\0';
    for (char* ptr = buf + 1; *ptr; ptr++)
    {
        if (*ptr == '\n')
        {
            *ptr = '\0';
            str_cnt++;
        }
    }
    return str_cnt;
}

/**
 * возвращает массив из str_cnt элементов типа string_entry_t
*/
static string_entry_t *create_index(const char *buf, size_t buf_size, size_t str_cnt)
{
    // выделяем память
    string_entry_t *index = calloc(str_cnt, sizeof(string_entry_t));
    if (index == NULL)
    {
        LERRNO(LERR_ALLOC);
        LERRSTR("unable to allocate memory");
        return NULL;
    }
    size_t idx_cnt = 0;
    // первый и последний символы - частные случаи
    index[idx_cnt].begin = buf;
    for (size_t i = 0; i < buf_size - 1; i++)
    {
        if (buf[i] == '\0')
        {
            index[idx_cnt].end = buf + i;
            idx_cnt++;
            index[idx_cnt].begin = buf + i + 1;
        }
    }
    index[idx_cnt].end = buf + buf_size - 1;
    return index;
}

/**
 * \brief пишет строки из индекса в файл fd
 * 
 * возвращает 0, если все получилось, иначе -1
*/

static int write_by_index(FILE *fdout, string_entry_t *index, size_t str_cnt)
{
    if (fdout == NULL)
    {
        LERRNO(LERR_FILE_IO);
        LERRSTR("file descriptor is null");
        return -1;
    }
    // выводим первые str_cnt строк из index
    for (size_t i = 0; i < str_cnt; i++)
    {
        if (fputs(index[i].begin, fdout) == EOF)
        {
            LERRNO(LERR_FILE_IO);
            LERRSTR("fputs failed");
            return -1;
        }
        if (fputc('\n', fdout) == EOF)
        {
            LERRNO(LERR_FILE_IO);
            LERRSTR("fputc failed");
            return -1;
        }
    }
    // перевод строки в конце
    if (fputc('\n', fdout) == EOF)
    {
        LERRNO(LERR_FILE_IO);
        LERRSTR("fputc failed");
        return -1;
    }
    return 0;
}

static string_entry_t* copy_index(string_entry_t *src, size_t str_cnt)
{
    string_entry_t *copy = calloc(str_cnt, sizeof(string_entry_t));
    if (copy == NULL)
    {
        LERRNO(LERR_ALLOC);
        LERRSTR("unable to allocate memory");
        return NULL;
    }
    memcpy(copy, src, str_cnt * sizeof(string_entry_t));
    return copy;
}

#ifndef TEST
int main(const int argc, const char* argv[])
#else
int dummy()
#endif
{
    size_t size = 0;
    char *buf = create_string_from_file("onegin.txt", &size);
    if (buf == NULL)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Unable to read input file. Does it exist?\n");
        return -1;
    }

    size_t str_cnt = parse_string(buf);

    // игнорируем \0 в начале
    string_entry_t *index = create_index(buf + 1, size - 1, str_cnt);
    if (index == NULL)
    {
        LERRPRINT("Errors");
        fprintf(stderr, "Can't create index\n");
        return -1;
    }

    //скопируем индекс, чтобы сохранить его изначальное состояние
    string_entry_t *index_orig = copy_index(index, str_cnt);
    if (index_orig == NULL)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Can't copy index\n");
        return -1;
    }

    FILE *fdout = fopen("onegin_parsed.txt", "w");
    if (fdout == NULL)
    {
        fprintf(stderr, "Unable to open output file\n");   
        return -1;
    }

    // сортируем и выводим
    // обычная сортировка
    bubble_sort(index, str_cnt, sizeof(string_entry_t), comp);
    if (write_by_index(fdout, index, str_cnt) != 0)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Output error\n");
        return -1;
    }

    // перевернутая
    bubble_sort(index, str_cnt, sizeof(string_entry_t), comp_rev);
    if (write_by_index(fdout, index, str_cnt) != 0)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Output error\n");
        return -1;
    }

    // оригинал текста
    if (write_by_index(fdout, index_orig, str_cnt) != 0)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Output error\n");
        return -1;
    }

    if (fclose(fdout) != 0)
    {
        LERRPRINT("Error");
        fprintf(stderr, "Output error\n");
        return -1;
    }

    // освобождаем память
    free(buf);
    free(index);
    free(index_orig);
    return 0;
}