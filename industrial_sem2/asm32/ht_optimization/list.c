#include <stdio.h>
#include <string.h>

#include "list.h"

#define MAX_CMD_LINE_LENGTH 100

/**
 * Turns on some expensive checks
 * TODO: comment it before releasing
 */
#define LIST_DEBUG_MODE

static elem_t POISON = 0xDEAD;
static elem_t CANARY = 0xBEEF;

static lstatus_t list_expand  (list_t *list);
//static lstatus_t list_validate(list_t *list);
static lstatus_t free_cell    (list_t *list, int iter);
static lstatus_t claim_cell   (list_t *list, int *claimed_cell_iter);
//static int       validator_proxy(list_t *list);

#define LIST_DATA(iter)  list->buffer[iter].data
#define LIST_NEXT(iter)  list->buffer[iter].next
#define LIST_PREV(iter)  list->buffer[iter].prev

#define ITER_WRAP(value) (list_iter_t){value}

// TODO: review
// #define LIST_CHECK(list)             \
// {                                    \
//     if (validator_proxy(list) != 0)  \
//         return LSTATUS_LIST_INVALID; \
// }

#define LIST_CHECK(list)             \
{                                    \
}

#define LIST_CHECK_ITER(iter)          \
{                                      \
    if (iter.value == 0)               \
        return LSTATUS_LIST_NULL_ITER; \
}

lstatus_t list_construct(list_t *list, int capacity)
{
    lstatus_t status = 0;

    // first element in the buffer is fictive
    list->buffer_size = capacity + 1;
    list->used_size   = 0;

    list->buffer = (list_node_t*)calloc(list->buffer_size, sizeof(list_node_t));
    if (list->buffer == NULL)
    {
        free(list->buffer);
        LSTATUS(LSTATUS_ERR_ALLOC, "");
        return status;
    }

    for (int i = 0; i < list->buffer_size; i++)
        LIST_DATA(i) = POISON;

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

lstatus_t list_push_front(list_t *list, elem_t elem)
{
    lstatus_t status = 0;
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

lstatus_t list_push_back(list_t *list, elem_t elem)
{
    lstatus_t status = 0;
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

lstatus_t list_pop_front(list_t *list)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    if (list->head == 0)
        return LSTATUS_LIST_EMPTY;

    LIST_DATA(list->head) = POISON;

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

lstatus_t list_pop_back(list_t *list)
{
    lstatus_t status = 0;
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

lstatus_t list_insert_after(list_t *list, list_iter_t iter, elem_t elem)
{
    lstatus_t status = 0;
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

// lstatus_t list_insert_before(list_t *list, list_iter_t iter, elem_t elem)
// {
//     lstatus_t status = 0;
//     LIST_CHECK(list);
//     LIST_CHECK_ITER(iter);

//     if (iter.value == list->tail)
//         return list_push_back(list, elem);

//     list_iter_t prev_iter = list_prev(list, iter);

//     return list_insert_after(list, prev_iter, elem);
// }

lstatus_t list_remove(list_t *list, list_iter_t iter)
{
    lstatus_t status = 0;
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

lstatus_t list_linearize(list_t *list)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    int size = 0;
    status = list_size(list, &size);
    if (status != LSTATUS_OK)
        return status;

    list_t new_list = {0};
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

lstatus_t list_size(list_t *list, int *out_size)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    *out_size = list->used_size;

    return LSTATUS_OK;
}

lstatus_t list_data(list_t *list, list_iter_t iter, elem_t **elem)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    if (iter.value >= list->buffer_size)
    {
        LSTATUS(LSTATUS_LIST_ADDRESS, "iterator out of bounds");
        return status;
    }
    
    *elem = &LIST_DATA(iter.value);
    return LSTATUS_OK;
}

lstatus_t list_iter_lookup(list_t *list, int position, list_iter_t *iter_out)
{
    lstatus_t status = 0;
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
            LSTATUS(LSTATUS_LIST_ADDRESS, "position out of range");
            return status;
        }
    }

    *iter_out = ITER_WRAP(iter);
    return LSTATUS_OK;
}

lstatus_t list_next(list_t *list, list_iter_t *iter)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    if (iter->value >= list->buffer_size)
    {
        LSTATUS(LSTATUS_LIST_ADDRESS, "iterator out of bounds");
        return status;
    }

    *iter = ITER_WRAP(LIST_NEXT(iter->value));
    return LSTATUS_OK;
}

lstatus_t list_prev(list_t *list, list_iter_t *iter)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    if (iter->value >= list->buffer_size)
    {
        LSTATUS(LSTATUS_LIST_ADDRESS, "iterator out of bounds");
        return status;
    }

    *iter = ITER_WRAP(LIST_PREV(iter->value));
    return LSTATUS_OK;
}

