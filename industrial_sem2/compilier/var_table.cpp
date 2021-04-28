#include "var_table.hpp"

/**
 * Looks for a variable in a scope
 * 
 * \param \c scope Scope
 * \param \c name Variable name
 * \param \c offset_out Where to write offset
 */
static lstatus_t var_table_find_in_scope(list_t<var_entry_t> *scope, string_view_t name, int *offset_out);

lstatus_t var_table_construct(var_table_t *table)
{
    LSCHK(list_construct(&table->scopes));
    table->curr_frame_var_cnt = 0;
    return LSTATUS_OK;
}

lstatus_t var_table_destruct(var_table_t *table)
{
    list_iter_t curr_scope_iter = NULLITER, end_scope_iter = NULLITER;
    LSCHK(list_begin(&table->scopes, &curr_scope_iter));
    LSCHK(list_end(&table->scopes, &end_scope_iter));

    while (!list_iter_cmp(curr_scope_iter, end_scope_iter))
    {
        list_t<var_entry_t> *curr_scope = nullptr;
        LSCHK(list_data(&table->scopes, curr_scope_iter, &curr_scope));

        LSCHK(list_destruct(curr_scope));

        LSCHK(list_next(&table->scopes, &curr_scope_iter));   
    }
    
    LSCHK(list_destruct(&table->scopes));
    return LSTATUS_OK;
}

lstatus_t var_table_find(var_table_t *table, string_view_t name, int *offset_out)
{
    lstatus_t status = LSTATUS_OK;

    list_iter_t curr_scope_iter = NULLITER, end_scope_iter = NULLITER;
    LSCHK(list_begin(&table->scopes, &curr_scope_iter));
    LSCHK(list_end(&table->scopes, &end_scope_iter));

    while (!list_iter_cmp(curr_scope_iter, end_scope_iter))
    {
        list_t<var_entry_t> *curr_scope = nullptr;
        LSCHK(list_data(&table->scopes, curr_scope_iter, &curr_scope));

        status = var_table_find_in_scope(curr_scope, name, offset_out);
        if (status == LSTATUS_OK)
            return LSTATUS_OK;
        else if (status != LSTATUS_NOT_FOUND)
            return status;

        LSCHK(list_next(&table->scopes, &curr_scope_iter));   
    }

    return LSTATUS_NOT_FOUND;
}

lstatus_t var_table_add(var_table_t *table, string_view_t name, int *offset_out)
{
    // RBP-8 - address of the first variable in the frame
    // (as RBP is pointing to saved RBP)
    *offset_out = -8 * (table->curr_frame_var_cnt + 1);

    LSCHK(var_table_add(table, name, *offset_out));

    table->curr_frame_var_cnt++;
    return LSTATUS_OK;
}

lstatus_t var_table_add(var_table_t *table, string_view_t name, int offset)
{
    lstatus_t status = LSTATUS_OK;

    list_iter_t top_scope_iter = NULLITER;
    LSCHK(list_begin(&table->scopes, &top_scope_iter));

    list_t<var_entry_t> *top_scope = nullptr;
    LSCHK(list_data(&table->scopes, top_scope_iter, &top_scope));

    int offset_out = 0;
    status = var_table_find_in_scope(top_scope, name, &offset_out);
    if (status == LSTATUS_NOT_FOUND)
    {
        LSCHK(list_push_front(top_scope, (var_entry_t){ name, offset }));
        return LSTATUS_OK;
    }
    else if (status == LSTATUS_OK)
        return LSTATUS_ALREADY_PRESENT;
    else
        return status;
}

lstatus_t var_table_push_scope(var_table_t *table)
{
    list_t<var_entry_t> new_scope = {};
    LSCHK(list_construct(&new_scope));
    LSCHK(list_push_back(&table->scopes, new_scope));
    return LSTATUS_OK;
}

lstatus_t var_table_pop_scope(var_table_t *table)
{
    list_iter_t top_scope_iter = NULLITER;
    LSCHK(list_begin(&table->scopes, &top_scope_iter));

    list_t<var_entry_t> *top_scope = nullptr;
    LSCHK(list_data(&table->scopes, top_scope_iter, &top_scope));

    int scope_size = 0;
    LSCHK(list_size(top_scope, &scope_size));
    table->curr_frame_var_cnt -= scope_size;

    LSCHK(list_destruct(top_scope));

    LSCHK(list_pop_back(&table->scopes));
    return LSTATUS_OK;
}

static lstatus_t var_table_find_in_scope(list_t<var_entry_t> *scope, string_view_t name, int *offset_out)
{
    list_iter_t scope_iter = NULLITER, scope_end = NULLITER;

    LSCHK(list_begin(scope, &scope_iter));
    LSCHK(list_end(scope, &scope_end));

    while (!list_iter_cmp(scope_iter, scope_end))
    {
        var_entry_t *var = nullptr;
        LSCHK(list_data(scope, scope_iter, &var));

        if (strview_equ(&name, &var->name))
        {
            *offset_out = var->offset;
            return LSTATUS_OK;
        }

        LSCHK(list_next(scope, &scope_iter));
    }

    return LSTATUS_NOT_FOUND;
}