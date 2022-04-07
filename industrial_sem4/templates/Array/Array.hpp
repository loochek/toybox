#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <initializer_list>
#include <type_traits>
#include "StaticStorage.hpp"
#include "DynamicStorage.hpp"
#include "ChunkedStorage.hpp"

template<typename Array, typename T>
class ArrayConstIterator;

template<typename Array, typename T>
class ArrayIterator
{
public:
    struct ArrayIteratorTag {};

    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    ArrayIterator() = delete;

    // Prefix
    ArrayIterator operator++()
    {
        operator+=(1);
        return *this;
    }

    // Postfix
    ArrayIterator operator++(int dummy)
    {
        ArrayIterator old_iter = *this;
        operator+=(1);
        return old_iter;
    }

    // Prefix
    ArrayIterator operator--()
    {
        operator-=(1);
        return *this;
    }

    // Postfix
    ArrayIterator operator--(int dummy)
    {
        ArrayIterator old_iter = *this;
        operator-=(1);
        return old_iter;
    }

    ArrayIterator operator+=(difference_type diff)
    {
        if (index_ + diff > array_->Size())
            throw std::out_of_range("Iterator out of range");

        index_ += diff;
        return *this;
    }

    ArrayIterator operator-=(difference_type diff)
    {
        if (index_ - diff < 0)
            throw std::out_of_range("Iterator out of range");

        index_ -= diff;
        return *this;
    }

    ArrayIterator operator+(difference_type diff) const
    {
        if (index_ + diff > array_->Size())
            throw std::out_of_range("Iterator out of range");

        return ArrayIterator(array_, index_ + diff);
    }

    ArrayIterator operator-(difference_type diff) const
    {
        if (index_ - diff < 0)
            throw std::out_of_range("Iterator out of range");

        return ArrayIterator(array_, index_ - diff);
    }

    T& operator*() const
    {
        return array_->operator[](index_);
    }

    T* operator->() const
    {
        return &array_->operator[](index_);
    }

private:
    ArrayIterator(Array *array, size_t index) : array_(array), index_(index) {}

    friend Array;
    friend class ArrayConstIterator<Array, T>;

        template
    <
        typename ITER1,
        typename ITER2,
        typename ITER1::ArrayIteratorTag*,
        typename ITER2::ArrayIteratorTag*
    >
    friend bool operator==(const ITER1 &a, const ITER2 &b);

    template
    <
        typename ITER1,
        typename ITER2,
        typename ITER1::ArrayIteratorTag*,
        typename ITER2::ArrayIteratorTag*
    >
    friend bool operator<(const ITER1 &a, const ITER2 &b);

    template
    <
        typename ITER1,
        typename ITER2,
        typename ITER1::ArrayIteratorTag*,
        typename ITER2::ArrayIteratorTag*
    >
    friend bool operator<=(const ITER1 &a, const ITER2 &b);

    template
    <
        typename ITER1,
        typename ITER2,
        typename ITER1::ArrayIteratorTag*,
        typename ITER2::ArrayIteratorTag*
    >
    friend ptrdiff_t operator-(const ITER1 &a, const ITER2 &b);

private:
    Array *array_;
    size_t index_;
};

template<typename Array, typename T>
class ArrayConstIterator
{
public:
    struct ArrayIteratorTag {};

    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = const T;
    using pointer           = const T*;
    using reference         = const T&;

    ArrayConstIterator() = delete;

    ArrayConstIterator(const ArrayIterator<Array, T> &iter) : array_(iter.array_), index_(iter.index_) {}

    // Prefix
    ArrayConstIterator operator++()
    {
        operator+=(1);
        return *this;
    }

    // Postfix
    ArrayConstIterator operator++(int dummy)
    {
        ArrayConstIterator old_iter = *this;
        operator+=(1);
        return old_iter;
    }

    // Prefix
    ArrayConstIterator operator--()
    {
        operator-=(1);
        return *this;
    }

    // Postfix
    ArrayConstIterator operator--(int dummy)
    {
        ArrayConstIterator old_iter = *this;
        operator-=(1);
        return old_iter;
    }

    ArrayConstIterator operator+=(difference_type diff)
    {
        if (index_ + diff > array_->Size())
            throw std::out_of_range("Iterator out of range");

        index_ += diff;
        return *this;
    }

    ArrayConstIterator operator-=(difference_type diff)
    {
        if (index_ - diff < 0)
            throw std::out_of_range("Iterator out of range");

        index_ -= diff;
        return *this;
    }

    ArrayConstIterator operator+(difference_type diff) const
    {
        if (index_ + diff > array_->Size())
            throw std::out_of_range("Iterator out of range");

        return ArrayConstIterator(array_, index_ + diff);
    }

    ArrayConstIterator operator-(difference_type diff) const
    {
        if (index_ - diff < 0)
            throw std::out_of_range("Iterator out of range");

        return ArrayConstIterator(array_, index_ - diff);
    }

    const T& operator*() const
    {
        return array_->operator[](index_);
    }

    const T* operator->() const
    {
        return &array_->operator[](index_);
    }

private:
    ArrayConstIterator(const Array *array, size_t index) : array_(array), index_(index) {}