lstatus_t list_begin(list_t *list, list_iter_t *iter)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    *iter = ITER_WRAP(list->tail);
    return LSTATUS_OK;
}

lstatus_t list_end(list_t *list, list_iter_t *iter)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    *iter = NULLITER;
    return LSTATUS_OK;
}


// bool list_iter_cmp(list_iter_t iter1, list_iter_t iter2)
// {
//     return iter1.value == iter2.value;
// }

lstatus_t list_destruct(list_t *list)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    free(list->buffer);

    return LSTATUS_OK;
}

static lstatus_t list_expand(list_t *list)
{
    lstatus_t status = 0;
    LIST_CHECK(list);

    list_node_t *new_buffer = (list_node_t*)realloc(list->buffer, sizeof(list_node_t) * list->buffer_size * 2);
    if (new_buffer == NULL)
    {
        LSTATUS(LSTATUS_ERR_ALLOC, "");
        return status;
    }
    
    list->buffer = new_buffer;

    memset(list->buffer + list->buffer_size, 0, list->buffer_size * sizeof(list_node_t));

    for (int i = list->buffer_size; i < list->buffer_size * 2; i++)
        LIST_DATA(i) = POISON;
        
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

static lstatus_t free_cell(list_t *list, int iter)
{
    LIST_CHECK(list);

    LIST_PREV(iter)            = list->head_free;
    list->head_free            = iter;
    LIST_DATA(list->head_free) = POISON;

    LIST_CHECK(list);

    return LSTATUS_OK;
}

static lstatus_t claim_cell(list_t *list, int *claimed_cell_iter)
{
    LIST_CHECK(list);

    if (list->head_free == 0)
    {
        *claimed_cell_iter = 0;
        return LSTATUS_LIST_FULL;
    }

    *claimed_cell_iter = list->head_free;
    list->head_free = LIST_PREV(list->head_free);

    LIST_CHECK(list);

    return LSTATUS_OK;
}

// TODO: refactor debug tools
// #define CHECK_CONDITION(cond, msg, ...)                  \
//     if (!(cond))                                         \
//     {                                                    \
//         LERR(LERR_LIST_VALIDATION, #msg, ##__VA_ARGS__); \
//         return LIST_ERROR;                               \
//     }

// #define CHECK_POISON(iter) \
//     CHECK_CONDITION(LIST_DATA(iter) == POISON, "Expected poison at cell %zu", iter)

// static list_status_t list_validate(list_t *list)
// {
//     LERR_RESET();

// // basic checks
    
//     CHECK_CONDITION(list->canary1 == CANARY, "canary1 fault");
//     CHECK_CONDITION(list->canary2 == CANARY, "canary2 fault");

//     CHECK_POISON((size_t)0);

//     CHECK_CONDITION(LIST_NEXT(list->head) == 0, "next[head] is not zero");
//     CHECK_CONDITION(LIST_PREV(list->tail) == 0, "prev[tail] is not zero");
//     CHECK_CONDITION(LIST_NEXT(0)          == 0, "next[0] is not zero");
//     CHECK_CONDITION(LIST_PREV(0)          == 0, "prev[0] is not zero");

// // connectivity check

// #ifdef LIST_DEBUG_MODE
//     size_t used_cnt  = 0;
//     size_t prev_iter = 0;

//     for (size_t cur_iter = list->tail; cur_iter != 0; cur_iter = LIST_NEXT(cur_iter))
//     {
//         if (cur_iter != list->tail && cur_iter != list->head)
//         {
//             CHECK_CONDITION(prev_iter == LIST_PREV(cur_iter) && cur_iter == LIST_NEXT(prev_iter),
//                         "prev-next misconnect between %zu and %zu: connectivity seems to be broken",
//                         prev_iter, cur_iter);
//         }
        
//         used_cnt++;
//         CHECK_CONDITION(used_cnt < list->buffer_size,
//                          "Too many real elements: connectivity seems to be broken\n");

//         prev_iter = cur_iter;
//     }
    
//     // check size
//     CHECK_CONDITION(list->used_size == used_cnt, "Used elements count mismatch");

//     size_t free_cnt  = 0;
//            prev_iter = 0;

//     for (size_t cur_iter = list->head_free; cur_iter != 0; cur_iter = LIST_PREV(cur_iter))
//     {
//         CHECK_POISON(cur_iter);

//         free_cnt++;
//         CHECK_CONDITION(free_cnt < list->buffer_size,
//                          "Too many free elements: connectivity seems to be broken");

//         prev_iter = cur_iter;
//     }

//     CHECK_CONDITION(used_cnt + free_cnt + 1 == list->buffer_size, "Element count mismatch");
// #endif

//     return LIST_OK;
// }

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