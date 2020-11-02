#include "list.h"
#include <stdio.h>

int main()
{
        list_t list;
    list_construct(&list, 10);
    list_visualise(&list);
    
    list_push_front(&list, 1);
    list_visualise(&list);
    list_push_back (&list, 2);
    list_visualise(&list);

    list_push_front(&list, 3);
    list_visualise(&list);
    list_push_back (&list, 4);
    list_visualise(&list);

    list_push_front(&list, 5);
    list_visualise(&list);
    list_push_back (&list, 6);
    list_visualise(&list);

    list_push_front(&list, 7);
    list_visualise(&list);
    list_push_back (&list, 8);
    list_visualise(&list);

    list_push_front(&list, 9);
    list_visualise(&list);
    list_push_back (&list, 10);
    list_visualise(&list);
    return 0;
}