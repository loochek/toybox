#ifndef STRING_HPP
#define STRING_HPP

#include "StringBase.hpp"

template<typename CharType, template<typename T_> class Allocator = std::allocator>
class String : public StringBase<CharType, StringView<CharType>>
{
    friend StringBase<CharType, StringView<CharType>>;
    using alloc = std::allocator_traits<Allocator<T>>;

    static constexpr size_t MINIMAL_CAPACITY = 16;

public:
    void Reserve(size_t desired_capacity)
    {
        if (desired_capacity <= capacity_)
            return;

        size_t new_capacity = 1;
        while (new_capacity < desired_capacity)
            new_capacity <<= 1;

        SetCapacity(new_capacity);   
    }

    void Shrink()
    {
        size_t needed_capacity = 1;
        while (needed_capacity < size_)
            needed_capacity <<= 1;

        if (needed_capacity == capacity_)
            return;

        SetCapacity(needed_capacity);
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
    
    void SetCapacity(size_t new_capacity)
    {
        assert((new_capacity != 0) && !(new_capacity & (new_capacity - 1)));
        assert(new_capacity >= size_);
        new_capacity = std::max(new_capacity, size_t(MINIMAL_CAPACITY));

        CharType* new_data = alloc::allocate(alloc_, new_capacity);
        memcpy(new_data, data_, sizeof(CharType) * size_)

        if (data_ != nullptr)
            alloc::deallocate(alloc_, data_, capacity_);

        data_ = new_data;
        capacity_ = new_capacity;
    }

private:
    CharType* data_;
    Allocator<CharType> alloc_;
};

#endif
