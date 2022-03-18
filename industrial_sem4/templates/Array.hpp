#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <cstdio>
#include <cassert>

template<typename T, size_t N>
class StaticStorage
{
public:
    StaticStorage()
    {
        std::cout << "StaticStorage in use\n";
    }

    T& AccessData(size_t index)
    {
        assert(0 <= index && index < N);
        return data_[index];
    }

    const T& AccessData(size_t index) const
    {
        assert(0 <= index && index < N);
        return data_[index];
    }

private:
    T data_[N];
};

template<typename T, size_t N>
class DynamicStorage
{
public:
    DynamicStorage()
    {
        std::cout << "DynamicStorage in use\n";
        data_ = new T[N];
    }

    ~DynamicStorage()
    {
        delete[] data_;
    }

    T& AccessData(size_t index)
    {
        assert(0 <= index && index < N);
        return data_[index];
    }

    const T& AccessData(size_t index) const
    {
        assert(0 <= index && index < N);
        return data_[index];
    }

private:
    T *data_;
};

template
<
    typename T,
    ssize_t N,
    template<typename T_, size_t N_> class Storage = StaticStorage
>
class Array : Storage<T, N>
{
public:
    size_t Size() const
    {
        return N;
    }

    T& operator[](size_t index)
    {
        return this->AccessData(index);
    }

    const T& operator[](size_t index) const
    {
        return this->AccessData(index);
    }
};

template
<
    size_t N,
    template<typename T_, size_t N_> class Storage
>
class Array<bool, N, Storage> : Storage<uint8_t, N / 8 + !!(N % 8)>
{
    class Reference;

public:
    size_t Size() const
    {
        return N;
    }

    Array<bool, N, Storage>::Reference operator[](size_t index)
    {
        assert(0 <= index && index < N);
        size_t bit_number = 7 - index % 8;
        return Array<bool, N, Storage>::Reference(this->AccessData(index / 8), bit_number);
    }

    const Array<bool, N, Storage>::Reference operator[](size_t index) const
    {
        assert(0 <= index && index < N);
        size_t bit_number = 7 - index % 8;
        return Array<bool, N, Storage>::Reference(const_cast<uint8_t&>(this->AccessData(index / 8)), bit_number);
    }

private:
    class Reference
    {
    public:
        operator bool() const
        {
            return (byte_ >> bit_) & 0x1;
        }

        Reference& operator=(bool value)
        {
            byte_ &= ~(1 << bit_);
            byte_ |= (1 << bit_) * value;
            return *this;
        }

        Reference(uint8_t &byte, const uint8_t bit) :
            byte_(byte), bit_(bit)
        {
            assert(0 <= bit && bit < 8);
        }

        uint8_t &byte_;
        const uint8_t bit_;
    };
};

#endif
