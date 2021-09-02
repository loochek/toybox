#ifndef LSTATUS_H
#define LSTATUS_H

#include <stdio.h>

/**
 * Status reporting system of this project. 
 * 
 * Widely used to handle things that might go wrong in order to
 * maintain the defense programming approach in this project. 
 */

#define MAX_STATUS_STR_LENGTH 100

typedef enum
{
    LSTATUS_OK,
    LSTATUS_BAD_ALLOC,
    LSTATUS_IO_FAIL,
    LSTATUS_SFML_FAIL,
} lstatus_e;

/// Stores an extended info message about last non-OK status
extern char lstatus_str[];

extern const char *lstatus_str_repr[];

/**
 * Updates global status string
 */
#define LSTATUS_STR(st_str, ...) { snprintf(lstatus_str, MAX_STATUS_STR_LENGTH, st_str, ##__VA_ARGS__); }

/**
 * Updates status local variable and global status string
 */
#define LSTATUS(st, st_str, ...) { status = st; LSTATUS_STR(st_str, ##__VA_ARGS__); }

/**
 * Peculiar "assertion" for an expression to be OK. 
 * If the value of the expression is different from LSTATUS_OK, then this value is returned immediately.
 */
#define LSCHK(expr) { lstatus_e __status = expr; if (__status != LSTATUS_OK) return __status; }

#endif