#ifndef DICT_HPP
#define DICT_HPP

#include "../list/list.hpp"
#include "../lstatus/lstatus.hpp"

/**
 * Implements simple dictionary based on chaining hash table.
 * Dictionary is designed to be non-owning,
 * so you must keep validity of pointers that you passed to a dictionary
 */

struct chain_ring_t
{
    const char *key;
    const char *value;
};

struct dict_t
{
    list_t<chain_ring_t> *buckets;
    int buckets_count;
};

/**
 * Initializes a dictionary
 * 
 * \param \c dict Pointer to the dictionary
 * \param \c bucket_cnt Initial buckets count
 */
lstatus_t dict_construct(dict_t *dict, int bucket_cnt);

/**
 * Deinitializes a dictionary
 * 
 * \param \c dict Pointer to the dictionary
 */
lstatus_t dict_destruct(dict_t *dict);

/**
 * Inserts an element into the dictionary
 * If element with such key already exists, it updates it with provided value
 * You must keep pointers valid as dictionary is non-owning!
 * 
 * \param \c dict  Pointer to the dictionary
 * \param \c key   Key
 * \param \c value Value
 */
lstatus_t dict_insert(dict_t *dict, const char *key, const char *value);

/**
 * Searches an element in the dictionary
 * Provides a pointer to the value,
 * or returns LSTATUS_DICT_BAD_KEY if key is not present in the dictionary
 * 
 * \param \c dict Pointer to the dictionary
 * \param \c key  Key
 * \param \c out_value Where to write a pointer to the value
 */
lstatus_t dict_lookup(dict_t *dict, const char *key, const char **out_value);

#endif