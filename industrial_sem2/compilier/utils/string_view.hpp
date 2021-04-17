#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

/**
 * A simple proxy for using non-null terminated strings
 */
struct string_view_t
{
    const char *str;
    int length;
};

/**
 * Compares two non-null terminated strings for equality
 * 
 * \param \c str1 First string
 * \param \c str2 Second string
 */
bool strview_equ(const string_view_t *str1, const string_view_t *str2);

/**
 * Compares non-null terminated string and regular C string for equality
 * 
 * \param \c str Non-null terminated string
 * \param \c c_str C string
 */
bool strview_c_equ(const string_view_t *str, const char *c_str);

#endif