#ifndef COMPILATION_ERROR_HPP
#define COMPILATION_ERROR_HPP

const int ERROR_STRING_MAX_LEN = 100;

/**
 * A structure which used at all compiler stages for generalized error handling
 */
struct compilation_error_t
{
    int  row;
    int  col;
    char err_string[ERROR_STRING_MAX_LEN + 1];
};

/**
 * Common macro for setting compilation error
 */
#define COMPILATION_ERROR(err_struct, err_row, err_col, err_str, ...)              \
{                                                                                  \
    snprintf(err_struct->err_string, LSTATUS_MAX_MSG_LEN, err_str, ##__VA_ARGS__); \
    err_struct->row = err_row;                                                     \
    err_struct->col = err_col;                                                     \
}

/**
 * Prints pretty formatted error message
 * If row equals -1, row and collumn are not displayed
 * 
 * \param \c comp_error Compilation error structure
 */
void print_error(compilation_error_t *comp_error);

#endif