#include <cassert>
#include "TrackedInt.hpp"
#include "HtmlLogger.hpp"
#include "ConsoleLogger.hpp"
#include "DotLogger.hpp"

TrackedInt test(TrackedInt a, TrackedInt b, TrackedInt c)
{
    FUNC_ENTRY;
    return a + b + c;
}

TrackedInt testRef(TrackedInt &a, TrackedInt &b, TrackedInt &c)
{
    FUNC_ENTRY;
    return a + b + c;
}

TrackedInt test2(TrackedInt a, TrackedInt b, TrackedInt c)
{
    FUNC_ENTRY;

    INT(sum, 0);
    sum += a;
    sum += b;
    sum += c;
    
    return sum;
}

TrackedInt testRef2(TrackedInt &a, TrackedInt &b, TrackedInt &c)
{
    FUNC_ENTRY;
    
    INT(sum, 0);
    sum += a;
    sum += b;
    sum += c;
    
    return sum;
}

void testEntry()
{
    FUNC_ENTRY;

    INT(a, 10);

    INT(b, 20);
    INT(c, 30);

    INT(sum1, test(a, b, c));
    INT(sum2, testRef(a, b, c));

    a = 1;
    b = 2;
    c = 3;

    INT(sum3, test2(a, b, c));
    INT(sum4, testRef2(a, b, c));
}

int main()
{
    DotLogger &dotLogger = DotLogger::getInstance();
    HtmlLogger &htmlLogger = HtmlLogger::getInstance();
    dotLogger.start();
    htmlLogger.start("log.html");
    
    BaseLogger::sLoggerInUse = &DotLogger::getInstance();
    // BaseLogger::sLoggerInUse = &HtmlLogger::getInstance();
    // BaseLogger::sLoggerInUse = &ConsoleLogger::getInstance();
    
    testEntry();

    dotLogger.finish();
    htmlLogger.finish();
    return 0;
}