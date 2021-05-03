#include <cstdlib>
#include <cstdio>

#include "lstatus.hpp"
#include "file_utils.hpp"

#define ERROR_HANDLER() goto error_handler

static lstatus_t get_file_size(FILE *file, int *size_out);

lstatus_t create_buffer_from_file(const char* file_name, char **buffer_out)
{
    lstatus_t status = LSTATUS_OK;

    FILE *file      = nullptr;
    char *buf       = nullptr; 
    int   data_size = 0;

    file = fopen(file_name, "r");
    if (file == nullptr)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "can't open file %s", file_name);
        ERROR_HANDLER();
    }

    status = get_file_size(file, &data_size);
    if (status != LSTATUS_OK)
    {
        fclose(file);
        ERROR_HANDLER();
    }

    // +1 for \0
    buf = (char*)calloc(data_size + 1, sizeof(char));
    if (buf == nullptr)
    {
        LSTATUS(LSTATUS_BAD_ALLOC, "");
        fclose(file);
        ERROR_HANDLER();
    }

    if (fread(buf, sizeof(char), data_size, file) != data_size)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "reading failed");
        fclose(file);
        ERROR_HANDLER();
    }

    *buffer_out = buf;
    fclose(file);
    return LSTATUS_OK;

error_handler:
    free(buf);
    return status;
}

static lstatus_t get_file_size(FILE *file, int *size_out)
{
    lstatus_t status = LSTATUS_OK;
    int file_size = 0;

    if (fseek(file, 0, SEEK_END) != 0)
        ERROR_HANDLER();

    file_size = ftell(file);
    if (file_size == -1)
        ERROR_HANDLER();
    if (fseek(file, 0, SEEK_SET) != 0)
        ERROR_HANDLER();

    *size_out = file_size;
    return LSTATUS_OK;

error_handler:
    LSTATUS(LSTATUS_FILE_IO_ERR, "file access failed");
    return status;
}