#ifndef STRING_DYNAMIC_BUFFER_HPP
#define STRING_DYNAMIC_BUFFER_HPP

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <memory>

template<typename CharType, template<typename T_> class Allocator>
class StringDynamicBuffer
{
    using Alloc = std::allocator_traits<Allocator<uint8_t>>;

public:
    static StringDynamicBuffer* CreateBuffer(size_t capacity)
    {
        Allocator<uint8_t> alloc;

        size_t alloc_size = sizeof(StringDynamicBuffer) + sizeof(CharType) * capacity;
        StringDynamicBuffer *buffer = (StringDynamicBuffer*)Alloc::allocate(alloc, alloc_size);
        memset(buffer, 0, alloc_size);

        new (buffer) StringDynamicBuffer(capacity, alloc_size, std::move(alloc));
        return buffer;
    }

    // Clone & unref
    StringDynamicBuffer* Detach(size_t new_capacity = 0)
    {
        if (new_capacity == 0)
            new_capacity = capacity_;

        assert(new_capacity >= capacity_);

        StringDynamicBuffer* detached = CreateBuffer(new_capacity);
        memcpy(detached->data_, data_, sizeof(CharType) * capacity_);

        Unref();
        return detached;
    }

    void Ref()
    {
        ref_count_++;
    }

    void Unref()
    {
        assert(ref_count_ > 0);

        ref_count_--;
        if (ref_count_ == 0)
        {
            Allocator<uint8_t> alloc = std::move(alloc_);
            Alloc::deallocate(alloc, (uint8_t*)this, self_size_);
        }
    }

    bool Shared()
    {
        return ref_count_ > 1;
    }

    CharType* Data()
    {
        return data_;
    }

    size_t Capacity()
    {
        return capacity_;
    }

private:
    StringDynamicBuffer() = delete;

    StringDynamicBuffer(size_t capacity, size_t self_size, Allocator<uint8_t> alloc) :
        ref_count_(1), capacity_(capacity), self_size_(self_size), alloc_(std::move(alloc)) {}

private:
    size_t ref_count_;
    size_t capacity_;
    size_t self_size_;
    Allocator<uint8_t> alloc_;
    CharType data_[];
};

#endif
