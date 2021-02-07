#include "lerror.hpp"
#include "file_utils.hpp"

int get_file_size(FILE *file)
{
    LERR_RESET();

    if (file == NULL)
        LERR_RETURN(LERR_FILE_IO, -1, "file pointer is null");
    
    // определяем размер файла
    if (fseek(file, 0, SEEK_END) != 0)
        LERR_RETURN(LERR_FILE_IO, -1, "io function failed");

    int file_size = ftell(file);
    if (file_size == -1)
        LERR_RETURN(LERR_FILE_IO, -1, "io function failed");
    if (fseek(file, 0, SEEK_SET) != 0)
        LERR_RETURN(LERR_FILE_IO, -1, "io function failed");

    return file_size;
}

int create_buffer_from_file(const char* file_name, char **buf_ptr)
{
    LERR_RESET();

    FILE *file = fopen(file_name, "r");
    if (file == NULL)
        LERR_RETURN(LERR_FILE_IO, -1, "file pointer is null");

    int data_size = get_file_size(file);
    if (LERR_PRESENT())
    {
        fclose(file);
        return -1;
    }

    *buf_ptr = (char*)calloc(data_size + 1, sizeof(char));
    if (*buf_ptr == NULL)
    {
        LERR(LERR_ALLOC, "unable to allocate memory");
        fclose(file);
        return -1;
    }

    if (fread(*buf_ptr, sizeof(char), data_size, file) != data_size)
    {
        LERR(LERR_FILE_IO, "io function failed");
        free(*buf_ptr);
        fclose(file);
        return -1;
    }

    fclose(file);
    return data_size;
}