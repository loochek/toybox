#include "lstatus.hpp"

char __lstatus_msg[LSTATUS_MAX_MSG_LEN + 1] = {0};

const char *__lstatus_str_repr[] =
{
    "LSTATUS_OK",
    "LSTATUS_BAD_ALLOC",
    "LSTATUS_FILE_IO_ERR",
    "LSTATUS_LIST_EMPTY",
    "LSTATUS_LIST_FULL",
    "LSTATUS_LIST_INVALID",
    "LSTATUS_LIST_NULL_ITER",
    "LSTATUS_LIST_BAD_ADDRESS",
    "LSTATUS_DICT_BAD_KEY",
    "LSTATUS_DICT_INVALID"
};