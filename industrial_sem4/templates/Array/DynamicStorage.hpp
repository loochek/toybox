#ifndef DYNAMIC_STORAGE_HPP
#define DYNAMIC_STORAGE_HPP

#include <cstddef>
#include <cassert>
#include <stdexcept>
#include <new>

template<typename T, size_t SIZE = 0>
class DynamicStorage
{
    static constexpr size_t MINIMAL_CAPACITY = 16;
    
public:
    DynamicStorage() noexcept :
        data_(nullptr), size_(0), capacity_(0)
    {
    }

    DynamicStorage(size_t size) :
        data_(nullptr), size_(0), capacity_(0)
    {
        Reserve(size);
        ssize_t i = 0;
        
        try
        {
            for (; i < size; i++)
                new (&data_[i]) T();
        }
        catch (...)
        {
            for (i--; i >= 0; i--)
                data_[i].~T();

            this->~DynamicStorage();
            throw;
        }

        size_ = size;
    }

    DynamicStorage(const DynamicStorage &other) :
        data_(nullptr), size_(0), capacity_(0)
    {
        Reserve(other.size_);
        ssize_t i = 0;

        try
        {
            for (; i < other.size_; i++)
                new (&data_[i]) T(other.data_[i]);
        }
        catch (...)
        {
            for (i--; i >= 0; i--)
                data_[i].~T();

            this->~DynamicStorage();
            throw;
        }

        size_ = other.size_;
    }

    DynamicStorage(DynamicStorage &&other) noexcept :
        data_(other.data_), capacity_(other.capacity_), size_(other.size_)
    {
        other.data_ = nullptr;
        other.capacity_ = 0;
        other.size_ = 0;
    }

    ~DynamicStorage()
    {
        for (size_t i = 0; i < size_; i++)
            data_[i].~T();

        ::operator delete(data_);
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    DynamicStorage &operator=(const DynamicStorage &other)
    {
        Clear();
        Reserve(other.size_);

        ssize_t i = 0;
        try
        {
            for (; i < other.size_; i++)
                new (&data_[i]) T(other.data_[i]);
        }
        catch (...)
        {
            for (i--; i >= 0; i--)
                data_[i].~T();

            this->~DynamicStorage();
            throw;
        }

        size_ = other.size_;
        return *this;
    }

    DynamicStorage &operator=(DynamicStorage &&other) noexcept
    {
        this->~DynamicStorage();
        data_ = other.data_;
        capacity_ = other.capacity_;
        size_ = other.size_;

        other.data_ = nullptr;
        other.capacity_ = 0;
        other.size_ = 0;

        return *this;
    }

    T& Access(size_t index) noexcept
    {
        return const_cast<T&>(static_cast<const DynamicStorage*>(this)->Access(index));
    }

    const T& Access(size_t index) const noexcept
    {
        assert(index < size_);
        return data_[index];
    }

    T *Data() noexcept
    {
        return data_;
    }

    const T *Data() const noexcept
    {
        return data_;
    }

    T& ReserveBack()
    {
        Reserve(size_ + 1);
        return data_[size_++];
    }

    void ReserveRollback() noexcept
    {
        assert(size_ != 0);
        size_--;
    }

    void PopBack()
    {
        assert(size_ != 0);
        data_[--size_].~T();
    }    

    void Clear()
    {
        for (ssize_t i = 0; i < size_; i++)
            data_[i].~T();

        size_ = 0;
    }

    void Resize(size_t new_size)
    {
        if (new_size <= size_)
        {
            for (ssize_t i = new_size; i < size_; i++)
                data_[i].~T();
        }
        else
        {
            Reserve(new_size);
            ssize_t i = size_;
            try
            {
                for (; i < new_size; i++)
                    new (&data_[i]) T();
            }
            catch (...)
            {
                for (i--; i >= size_; i--)
                    data_[i].~T();

                throw;
            }
        }

        size_ = new_size;
    }

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

    const size_t Size() const noexcept
    {
        return size_;
    }

private:
    void SetCapacity(size_t new_capacity)
    {
        assert((new_capacity != 0) && !(new_capacity & (new_capacity - 1)));
        assert(new_capacity >= size_);
        new_capacity = std::max(new_capacity, size_t(MINIMAL_CAPACITY));

        T* new_data = (T*)::operator new(new_capacity * sizeof(T));
        ssize_t i = 0;
        try
        {
            for (; i < size_; i++)
                new (&new_data[i]) T(std::move_if_noexcept(data_[i]));
        }
        catch (...)
        {
            // Not reachable if T's move constructor is noexcept
            for (i--; i >= 0; i--)
                new_data[i].~T();

            ::operator delete(new_data);
            throw;
        }

        for (ssize_t i = 0; i < size_; i++)
            data_[i].~T();
        
        if (data_ != nullptr)
            ::operator delete(data_);

        data_ = new_data;
        capacity_ = new_capacity;
    }

private:
    T *data_;

    size_t size_;
    size_t capacity_;
};

#endif
