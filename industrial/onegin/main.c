#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "custom_strcmp.h"

// Будем хранить указатели на начало и конец строк (для перевернутого компаратора) - их и будем сортировать
typedef struct
{
    const char *begin;
    const char *end;
}  string_entry_t;

typedef int (*comp_t)(const void *a_ptr, const void *b_ptr);

void bubble_sort(void *data, size_t elem_count, size_t elem_size, comp_t cmp_func)
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

// компаратор строк
int comp(const void *a_ptr, const void *b_ptr)
{
    string_entry_t a = *(string_entry_t*)a_ptr;
    string_entry_t b = *(string_entry_t*)b_ptr;
    return custom_strcmp(a.begin, b.begin, 1);
}

// перевернутый компаратор
int comp_rev(const void *a_ptr, const void *b_ptr)
{
    string_entry_t a = *(string_entry_t*)a_ptr;
    string_entry_t b = *(string_entry_t*)b_ptr;
    // т.к. .end указывает на нуль-терминатор
    return custom_strcmp(a.end - 1, b.end - 1, -1);
}

int get_file_size(FILE *fd)
{
    // определяем размер файла

    if (fseek(fd, 0, SEEK_END) != 0)
        return -1;

    int file_size = ftell(fd);
    if (file_size == -1)
        return -1;
    if (fseek(fd, 0, SEEK_SET) != 0)
        return -1;

    return file_size;
}

// выдает нуль-терминированную строку с пробелом в начале и гарантированным \n в конце
// в случае ошибки вернет нулевой указатель
char* create_string_from_file(const char* file_name, size_t *str_size_ptr)
{
    FILE *file = fopen(file_name, "r");
    int data_size = -1;
    if ((data_size = get_file_size(file)) == -1)
        return NULL;

    // +2 для пробела, возможного \n в конце и нуль-терминатора
    char* buf = calloc(data_size + 3, sizeof(char));
    if (buf == NULL)
    {
        fclose(file);
        return NULL;
    }

    // читаем файл напрямую в буфер (оставляем место под пробел)
    if (fread(buf + 1, sizeof(char), data_size, file) != data_size)
    {
        free(buf);
        fclose(file);
        return NULL;
    }
    // т. к. мы сдвинулись на 1
    data_size++;
    // пробел в начале
    *buf = ' ';

    // \n в конце (если его не было)
    if (buf[data_size - 1] != '\n')
    {
        buf[data_size] = '\n';
        data_size++;
    }

    // \0 обеспечивается calloc'ом

    if (fclose(file) != 0)
    {
        free(buf);
        fclose(file);
        return NULL;
    }
    *str_size_ptr = data_size;
    return buf;
}

// заменяет \n и пробел в начале на \0, считает количество строк
// минимальная строка - " ", иначе поведение неопределено
size_t parse_string(char *buf)
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

// строит индекс
// если не получилось, возвращает NULL
// иначе возвращает массив из str_cnt элементов типа string_entry_t
string_entry_t *create_index(const char *buf, size_t buf_size, size_t str_cnt)
{
    // выделяем память
    string_entry_t *index = calloc(str_cnt, sizeof(string_entry_t));
    if (index == NULL)
        return NULL;

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

// пишет строки из strings в файл fd
int write_output(FILE *fdout, string_entry_t *index, size_t str_cnt)
{
    if (fdout == NULL)
        return -1;
    // выводим первые str_cnt строк из index
    for (size_t i = 0; i < str_cnt; i++)
    {
        if (fputs(index[i].begin, fdout) == EOF)
            return -1;
        if (fputc('\n', fdout) == EOF)
            return -1;
    }
    // перевод строки в конце
    if (fputc('\n', fdout) == EOF)
        return -1;
    return 0;
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
        printf("Read file error\n");
        return -1;
    }
    size_t str_cnt = parse_string(buf);
    // игнорируем \0 в начале
    string_entry_t *index = create_index(buf + 1, size - 1, str_cnt);
    if (index == NULL)
    {
        printf("Index creation error\n");
        return -1;
    }
    //скопируем индекс, чтобы сохранить его изначальное состояние
    string_entry_t *index_orig = calloc(str_cnt, sizeof(string_entry_t));
    if (index_orig == NULL)
    {
        printf("Malloc error\n");
        return -1;
    }
    memcpy(index_orig, index, str_cnt * sizeof(string_entry_t));

    FILE *fdout = fopen("onegin_parsed.txt", "w");
    if (fdout == NULL)
    {
        printf("Output error\n");
        return -1;
    }
    // сортируем и выводим
    // обычная сортировка
    bubble_sort(index, str_cnt, sizeof(string_entry_t), comp);
    if (write_output(fdout, index, str_cnt) != 0)
    {
        printf("Output error\n");
        return -1;
    }
    // перевернутая
    bubble_sort(index, str_cnt, sizeof(string_entry_t), comp_rev);
    if (write_output(fdout, index, str_cnt) != 0)
    {
        printf("Output error\n");
        return -1;
    }
    // оригинал текста
    if (write_output(fdout, index_orig, str_cnt) != 0)
    {
        printf("Output error\n");
        return -1;
    }

    fclose(fdout);
    // освобождаем память
    free(buf);
    free(index);
    free(index_orig);
    return 0;
}