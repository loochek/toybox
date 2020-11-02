#include <stdlib.h>

typedef int elem_t;

typedef struct
{
    elem_t *data;
    size_t *next;
    size_t *prev;

    size_t  head;
    size_t  tail;

    size_t  head_free;
    size_t  arr_size;
} list_t;

typedef enum
{
    LIST_OK,
    LIST_EMPTY,
    LIST_ERROR,
    LIST_OUT_OF_BOUNDS
} list_status_t;

list_status_t list_construct(list_t *list, size_t capacity);
list_status_t list_push_front(list_t *list, elem_t elem);
list_status_t list_push_back(list_t *list, elem_t elem);
list_status_t list_pop_front(list_t *list);
list_status_t list_pop_back(list_t *list);
list_status_t list_insert(list_t *list, size_t index, elem_t elem);
list_status_t list_remove(list_t *list, size_t index);
elem_t *list_at(list_t *list, size_t index);
void list_print(list_t *list);
void list_destruct(list_t *list);
void list_visualise(list_t *list);