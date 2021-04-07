#include "../lstatus/lstatus.hpp"
#include "../dict/dict.hpp"

/**
 * Dictionary database
 */

/**
 * Loads database from file and builds a dictionary on it. 
 * Database entries must follow format "key:value<line break>". 
 * There are no warranty that badly formatted data is parsed correctly. 
 * Database is loaded to the buffer, pointer is passed to you. You must free it when database is no
 * more needed. Dictionary is broken after buffer releasing.
 * 
 * \param \c filename Database file name
 * \param \c dict     Pointer to the dictionary where to put the keys
 * \param \c buffer   Where to write pointer to allocated buffer. You are responsible to free it
 */
lstatus_t db_load(const char *file_name, dict_t *dict, char **buffer_out);