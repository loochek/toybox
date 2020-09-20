#include <stdlib.h>
#include <stdio.h>

/** \file */

/**
 * устанавливает указатель чтения в начало!
 * в случае ошибки возвращает -1
*/
int get_file_size(FILE *fd);

/**
 * \brief выдает нуль-терминированную строку с пробелом в начале и гарантированным \n в конце
 * 
 * в случае ошибки вернет нулевой указатель
*/
char* create_string_from_file(const char* file_name, size_t *str_size_ptr);