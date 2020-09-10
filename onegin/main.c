#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "custom_strcmp.h"

/* ввод вывод обернуть в функции done
свой strcmp done
malloc done
*/

#define MAX_STRINGS_COUNT 10000
//const int MAX_STRINGS_COUNT = 10000;

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
    // в худшем случае нам нужно sizeof(char) * (file_size + 1) памяти
    // память для \0 обеспечена \n'ами, для последней строки делаем на всякий случай +1
    int file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // выделяем все одним блоком - строки будем хранить последовательно
    char *mem = malloc(sizeof(char) * (file_size + 1));
    mem_start = mem;
    strings[0] = mem;

    // счетчик строк
    int str_cnt = 0; 
    
    // считываем посимвольно
    int c;
    while (str_cnt < MAX_STRINGS_COUNT && (c = fgetc(fd)) != EOF)
    {
        if (c == '\n')
        {
            *mem = '\0';
            mem++;
            str_cnt++;
            strings[str_cnt] = mem;
        }
        else
        {
            *mem = c;
            mem++;
        }
    }
    *mem = '\0';
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
        fputs(strings[i], fdout);
        fputc('\n', fdout);
    }
    fclose(fdout);
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
    free(mem_start);
    return 0;
}