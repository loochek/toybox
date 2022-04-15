#ifndef STATIC_STORAGE_HPP
#define STATIC_STORAGE_HPP

#include <cstddef>
#include <cassert>

template<typename T, template<typename T_> class Dummy, size_t StaticSize>
class StaticStorage
{
public:
    using IsContiguous = std::true_type;
    
    StaticStorage() = default;
    StaticStorage(const StaticStorage &other) = default;

    template<typename T_ = T, std::enable_if_t<std::is_nothrow_move_constructible_v<T_>>* = nullptr>
    StaticStorage(StaticStorage &&other) noexcept
    {
        for (ssize_t i = 0; i < StaticSize; i++)
            data_[i] = std::move(other.data_[i]);
    }

    template<typename T_ = T, std::enable_if_t<!std::is_nothrow_move_constructible_v<T_>>* = nullptr>
    StaticStorage(StaticStorage &&other) : StaticStorage(other) {}

    ~StaticStorage() = default;

    StaticStorage &operator=(const StaticStorage &other) = default;

    template<typename T_ = T, std::enable_if_t<std::is_nothrow_move_constructible_v<T_>>* = nullptr>
    StaticStorage &operator=(StaticStorage &&other) noexcept
    {
        for (ssize_t i = 0; i < StaticSize; i++)
            data_[i] = std::move(other.data_[i]);
    }

    template<typename T_ = T, std::enable_if_t<!std::is_nothrow_move_constructible_v<T_>>* = nullptr>
    StaticStorage &operator=(StaticStorage &&other)
    {
        return operator=(other);
    }

    inline T& Access(size_t index) noexcept
    {
        return const_cast<T&>(static_cast<const StaticStorage*>(this)->Access(index));
    }

    inline const T& Access(size_t index) const noexcept
    {
        assert(index < StaticSize);
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
        return StaticSize;
    }

private:
    T data_[StaticSize];
};

#endif
