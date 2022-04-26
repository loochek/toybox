#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

#include <cassert>
#include <type_traits>
#include "StringBase.hpp"

template<bool Mutable, typename CharType = char>
class StringView : public StringBase<CharType, StringView<Mutable, CharType>, Mutable>
{
    using Base = StringBase<CharType, StringView<Mutable, CharType>, Mutable>;
    friend Base;

public:
    using Base::operator=;

    // New non-owning string
    StringView(const CharType* buffer, size_t capacity) noexcept :
        data_(const_cast<CharType*>(buffer)), size_(0), capacity_(capacity)
    {
        assert(capacity != 0);
        memset(data_, 0, sizeof(CharType) * capacity_);
    }

    // Initialize from existing string
    StringView(const CharType* buffer, size_t capacity, size_t size) noexcept :
        data_(const_cast<CharType*>(buffer)), size_(size), capacity_(capacity)
    {
        assert(capacity != 0);
    }

    StringView(const StringView &other) noexcept = delete;

    StringView(StringView &&other) :
        data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~StringView()
    {
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    StringView& operator=(StringView &&other)
    {
        if (this != &other)
            StringView(std::move(other)).Swap(*this);
        
        return *this;
    }

    void Swap(StringView &other)
    {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
    }

private:
    template<bool Cond = Mutable, std::enable_if_t<Cond, int> = 0>
    CharType* Data() noexcept
    {
        return data_;
    }

    const CharType* Data() const noexcept
    {
        return data_;
    }

    template<bool Cond = Mutable, std::enable_if_t<Cond, int> = 0>
    size_t& ImplSize() noexcept
    {
        return size_;
    }

    const size_t& ImplSize() const noexcept
    {
        return size_;
    }

    void Reserve(size_t chars_count)
    {
        if (chars_count + 1 > capacity_) // Null terminator
            throw std::logic_error("StringView capacity exceeded");
    }

private:
    CharType* data_;

    size_t size_;
    size_t capacity_;
};

template<typename CharType = char>
using MutableStringView = StringView<true, CharType>;

template<typename CharType = char>
using ConstStringView = StringView<false, CharType>;

#endif
