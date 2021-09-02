#include "lstatus.h"

char lstatus_str[MAX_STATUS_STR_LENGTH + 1] = {0};

const char *lstatus_str_repr[] =
{
    "LSTATUS_OK",
    "LSTATUS_BAD_ALLOC",
    "LSTATUS_IO_FAIL",
    "LSTATUS_SFML_FAIL",
};