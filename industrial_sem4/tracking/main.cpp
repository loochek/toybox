#include <cassert>
#include "TrackedInt.hpp"
#include "HtmlLogger.hpp"

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

void recursive(TrackedInt depth)
{
    FUNC_ENTRY;

    if (depth <= 0)
        return;

    INT(a, 1);
    INT(b, 2);
    INT(c, 3);
    INT(sum, a + b + c);
    INTCOPY(aCopy, a);
    INTCOPY(bCopy, b);
    INTCOPY(cCopy, c);

    aCopy = a;
    bCopy = 5;
    cCopy -= 3;

    recursive(depth - 1);
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

    recursive(2);
}

int main()
{
    HtmlLogger &logger = HtmlLogger::getInstance();
    logger.start("log.html");
    
    BaseLogger::sLoggerInUse = &HtmlLogger::getInstance();
    
    testEntry();

    logger.finish();
    return 0;
}