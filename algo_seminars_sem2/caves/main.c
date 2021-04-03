#include <stdlib.h>
#include <stdio.h>

#define VEC_INIT_CAP 16

typedef struct
{
    long long dst;
    long long cost;
} edge_t;

typedef struct
{
    edge_t *array;
    long long     size;
    long long     capacity;
} vector_t;

void vector_construct(vector_t *v)
{
    v->size = 0;
    v->capacity = 16;
    v->array = (edge_t*)calloc(VEC_INIT_CAP, sizeof(edge_t));
}

void vector_insert(vector_t *v, edge_t elem)
{
    if (v->size == v->capacity)
    {
        v->capacity *= 2;
        v->array = (edge_t*)realloc(v->array, v->capacity * sizeof(edge_t));
    }

    v->array[v->size] = elem;
    v->size++;
}

edge_t vector_at(vector_t *v, long long idx)
{
    return v->array[idx];
}

void vector_destruct(vector_t *v)
{
    free(v->array);
}

#define INF 100000000000000

int main()
{
    long long n = 0, m = 0, k = 0, s = 0, f = 0;
    scanf("%lld %lld %lld", &n, &m, &k);

    vector_t *edges = (vector_t*)calloc(n, sizeof(vector_t));
    for (long long i = 0; i < n; i++)
        vector_construct(&edges[i]);

    long long *fire_caves = (long long*)calloc(k, sizeof(long long));
    for (long long i = 0; i < k; i++)
    {
        scanf("%lld", &fire_caves[i]);
        fire_caves[i]--;
    }

    for (long long i = 0; i < m; i++)
    {
        long long u = 0, v = 0, c = 0;
        scanf("%lld %lld %lld", &u, &v, &c);
        vector_insert(&edges[u - 1], (edge_t){ v - 1, c });
        vector_insert(&edges[v - 1], (edge_t){ u - 1, c });
    }

    scanf("%lld %lld", &s, &f);
    s--; f--;

    // first pass: fire caves

    long long *dist = (long long*)calloc(n, sizeof(long long));
    long long *used = (long long*)calloc(n, sizeof(long long));

    for (long long i = 0; i < n; i++)
        dist[i] = INF;
    dist[f] = 0;

    while (1)
    {
        long long min_dist = INF, curr_vert = -1;
        for (long long i = 0; i < n; i++)
        {
            if (!used[i] && dist[i] < min_dist)
            {
                min_dist = dist[i];
                curr_vert = i;
            }
        }

        if (min_dist == INF)
            break;

        used[curr_vert] = 1;

        vector_t *neigh = &edges[curr_vert];
        for (long long i = 0; i < neigh->size; i++)
        {
            edge_t e = vector_at(neigh, i);
            long long dst = e.dst, cost = e.cost;

            if (dist[curr_vert] + cost < dist[dst])
                dist[dst] = dist[curr_vert] + cost;
        }
    }

    long long nearest_fire_dist = INF;
    for (long long i = 0; i < k; i++)
    {
        if (dist[fire_caves[i]] < nearest_fire_dist)
            nearest_fire_dist = dist[fire_caves[i]];
    }

    long long victims_dist = dist[s];

    if (victims_dist < nearest_fire_dist)
        printf("%lld\n", victims_dist);
    else
        printf("-1\n");

    free(dist);
    free(used);
    free(fire_caves);
    
    for (long long i = 0; i < n; i++)
        vector_destruct(&edges[i]);

    free(edges);
    return 0;
}