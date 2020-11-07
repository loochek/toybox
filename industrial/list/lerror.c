#include "lerror.h"

/** \file */

lerror_t __lerrno = LERR_NO_ERROR;
char __lerr_str[MAX_LERR_MSG_LEN + 1];
char __lerr_func_name[MAX_LERR_MSG_LEN + 1];
