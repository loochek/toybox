#include <stdlib.h>
#include <stdbool.h>

typedef int    elem_t;

/**
 * "Iterator" is the index in the physical buffer
 *  It's wrapped into struct to hide internals from the user in public interface
 *  (especially to avoid confusion between the physical index in the buffer
 *  and the logical position of the element in the list)
 */
typedef struct
{
    size_t value;
} list_iter_t;

typedef struct
{
    elem_t data;
    size_t next;
    size_t prev;
} list_node_t;

typedef struct
{
    size_t       canary1;

    list_node_t *buffer;

    size_t       head;
    size_t       tail;
    size_t       head_free;

    size_t       buffer_size;
    size_t       used_size;
    
    bool         linear;

    size_t       canary2;
} list_t;

typedef enum
{
    LIST_OK,
    LIST_EMPTY,
    LIST_ERROR
} list_status_t;

// fast methods
// basic
list_status_t  list_construct      (list_t *list, size_t capacity);
void           list_destruct       (list_t *list);
list_status_t  list_push_front     (list_t *list, elem_t elem);
list_status_t  list_push_back      (list_t *list, elem_t elem);
list_status_t  list_pop_front      (list_t *list);
list_status_t  list_pop_back       (list_t *list);
// returns -1 if failed
// 0 is an invalid iterator
int            list_size           (list_t *list);

// iterators

#define NULLITER (list_iter_t){0}

list_iter_t    list_next           (list_t *list, list_iter_t iter);
list_iter_t    list_prev           (list_t *list, list_iter_t iter);
list_iter_t    list_begin          (list_t *list);
list_iter_t    list_end            (list_t *list);
bool           list_iter_cmp       (list_iter_t iter1, list_iter_t iter2);

// iteration methods
elem_t        *list_data           (list_t *list, list_iter_t iter);
list_status_t  list_insert_after   (list_t *list, list_iter_t iter, elem_t elem);
list_status_t  list_insert_before  (list_t *list, list_iter_t iter, elem_t elem);
list_status_t  list_remove         (list_t *list, list_iter_t iter);

// linear methods (slow!)
list_status_t  list_linearize      (list_t *list);
list_iter_t    list_iter_lookup    (list_t *list, size_t position);

// debugging methods
void           list_visualize_phys (list_t *list, const char *img_file_name);
list_status_t  list_visualize_fancy(list_t *list, const char *img_file_name);
// returns Dump ID
int            list_html_dump      (list_t *list);