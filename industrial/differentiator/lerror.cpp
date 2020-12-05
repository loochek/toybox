#include "lerror.hpp"

lerror_t __lerrno  = LERR_NO_ERROR;

char __lerr_str      [LERR_MAX_MSG_LEN   + 1];
char __lerr_func_name[LERR_MAX_FUNC_NAME + 1];
