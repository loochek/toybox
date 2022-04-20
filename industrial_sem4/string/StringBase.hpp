#ifndef STRING_BASE_HPP
#define STRING_BASE_HPP

#include <cstring>
#include <type_traits>
#include <stdexcept>

template<typename CharType, typename StorageProvider>
class StringBase
{
    static_assert(std::is_fundamental_v<CharType>, "CharType must be fundamental type");

public:
    StringBase() = delete;
    StringBase(size_t size) noexcept : size_(size), impl_(static_cast<StorageProvider*>(this)) {}

    StorageProvider& operator=(const char *str)
    {
        size_t size = strlen(str);
        impl_->Reserve(size);
        strcpy(impl_->Data(), str);
        size_ = size;

        return static_cast<StorageProvider&>(*this);
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
        if (index >= size_)
            throw std::logic_error("String index out of range");

        return Access(index);
    }

    const CharType& operator[](size_t index) const
    {
        if (index >= size_)
            throw std::logic_error("String index out of range");

        return Access(index);
    }

    void PushBack(CharType ch)
    {
        impl_->Reserve(size_ + 1);
        impl_->Data()[size_++] = ch;
    }

    void PopBack()
    {
        impl_->Data()[size_--] = 0;
    }

    const CharType* CStr() const noexcept
    {
        return impl_->Data();
    }

    size_t Size() const noexcept
    {
        return size_;
    }

protected:
    size_t size_;
    StorageProvider *impl_;
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
