#include <cstdlib>
#include <cstdbool>
#include <cstdio>
#include <cstring>

#include "../lstatus/lstatus.hpp"

/**
 * "Iterator" is the index in the physical buffer
 *  It's wrapped into struct to hide internals from the user in public interface
 *  (especially to avoid confusion between the physical index in the buffer
 *  and the logical position of the element in the list)
 *  Anyway, internals uses unwrapped int because of simplicity
 */
typedef struct
{
    int value;
} list_iter_t;

/**
 * List data structure
 * Fast insertions in any place of sequence, but slow random access
 */

template<typename T>
struct list_node_t
{
    T   data;
    int next;
    int prev;
};

template<typename T>
struct list_t
{
    int  canary1;

    list_node_t<T> *buffer;

    int  head;
    int  tail;
    int  head_free;

    int  buffer_size;
    int  used_size;
    
    bool linear;

    int  canary2;
};

/**
 * Initializes a list instance
 *
 * \param \c list List pointer
 * \param \c capacity Initial capacity
 */
template<typename T>
lstatus_t list_construct(list_t<T> *list, int capacity);

/**
 * Deinitializes a list instance
 *
 * \param \c list List pointer
 */
template<typename T>
lstatus_t list_destruct(list_t<T> *list);

/**
 * Inserts an element to the front of the list
 *
 * \param \c list List pointer
 * \param \c elem Element to insert
 */
template<typename T>
lstatus_t list_push_front(list_t<T> *list, T elem);

/**
 * Initializes a list instance
 *
 * \param \c list List pointer
 * \param \c capacity Initial capacity
 */
template<typename T>
lstatus_t list_push_back(list_t<T> *list, T elem);

/**
 * Removes an element from the front of the list
 *
 * \param \c list List pointer
 */
template<typename T>
lstatus_t list_pop_front(list_t<T> *list);

/**
 * Removes an element from the back of the list
 *
 * \param \c list List pointer
 */
template<typename T>
lstatus_t list_pop_back(list_t<T> *list);

/**
 * Tells a size of the list
 *
 * \param \c list List pointer
 * \param \c out_size Where to write the size
 */
template<typename T>
lstatus_t list_size(list_t<T> *list, int *out_size);

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
template<typename T>
lstatus_t list_next(list_t<T> *list, list_iter_t *iter);

/**
 * Moves the iterator to the previous element
 *
 * \param \c list List pointer
 * \param \c iter Iterator to move
 */
template<typename T>
lstatus_t list_prev(list_t<T> *list, list_iter_t *iter);

/**
 * Provides an iterator to the beginning of the list
 *
 * \param \c list List pointer
 * \param \c iter Where to write the iterator
 */
template<typename T>
lstatus_t list_begin(list_t<T> *list, list_iter_t *iter);

/**
 * Provides an iterator to the end of the list
 *
 * \param \c list List pointer
 * \param \c iter Where to write the iterator
 */
template<typename T>
lstatus_t list_end(list_t<T> *list, list_iter_t *iter);

/**
 * Returns true if the iterators are equal
 * 
 * \param \c iter1 First iterator
 * \param \c iter2 Second iterator
 */
bool list_iter_cmp(list_iter_t iter1, list_iter_t iter2);

/**
 * Provides access to element by iterator
 *
 * \param \c list List pointer
 * \param \c iter Iterator to the element
 * \param \c elem Where to write a pointer to the element
 */
template<typename T>
lstatus_t list_data(list_t<T> *list, list_iter_t iter, T **elem);

/**
 * Inserts an element after the element pointed to by the iterator
 *
 * \param \c list List pointer
 * \param \c iter Iterator
 * \param \c elem Element to insert
 */
template<typename T>
lstatus_t list_insert_after(list_t<T> *list, list_iter_t iter, T elem);

/**
 * Inserts an element before the element pointed to by the iterator
 *
 * \param \c list List pointer
 * \param \c iter Iterator
 * \param \c elem Element to insert
 */
