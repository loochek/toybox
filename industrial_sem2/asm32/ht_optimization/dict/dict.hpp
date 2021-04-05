#ifndef DICT_HPP
#define DICT_HPP

#include "../list/list.hpp"
#include "../lstatus/lstatus.hpp"

/**
 * To be exactly 8 bytes
 * Say hello to the SIMD extensions!
 */
const int WORD_MAX_LEN = 31;

/**
 * Implements simple dictionary based on chaining hash table.
 * Keys and values are C strings with a maximum length of WORD_MAX_LEN
 */

/**
 * Strings are short since we use this structure as English-to-Russian dictionary,
 * so it's easier to store strings directly in the chain.
 */
struct chain_ring_t
{
    char key  [WORD_MAX_LEN + 1];
    char value[WORD_MAX_LEN + 1];
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