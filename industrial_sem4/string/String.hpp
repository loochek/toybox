#ifndef STRING_HPP
#define STRING_HPP

#include "StringBase.hpp"

template<typename CharType = char, template<typename T_> class Allocator = std::allocator>
class String : public StringBase<CharType, String<CharType>>
{
    using Base = StringBase<CharType, String<CharType>>;
    friend Base;

    using alloc = std::allocator_traits<Allocator<CharType>>;

    static constexpr size_t SSO_CAPACITY = sizeof(void*) / sizeof(CharType);
    static constexpr size_t MINIMAL_DYN_CAPACITY = 16;
    static_assert(SSO_CAPACITY < MINIMAL_DYN_CAPACITY);

public:
    String() : data_(nullptr), size_(0), capacity_(0) {}

    String(size_t size) : String()
    {
        Reserve(size);
    }

    String(const String &other) : String()
    {
        Reserve(other.size_);
        memcpy(Data(), other.Data(), sizeof(CharType) * (other.size_ + 1));
        size_ = other.size_;
    }

    String(String &&other) : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    String(const char *str) : String()
    {
        Base::operator=(str);
    }

    ~String()
    {
        if (!IsSSOInUse())
            alloc_.deallocate(data_, capacity_);

        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    String& operator=(const String &other)
    {
        Base::Clear();
        Reserve(other.size_);
        memcpy(Data(), other.Data(), sizeof(CharType) * (other.size_ + 1));

        size_ = other.size_;
        return *this;
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

        return *this;
    }

    String& operator=(const char *str)
    {
        Base::operator=(str);
        return *this;
    }

    void Reserve(size_t chars_count)
    {
        chars_count++; // Null terminator
        if (chars_count <= capacity_)
            return;

        size_t new_capacity = 1;
        while (new_capacity < chars_count)
            new_capacity <<= 1;

        SetCapacity(new_capacity);
    }

    void Shrink()
    {
        size_t needed_capacity = 1;
        while (needed_capacity < size_ + 1)
            needed_capacity <<= 1;

        if (needed_capacity == capacity_ || needed_capacity < MINIMAL_DYN_CAPACITY)
            return;

        SetCapacity(needed_capacity);
    }

private:
    CharType* Data() noexcept
    {
        return IsSSOInUse() ? sso_data_ : data_;
    }

    const CharType* Data() const noexcept
    {
        return IsSSOInUse() ? sso_data_ : data_;
    }

    void SetCapacity(size_t new_capacity)
    {
        assert((new_capacity != 0) && !(new_capacity & (new_capacity - 1)));
        assert(new_capacity >= size_);

        if (new_capacity <= SSO_CAPACITY)
        {
            capacity_ = new_capacity;
            return;
        }

        new_capacity = std::max(new_capacity, size_t(MINIMAL_DYN_CAPACITY));

        CharType* new_data = alloc::allocate(alloc_, new_capacity);
        memset(new_data, 0, sizeof(CharType) * new_capacity);
        if (IsSSOInUse() && new_capacity > sizeof(void*) / sizeof(CharType))
        {
            // SSO -> dynamic transition occured
            memcpy(new_data, sso_data_, sizeof(CharType) * (size_ + 1));
        }
        else if (data_ != nullptr)
        {
            memcpy(new_data, data_, sizeof(CharType) * (size_ + 1));
            alloc::deallocate(alloc_, data_, capacity_);
        }

        data_ = new_data;
        capacity_ = new_capacity;
    }

    bool IsSSOInUse() const
    {
        return capacity_ <= sizeof(void*) / sizeof(CharType);
    }

private:
    union
    {
        CharType* data_;
        CharType sso_data_[0];
    };

    size_t size_;
    size_t capacity_;

    Allocator<CharType> alloc_;
};

#endif