template<typename T>
lstatus_t list_insert_before(list_t<T> *list, list_iter_t iter, T elem);

/**
 * Removes the element by the iterator
 * Note that the iterator becomes invalid in case of LSTATUS_OK
 *
 * \param \c list List pointer
 * \param \c iter Iterator
 */
template<typename T>
lstatus_t list_remove(list_t<T> *list, list_iter_t iter);

/**
 * Slow method, but after reordering list_iter_lookup is not linear
 * Also push_front and pop_front don't break that order
 *
 * \param \c list List pointer
 */
template<typename T>
lstatus_t list_linearize(list_t<T> *list);

/**
 * Returns iterator to element from it's position in the list
 * Slow if list isn't linearized
 * 
 * \param \c list List pointer
 * \param \c position Position to lookup
 * \param \c iter_out Where to write the iterator
 */
template<typename T>
lstatus_t list_iter_lookup(list_t<T> *list, int position, list_iter_t *iter_out);

/**
 * Checks health of the list
 * The list is invalid if return value isn't LSTATUS_OK
 * 
 * \param \c list List pointer
 */
template<typename T>
lstatus_t list_validate(list_t<T> *list);

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




//----------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------

//#define MAX_CMD_LINE_LENGTH 100

//static int POISON = 0xDEAD;
static int CANARY = 0xBEEF;

template<typename T>
static lstatus_t list_expand(list_t<T> *list);

template<typename T>
static lstatus_t free_cell(list_t<T> *list, int iter);

template<typename T>
static lstatus_t claim_cell(list_t<T> *list, int *claimed_cell_iter);

//static int       validator_proxy(list_t *list);

#define LIST_DATA(iter)  list->buffer[iter].data
#define LIST_NEXT(iter)  list->buffer[iter].next
#define LIST_PREV(iter)  list->buffer[iter].prev

#define ITER_WRAP(value) (list_iter_t){value}

#ifdef DEBUG
#define LIST_CHECK(dict) LSCHK(list_validate(list))
#else
#define LIST_CHECK(dict)
#endif

#define LIST_CHECK_ITER(iter)          \
{                                      \
    if (iter.value == 0)               \
        return LSTATUS_LIST_NULL_ITER; \
}

