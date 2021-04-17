#include "list.hpp"

/**
 * Implementations of common iterator things of list
 */

bool list_iter_cmp(list_iter_t iter1, list_iter_t iter2)
{
    return iter1.value == iter2.value;
}