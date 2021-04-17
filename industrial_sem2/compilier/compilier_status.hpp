#ifndef COMPILIER_STATUS_HPP
#define COMPILIER_STATUS_HPP

/**
 * A special compilier status structure
 * which used at all compilier stages for generalized error reporting
 */

const int ERROR_STRING_MAX_LEN = 100;

struct compilier_status_t
{
    bool error_occured;
    int  row;
    int  col;
    char error_string[ERROR_STRING_MAX_LEN + 1];
};

#define COMPILATION_ERROR(err_row, err_col, err_str, ...)                             \
{                                                                                     \
    comp_status->error_occured = true;                                                \
    snprintf(comp_status->error_string, LSTATUS_MAX_MSG_LEN, err_str, ##__VA_ARGS__); \
    comp_status->row = err_row;                                                       \
    comp_status->col = err_col;                                                       \
}

#endif