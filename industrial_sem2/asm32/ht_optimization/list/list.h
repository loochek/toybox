#include <stdlib.h>
#include <stdbool.h>

#include "../lstatus/lstatus.h"

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
    int value;
} list_iter_t;

typedef struct
{
    elem_t data;
    int    next;
    int    prev;
} list_node_t;

typedef struct
{
    int          canary1;

    list_node_t *buffer;

    int          head;
    int          tail;
    int          head_free;

    int          buffer_size;
    int          used_size;
    
    bool         linear;

    int          canary2;
} list_t;

/**
 * Initializes a list instance
 *
 * \param \c list List pointer
 * \param \c capacity Initial capacity
 */
lstatus_t list_construct(list_t *list, int capacity);

/**
 * Deinitializes a list instance
 *
 * \param \c list List pointer
 */
lstatus_t list_destruct(list_t *list);

/**
 * Inserts an element to the front of the list
 *
 * \param \c list List pointer
 * \param \c elem Element to insert
 */
lstatus_t list_push_front(list_t *list, elem_t elem);

/**
 * Initializes a list instance
 *
 * \param \c list List pointer
 * \param \c capacity Initial capacity
 */
lstatus_t list_push_back(list_t *list, elem_t elem);

/**
 * Removes an element from the front of the list
 *
 * \param \c list List pointer
 */
lstatus_t list_pop_front(list_t *list);

/**
 * Removes an element from the back of the list
 *
 * \param \c list List pointer
 */
lstatus_t list_pop_back(list_t *list);

/**
 * Tells a size of the list
 *
 * \param \c list List pointer
 * \param \c out_size Where to write the size
 */
lstatus_t list_size(list_t *list, int *out_size);

/**
 * As null pointer, but for list iterators
 */
#define NULLITER (list_iter_t){0}

/**
 * Moves the iterator to the next element
 *
 * \param \c list List pointer
 * \param \c iter Iterator to move
 */
lstatus_t list_next(list_t *list, list_iter_t *iter);

/**
 * Moves the iterator to the previous element
 *
 * \param \c list List pointer
 * \param \c iter Iterator to move
 */
lstatus_t list_prev(list_t *list, list_iter_t *iter);

/**
 * Provides an iterator to the beginning of the list
 *
 * \param \c list List pointer
 * \param \c iter Where to write the iterator
 */
lstatus_t list_begin(list_t *list, list_iter_t *iter);

/**
 * Provides an iterator to the end of the list
 *
 * \param \c list List pointer
 * \param \c iter Where to write the iterator
 */
lstatus_t list_end(list_t *list, list_iter_t *iter);

/**
 * Provides access to element by iterator
 *
 * \param \c list List pointer
 * \param \c iter Iterator to the element
 * \param \c elem Where to write a pointer to the element
 */
lstatus_t list_data(list_t *list, list_iter_t iter, elem_t **elem);

/**
 * Inserts an element after the element pointed to by the iterator
 *
 * \param \c list List pointer
 * \param \c iter Iterator
 * \param \c elem Element to insert
 */
lstatus_t list_insert_after(list_t *list, list_iter_t iter, elem_t elem);

/**
 * Inserts an element before the element pointed to by the iterator
 *
 * \param \c list List pointer
 * \param \c iter Iterator
 * \param \c elem Element to insert
 */
lstatus_t list_insert_before(list_t *list, list_iter_t iter, elem_t elem);

/**
 * Removes the element by the iterator
 * Note that the iterator becomes invalid in case of LSTATUS_OK
 *
 * \param \c list List pointer
 * \param \c iter Iterator
 */
lstatus_t list_remove(list_t *list, list_iter_t iter);

/**
 * Slow method, but after reordering list_iter_lookup is not linear
 * Also push_front and pop_front don't break that order
 *
 * \param \c list List pointer
 */
lstatus_t list_linearize(list_t *list);

/**
 * Returns iterator to element from it's position in the list
 * Slow if list isn't linearized
 * 
 * \param \c list List pointer
 * \param \c position Position to lookup
 * \param \c iter_out Where to write the iterator
 */
lstatus_t list_iter_lookup(list_t *list, int position, list_iter_t *iter_out);

/**
 * Returns true if the iterators are equal
 * 
 * \param \c iter1 First iterator
 * \param \c iter2 Second iterator
 */
bool list_iter_cmp(list_iter_t iter1, list_iter_t iter2);

// TODO: review debug tools

// /**
//  * Debugging method
//  * Shows physical layout of the list
//  */
// void list_visualize_phys(list_t *list, const char *img_file_name);

// /**
//  * LERR-affecting
//  * Debugging method
//  * Shows logical layout of the list
//  */
// list_status_t list_visualize_fancy(list_t *list, const char *img_file_name);

// /**
//  * Debugging method
//  * returns Dump ID
//  */
// int list_html_dump(list_t *list);