#include <stdio.h>
#include <string.h>

#include "lerror.h"
#include "list.h"

#define MAX_CMD_LINE_LENGTH 100

// comment it before releasing
#define LIST_DEBUG

static elem_t POISON = 0xDEAD;
static elem_t CANARY = 0xBEEF;

static list_status_t list_expand  (list_t *list);
static list_status_t list_validate(list_t *list);
static void          free_cell    (list_t *list, size_t iter);
static size_t        claim_cell   (list_t *list);

#define LIST_DATA(iter) list->buffer[iter].data
#define LIST_NEXT(iter) list->buffer[iter].next
#define LIST_PREV(iter) list->buffer[iter].prev

#define LIST_CHECK_COND list_validate(list) == LIST_OK

#define LIST_CHECK(list, to_ret)           \
    if (!(LIST_CHECK_COND))                    \
    {                                          \
        int dump_id = list_html_dump(list);    \
        printf("List validation failed: %s\n"  \
            "See more in log.html\n"        \
            "Dump id: %d\n",                \
            __lerr_str, dump_id);           \
                                            \
        return to_ret;                         \
    }

#define LIST_CHECK_ITER(iter, to_ret) if (iter.value == 0) return to_ret;

list_status_t list_construct(list_t *list, size_t capacity)
{
    // first element in the buffer is fictive
    list->buffer_size = capacity + 1;
    list->used_size   = 0;

    list->buffer = (list_node_t*)calloc(list->buffer_size, sizeof(list_node_t));

    if (list->buffer == NULL)
    {
        free(list->buffer);
        return LIST_ERROR;
    }

    for (size_t i = 0; i < list->buffer_size; i++)
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

        for (size_t i = 1; i < list->buffer_size - 1; i++)
            LIST_PREV(i) = i + 1;

        LIST_PREV(list->buffer_size - 1) = 0;

        list->head_free = 1;
    }
    
    list->linear = true;

    list->canary1 = CANARY;
    list->canary2 = CANARY;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_push_front(list_t *list, elem_t elem)
{
    LIST_CHECK(list, LIST_ERROR);

    size_t claimed_cell = claim_cell(list);
    if (claimed_cell == 0)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_cell = claim_cell(list);
    }
    
    LIST_DATA(claimed_cell) = elem;
    LIST_PREV(claimed_cell) = list->head;
    LIST_NEXT(claimed_cell) = 0;

    if (list->head != 0)
        LIST_NEXT(list->head)  = claimed_cell;
    
    list->head = claimed_cell;
    
    // if list was empty, set tail too
    if (list->tail == 0)
        list->tail = claimed_cell;

    list->used_size++;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_push_back(list_t *list, elem_t elem)
{
    LIST_CHECK(list, LIST_ERROR);

    size_t claimed_cell = claim_cell(list);
    if (claimed_cell == 0)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_cell = claim_cell(list);
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

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_pop_front(list_t *list)
{
    LIST_CHECK(list, LIST_ERROR);

    if (list->head == 0)
        return LIST_EMPTY;

    LIST_DATA(list->head) = POISON;

    size_t to_remove = list->head;
    list->head = LIST_PREV(list->head);
    LIST_NEXT(list->head) = 0;

    // if last element was removed, set tail too
    if (list->head == 0)
        list->tail = 0;

    free_cell(list, to_remove);

    list->used_size--;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_pop_back(list_t *list)
{
    LIST_CHECK(list, LIST_ERROR);

    if (list->tail == 0)
        return LIST_EMPTY;

    list->linear = false;

    LIST_DATA(list->tail) = 0;
    size_t new_tail = LIST_NEXT(list->tail);
    free_cell(list, list->tail);

    LIST_PREV(new_tail) = 0;
    list->tail = new_tail;

    // if last element was removed, set head too
    if (list->tail == 0)
        list->head = 0;

    list->used_size--;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_insert_after(list_t *list, list_iter_t iter, elem_t elem)
{
    LIST_CHECK(list, LIST_ERROR);
    LIST_CHECK_ITER(iter, LIST_ERROR);

    if (iter.value == list->head)
        return list_push_front(list, elem);

    size_t claimed_cell = claim_cell(list);
    if (claimed_cell == 0)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_cell = claim_cell(list);
    }
    
    list->linear = false;

    LIST_DATA(claimed_cell)          = elem;
    LIST_PREV(claimed_cell)          = iter.value;
    LIST_NEXT(claimed_cell)          = LIST_NEXT(iter.value);
    LIST_PREV(LIST_NEXT(iter.value)) = claimed_cell;
    LIST_NEXT(iter.value)            = claimed_cell;

    list->used_size++;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_insert_before(list_t *list, list_iter_t iter, elem_t elem)
{
    LIST_CHECK(list, LIST_ERROR);
    LIST_CHECK_ITER(iter, LIST_ERROR);

    if (iter.value == list->tail)
        return list_push_back(list, elem);

    list_iter_t prev_iter = list_prev(list, iter);

    return list_insert_after(list, prev_iter, elem);
}

list_status_t list_remove(list_t *list, list_iter_t iter)
{
    LIST_CHECK(list, LIST_ERROR);
    LIST_CHECK_ITER(iter, LIST_ERROR);

    if (iter.value == list->head)
        return list_pop_front(list);

    if (iter.value == list->tail)
        return list_pop_back(list);

    list->linear = false;

    size_t left      = LIST_PREV(iter.value);
    size_t right     = LIST_NEXT(iter.value);
    LIST_NEXT(left)  = right;
    LIST_PREV(right) = left;

    free_cell(list, iter.value);

    list->used_size--;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_linearize(list_t *list)
{
    LIST_CHECK(list, LIST_ERROR);

    int size = list_size(list);
    if (size == -1)
        return LIST_ERROR;

    list_t new_list = {0};
    if (list_construct(&new_list, size) != LIST_OK)
        return LIST_ERROR;

    for (list_iter_t iter = list_begin(list); !list_iter_cmp(iter, list_end(list));
         iter = list_next(list, iter))
    {
        if (list_push_front(&new_list, *list_data(list, iter)) != LIST_OK)
        {
            list_destruct(&new_list);
            return LIST_ERROR;
        }
    }

    LIST_CHECK(&new_list, LIST_ERROR);

    list_destruct(list);
    *list = new_list;

    return LIST_OK;
}

int list_size(list_t *list)
{
    LIST_CHECK(list, -1);

    return list->used_size;
}

elem_t *list_data(list_t *list, list_iter_t iter)
{
    LIST_CHECK(list, NULL);

    if (iter.value >= list->buffer_size)
        return NULL;
    
    return &LIST_DATA(iter.value);
}

list_iter_t list_iter_lookup(list_t *list, size_t position)
{
    LIST_CHECK(list, NULLITER);

    if (list->linear)
        return (list_iter_t){position + 1};

    list_iter_t iter = list_begin(list);
    for (size_t i = 0; i < position; i++)
    {
        iter = list_next(list, iter);
        if (list_iter_cmp(iter, list_end(list)))
            return NULLITER;
    }

    return iter;
}

list_iter_t list_next(list_t *list, list_iter_t iter)
{
    LIST_CHECK(list, NULLITER);

    if (LIST_NEXT(iter.value) == 0)
        return NULLITER;

    return (list_iter_t){LIST_NEXT(iter.value)};
}

list_iter_t list_prev(list_t *list, list_iter_t iter)
{
    LIST_CHECK(list, NULLITER);

    if (LIST_PREV(iter.value) == 0)
        return NULLITER;
        
    return (list_iter_t){LIST_PREV(iter.value)};
}

list_iter_t list_begin(list_t *list)
{
    LIST_CHECK(list, NULLITER);

    return (list_iter_t){list->tail};
}

list_iter_t list_end(list_t *list)
{
    return NULLITER;
}

bool list_iter_cmp(list_iter_t iter1, list_iter_t iter2)
{
    return iter1.value == iter2.value;
}

void list_destruct(list_t *list)
{
    free(list->buffer);
}

#define HEAD_LABEL_ID      10000
#define TAIL_LABEL_ID      10001
#define HEAD_FREE_LABEL_ID 10002

static inline void emit_node(FILE* dot_file, list_t * list, size_t iter)
{
    if (LIST_DATA(iter) == POISON)
        fprintf(dot_file, "%zu [label=\"&#9762;\" xlabel=%zu];\n",
                iter, iter);
    else
        fprintf(dot_file, "%zu [label=%d xlabel=%zu];\n",
                 iter, LIST_DATA(iter), iter);
}

void list_visualize_phys(list_t *list, const char *img_file_name)
{
    FILE *dot_file = fopen("list.dot", "w");

    fprintf(dot_file, "digraph\n"
                      "{\n"
                      "node [shape=box]; rankdir=LR;\n");

    fprintf(dot_file, "{\n");
    for (size_t i = 0; i < list->buffer_size; i++)
        emit_node(dot_file, list, i);
    fprintf(dot_file, "}\n");

    fprintf(dot_file, "{\n");
    fprintf(dot_file, "{%d [label=\"tail\"];}\n"     , TAIL_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"head\"];}\n"     , HEAD_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"head_free\"];}\n", HEAD_FREE_LABEL_ID);
    fprintf(dot_file, "}\n");

    for (size_t i = 0; i < list->buffer_size - 1; i++)
        fprintf(dot_file, "{edge[style=invis, weight=1000] %zu->%zu}\n", i, i + 1);

    for (size_t i = 0; i < list->buffer_size; i++)
    {
        fprintf(dot_file,
                "{edge[constraint=false, arrowhead=vee, color=\"#%02X%02X%02X\"]  %zu->%zu}\n",
                rand() % 128 + 64, rand() % 128 + 64, rand() % 128 + 64, i, LIST_NEXT(i));
        fprintf(dot_file,
                "{edge[constraint=false, arrowhead=crow, color=\"#%02X%02X%02X\"] %zu->%zu}\n",
                rand() % 128 + 64, rand() % 128 + 64, rand() % 128 + 64, i, LIST_PREV(i));
    }

    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_LABEL_ID     , list->head);
    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", TAIL_LABEL_ID     , list->tail);
    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_FREE_LABEL_ID, list->head_free);

    fprintf(dot_file, "}\n");
    fclose(dot_file);

    char cmd_buf[MAX_CMD_LINE_LENGTH + 1];
    snprintf(cmd_buf, MAX_CMD_LINE_LENGTH, "dot list.dot -Tpng > %s", img_file_name);
    system(cmd_buf);
}

list_status_t list_visualize_fancy(list_t *list, const char *img_file_name)
{
    // we don't use LIST_CHECK macro because dump function depends on this function
    if (!(LIST_CHECK_COND))
        return LIST_ERROR;

    FILE *dot_file = fopen("list.dot", "w");

    fprintf(dot_file, "digraph G\n"
                      "{\n"
                      "node [shape=box];\n");

    emit_node(dot_file, list, 0);

    // unite used cells
    fprintf(dot_file, "{rank=same\n;");
    
    for (size_t iter = list->tail; iter != 0; iter = LIST_NEXT(iter))
    {
        emit_node(dot_file, list, iter);
        fprintf(dot_file, "{edge[color=tomato2]     %zu->%zu}\n", iter, LIST_NEXT(iter));
        fprintf(dot_file, "{edge[color=dodgerblue2] %zu->%zu}\n", iter, LIST_PREV(iter));
    }

    fprintf(dot_file, "}\n;");

    // unite free cells
    fprintf(dot_file, "{rank=same\n;");
    for (size_t iter = list->head_free; iter != 0; iter = LIST_PREV(iter))
    {
        emit_node(dot_file, list, iter);
        fprintf(dot_file, "{edge[color=dodgerblue2] %zu->%zu}\n", iter, LIST_PREV(iter));
    }
    fprintf(dot_file, "}\n;");

    fprintf(dot_file, "{%d [label=\"head\"];}\n"     , HEAD_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"tail\"];}\n"     , TAIL_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"head_free\"];}\n", HEAD_FREE_LABEL_ID);

    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_LABEL_ID     , list->head);
    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", TAIL_LABEL_ID     , list->tail);
    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_FREE_LABEL_ID, list->head_free);
    fprintf(dot_file, "}\n");
    fclose(dot_file);

    char cmd_buf[MAX_CMD_LINE_LENGTH + 1] = {0};
    snprintf(cmd_buf, MAX_CMD_LINE_LENGTH, "dot list.dot -Tpng > %s", img_file_name);
    system(cmd_buf);

    return LIST_OK;
}

