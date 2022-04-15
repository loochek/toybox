#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <initializer_list>
#include <type_traits>
#include "StaticStorage.hpp"
#include "DynamicStorage.hpp"
#include "ChunkedStorage.hpp"

template<typename Array, bool IsConst>
class ArrayIteratorBase
{
public:
    using iterator_category = Array::IteratorCategory;
    using difference_type   = Array::DifferenceType;
    using value_type        = Array::ValueType;
    using reference         = std::conditional_t<IsConst, typename Array::ConstReference,
                                                 typename Array::Reference>;

    ArrayIteratorBase() = delete;

    // Prefix
    ArrayIteratorBase operator++()
    {
        operator+=(1);
        return *this;
    }

    // Postfix
    ArrayIteratorBase operator++(int dummy)
    {
        ArrayIteratorBase old_iter = *this;
        operator+=(1);
        return old_iter;
    }

    // Prefix
    ArrayIteratorBase operator--()
    {
        operator-=(1);
        return *this;
    }

    // Postfix
    ArrayIteratorBase operator--(int dummy)
    {
        ArrayIteratorBase old_iter = *this;
        operator-=(1);
        return old_iter;
    }

    ArrayIteratorBase operator+=(difference_type diff)
    {
        if (index_ + diff > array_->Size())
            throw std::out_of_range("Iterator out of range");

        index_ += diff;
        return *this;
    }

    ArrayIteratorBase operator-=(difference_type diff)
    {
        if (index_ - diff < 0)
            throw std::out_of_range("Iterator out of range");

        index_ -= diff;
        return *this;
    }

    ArrayIteratorBase operator+(difference_type diff) const
    {
        if (index_ + diff > array_->Size())
            throw std::out_of_range("Iterator out of range");

        return ArrayIteratorBase(array_, index_ + diff);
    }

    ArrayIteratorBase operator-(difference_type diff) const
    {
        if (index_ - diff < 0)
            throw std::out_of_range("Iterator out of range");

        return ArrayIteratorBase(array_, index_ - diff);
    }

    reference operator*() const
    {
        return array_->operator[](index_);
    }

    template<typename Array_ = Array>
    typename Array_::Pointer operator->() const
    {
        return &array_->operator[](index_);
    }

    template<bool IsConst_>
    ptrdiff_t operator-(const ArrayIteratorBase<Array, IsConst_> &other)
    {
        if (array_ != other.array_)
            throw std::logic_error("Trying to substract iterators from different arrays");

        return index_ - other.index_;
    }

    template<bool IsConst_>
    bool operator==(const ArrayIteratorBase<Array, IsConst_> &other)
    {
        if (array_ != other.array_)
            throw std::logic_error("Trying to compare iterators from different arrays");

        return index_ == other.index_;
    }

    template<bool IsConst_>
    bool operator!=(const ArrayIteratorBase<Array, IsConst_> &other)
    {
        return !(*this == other);
    }

    template<bool IsConst_>
    bool operator<(const ArrayIteratorBase<Array, IsConst_> &other)
    {
        if (array_ != other.array_)
            throw std::logic_error("Trying to compare iterators from different arrays");

        return index_ < other.index_;
    }

    template<bool IsConst_>
    bool operator<=(const ArrayIteratorBase<Array, IsConst_> &other)
    {
        if (index_ != other.index_)
            throw std::logic_error("Trying to compare iterators from different arrays");

        return index_ <= other.index_;
    }

    template<bool IsConst_>
    bool operator>(const ArrayIteratorBase<Array, IsConst_> &other)
    {
        return !(*this <= other);
    }

    template<bool IsConst_>
    bool operator>=(const ArrayIteratorBase<Array, IsConst_> &other)
    {
        return !(*this < other);
    }

private:
    ArrayIteratorBase(Array *array, size_t index) : array_(array), index_(index) {}
    ArrayIteratorBase(const Array *array, size_t index) : array_(const_cast<Array*>(array)), index_(index) {}

