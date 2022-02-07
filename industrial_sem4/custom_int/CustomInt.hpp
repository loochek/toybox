#ifndef CUSTOM_INT_HPP
#define CUSTOM_INT_HPP

#include <cstdio>

#define CUSTOMINT_UNARY_OPERATOR(OPER, DISP) \
    const CustomInt operator OPER() const \
    { \
        printf("[CustomInt] %p: " DISP "%d = %d\n", \
               this, mValue, OPER mValue); \
        return CustomInt(OPER mValue); \
    }

#define CUSTOMINT_BINARY_OPERATOR(OPER, DISP) \
    const CustomInt operator OPER(const CustomInt &other) const \
    { \
        printf("[CustomInt] %p and %p: %d " DISP " %d = %d\n", \
               this, &other, mValue, other.mValue, mValue OPER other.mValue); \
        return CustomInt(mValue OPER other.mValue); \
    }

#define CUSTOMINT_BINARY_ASSN_OPERATOR(OPER, DISP) \
    CustomInt &operator OPER##=(const CustomInt &other) \
    { \
        printf("[CustomInt] %p and %p: %d " DISP " %d = %d\n", \
               this, &other, mValue, other.mValue, mValue OPER other.mValue); \
        mValue OPER##= other.mValue; \
        return *this; \
    }

#define CUSTOMINT_COMP_OPERATOR(OPER) \
    bool operator OPER(const CustomInt &other) const\
    { \
        printf("[CustomInt] %p and %p: %d " #OPER " %d = %s\n", \
               this, &other, mValue, other.mValue, mValue OPER other.mValue ? "true" : "false"); \
        return mValue OPER other.mValue; \
    }

class CustomInt
{
public:
    CustomInt(int value = 0)
    {
        mValue = value;
        printf("[CustomInt] %p was initialized with %d\n", this, mValue);
    }

    CustomInt(const CustomInt &other)
    {
        mValue = other.mValue;
        printf("[CustomInt] %p was initialized with %d as copy of %p\n", this, mValue, &other);
    }

    CustomInt &operator=(const CustomInt &other)
    {
        mValue = other.mValue;
        printf("[CustomInt] %p was updated with %d during assignment from %p\n", this, mValue, &other);
        return *this;
    }

    CustomInt &operator=(const int other)
    {
        mValue = other;
        printf("[CustomInt] %p was updated with %d during assignment\n", this, mValue);
        return *this;
    }

    CUSTOMINT_BINARY_OPERATOR(+, "+")
    CUSTOMINT_BINARY_OPERATOR(-, "-")
    CUSTOMINT_BINARY_OPERATOR(*, "*")
    CUSTOMINT_BINARY_OPERATOR(/, "/")
    CUSTOMINT_BINARY_OPERATOR(%, "%%")
    CUSTOMINT_BINARY_OPERATOR(&, "&")
    CUSTOMINT_BINARY_OPERATOR(|, "|")
    CUSTOMINT_BINARY_OPERATOR(^, "^")

    CUSTOMINT_UNARY_OPERATOR(~, "~")

    CUSTOMINT_BINARY_ASSN_OPERATOR(+, "+")
    CUSTOMINT_BINARY_ASSN_OPERATOR(-, "-")
    CUSTOMINT_BINARY_ASSN_OPERATOR(*, "*")
    CUSTOMINT_BINARY_ASSN_OPERATOR(/, "/")
    CUSTOMINT_BINARY_ASSN_OPERATOR(%, "%%")
    CUSTOMINT_BINARY_ASSN_OPERATOR(&, "&")
    CUSTOMINT_BINARY_ASSN_OPERATOR(|, "|")
    CUSTOMINT_BINARY_ASSN_OPERATOR(^, "^")

    CUSTOMINT_COMP_OPERATOR(<)
    CUSTOMINT_COMP_OPERATOR(>)
    CUSTOMINT_COMP_OPERATOR(==)
    CUSTOMINT_COMP_OPERATOR(<=)
    CUSTOMINT_COMP_OPERATOR(>=)
    CUSTOMINT_COMP_OPERATOR(!=)

protected:
    int mValue;
};

#endif