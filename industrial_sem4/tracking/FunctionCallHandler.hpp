#ifndef FUNCTION_CALL_HANDLER_HPP
#define FUNCTION_CALL_HANDLER_HPP

#include <string>
#include <cstdio>

const int SCOPE_PAD = 4;

#define FUNC_ENTRY FunctionCallHandler funcEntry(__PRETTY_FUNCTION__)
#define PADDED_PRINT(fmt) printf("%*s" fmt, SCOPE_PAD * FunctionCallHandler::getCurrentDepth(), "")
#define PADDED_PRINTF(fmt, ...) printf("%*s" fmt, SCOPE_PAD * FunctionCallHandler::getCurrentDepth(), "", __VA_ARGS__)

class FunctionCallHandler
{
public:
    FunctionCallHandler(const std::string &function)
    {
        PADDED_PRINTF("%s\n", function.c_str());
        PADDED_PRINT("{\n");
        sDepthCounter++;
    }

    ~FunctionCallHandler()
    {
        sDepthCounter--;
        PADDED_PRINT("}\n");
    }

    static int getCurrentDepth() { return sDepthCounter; }

protected:
    static int sDepthCounter;
};

#endif