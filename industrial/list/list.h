#include <stdlib.h>
#include <stdbool.h>

typedef int elem_t;

typedef struct
{
    size_t canary1;

    elem_t *data;
    size_t *next;
    size_t *prev;

    size_t  head;
    size_t  tail;

    size_t  head_free;
    size_t  arr_size;
    bool    linear;

    size_t canary2;
} list_t;

typedef enum
{
    LIST_OK,
    LIST_EMPTY,
    LIST_ERROR,
    LIST_OUT_OF_BOUNDS
} list_status_t;

list_status_t  list_construct      (list_t *list, size_t capacity);
list_status_t  list_push_front     (list_t *list, elem_t elem);
list_status_t  list_push_back      (list_t *list, elem_t elem);
list_status_t  list_pop_front      (list_t *list);
list_status_t  list_pop_back       (list_t *list);
list_status_t  list_insert         (list_t *list, size_t index, elem_t elem);
list_status_t  list_remove         (list_t *list, size_t index);
list_status_t  list_linearize      (list_t *list);
int            list_size           (list_t *list);
elem_t        *list_at             (list_t *list, size_t index);
void           list_print          (list_t *list);
void           list_destruct       (list_t *list);
void           list_visualise_phys (list_t *list, const char *img_file_name);
list_status_t  list_visualise_fancy(list_t *list, const char *img_file_name);
int            list_html_dump      (list_t *list);