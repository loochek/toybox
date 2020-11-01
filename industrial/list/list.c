#include <stdio.h>

#include "list.h"

static list_status_t list_expand(list_t *list);
static int claim_cell(list_t *list);
static void free_cell(list_t *list, size_t pos);

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

    list->head = 1;
    list->tail = 1;

    for (size_t i = 2; i < list->arr_size - 1; i++)
        list->next[i] = i + 1;
    list->next[list->arr_size - 1] = 0;

    for (size_t i = 3; i < list->arr_size; i++)
        list->prev[i] = i - 1;
    list->prev[2] = 0;

    list->prev[1] = 0;
    list->next[1] = 0;

    list->head_free = list->arr_size - 1;

    return LIST_OK;
}

list_status_t list_push_front(list_t *list, elem_t elem)
{
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

    return LIST_OK;
}

list_status_t list_push_back(list_t *list, elem_t elem)
{
    int claimed_pos = claim_cell(list);
    if (claimed_pos == -1)
    {
        if (list_expand(list) != LIST_OK)
            return LIST_ERROR;
        claimed_pos = claim_cell(list);
    }

    list->next[claimed_pos] = list->tail;
    list->prev[list->tail]  = claimed_pos;
    list->prev[claimed_pos] = 0;
    list->tail              = claimed_pos;
    
    list->data[list->tail] = elem;

    return LIST_OK;
}

list_status_t list_pop_front(list_t *list)
{
    if (list->head == list->tail)
        return LIST_EMPTY;

    list->data[list->prev[list->head]] = 0;
    free_cell(list, list->head);

    list->next[list->prev[list->head]] = 0;
    list->head = list->prev[list->head];

    return LIST_OK;
}

list_status_t list_pop_back(list_t *list)
{
    if (list->head == list->tail)
        return LIST_EMPTY;

    list->data[list->tail] = 0;
    size_t new_tail = list->next[list->tail];
    free_cell(list, list->tail);

    list->prev[new_tail] = 0;
    list->tail = new_tail;

    return LIST_OK;
}

list_status_t list_insert(list_t *list, size_t index, elem_t elem)
{
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

    list->data[claimed_pos]             = elem;
    list->prev[claimed_pos]             = current_pos;
    list->next[claimed_pos]             = list->next[current_pos];
    list->prev[list->next[current_pos]] = claimed_pos;
    list->next[current_pos]             = claimed_pos;

    return LIST_OK;
}

list_status_t list_remove(list_t *list, size_t index)
{
    int current_pos = list->tail;
    for (size_t i = 0; i < index; i++)
    {
        current_pos = list->next[current_pos];
        if (current_pos == list->head)
            return LIST_OUT_OF_BOUNDS;
    }
    if (current_pos == list->tail)
        return list_pop_back(list);
    if (current_pos == list->prev[list->head])
        return list_pop_front(list);

    list->data[current_pos] = 0;
    size_t left       = list->prev[current_pos];
    size_t right      = list->next[current_pos];
    list->next[left]  = right;
    list->prev[right] = left;

    free_cell(list, current_pos);

    return LIST_OK;
}

elem_t *list_at(list_t *list, size_t index)
{
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
    printf("\n");
    printf("data: ");
    for (size_t i = 0; i < list->arr_size; i++)
        printf("%4d ", list->data[i]);
    printf("\n");
    printf("next: ");
    for (size_t i = 0; i < list->arr_size; i++)
        printf("%4zu ", list->next[i]);
    printf("\n");
    printf("prev: ");
    for (size_t i = 0; i < list->arr_size; i++)
        printf("%4zu ", list->prev[i]);
    printf("\nhead: %zu\ntail: %zu\nhead_free: %zu\n\n", list->head, list->tail, list->head_free);
    printf("\n");
}

void list_destruct(list_t *list)
{
    free(list->data);
    free(list->prev);
    free(list->next);
}

static list_status_t list_expand(list_t *list)
{
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

    for (size_t i = list->arr_size; i < list->arr_size * 2 - 1; i++)
        list->next[i] = i + 1;
    list->next[list->arr_size * 2 - 1] = 0;

    for (size_t i = list->arr_size + 1; i < list->arr_size * 2; i++)
        list->prev[i] = i - 1;
    list->prev[list->arr_size] = list->head_free;
    list->next[list->head_free] = list->arr_size;
    list->head_free = list->arr_size * 2 - 1;

    list->arr_size *= 2;

    return LIST_OK;
}

static void free_cell(list_t *list, size_t pos)
{
    list->next[pos]             = 0;
    list->prev[pos]             = list->head_free;
    list->next[list->head_free] = pos;
    list->head_free             = pos;
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