#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <initializer_list>
#include "StaticStorage.hpp"
#include "DynamicStorage.hpp"
#include "ChunkedStorage.hpp"

template
<
    typename T,
    template<typename T_, size_t N_> class Storage = DynamicStorage,
    size_t N = 0
>
class Array
{
public:
    Array() : storage_() {}
    Array(size_t size) : storage_(size) {}

    Array(const std::initializer_list<T> &list)
    {
        storage_.Reserve(list.size());
        for (const T &elem : list)
            new (&storage_.ReserveBack()) T(elem);
    }

    Array(const Array &other) = default;
    Array(Array &&other) = default;

    Array& operator=(const Array &other)
    {
        storage_ = other.storage_;
        return *this;
    }

    Array& operator=(Array &&other)
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

    T& operator[](size_t index)
    {
        return const_cast<T&>(static_cast<const Array*>(this)->operator[](index));
    }

    const T& operator[](size_t index) const
    {
        return storage_.Access(index);
    }

    T& Back()
    {
        return const_cast<T&>(static_cast<const Array*>(this)->Back());
    }

    const T& Back() const
    {
        return storage_.Access(storage_.Size() - 1);
    }

    T& At(size_t index)
    {
        return const_cast<T&>(static_cast<const Array*>(this)->At(index));
    }

    const T& At(size_t index) const
    {
        if (index >= storage_.Size())
            throw std::out_of_range("Array index out of range");

        return storage_.Access(index);
    }

    T *Data()
    {
        return storage_.Data();
    }

    const T *Data() const
    {
        return storage_.Data();
    }

    void PushBack(T elem)
    {
        storage_.ReserveBack() = std::move(elem);
    }

    template<typename... Args>
    void EmplaceBack(Args... args)
    {
        new (&storage_.ReserveBack()) T(std::forward<Args>(args)...);
    }

    void PopBack()
    {
        storage_.PopBack();
    }

    void Clear()
    {
        storage_.Clear();
    }

    void Resize(size_t new_size)
    {
        storage_.Resize(new_size);
    }

    void Reserve(size_t capacity)
    {
        storage_.Reserve(capacity);
    }

    void Shrink()
    {
        storage_.Shrink();
    }

    const size_t Size() const
    {
        return storage_.Size();
    }

    const bool Empty() const
    {
        return storage_.Size() == 0;
    }

private:
    Storage<T, N> storage_;
};

template
<
    template<typename T_, size_t N_> class Storage,
    size_t N
>
class Array<bool, Storage, N>
{
    class Reference;
    static constexpr size_t BITS = 8; // Bits count in uint8_t

public:
    Array() : storage_() {}
    Array(size_t size) : storage_(CalculateSpace(size)), bool_size_(size) {}

    Array(const Array &other) = default;

    Array(Array &&other)
    {
        storage_ = std::move(other.storage_);
        bool_size_ = other.bool_size_;
        other.bool_size_ = 0;
    }

    Array(const std::initializer_list<bool> &list)
    {
        storage_.Reserve(CalculateSpace(list.size()));
        bool_size_ = 0;
        for (bool elem : list)
            PushBack(elem);
    }

    Array& operator=(const Array &other)
    {
        storage_ = other.storage_;
        bool_size_ = other.bool_size_;

        return *this;
    }

    Array& operator=(Array &&other)
    {
        storage_ = std::move(other.storage_);
        bool_size_ = other.bool_size_;
        other.bool_size_ = 0;

        return *this;
    }

    Array& operator=(const std::initializer_list<bool> &list)
    {
        storage_.Clear();
        storage_.Reserve(CalculateSpace(list.size()));
        bool_size_ = 0;
        for (bool elem : list)
            PushBack(elem);

        return *this;
    }

    Array<bool, Storage, N>::Reference operator[](size_t index)
    {
        size_t bit_number = BITS - index % BITS - 1;
        return Array<bool, Storage, N>::Reference(storage_.Access(index / BITS), bit_number);
    }

    const Array<bool, Storage, N>::Reference operator[](size_t index) const
    {
        size_t bit_number = BITS - index % BITS - 1;
        return Array<bool, Storage, N>::Reference(const_cast<uint8_t&>(storage_.Access(index / BITS)), bit_number);
    }

    Array<bool, Storage, N>::Reference Back()
    {
        return static_cast<Array*>(this)->operator[](bool_size_ - 1);
    }

    const Array<bool, Storage, N>::Reference Back() const
    {
        return static_cast<const Array*>(this)->operator[](bool_size_ - 1);
    }

    Array<bool, Storage, N>::Reference At(size_t index)
    {
        if (index >= bool_size_)
            throw std::out_of_range("Array index out of range");

        return static_cast<Array*>(this)->operator[](index);
    }

    const Array<bool, Storage, N>::Reference At(size_t index) const
    {
        if (index >= bool_size_)
            throw std::out_of_range("Array index out of range");

        return static_cast<const Array*>(this)->operator[](index);
    }

    void PushBack(bool elem)
    {
        if (bool_size_ % BITS == 0)
            storage_.ReserveBack() = 0;

        this->operator[](bool_size_++) = elem;
    }

    void EmplaceBack(bool elem)
    {
        PushBack(elem);
    }

    void PopBack()
    {
        bool_size_--;
        if (bool_size_ % BITS == 0)
            storage_.PopBack();
    }

    void Clear()
    {
        bool_size_ = 0;
        storage_.Clear();
    }

    void Resize(size_t new_size)
    {
        bool_size_ = new_size;
        storage_.Resize(CalculateSpace(new_size));
    }

    void Reserve(size_t capacity)
    {
        storage_.Reserve(CalculateSpace(capacity));
    }

    void Shrink()
    {
        storage_.Shrink();
    }

    const size_t Size() const
    {
        return bool_size_;
    }

    const bool Empty() const
    {
        return storage_.Size() == 0;
    }

private:
    class Reference
    {
    public:
        operator bool() const
        {
            return (byte_ >> bit_) & 0x1;
        }

        Reference& operator=(bool value)
        {
            byte_ &= ~(1 << bit_);
            byte_ |= (1 << bit_) * value;
            return *this;
        }

        Reference(uint8_t &byte, const uint8_t bit) :
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
    Storage<uint8_t, CalculateSpace(N)> storage_;
    size_t bool_size_;
};

#endif
