#include "list.h"
#include <assert.h>
#include <stdio.h>

void test_basic()
{
    list_t list;
    assert(list_construct(&list, 5) == LIST_OK);

    for (int i = 1; i <= 10; i++)
        assert(list_push_front(&list, i) == LIST_OK);

    int correct1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int i = 0; i < 10; i++)
        assert(*list_linear_access(&list, i) == correct1[i]);

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
    for (int i = 0; i < 20; i++)
        assert(*list_linear_access(&list, i) == correct2[i]);

    for (int i = 0; i < 10; i++)
        assert(list_pop_back(&list) == LIST_OK);

    int correct3[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    for (int i = 0; i < 10; i++)
        assert(*list_linear_access(&list, i) == correct3[i]);

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
    for (int i = 0; i < 20; i += 2)
    {
        assert(list_insert_after(&list, list_logic_to_phys(&list, i), cnt) == LIST_OK);
        cnt++;
    }

    int correct1[] = {10, 1, 8, 2, 6, 3, 4, 4, 2, 5, 1, 6, 3, 7, 5, 8, 7, 9, 9, 10};
    for (int i = 0; i < 20; i++)
        assert(*list_linear_access(&list, i) == correct1[i]);
    
    for (int i = 1; i <= 10; i++)
        assert(list_remove(&list, list_logic_to_phys(&list, i)) == LIST_OK);

    int correct2[] = {10, 8, 6, 4, 2, 1, 3, 5, 7, 9};
    for (int i = 0; i < 10; i++)
        assert(*list_linear_access(&list, i) == correct2[i]);

    assert(list_size(&list) == 10);

    assert(list_linearize(&list) == LIST_OK);

    assert(list_push_front(&list, 101) == LIST_OK);
    assert(list_pop_front(&list) == LIST_OK);
    assert(list_size(&list) == 10);
    assert(list.linear == true);
    assert(list_pop_back(&list) == LIST_OK);
    assert(list.linear == false);

    for (int i = 0; i < 9; i++)
        assert(list_remove(&list, list_logic_to_phys(&list, 0)) == LIST_OK);

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