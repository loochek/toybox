#ifndef FORMATTED_ERROR_HPP
#define FORMATTED_ERROR_HPP

#include <stdexcept>
#include <cstdarg>

const int MAX_ERR_MSG_LEN = 256;

class FormattedError : public std::exception
{
public:
    explicit FormattedError(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vsnprintf(mErrorMessage, MAX_ERR_MSG_LEN, fmt, args);
        va_end(args);
    }

    virtual const char* what() const noexcept
    {
       return mErrorMessage;
    }

protected:
    char mErrorMessage[MAX_ERR_MSG_LEN + 1];
};

#endif