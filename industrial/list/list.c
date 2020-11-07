#include <stdio.h>
#include <string.h>

#include "lerror.h"
#include "list.h"

// poison for data
static elem_t POISON = 0xDEAD;

static elem_t CANARY = 0xBEEF;

static list_status_t list_expand(list_t *list);
static list_status_t list_validate(list_t *list);

static int  claim_cell(list_t *list);
static void free_cell (list_t *list, size_t pos);

#define LIST_CHECK_COND list_validate(list) == LIST_OK

#define LIST_CHECK(list)                                \
if (!(LIST_CHECK_COND))                                 \
{                                                       \
    printf("List validation failed: %s\n", __lerr_str); \
    list_print(list);                                   \
    return LIST_ERROR;                                  \
}   

list_status_t list_construct(list_t *list, size_t capacity)
{
    list->arr_size = capacity + 3;

    list->data = (elem_t*)calloc(list->arr_size, sizeof(elem_t));
    list->next = (size_t*)calloc(list->arr_size, sizeof(size_t));
    list->prev = (size_t*)calloc(list->arr_size, sizeof(size_t));

    if (list->data == NULL || list->prev == NULL || list->next == NULL)
    {
        free(list->data);
        free(list->prev);
        free(list->next);
        return LIST_ERROR;
    }

    for (size_t i = 0; i < list->arr_size; i++)
        list->data[i] = POISON;

    list->head = 1;
    list->tail = 1;

    for (size_t i = 2; i < list->arr_size - 1; i++)
        list->prev[i] = i + 1;
    list->prev[list->arr_size - 1] = 0;

    for (size_t i = 3; i < list->arr_size; i++)
        list->next[i] = i - 1;
    list->next[2] = 0;

    list->prev[1] = 0;
    list->next[1] = 0;

    list->head_free = 2;

    list->linear = true;

    list->canary1 = CANARY;
    list->canary2 = CANARY;

    LIST_CHECK(list);

    return LIST_OK;
}

list_status_t list_push_front(list_t *list, elem_t elem)
{
    LIST_CHECK(list);

    int claimed_pos = claim_cell(list);
    if (claimed_pos == -1)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_pos = claim_cell(list);
    }
    
    list->data[list->head] = elem;

    list->prev[claimed_pos] = list->head;
    list->next[list->head]  = claimed_pos;
    list->next[claimed_pos] = 0;
    list->head              = claimed_pos;

    LIST_CHECK(list);

    return LIST_OK;
}

list_status_t list_push_back(list_t *list, elem_t elem)
{
    LIST_CHECK(list);

    int claimed_pos = claim_cell(list);
    if (claimed_pos == -1)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_pos = claim_cell(list);
    }

    list->linear = false;

    list->next[claimed_pos] = list->tail;
    list->prev[list->tail]  = claimed_pos;
    list->prev[claimed_pos] = 0;
    list->tail              = claimed_pos;
    
    list->data[list->tail] = elem;

    LIST_CHECK(list);

    return LIST_OK;
}

list_status_t list_pop_front(list_t *list)
{
    LIST_CHECK(list);

    if (list->head == list->tail)
        return LIST_EMPTY;

    size_t to_remove = list->head;
    list->head = list->prev[list->head];
    list->next[list->head] = 0;
    list->data[list->head] = POISON;

    free_cell(list, to_remove);

    LIST_CHECK(list);

    return LIST_OK;
}

list_status_t list_pop_back(list_t *list)
{
    LIST_CHECK(list);

    if (list->head == list->tail)
        return LIST_EMPTY;

    list->linear = false;

    list->data[list->tail] = 0;
    size_t new_tail = list->next[list->tail];
    free_cell(list, list->tail);

    list->prev[new_tail] = 0;
    list->tail = new_tail;

    LIST_CHECK(list);

    return LIST_OK;
}

