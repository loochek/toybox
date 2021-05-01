#ifndef VAR_TABLE_HPP
#define VAR_TABLE_HPP

#include "utils/lstatus.hpp"
#include "utils/string_view.hpp"
#include "utils/list.hpp"

struct var_entry_t
{
    string_view_t name;
    int32_t offset;
};

struct scope_t
{
    list_t<var_entry_t> vars;
   
    // number of variables of current frame in current scope
    int curr_scope_frame_var_cnt;
};

/**
 * A structure used for variable management during code generation
 */
struct var_table_t
{
    list_t<scope_t> scopes;

    // number of variables of current frame
    int curr_frame_var_cnt;

    // data for determining size of the stack frame of the current function
    int max_var_cnt;
};

/**
 * Initializes an empty var table
 * 
 * \param \c table Var table
 * \param \c name Variable name
 */
lstatus_t var_table_construct(var_table_t *table);

/**
 * Deinitializes var table
 * 
 * \param \c table Var table
 */
lstatus_t var_table_destruct(var_table_t *table);

/**
 * Allocates a new variable on the stack in the current scope
 * 
 * \param \c table Var table
 * \param \c name Variable name
 * \param \c offset_out Where to write allocated offset
 */
lstatus_t var_table_add(var_table_t *table, string_view_t name, int32_t *offset_out);

/**
 * Puts a new variable with custom offset in the current scope
 * Used for stack arguments
 * 
 * \param \c table Var table
 * \param \c name Variable name
 * \param \c offset Offset
 */
lstatus_t var_table_add(var_table_t *table, string_view_t name, int32_t offset);

/**
 * Tells offset of the variable according to scopes
 * 
 * \param \c table Var table
 * \param \c name Variable name
 * \param \c offset_out Where to write offset
 */
lstatus_t var_table_find(var_table_t *table, string_view_t name, int32_t *offset_out);

/**
 * Pushes a new scope in the var table
 * 
 * \param \c table Var table
 */
lstatus_t var_table_push_scope(var_table_t *table);

/**
 * Pops a top scope from the var table
 * 
 * \param \c table Var table
 */
lstatus_t var_table_pop_scope(var_table_t *table);

#endif