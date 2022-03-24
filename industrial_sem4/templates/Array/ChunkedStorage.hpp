#ifndef CHUNKED_STORAGE_HPP
#define CHUNKED_STORAGE_HPP

#include <cstddef>
#include <cassert>
#include <stdexcept>
#include "DynamicStorage.hpp"

template<typename T, size_t N = 0>
class ChunkedStorage
{
    static constexpr size_t CHUNK_SIZE = 1024;
    static constexpr size_t MINIMAL_CAPACITY = 1; // in chunks

public:
    ChunkedStorage() :
        size_(0), capacity_(0)
    {
    }

    ChunkedStorage(size_t size) :
        size_(0), capacity_(0)
    {
        Reserve(size);
        for (ssize_t i = 0; i < size; i++)
            new (&AccessIntl(i)) T();

        size_ = size;
    }

    ChunkedStorage(const ChunkedStorage &other) :
        size_(0), capacity_(0)
    {
        Reserve(other.size_);
        for (ssize_t i = 0; i < other.size_; i++)
            new (&AccessIntl(i)) T(other.Access(i));

        size_ = other.size_;
    }

    ChunkedStorage(ChunkedStorage &&other) :
        chunks_(std::move(other.chunks_)), capacity_(other.capacity_), size_(other.size_)
    {
        other.capacity_ = 0;
        other.size_ = 0;
    }

    ~ChunkedStorage()
    {
        for (ssize_t i = 0; i < size_; i++)
            AccessIntl(i).~T();

        for (ssize_t i = 0; i < capacity_; i++)
            free(chunks_.Access(i));

        size_ = 0;
        capacity_ = 0;
    }

    ChunkedStorage &operator=(const ChunkedStorage &other)
    {
        Clear();
        Reserve(other.size_);
        for (ssize_t i = 0; i < other.size_; i++)
            new (&AccessIntl(i)) T(other.Access(i));

        size_ = other.size_;
        return *this;
    }

    ChunkedStorage &operator=(ChunkedStorage &&other)
    {
        this->~ChunkedStorage();
        chunks_ = std::move(other.chunks_);
        capacity_ = other.capacity_;
        size_ = other.size_;

        other.capacity_ = 0;
        other.size_ = 0;

        return *this;
    }

    T& Access(size_t index)
    {
        assert(index < size_);
        return AccessIntl(index);
    }

    const T& Access(size_t index) const
    {
        assert(index < size_);
        return AccessIntl(index);
    }

    T& ReserveBack()
    {
        Reserve(size_ + 1);
        return AccessIntl(size_++);
    }

    void PopBack()
    {
        assert(size_ != 0);
        AccessIntl(--size_).~T();
    }

    void Clear()
    {
        for (ssize_t i = 0; i < size_; i++)
            AccessIntl(i).~T();

        size_ = 0;
    }

    void Resize(size_t new_size)
    {
        if (new_size <= size_)
        {
            for (ssize_t i = new_size; i < size_; i++)
                AccessIntl(i).~T();
        }
        else
        {
            Reserve(new_size);
            for (ssize_t i = size_; i < new_size; i++)
                new (&AccessIntl(i)) T();
        }

        size_ = new_size;
    }

    void Reserve(size_t desired_capacity)
    {
        if (desired_capacity <= capacity_ * CHUNK_SIZE)
            return;

        SetCapacity(SizeToChunks(desired_capacity));
    }

    void Shrink()
    {
        size_t needed_chunks = SizeToChunks(size_);
        if (needed_chunks == capacity_)
            return;

        SetCapacity(needed_chunks);
    }

    const size_t Size() const
    {
        return size_;
    }

private:
    void SetCapacity(size_t chunks)
    {
        assert(chunks * CHUNK_SIZE >= size_);
        chunks = std::max(chunks, size_t(MINIMAL_CAPACITY));

        if (chunks <= capacity_)
        {
            for (ssize_t ch = chunks; ch < capacity_; ch++)
                free(chunks_.Access(ch));

            chunks_.Resize(capacity_);
        }
        else
        {
            chunks_.Resize(chunks);
            for (ssize_t ch = capacity_; ch < chunks; ch++)
            {
                T *chunk = (T*)calloc(CHUNK_SIZE, sizeof(T));;
                if (chunk == nullptr)
                    throw std::bad_alloc();

                chunks_.Access(ch) = chunk;
            }
        }

        capacity_ = chunks;
    }

    const T& AccessIntl(size_t index) const
    {
        return chunks_.Access(index / CHUNK_SIZE)[index % CHUNK_SIZE];
    }

    T& AccessIntl(size_t index)
    {
        return const_cast<T&>(static_cast<const ChunkedStorage*>(this)->AccessIntl(index));
    }

    static size_t SizeToChunks(size_t size)
    {
        size_t new_chunks = size / CHUNK_SIZE;
        if (new_chunks * CHUNK_SIZE < size)
            new_chunks++;

        return new_chunks;
    }

private:
    DynamicStorage<T*> chunks_;

    size_t size_;     // in elements
    size_t capacity_; // in chunks
};

#endif