list_status_t list_insert(list_t *list, size_t index, elem_t elem)
{
    LIST_CHECK(list);

    int current_pos = list->tail;
    for (size_t i = 0; i < index; i++)
    {
        current_pos = list->next[current_pos];
        if (current_pos == list->head)
            return LIST_OUT_OF_BOUNDS;
    }

    if (current_pos == list->prev[list->head])
        return list_push_front(list, elem);

    int claimed_pos = claim_cell(list);
    if (claimed_pos == -1)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_pos = claim_cell(list);
    }

    list->linear = false;

    list->data[claimed_pos]             = elem;
    list->prev[claimed_pos]             = current_pos;
    list->next[claimed_pos]             = list->next[current_pos];
    list->prev[list->next[current_pos]] = claimed_pos;
    list->next[current_pos]             = claimed_pos;

    LIST_CHECK(list);

    return LIST_OK;
}

list_status_t list_remove(list_t *list, size_t index)
{
    LIST_CHECK(list);

    int current_pos = list->tail;
    for (size_t i = 0; i < index; i++)
    {
        current_pos = list->next[current_pos];
        if (current_pos == list->head)
            return LIST_OUT_OF_BOUNDS;
    }

    if (current_pos == list->prev[list->head])
        return list_pop_front(list);

    list->linear = false;

    if (current_pos == list->tail)
        return list_pop_back(list);

    list->data[current_pos] = 0;
    size_t left       = list->prev[current_pos];
    size_t right      = list->next[current_pos];
    list->next[left]  = right;
    list->prev[right] = left;

    free_cell(list, current_pos);

    LIST_CHECK(list);

    return LIST_OK;
}

list_status_t list_linearize(list_t *list)
{
    LIST_CHECK(list);

    int size = list_size(list);
    if (size == -1)
        return LIST_ERROR;

    list_t new_list;
    if (list_construct(&new_list, size) != LIST_OK)
        return LIST_ERROR;

    size_t cur_pos = list->tail;
    while (cur_pos != list->head)
    {
        if (list_push_front(&new_list, list->data[cur_pos]) != LIST_OK)
        {
            list_destruct(&new_list);
            return LIST_ERROR;
        }
        cur_pos = list->next[cur_pos];
    }

    list_destruct(list);
    *list = new_list;

    LIST_CHECK(list);

    return LIST_OK;
}

int list_size(list_t *list)
{
    if (!(LIST_CHECK_COND))
        return -1;

    size_t size = 0;
    size_t cur_pos = list->tail;
    while (cur_pos != list->head)
    {
        size++;
        cur_pos = list->next[cur_pos];
    }
    return size;
}

elem_t *list_at(list_t *list, size_t index)
{
    if (!(LIST_CHECK_COND))
        return NULL;

    if (list->linear)
        return &list->data[index + 1];

    size_t current_pos = list->tail;
    for (size_t i = 0; i < index; i++)
    {
        current_pos = list->next[current_pos];
        if (current_pos == list->head)
            return NULL;
    }
    return &list->data[current_pos];
}

void list_print(list_t *list)
{
    printf("List dump:\n");
    printf("data: ");
    for (size_t i = 0; i < list->arr_size; i++)
        printf("%7d ", list->data[i]);
    printf("\nnext: ");
    for (size_t i = 0; i < list->arr_size; i++)
        printf("%7zu ", list->next[i]);
    printf("\nprev: ");
    for (size_t i = 0; i < list->arr_size; i++)
        printf("%7zu ", list->prev[i]);
    printf("\nhead: %zu\ntail: %zu\nhead_free: %zu\nlinear: %d\n\n",
            list->head, list->tail, list->head_free, list->linear);
}

#define HEAD_LABEL_ID      10000
#define TAIL_LABEL_ID      10001
#define HEAD_FREE_LABEL_ID 10002

static inline void emit_node(FILE* dot_file, list_t * list, size_t id)
{
    if (list->data[id] == POISON)
        fprintf(dot_file, "%zu [label=\"&#9762;\" xlabel=%zu];\n", id, id);
    else
        fprintf(dot_file, "%zu [label=%d xlabel=%zu];\n", id, list->data[id], id);
}

