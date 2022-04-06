#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <initializer_list>
#include <type_traits>
#include "StaticStorage.hpp"
#include "DynamicStorage.hpp"
#include "ChunkedStorage.hpp"

template
<
    typename T,
    template<typename T_, size_t SIZE_> class Storage = DynamicStorage,
    size_t SIZE = 0
>
class Array
{
public:
    Array() : storage_() {}
    explicit Array(size_t size) : storage_(size) {}

    Array(const std::initializer_list<T> &list)
    {
        storage_.Reserve(list.size());
        for (const T &elem : list)
            new (&storage_.ReserveBack()) T(elem);
    }

    Array(const Array &other) = default;
    Array(Array &&other) noexcept(std::is_nothrow_move_constructible_v<Storage<T, SIZE>>) = default;

    Array& operator=(const Array &other)
    {
        storage_ = other.storage_;
        return *this;
    }

    Array& operator=(Array &&other) noexcept(std::is_nothrow_move_constructible_v<Storage<T, SIZE>>)
    {
        storage_ = std::move(other.storage_);
        return *this;
    }

    Array& operator=(const std::initializer_list<T> &list)
    {
        storage_.Clear();
        storage_.Reserve(list.size());
        for (const T &elem : list)
            new (&storage_.ReserveBack()) T(elem);

        return *this;
    }

    inline T& operator[](size_t index) noexcept
    {
        return const_cast<T&>(static_cast<const Array*>(this)->operator[](index));
    }

    inline const T& operator[](size_t index) const noexcept
    {
        return storage_.Access(index);
    }

    inline T& Back() noexcept
    {
        return const_cast<T&>(static_cast<const Array*>(this)->Back());
    }

    inline const T& Back() const noexcept
    {
        return storage_.Access(storage_.Size() - 1);
    }

    inline T& At(size_t index)
    {
        return const_cast<T&>(static_cast<const Array*>(this)->At(index));
    }

    inline const T& At(size_t index) const
    {
        if (index >= storage_.Size())
            throw std::out_of_range("Array index out of range");

        return storage_.Access(index);
    }

    inline T *Data() noexcept
    {
        return storage_.Data();
    }

    inline const T *Data() const noexcept
    {
        return storage_.Data();
    }

    inline void PushBack(T elem)
    {
        storage_.ReserveBack() = std::move(elem);
    }

    template<typename... Args>
    inline void EmplaceBack(Args&&... args)
    {
        T &place = storage_.ReserveBack();

        try
        {
            new (&place) T(std::forward<Args>(args)...);
        }
        catch (...)
        {
            storage_.ReserveRollback();
        }
    }

    inline void PopBack()
    {
        storage_.PopBack();
    }

    inline void Clear()
    {
        storage_.Clear();
    }

    inline void Resize(size_t new_size)
    {
        storage_.Resize(new_size);
    }

    inline void Reserve(size_t capacity)
    {
        storage_.Reserve(capacity);
    }

    inline void Shrink()
    {
        storage_.Shrink();
    }

    inline const size_t Size() const noexcept
    {
        return storage_.Size();
    }

    inline const bool Empty() const noexcept
    {
        return storage_.Size() == 0;
    }

private:
    Storage<T, SIZE> storage_;
};

template
<
    template<typename T_, size_t SIZE_> class Storage,
    size_t SIZE
>
class Array<bool, Storage, SIZE>
{
    class Reference;
    static constexpr size_t BITS = 8; // Bits count in uint8_t

public:
    Array() : storage_(), bool_size_(0) {}
    explicit Array(size_t size) : storage_(CalculateSpace(size)), bool_size_(size) {}

    Array(const Array &other) = default;

    Array(Array &&other) noexcept : bool_size_(0)
    {
        storage_ = std::move(other.storage_);
        bool_size_ = other.bool_size_;
        other.bool_size_ = 0;
    }

