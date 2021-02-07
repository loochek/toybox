#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdlib.h>
#include <stdio.h>

/**
 * returns -1 if failed
 * LERR-affecting
 */

int get_file_size(FILE *file);

/**
 * returns -1 if failed, else returns buffer size
 * LERR-affecting
 */
int create_buffer_from_file(const char* file_name, char **buf_ptr);

#endif