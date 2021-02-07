#include "headers/string_lib.h"

#include "headers/lerror.h"

/** \file */

static char* create_buffer_from_file(const char* file_name, size_t *str_size_ptr);
static int get_file_size(FILE *fd);
static size_t parse_string(char *buf);
static string_index_t *create_index(char *buf, size_t buf_size, size_t str_cnt);

string_index_t *create_index_from_file(const char* file_name)
{
    size_t buf_size = 0;
    char *buf = create_buffer_from_file(file_name, &buf_size);
    if (buf == NULL)
        return NULL;

    size_t str_cnt = parse_string(buf);

    // игнорируем \0 в начале
    string_index_t *index = create_index(buf + 1, buf_size - 1, str_cnt);
    if (index == NULL)
    {
        free(buf);
        return NULL;
    }
    index->buf = buf;
    return index;
}

void destroy_index(string_index_t *index)
{
    if (index == NULL)
        return;
    free(index->buf);
    free(index->string_entries);
    free(index);
}

int write_by_index(FILE *fout, string_index_t *index)
{
    if (fout == NULL)
        LERR_RETURN_MINUS_ONE(LERR_FILE_IO, "file pointer is null");

    for (size_t i = 0; i < index->str_cnt; i++)
    {
        if (fputs(index->string_entries[i].begin, fout) == EOF)
            LERR_RETURN_MINUS_ONE(LERR_FILE_IO, "fputs failed");
        if (fputc('\n', fout) == EOF)
            LERR_RETURN_MINUS_ONE(LERR_FILE_IO, "fputc failed");
    }
    // перевод строки в конце
    if (fputc('\n', fout) == EOF)
        LERR_RETURN_MINUS_ONE(LERR_FILE_IO, "fputc failed");
    return 0;
}

string_index_t* copy_index(string_index_t *src)
{
    string_index_t *copy = calloc(1, sizeof(string_index_t));
    if (copy == NULL)
        LERR_RETURN_NULL(LERR_MALLOC, "unable to allocate memory");
    copy->buf = NULL; // у копии нет собственного буфера
    copy->str_cnt = src->str_cnt;
    copy->string_entries = calloc(src->str_cnt, sizeof(string_entry_t));
    if (copy->string_entries == NULL)
    {
        free(copy);
        LERR_RETURN_NULL(LERR_MALLOC, "unable to allocate memory");
    }
    memcpy(copy->string_entries, src->string_entries, src->str_cnt * sizeof(string_entry_t));
    return copy;
}

/**
 * в случае ошибки вернет нулевой указатель
 * не записывает указатель на buf!
*/
static string_index_t *create_index(char *buf, size_t buf_size, size_t str_cnt)
{
    // выделяем память
    string_entry_t *entry_arr = calloc(str_cnt, sizeof(string_entry_t));
    if (entry_arr == NULL)
        LERR_RETURN_NULL(LERR_MALLOC, "unable to allocate memory");
    size_t idx_cnt = 0;
    // первый и последний символы - частные случаи
    entry_arr[idx_cnt].begin = buf;
    for (size_t i = 0; i < buf_size - 1; i++)
    {
        if (buf[i] == '\0')
        {
            entry_arr[idx_cnt].end = buf + i;
            idx_cnt++;
            entry_arr[idx_cnt].begin = buf + i + 1;
        }
    }
    entry_arr[idx_cnt].end = buf + buf_size - 1;

    string_index_t *index = calloc(1, sizeof(string_index_t));
    if (index == NULL)
    {
        free(entry_arr);
        LERR_RETURN_NULL(LERR_MALLOC, "unable to allocate memory");
    }
        
    index->string_entries = entry_arr;
    index->str_cnt = str_cnt;
    return index;
}

/**
 * \brief выдает нуль-терминированную строку с пробелом в начале и гарантированным \n в конце
 * 
 * в случае ошибки вернет нулевой указатель
*/
static char* create_buffer_from_file(const char* file_name, size_t *str_size_ptr)
{
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
        LERR_RETURN_NULL(LERR_FILE_IO, "file pointer is null");
    int data_size = -1;
    if ((data_size = get_file_size(file)) == -1)
    {
        // не мы сгенерировали ошибку, так что информация уже записана
        fclose(file);
        return NULL;
    }

    // +2 для пробела, возможного \n в конце и нуль-терминатора
    char* buf = calloc(data_size + 3, sizeof(char));
    if (buf == NULL)
    {
        LERR(LERR_MALLOC, "unable to allocate memory");
        fclose(file);
        return NULL;
    }

    // читаем файл напрямую в буфер (оставляем место под пробел)
    if (fread(buf + 1, sizeof(char), data_size, file) != data_size)
    {
        LERR(LERR_FILE_IO, "fread failed");
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
        LERR(LERR_FILE_IO, "fclose failed");
        free(buf);
        fclose(file);
        return NULL;
    }
    *str_size_ptr = data_size;
    return buf;
}

/**
 * устанавливает указатель чтения в начало!
 * в случае ошибки возвращает -1
*/
static int get_file_size(FILE *fd)
{
    if (fd == NULL)
        LERR_RETURN_MINUS_ONE(LERR_FILE_IO, "file pointer is null");
    
    // определяем размер файла
    if (fseek(fd, 0, SEEK_END) != 0)
        LERR_RETURN_MINUS_ONE(LERR_FILE_IO, "fseek failed");

    int file_size = ftell(fd);
    if (file_size == -1)
        LERR_RETURN_MINUS_ONE(LERR_FILE_IO, "ftell failed");
    if (fseek(fd, 0, SEEK_SET) != 0)
        LERR_RETURN_MINUS_ONE(LERR_FILE_IO, "fseek failed");
    return file_size;
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