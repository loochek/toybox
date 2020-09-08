#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// чтобы не использовать malloc лишний раз, выделим память заранее в сегменте данных
// (ИМХО сложно его использовать, не зная размер входных данных)
// (так что, надеюсь, за такое выделение меня не побьют        )
// 10000 строк по 100 символов хватит
char mem[10000][100];
// еще будем хранить указатели на строки - их и будем сортировать
char* strings[10000];

// компаратор строк. Обратите внимание, что раз сортить мы будем указатели,
// то qsort будет нам передавать указатели на указатели на строки
int comp(const void *a, const void *b)
{
    // разыменовываем УнУнС просто в указатель на строку
    return strcmp(*(const char**)a, *(const char**)b);
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
    do_unit_tests();
    FILE *fd = fopen("onegin.txt", "r");
    // счетчик строк
    int str_cnt = 0;
    // считываем строки пока они есть и записываем указатели на них в strings
    while (fgets(mem[str_cnt], 100, fd) != NULL)
    {
        strings[str_cnt] = mem[str_cnt];
        str_cnt++;
    }
    fclose(fd);
    // еще раз: сортим указатели!
    qsort(strings, str_cnt, sizeof(char*), comp);

    // выводим в файл
    FILE *fdout = fopen("onegin_parsed.txt", "w");
    for (int i = 0; i < str_cnt; i++)
        fputs(strings[i], fdout);
    fclose(fdout);
    return 0;
}