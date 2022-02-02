#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>
#include <cstdarg>

enum class LogLevel
{
    Info,
    Warning,
    Error
};

class Logger
{
public:
    static void log(LogLevel logLevel, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        log(logLevel, fmt, args);
        va_end(args);
    }

    static void log(LogLevel logLevel, const char *fmt, va_list args)
    {
        switch (logLevel)
        {
        case LogLevel::Info:
            printf("[INFO] ");
            break;

        case LogLevel::Warning:
            printf("[WARN] ");
            break;

        case LogLevel::Error:
            printf("[ERR] ");
        }

        log_internal(fmt, args);
        printf("\n");
    }
    
private:
    static void log_internal(const char *fmt, va_list args)
    {
        vprintf(fmt, args);
    }
};

#endif