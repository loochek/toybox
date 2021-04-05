#include "list.hpp"

/**
 * Common iterator things of list
 */

/**
 * Returns true if the iterators are equal
 * 
 * \param \c iter1 First iterator
 * \param \c iter2 Second iterator
 */
bool list_iter_cmp(list_iter_t iter1, list_iter_t iter2)
{
    return iter1.value == iter2.value;
}