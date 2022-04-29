#ifndef STRING_HPP
#define STRING_HPP

#include <cassert>
#include "StringBase.hpp"
#include "StringDynamicBuffer.hpp"
#include "StringView.hpp"

template<typename CharType = char, template<typename T_> class Allocator = std::allocator>
class String : public StringBase<CharType, String<CharType>, true>
{
    using Base = StringBase<CharType, String<CharType>, true>;
    friend Base;

    using BufferType = StringDynamicBuffer<CharType, Allocator>;

    static constexpr size_t SSO_CAPACITY = sizeof(void*) / sizeof(CharType);
    static constexpr size_t MINIMAL_DYN_CAPACITY = 16;
    static_assert(SSO_CAPACITY > 0);
    static_assert(SSO_CAPACITY < MINIMAL_DYN_CAPACITY);

public:
    using Base::operator=;

    String() : buffer_(nullptr), sso_(true), size_(0) {}

    explicit String(size_t size) : String()
    {
        Reserve(size);
        size_ = size;
    }

    String(const String &other) : String()
    {
        if (other.sso_)
            memcpy(sso_data_, other.sso_data_, sizeof(CharType) * SSO_CAPACITY);
        else
        {
            sso_ = false;
            buffer_ = other.buffer_;
            other.buffer_->Ref();
        }

        size_ = other.size_;
    }

    String(String &&other) :
        buffer_(other.buffer_), size_(other.size_)
    {
        other.buffer_ = nullptr;
        other.sso_ = true;
        other.size_ = 0;
    }

    String(const CharType *str) : String()
    {
        Base::operator=(str);
    }

    ~String()
    {
        if (!sso_)
            buffer_->Unref();

        buffer_ = nullptr;
        sso_ = true;
        size_ = 0;
    }

    String& operator=(const String &other)
    {
        if (this != &other)
            String(other).Swap(*this);
        
        return *this;
    }

    String& operator=(String &&other)
    {
        if (this != &other)
            String(std::move(other)).Swap(*this);
        
        return *this;
    }

    // Remains valid until string modification!
    auto View() const noexcept
    {
        return ConstStringView<CharType>(ConstData(), Capacity(), size_);
    }

    void Reserve(size_t chars_count)
    {
        chars_count++; // Null terminator
        if (chars_count <= Capacity())
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

        if (needed_capacity == Capacity() || needed_capacity < MINIMAL_DYN_CAPACITY)
            return;

        SetCapacity(needed_capacity);
    }

    void Swap(String &other)
    {
        std::swap(buffer_, other.buffer_);
        std::swap(size_, other.size_);
    }

private:
    CharType* MutableData() noexcept
    {
        EnsureWriteability();
        return const_cast<CharType*>(static_cast<const String*>(this)->ConstData());
    }

    const CharType* ConstData() const noexcept
    {
        return sso_ ? sso_data_ : buffer_->Data();
    }

    size_t& ImplSize() noexcept
    {
        return size_;
    }

    const size_t& ImplSize() const noexcept
    {
        return size_;
    }

    size_t Capacity() const
    {
        return sso_ ? SSO_CAPACITY : buffer_->Capacity();
    }

    void SetCapacity(size_t new_capacity)
    {
        assert((new_capacity != 0) && !(new_capacity & (new_capacity - 1)));
        assert(new_capacity >= size_);

        if (new_capacity <= SSO_CAPACITY)
            return;

        new_capacity = std::max(new_capacity, size_t(MINIMAL_DYN_CAPACITY));

        BufferType *new_buffer = nullptr;
        if (sso_)
        {
            // SSO -> dynamic transition occured
            new_buffer = BufferType::CreateBuffer(new_capacity);
            memcpy(new_buffer->Data(), sso_data_, sizeof(CharType) * (size_ + 1));
            sso_ = false;
        }
        else
        {
            // Reallocation / unshare
            new_buffer = buffer_->Detach(new_capacity);
        }

        buffer_ = new_buffer;
    }

    void EnsureWriteability()
    {
        if (!sso_ && buffer_->Shared())
        {
            // Unshare
            buffer_ = buffer_->Detach();
        }
    }

private:
    union
    {
        StringDynamicBuffer<CharType, Allocator> *buffer_;
        CharType sso_data_[SSO_CAPACITY];
    };

    bool sso_;
    size_t size_;
};

#endif
