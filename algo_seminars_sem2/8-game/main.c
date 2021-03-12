#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    size_t cell1;
    size_t cell2;
    size_t cell3;
    size_t cell4;
    size_t cell5;
    size_t cell6;
    size_t cell7;
    size_t cell8;
    size_t cell9;
} game_state_t;


typedef struct
{
    size_t tail_pos;
    size_t head_pos;

    game_state_t *buffer;
} queue_t;

#define BUFFER_SIZE 500000

void queue_init(queue_t *q)
{
    q->buffer = (game_state_t*)calloc(BUFFER_SIZE, sizeof(game_state_t));
    q->tail_pos = 1;
    q->head_pos = 0;
}

void queue_deinit(queue_t *q)
{
    free(q->buffer);
}

bool queue_empty(queue_t *q)
{
    return q->tail_pos > q->head_pos;
}

void queue_push(queue_t *q, game_state_t elem)
{
    q->head_pos++;
    q->buffer[q->head_pos] = elem;
}

game_state_t queue_pop(queue_t *q)
{
    game_state_t res = q->buffer[q->tail_pos];
    q->tail_pos++;
    return res;
}


#define HASH_A 31
#define HASH_B 17
#define HASH_MOD  1000000007

#define HT_BUCKET_COUNT     100003
#define MAX_VAR_NAME_LENGTH 100000

typedef struct chain_ring
{
    size_t key;
    size_t val;

    struct chain_ring *next;
} chain_ring_t;

static size_t calc_hash(size_t key)
{
    return ((HASH_A * key) % HASH_MOD + HASH_B) % HASH_MOD;
}

/**
 * Возвращает запись из хеш-таблицы по ключу.
 * Если записи по данному ключу нет, то она создается
*/
static chain_ring_t *ht_locate(chain_ring_t **table, size_t key)
{
    size_t hash = calc_hash(key);
    
    chain_ring_t* ring = table[hash % HT_BUCKET_COUNT];

    while (ring != NULL)
    {
        if (ring->key == key)
            return ring;

        ring = ring->next;
    }

    ring = (chain_ring_t*)calloc(1, sizeof(chain_ring_t));
    ring->key = key;
    ring->val = 0;

    ring->next = table[hash % HT_BUCKET_COUNT];
    table[hash % HT_BUCKET_COUNT] = ring;

    return ring;
}

game_state_t read_state()
{
    game_state_t start_state = {0};
    char c = 0;

    c = getchar();
    if (c == ' ')
        c = '0';
    start_state.cell1 = c - '0';
    c = getchar();
    if (c == ' ')
        c = '0';
    start_state.cell2 = c - '0';
    c = getchar();
    if (c == ' ')
        c = '0';
    start_state.cell3 = c - '0';
    c = getchar();
    c = getchar();
    if (c == ' ')
        c = '0';
    start_state.cell4 = c - '0';
    c = getchar();
    if (c == ' ')
        c = '0';
    start_state.cell5 = c - '0';
    c = getchar();
    if (c == ' ')
        c = '0';
    start_state.cell6 = c - '0';
    c = getchar();
    c = getchar();
    if (c == ' ')
        c = '0';
    start_state.cell7 = c - '0';
    c = getchar();
    if (c == ' ')
        c = '0';
    start_state.cell8 = c - '0';
    c = getchar();
    if (c == ' ')
        c = '0';
    start_state.cell9 = c - '0';

    return start_state;
}

size_t hash(game_state_t state)
{
    size_t res = state.cell1;
    res = res * 10 + state.cell2;
    res = res * 10 + state.cell3;
    res = res * 10 + state.cell4;
    res = res * 10 + state.cell5;
    res = res * 10 + state.cell6;
    res = res * 10 + state.cell7;
    res = res * 10 + state.cell8;
    res = res * 10 + state.cell9;

    return res;
}

#define EMPTY_CELL (size_t)(0)

bool check_win_state(game_state_t state)
{
    return state.cell1 == 1 &&
           state.cell2 == 2 &&
           state.cell3 == 3 &&
           state.cell4 == 4 &&
           state.cell5 == 5 &&
           state.cell6 == 6 &&
           state.cell7 == 7 &&
           state.cell8 == 8 &&
           state.cell9 == EMPTY_CELL;
}

