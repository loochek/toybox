#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

#include <cassert>
#include <type_traits>
#include "StringBase.hpp"

template<typename CharType = char>
class StringView : public StringBase<CharType, StringView<CharType>>
{
    friend StringBase<CharType, StringView<CharType>>;

public:
    using StringBase<CharType, StringView<CharType>>::operator=;

    StringView(CharType* buffer, size_t size) noexcept : data_(buffer), size_(size - 1), capacity_(size)
    {
        assert(size != 0);
    }

    explicit StringView(CharType* str) noexcept : data_(str), size_(strlen(str)), capacity_(size_ + 1) {}

    StringView(const StringView &other) noexcept = default;

    ~StringView()
    {
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

private:
    void Reserve(size_t desired_capacity)
    {
        if (desired_capacity > capacity_)
            throw std::logic_error("StringView capacity exceeded");
    }

private:
    CharType* data_;

    size_t size_;
    size_t capacity_;
};

#endif
