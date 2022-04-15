#ifndef CHUNKED_STORAGE_HPP
#define CHUNKED_STORAGE_HPP

#include <type_traits>
#include <cstddef>
#include <cassert>
#include <stdexcept>
#include <new>
#include "DynamicStorage.hpp"

template<typename T, size_t Dummy = 0>
class ChunkedStorage
{
    static constexpr size_t CHUNK_SIZE = 1024;
    static constexpr size_t MINIMAL_CAPACITY = 1; // in chunks

public:
    using IsContiguous = std::false_type;

    ChunkedStorage() noexcept :
        size_(0), capacity_(0)
    {
    }

    explicit ChunkedStorage(size_t size) :
        size_(0), capacity_(0)
    {
        Reserve(size);
        ssize_t i = 0;

        try
        {
            for (; i < size; i++)
                new (&AccessIntl(i)) T();
        }
        catch (...)
        {
            for (i--; i >= 0; i--)
                AccessIntl(i).~T();

            this->~ChunkedStorage();
            throw;
        }
        
        size_ = size;
    }

    ChunkedStorage(const ChunkedStorage &other) :
        size_(0), capacity_(0)
    {
        Reserve(other.size_);
        ssize_t i = 0;

        try
        {
            for (; i < other.size_; i++)
                new (&AccessIntl(i)) T(other.Access(i));
        }
        catch (...)
        {
            for (i--; i >= 0; i--)
                AccessIntl(i).~T();

            this->~ChunkedStorage();
            throw;
        }

        size_ = other.size_;
    }

    ChunkedStorage(ChunkedStorage &&other) noexcept :
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
            ::operator delete(chunks_.Access(i));

        size_ = 0;
        capacity_ = 0;
    }

    ChunkedStorage &operator=(const ChunkedStorage &other)
    {
        if (this == &other)
            return *this;

        Clear();
        Reserve(other.size_);
        ssize_t i = 0;

        try
        {
            for (; i < other.size_; i++)
                new (&AccessIntl(i)) T(other.Access(i));
        }
        catch (...)
        {
            for (i--; i >= 0; i--)
                AccessIntl(i).~T();

            throw;
        }

        size_ = other.size_;
        return *this;
    }

    ChunkedStorage &operator=(ChunkedStorage &&other) noexcept
    {
        if (this == &other)
            return *this;
            
        this->~ChunkedStorage();
        chunks_ = std::move(other.chunks_);
        capacity_ = other.capacity_;
        size_ = other.size_;

        other.capacity_ = 0;
        other.size_ = 0;

        return *this;
    }

    inline T& Access(size_t index) noexcept
    {
        assert(index < size_);
        return AccessIntl(index);
    }

    inline const T& Access(size_t index) const noexcept
    {
        assert(index < size_);
        return AccessIntl(index);
    }

    inline T& ReserveBack()
    {
        Reserve(size_ + 1);
        return AccessIntl(size_++);
    }
    
    inline void ReserveRollback() noexcept
    {
        assert(size_ != 0);
        size_--;
    }

    inline void PopBack()
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

    inline const size_t Size() const noexcept
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
                ::operator delete(chunks_.Access(ch));

            chunks_.Resize(capacity_);
        }
        else
        {
            chunks_.Resize(chunks);
            for (ssize_t ch = capacity_; ch < chunks; ch++)
            {
                T *chunk = (T*)::operator new(CHUNK_SIZE * sizeof(T));
                chunks_.Access(ch) = chunk;
            }
        }

        capacity_ = chunks;
    }

    inline const T& AccessIntl(size_t index) const noexcept
    {
        return chunks_.Access(index / CHUNK_SIZE)[index % CHUNK_SIZE];
    }

    inline T& AccessIntl(size_t index) noexcept
    {
        return const_cast<T&>(static_cast<const ChunkedStorage*>(this)->AccessIntl(index));
    }

    constexpr static size_t SizeToChunks(size_t size) noexcept
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
