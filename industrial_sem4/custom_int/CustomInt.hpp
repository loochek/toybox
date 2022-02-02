#ifndef CUSTOM_INT_HPP
#define CUSTOM_INT_HPP

#include <cstdio>

class CustomInt
{
public:
    CustomInt(int value = 0)
    {
        mValue = value;
        printf("CustomInt at %p was initialized with %d\n", this, mValue);
    }

    CustomInt(const CustomInt &other)
    {
        mValue = other.mValue;
        printf("CustomInt at %p was initialized with %d as copy of %p\n", this, mValue, &other);
    }

    CustomInt &operator=(const CustomInt &other)
    {
        mValue = other.mValue;
        printf("CustomInt at %p was updated with %d during assignment from %p\n", this, mValue, &other);
        return *this;
    }

    CustomInt &operator=(const int other)
    {
        mValue = other;
        printf("CustomInt at %p was updated with %d during assignment\n", this, mValue);
        return *this;
    }

    const CustomInt operator+(const CustomInt &other) const
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were added with result %d\n",
               this, &other, mValue, other.mValue, mValue + other.mValue);
        return CustomInt(mValue + other.mValue);
    }

    const CustomInt operator-(const CustomInt &other) const
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were subtracted with result %d\n",
               this, &other, mValue, other.mValue, mValue - other.mValue);
        return CustomInt(mValue - other.mValue);
    }

    const CustomInt operator*(const CustomInt &other) const
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were multiplied with result %d\n",
               this, &other, mValue, other.mValue, mValue * other.mValue);
        return CustomInt(mValue * other.mValue);
    }

    const CustomInt operator/(const CustomInt &other) const
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were divided with result %d\n",
               this, &other, mValue, other.mValue, mValue / other.mValue);
        return CustomInt(mValue / other.mValue);
    }

    const CustomInt operator%(const CustomInt &other) const
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were moduled with result %d\n",
               this, &other, mValue, other.mValue, mValue % other.mValue);
        return CustomInt(mValue % other.mValue);
    }

    const CustomInt operator&(const CustomInt &other) const
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were bitwise ANDed with result %d\n",
               this, &other, mValue, other.mValue, mValue & other.mValue);
        return CustomInt(mValue & other.mValue);
    }

    const CustomInt operator|(const CustomInt &other) const
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were bitwise ORed with result %d\n",
               this, &other, mValue, other.mValue, mValue | other.mValue);
        return CustomInt(mValue | other.mValue);
    }

    const CustomInt operator^(const CustomInt &other) const
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were bitwise XORed with result %d\n",
               this, &other, mValue, other.mValue, mValue ^ other.mValue);
        return CustomInt(mValue ^ other.mValue);
    }

    const CustomInt operator~() const
    {
        printf("CustomInt at %p with value %d was bitwise NOTed with result %d\n", this, mValue, ~mValue);
        return CustomInt(~mValue);
    }

    CustomInt &operator+=(const CustomInt &other)
    {
        printf("CustomInt at %p with value %d was added with CustomInt at %p with value %d with result %d\n",
               this, mValue, &other, other.mValue, mValue + other.mValue);
        mValue += other.mValue;
        return *this;
    }

    CustomInt &operator-=(const CustomInt &other)
    {
        printf("CustomInt at %p with value %d was subtracted with CustomInt at %p with value %d with result %d\n",
               this, mValue, &other, other.mValue, mValue - other.mValue);
        mValue -= other.mValue;
        return *this;
    }

    CustomInt &operator*=(const CustomInt &other)
    {
        printf("CustomInt at %p with value %d was multiplied with CustomInt at %p with value %d with result %d\n",
               this, mValue, &other, other.mValue, mValue * other.mValue);
        mValue *= other.mValue;
        return *this;
    }

    CustomInt &operator/=(const CustomInt &other)
    {
        printf("CustomInt at %p with value %d was divided with CustomInt at %p with value %d with result %d\n",
               this, mValue, &other, other.mValue, mValue / other.mValue);
        mValue /= other.mValue;
        return *this;
    }

    CustomInt &operator%=(const CustomInt &other)
    {
        printf("CustomInt at %p with value %d was moduled with CustomInt at %p with value %d with result %d\n",
               this, mValue, &other, other.mValue, mValue % other.mValue);
        mValue %= other.mValue;
        return *this;
    }

    CustomInt &operator&=(const CustomInt &other)
    {
        printf("CustomInt at %p with value %d was bitwise ANDed with CustomInt at %p with value %d with result %d\n",
               this, mValue, &other, other.mValue, mValue & other.mValue);
        mValue &= other.mValue;
        return *this;
    }

    CustomInt &operator|=(const CustomInt &other)
    {
        printf("CustomInt at %p with value %d was bitwise ORed with CustomInt at %p with value %d with result %d\n",
               this, mValue, &other, other.mValue, mValue | other.mValue);
        mValue |= other.mValue;
        return *this;
    }

    CustomInt &operator^=(const CustomInt &other)
    {
        printf("CustomInt at %p with value %d was bitwise XORed with CustomInt at %p with value %d with result %d\n",
               this, mValue, &other, other.mValue, mValue ^ other.mValue);
        mValue ^= other.mValue;
        return *this;
    }

    bool operator<(const CustomInt &other)
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were less compared with result %s\n",
               this, &other, mValue, other.mValue, mValue < other.mValue ? "\"true\"" : "\"false\"");
        return mValue < other.mValue;
    }

    bool operator>(const CustomInt &other)
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were more compared with result %s\n",
               this, &other, mValue, other.mValue, mValue > other.mValue ? "\"true\"" : "\"false\"");
        return mValue > other.mValue;
    }

    bool operator==(const CustomInt &other)
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were compared to be equal with result %s\n",
               this, &other, mValue, other.mValue, mValue == other.mValue ? "\"true\"" : "\"false\"");
        return mValue == other.mValue;
    }

    bool operator<=(const CustomInt &other)
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were less-or-equal compared with result %s\n",
               this, &other, mValue, other.mValue, mValue <= other.mValue ? "\"true\"" : "\"false\"");
        return mValue <= other.mValue;
    }

    bool operator>=(const CustomInt &other)
    {
        printf("CustomInts at %p and %p with values %d and %d correspondingly were more-or-equal compared with result %s\n",
               this, &other, mValue, other.mValue, mValue >= other.mValue ? "\"true\"" : "\"false\"");
        return mValue >= other.mValue;
    }

protected:
    int mValue;
};

#endif