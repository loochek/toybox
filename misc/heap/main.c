#include <stdlib.h>
#include <stdio.h>

const size_t INF = 1e9;

void push(size_t val, size_t *curr_heap_size, size_t heap[])
{
    (*curr_heap_size)++;
    size_t pos = *curr_heap_size;
    heap[pos] = val;
    while (pos != 1)
    {
        if (heap[pos] < heap[pos / 2])
        {
            size_t tmp = heap[pos];
            heap[pos] = heap[pos / 2];
            heap[pos / 2] = tmp;
            pos /= 2;
        }
        else
            break;
    }
}

int pop_min(size_t *curr_heap_size, size_t heap[])
{
    size_t ans = heap[1];
    size_t last = heap[*curr_heap_size];
    heap[1] = last;
    heap[*curr_heap_size] = INF;
    (*curr_heap_size)--;
    
    size_t pos = 1;
    while (1)
    {
        size_t parent = heap[pos];
        size_t first_child = INF;
        if (pos * 2 <= *curr_heap_size)
            first_child = heap[pos * 2];

        size_t second_child = INF;
        if (pos * 2 + 1 <= *curr_heap_size)
            second_child = heap[pos * 2 + 1];

        if (first_child <= second_child && first_child < parent)
        {
            size_t tmp = heap[pos];
            heap[pos] = heap[pos * 2];
            heap[pos * 2] = tmp;
            pos *= 2;
        }
        else if (second_child <= first_child && second_child < parent)
        {
            size_t tmp = heap[pos];
            heap[pos] = heap[pos * 2 + 1];
            heap[pos * 2 + 1] = tmp;
            pos = pos * 2 + 1;
        }
        else
            break;
    }
    return ans;
}

int main()
{
    size_t n = 0;
    scanf("%zu", &n);

    size_t *heap = (size_t*)calloc(n + 1, sizeof(size_t));
    for (size_t i = 0; i < n; i++)
        heap[i] = INF;

    size_t heap_size = 0;

    for (size_t i = 0; i < n; i++)
    {
        size_t elem = 0;
        scanf("%zu", &elem);
        push(elem, &heap_size, heap);
    }

    size_t ans = 0;
    while (heap_size > 1)
    {
        size_t elem1 = pop_min(&heap_size, heap);
        size_t elem2 = pop_min(&heap_size, heap);
        ans += elem1 + elem2;
        push(elem1 + elem2, &heap_size, heap);
    }
    printf("%g\n", (double)ans / 100);
    free(heap);
    return 0;
}