template<typename T>
lstatus_t list_construct(list_t<T> *list, int capacity)
{
    lstatus_t status = LSTATUS_OK;

    // first element in the buffer is fictive
    list->buffer_size = capacity + 1;
    list->used_size   = 0;

    list->buffer = (list_node_t<T>*)calloc(list->buffer_size, sizeof(list_node_t<T>));
    if (list->buffer == NULL)
    {
        free(list->buffer);
        LSTATUS(LSTATUS_BAD_ALLOC, "");
        return status;
    }

    //for (int i = 0; i < list->buffer_size; i++)
    //    LIST_DATA(i) = POISON;

    list->head      = 0;
    list->tail      = 0;
    list->head_free = 0;

    if (capacity > 0)
    {
        // initially, all cells are free
        // so unite them into chain of free cells
        // we want only to claim/free cell, so singly-linked list is enough
        // head_free at the beginning and reverse order to maintain linearity when push_front / pop_front

        for (int i = 1; i < list->buffer_size - 1; i++)
            LIST_PREV(i) = i + 1;

        LIST_PREV(list->buffer_size - 1) = 0;

        list->head_free = 1;
    }
    
    list->linear = true;

    list->canary1 = CANARY;
    list->canary2 = CANARY;

    LIST_CHECK(list);

    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_push_front(list_t<T> *list, T elem)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    int claimed_cell = 0;
    status = claim_cell(list, &claimed_cell);
    if (status != LSTATUS_OK)
    {
        status = list_expand(list);
        if (status != LSTATUS_OK)
            return status;

        status = claim_cell(list, &claimed_cell);
        if (status != LSTATUS_OK)
            return status;
    }
    
    LIST_DATA(claimed_cell) = elem;
    LIST_PREV(claimed_cell) = list->head;
    LIST_NEXT(claimed_cell) = 0;

    if (list->head != 0)
        LIST_NEXT(list->head) = claimed_cell;
    
    list->head = claimed_cell;
    
    // if list was empty, set tail too
    if (list->tail == 0)
        list->tail = claimed_cell;

    list->used_size++;

    LIST_CHECK(list);
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_push_back(list_t<T> *list, T elem)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    int claimed_cell = 0;
    status = claim_cell(list, &claimed_cell);
    if (status != LSTATUS_OK)
    {
        status = list_expand(list);
        if (status != LSTATUS_OK)
            return status;

        status = claim_cell(list, &claimed_cell);
        if (status != LSTATUS_OK)
            return status;
    }

    list->linear = false;

    LIST_DATA(claimed_cell) = elem;
    LIST_NEXT(claimed_cell) = list->tail;
    LIST_PREV(claimed_cell) = 0;

    if (list->tail != 0)
        LIST_PREV(list->tail) = claimed_cell;

    list->tail = claimed_cell;

    // if list was empty, set head too
    if (list->head == 0)
        list->head = claimed_cell;
    
    list->used_size++;

    LIST_CHECK(list);
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_pop_front(list_t<T> *list)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    if (list->head == 0)
        return LSTATUS_LIST_EMPTY;

    //LIST_DATA(list->head) = POISON;

    int to_remove = list->head;
    list->head = LIST_PREV(list->head);
    LIST_NEXT(list->head) = 0;

    // if last element was removed, set tail too
    if (list->head == 0)
        list->tail = 0;

    status = free_cell(list, to_remove);
    if (status != LSTATUS_OK)
        return status;

    list->used_size--;

    LIST_CHECK(list);
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_pop_back(list_t<T> *list)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    if (list->tail == 0)
        return LSTATUS_LIST_EMPTY;

    list->linear = false;

    LIST_DATA(list->tail) = 0;
    int new_tail = LIST_NEXT(list->tail);
    status = free_cell(list, list->tail);
    if (status != LSTATUS_OK)
        return status;

    LIST_PREV(new_tail) = 0;
    list->tail = new_tail;

    // if last element was removed, set head too
    if (list->tail == 0)
        list->head = 0;

    list->used_size--;

    LIST_CHECK(list);
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_insert_after(list_t<T> *list, list_iter_t iter, T elem)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);
    LIST_CHECK_ITER(iter);

    if (iter.value == list->head)
        return list_push_front(list, elem);

    int claimed_cell = 0;
    status = claim_cell(list, &claimed_cell);
    if (status != LSTATUS_OK)
    {
        status = list_expand(list);
        if (status != LSTATUS_OK)
            return status;

        status = claim_cell(list, &claimed_cell);
        if (status != LSTATUS_OK)
            return status;
    }
    
    list->linear = false;

    LIST_DATA(claimed_cell)          = elem;
    LIST_PREV(claimed_cell)          = iter.value;
    LIST_NEXT(claimed_cell)          = LIST_NEXT(iter.value);
    LIST_PREV(LIST_NEXT(iter.value)) = claimed_cell;
    LIST_NEXT(iter.value)            = claimed_cell;

    list->used_size++;

    LIST_CHECK(list);

    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_insert_before(list_t<T> *list, list_iter_t iter, T elem)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);
    LIST_CHECK_ITER(iter);

    if (iter.value == list->tail)
        return list_push_back(list, elem);

    status = list_prev(list, &iter);
    if (status != LSTATUS_OK)
        return status;

    return list_insert_after(list, iter, elem);
}

