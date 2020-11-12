#include "list.h"
#include <stdio.h>

int main()
{
    list_t list;
    list_construct(&list, 5);
    
    list_push_back(&list, 1);
    list_visualise_fancy(&list);
    list_visualise_phys(&list);

    list_push_front(&list, 2);
    list_visualise_fancy(&list);
    list_visualise_phys(&list);

    list_push_back(&list, 3);
    list_visualise_fancy(&list);
    list_visualise_phys(&list);

    list_push_front(&list, 4);
    list_visualise_fancy(&list);
    list_visualise_phys(&list);

    list_push_back(&list, 5);
    list_visualise_fancy(&list);
    list_visualise_phys(&list);

    list_push_front(&list, 6);
    list_visualise_fancy(&list);
    list_visualise_phys(&list);

    list_linearize(&list);
    list_visualise_fancy(&list);
    list_visualise_phys(&list);

    list_destruct(&list);
    return 0;
}