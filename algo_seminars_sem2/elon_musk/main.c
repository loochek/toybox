#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int src;
    int dest;
} edge_t;

#define HASH_A 31
#define HASH_B 17
#define HASH_MOD  1000000007

#define HT_BUCKET_COUNT     1000003
#define MAX_VAR_NAME_LENGTH 100000

typedef struct chain_ring
{
    edge_t key;

    struct chain_ring *next;
} chain_ring_t;

static int calc_hash(edge_t edge)
{
    int hash1 = ((HASH_A * edge.src ) % HASH_MOD + HASH_B) % HASH_MOD;
    int hash2 = ((HASH_A * edge.dest) % HASH_MOD + HASH_B) % HASH_MOD;
    return (hash1 * HASH_A + hash2) % HASH_MOD;
}

static chain_ring_t *ht_get(chain_ring_t **table, edge_t key)
{
    size_t hash = calc_hash(key);
    
    chain_ring_t* ring = table[hash % HT_BUCKET_COUNT];

    while (ring != NULL)
    {
        if (ring->key.src == key.src && ring->key.dest == key.dest)
            return ring;

        ring = ring->next;
    }

    return NULL;
}

/**
 * Возвращает запись из хеш-таблицы по ключу.
 * Если записи по данному ключу нет, то она создается
*/
static chain_ring_t *ht_insert(chain_ring_t **table, edge_t key)
{
    size_t hash = calc_hash(key);
    
    chain_ring_t* ring = table[hash % HT_BUCKET_COUNT];

    while (ring != NULL)
    {
        if (ring->key.src == key.src && ring->key.dest == key.dest)
            return ring;

        ring = ring->next;
    }

    ring = (chain_ring_t*)calloc(1, sizeof(chain_ring_t));
    ring->key = key;

    ring->next = table[hash % HT_BUCKET_COUNT];
    table[hash % HT_BUCKET_COUNT] = ring;

    return ring;
}

int dsu_find(int u, int parent[])
{
    if (parent[u] == u)
        return u;

    return dsu_find(parent[u], parent);
}

void dsu_unite(int u, int v, int parent[])
{
    u = dsu_find(u, parent);
    v = dsu_find(v, parent);

    parent[u] = v;
}

int main()
{
    chain_ring_t **table = (chain_ring_t**)calloc(HT_BUCKET_COUNT, sizeof(chain_ring_t));

    int v = 0, e = 0, m = 0;
    scanf("%d %d %d", &v, &e, &m);

    edge_t *edges = (edge_t*)calloc(e, sizeof(edge_t));
    for (int i = 0; i < e; i++)
        scanf("%d %d", &edges[i].src, &edges[i].dest);

    edge_t *edges2 = (edge_t*)calloc(m, sizeof(edge_t));
    for (int i = 0; i < m; i++)
    {
        scanf("%d %d", &edges2[i].src, &edges2[i].dest);
        ht_insert(table, edges2[i]);
        ht_insert(table, (edge_t){ edges2[i].dest, edges2[i].src });
    }

    int *parent = (int*)calloc(v, sizeof(int));
    for (int i = 0 ; i < v; i++)
        parent[i] = i;

    int comp_cnt = v;
    for (int i = 0; i < e; i++)
    {
        edge_t edge = edges[i];
        if (ht_get(table, edge) != NULL)
            continue;

        if (dsu_find(edge.src, parent) == dsu_find(edge.dest, parent))
            continue;
        else
        {
            dsu_unite(edge.src, edge.dest, parent);
            comp_cnt--;
        }
    }

    int *ans = (int*)calloc(m + 1, sizeof(int));
    ans[m] = comp_cnt;

    for (int i = 0; i < m; i++)
    {
        edge_t edge = edges2[m - i - 1];

        if (dsu_find(edge.src, parent) != dsu_find(edge.dest, parent))
        {
            dsu_unite(edge.src, edge.dest, parent);
            comp_cnt--;
        }

        ans[m - i - 1] = comp_cnt;
    }

    for (int i = 0; i < m + 1; i++)
        printf("%d\n", ans[i]);

    for (size_t i = 0; i < HT_BUCKET_COUNT; i++)
    {
        chain_ring_t *ring = table[i];

        while (ring != NULL)
        {
            chain_ring_t *curr_ring = ring;
            ring = ring->next;

            free(curr_ring);
        }
    }

    free(table);
    free(edges);
    free(edges2);
    free(parent);
    free(ans);
}