template<typename T>
lstatus_t list_remove(list_t<T> *list, list_iter_t iter)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);
    LIST_CHECK_ITER(iter);

    if (iter.value == list->head)
        return list_pop_front(list);

    if (iter.value == list->tail)
        return list_pop_back(list);

    list->linear = false;

    int left         = LIST_PREV(iter.value);
    int right        = LIST_NEXT(iter.value);
    LIST_NEXT(left)  = right;
    LIST_PREV(right) = left;

    list->used_size--;

    status = free_cell(list, iter.value);
    if (status != LSTATUS_OK)
        return status;

    LIST_CHECK(list);
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_linearize(list_t<T> *list)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    int size = 0;
    status = list_size(list, &size);
    if (status != LSTATUS_OK)
        return status;

    list_t<T> new_list = {0};
    if (list_construct(&new_list, size) != LSTATUS_OK)
        return status;

    for (int iter = list->tail; iter != 0; iter = LIST_NEXT(iter))
    {
        if ((status = list_push_front(&new_list, LIST_DATA(iter))) != LSTATUS_OK)
            return status;
    }

    LIST_CHECK(&new_list);

    status = list_destruct(list);
    if (status != LSTATUS_OK)
    {
        // status ommited
        list_destruct(&new_list);
        return status;
    }

    *list = new_list;
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_size(list_t<T> *list, int *out_size)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    *out_size = list->used_size;

    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_data(list_t<T> *list, list_iter_t iter, T **elem)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    if (iter.value >= list->buffer_size)
    {
        LSTATUS(LSTATUS_LIST_BAD_ADDRESS, "iterator out of bounds");
        return status;
    }
    
    *elem = &LIST_DATA(iter.value);
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_iter_lookup(list_t<T> *list, int position, list_iter_t *iter_out)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    if (list->linear)
    {
        *iter_out = ITER_WRAP(position + 1);
        return LSTATUS_OK;
    }

    int iter = list->tail;
    for (int i = 0; i < position; i++)
    {
        iter = LIST_NEXT(iter);
        if (iter == 0)
        {
            LSTATUS(LSTATUS_LIST_BAD_ADDRESS, "position out of range");
            return status;
        }
    }

    *iter_out = ITER_WRAP(iter);
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_next(list_t<T> *list, list_iter_t *iter)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    if (iter->value >= list->buffer_size)
    {
        LSTATUS(LSTATUS_LIST_BAD_ADDRESS, "iterator out of bounds");
        return status;
    }

    *iter = ITER_WRAP(LIST_NEXT(iter->value));
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_prev(list_t<T> *list, list_iter_t *iter)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    if (iter->value >= list->buffer_size)
    {
        LSTATUS(LSTATUS_LIST_BAD_ADDRESS, "iterator out of bounds");
        return status;
    }

    *iter = ITER_WRAP(LIST_PREV(iter->value));
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_begin(list_t<T> *list, list_iter_t *iter)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    *iter = ITER_WRAP(list->tail);
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_end(list_t<T> *list, list_iter_t *iter)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    *iter = NULLITER;
    return LSTATUS_OK;
}

template<typename T>
lstatus_t list_destruct(list_t<T> *list)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    free(list->buffer);

    return LSTATUS_OK;
}

template<typename T>
static lstatus_t list_expand(list_t<T> *list)
{
    lstatus_t status = LSTATUS_OK;
    LIST_CHECK(list);

    list_node_t<T> *new_buffer = (list_node_t<T>*)realloc(list->buffer, sizeof(list_node_t<T>) * list->buffer_size * 2);
    if (new_buffer == NULL)
    {
        LSTATUS(LSTATUS_BAD_ALLOC, "");
        return status;
    }
    
    list->buffer = new_buffer;

    memset(list->buffer + list->buffer_size, 0, list->buffer_size * sizeof(list_node_t<T>));

    //for (int i = list->buffer_size; i < list->buffer_size * 2; i++)
    //    LIST_DATA(i) = POISON;
        
    for (int i = list->buffer_size; i < list->buffer_size * 2 - 1; i++)
        LIST_PREV(i) = i + 1;
    LIST_PREV(list->buffer_size * 2 - 1) = 0;

    // if there were no free cells, then simply set head of free blocks
    // else the last cell of free blocks is always last element of buffer
    if (list->head_free == 0)
        list->head_free = list->buffer_size;
    else
        LIST_PREV(list->buffer_size - 1) = list->buffer_size;

    list->buffer_size *= 2;

    LIST_CHECK(list);
    return LSTATUS_OK;
}

