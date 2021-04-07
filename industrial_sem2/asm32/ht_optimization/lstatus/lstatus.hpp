#ifndef LSTATUS_HPP
#define LSTATUS_HPP

/**
 * Custom status reporting system
 */

const int LSTATUS_MAX_MSG_LEN = 100;

enum lstatus_t
{
    LSTATUS_OK = 0,
    LSTATUS_BAD_ALLOC,
    LSTATUS_FILE_IO_ERR,
    LSTATUS_LIST_EMPTY,
    LSTATUS_LIST_FULL,
    LSTATUS_LIST_INVALID,
    LSTATUS_LIST_NULL_ITER,
    LSTATUS_LIST_BAD_ADDRESS,
    LSTATUS_DICT_BAD_KEY
};

/// Stores an info message from the last LSTATUS call
extern char __lstatus_msg[];

/// String representations of LSTATUS'es
extern const char *__lstatus_str_repr[];

#define LSTATUS_SET(st) { status = st; }

#define LSTATUSSTR(st_str, ...)                                          \
{                                                                        \
    snprintf(__lstatus_msg, LSTATUS_MAX_MSG_LEN, st_str, ##__VA_ARGS__); \
}

/**
 * Updates status local variable and __lstatus_msg
 */
#define LSTATUS(st, st_str, ...) { LSTATUS_SET(st); LSTATUSSTR(st_str, ##__VA_ARGS__); }

/**
 * Checks expression to be LSTATUS_OK, else returns status
 */
#define LSCHK(expr) { status = expr; if (status != LSTATUS_OK) return status; }

#define LS_ERR_PRINT() fprintf(stderr, "[ERROR] %s: %s\n", __lstatus_str_repr[status], __lstatus_msg)

#endif