int list_html_dump(list_t *list)
{
    FILE *html_file = fopen("log.html", "a");

    int dump_id = rand();

    fprintf(html_file, "<hr><pre>\n"
                       "List dump No. %d:\n"
                       "Validation result: %s\n",
                       dump_id, list_validate(list) == LIST_OK ? "OK" : __lerr_str);

    fprintf(html_file, "data: ");
    for (size_t i = 0; i < list->buffer_size; i++)
        fprintf(html_file, "%7d ", LIST_DATA(i));

    fprintf(html_file, "\nnext: ");
    for (size_t i = 0; i < list->buffer_size; i++)
        fprintf(html_file, "%7zu ", LIST_NEXT(i));

    fprintf(html_file, "\nprev: ");
    for (size_t i = 0; i < list->buffer_size; i++)
        fprintf(html_file, "%7zu ", LIST_PREV(i));

    fprintf(html_file, "\n\n"
                       "head:      %4zu\n"
                       "tail:      %4zu\n"
                       "head_free: %4zu\n"
                       "linear:    %4d\n\n",
                        list->head, list->tail, list->head_free, list->linear);
    
    char img_file_name[MAX_CMD_LINE_LENGTH + 1];

    snprintf(img_file_name, MAX_CMD_LINE_LENGTH, "images/fancy_img_%d.png", dump_id);
    if (list_visualize_fancy(list, img_file_name) == LIST_OK)
    {
        fprintf(html_file, "Fancy visualization:\n"
                           "<img src=\"%s\" width=\"40%%\">\n",
                            img_file_name);
    }
    else
        fprintf(html_file, "Fancy visualization can't be displayed because of validation fail\n");

    snprintf(img_file_name, MAX_CMD_LINE_LENGTH, "images/phys_img_%d.png", dump_id);
    list_visualize_phys(list, img_file_name);
    fprintf(html_file, "Physical visualization:\n"
                       "<img src=\"%s\" width=\"60%%\">\n",
                        img_file_name);

    fprintf(html_file, "</pre>\n");
    fclose(html_file);

    return dump_id;
}

