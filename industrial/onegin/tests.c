#include <stdio.h>
#include <assert.h>

#include "sort.h"

// английские буквы

static void unit_test_1()
{
    const char *a = "\\\\\\......34153425A2345l3425p55h5a5"; // Alpha
    const char b[] = "2435B3425253e№;:?t234a234ы"; // Beta
    assert(custom_strcmp(a, b, 1) < 0);
    assert(custom_strcmp(b, a, 1) > 0);
}

static void unit_test_2()
{
    const char *a = "////////\\\\\\@#$^&*A34522435l3p3h3a333"; // Alpha
    const char *b = "---\"^^^^^^A^l^p^h^a^"; // Alpha
    assert(custom_strcmp(a, b, 1) == 0);
}

static void unit_test_3()
{
    const char *a = "345C354h354a345r354l345i345e"; // Charlie
    const char *b = "B555555555eta"; // Beta
    assert(custom_strcmp(a, b, 1) > 0);
    assert(custom_strcmp(b, a, 1) < 0);
}

// русские буквы (заданы escape-последовательностями, не знаю как иначе)

static void unit_test_4()
{
    const char *a = "////////////\\\\\\-----1\xc0\1\xeb\1\xfc\1\xf4\1\xe0"; // Альфа 
    const char *b = "----\xc1\xe5\xf2\xe0"; // Бета
    assert(custom_strcmp(a, b, 1) < 0);
    assert(custom_strcmp(b, a, 1) > 0);
}

static void unit_test_5()
{
    // ЛоЛКеК
    const char *a = "2020202020\xcb----\xee$%\"^&\xcb\\324\xca\234\xe5\234\3\xca\2020";
    // ЛоЛКеК
    const char *b = "1111----//////\\\\\\---\xcb---\xee----\xcb---\xca--\xe5--\xca---";
    assert(custom_strcmp(a, b, 1) == 0);
}

int int_cmp(const void *a, const void *b)
{
    return *(int*)a - *(int*)b;
}

// сортировка
static void sort_test(int elem_count, int iterations)
{
    int *arr         = calloc(elem_count, sizeof(int));
    int *arr_trusted = calloc(elem_count, sizeof(int));
    for (int iter_cnt = 0; iter_cnt < iterations; iter_cnt++)
    {
        qsort_custom(arr        , elem_count, sizeof(int), int_cmp);
        qsort       (arr_trusted, elem_count, sizeof(int), int_cmp);
        for (int i = 0; i < elem_count; i++)
        {
            if (arr[i] != arr_trusted[i])
            {
                printf("Incorrect sorting!\nSort function output:\n");
                for (int i = 0; i < elem_count; i++)
                    printf("%d ", arr[i]);
                printf("\nCorrect output:\n");
                for (int i = 0; i < elem_count; i++)
                    printf("%d ", arr_trusted[i]);
                printf("\n");
                assert(0);
            }
        }
    }
    free(arr);
    free(arr_trusted);
}

#ifdef TEST
int main()
#else
int dummy()
#endif
{
    unit_test_1();
    unit_test_2();
    unit_test_3();
    unit_test_4();
    unit_test_5();
    sort_test(1000, 10000);
    printf("Testing done, no errors!\n");
    return 0;
}