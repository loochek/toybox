#ifndef STATIC_STORAGE_HPP
#define STATIC_STORAGE_HPP

#include <cstddef>
#include <cassert>

template<typename T, size_t N>
class StaticStorage
{
public:
    StaticStorage() : data_((T*)buffer_)
    {
        for (ssize_t i = 0; i < N; i++)
            new (&data_[i]) T();
    }

    StaticStorage(const StaticStorage &other) : data_((T*)buffer_)
    {
        for (ssize_t i = 0; i < N; i++)
            new (&data_[i]) T(other.data_[i]);
    }

    StaticStorage(StaticStorage &&other) : data_((T*)buffer_)
    {
        for (ssize_t i = 0; i < N; i++)
            new (&data_[i]) T(std::move(other.data_[i]));
    }

    ~StaticStorage()
    {
        for (ssize_t i = 0; i < N; i++)
            data_[i].~T();
    }

    StaticStorage &operator=(const StaticStorage &other)
    {
        for (ssize_t i = 0; i < N; i++)
            data_[i] = other.data_[i];

        return *this;
    }

    StaticStorage &operator=(StaticStorage &&other)
    {
        for (ssize_t i = 0; i < N; i++)
            data_[i] = std::move(other.data_[i]);

        return *this;
    }

    T& Access(size_t index)
    {
        return const_cast<T&>(static_cast<const StaticStorage*>(this)->Access(index));
    }

    const T& Access(size_t index) const
    {
        assert(index < N);
        return data_[index];
    }

    T *Data()
    {
        return data_;
    }

    const T *Data() const
    {
        return data_;
    }

    const size_t Size() const
    {
        return N;
    }

private:
    T *data_;
    uint8_t buffer_[N * sizeof(T)];
};

#endif
