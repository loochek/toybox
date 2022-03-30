#ifndef STATIC_STORAGE_HPP
#define STATIC_STORAGE_HPP

#include <cstddef>
#include <cassert>

template<typename T, size_t SIZE>
class StaticStorage
{
public:
    StaticStorage() : data_((T*)buffer_)
    {
        ssize_t i = 0;

        try
        {
            for (; i < SIZE; i++)
                new (&data_[i]) T();
        }
        catch (...)
        {
            for (i--; i >= 0; i--)
                data_[i].~T();

            throw;
        }
    }

    StaticStorage(const StaticStorage &other) : data_((T*)buffer_)
    {
        ssize_t i = 0;

        try
        {
            for (; i < SIZE; i++)
                new (&data_[i]) T(other.data_[i]);
        }
        catch (...)
        {
            for (i--; i >= 0; i--)
                data_[i].~T();

            throw;
        }
    }

    StaticStorage(StaticStorage &&other) noexcept(std::is_nothrow_move_constructible_v<T>) :
        data_((T*)buffer_)
    {
        ssize_t i = 0;

        try
        {
            for (; i < SIZE; i++)
                new (&data_[i]) T(std::move_if_noexcept(other.data_[i]));
        }
        catch (...)
        {
            // Not reachable if T's move constructor is noexcept

            for (i--; i >= 0; i--)
                data_[i].~T();

            throw;
        }
    }

    ~StaticStorage()
    {
        for (ssize_t i = 0; i < SIZE; i++)
            data_[i].~T();
    }

    StaticStorage &operator=(const StaticStorage &other)
    {
        if (this == &other)
            return *this;

        ssize_t i = 0;
        this->~StaticStorage();

        try
        {
            for (; i < SIZE; i++)
                new (&data_[i]) T(other.data_[i]);
        }
        catch (...)
        {
            for (i--; i >= 0; i--)
                data_[i].~T();

            throw;
        }

        return *this;
    }

    StaticStorage &operator=(StaticStorage &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        if (this == &other)
            return *this;

        ssize_t i = 0;
        this->~StaticStorage();

        try
        {
            for (; i < SIZE; i++)
                new (&data_[i]) T(std::move_if_noexcept(other.data_[i]));
        }
        catch (...)
        {
            // Not reachable if T's move constructor is noexcept

            for (i--; i >= 0; i--)
                data_[i].~T();

            throw;
        }
        
        return *this;
    }

    inline T& Access(size_t index) noexcept
    {
        return const_cast<T&>(static_cast<const StaticStorage*>(this)->Access(index));
    }

    inline const T& Access(size_t index) const noexcept
    {
        assert(index < SIZE);
        return data_[index];
    }

    inline T *Data() noexcept
    {
        return data_;
    }

    inline const T *Data() const noexcept
    {
        return data_;
    }

    inline const size_t Size() const noexcept
    {
        return SIZE;
    }

private:
    T *data_;
    uint8_t buffer_[SIZE * sizeof(T)];
};

#endif
