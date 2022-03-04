//#define TRACKING_DISABLE_MOVE
#define LOG_DISABLE_DTORS

#include <cassert>
#include <cstring>
#include "TrackedInt.hpp"
#include "HtmlLogger.hpp"
#include "ConsoleLogger.hpp"
#include "DotLogger.hpp"

template<typename T>
struct remove_reference
{
    typedef T type;
};

template<typename T>
struct remove_reference<T&>
{
    typedef T type;
};

template<typename T>
struct remove_reference<T&&>
{
    typedef T type;
};

template<typename T>
typename remove_reference<T>::type &&move(T &&arg)
{
    return static_cast<typename remove_reference<T>::type&&>(arg);
}

template<typename T>
T&& forward(T &arg)
{
    return static_cast<T&&>(arg);
}

template<typename T, typename Arg>
T *createDynamic(Arg &&arg)
{
    return new T(std::forward<Arg>(arg));
}

TrackedInt square(TrackedInt arg)
{
    FUNC_ENTRY;
    return arg * arg;
}

// void testEntry()
// {
//     INT(a, 10);
//     INT(result, square(std::move(a)));
//     // a is not used anymore
// }

// void testEntry()
// {
//     FUNC_ENTRY;

//     INT(lval, 10);

//     TrackedInt *lvalue_copy = createDynamic<TrackedInt>(lval);
//     TrackedInt *rvalue_copy = createDynamic<TrackedInt>(TrackedInt(10));
// }

int main(int argc, char *argv[])
{       
    if (argc < 2 || !strcmp(argv[1], "console"))
    {
        BaseLogger::sLoggerInUse = &ConsoleLogger::getInstance();
    }
    else if (!strcmp(argv[1], "html"))
    {
        BaseLogger::sLoggerInUse = &HtmlLogger::getInstance();
    }
    else if (!strcmp(argv[1], "dot"))
    {
        BaseLogger::sLoggerInUse = &DotLogger::getInstance();
    }

    BaseLogger::sLoggerInUse->start();
    testEntry();
    BaseLogger::sLoggerInUse->finish();

    return 0;
}
