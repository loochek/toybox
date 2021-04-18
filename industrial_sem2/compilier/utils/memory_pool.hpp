#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include "list.hpp"

/**
 * Provides an efficient memory allocation for elements of the same type
 * (TODO: replace placeholder to the real structure)
 */
template<typename T>
struct memory_pool_t
{
    // dummy
};

/**
 * Initializes a pool
 * 
 * \param \c pool Memory pool
 */
template<typename T>
lstatus_t memory_pool_construct(memory_pool_t<T> *pool);

/**
 * Allocates memory from the pool
 * 
 * \param \c pool Memory pool
 * \param \c ptr_out Where to write pointer to allocated memory block
 */
template<typename T>
lstatus_t memory_pool_alloc(memory_pool_t<T> *pool, T **ptr_out);

/**
 * Returns memory to the pool
 * 
 * \param \c pool Memory pool
 * \param \c ptr  Memory block to return
 */
template<typename T>
lstatus_t memory_pool_free(memory_pool_t<T> *pool, T *ptr);

/**
 * Deinitializes a pool
 * 
 * \param \c pool Memory pool
 */
template<typename T>
lstatus_t memory_pool_destruct(memory_pool_t<T> *pool);

//----------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------

template<typename T>
lstatus_t memory_pool_construct(memory_pool_t<T> *pool)
{
    return LSTATUS_OK;
}

template<typename T>
lstatus_t memory_pool_alloc(memory_pool_t<T> *pool, T **ptr_out)
{
    lstatus_t status = LSTATUS_OK;

    *ptr_out = (T*)calloc(1, sizeof(T));
    if (*ptr_out == nullptr)
    {
        LSTATUS(LSTATUS_BAD_ALLOC, "");
        return status;
    }

    return LSTATUS_OK;
}

template<typename T>
lstatus_t memory_pool_free(memory_pool_t<T> *pool, T *ptr)
{
    free(ptr);
    return LSTATUS_OK;
}

template<typename T>
lstatus_t memory_pool_destruct(memory_pool_t<T> *pool)
{
    return LSTATUS_OK;
}

#endif