static list_status_t list_expand(list_t *list)
{
    LIST_CHECK(list, LIST_ERROR);

    // expand only if no free cells
    if (list->head_free != 0)
        return LIST_OK;

    list_node_t *new_buffer = (list_node_t*)realloc(list->buffer, sizeof(list_node_t) * list->buffer_size * 2);
    if (new_buffer == NULL)
        return LIST_ERROR;
    
    list->buffer = new_buffer;

    memset(list->buffer + list->buffer_size, 0, list->buffer_size * sizeof(list_node_t));

    for (size_t i = list->buffer_size; i < list->buffer_size * 2; i++)
        LIST_DATA(i) = POISON;
        
    for (size_t i = list->buffer_size; i < list->buffer_size * 2 - 1; i++)
        LIST_PREV(i) = i + 1;
    LIST_PREV(list->buffer_size * 2 - 1) = 0;

    list->head_free = list->buffer_size;

    list->buffer_size *= 2;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

static void free_cell(list_t *list, size_t iter)
{
    LIST_PREV(iter)            = list->head_free;
    list->head_free            = iter;
    LIST_DATA(list->head_free) = POISON;
}

static size_t claim_cell(list_t *list)
{
    if (list->head_free == 0)
        return 0;

    size_t claimed_cell = list->head_free;
    list->head_free = LIST_PREV(list->head_free);

    return claimed_cell;
}

#define CHECK_CONDITION(cond, msg, ...)                  \
    if (!(cond))                                         \
    {                                                    \
        LERR(LERR_LIST_VALIDATION, #msg, ##__VA_ARGS__); \
        return LIST_ERROR;                               \
    }

#define CHECK_POISON(iter) \
    CHECK_CONDITION(LIST_DATA(iter) == POISON, "Expected poison at cell %zu", iter)

static list_status_t list_validate(list_t *list)
{
    __lerrno = LERR_NO_ERROR;

// basic checks
    
    CHECK_CONDITION(list->canary1 == CANARY, "canary1 fault");
    CHECK_CONDITION(list->canary2 == CANARY, "canary2 fault");

    CHECK_POISON((size_t)0);

    CHECK_CONDITION(LIST_NEXT(list->head)      == 0, "next[head] is not zero");
    CHECK_CONDITION(LIST_PREV(list->tail)      == 0, "prev[tail] is not zero");
    CHECK_CONDITION(LIST_NEXT(0)               == 0, "next[0] is not zero");
    CHECK_CONDITION(LIST_PREV(0)               == 0, "prev[0] is not zero");

// connectivity check

#ifdef LIST_DEBUG
    size_t used_cnt  = 0;
    size_t cur_iter  = list->tail;
    size_t prev_iter = 0;
    while (cur_iter != 0)
    {
        if (cur_iter != list->tail && cur_iter != list->head)
        {
            CHECK_CONDITION(prev_iter == LIST_PREV(cur_iter) && cur_iter == LIST_NEXT(prev_iter),
                        "prev-next misconnect between %zu and %zu: connectivity seems to be broken",
                        prev_iter, cur_iter);
        }
        
        used_cnt++;
        CHECK_CONDITION(used_cnt < list->buffer_size,
                         "Too many real elements: connectivity seems to be broken\n");

        prev_iter = cur_iter;
        cur_iter = LIST_NEXT(cur_iter);
    }
    
    // check size
    CHECK_CONDITION(list->used_size == used_cnt, "Used elements count mismatch");

    size_t free_cnt  = 0;
           prev_iter = 0;
           cur_iter  = list->head_free;

    while (cur_iter != 0)
    {
        CHECK_POISON(cur_iter);

        free_cnt++;
        CHECK_CONDITION(free_cnt < list->buffer_size,
                         "Too many free elements: connectivity seems to be broken");

        prev_iter = cur_iter;
        cur_iter = LIST_PREV(cur_iter);
    }

    CHECK_CONDITION(used_cnt + free_cnt + 1 == list->buffer_size, "Element count mismatch");
#endif

    return LIST_OK;
}