    friend Array;

    template
    <
        typename ITER1,
        typename ITER2,
        typename ITER1::ArrayIteratorTag*,
        typename ITER2::ArrayIteratorTag*
    >
    friend bool operator==(const ITER1 &a, const ITER2 &b);

    template
    <
        typename ITER1,
        typename ITER2,
        typename ITER1::ArrayIteratorTag*,
        typename ITER2::ArrayIteratorTag*
    >
    friend bool operator<(const ITER1 &a, const ITER2 &b);

    template
    <
        typename ITER1,
        typename ITER2,
        typename ITER1::ArrayIteratorTag*,
        typename ITER2::ArrayIteratorTag*
    >
    friend bool operator<=(const ITER1 &a, const ITER2 &b);

    template
    <
        typename ITER1,
        typename ITER2,
        typename ITER1::ArrayIteratorTag*,
        typename ITER2::ArrayIteratorTag*
    >
    friend ptrdiff_t operator-(const ITER1 &a, const ITER2 &b);

private:
    const Array *array_;
    size_t index_;
};

template
<
    typename ITER1,
    typename ITER2,
    typename ITER1::ArrayIteratorTag* = nullptr,
    typename ITER2::ArrayIteratorTag* = nullptr
>
ptrdiff_t operator-(const ITER1 &a, const ITER2 &b)
{
    if (a.array_ != b.array_)
        throw std::logic_error("Trying to substract iterators from different arrays");

    return a.index_ - b.index_;
}

template
<
    typename ITER1,
    typename ITER2,
    typename ITER1::ArrayIteratorTag* = nullptr,
    typename ITER2::ArrayIteratorTag* = nullptr
>
bool operator==(const ITER1 &a, const ITER2 &b)
{
    if (a.array_ != b.array_)
        throw std::logic_error("Trying to compare iterators from different arrays");

    return a.index_ == b.index_;
}

template
<
    typename ITER1,
    typename ITER2,
    typename ITER1::ArrayIteratorTag* = nullptr,
    typename ITER2::ArrayIteratorTag* = nullptr
>
bool operator!=(const ITER1 &a, const ITER2 &b)
{
    return !(a == b);
}

template
<
    typename ITER1,
    typename ITER2,
    typename ITER1::ArrayIteratorTag* = nullptr,
    typename ITER2::ArrayIteratorTag* = nullptr
>
bool operator<(const ITER1 &a, const ITER2 &b)
{
    if (a.array_ != b.array_)
        throw std::logic_error("Trying to compare iterators from different arrays");

    return a.index_ < b.index_;
}

template
<
    typename ITER1,
    typename ITER2,
    typename ITER1::ArrayIteratorTag* = nullptr,
    typename ITER2::ArrayIteratorTag* = nullptr
>
bool operator<=(const ITER1 &a, const ITER2 &b)
{
    if (a.array_ != b.array_)
        throw std::logic_error("Trying to compare iterators from different arrays");

    return a.index_ <= b.index_;
}

template
<
    typename ITER1,
    typename ITER2,
    typename ITER1::ArrayIteratorTag* = nullptr,
    typename ITER2::ArrayIteratorTag* = nullptr
>
bool operator>(const ITER1 &a, const ITER2 &b)
{
    return !(a <= b);
}

template
<
    typename ITER1,
    typename ITER2,
    typename ITER1::ArrayIteratorTag* = nullptr,
    typename ITER2::ArrayIteratorTag* = nullptr
>
bool operator>=(const ITER1 &a, const ITER2 &b)
{
    return !(a < b);
}

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

    inline ArrayIterator<Array, T> begin()
    {
        return ArrayIterator<Array, T>(this, 0);
    }

    inline ArrayConstIterator<Array, T> begin() const
    {
        return cbegin();
    }

    inline ArrayConstIterator<Array, T> cbegin() const
    {
        return ArrayConstIterator<Array, T>(this, 0);
    }

    inline ArrayIterator<Array, T> end()
    {
        return ArrayIterator<Array, T>(this, Size());
    }

    inline ArrayConstIterator<Array, T> end() const
    {
        return cend();
    }

    inline ArrayConstIterator<Array, T> cend() const
    {
        return ArrayConstIterator<Array, T>(this, Size());
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

    inline Reference operator[](size_t index) noexcept
    {
        size_t bit_number = BITS - index % BITS - 1;
        return Reference(storage_.Access(index / BITS), bit_number);
    }

    inline const Reference operator[](size_t index) const noexcept
    {
        size_t bit_number = BITS - index % BITS - 1;
        return Reference(const_cast<uint8_t&>(storage_.Access(index / BITS)), bit_number);
    }

    inline Reference Back() noexcept
    {
        return operator[](bool_size_ - 1);
    }

    inline const Reference Back() const noexcept
    {
        return operator[](bool_size_ - 1);
    }

    inline Reference At(size_t index)
    {
        if (index >= bool_size_)
            throw std::out_of_range("Array index out of range");

        return operator[](index);
    }

    inline const Reference At(size_t index) const
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
