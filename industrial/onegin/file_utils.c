#include "file_utils.h"

#include "lerror.h"

/** \file */

int get_file_size(FILE *fd)
{
    if (fd == NULL)
    {
        LERRNO(LERR_FILE_IO);
        LERRSTR("file descriptor is null");
        return -1;
    }

    // определяем размер файла

    if (fseek(fd, 0, SEEK_END) != 0)
    {
        LERRNO(LERR_FILE_IO);
        LERRSTR("fseek failed");
        return -1;
    }

    int file_size = ftell(fd);
    if (file_size == -1)
    {
        LERRNO(LERR_FILE_IO);
        LERRSTR("ftell failed");
        return -1;
    }
    if (fseek(fd, 0, SEEK_SET) != 0)
    {
        LERRNO(LERR_FILE_IO);
        LERRSTR("fseek failed");
        return -1;
    }

    return file_size;
}

char* create_string_from_file(const char* file_name, size_t *str_size_ptr)
{
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        LERRNO(LERR_FILE_IO);
        LERRSTR("file descriptor is null");
        return NULL;
    }

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
        LERRNO(LERR_ALLOC);
        LERRSTR("unable to allocate memory");
        fclose(file);
        return NULL;
    }

    // читаем файл напрямую в буфер (оставляем место под пробел)
    if (fread(buf + 1, sizeof(char), data_size, file) != data_size)
    {
        LERRNO(LERR_FILE_IO);
        LERRSTR("fread failed");
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
        LERRNO(LERR_FILE_IO);
        LERRSTR("fclose failed");
        free(buf);
        fclose(file);
        return NULL;
    }
    *str_size_ptr = data_size;
    return buf;
}