int main()
{
    chain_ring_t **color = (chain_ring_t**)calloc(HT_BUCKET_COUNT, sizeof(chain_ring_t));
    chain_ring_t **dist = (chain_ring_t**)calloc(HT_BUCKET_COUNT, sizeof(chain_ring_t));

    game_state_t start_state = read_state();

    queue_t q = {0};
    queue_init(&q);
    queue_push(&q, start_state);
    ht_locate(color, hash(start_state))->val = 1;

    bool solved = false;
    while (!queue_empty(&q))
    {
        game_state_t curr_state = queue_pop(&q);

        if (ht_locate(color, hash(curr_state))->val == 2)
            continue;

        size_t curr_path_len = ht_locate(dist, hash(curr_state))->val;
        if (curr_path_len > 50)
            continue;

        if (check_win_state(curr_state))
        {
            printf("%zu\n", curr_path_len);
            solved = true;
            break;
        }

        if (curr_state.cell1 == EMPTY_CELL)
        {
            game_state_t new_state = curr_state;
            new_state.cell1 = curr_state.cell2;
            new_state.cell2 = EMPTY_CELL;
            
            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
            

            new_state = curr_state;
            new_state.cell1 = curr_state.cell4;
            new_state.cell4 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
        }

        if (curr_state.cell3 == EMPTY_CELL)
        {
            game_state_t new_state = curr_state;
            new_state.cell3 = curr_state.cell2;
            new_state.cell2 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell3 = curr_state.cell6;
            new_state.cell6 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
        }

        if (curr_state.cell7 == EMPTY_CELL)
        {
            game_state_t new_state = curr_state;
            new_state.cell7 = curr_state.cell4;
            new_state.cell4 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell7 = curr_state.cell8;
            new_state.cell8 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
        }

        if (curr_state.cell9 == EMPTY_CELL)
        {
            game_state_t new_state = curr_state;
            new_state.cell9 = curr_state.cell6;
            new_state.cell6 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell9 = curr_state.cell8;
            new_state.cell8 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
        }

        if (curr_state.cell2 == EMPTY_CELL)
        {
            game_state_t new_state = curr_state;
            new_state.cell2 = curr_state.cell1;
            new_state.cell1 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell2 = curr_state.cell3;
            new_state.cell3 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell2 = curr_state.cell5;
            new_state.cell5 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
        }

        if (curr_state.cell4 == EMPTY_CELL)
        {
            game_state_t new_state = curr_state;
            new_state.cell4 = curr_state.cell1;
            new_state.cell1 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell4 = curr_state.cell5;
            new_state.cell5 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell4 = curr_state.cell7;
            new_state.cell7 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
        }

        if (curr_state.cell6 == EMPTY_CELL)
        {
            game_state_t new_state = curr_state;
            new_state.cell6 = curr_state.cell3;
            new_state.cell3 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell6 = curr_state.cell5;
            new_state.cell5 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell6 = curr_state.cell9;
            new_state.cell9 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
        }

        if (curr_state.cell8 == EMPTY_CELL)
        {
            game_state_t new_state = curr_state;
            new_state.cell8 = curr_state.cell5;
            new_state.cell5 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell8 = curr_state.cell7;
            new_state.cell7 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell8 = curr_state.cell9;
            new_state.cell9 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
        }

        if (curr_state.cell5 == EMPTY_CELL)
        {
            game_state_t new_state = curr_state;
            new_state.cell5 = curr_state.cell2;
            new_state.cell2 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell5 = curr_state.cell4;
            new_state.cell4 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell5 = curr_state.cell6;
            new_state.cell6 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }

            new_state = curr_state;
            new_state.cell5 = curr_state.cell8;
            new_state.cell8 = EMPTY_CELL;

            if (ht_locate(color, hash(new_state))->val == 0)
            {
                queue_push(&q, new_state);
                ht_locate(color, hash(new_state))->val = 1;
                ht_locate(dist, hash(new_state))->val = curr_path_len + 1;
            }
        }
    }

    if (!solved)
        printf("-1\n");

    return 0;
}