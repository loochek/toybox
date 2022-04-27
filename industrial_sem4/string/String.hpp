#ifndef STRING_HPP
#define STRING_HPP

#include <cassert>
#include "StringBase.hpp"
#include "StringView.hpp"

// TODO: move allocator to buffer

template<typename CharType>
struct StringDynamicBuffer
{
    size_t ref_count_;
    size_t capacity_;
    CharType data_[0];
};

template<typename CharType = char, template<typename T_> class Allocator = std::allocator>
class String : public StringBase<CharType, String<CharType>, true>
{
    using Base = StringBase<CharType, String<CharType>, true>;
    friend Base;

    using alloc = std::allocator_traits<Allocator<uint8_t>>;

    static constexpr size_t SSO_CAPACITY = sizeof(void*) / sizeof(CharType);
    static constexpr size_t MINIMAL_DYN_CAPACITY = 16;
    static_assert(SSO_CAPACITY > 0);
    static_assert(SSO_CAPACITY < MINIMAL_DYN_CAPACITY);

public:
    using Base::operator=;

    String() : buffer_(nullptr), sso_(true), size_(0) {}

    String(size_t size) : String()
    {
        Reserve(size);
    }

    String(const String &other) : buffer_(other.buffer_), sso_(false), size_(other.size_)
    {
        other.buffer_->ref_count_++;
    }

    String(String &&other) :
        buffer_(other.buffer_), size_(other.size_), alloc_(std::move(other.alloc_))
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
        if (!sso_ && buffer_->ref_count_ == 1)
            alloc_.deallocate((uint8_t*)buffer_, CalculateBufferSize(buffer_->capacity_));
        else
            buffer_->ref_count_--;
            
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
        std::swap(alloc_, other.alloc_);
    }

private:
    CharType* MutableData() noexcept
    {
        EnsureWriteability();
        return const_cast<CharType*>(static_cast<const String*>(this)->ConstData());
    }

    const CharType* ConstData() const noexcept
    {
        return sso_ ? sso_data_ : buffer_->data_;
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
        return sso_ ? SSO_CAPACITY : buffer_->capacity_;
    }

    void SetCapacity(size_t new_capacity)
    {
        assert((new_capacity != 0) && !(new_capacity & (new_capacity - 1)));
        assert(new_capacity >= size_);

        if (new_capacity <= SSO_CAPACITY)
            return;

        new_capacity = std::max(new_capacity, size_t(MINIMAL_DYN_CAPACITY));

        StringDynamicBuffer<CharType> *new_buffer = nullptr;
        if (sso_)
        {
            // SSO -> dynamic transition occured
            new_buffer = CreateBuffer(new_capacity, false);
            memcpy(new_buffer->data_, sso_data_, sizeof(CharType) * (size_ + 1));
            sso_ = false;
        }
        else
        {
            // Reallocation / unshare
            new_buffer = CreateBuffer(new_capacity, true);
            if (buffer_->ref_count_ == 0)
                alloc::deallocate(alloc_, (uint8_t*)buffer_, CalculateBufferSize(buffer_->capacity_));
        }

        buffer_ = new_buffer;
    }

    StringDynamicBuffer<CharType>* CreateBuffer(size_t capacity, bool copy_from_old)
    {
        assert((capacity != 0) && !(capacity & (capacity - 1)));

        StringDynamicBuffer<CharType> *new_buffer = nullptr;
        if (copy_from_old)
        {
            assert(buffer_ != nullptr);
            size_t alloc_size = CalculateBufferSize(buffer_->capacity_);
            new_buffer = (StringDynamicBuffer<CharType>*)alloc::allocate(alloc_, alloc_size);
            memcpy(new_buffer, buffer_, alloc_size);
            buffer_->ref_count_--;
        }
        else
        {
            new_buffer = (StringDynamicBuffer<CharType>*)alloc::allocate(alloc_, CalculateBufferSize(capacity));
            memset(new_buffer, 0, CalculateBufferSize(capacity));
            new_buffer->capacity_ = capacity;
        }

        new_buffer->ref_count_ = 1;
        return new_buffer;
    }

    void EnsureWriteability()
    {
        if (!sso_ && buffer_->ref_count_ > 1)
        {
            // Unshare
            SetCapacity(buffer_->capacity_);
        }
    }

    static constexpr size_t CalculateBufferSize(size_t capacity)
    {
        return sizeof(StringDynamicBuffer<CharType>) + sizeof(CharType) * capacity;
    }

private:
    union
    {
        StringDynamicBuffer<CharType> *buffer_;
        CharType sso_data_[0];
    };

    bool sso_;
    size_t size_;
    Allocator<uint8_t> alloc_;
};

#endif
