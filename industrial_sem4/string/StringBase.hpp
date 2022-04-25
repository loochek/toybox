#ifndef STRING_BASE_HPP
#define STRING_BASE_HPP

#include <cstring>
#include <type_traits>
#include <stdexcept>

template<typename CharType, typename StringImpl>
class StringBase
{
    static_assert(std::is_fundamental_v<CharType>, "CharType must be fundamental type");

public:
    StringBase() noexcept : impl_(static_cast<StringImpl*>(this)) {}

    StringBase(const StringBase& /*other*/) : impl_(static_cast<StringImpl*>(this)) {}

    StringImpl& operator=(const char *str)
    {
        size_t size = strlen(str);
        impl_->Reserve(size);
        strcpy(impl_->Data(), str);
        impl_->size_ = size;

        return static_cast<StringImpl&>(*this);
    }

    CharType& Access(size_t index) noexcept
    {
        return impl_->Data()[index];
    }

    const CharType& Access(size_t index) const noexcept
    {
        return impl_->Data()[index];
    }

    CharType& operator[](size_t index)
    {
        if (index >= impl_->size_)
            throw std::logic_error("String index out of range");

        return Access(index);
    }

    const CharType& operator[](size_t index) const
    {
        if (index >= impl_->size_)
            throw std::logic_error("String index out of range");

        return Access(index);
    }

    void PushBack(CharType ch)
    {
        impl_->Reserve(impl_->size_ + 1);
        impl_->Data()[impl_->size_++] = ch;
    }

    void PopBack()
    {
        if (impl_->size_ == 0)
            throw std::logic_error("Trying to pop from empty string");

        impl_->Data()[--impl_->size_] = 0;
    }

    void Clear() noexcept
    {
        impl_->size_ = 0;
    }

    const CharType* CStr() const noexcept
    {
        return impl_->Data();
    }

    size_t Size() const noexcept
    {
        return impl_->size_;
    }

private:
    StringImpl *impl_;
};

template<typename String, bool IsConst>
class StringBaseIterator
{
public:
    using iterator_category = std::contiguous_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = typename String::ChType;
    using reference         = std::conditional_t<IsConst, const value_type&, value_type&>;

    StringBaseIterator() = delete;

    StringBaseIterator(String *string, size_t index) : string_(string), index_(index) {}

    StringBaseIterator(const String *string, size_t index) : string_(const_cast<String*>(string)), index_(index)
    {
        static_assert(IsConst, "Cannot initialize const iterator with non-const string");
    }

    // Prefix
    StringBaseIterator& operator++()
    {
        operator+=(1);
        return *this;
    }

    // Postfix
    StringBaseIterator operator++(int /*dummy*/)
    {
        StringBaseIterator old_iter = *this;
        operator+=(1);
        return old_iter;
    }

    // Prefix
    StringBaseIterator& operator--()
    {
        operator-=(1);
        return *this;
    }

    // Postfix
    StringBaseIterator operator--(int /*dummy*/)
    {
        StringBaseIterator old_iter = *this;
        operator-=(1);
        return old_iter;
    }

    StringBaseIterator& operator+=(difference_type diff)
    {
        if (index_ + diff > string_->Size())
            throw std::out_of_range("Iterator out of range");

        index_ += diff;
        return *this;
    }

    StringBaseIterator& operator-=(difference_type diff)
    {
        if (index_ - diff < 0)
            throw std::out_of_range("Iterator out of range");

        index_ -= diff;
        return *this;
    }

    StringBaseIterator operator+(difference_type diff) const
    {
        if (index_ + diff > string_->Size())
            throw std::out_of_range("Iterator out of range");

        return StringBaseIterator(string_, index_ + diff);
    }

    StringBaseIterator operator-(difference_type diff) const
    {
        if (index_ - diff < 0)
            throw std::out_of_range("Iterator out of range");

        return StringBaseIterator(string_, index_ - diff);
    }

    reference operator*() const
    {
        return string_->operator[](index_);
    }

    template<bool IsConst_>
    ptrdiff_t operator-(const StringBaseIterator<String, IsConst_> &other)
    {
        if (string_ != other.string_)
            throw std::logic_error("Trying to substract iterators from different strings");

        return index_ - other.index_;
    }

    template<bool IsConst_>
    bool operator==(const StringBaseIterator<String, IsConst_> &other)
    {
        if (string_ != other.string_)
            throw std::logic_error("Trying to compare iterators from different strings");

        return index_ == other.index_;
    }

    template<bool IsConst_>
    bool operator!=(const StringBaseIterator<String, IsConst_> &other)
    {
        return !(*this == other);
    }

    template<bool IsConst_>
    bool operator<(const StringBaseIterator<String, IsConst_> &other)
    {
        if (string_ != other.string_)
            throw std::logic_error("Trying to compare iterators from different strings");

        return index_ < other.index_;
    }

    template<bool IsConst_>
    bool operator<=(const StringBaseIterator<String, IsConst_> &other)
    {
        if (index_ != other.index_)
            throw std::logic_error("Trying to compare iterators from different strings");

        return index_ <= other.index_;
    }

    template<bool IsConst_>
    bool operator>(const StringBaseIterator<String, IsConst_> &other)
    {
        return !(*this <= other);
    }

    template<bool IsConst_>
    bool operator>=(const StringBaseIterator<String, IsConst_> &other)
    {
        return !(*this < other);
    }

    friend StringBaseIterator<String, !IsConst>;

private:
    String *string_;
    size_t index_;
};

template<typename String>
const typename String::ChType& Front(const String &str)
{
    return str[0];
}

template<typename String>
typename String::ChType& Front(String &str)
{
    return str[0];
}

template<typename String>
const typename String::ChType& Back(const String &str)
{
    return str[str.Size() - 1];
}

template<typename String>
typename String::ChType& Back(String &str)
{
    return str[str.Size() - 1];
}

template<typename String>
auto Begin(String &str)
{
    return StringBaseIterator<String, false>(&str, 0);
}

template<typename String>
auto Begin(const String &str)
{
    return CBegin(str);
}

template<typename String>
auto CBegin(const String &str)
{
    return StringBaseIterator<String, true>(&str, str.Size());
}

template<typename String>
auto End(String &str)
{
    return StringBaseIterator<String, false>(&str, str.Size());
}

template<typename String>
auto End(const String &str)
{
    return CEnd(str);
}

template<typename String>
auto CEnd(const String &str)
{
    return StringBaseIterator<String, true>(&str, str.Size());
}

#endif
