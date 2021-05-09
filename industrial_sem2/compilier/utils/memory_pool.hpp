#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include "list.hpp"

template<typename T>
union pool_object_t
{
    T elem;
    pool_object_t<T>* next_free;
};

/**
 * Provides an efficient memory allocation for elements of the same size
 */
template<typename T>
struct memory_pool_t
{
#ifndef MEMORY_POOL_DEBUG
    int canary1;

    list_t<pool_object_t<T>*> blocks;
    pool_object_t<T>* free_head;

    int canary2;
#endif
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
 * Do nothing if a null pointer passed
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

/**
 * Validates a pool
 * 
 * \param \c pool Memory pool
 */
template<typename T>
lstatus_t memory_pool_validate(memory_pool_t<T> *pool);

//----------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------

/**
 * Inserts a new block in the beginning
 * 
 * \param \c pool Memory pool
 */
template<typename T>
static lstatus_t insert_block(memory_pool_t<T> *pool);

const int POOL_BLOCK_SIZE = 16384;

const int POOL_CANARY = 0xDFCD;

#define POOL_LSCHK_LOCAL(expr, num) { status = expr; if (status != LSTATUS_OK) POOL_ERROR_HANDLER(num); }
#define POOL_ERROR_HANDLER(num) goto error_handler##num

#ifdef DEBUG
#define POOL_CHECK(pool) LSCHK(memory_pool_validate(pool))
#else
#define POOL_CHECK(pool)
#endif


template<typename T>
lstatus_t memory_pool_construct(memory_pool_t<T> *pool)
{
#ifndef MEMORY_POOL_DEBUG
    LSCHK(list_construct(&pool->blocks));

    pool->canary1 = POOL_CANARY;
    pool->canary2 = POOL_CANARY;

    POOL_CHECK(pool);

#endif

    return LSTATUS_OK;
}

template<typename T>
lstatus_t memory_pool_alloc(memory_pool_t<T> *pool, T **ptr_out)
{
#ifndef MEMORY_POOL_DEBUG

    POOL_CHECK(pool);

    if (pool->free_head == nullptr)
        LSCHK(pool_insert_block(pool));

    pool_object_t<T> *obj = pool->free_head;
    pool->free_head = obj->next_free;

    obj->elem = {};
    *ptr_out = &obj->elem;
    return LSTATUS_OK;

#else

    lstatus_t status = LSTATUS_OK;

    *ptr_out = (T*)calloc(1, sizeof(T));
    if (*ptr_out == nullptr)
    {
        LSTATUS(LSTATUS_BAD_ALLOC, "");
        return status;
    }

    return LSTATUS_OK;

#endif
}

template<typename T>
lstatus_t memory_pool_free(memory_pool_t<T> *pool, T *ptr)
{
#ifndef MEMORY_POOL_DEBUG

    POOL_CHECK(pool);

    // free function must be tolerant to null pointer
    if (ptr == nullptr)
        return LSTATUS_OK;

    // because of pool_object_t's internal representation
    pool_object_t<T> *obj = (pool_object_t<T>*)ptr;

    obj->next_free = pool->free_head;
    pool->free_head = obj;

    return LSTATUS_OK;

#else

    free(ptr);
    return LSTATUS_OK;

#endif
}

template<typename T>
lstatus_t memory_pool_destruct(memory_pool_t<T> *pool)
{
#ifndef MEMORY_POOL_DEBUG

    POOL_CHECK(pool);

    list_iter_t blocks_iter = NULLITER, blocks_end = NULLITER;
    LSCHK(list_begin(&pool->blocks, &blocks_iter));
    LSCHK(list_end(&pool->blocks, &blocks_end));

    while (!list_iter_cmp(blocks_iter, blocks_end))
    {
        pool_object_t<T> **block_ptr = nullptr;
        LSCHK(list_data(&pool->blocks, blocks_iter, &block_ptr));

        free(*block_ptr);

        LSCHK(list_next(&pool->blocks, &blocks_iter));
    }

    LSCHK(list_destruct(&pool->blocks));

#endif

    return LSTATUS_OK;
}

template<typename T>
lstatus_t memory_pool_validate(memory_pool_t<T> *pool)
{
    lstatus_t status = LSTATUS_OK;

    if (pool == nullptr)
    {
        LSTATUS(LSTATUS_POOL_INVALID, "null pointer passed");
        return status;
    }

    if (pool->canary1 != POOL_CANARY || pool->canary2 != POOL_CANARY)
    {
        LSTATUS(LSTATUS_POOL_INVALID, "canary fault");
        return status;
    }

    LSCHK(list_validate(&pool->blocks));
    return LSTATUS_OK;
}

template<typename T>
static lstatus_t pool_insert_block(memory_pool_t<T> *pool)
{
    lstatus_t status = LSTATUS_OK;

    pool_object_t<T>* block = nullptr;

    block = (pool_object_t<T>*)calloc(POOL_BLOCK_SIZE, sizeof(pool_object_t<T>));
    if (block == nullptr)
    {
        LSTATUS(LSTATUS_BAD_ALLOC, "");
        POOL_ERROR_HANDLER(1);
    }

    block[0].next_free = pool->free_head;
    for (int i = 1; i < POOL_BLOCK_SIZE; i++)
        block[i].next_free = &block[i - 1];

    pool->free_head = &block[POOL_BLOCK_SIZE - 1];

    POOL_LSCHK_LOCAL(list_push_back(&pool->blocks, block), 2);
    return LSTATUS_OK;

error_handler2:
    free(block);

error_handler1:
    LSCHK(list_destruct(&pool->blocks));

error_handler0:
    return status;
}

#endif