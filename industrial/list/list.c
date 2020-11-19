#include <stdio.h>
#include <string.h>

#include "lerror.h"
#include "list.h"

#define MAX_CMD_LINE_LENGTH 100
#define LIST_DEBUG

static elem_t POISON = 0xDEAD;
static elem_t CANARY = 0xBEEF;

static list_status_t list_expand  (list_t *list);
static list_status_t list_validate(list_t *list);
static void          free_cell    (list_t *list, list_iter_t iter);
static list_iter_t   claim_cell   (list_t *list);

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

list_status_t list_construct(list_t *list, size_t capacity)
{
    list->buffer_size = capacity + 3;
    list->used_size   = 0;

    list->data = (elem_t*)calloc(list->buffer_size, sizeof(elem_t));
    list->next = (size_t*)calloc(list->buffer_size, sizeof(size_t));
    list->prev = (size_t*)calloc(list->buffer_size, sizeof(size_t));

    if (list->data == NULL || list->prev == NULL || list->next == NULL)
    {
        free(list->data);
        free(list->prev);
        free(list->next);
        return LIST_ERROR;
    }

    for (size_t i = 0; i < list->buffer_size; i++)
        list->data[i] = POISON;

    list->head    = 1;
    list->tail    = 1;
    list->prev[1] = 0;
    list->next[1] = 0;

    // head_free at the beginning and reverse order to maintain linearity when push_front / pop_front
    for (size_t i = 2; i < list->buffer_size - 1; i++)
        list->prev[i] = i + 1;
    list->prev[list->buffer_size - 1] = 0;

    for (size_t i = 3; i < list->buffer_size; i++)
        list->next[i] = i - 1;
    list->next[2] = 0;

    list->head_free = 2;

    list->linear = true;

    list->canary1 = CANARY;
    list->canary2 = CANARY;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_push_front(list_t *list, elem_t elem)
{
    LIST_CHECK(list, LIST_ERROR);

    list_iter_t claimed_cell = claim_cell(list);
    if (claimed_cell == 0)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_cell = claim_cell(list);
    }
    
    list->data[list->head] = elem;

    list->prev[claimed_cell] = list->head;
    list->next[list->head]  = claimed_cell;
    list->next[claimed_cell] = 0;
    list->head              = claimed_cell;

    list->used_size++;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_push_back(list_t *list, elem_t elem)
{
    LIST_CHECK(list, LIST_ERROR);

    list_iter_t claimed_cell = claim_cell(list);
    if (claimed_cell == 0)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_cell = claim_cell(list);
    }

    list->linear = false;

    list->next[claimed_cell] = list->tail;
    list->prev[list->tail]  = claimed_cell;
    list->prev[claimed_cell] = 0;
    list->tail              = claimed_cell;
    
    list->data[list->tail] = elem;

    list->used_size++;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_pop_front(list_t *list)
{
    LIST_CHECK(list, LIST_ERROR);

    if (list->head == list->tail)
        return LIST_EMPTY;

    list_iter_t to_remove = list->head;
    list->head = list->prev[list->head];
    list->next[list->head] = 0;
    list->data[list->head] = POISON;

    free_cell(list, to_remove);

    list->used_size--;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_pop_back(list_t *list)
{
    LIST_CHECK(list, LIST_ERROR);

    if (list->head == list->tail)
        return LIST_EMPTY;

    list->linear = false;

    list->data[list->tail] = 0;
    size_t new_tail = list->next[list->tail];
    free_cell(list, list->tail);

    list->prev[new_tail] = 0;
    list->tail = new_tail;

    list->used_size--;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_insert_after(list_t *list, list_iter_t iter, elem_t elem)
{
    LIST_CHECK(list, LIST_ERROR);

    if (iter == list->prev[list->head])
        return list_push_front(list, elem);

    list_iter_t claimed_cell = claim_cell(list);
    if (claimed_cell == 0)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_cell = claim_cell(list);
    }
    
    list->linear = false;

    list->data[claimed_cell]     = elem;
    list->prev[claimed_cell]     = iter;
    list->next[claimed_cell]     = list->next[iter];
    list->prev[list->next[iter]] = claimed_cell;
    list->next[iter]             = claimed_cell;

    list->used_size++;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

list_status_t list_insert_before(list_t *list, list_iter_t iter, elem_t elem)
{
    LIST_CHECK(list, LIST_ERROR);

    if (iter == list->tail)
        return list_push_back(list, elem);

    return list_insert_after(list, list->prev[iter], elem);
}

list_status_t list_remove(list_t *list, list_iter_t iter)
{
    LIST_CHECK(list, LIST_ERROR);

    if (iter == list->prev[list->head])
        return list_pop_front(list);

    if (iter == list->tail)
        return list_pop_back(list);

    list->linear = false;

    size_t left       = list->prev[iter];
    size_t right      = list->next[iter];
    list->next[left]  = right;
    list->prev[right] = left;

    free_cell(list, iter);

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

    for (list_iter_t iter = list_begin(list); iter != list_end(list); iter = list_next(list, iter))
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

    if (iter >= list->buffer_size)
        return NULL;
    
    return &list->data[iter];
}

list_iter_t list_iter_lookup(list_t *list, size_t position)
{
    LIST_CHECK(list, 0);

    if (list->linear)
        return position + 1;

    list_iter_t iter = list_begin(list);
    for (size_t i = 0; i < position; i++)
    {
        iter = list_next(list, iter);
        if (iter == list_end(list))
            return 0;
    }

    return iter;
}

list_iter_t list_next(list_t *list, list_iter_t iter)
{
    LIST_CHECK(list, 0);

    if (list->next[iter] == 0)
        return 0;

    return list->next[iter];
}

list_iter_t list_prev(list_t *list, list_iter_t iter)
{
    LIST_CHECK(list, 0);

    if (list->prev[iter] == 0)
        return 0;
        
    return list->prev[iter];
}

list_iter_t list_begin(list_t *list)
{
    LIST_CHECK(list, 0);

    return list->tail;
}

list_iter_t list_end(list_t *list)
{
    LIST_CHECK(list, 0);
    return list->head;
}

void list_destruct(list_t *list)
{
    free(list->data);
    free(list->prev);
    free(list->next);
}

#define HEAD_LABEL_ID      10000
#define TAIL_LABEL_ID      10001
#define HEAD_FREE_LABEL_ID 10002

static inline void emit_node(FILE* dot_file, list_t * list, list_iter_t iter)
{
    if (list->data[iter] == POISON)
        fprintf(dot_file, "%zu [label=\"&#9762;\" xlabel=%zu];\n",
                iter, iter);
    else
        fprintf(dot_file, "%zu [label=%d xlabel=%zu];\n",
                 iter, list->data[iter], iter);
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
                rand() % 128 + 64, rand() % 128 + 64, rand() % 128 + 64, i, list->next[i]);
        fprintf(dot_file,
                "{edge[constraint=false, arrowhead=crow, color=\"#%02X%02X%02X\"] %zu->%zu}\n",
                rand() % 128 + 64, rand() % 128 + 64, rand() % 128 + 64, i, list->prev[i]);
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
    
    for (list_iter_t iter = list_begin(list); iter != list_end(list); iter = list_next(list, iter))
        emit_node(dot_file, list, iter);

    emit_node(dot_file, list, list_end(list));
    fprintf(dot_file, "}\n;");

    // unite free cells
    fprintf(dot_file, "{rank=same\n;");
    list_iter_t iter = list->head_free;
    while (list->prev[iter] != 0)
    {
        emit_node(dot_file, list, iter);
        iter = list->prev[iter];
    }
    emit_node(dot_file, list, iter);
    fprintf(dot_file, "}\n;");

    fprintf(dot_file, "{%d [label=\"head\"];}\n"     , HEAD_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"tail\"];}\n"     , TAIL_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"head_free\"];}\n", HEAD_FREE_LABEL_ID);

    for (size_t i = 0; i < list->buffer_size; i++)
    {
        fprintf(dot_file, "{edge[color=tomato2]     %zu->%zu}\n", i, list->next[i]);
        fprintf(dot_file, "{edge[color=dodgerblue2] %zu->%zu}\n", i, list->prev[i]);
    }

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
        fprintf(html_file, "%7d ", list->data[i]);

    fprintf(html_file, "\nnext: ");
    for (size_t i = 0; i < list->buffer_size; i++)
        fprintf(html_file, "%7zu ", list->next[i]);

    fprintf(html_file, "\nprev: ");
    for (size_t i = 0; i < list->buffer_size; i++)
        fprintf(html_file, "%7zu ", list->prev[i]);

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

    elem_t *new_data = (elem_t*)realloc(list->data, sizeof(elem_t) * list->buffer_size * 2);
    size_t *new_prev = (size_t*)realloc(list->prev, sizeof(size_t) * list->buffer_size * 2);
    size_t *new_next = (size_t*)realloc(list->next, sizeof(size_t) * list->buffer_size * 2);
    if (new_data == NULL || new_prev == NULL || new_next == NULL)
    {
        list->data = new_data == NULL ? list->data : new_data;
        list->prev = new_prev == NULL ? list->prev : new_prev;
        list->next = new_next == NULL ? list->next : new_next;
        return LIST_ERROR;
    }
    
    list->data = new_data;
    list->prev = new_prev;
    list->next = new_next;

    memset(list->data + list->buffer_size, 0, list->buffer_size * sizeof(elem_t));
    memset(list->next + list->buffer_size, 0, list->buffer_size * sizeof(size_t));
    memset(list->prev + list->buffer_size, 0, list->buffer_size * sizeof(size_t));

    for (size_t i = list->buffer_size; i < list->buffer_size * 2; i++)
        list->data[i] = POISON;
        
    for (size_t i = list->buffer_size; i < list->buffer_size * 2 - 1; i++)
        list->prev[i] = i + 1;
    list->prev[list->buffer_size * 2 - 1] = 0;
    // the tail of free cells is always last cell of buffer
    list->prev[list->buffer_size - 1] = list->buffer_size;

    for (size_t i = list->buffer_size + 1; i < list->buffer_size * 2; i++)
        list->next[i] = i - 1;
    list->next[list->buffer_size] = list->buffer_size - 1;

    list->buffer_size *= 2;

    LIST_CHECK(list, LIST_ERROR);

    return LIST_OK;
}

static void free_cell(list_t *list, list_iter_t iter)
{
    list->next[iter]             = 0;
    list->prev[iter]             = list->head_free;
    list->next[list->head_free]  = iter;
    list->head_free              = iter;
    list->data[list->head_free]  = POISON;
}

static list_iter_t claim_cell(list_t *list)
{
    if (list->prev[list->head_free] == 0)
        return 0;

    list_iter_t to_ret = list->head_free;
    list->head_free = list->prev[list->head_free];
    list->next[list->head_free] = 0;

    return to_ret;
}

#define ASSERT_CONDITION(cond, msg, ...)             \
if (!(cond))                                         \
{                                                    \
    LERR(LERR_LIST_VALIDATION, #msg, ##__VA_ARGS__); \
    return LIST_ERROR;                               \
}

#define CHECK_POISON(iter) \
ASSERT_CONDITION(list->data[iter] == POISON, "Expected poison at cell %zu", iter)

static list_status_t list_validate(list_t *list)
{
    __lerrno = LERR_NO_ERROR;
    
    ASSERT_CONDITION(list->canary1 == CANARY, "canary1 fault");
    ASSERT_CONDITION(list->canary2 == CANARY, "canary2 fault");

    CHECK_POISON((list_iter_t)0);
    CHECK_POISON(list->head);
    CHECK_POISON(list->head_free);

    ASSERT_CONDITION(list->next[list->head]      == 0, "next[head] is not zero");
    ASSERT_CONDITION(list->prev[list->tail]      == 0, "prev[tail] is not zero");
    ASSERT_CONDITION(list->next[list->head_free] == 0, "next[head_free] is not zero");
    ASSERT_CONDITION(list->next[0]               == 0, "next[0] is not zero");
    ASSERT_CONDITION(list->prev[0]               == 0, "prev[0] is not zero");

#ifdef LIST_DEBUG
    size_t      cnt_real  = 0;
    list_iter_t cur_iter  = list->tail;
    list_iter_t prev_iter = 0;
    while (cur_iter != list->head)
    {
        if (prev_iter != 0)
        {
            ASSERT_CONDITION(prev_iter == list->prev[cur_iter] && cur_iter == list->next[prev_iter],
                         "prev-next misconnect between %zu and %zu: connectivity seems to be broken",
                         prev_iter, cur_iter);
        }

        cnt_real++;
        ASSERT_CONDITION(cnt_real < list->buffer_size,
                         "Too many real elements: connectivity seems to be broken\n");

        prev_iter = cur_iter;
        cur_iter = list->next[cur_iter];
    }
    // check size
    ASSERT_CONDITION(list->used_size == cnt_real, "Used elements count mismatch");

    // head
    cnt_real++;

    size_t cnt_free = 0;
    prev_iter = 0;
    cur_iter = list->head_free;
    while (cur_iter != 0)
    {
        if (cur_iter != list->head_free && list->prev[cur_iter] != 0)
        {
            ASSERT_CONDITION(prev_iter == list->next[cur_iter] && cur_iter == list->prev[prev_iter],
                         "prev-next misconnect between %zu and %zu: connectivity seems to be broken",
                         cur_iter, prev_iter);
        }

        CHECK_POISON(cur_iter);

        cnt_free++;
        ASSERT_CONDITION(cnt_free < list->buffer_size,
                         "Too many free elements: connectivity seems to be broken");

        prev_iter = cur_iter;
        cur_iter = list->prev[cur_iter];
    }

    ASSERT_CONDITION(cnt_real + cnt_free + 1 == list->buffer_size, "Element count mismatch");
#endif

    return LIST_OK;
}