template<typename T>
static lstatus_t free_cell(list_t<T> *list, int iter)
{
    // there is no LIST_CHECK, since it's an internal routine so list can be invalid here

    LIST_PREV(iter)            = list->head_free;
    list->head_free            = iter;
    //LIST_DATA(list->head_free) = POISON;

    return LSTATUS_OK;
}

template<typename T>
static lstatus_t claim_cell(list_t<T> *list, int *claimed_cell_iter)
{
    // there is no LIST_CHECK, since it's an internal routine so list can be invalid here

    if (list->head_free == 0)
    {
        *claimed_cell_iter = 0;
        return LSTATUS_LIST_FULL;
    }

    *claimed_cell_iter = list->head_free;
    list->head_free = LIST_PREV(list->head_free);

    return LSTATUS_OK;
}

#define CHECK_COND(cond, msg, ...)                          \
{                                                           \
    if (!(cond))                                            \
    {                                                       \
        LSTATUS(LSTATUS_LIST_INVALID, #msg, ##__VA_ARGS__); \
        return status;                                      \
    }                                                       \
}

#define CHECK_POISON(iter) \
    CHECK_COND(LIST_DATA(iter) == POISON, "Expected poison at cell %d", iter)

template<typename T>
lstatus_t list_validate(list_t<T> *list)
{
    lstatus_t status = LSTATUS_OK;

// basic checks
    CHECK_COND(list->canary1 == CANARY, "canary1 fault");
    CHECK_COND(list->canary2 == CANARY, "canary2 fault");

    //CHECK_POISON(0);

    CHECK_COND(LIST_NEXT(list->head) == 0, "next[head] is not zero");
    CHECK_COND(LIST_PREV(list->tail) == 0, "prev[tail] is not zero");
    CHECK_COND(LIST_NEXT(0)          == 0, "next[0] is not zero");
    CHECK_COND(LIST_PREV(0)          == 0, "prev[0] is not zero");

// connectivity check

#ifdef DEBUG
    int used_cnt  = 0;
    int prev_iter = 0;

    for (int cur_iter = list->tail; cur_iter != 0; cur_iter = LIST_NEXT(cur_iter))
    {
        if (cur_iter != list->tail && cur_iter != list->head)
        {
            CHECK_COND(prev_iter == LIST_PREV(cur_iter) && cur_iter == LIST_NEXT(prev_iter),
                       "prev-next misconnect between %d and %d: connectivity seems to be broken",
                       prev_iter, cur_iter);
        }
        
        used_cnt++;
        CHECK_COND(used_cnt < list->buffer_size,
                   "Too many real elements: connectivity seems to be broken\n");

        prev_iter = cur_iter;
    }
    
    // check size
    CHECK_COND(list->used_size == used_cnt, "Used elements count mismatch");

    int free_cnt  = 0;
        prev_iter = 0;

    for (int cur_iter = list->head_free; cur_iter != 0; cur_iter = LIST_PREV(cur_iter))
    {
        //CHECK_POISON(cur_iter);

        free_cnt++;
        CHECK_COND(free_cnt < list->buffer_size,
                   "Too many free elements: connectivity seems to be broken");

        prev_iter = cur_iter;
    }

    CHECK_COND(used_cnt + free_cnt + 1 == list->buffer_size, "Element count mismatch");
#endif

    return LSTATUS_OK;
}

// ///////////////////////////////////////////
// /// Debugging stuff                     ///
// ///////////////////////////////////////////

// #define LIST_CHECK_COND list_validate(list) == LIST_OK

// #define HEAD_LABEL_ID      10000
// #define TAIL_LABEL_ID      10001
// #define HEAD_FREE_LABEL_ID 10002

// static void emit_node(FILE* dot_file, list_t *list, size_t iter)
// {
//     if (LIST_DATA(iter) == POISON)
//         fprintf(dot_file, "{ %zu [label=\"&#9762;\" xlabel=%zu] };\n", iter, iter);
//     else
//         fprintf(dot_file, "{ %zu [label=%d xlabel=%zu] };\n", iter, LIST_DATA(iter), iter);
// }

// void list_visualize_phys(list_t *list, const char *img_file_name)
// {
//     FILE *dot_file = fopen("list.dot", "w");

//     fprintf(dot_file, "digraph\n"
//                       "{\n"
//                       "node [shape=box]; rankdir=LR;\n");

//     for (size_t i = 0; i < list->buffer_size; i++)
//         emit_node(dot_file, list, i);

//     fprintf(dot_file, "{ %d [label=\"tail\"] };\n"     , TAIL_LABEL_ID);
//     fprintf(dot_file, "{ %d [label=\"head\"] };\n"     , HEAD_LABEL_ID);
//     fprintf(dot_file, "{ %d [label=\"head_free\"] };\n", HEAD_FREE_LABEL_ID);

//     for (size_t i = 0; i < list->buffer_size - 1; i++)
//         fprintf(dot_file, "{ edge[style=invis, weight=1000] %zu->%zu };\n", i, i + 1);

//     for (size_t i = 0; i < list->buffer_size; i++)
//     {
//         fprintf(dot_file,
//                 "{ edge[constraint=false, arrowhead=vee, color=\"#%02X%02X%02X\"]  %zu->%zu };\n",
//                 rand() % 128 + 64, rand() % 128 + 64, rand() % 128 + 64, i, LIST_NEXT(i));
//         fprintf(dot_file,
//                 "{ edge[constraint=false, arrowhead=crow, color=\"#%02X%02X%02X\"] %zu->%zu };\n",
//                 rand() % 128 + 64, rand() % 128 + 64, rand() % 128 + 64, i, LIST_PREV(i));
//     }

//     fprintf(dot_file, "{ edge[color=darkgreen]  %d->%zu };\n", HEAD_LABEL_ID     , list->head);
//     fprintf(dot_file, "{ edge[color=darkgreen]  %d->%zu };\n", TAIL_LABEL_ID     , list->tail);
//     fprintf(dot_file, "{ edge[color=darkgreen]  %d->%zu };\n", HEAD_FREE_LABEL_ID, list->head_free);

//     fprintf(dot_file, "}\n");
//     fclose(dot_file);

//     char cmd_buf[MAX_CMD_LINE_LENGTH + 1];
//     snprintf(cmd_buf, MAX_CMD_LINE_LENGTH, "dot list.dot -Tpng > %s", img_file_name);
//     system(cmd_buf);
// }

// list_status_t list_visualize_fancy(list_t *list, const char *img_file_name)
// {
//     // we don't use LIST_CHECK macro because dump function depends on this function
//     if (!(LIST_CHECK_COND))
//         return LIST_ERROR;

//     FILE *dot_file = fopen("list.dot", "w");

//     fprintf(dot_file, "digraph G\n"
//                       "{\n"
//                       "node [shape=box];\n");

//     emit_node(dot_file, list, 0);

//     // unite used cells
//     fprintf(dot_file, "{rank=same\n;");
    
//     for (size_t iter = list->tail; iter != 0; iter = LIST_NEXT(iter))
//     {
//         emit_node(dot_file, list, iter);
//         fprintf(dot_file, "{edge[color=tomato2]     %zu->%zu}\n", iter, LIST_NEXT(iter));
//         fprintf(dot_file, "{edge[color=dodgerblue2] %zu->%zu}\n", iter, LIST_PREV(iter));
//     }

//     fprintf(dot_file, "}\n;");

//     // unite free cells
//     fprintf(dot_file, "{rank=same\n;");
//     for (size_t iter = list->head_free; iter != 0; iter = LIST_PREV(iter))
//     {
//         emit_node(dot_file, list, iter);
//         fprintf(dot_file, "{edge[color=dodgerblue2] %zu->%zu}\n", iter, LIST_PREV(iter));
//     }
//     fprintf(dot_file, "}\n;");

//     fprintf(dot_file, "{%d [label=\"head\"];}\n"     , HEAD_LABEL_ID);
//     fprintf(dot_file, "{%d [label=\"tail\"];}\n"     , TAIL_LABEL_ID);
//     fprintf(dot_file, "{%d [label=\"head_free\"];}\n", HEAD_FREE_LABEL_ID);

//     fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_LABEL_ID     , list->head);
//     fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", TAIL_LABEL_ID     , list->tail);
//     fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_FREE_LABEL_ID, list->head_free);
//     fprintf(dot_file, "}\n");
//     fclose(dot_file);

//     char cmd_buf[MAX_CMD_LINE_LENGTH + 1] = {0};
//     snprintf(cmd_buf, MAX_CMD_LINE_LENGTH, "dot list.dot -Tpng > %s", img_file_name);
//     system(cmd_buf);

//     return LIST_OK;
// }

// int list_html_dump(list_t *list)
// {
//     FILE *html_file = fopen("log.html", "a");

//     int dump_id = rand();

//     fprintf(html_file, "<hr><pre>\n"
//                        "List dump No. %d:\n"
//                        "Validation result: %s\n",
//                        dump_id, list_validate(list) == LIST_OK ? "OK" : __lerr_str);

//     fprintf(html_file, "data: ");
//     for (size_t i = 0; i < list->buffer_size; i++)
//     {
//         if (LIST_DATA(i) == POISON)
//             fprintf(html_file, " poison ");
//         else
//             fprintf(html_file, "%7d ", LIST_DATA(i));
//     }

//     fprintf(html_file, "\nnext: ");
//     for (size_t i = 0; i < list->buffer_size; i++)
//         fprintf(html_file, "%7zu ", LIST_NEXT(i));

//     fprintf(html_file, "\nprev: ");
//     for (size_t i = 0; i < list->buffer_size; i++)
//         fprintf(html_file, "%7zu ", LIST_PREV(i));

//     fprintf(html_file, "\n\n"
//                        "head:      %4zu\n"
//                        "tail:      %4zu\n"
//                        "head_free: %4zu\n"
//                        "linear:    %4d\n\n",
//                         list->head, list->tail, list->head_free, list->linear);
    
//     char img_file_name[MAX_CMD_LINE_LENGTH + 1];

//     snprintf(img_file_name, MAX_CMD_LINE_LENGTH, "images/fancy_img_%d.png", dump_id);
//     if (list_visualize_fancy(list, img_file_name) == LIST_OK)
//     {
//         fprintf(html_file, "Fancy visualization:\n"
//                            "<img src=\"%s\" width=\"40%%\">\n",
//                             img_file_name);
//     }
//     else
//         fprintf(html_file, "Fancy visualization can't be displayed because of validation fail\n");

//     snprintf(img_file_name, MAX_CMD_LINE_LENGTH, "images/phys_img_%d.png", dump_id);
//     list_visualize_phys(list, img_file_name);
//     fprintf(html_file, "Physical visualization:\n"
//                        "<img src=\"%s\" width=\"60%%\">\n",
//                         img_file_name);

//     fprintf(html_file, "</pre>\n");
//     fclose(html_file);

//     return dump_id;
// }

// static int validator_proxy(list_t *list)
// {
//     if (!(LIST_CHECK_COND))
//     {   
//         int dump_id = list_html_dump(list); 
//         printf("List validation failed: %s\n"
//                "See more in log.html\n"
//                "Dump id: %d\n",
//                 __lerr_str, dump_id);
//         return -1;
//     }

//     return 0;
// }