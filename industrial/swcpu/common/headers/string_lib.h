#include <stdlib.h>
#include <stdio.h>

/**
 * \file
 * Библиотека по работе со строками
 * Позволяет загружать строки из текстового файла, предоставляет эффективные методы работы с ними
*/

typedef struct
{
    char *begin;
    char *end;
}  string_entry_t;

typedef struct
{
    string_entry_t* string_entries;
    size_t str_cnt;
    char *buf;
}  string_index_t;

/**
 * \brief загружает текстовый файл и строит индекс по его строкам
 * 
 * в случае ошибки возвращает нулевой указатель
*/
string_index_t *create_index_from_file(const char* file_name);

/**
 * \brief пишет строки из индекса в файл fd
 * 
 * возвращает 0, если все получилось, иначе -1
*/
int write_by_index(FILE *fout, string_index_t *index);

/**
 * \brief делает копию индекса
 * 
 * возвращает нулевой указатель в случае ошибки
*/
string_index_t* copy_index(string_index_t *src);

/**
 * \brief Освобождает память
*/
void destroy_index(string_index_t *index);