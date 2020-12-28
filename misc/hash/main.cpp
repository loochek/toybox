#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

template<typename T>
struct avl_node_t
{
    T key;
    int height;

    avl_node_t<T> *left_child;
    avl_node_t<T> *right_child;

    avl_node_t() : key(0), height(1), left_child(nullptr), right_child(nullptr) {}
    avl_node_t(const T &key) : key(key), height(1), left_child(nullptr), right_child(nullptr) {}
};

template<typename T>
int avl_get_height(const avl_node_t<T> *node)
{
    return node == nullptr ? 0 : node->height;
}

template<typename T>
int avl_get_balance_factor(const avl_node_t<T> *node)
{
    if (node == nullptr)
        return 0;
    
    return avl_get_height(node->left_child) - avl_get_height(node->right_child);
}

template<typename T>
avl_node_t<T> *avl_get_left_child(const avl_node_t<T> *node)
{
    return node == nullptr ? nullptr : node->left_child;
}

template<typename T>
avl_node_t<T> *avl_get_right_child(const avl_node_t<T> *node)
{
    return node == nullptr ? nullptr : node->right_child;
}

template<typename T>
void avl_fix_height(avl_node_t<T> *node)
{
    if (node == nullptr)
        return;
    
    int left_height  = avl_get_height(node->left_child);
    int right_height = avl_get_height(node->right_child);
    
    node->height = (left_height > right_height ? left_height : right_height) + 1;
}

template<typename T>
avl_node_t<T> *avl_rotate_left(avl_node_t<T> *node)
{
    if (node == nullptr)
        return node;
    if (node->right_child == nullptr)
        return node;

    avl_node_t<T> *new_root = node->right_child;
    node->right_child = new_root->left_child;
    new_root->left_child = node;

    avl_fix_height(node);
    avl_fix_height(new_root);
    
    return new_root;
}

template<typename T>
avl_node_t<T> *avl_rotate_right(avl_node_t<T> *node)
{
    if (node == nullptr)
        return node;
    if (node->left_child == nullptr)
        return node;

    avl_node_t<T> *new_root = node->left_child;
    node->left_child = new_root->right_child;
    new_root->right_child = node;

    avl_fix_height(node);
    avl_fix_height(new_root);

    return new_root;
}

template<typename T>
avl_node_t<T> *avl_balance(avl_node_t<T> *node)
{
    if (node == nullptr)
        return node;

    if (avl_get_balance_factor(node) == -2)
    {
        if (avl_get_balance_factor(node->right_child) > 0)
            node->right_child = avl_rotate_right(node->right_child);
        node = avl_rotate_left(node);
    }

    if (avl_get_balance_factor(node) == 2)
    {
        if (avl_get_balance_factor(node->left_child ) < 0)
            node->left_child = avl_rotate_left(node->left_child);
        node = avl_rotate_right(node);
    }

    assert(avl_get_balance_factor(node) <= 1 && avl_get_balance_factor(node) >= -1);
    return node;   
}

template<typename T>
avl_node_t<T> *avl_find(avl_node_t<T> *node, const T &key)
{
    if (node == nullptr)
        return nullptr;
    
    if (key < node->key)
        return avl_find(node->left_child, key);
    else if (key > node->key)
        return avl_find(node->right_child, key);
    else
        return node;
}

template<typename T>
avl_node_t<T> *avl_insert(avl_node_t<T> *node, const T &key)
{
    if (node == nullptr)
        return new avl_node_t<T>(key);

    if (key < node->key)
        node->left_child  = avl_insert(node->left_child , key);        
    else
        node->right_child = avl_insert(node->right_child, key);

    avl_fix_height(node);
    return avl_balance(node);
}

template<typename T>
void avl_destruct(avl_node_t<T> *node)
{
    if (node == nullptr)
        return;
    
    avl_destruct(node->left_child);
    avl_destruct(node->right_child);
    delete node;
}

const size_t MAX_STR_LEN = 1e5 + 2;
size_t HASH_MOD1    = 1000000007;
size_t HASH_BASE1   = 7;
size_t HASH_MOD2    = 1000000009;
size_t HASH_BASE2   = 9;

static size_t get_hash(size_t l, size_t r, size_t pref_hashes1[], size_t precalc_pows1[],
                                           size_t pref_hashes2[], size_t precalc_pows2[])
{
    size_t hash1 = (HASH_MOD1 * 2 + pref_hashes1[r + 1] - (pref_hashes1[l] * precalc_pows1[r - l + 1]) % HASH_MOD1) % HASH_MOD1;
    size_t hash2 = (HASH_MOD2 * 2 + pref_hashes2[r + 1] - (pref_hashes2[l] * precalc_pows2[r - l + 1]) % HASH_MOD2) % HASH_MOD2;
    return hash1 * hash2;
}

int main()
{
    char *str = (char*)calloc(MAX_STR_LEN + 2, sizeof(char));
    fgets(str, MAX_STR_LEN, stdin);

    size_t str_len = strlen(str);

    size_t *precalc_pows1 = (size_t*)calloc(str_len + 1, sizeof(size_t));
    size_t *pref_hashes1  = (size_t*)calloc(str_len + 1, sizeof(size_t));

    size_t *precalc_pows2 = (size_t*)calloc(str_len + 1, sizeof(size_t));
    size_t *pref_hashes2  = (size_t*)calloc(str_len + 1, sizeof(size_t));

    pref_hashes1 [0] = 0;
    precalc_pows1[0] = 1;
    pref_hashes2 [0] = 0;
    precalc_pows2[0] = 1;
    for (size_t i = 0; i < str_len; i++)
    {
        pref_hashes1 [i + 1] = (pref_hashes1[i] * HASH_BASE1 % HASH_MOD1 + str[i] - 96 + HASH_MOD1) % HASH_MOD1;
        precalc_pows1[i + 1] = precalc_pows1[i] * HASH_BASE1 % HASH_MOD1;
        pref_hashes2 [i + 1] = (pref_hashes2[i] * HASH_BASE2 % HASH_MOD2 + str[i] - 96 + HASH_MOD2) % HASH_MOD2;
        precalc_pows2[i + 1] = precalc_pows2[i] * HASH_BASE2 % HASH_MOD2;
    }

    size_t best_pos = 0;

    size_t search_l_bound = 0, search_r_bound = str_len;
    while (search_r_bound - search_l_bound > 1)
    {
        size_t substr_size = (search_l_bound + search_r_bound) / 2;

        avl_node_t<size_t> *tree_root = NULL;

        bool found = false;
        for (size_t i = 0; i < str_len - substr_size + 1; i++)
        {
            size_t hash = get_hash(i, i + substr_size - 1, pref_hashes1, precalc_pows1, pref_hashes2, precalc_pows2);
            if (avl_find(tree_root, hash) != nullptr)
            {
                search_l_bound = substr_size;
                found = true;
                best_pos = i;
                break;
            }
            tree_root = avl_insert(tree_root, hash);
        }
        if (!found)
            search_r_bound = substr_size;

        avl_destruct(tree_root);
    }

    for (size_t i = 0; i < search_l_bound; i++)
        putc(str[best_pos + i], stdout);

    printf("\n");
    
    free(str);
    free(pref_hashes1);
    free(precalc_pows1);
    free(pref_hashes2);
    free(precalc_pows2);
    return 0;
}