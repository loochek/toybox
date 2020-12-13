#include <string.h>

#include "string_view.h"

bool strview_equ(const string_view_t *str1, const string_view_t *str2)
{
    if (str1->length != str2->length)
        return false;
    
    if (strncmp(str1->str, str2->str, str1->length) != 0)
        return false;
    
    return true;
}

bool strview_c_equ(const string_view_t *str, const char *c_str)
{
    size_t c_str_len = strlen(c_str);

    if (str->length != c_str_len)
        return false;
    
    if (strncmp(str->str, c_str, str->length) != 0)
        return false;
    
    return true;
}