#include <cassert>
#include "TrackedInt.hpp"
#include "HtmlLogger.hpp"
#include "ConsoleLogger.hpp"
#include "DotLogger.hpp"

void binSearch()
{
    FUNC_ENTRY;
    
    INT(l, 1);
    INT(r, 101);
    INT(c, 27);

    while (r - l > TrackedInt(1))
    {
        TrackedInt m = (l + r) / TrackedInt(2);

        if (m <= c)
            l = m;
        else
            r = m;
    }
}

TrackedInt fib(TrackedInt num)
{
    FUNC_ENTRY;

    if (num <= 2)
        return 1;

    return fib(num - 2) + fib(num - 1);
}

TrackedInt test(TrackedInt a, TrackedInt b, TrackedInt c)
{
    FUNC_ENTRY;
    return a + b + c;
}

TrackedInt testRef(TrackedInt &a, TrackedInt &b, TrackedInt &c)
{
    FUNC_ENTRY;
    return a * (b - c);
}

void testEntry()
{
    FUNC_ENTRY;

    //binSearch();

    // INT(res1, test(a, b, c));
    // INT(res2, testRef(a, b, c));

    INT(result, 0);
    result = fib(4);

    INT(a, result);
}

int main()
{
    DotLogger &logger = DotLogger::getInstance();
    logger.start();
    
    BaseLogger::sLoggerInUse = &DotLogger::getInstance();
    
    testEntry();

    logger.finish();
    return 0;
}