    friend Array;
    friend ArrayIteratorBase<Array, !IsConst>;

private:
    Array *array_;
    size_t index_;
};

template<typename Array>
using ArrayIterator = ArrayIteratorBase<Array, false>;

template<typename Array>
using ArrayConstIterator = ArrayIteratorBase<Array, true>;

template
<
    typename T,
    template<typename T_, template<typename T__> class Allocator_, size_t StaticSize_> class Storage = DynamicStorage,
    template<typename T_> class Allocator = std::allocator,
    size_t StaticSize = 0
>
class Array
{
public:
    using IteratorCategory = std::conditional_t<Storage<T, Allocator, StaticSize>::IsContiguous::value,
                                                std::random_access_iterator_tag, std::contiguous_iterator_tag>;
    using DifferenceType   = std::ptrdiff_t;
    using ValueType        = T;
    using Pointer          = T*;
    using ConstPointer     = const T*;
    using Reference        = T&;
    using ConstReference   = const T&;

    Array() : storage_() {}
    explicit Array(size_t size) : storage_(size) {}

    Array(const std::initializer_list<ValueType> &list)
    {
        storage_.Reserve(list.size());
        for (ConstReference elem : list)
            new (&storage_.ReserveBack()) ValueType(elem);
    }

    Array(const Array &other) = default;
    Array(Array &&other) noexcept(std::is_nothrow_move_constructible_v<Storage<ValueType, Allocator, StaticSize>>) = default;

    Array& operator=(const Array &other)
    {
        storage_ = other.storage_;
        return *this;
    }

    Array& operator=(Array &&other) noexcept(std::is_nothrow_move_constructible_v<Storage<ValueType, Allocator, StaticSize>>)
    {
        storage_ = std::move(other.storage_);
        return *this;
    }

    Array& operator=(const std::initializer_list<ValueType> &list)
    {
        storage_.Clear();
        storage_.Reserve(list.size());
        for (ConstReference elem : list)
            new (&storage_.ReserveBack()) ValueType(elem);

        return *this;
    }

    inline Reference operator[](size_t index) noexcept
    {
        return const_cast<Reference>(static_cast<const Array*>(this)->operator[](index));
    }

    inline ConstReference operator[](size_t index) const noexcept
    {
        return storage_.Access(index);
    }

    inline Reference Back() noexcept
    {
        return const_cast<Reference>(static_cast<const Array*>(this)->Back());
    }

    inline ConstReference Back() const noexcept
    {
        return storage_.Access(storage_.Size() - 1);
    }

    inline Reference At(size_t index)
    {
        return const_cast<Reference>(static_cast<const Array*>(this)->At(index));
    }

    inline ConstReference At(size_t index) const
    {
        if (index >= storage_.Size())
            throw std::out_of_range("Array index out of range");

        return storage_.Access(index);
    }

    inline Pointer Data() noexcept
    {
        return storage_.Data();
    }

    inline ConstPointer Data() const noexcept
    {
        return storage_.Data();
    }

    inline void PushBack(ValueType elem)
    {
        storage_.ReserveBack() = std::move(elem);
    }

