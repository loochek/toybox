#include "list.h"
#include <stdio.h>
#include <time.h>

int main()
{
    srand(time(NULL));

    list_t list;
    list_construct(&list, 5);
    
    list_push_back(&list, 1);
    list_html_dump(&list);

    list_push_front(&list, 2);
    list_html_dump(&list);

    list_push_back(&list, 3);
    list_html_dump(&list);

    list_push_front(&list, 4);
    list_html_dump(&list);

    list_push_back(&list, 5);
    list_html_dump(&list);

    list_push_front(&list, 6);
    list_html_dump(&list);

    list_linearize(&list);
    list_html_dump(&list);

    list_destruct(&list);
    return 0;
}