void list_visualise_safe(list_t *list)
{
    FILE *dot_file = fopen("list.dot", "w");

    fprintf(dot_file, "digraph G\n{\nnode [shape=box];\n");

    emit_node(dot_file, list, 0);

    fprintf(dot_file, "{\nrank=same;\n");
    for (size_t i = 1; i < list->arr_size; i++)
        emit_node(dot_file, list, i);
    fprintf(dot_file, "}\n");

    fprintf(dot_file, "{\nrank=same; rankdir=LR\n");
    fprintf(dot_file, "{%d [label=\"tail\"];}\n"     , TAIL_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"head\"];}\n"     , HEAD_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"head_free\"];}\n", HEAD_FREE_LABEL_ID);
    fprintf(dot_file, "}\n");

    for (size_t i = 0; i < list->arr_size; i++)
    {
        fprintf(dot_file, "{edge[color=tomato2]     %zu->%zu}\n", i, list->next[i]);
        fprintf(dot_file, "{edge[color=dodgerblue2] %zu->%zu}\n", i, list->prev[i]);
    }

    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_LABEL_ID     , list->head);
    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", TAIL_LABEL_ID     , list->tail);
    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_FREE_LABEL_ID, list->head_free);
    fprintf(dot_file, "}\n");
    fclose(dot_file);

    system("dot list.dot -Tpng > out.png");
    system("gwenview out.png");
}

list_status_t list_visualise_fancy(list_t *list)
{
    LIST_CHECK(list);

    FILE *dot_file = fopen("list.dot", "w");

    fprintf(dot_file, "digraph G\n{\nnode [shape=box];\n");

    emit_node(dot_file, list, 0);

    // unite used cells
    fprintf(dot_file, "{rank=same\n;");
    size_t cur_pos = list->tail;
    while (cur_pos != list->head)
    {
        emit_node(dot_file, list, cur_pos);
        cur_pos = list->next[cur_pos];
    }
    emit_node(dot_file, list, cur_pos);
    fprintf(dot_file, "}\n;");

    // unite free cells
    fprintf(dot_file, "{rank=same\n;");
    cur_pos = list->head_free;
    while (list->prev[cur_pos] != 0)
    {
        emit_node(dot_file, list, cur_pos);
        cur_pos = list->prev[cur_pos];
    }
    emit_node(dot_file, list, cur_pos);
    fprintf(dot_file, "}\n;");

    fprintf(dot_file, "{%d [label=\"head\"];}\n"     , HEAD_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"tail\"];}\n"     , TAIL_LABEL_ID);
    fprintf(dot_file, "{%d [label=\"head_free\"];}\n", HEAD_FREE_LABEL_ID);

    for (size_t i = 0; i < list->arr_size; i++)
    {
        fprintf(dot_file, "{edge[color=tomato2]     %zu->%zu}\n", i, list->next[i]);
        fprintf(dot_file, "{edge[color=dodgerblue2] %zu->%zu}\n", i, list->prev[i]);
    }

    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_LABEL_ID     , list->head);
    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", TAIL_LABEL_ID     , list->tail);
    fprintf(dot_file, "{edge[color=darkgreen]  %d->%zu}\n", HEAD_FREE_LABEL_ID, list->head_free);
    fprintf(dot_file, "}\n");
    fclose(dot_file);

    system("dot list.dot -Tpng > out.png");
    system("gwenview out.png");

    return LIST_OK;
}


void list_destruct(list_t *list)
{
    free(list->data);
    free(list->prev);
    free(list->next);
}

