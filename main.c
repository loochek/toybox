#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* ввод вывод обернуть в функции done
свой strcmp
malloc done
*/

#define MAX_LENGTH 100
#define MAX_STRINGS_COUNT 10000

// Будем хранить указатели на строки - их и будем сортировать
// Сами строки будем хранить в памяти, выделяемой malloc'ом
char* strings[MAX_STRINGS_COUNT];

// так как библиотечная не может в русские буквы
int is_alpha(unsigned char c)
{
    return ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 192 && c <= 223) ||
        (c >= 224 && c <= 255));
}

// компаратор строк. Обратите внимание, что раз сортить мы будем указатели,
// то qsort будет нам передавать указатели на указатели на строки
int comp(const void *f, const void *s)
{
    // разыменовываем УнУнС просто в указатели на строку
    const unsigned char *a = *(const char**)f;
    const unsigned char *b = *(const char**)s;

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

        printf("%c %c\n", *a, *b);
        printf("%u %u\n", *a, *b);
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

void unit_test_1()
{
    const char *a = "Alpha";
    const char b[] = "Beta";
    const char *c = b;
    assert(comp(&a, &c) < 0);
    assert(comp(&c, &a) > 0);
}

void unit_test_2()
{
    const char *a = "Alpha";
    const char *b = "Alpha";
    assert(comp(&a, &b) == 0);
}

void unit_test_3()
{
    const char *a = "Charlie";
    const char *b = "Beta";
    assert(comp(&a, &b) > 0);
    assert(comp(&b, &a) < 0);
}

void do_unit_tests()
{
    unit_test_1();
    unit_test_2();
    unit_test_3();
}

int main()
{
#ifdef DO_UNIT_TESTS
    do_unit_tests();
#endif
    // читаем файл5
    int str_cnt = read_input();
    // еще раз: сортим указатели!
    qsort(strings, str_cnt, sizeof(char*), comp);
    // выводим
    write_output(str_cnt);
    // освобождаем память
    cleanup(str_cnt);
    return 0;
}