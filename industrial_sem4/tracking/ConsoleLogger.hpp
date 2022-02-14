#ifndef CONSOLE_LOGGER_HPP
#define CONSOLE_LOGGER_HPP

#include "BaseLogger.hpp"

#define CL_PADDED_PRINT(fmt) printf("%*s" fmt, mCurrDepth * NEST_PAD, "")
#define CL_PADDED_PRINTF(fmt, ...) printf("%*s" fmt, mCurrDepth * NEST_PAD, "", __VA_ARGS__)

class ConsoleLogger : public BaseLogger
{
public:
    virtual ~ConsoleLogger() {}

    virtual void functionEnter(const std::string &prettyName) override
    {
        CL_PADDED_PRINTF("%s\n", prettyName.c_str());
        CL_PADDED_PRINT("{\n");
        mCurrDepth++;
    }

    virtual void functionLeave() override
    {
        mCurrDepth--;
        CL_PADDED_PRINT("}\n");
    }

    virtual void simpleCtor(const TrackedInt &obj) override
    {
        CL_PADDED_PRINTF("[New] \"%s\" (%d|%p) <-- %d\n", obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue);
    }

    virtual void parentPresentCtor(const TrackedInt &obj, const TrackedInt &parent,
                                   const std::string &oper) override
    {
        CL_PADDED_PRINTF("[New] \"%s\" (%d|%p) <--(%d)-- %s\"%s\" (%d|%p)\n", 
            obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue, oper.c_str(),
            parent.mName.c_str(), parent.mObjIndex, &parent);
    }

    virtual void parentsPresentCtor(const TrackedInt &obj, const TrackedInt &parent1, const TrackedInt &parent2,
                                    const std::string &oper) override
    {
        CL_PADDED_PRINTF("[New] \"%s\" (%d|%p) <--(%d)-- \"%s\" (%d|%p) %s \"%s\" (%d|%p)\n",
            obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue,
            parent1.mName.c_str(), parent1.mObjIndex, &parent1, oper.c_str(),
            parent2.mName.c_str(), parent2.mObjIndex, &parent2);
    }

    virtual void copyCtor(const TrackedInt &obj, const TrackedInt &parent) override
    {
        CL_PADDED_PRINTF("[\033[0;31mCOPY\033[0m] \"%s\" (%d|%p) <--(%d)-- \"%s\" (%d|%p)\n",
            obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue,
            parent.mName.c_str(), parent.mObjIndex, &parent);
    }

    virtual void assignment(const TrackedInt &obj, const TrackedInt &parent, const std::string &oper) override
    {
        CL_PADDED_PRINTF("[Assign] \"%s\" (%d|%p) <--(%d)-- %s= \"%s\" (%d|%p)\n",
            obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue, oper.c_str(),
            parent.mName.c_str(), parent.mObjIndex, &parent);
    }

    static ConsoleLogger &getInstance()
    {
        static ConsoleLogger instance;
        return instance;
    }

protected:
    ConsoleLogger() : mCurrDepth(0) {}

    ConsoleLogger(const ConsoleLogger&) = delete;
    ConsoleLogger& operator=(const ConsoleLogger&) = delete;

protected:
    static const int NEST_PAD = 4;

    int mCurrDepth;
};

#endif