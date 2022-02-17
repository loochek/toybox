#ifndef TRACKED_INT_HPP
#define TRACKED_INT_HPP

#include <cstdio>
#include <string>
#include <sstream>
#include "FunctionCallHandler.hpp"
#include "BaseLogger.hpp"

#define TRACKEDINT_UNARY_OPERATOR(OPER, DISP) \
    TrackedInt operator OPER() const \
    { \
        FUNC_ENTRY; \
        return TrackedInt(OPER mValue, DISP, *this); \
    }

#define TRACKEDINT_BINARY_OPERATOR(OPER, DISP) \
    TrackedInt operator OPER(const TrackedInt &other) const \
    { \
        FUNC_ENTRY; \
        return TrackedInt(mValue OPER other.mValue, DISP, *this, other); \
    }

#define TRACKEDINT_BINARY_ASSN_OPERATOR(OPER, DISP) \
    TrackedInt &operator OPER##=(const TrackedInt &other) \
    { \
        mValue OPER##= other.mValue; \
        BaseLogger::sLoggerInUse->assignment(*this, other, DISP); \
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
        handleTemp(mName);
        BaseLogger::sLoggerInUse->simpleCtor(*this);
    }

    ~TrackedInt()
    {
        BaseLogger::sLoggerInUse->dtor(*this);
    }

    TrackedInt(const TrackedInt &other, const std::string &name = "") :
        mValue(other.mValue),
        mObjIndex(sObjIndexCount++),
        mName(name)
    {
        handleTemp(mName);
        BaseLogger::sLoggerInUse->copyCtor(*this, other);
        sCopyCount++;
    }

    TrackedInt &operator=(const TrackedInt &other)
    {
        mValue = other.mValue;
        BaseLogger::sLoggerInUse->copyAssignment(*this, other);
        sCopyCount++;
        return *this;
    }

#ifndef TRACKING_DISABLE_MOVE
    TrackedInt(TrackedInt &&other, const std::string &name = "") :
        mValue(other.mValue),
        mObjIndex(sObjIndexCount++),
        mName(name)
    {
        handleTemp(mName);
        BaseLogger::sLoggerInUse->moveCtor(*this, other);
    }
    
    TrackedInt &operator=(TrackedInt &&other)
    {
        mValue = other.mValue;
        BaseLogger::sLoggerInUse->moveAssignment(*this, other);
        return *this;
    }
#endif

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
        handleTemp(mName);
        BaseLogger::sLoggerInUse->parentPresentCtor(*this, parent, oper);
    }

    TrackedInt(int value, const std::string &oper, const TrackedInt &parent1, const TrackedInt &parent2) :
        mValue(value),
        mObjIndex(sObjIndexCount++)
    {
        handleTemp(mName);
        BaseLogger::sLoggerInUse->parentsPresentCtor(*this, parent1, parent2, oper);
    }

    void handleTemp(std::string &name)
    {
        if (name != "")
            return;

        sTmpObjectsCount++;
        std::ostringstream stream;
        stream << "temp object " << mObjIndex;
        name = stream.str();
    }

protected:
    int mValue;
    int mObjIndex;

    std::string mName;
    static int sObjIndexCount;

    static int sTmpObjectsCount;
    static int sCopyCount;

    friend class ConsoleLogger;
    friend class HtmlLogger;
    friend class DotLogger;
};

#define INT(name, value) TrackedInt name(value, #name)

#endif