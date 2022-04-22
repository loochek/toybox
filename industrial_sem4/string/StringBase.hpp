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

    StringBase(const StringBase &other) : impl_(static_cast<StringImpl*>(this)) {}

    StringImpl& operator=(const char *str)
    {
        size_t size = strlen(str);
        impl_->Reserve(size + 1);
        strcpy(impl_->data_, str);
        impl_->size_ = size;

        return static_cast<StringImpl&>(*this);
    }

    CharType& Access(size_t index) noexcept
    {
        return impl_->data_[index];
    }

    const CharType& Access(size_t index) const noexcept
    {
        return impl_->data_[index];
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
        impl_->Reserve(impl_->size_ + 2); // Null terminator
        impl_->Data()[impl_->size_++] = ch;
    }

    void PopBack() noexcept
    {
        impl_->data_[impl_->size_--] = 0;
    }

    void Clear() noexcept
    {
        impl_->size_ = 0;
    }

    const CharType* CStr() const noexcept
    {
        return impl_->data_;
    }

    size_t Size() const noexcept
    {
        return impl_->size_;
    }

private:
    StringImpl *impl_;
};

template<typename CharType, typename StorageProvider>
CharType &Front(StringBase<CharType, StorageProvider> &str)
{
    return str[0];
}

template<typename CharType, typename StorageProvider>
CharType &Back(StringBase<CharType, StorageProvider> &str)
{
    return str[str.Size() - 1];
}

#endif
