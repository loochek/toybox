#include <stdlib.h>
#include <stdio.h>

/*
returns -1 if failed
throws LERR
*/
int get_file_size(FILE *file);

/*
returns -1 if failed, else returns buffer size
throws LERR
*/
int create_buffer_from_file(const char* file_name, char **buf_ptr);