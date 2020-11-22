#include <cstdlib>
#include <cstdio>

template<typename T, typename T_functor>
class segment_tree
{
public:

    // construct segment tree over size elements
    void construct(size_t size, T neutral)
    {
        this->tree_size = 1;
        while (this->tree_size < size)
            this->tree_size <<= 1;

        this->neutral_element = neutral;

        this->tree     = (T*)calloc(2 * tree_size, sizeof(T));
        this->addition = (T*)calloc(2 * tree_size, sizeof(T));

        for (size_t i = 1; i < tree_size * 2; i++)
        {
            this->tree    [i] = neutral;    
            this->addition[i] = neutral;
        }   
    }

    void initialize_with(T arr[], size_t size)
    {
        for (size_t i = tree_size; i < tree_size * 2; i++)
            this->tree[i] = neutral_element;

        for (size_t i = 0; i < size; i++)
            this->tree[tree_size + i] = arr[i];

        for (size_t i = tree_size - 1; i > 0; i--)
            this->tree[i] = T_functor()(tree[i * 2], tree[i * 2 + 1]);
    }

    // TODO: rewrite for mass operations
    // void set(size_t index, T value)
    // {
    //     set_recursive(1, 0, tree_size - 1, index, value);
    // }

    T request(size_t l, size_t r)
    {
        return request_recursive(1, 0, tree_size - 1, l, r);
    }

    void add(size_t l, size_t r, T val)
    {
        add_recursive(1, 0, tree_size - 1, l, r, val);
    }

    void destruct()
    {
        free(this->tree);
    }

private:

    T  *tree;
    T  *addition;
    T   neutral_element;
    size_t tree_size;

    void push_addition(size_t curr_vertice, size_t curr_l, size_t curr_r)
    {
        size_t seg_size = curr_r - curr_l + 1;
        addition[curr_vertice * 2]     += addition[curr_vertice];
        addition[curr_vertice * 2 + 1] += addition[curr_vertice];
        size_t mid = (curr_l + curr_r) / 2;
        tree[curr_vertice * 2]     += addition[curr_vertice] * (mid - curr_l + 1);
        tree[curr_vertice * 2 + 1] += addition[curr_vertice] * (curr_r - mid);
        addition[curr_vertice] = 0;
    }

    void add_recursive(size_t curr_vertice, size_t curr_l, size_t curr_r,
                        size_t req_l, size_t req_r, T value)
    {
        if (curr_r < req_l || req_r < curr_l)
            return;

        if (req_l <= curr_l && curr_r <= req_r)
        {
            addition[curr_vertice] += value;
            tree    [curr_vertice] += value * (curr_r - curr_l + 1);
            return;
        }

        push_addition(curr_vertice, curr_l, curr_r);

        size_t mid = (curr_l + curr_r) / 2;

        add_recursive(curr_vertice * 2    , curr_l , mid   , req_l, req_r, value);
        add_recursive(curr_vertice * 2 + 1, mid + 1, curr_r, req_l, req_r, value);

        tree[curr_vertice] = T_functor()(tree[curr_vertice * 2], tree[curr_vertice * 2 + 1]);
    }

    T request_recursive(size_t curr_vertice, size_t curr_l, size_t curr_r, size_t req_l, size_t req_r)
    {
        if (curr_r < req_l || req_r < curr_l)
            return neutral_element;

        if (req_l <= curr_l && curr_r <= req_r)
            return tree[curr_vertice];

        push_addition(curr_vertice, curr_l, curr_r);

        size_t mid = (curr_l + curr_r) / 2;

        return T_functor()(request_recursive(curr_vertice * 2, curr_l , mid, req_l, req_r),
                request_recursive(curr_vertice * 2 + 1, mid + 1, curr_r, req_l, req_r));
    }

};

struct pair
{
    long long max_value;
    long long xor_value;
};

struct functor_sum_int
{
public:
    long long operator()(long long x, long long y)
    {
        return x + y;
    }
};

const long long INF = 1e9;

int main()
{
    size_t n = 0, m = 0;
    scanf("%zu %zu", &n, &m);

    segment_tree<long long, functor_sum_int> tree;
    tree.construct(n, 0);

    long long *arr = (long long*)calloc(n, sizeof(long long));
    for (size_t i = 0; i < n; i++)
        scanf("%lld", &arr[i]);
    tree.initialize_with(arr, n);

    for (size_t i = 0; i < m; i++)
    {
        long long cmd = 0;
        scanf("%lld", &cmd);

        if (cmd == 1)
        {
            size_t l = 0, r = 0;
            scanf("%zu %zu", &l, &r);
    
            printf("%lld\n", tree.request(l, r));
        }
        else if (cmd == 2)
        {
            size_t l = 0, r = 0;
            long long val = 0;
            scanf("%zu %zu %lld", &l, &r, &val);
            tree.add(l, r, val);
        }
    }

    tree.destruct();
    return 0;
}