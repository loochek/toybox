#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "StaticStorage.hpp"
#include "DynamicStorage.hpp"

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

// template
// <
//     template<typename T_, size_t N_> class Storage,
//     size_t N
// >
// class Array<bool, N, Storage>
// {
//     class Reference;

// public:
//     size_t Size() const
//     {
//         return N;
//     }

//     Array<bool, N, Storage>::Reference operator[](size_t index)
//     {
//         assert(0 <= index && index < N);
//         size_t bit_number = 7 - index % 8;
//         return Array<bool, N, Storage>::Reference(this->AccessData(index / 8), bit_number);
//     }

//     const Array<bool, N, Storage>::Reference operator[](size_t index) const
//     {
//         assert(0 <= index && index < N);
//         size_t bit_number = 7 - index % 8;
//         return Array<bool, N, Storage>::Reference(const_cast<uint8_t&>(this->AccessData(index / 8)), bit_number);
//     }

// private:
//     class Reference
//     {
//     public:
//         operator bool() const
//         {
//             return (byte_ >> bit_) & 0x1;
//         }

//         Reference& operator=(bool value)
//         {
//             byte_ &= ~(1 << bit_);
//             byte_ |= (1 << bit_) * value;
//             return *this;
//         }

//         Reference(uint8_t &byte, const uint8_t bit) :
//             byte_(byte), bit_(bit)
//         {
//             assert(0 <= bit && bit < 8);
//         }

//         uint8_t &byte_;
//         const uint8_t bit_;
//     };

//     Storage<uint8_t, N / 8 + !!(N % 8)> storage_;
// };

#endif
