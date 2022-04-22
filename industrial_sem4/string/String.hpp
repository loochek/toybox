#ifndef STRING_HPP
#define STRING_HPP

#include "StringBase.hpp"

template<typename CharType = char, template<typename T_> class Allocator = std::allocator>
class String : public StringBase<CharType, String<CharType>>
{
    using Base = StringBase<CharType, String<CharType>>;
    friend Base;

    using alloc = std::allocator_traits<Allocator<CharType>>;

    static constexpr size_t MINIMAL_CAPACITY = 16;

public:
    // using StringBase<CharType, String<CharType>>::operator=;

    String() : data_(nullptr), size_(0), capacity_(0) {}

    String(size_t size) : data_(nullptr), size_(size), capacity_(0)
    {
        Reserve(size + 1);
    }

    String(const String &other) : data_(nullptr), size_(other.size_), capacity_(0)
    {
        Reserve(other.size_ + 1);
        memcpy(data_, other.data_, sizeof(CharType) * other.size_);
    }

    String(String &&other) : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~String()
    {
        alloc_.deallocate(data_, capacity_);
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    String& operator=(const String &other)
    {
        Base::Clear();
        Reserve(other.size_ + 1);
        memcpy(data_, other.data_, sizeof(CharType) * other.size_);

        size_ = other.size_;
    }

    String& operator=(String &&other)
    {
        this->~String();
        data_ = other.data_;
        data_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    void Reserve(size_t desired_capacity)
    {
        if (desired_capacity <= capacity_)
            return;

        size_t new_capacity = 1;
        while (new_capacity < desired_capacity)
            new_capacity <<= 1;

        SetCapacity(new_capacity);   
    }

    void Shrink()
    {
        size_t needed_capacity = 1;
        while (needed_capacity < size_ + 1)
            needed_capacity <<= 1;

        if (needed_capacity == capacity_)
            return;

        SetCapacity(needed_capacity);
    }

private:
    void SetCapacity(size_t new_capacity)
    {
        assert((new_capacity != 0) && !(new_capacity & (new_capacity - 1)));
        assert(new_capacity >= size_);
        new_capacity = std::max(new_capacity, size_t(MINIMAL_CAPACITY));

        CharType* new_data = alloc::allocate(alloc_, new_capacity);
        memcpy(new_data, data_, sizeof(CharType) * size_);

        if (data_ != nullptr)
            alloc::deallocate(alloc_, data_, capacity_);

        data_ = new_data;
        capacity_ = new_capacity;
    }

private:
    CharType* data_;
    size_t size_;
    size_t capacity_;

    Allocator<CharType> alloc_;
};

#endif
