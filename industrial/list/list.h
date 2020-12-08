#include <stdlib.h>
#include <stdbool.h>

typedef int elem_t;

/**
 * List data structure
 * Fast insertions in any place of sequence, but slow random access
 */

/**
 * "Iterator" is the index in the physical buffer
 *  It's wrapped into struct to hide internals from the user in public interface
 *  (especially to avoid confusion between the physical index in the buffer
 *  and the logical position of the element in the list)
 *  Anyway, internals uses unwrapped size_t because of simplicity
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

/**
 * LERR-affecting
 */
list_status_t list_construct(list_t *list, size_t capacity);

/**
 * LERR-affecting
 */
void list_destruct(list_t *list);

/**
 * LERR-affecting
 */
list_status_t list_push_front(list_t *list, elem_t elem);

/**
 * LERR-affecting
 */
list_status_t list_push_back(list_t *list, elem_t elem);

/**
 * LERR-affecting
 */
list_status_t list_pop_front(list_t *list);

/**
 * LERR-affecting
 */
list_status_t list_pop_back(list_t *list);

/**
 * LERR-affecting
 * returns -1 if failed
 */
int list_size(list_t *list);

/**
 * As null pointer, but for list iterators
 */
#define NULLITER (list_iter_t){0}

/**
 * LERR-affecting
 */
list_iter_t list_next(list_t *list, list_iter_t iter);

/**
 * LERR-affecting
 */
list_iter_t list_prev(list_t *list, list_iter_t iter);

/**
 * LERR-affecting
 */
list_iter_t list_begin(list_t *list);

list_iter_t list_end     (list_t *list);
bool        list_iter_cmp(list_iter_t iter1, list_iter_t iter2);

/**
 * LERR-affecting
 * return NULL if error
 */
elem_t *list_data(list_t *list, list_iter_t iter);

/**
 * LERR-affecting
 */
list_status_t list_insert_after(list_t *list, list_iter_t iter, elem_t elem);

/**
 * LERR-affecting
 */
list_status_t list_insert_before(list_t *list, list_iter_t iter, elem_t elem);

/**
 * LERR-affecting
 * Notice that iter is not valid in case of LIST_OK
 */
list_status_t list_remove(list_t *list, list_iter_t iter);

/**
 * LERR-affecting
 * Slow method, but after reordering list_iter_lookup is not linear
 * Also push_front and pop_front don't break that order
 */
list_status_t list_linearize(list_t *list);

/**
 * LERR-affecting
 * Returns iterator to element from it's position in the list
 * Slow if list isn't linearized
 */
list_iter_t list_iter_lookup(list_t *list, size_t position);

/**
 * Debugging method
 * Shows physical layout of the list
 */
void list_visualize_phys(list_t *list, const char *img_file_name);

/**
 * LERR-affecting
 * Debugging method
 * Shows logical layout of the list
 */
list_status_t list_visualize_fancy(list_t *list, const char *img_file_name);

/**
 * Debugging method
 * returns Dump ID
 */
int list_html_dump(list_t *list);