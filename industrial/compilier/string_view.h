#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stdlib.h>
#include <stdbool.h>
/**
 * Simple proxy to use not null terminated strings
 */

typedef struct
{
    const char *str;
    size_t      length;
} string_view_t;

bool strview_equ  (const string_view_t *str1, const string_view_t *str2);
bool strview_c_equ(const string_view_t *str , const char *c_str);

#endif