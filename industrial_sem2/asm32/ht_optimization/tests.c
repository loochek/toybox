#include "list.h"
#include <assert.h>
#include <stdio.h>

void check(list_t *list, int correct_arr[], size_t correct_size)
{
    size_t cnt = 0;
    for (list_iter_t i = list_begin(list); !list_iter_cmp(i, list_end(list)); i = list_next(list, i))
    {
        assert(*list_data(list, i) == correct_arr[cnt]);
        cnt++;
    }
    assert(cnt             == correct_size);
    assert(list_size(list) == correct_size);
}

void test_basic()
{
    list_t list;
    assert(list_construct(&list, 5) == LIST_OK);

    for (int i = 1; i <= 10; i++)
        assert(list_push_front(&list, i) == LIST_OK);

    int correct1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    check(&list, correct1, 10);

    for (int i = 1; i <= 10; i++)
        assert(list_pop_front(&list) == LIST_OK);

    assert(list_size(&list) == 0);
    assert(list.linear == true);

    for (int i = 1; i <= 10; i++)
    {
        assert(list_push_front(&list, i * 2 - 1) == LIST_OK);
        assert(list_push_back (&list, i * 2    ) == LIST_OK);
    }

    int correct2[] = {20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    check(&list, correct2, 20);

    for (int i = 0; i < 10; i++)
        assert(list_pop_back(&list) == LIST_OK);

    int correct3[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    check(&list, correct3, 10);

    for (int i = 0; i < 10; i++)
        assert(list_pop_front(&list) == LIST_OK);

    assert(list_size(&list) == 0);
    assert(list_pop_front(&list) == LIST_EMPTY);
    assert(list_pop_back (&list) == LIST_EMPTY);

    list_destruct(&list);
}

void test_advanced()
{
    list_t list;
    assert(list_construct(&list, 5) == LIST_OK);

    for (int i = 1; i <= 5; i++)
    {
        assert(list_push_front(&list, i * 2 - 1) == LIST_OK);
        assert(list_push_back (&list, i * 2    ) == LIST_OK);
    }

    int cnt = 1;
    for (list_iter_t i = list_begin(&list); !list_iter_cmp(i, list_end(&list));
         i = list_next(&list, list_next(&list, i)))
    {
        assert(list_insert_after(&list, i, cnt) == LIST_OK);
        cnt++;
    }

    int correct1[] = {10, 1, 8, 2, 6, 3, 4, 4, 2, 5, 1, 6, 3, 7, 5, 8, 7, 9, 9, 10};
    check(&list, correct1, 20);
    
    for (list_iter_t i = list_next(&list, list_begin(&list)); !list_iter_cmp(i, list_end(&list));)
    {
        list_iter_t old_i = i;
        i = list_next(&list, list_next(&list, i));
        assert(list_remove(&list, old_i) == LIST_OK);
        if (list_iter_cmp(i, list_end(&list)))
            break;
    }

    int correct2[] = {10, 8, 6, 4, 2, 1, 3, 5, 7, 9};
    check(&list, correct2, 10);

    assert(list_linearize(&list) == LIST_OK);

    assert(list_push_front(&list, 101) == LIST_OK);
    assert(list_pop_front(&list) == LIST_OK);
    assert(list_size(&list) == 10);
    assert(list.linear == true);

    list_iter_t iter = list_iter_lookup(&list, 4);
    assert(!list_iter_cmp(iter, NULLITER));
    assert(list_insert_after (&list, iter, 103) == LIST_OK);
    assert(list.linear == false);
    assert(list_insert_before(&list, iter, 101) == LIST_OK);

    int correct3[] = {10, 8, 6, 4, 101, 2, 103, 1, 3, 5, 7, 9};
    check(&list, correct3, 12);

    list_iter_t iter103 = list_next(&list, iter);
    list_iter_t iter101 = list_prev(&list, iter);
    assert(!list_iter_cmp(iter101, NULLITER) && !list_iter_cmp(iter103, NULLITER));
    assert(list_remove(&list, iter101) == LIST_OK);
    assert(list_remove(&list, iter103) == LIST_OK);

    check(&list, correct2, 10);

    for (int i = 0; i < 10; i++)
        assert(list_pop_back(&list) == LIST_OK);

    assert(list_size(&list) == 0);

    list_destruct(&list);
}

int main()
{
    test_basic();
    test_advanced();
    printf("All tests passed!\n");
    return 0;
}