    template<typename... Args>
    inline void EmplaceBack(Args&&... args)
    {
        ValueType &place = storage_.ReserveBack();

        try
        {
            new (&place) ValueType(std::forward<Args>(args)...);
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

    inline ArrayIterator<Array> begin()
    {
        return ArrayIterator<Array>(this, 0);
    }

    inline ArrayConstIterator<Array> begin() const
    {
        return cbegin();
    }

    inline ArrayConstIterator<Array> cbegin() const
    {
        return ArrayConstIterator<Array>(this, 0);
    }

    inline ArrayIterator<Array> end()
    {
        return ArrayIterator<Array>(this, Size());
    }

    inline ArrayConstIterator<Array> end() const
    {
        return cend();
    }

    inline ArrayConstIterator<Array> cend() const
    {
        return ArrayConstIterator<Array>(this, Size());
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
    Storage<ValueType, Allocator, StaticSize> storage_;
};

template
<
    template<typename T_, template<typename T__> class Allocator_, size_t StaticSize_> class Storage,
    template<typename T_> class Allocator,
    size_t StaticSize
>
class Array<bool, Storage, Allocator, StaticSize>
{
    static constexpr size_t BITS = 8; // Bits count in uint8_t

    static constexpr size_t CalculateSpace(size_t capacity)
    {
        return capacity / BITS + !!(capacity % BITS);
    }

    class BoolReference;

public:

    using IteratorCategory = std::conditional_t<Storage<uint8_t, Allocator, CalculateSpace(StaticSize)>::IsContiguous::value,
                                                std::random_access_iterator_tag, std::contiguous_iterator_tag>;
    using DifferenceType   = std::ptrdiff_t;
    using ValueType        = bool;
    using Reference        = BoolReference;
    using ConstReference   = const BoolReference;

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

    inline BoolReference operator[](size_t index) noexcept
    {
        size_t bit_number = BITS - index % BITS - 1;
        return BoolReference(storage_.Access(index / BITS), bit_number);
    }

    inline const BoolReference operator[](size_t index) const noexcept
    {
        size_t bit_number = BITS - index % BITS - 1;
        return BoolReference(const_cast<uint8_t&>(storage_.Access(index / BITS)), bit_number);
    }

    inline BoolReference Back() noexcept
    {
        return operator[](bool_size_ - 1);
    }

    inline const BoolReference Back() const noexcept
    {
        return operator[](bool_size_ - 1);
    }

    inline BoolReference At(size_t index)
    {
        if (index >= bool_size_)
            throw std::out_of_range("Array index out of range");

        return operator[](index);
    }

    inline const BoolReference At(size_t index) const
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

    inline ArrayIterator<Array> begin()
    {
        return ArrayIterator<Array>(this, 0);
    }

    inline ArrayConstIterator<Array> begin() const
    {
        return cbegin();
    }

    inline ArrayConstIterator<Array> cbegin() const
    {
        return ArrayConstIterator<Array>(this, 0);
    }

    inline ArrayIterator<Array> end()
    {
        return ArrayIterator<Array>(this, Size());
    }

    inline ArrayConstIterator<Array> end() const
    {
        return cend();
    }

    inline ArrayConstIterator<Array> cend() const
    {
        return ArrayConstIterator<Array>(this, Size());
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
    class BoolReference
    {
    public:
        BoolReference(uint8_t &byte, uint8_t bit) noexcept:
            byte_(byte), bit_(bit)
        {
            assert(0 <= bit && bit < BITS);
        }

        BoolReference(const BoolReference &other) noexcept : byte_(other.byte_), bit_(other.bit_) {}
        BoolReference(BoolReference &&other) noexcept : byte_(other.byte_), bit_(other.bit_) {}

        BoolReference& operator=(bool value) noexcept
        {
            byte_ &= ~(1 << bit_);
            byte_ |= (1 << bit_) * value;
            return *this;
        }

        BoolReference& operator=(const BoolReference &other) noexcept
        {
            operator=(bool(other));
            return *this;
        }

        operator bool() const noexcept
        {
            return (byte_ >> bit_) & 0x1;
        }

    private:
        uint8_t &byte_;
        uint8_t bit_;
    };

private:
    Storage<uint8_t, Allocator, CalculateSpace(StaticSize)> storage_;
    size_t bool_size_;
};

template<typename T, size_t Size>
using StaticArray = Array<T, StaticStorage, std::allocator, Size>; // std::allocator is not used really

template<typename T, template<typename T_> class Allocator = std::allocator>
using DynamicArray = Array<T, DynamicStorage, Allocator, 0>;

template<typename T, template<typename T_> class Allocator = std::allocator>
using ChunkedArray = Array<T, ChunkedStorage, Allocator, 0>;

// namespace std
// {
//     void swap(typename Array<bool, DynamicStorage, 0>::BoolReference a, typename Array<bool, DynamicStorage, 0>::BoolReference b)
//     {
//         // std::swap(a, b);
//     }
// }

#endif
