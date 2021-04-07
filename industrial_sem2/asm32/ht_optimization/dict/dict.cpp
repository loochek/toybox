#include "dict.hpp"

const int HASH_BASE        = 31;
const int HASH_MOD         = 1000000007;
const int BUCKET_INIT_SIZE = 2;
const int DICT_CANARY      = 0xDEAD;

#ifdef DEBUG
#define DICT_CHECK(dict) LSCHK(dict_validate(dict))
#else
#define DICT_CHECK(dict)
#endif

/**
 * Calculates polynomial hash of the string
 */
static int dict_calc_hash(const char *string);

/**
 * Generic method to find ring in the dictionary. 
 * Common code for insertion and lookup. 
 * Returns LSTATUS_DICT_BAD_KEY if key is not present in the dictionary. 
 * 
 * \param \c dict Pointer to the dictionary
 * \param \c key  Key
 * \param \c ring_out Where to write a pointer to the ring
 */
static lstatus_t dict_find_ring(dict_t *dict, const char *key, chain_ring_t **ring_out);

lstatus_t dict_construct(dict_t *dict, int bucket_cnt)
{
    lstatus_t status = LSTATUS_OK;

    dict->buckets_count = bucket_cnt;
    dict->canary1 = DICT_CANARY;
    dict->canary2 = DICT_CANARY;

    dict->buckets = (list_t<chain_ring_t>*)calloc(bucket_cnt, sizeof(list_t<chain_ring_t>));
    if (dict->buckets == nullptr)
    {
        LSTATUS(LSTATUS_BAD_ALLOC, "");
        return status;
    }

    for (int i = 0; i < bucket_cnt; i++)
    {
        status = list_construct<chain_ring_t>(&dict->buckets[i], BUCKET_INIT_SIZE);
        if (status != LSTATUS_OK)
        {
            lstatus_t status2 = LSTATUS_OK;

            for (int j = 0; j < i; j++)
            {
                status2 = list_destruct(&dict->buckets[i]);
                if (status2 != LSTATUS_OK)
                    return status2;
            }

            free(dict->buckets);
            return status;
        }
    }

    DICT_CHECK(dict);
    return LSTATUS_OK;
}

lstatus_t dict_insert(dict_t *dict, const char *key, const char *value)
{
    lstatus_t status = LSTATUS_OK;
    DICT_CHECK(dict);

    chain_ring_t *ring_ptr = nullptr;
    status = dict_find_ring(dict, key, &ring_ptr);
    if (status == LSTATUS_OK)
    {
        // if key was found, update the value
        ring_ptr->value = value;
    }
    else if (status == LSTATUS_DICT_BAD_KEY)
    {
        // if key was not found, insert a new one

        int hash = dict_calc_hash(key) % dict->buckets_count;

        list_t<chain_ring_t> *bucket = &dict->buckets[hash];

        chain_ring_t ring = { key, value };
        LSCHK(list_push_back(bucket, ring));

        DICT_CHECK(dict);
    }

    // if dict_find_ring returns bad lstatus, just return it 
    return status;
}

lstatus_t dict_lookup(dict_t *dict, const char *key, const char **out_value)
{
    lstatus_t status = LSTATUS_OK;
    DICT_CHECK(dict);

    chain_ring_t *ring_ptr = nullptr;
    LSCHK(dict_find_ring(dict, key, &ring_ptr));

    *out_value = ring_ptr->value;
    return LSTATUS_OK;
}

lstatus_t dict_destruct(dict_t *dict)
{
    lstatus_t status = LSTATUS_OK;
    DICT_CHECK(dict);

    for (int i = 0; i < dict->buckets_count; i++)
    {
        LSCHK(list_destruct(&dict->buckets[i]));
    }

    free(dict->buckets);
    return LSTATUS_OK;
}

lstatus_t dict_validate(dict_t *dict)
{
    lstatus_t status = LSTATUS_OK;

    if (dict->canary1 != DICT_CANARY ||
        dict->canary2 != DICT_CANARY)
    {
        LSTATUS(LSTATUS_DICT_INVALID, "canary fault");
        return status;
    }

    for (int i = 0; i < dict->buckets_count; i++)
        LSCHK(list_validate(&dict->buckets[i]));

    return LSTATUS_OK;
}

static lstatus_t dict_find_ring(dict_t *dict, const char *key, chain_ring_t **ring_out)
{
    lstatus_t status = LSTATUS_OK;
    DICT_CHECK(dict);

    int hash = dict_calc_hash(key) % dict->buckets_count;

    list_t<chain_ring_t> *bucket = &dict->buckets[hash];

    list_iter_t iter = NULLITER, end_iter = NULLITER;
    LSCHK(list_begin(bucket, &iter));
    LSCHK(list_end  (bucket, &end_iter));

    while (!list_iter_cmp(iter, end_iter))
    {
        chain_ring_t *curr_ring = nullptr;
        LSCHK(list_data(bucket, iter, &curr_ring));

        if (strcmp(curr_ring->key, key) == 0)
        {
            *ring_out = curr_ring;
            return LSTATUS_OK;
        }

        LSCHK(list_next(bucket, &iter));
    }

    return LSTATUS_DICT_BAD_KEY;
}

static int dict_calc_hash(const char *string)
{
    long long hash = 0;

    const char *curr_char = string;
    while (*curr_char != '\0')
    {
        hash = (hash * HASH_BASE + (*curr_char)) % HASH_MOD;
        curr_char++;
    }

    return (int)hash;
}