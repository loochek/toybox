#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <cstddef>

template<typename CharType>
size_t StrLen(const CharType *str)
{
    size_t size = 0;
    while (str[size] != 0)
        size++;

    return size;
}

#endif
