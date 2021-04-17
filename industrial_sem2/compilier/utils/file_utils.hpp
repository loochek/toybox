#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "lstatus.hpp"

/**
 * Creates buffer from file
 * 
 * \param \c file_name File name
 * \param \c buffer_out Where to write pointer to the allocated buffer
 */
lstatus_t create_buffer_from_file(const char* file_name, char **buffer_out);

#endif