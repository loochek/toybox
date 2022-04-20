#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

#include <type_traits>
#include "StringBase.hpp"

template<typename CharType = char>
class StringView : public StringBase<CharType, StringView<CharType>>
{
    friend StringBase<CharType, StringView<CharType>>;

public:
    using StringBase<CharType, StringView<CharType>>::operator=;

    StringView(CharType* buffer, size_t capacity) :
        StringBase<CharType, StringView<CharType>>(0), data_(buffer), capacity_(capacity) {}

    // TODO: refactor that shit
    explicit StringView(CharType* str) :
        StringBase<CharType, StringView<CharType>>(strlen(str)), data_(str), capacity_(strlen(str)) {}

private:
    void Reserve(size_t size)
    {
        if (size > capacity_)
            throw std::logic_error("StringView capacity exceeded");
    }

    CharType* Data() noexcept
    {
        return data_;
    }

    const CharType* Data() const noexcept
    {
        return data_;
    }

private:
    CharType* data_;
    size_t capacity_;
};

#endif
