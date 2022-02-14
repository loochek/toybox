#ifndef BASE_LOGGER_HPP
#define BASE_LOGGER_HPP

#include <string>

class TrackedInt;

class BaseLogger
{
public:
    virtual ~BaseLogger() {}

    virtual void functionEnter(const std::string &prettyName) = 0;
    virtual void functionLeave() = 0;

    virtual void simpleCtor(const TrackedInt &obj) = 0;
    virtual void parentPresentCtor(const TrackedInt &obj, const TrackedInt &parent,
                                   const std::string &oper) = 0;
    virtual void parentsPresentCtor(const TrackedInt &obj, const TrackedInt &parent1, const TrackedInt &parent2,
                                    const std::string &oper) = 0;

    virtual void copyCtor(const TrackedInt &obj, const TrackedInt &parent) = 0;

    virtual void simpleAssignment(const TrackedInt &obj, const TrackedInt &parent) = 0;
    virtual void assignment(const TrackedInt &obj, const TrackedInt &parent, const std::string &oper) = 0;

public:
    static BaseLogger *sLoggerInUse;
};

#endif