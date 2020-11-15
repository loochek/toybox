#include <stdio.h>
#include <string.h>

#define LERR_MAX_MSG_LEN   100
#define LERR_MAX_FUNC_NAME 100

typedef enum
{
    LERR_NO_ERROR = 0,
    LERR_FILE_IO,
    LERR_ALLOC,
    LERR_AKINATOR_PARSE,
    LERR_AKINATOR_BRANCH,
    LERR_AKINATOR_END
} lerror_t;

extern lerror_t __lerrno;
extern char     __lerr_str[];
extern char     __lerr_func_name[];

#define LERR_RESET() __lerrno = LERR_NO_ERROR

#define LERR_PRESENT() (__lerrno != LERR_NO_ERROR)

#define LERRNO(err) { __lerrno = (err); strncpy(__lerr_func_name,  __func__, LERR_MAX_FUNC_NAME); }

#define LERRSTR(err_str, ...)                                    \
{                                                                \
    char tmp_buf[LERR_MAX_MSG_LEN + 1];                          \
    snprintf(tmp_buf, LERR_MAX_MSG_LEN, err_str, ##__VA_ARGS__); \
    strncpy(__lerr_str, tmp_buf, LERR_MAX_MSG_LEN);              \
}

#define LERR(err, err_string, ...) { LERRNO(err); LERRSTR(err_string, ##__VA_ARGS__); }

#define LERR_RETURN(err, ret_val, err_string, ...) \
{                                                  \
    LERR(err, err_string, ##__VA_ARGS__);          \
    return ret_val;                                \
}

#define LERRPRINT() fprintf(stderr, "An error occured in function %s: %s\n", \
                                        __lerr_func_name, __lerr_str)