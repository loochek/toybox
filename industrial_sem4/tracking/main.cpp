#include <cassert>
#include <cstring>
#define TRACKING_DISABLE_MOVE
#include "TrackedInt.hpp"
#include "HtmlLogger.hpp"
#include "ConsoleLogger.hpp"
#include "DotLogger.hpp"

TrackedInt test(TrackedInt a, TrackedInt b, TrackedInt c)
{
    FUNC_ENTRY;
    return a + b + c;
}

void testEntry()
{
    FUNC_ENTRY;

    INT(res1, test(TrackedInt(10), TrackedInt(20), TrackedInt(30)));

    INT(a, 0);
    INT(b, 0);
    INT(c, 0);

    a = 1;
    b = 2;
    c = 3;

    INT(res2, test(a, b, c));

    INT(res3, 0);
    res3 = (a - b) * c;
}

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