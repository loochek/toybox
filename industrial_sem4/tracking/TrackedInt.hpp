#ifndef TRACKED_INT_HPP
#define TRACKED_INT_HPP

#include <cstdio>
#include <string>
#include <sstream>
#include "FunctionCallHandler.hpp"

#define TRACKEDINT_UNARY_OPERATOR(OPER, DISP) \
    const TrackedInt operator OPER() const \
    { \
        FUNC_ENTRY; \
        return TrackedInt(OPER mValue, DISP, *this); \
    }

#define TRACKEDINT_BINARY_OPERATOR(OPER, DISP) \
    const TrackedInt operator OPER(const TrackedInt &other) const \
    { \
        FUNC_ENTRY; \
        return TrackedInt(mValue OPER other.mValue, DISP, *this, other); \
    }

#define TRACKEDINT_BINARY_ASSN_OPERATOR(OPER, DISP) \
    TrackedInt &operator OPER##=(const TrackedInt &other) \
    { \
        mValue OPER##= other.mValue; \
        PADDED_PRINTF("[Assign] \"%s\" (%p) <--(%d)-- " DISP "= \"%s\" (%p)\n", \
                      mName.c_str(), this, mValue, other.mName.c_str(), &other); \
        return *this; \
    }

#define TRACKEDINT_COMP_OPERATOR(OPER) \
    bool operator OPER(const TrackedInt &other) const\
    { \
        return mValue OPER other.mValue; \
    }

class TrackedInt
{
public:
    TrackedInt(int value = 0, const std::string &name = "") :
        mValue(value),
        mObjIndex(sObjIndexCount++),
        mName(name)
    {
        genTempName(mName);
        PADDED_PRINTF("[New] \"%s\" (%p) <---- %d\n", mName.c_str(), this, mValue);
    }

    TrackedInt(const TrackedInt &other, const std::string &name = "") :
        mValue(other.mValue),
        mObjIndex(sObjIndexCount++),
        mName(name)
    {        
        genTempName(mName);
        PADDED_PRINTF("[\033[0;31mCOPY\033[0m] \"%s\" (%p) <--(%d)-- \"%s\" (%p)\n",
                      mName.c_str(), this, mValue, other.mName.c_str(), &other);
    }

    TrackedInt &operator=(const TrackedInt &other)
    {
        mValue = other.mValue;
        PADDED_PRINTF("[Assign] \"%s\" (%p) <--(%d)-- \"%s\" (%p)\n",
                      mName.c_str(), this, mValue, other.mName.c_str(), &other);
        return *this;
    }

    TrackedInt &operator=(const int val)
    {
        mValue = val;
        PADDED_PRINTF("[Assign] \"%s\" (%p) <---- %d", mName.c_str(), this, mValue);
        return *this;
    }

    TRACKEDINT_BINARY_OPERATOR(+, "+")
    TRACKEDINT_BINARY_OPERATOR(-, "-")
    TRACKEDINT_BINARY_OPERATOR(*, "*")
    TRACKEDINT_BINARY_OPERATOR(/, "/")
    TRACKEDINT_BINARY_OPERATOR(%, "%%")
    TRACKEDINT_BINARY_OPERATOR(&, "&")
    TRACKEDINT_BINARY_OPERATOR(|, "|")
    TRACKEDINT_BINARY_OPERATOR(^, "^")

    TRACKEDINT_UNARY_OPERATOR(+, "+")
    TRACKEDINT_UNARY_OPERATOR(-, "-")
    TRACKEDINT_UNARY_OPERATOR(~, "~")

    TRACKEDINT_BINARY_ASSN_OPERATOR(+, "+")
    TRACKEDINT_BINARY_ASSN_OPERATOR(-, "-")
    TRACKEDINT_BINARY_ASSN_OPERATOR(*, "*")
    TRACKEDINT_BINARY_ASSN_OPERATOR(/, "/")
    TRACKEDINT_BINARY_ASSN_OPERATOR(%, "%%")
    TRACKEDINT_BINARY_ASSN_OPERATOR(&, "&")
    TRACKEDINT_BINARY_ASSN_OPERATOR(|, "|")
    TRACKEDINT_BINARY_ASSN_OPERATOR(^, "^")

    TRACKEDINT_COMP_OPERATOR(<)
    TRACKEDINT_COMP_OPERATOR(>)
    TRACKEDINT_COMP_OPERATOR(==)
    TRACKEDINT_COMP_OPERATOR(<=)
    TRACKEDINT_COMP_OPERATOR(>=)
    TRACKEDINT_COMP_OPERATOR(!=)

protected:
    TrackedInt(int value, const std::string &oper, const TrackedInt &parent) :
        mValue(value),
        mObjIndex(sObjIndexCount++)
    {
        genTempName(mName);
        PADDED_PRINTF("[New] \"%s\" (%p) <--(%d)-- %s\"%s\" (%p)\n",
                      mName.c_str(), this, mValue, oper.c_str(), parent.mName.c_str(), &parent);
    }

    TrackedInt(int value, const std::string &oper, const TrackedInt &leftParent, const TrackedInt &rightParent) :
        mValue(value),
        mObjIndex(sObjIndexCount++)
    {
        genTempName(mName);
        PADDED_PRINTF("[New] \"%s\" (%p) <--(%d)-- \"%s\" (%p) %s \"%s\" (%p)\n",
                      mName.c_str(), this, mValue, leftParent.mName.c_str(), &leftParent, oper.c_str(),
                      rightParent.mName.c_str(), &rightParent);
    }

    void genTempName(std::string &name)
    {
        if (name != "")
            return;

        std::ostringstream stream;
        stream << "temp object " << mObjIndex;
        name = stream.str();
    }

protected:
    int mValue;
    int mObjIndex;

    std::string mName;
    static int sObjIndexCount;
};

#define INT(name, value) TrackedInt name(value, #name)
#define INTCOPY(dst, src) TrackedInt dst(src, #dst)

#endif