static list_status_t list_expand(list_t *list)
{
    LIST_CHECK(list);

    elem_t *new_data = (elem_t*)realloc(list->data, sizeof(elem_t) * list->arr_size * 2);
    size_t *new_prev = (size_t*)realloc(list->prev, sizeof(size_t) * list->arr_size * 2);
    size_t *new_next = (size_t*)realloc(list->next, sizeof(size_t) * list->arr_size * 2);
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

    memset(list->data + list->arr_size, 0, list->arr_size * sizeof(elem_t));
    memset(list->next + list->arr_size, 0, list->arr_size * sizeof(size_t));
    memset(list->prev + list->arr_size, 0, list->arr_size * sizeof(size_t));

    for (size_t i = list->arr_size; i < list->arr_size * 2; i++)
        list->data[i] = POISON;
        
    for (size_t i = list->arr_size; i < list->arr_size * 2 - 1; i++)
        list->prev[i] = i + 1;
    list->prev[list->arr_size * 2 - 1] = 0;
    list->prev[list->arr_size - 1] = list->arr_size;

    for (size_t i = list->arr_size + 1; i < list->arr_size * 2; i++)
        list->next[i] = i - 1;
    list->next[list->arr_size] = list->arr_size - 1;

    list->arr_size *= 2;

    LIST_CHECK(list);

    return LIST_OK;
}

static void free_cell(list_t *list, size_t pos)
{
    list->next[pos]             = 0;
    list->prev[pos]             = list->head_free;
    list->next[list->head_free] = pos;
    list->head_free             = pos;
    list->data[list->head_free] = POISON;
}

static int claim_cell(list_t *list)
{
    if (list->prev[list->head_free] == 0)
        return -1;

    size_t to_ret = list->head_free;
    list->head_free = list->prev[list->head_free];
    list->next[list->head_free] = 0;

    return to_ret;
}

#define ASSERT_CONDITION(cond, msg, ...)                                              \
if (!(cond))                                                                          \
{                                                                                     \
    LERR(LERR_LIST_VALIDATION, #msg, ##__VA_ARGS__);                                  \
    return LIST_ERROR;                                                                \
}

#define CHECK_POISON(id) ASSERT_CONDITION(list->data[id] == POISON, "Expected poison at cell %zu", id)

static list_status_t list_validate(list_t *list)
{
    __lerrno = LERR_NO_ERROR;
    
    ASSERT_CONDITION(list->canary1 == CANARY, "canary1 fault");
    ASSERT_CONDITION(list->canary2 == CANARY, "canary2 fault");

    CHECK_POISON((size_t)0);
    CHECK_POISON(list->head);
    CHECK_POISON(list->head_free);

    ASSERT_CONDITION(list->next[list->head]      == 0, "next[head] is not zero");
    ASSERT_CONDITION(list->prev[list->tail]      == 0, "prev[tail] is not zero");
    ASSERT_CONDITION(list->next[list->head_free] == 0, "next[head_free] is not zero");
    ASSERT_CONDITION(list->next[0]               == 0, "next[0] is not zero");
    ASSERT_CONDITION(list->prev[0]               == 0, "prev[0] is not zero");

    size_t cnt_real = 0;
    size_t cur_pos  = list->tail;
    size_t prev_pos = 0;
    while (cur_pos != list->head)
    {
        if (prev_pos != 0)
        {
            ASSERT_CONDITION(prev_pos == list->prev[cur_pos],
                         "prev-next misconnect between %zu and %zu: connectivity seems to be broken",
                         prev_pos, cur_pos);
        }

        cnt_real++;
        ASSERT_CONDITION(cnt_real < list->arr_size,
                         "Too many real elements: connectivity seems to be broken\n");

        prev_pos = cur_pos;
        cur_pos = list->next[cur_pos];
    }
    cnt_real++;

    size_t cnt_free = 0;
    prev_pos = 0;
    cur_pos = list->head_free;
    while (cur_pos != 0)
    {
        if (cur_pos != list->head_free && list->prev[cur_pos] != 0)
        {
            ASSERT_CONDITION(prev_pos == list->next[cur_pos],
                         "prev-next misconnect between %zu and %zu: connectivity seems to be broken",
                         cur_pos, prev_pos);
        }

        CHECK_POISON(cur_pos);

        cnt_free++;
        ASSERT_CONDITION(cnt_free < list->arr_size,
                         "Too many free elements: connectivity seems to be broken");

        prev_pos = cur_pos;
        cur_pos = list->prev[cur_pos];
    }

    ASSERT_CONDITION(cnt_real + cnt_free + 1 == list->arr_size, "Element count mismatch");

    return LIST_OK;
}