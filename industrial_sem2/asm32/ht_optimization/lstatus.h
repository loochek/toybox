#ifndef LSTATUS_H
#define LSTATUS_H

/**
 * Custom status reporting system
 */

#define LSTATUS_MAX_MSG_LEN 100

typedef enum
{
    LSTATUS_OK = 0,
    LSTATUS_ERR_ALLOC,
    LSTATUS_LIST_EMPTY,
    LSTATUS_LIST_FULL,
    LSTATUS_LIST_INVALID,
    LSTATUS_LIST_NULL_ITER,
    LSTATUS_LIST_ADDRESS
} lstatus_t;

extern char __lstatus_msg[];

#define LSTATUS_SET(st) { status = st; }

#define LSTATUSSTR(st_str, ...)                                          \
{                                                                        \
    snprintf(__lstatus_msg, LSTATUS_MAX_MSG_LEN, st_str, ##__VA_ARGS__); \
}

#define LSTATUS(st, st_str, ...) { LSTATUS_SET(st); LSTATUSSTR(st_str, ##__VA_ARGS__); }

#endif