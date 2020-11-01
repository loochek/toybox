#include "list.h"
#include <stdio.h>

int main()
{
    list_t list;
    list_construct(&list, 10);
    
    list_push_front(&list, 1);
    list_push_back (&list, 2);

    list_push_front(&list, 3);
    list_push_back (&list, 4);

    list_push_front(&list, 5);
    list_push_back (&list, 6);

    list_push_front(&list, 7);
    list_push_back (&list, 8);

    list_push_front(&list, 9);
    list_push_back (&list, 10);

    list_insert(&list, 4, 100);
    list_remove(&list, 5);

    for (int i = 0; i < 10; i++)
        printf("%d ", *list_at(&list, i));
    printf("\n");

    list_print(&list);
    return 0;
}