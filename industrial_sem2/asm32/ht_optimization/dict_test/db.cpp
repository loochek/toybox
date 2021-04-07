#include "db.hpp"

#define ERROR_HANDLER() goto error_handler

/**
 * Creates buffer from file
 * 
 * \param \c file_name    File name
 * \param \c buffer_out   Where to write pointer to the allocated buffer
 * \param \c str_size_ptr Where to write size of the data
 */
static lstatus_t create_buffer_from_file(const char* file_name, char **buffer_out, int *data_size_ptr);

/**
 * Tells file size. 
 * Moves file pointer to the beginning!
 * 
 * \param \c fd       File pointer
 * \param \c size_out Whete to write size
*/
static lstatus_t get_file_size(FILE *file, int *size_out);

lstatus_t db_load(const char *file_name, dict_t *dict, char **buffer_out)
{
    lstatus_t status = LSTATUS_OK;

    char *buffer = nullptr;
    int data_size = 0;
    LSCHK(create_buffer_from_file(file_name, &buffer, &data_size));

    char *curr_key_begin   = buffer;
    char *curr_value_begin = nullptr;

    for (int pos = 0; pos < data_size; pos++)
    {
        if (buffer[pos] == ':')
        {
            buffer[pos] = '\0';
            curr_value_begin = &buffer[pos] + 1;
        }
        else if (buffer[pos] == '\n')
        {
            buffer[pos] = '\0';

            status = dict_insert(dict, curr_key_begin, curr_value_begin);
            if (status != LSTATUS_OK)
            {
                free(buffer);
                return status;
            }

            curr_key_begin = &buffer[pos] + 1;
        }
    }

    *buffer_out = buffer;
    return LSTATUS_OK;
}

static lstatus_t create_buffer_from_file(const char* file_name, char **buffer_out, int *data_size_ptr)
{
    lstatus_t status = LSTATUS_OK;

    FILE *file      = nullptr;
    char *buf       = nullptr; 
    int   data_size = 0;

    file = fopen(file_name, "r");
    if (file == nullptr)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "file pointer is null");
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

    // читаем файл напрямую в буфер (оставляем место под пробел)
    if (fread(buf, sizeof(char), data_size, file) != data_size)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "reading failed");
        fclose(file);
        ERROR_HANDLER();
    }

    *data_size_ptr = data_size;
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
    LSTATUS(LSTATUS_FILE_IO_ERR, "unable to seek file");
    return status;
}