    Array(const std::initializer_list<bool> &list) : bool_size_(0)
    {
        storage_.Reserve(CalculateSpace(list.size()));
        for (bool elem : list)
            PushBack(elem);
    }

    Array& operator=(const Array &other)
    {
        if (this == &other)
            return *this;

        bool_size_ = 0;
        storage_ = other.storage_;
        bool_size_ = other.bool_size_;

        return *this;
    }

    Array& operator=(Array &&other) noexcept
    {
        if (this == &other)
            return *this;
        
        bool_size_ = 0;
        storage_ = std::move(other.storage_);
        bool_size_ = other.bool_size_;
        other.bool_size_ = 0;

        return *this;
    }

    Array& operator=(const std::initializer_list<bool> &list)
    {
        bool_size_ = 0;
        storage_.Clear();
        storage_.Reserve(CalculateSpace(list.size()));
        for (bool elem : list)
            PushBack(elem);

        return *this;
    }

    inline Array<bool, Storage, SIZE>::Reference operator[](size_t index) noexcept
    {
        size_t bit_number = BITS - index % BITS - 1;
        return Array<bool, Storage, SIZE>::Reference(storage_.Access(index / BITS), bit_number);
    }

    inline const Array<bool, Storage, SIZE>::Reference operator[](size_t index) const noexcept
    {
        size_t bit_number = BITS - index % BITS - 1;
        return Array<bool, Storage, SIZE>::Reference(const_cast<uint8_t&>(storage_.Access(index / BITS)), bit_number);
    }

    inline Array<bool, Storage, SIZE>::Reference Back() noexcept
    {
        return operator[](bool_size_ - 1);
    }

    inline const Array<bool, Storage, SIZE>::Reference Back() const noexcept
    {
        return operator[](bool_size_ - 1);
    }

    inline Array<bool, Storage, SIZE>::Reference At(size_t index)
    {
        if (index >= bool_size_)
            throw std::out_of_range("Array index out of range");

        return operator[](index);
    }

    inline const Array<bool, Storage, SIZE>::Reference At(size_t index) const
    {
        if (index >= bool_size_)
            throw std::out_of_range("Array index out of range");

        return operator[](index);
    }

    inline void PushBack(bool elem)
    {
        if (bool_size_ % BITS == 0)
            storage_.ReserveBack() = 0;

        operator[](bool_size_++) = elem;
    }

    inline void EmplaceBack(bool elem)
    {
        PushBack(elem);
    }

    inline void PopBack() noexcept
    {
        bool_size_--;
        if (bool_size_ % BITS == 0)
            storage_.PopBack();
    }

    inline void Clear() noexcept
    {
        bool_size_ = 0;
        storage_.Clear();
    }

    inline void Resize(size_t new_size)
    {
        bool_size_ = new_size;
        storage_.Resize(CalculateSpace(new_size));
    }

    inline void Reserve(size_t capacity)
    {
        storage_.Reserve(CalculateSpace(capacity));
    }

    inline void Shrink()
    {
        storage_.Shrink();
    }

    inline const size_t Size() const noexcept
    {
        return bool_size_;
    }

    inline const bool Empty() const noexcept
    {
        return storage_.Size() == 0;
    }

private:
    class Reference
    {
    public:
        operator bool() const noexcept
        {
            return (byte_ >> bit_) & 0x1;
        }

        Reference& operator=(bool value) noexcept
        {
            byte_ &= ~(1 << bit_);
            byte_ |= (1 << bit_) * value;
            return *this;
        }

        Reference(uint8_t &byte, const uint8_t bit) noexcept:
            byte_(byte), bit_(bit)
        {
            assert(0 <= bit && bit < BITS);
        }

        uint8_t &byte_;
        const uint8_t bit_;
    };

    static constexpr size_t CalculateSpace(size_t capacity)
    {
        return capacity / BITS + !!(capacity % BITS);
    }

private:
    Storage<uint8_t, CalculateSpace(SIZE)> storage_;
    size_t bool_size_;
};

#endif
