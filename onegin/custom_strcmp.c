#include "custom_strcmp.h"

// так как библиотечная не может в русские буквы
static int is_alpha(unsigned char c)
{
    return ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 192 && c <= 223) ||
        (c >= 224 && c <= 255));
}

int custom_strcmp(const char *a, const char *b)
{
    // сравниваем строки a и b без учета знаков препинания и пробелов
    // иначе говоря, рассмариваем только буквы a..z A..Z а..я А...Я

    while (*a != '\0' && *b != '\0')
    {
        // игнорируем не-буквы
        while (!is_alpha(*a) && *a != '\0')
            a++;
        while (!is_alpha(*b) && *b != '\0')
            b++;
        if (*a == 0 || *b == 0)
            break;

        if (*a > *b)
            return 1;
        else if (*a < *b)
            return -1;
        else
        {
            a++;
            b++;
        }
        
    }
    if (*a != '\0' && *b == '\0')
        return 1;
    else if (*a == '\0' && *b != '\0')
        return -1;
    else
        return 0;
}

// перевернутый компаратор
// просто ++ заменены на --, так как наша организация памяти позволяет
// (строки нуль-терминированы с обоих сторон)
// также помним, что указатели указывают на нуль-терминатор
int custom_strcmp_rev(const char *a, const char *b)
{
    a--; b--;
    while (*a != '\0' && *b != '\0')
    {
        // игнорируем не-буквы
        while (!is_alpha(*a) && *a != '\0')
            a--;
        while (!is_alpha(*b) && *b != '\0')
            b--;
        if (*a == 0 || *b == 0)
            break;

        if (*a > *b)
            return 1;
        else if (*a < *b)
            return -1;
        else
        {
            a--;
            b--;
        }
    }
    if (*a != '\0' && *b == '\0')
        return 1;
    else if (*a == '\0' && *b != '\0')
        return -1;
    else
        return 0;
}