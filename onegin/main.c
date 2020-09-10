#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "custom_strcmp.h"

/* ввод вывод обернуть в функции done
свой strcmp done
malloc done
*/

#define MAX_LENGTH 100
#define MAX_STRINGS_COUNT 10000

// Будем хранить указатели на строки - их и будем сортировать
// Сами строки будем хранить в памяти, выделяемой malloc'ом
char* strings[MAX_STRINGS_COUNT];

// компаратор строк. Обратите внимание, что раз сортить мы будем указатели,
// то qsort будет нам передавать указатели на указатели на строки
int comp(const void *f, const void *s)
{
    // разыменовываем УнУнС просто в указатели на строку
    return custom_strcmp(*(const unsigned char**)f, *(const unsigned char**)s);
}

// считывает строки из файла
// результат: str_cnt указателей на строки в strings
// не забудьте освободить эту память!
int read_input()
{
    FILE *fd = fopen("onegin.txt", "r");
    // счетчик строк
    int str_cnt = 0;
    // выделяем память под первую строку
    char *str_ptr = malloc(sizeof(char) * MAX_LENGTH);
    // считываем строки и записываем указатели на них в strings
    while (str_cnt < MAX_STRINGS_COUNT && fgets(str_ptr, MAX_LENGTH, fd) != NULL)
    {
        strings[str_cnt] = str_ptr;
        // выделяем память под очередную строку
        str_ptr = malloc(sizeof(char) * MAX_LENGTH);
        str_cnt++;
    }
    // освобождаем последнюю неиспользованную память
    // (издержки реализации)
    // (зато ничего не копируем)
    free(str_ptr);
    fclose(fd);

    // возвращаем количество считанных строк
    return str_cnt;
}

// пишет строки из strings в файл
void write_output(int str_cnt)
{
    // выводим первые str_cnt строк из strings
    FILE *fdout = fopen("onegin_parsed.txt", "w");
    for (int i = 0; i < str_cnt; i++)
        fputs(strings[i], fdout);
    fclose(fdout);
}

// освобождает память
void cleanup(int str_cnt)
{
    for (int i = 0; i < str_cnt; i++)
        free(strings[i]);
}

#ifndef TEST
int main()
#else
int dummy()
#endif
{
    // читаем файл
    int str_cnt = read_input();
    // еще раз: сортим указатели!
    qsort(strings, str_cnt, sizeof(char*), comp);
    // выводим
    write_output(str_cnt);
    // освобождаем память
    cleanup(str_cnt);
    return 0;
}