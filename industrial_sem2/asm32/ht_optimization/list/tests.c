#include "list.h"
#include <assert.h>
#include <stdio.h>

#define LASS(expr) { assert((expr) == LSTATUS_OK); }

void check(list_t *list, int correct_arr[], int correct_size)
{
    int cnt = 0;
    list_iter_t iter     = NULLITER;
    list_iter_t end_iter = NULLITER;
    LASS(list_begin(list, &iter));
    LASS(list_end  (list, &end_iter));

    for (; !list_iter_cmp(iter, end_iter); list_next(list, &iter))
    {
        int *elem = NULL;
        LASS(list_data(list, iter, &elem));
        assert(*elem == correct_arr[cnt]);
        cnt++;
    }
    assert(cnt == correct_size);

    int lsize = 0;
    LASS(list_size(list, &lsize));
    assert(lsize == correct_size);
}

void test_basic()
{
    int lsize = 0;

    list_t list;
    LASS(list_construct(&list, 5));

    for (int i = 1; i <= 10; i++)
        LASS(list_push_front(&list, i));

    int correct1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    check(&list, correct1, 10);

    for (int i = 1; i <= 10; i++)
        LASS(list_pop_front(&list));

    LASS(list_size(&list, &lsize));
    assert(lsize == 0);
    assert(list.linear == true);

    for (int i = 1; i <= 10; i++)
    {
        LASS(list_push_front(&list, i * 2 - 1));
        LASS(list_push_back (&list, i * 2    ));
    }

    int correct2[] = {20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    check(&list, correct2, 20);

    for (int i = 0; i < 10; i++)
        LASS(list_pop_back(&list));

    int correct3[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    check(&list, correct3, 10);

    for (int i = 0; i < 10; i++)
        LASS(list_pop_front(&list));

    LASS(list_size(&list, &lsize));
    assert(lsize == 0);
    assert(list_pop_front(&list) == LSTATUS_LIST_EMPTY);
    assert(list_pop_back (&list) == LSTATUS_LIST_EMPTY);

    LASS(list_destruct(&list));
}

void test_advanced()
{
    list_iter_t iter     = NULLITER;
    list_iter_t end_iter = NULLITER;
    int         lsize = 0;
    
    list_t list;
    LASS(list_construct(&list, 5));

    for (int i = 1; i <= 5; i++)
    {
        LASS(list_push_front(&list, i * 2 - 1));
        LASS(list_push_back (&list, i * 2    ));
    }

    int cnt = 1;
    LASS(list_begin(&list, &iter));
    LASS(list_end  (&list, &end_iter));

    for (; !list_iter_cmp(iter, end_iter); list_next(&list, &iter), list_next(&list, &iter))
    {
        LASS(list_insert_after(&list, iter, cnt));
        cnt++;
    }

    int correct1[] = {10, 1, 8, 2, 6, 3, 4, 4, 2, 5, 1, 6, 3, 7, 5, 8, 7, 9, 9, 10};
    check(&list, correct1, 20);
    
    LASS(list_begin(&list, &iter));
    LASS(list_next (&list, &iter));
    LASS(list_end  (&list, &end_iter));

    for (; !list_iter_cmp(iter, end_iter);)
    {
        list_iter_t old_iter = iter;
        LASS(list_next(&list, &iter));
        LASS(list_next(&list, &iter));
        LASS(list_remove(&list, old_iter));
        if (list_iter_cmp(iter, end_iter))
            break;
    }

    int correct2[] = {10, 8, 6, 4, 2, 1, 3, 5, 7, 9};
    check(&list, correct2, 10);

    LASS(list_linearize(&list));

    LASS(list_push_front(&list, 101));
    LASS(list_pop_front(&list));
    LASS(list_size(&list, &lsize));
    assert(lsize == 10);
    assert(list.linear == true);

    LASS(list_iter_lookup(&list, 4, &iter));
    assert(!list_iter_cmp(iter, NULLITER));
    LASS(list_insert_after(&list, iter, 103));
    assert(list.linear == false);
    LASS(list_insert_before(&list, iter, 101));

    int correct3[] = {10, 8, 6, 4, 101, 2, 103, 1, 3, 5, 7, 9};
    check(&list, correct3, 12);

    list_iter_t iter103 = iter, iter101 = iter;
    LASS(list_next(&list, &iter103));
    LASS(list_prev(&list, &iter101));
    assert(!list_iter_cmp(iter101, NULLITER) && !list_iter_cmp(iter103, NULLITER));
    LASS(list_remove(&list, iter101));
    LASS(list_remove(&list, iter103));

    check(&list, correct2, 10);

    for (int i = 0; i < 10; i++)
        LASS(list_pop_back(&list));

    LASS(list_size(&list, &lsize));
    assert(lsize == 0);

    LASS(list_destruct(&list));
}

int main()
{
    test_basic();
    test_advanced();
    printf("All tests passed!\n");
    return 0;
}