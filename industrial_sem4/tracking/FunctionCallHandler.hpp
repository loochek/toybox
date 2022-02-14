#ifndef FUNCTION_CALL_HANDLER_HPP
#define FUNCTION_CALL_HANDLER_HPP

#include <string>
#include <cstdio>
#include "BaseLogger.hpp"

#define FUNC_ENTRY FunctionCallHandler funcEntry(__PRETTY_FUNCTION__)

class FunctionCallHandler
{
public:
    FunctionCallHandler(const std::string &function)
    {
        BaseLogger::sLoggerInUse->functionEnter(function);
    }

    ~FunctionCallHandler()
    {
        BaseLogger::sLoggerInUse->functionLeave();
    }
};

#endif