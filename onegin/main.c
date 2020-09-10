#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "custom_strcmp.h"

// в gcc с константой не компилируется, а с clang'ом не работает отладчик
// в релизе можно будет заменить

//#define MAX_STRINGS_COUNT 10000
const int MAX_STRINGS_COUNT = 10000;

// Будем хранить указатели на начало и конец строк (для перевернутого компаратора) - их и будем сортировать
// Сами строки будем хранить в памяти, выделяемой malloc'ом

typedef struct
{
    char *begin;
    char *end;
}  string_entry_t;

string_entry_t strings[MAX_STRINGS_COUNT];

// компаратор строк
int comp(const void *f, const void *s)
{
    string_entry_t a = *(string_entry_t*)f;
    string_entry_t b = *(string_entry_t*)s;
    return custom_strcmp((const unsigned char*)a.begin, (const unsigned char*)b.begin);
}

// перевернутый компаратор
int comp_rev(const void *f, const void *s)
{
    string_entry_t a = *(string_entry_t*)f;
    string_entry_t b = *(string_entry_t*)s;
    return custom_strcmp_rev((const unsigned char*)a.end, (const unsigned char*)b.end);
}

// указатель на выделенный кусок памяти
static char *mem_start;

// считывает строки из файла
// результат: str_cnt указателей на строки в strings
// не забудьте освободить память по указателю mem_start!
// из за \n'а в конце файла пихает пустую строку, ну и ладно
int read_input()
{
    FILE *fd = fopen("onegin.txt", "r");
 
    // определяем размер файла

    fseek(fd, 0, SEEK_END);
    // в худшем случае нам нужно sizeof(char) * (file_size + 2) памяти
    // память для \0 обеспечена \n'ами, для последней строки делаем на всякий случай +1
    // еще первым символом запихнем \0, тогда можем бежать по строке и в обратную сторону
    // итого +2
    int file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // выделяем все одним блоком - строки будем хранить последовательно
    mem_start = malloc(sizeof(char) * (file_size + 2));
    // этим указателем будем двигаться по памяти
    char *mem = mem_start;
    // тот самый \0
    *mem = '\0';
    mem++;
    // далее уже строки
    strings[0].begin = mem;
    // счетчик строк
    int str_cnt = 0;
    // считываем посимвольно
    int c;
    while (str_cnt < MAX_STRINGS_COUNT && (c = fgetc(fd)) != EOF)
    {
        if (c == '\n')
        {
            *mem = '\0';
            strings[str_cnt].end = mem;
            mem++;
            str_cnt++;
            strings[str_cnt].begin = mem;
        }
        else
        {
            *mem = c;
            mem++;
        }
    }
    *mem = '\0';
    strings[str_cnt].end = mem;
    str_cnt++;
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
    {
        fputs(strings[i].begin, fdout);
        fputc('\n', fdout);
    }
    fclose(fdout);
}

#ifndef TEST
int main(const int argc, const char* argv[])
#else
int dummy()
#endif
{
    // читаем файл
    int str_cnt = read_input();
    // сортируем
    if (argc >= 2 && strcmp(argv[1], "-r"))
        qsort(strings, str_cnt, sizeof(string_entry_t), comp_rev);
    else
        qsort(strings, str_cnt, sizeof(string_entry_t), comp);
    // выводим
    write_output(str_cnt);
    // освобождаем память
    free(mem_start);
    return 0;
}