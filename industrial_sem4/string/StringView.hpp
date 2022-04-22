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

    StringView(CharType* buffer, size_t size) noexcept :
        data_(buffer), size_(0), capacity_(size)
    {
        assert(size != 0);
        memset(buffer, 0, sizeof(CharType) * size);
    }

    explicit StringView(CharType* str) noexcept :
        data_(str), size_(strlen(str)), capacity_(size_ + 1) {}

    StringView(const StringView &other) noexcept = default;

    ~StringView()
    {
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

private:
    CharType* Data() noexcept
    {
        return data_;
    }

    const CharType* Data() const noexcept
    {
        return data_;
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

#endif
