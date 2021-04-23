#include "code_gen.hpp"
#include "utils/string_view.hpp"
#include "utils/list.hpp"
#include "binary_emitter.hpp"

/**
 * WARNING: this is prototype
 */

const register_t alloc_order[] = { REG_RDI, REG_RSI, REG_RCX, REG_RDX, REG_R8, REG_R9 , REG_R10, REG_R11, REG_RBX, REG_R12, REG_R13, REG_R14 };

struct gen_state_t
{
    list_t<string_view_t> var_table;
    compilation_error_t  *comp_err;
};

/**
 * Puts a new variable to the variable table
 * 
 * \param \c state Code gen state
 * \param \c name Variable name
 */
static lstatus_t var_table_add(gen_state_t *state, string_view_t name);

/**
 * Tells offset of the variable in the stack frame
 * \param \c state Code gen state
 * \param \c name Variable name
 * \param \c offset_out Where to write offset
 */
static lstatus_t var_table_find(gen_state_t *state, string_view_t name, int *offset_out);

/**
 * Cleans up the variable table
 * 
 * \param \c state Code gen state
 */
static lstatus_t var_table_cleanup(gen_state_t *state);

/// maybe in the future
// static void var_table_push_scope(gen_state_t *state);
// static void var_table_pop_scope (gen_state_t *state);

/**
 * Calculates Ershov numbers for expression nodes
 * 
 * \param \c expr Expression
 */
void calculate_regs_nums(ast_node_t *expr);

lstatus_t code_gen(ast_node_t *ast_root, compilation_error_t *comp_err)
{

}

/**
 * Evaluates expression
 * 
 * \param \c expr Expression root node
 * \param \c state Gen state
 */
lstatus_t evaluate_expression(ast_node_t *expr, int alloc_offset, gen_state_t *state);

/**
 * Helper function for calculating function arguments
 */
lstatus_t func_call_arg_helper(ast_node_t *args, int *alloc_offset, gen_state_t *state);

/**
 * Generates code for function declaration
 */
lstatus_t gen_func_decl(ast_node_t *func_decl, gen_state_t *state);

/**
 * Generates code for function arguments declaration
 */
lstatus_t func_decl_arg_helper(ast_node_t *arg_node, int *reg_offset, gen_state_t *state);

/**
 * Generates code for statement tree
 */
lstatus_t gen_statement(ast_node_t *stmt, gen_state_t *state);

/**
 * Generates code for variable declaration
 */
lstatus_t gen_var_decl(ast_node_t *var_decl, gen_state_t *state);

/**
 * Generates code for expression statement
 */
lstatus_t gen_expr_stmt(ast_node_t *expr_stmt, gen_state_t *state);

/**
 * Generates code for if statement
 */
lstatus_t gen_if(ast_node_t *if_node, gen_state_t *state);

/**
 * Generates code for while statement
 */
lstatus_t gen_while(ast_node_t *while_node, gen_state_t *state);

/**
 * Generates code for return statement
 */
lstatus_t gen_ret_stmt(ast_node_t *ret_stmt, gen_state_t *state);

#define CODE_GEN_ERROR(err_str, row, col, ...) \
    COMPILATION_ERROR(state->comp_err, row, col, err_str, ##__VA_ARGS__);

void calculate_regs_nums(ast_node_t *expr)
{
    if (expr == nullptr)
        return;

    if (expr->type == AST_NUMBER || expr->type == AST_IDENTIFIER || expr->type == AST_CALL)
        expr->regs_used = 1;
    else if (expr->type == AST_OPER_ASSIGN)
        expr->regs_used = expr->right_branch->regs_used;
    else
    {
        calculate_regs_nums(expr->left_branch);
        calculate_regs_nums(expr->right_branch);
        
        if (expr->left_branch->regs_used == expr->right_branch->regs_used)
            expr->regs_used = expr->left_branch->regs_used + 1;
        else if (expr->left_branch->regs_used > expr->right_branch->regs_used)
            expr->regs_used = expr->left_branch->regs_used;
        else
            expr->regs_used = expr->right_branch->regs_used;
    }
}

lstatus_t evaluate_expression(ast_node_t *expr, int alloc_offset, gen_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    register_t dst_reg = REG_DUMMY, src_reg = REG_DUMMY;

    switch (expr->type)
    {
    case AST_NUMBER:
    {
        LSCHK(emit_mov(nullptr, alloc_order[alloc_offset], expr->number));

        return LSTATUS_OK;
    }

    case AST_IDENTIFIER:
    {
        int var_offset = 0;

        status = var_table_find(state, expr->ident, &var_offset);
        if (status == LSTATUS_NOT_FOUND)
        {
            CODE_GEN_ERROR("variable %.*s is not defined",
                           expr->row, expr->col, expr->ident.length, expr->ident.str);
            return LSTATUS_CODE_GEN_FAIL;
        }
        else if (status != LSTATUS_OK)
            return status;

        LSCHK(emit_mov(nullptr, alloc_order[alloc_offset], REG_RBP, -var_offset * 8));

        return LSTATUS_OK;
    }

    case AST_OPER_ASSIGN:
    {
        int var_offset = 0;

        status = var_table_find(state, expr->left_branch->ident, &var_offset);
        if (status == LSTATUS_NOT_FOUND)
        {
            CODE_GEN_ERROR("variable %.*s is not defined",
                           expr->left_branch->row, expr->left_branch->col,
                           expr->left_branch->ident.length, expr->left_branch->ident.str);
            return LSTATUS_CODE_GEN_FAIL;
        }
        else if (status != LSTATUS_OK)
            return status;

        LSCHK(evaluate_expression(expr->right_branch, alloc_offset, state));

        LSCHK(emit_mov(nullptr, REG_RBP, -var_offset * 8, alloc_order[alloc_offset]));
    }

    case AST_CALL:
    {
        // push all currently used caller-save immediate registers - [rdi;alloc_offset)
        for (int i = 0; i < alloc_offset; i++)
            LSCHK(emit_push(nullptr, alloc_order[i]));
        
        int call_alloc_offset = 0;
        LSCHK(func_call_arg_helper(expr->right_branch, &call_alloc_offset, state));

        LSCHK(emit_call(nullptr, expr->left_branch->ident));
        LSCHK(emit_mov(nullptr, alloc_order[alloc_offset], REG_RAX));

        // pop caller-save immediate registers - [rdi;alloc_offset)
        for (int i = alloc_offset - 1; i >= 0; i--)
            LSCHK(emit_pop(nullptr, alloc_order[i]));
    }
    
    default:
    {
        if (expr->right_branch->regs_used > expr->left_branch->regs_used)
        {
            LSCHK(evaluate_expression(expr->right_branch, alloc_offset    , state));
            LSCHK(evaluate_expression(expr->left_branch , alloc_offset + 1, state));

            src_reg = alloc_order[alloc_offset];
            dst_reg = alloc_order[alloc_offset + 1];
        }
        else
        {
            LSCHK(evaluate_expression(expr->left_branch , alloc_offset    , state));
            LSCHK(evaluate_expression(expr->right_branch, alloc_offset + 1, state));

            src_reg = alloc_order[alloc_offset + 1];
            dst_reg = alloc_order[alloc_offset];
        }
        
        switch (expr->type)
        {
        case AST_OPER_ADD:
            LSCHK(emit_add(nullptr, dst_reg, src_reg));
            break;

        case AST_OPER_SUB:
            LSCHK(emit_sub(nullptr, dst_reg, src_reg));
            break;

        case AST_OPER_MUL:
            LSCHK(emit_imul(nullptr, dst_reg, src_reg));
            break;

        /// TODO: review
        // case AST_OPER_DIV:
        //     if (src_reg == REG_RDX)
        //     {
        //         LSCHK(emit_mov(nullptr, REG_R15, REG_RDX));
        //         LSCHK(emit_mov(nullptr, REG_RAX, dst_reg));
        //         LSCHK(emit_cdq(nullptr));
        //         LSCHK(emit_idiv(nullptr, REG_R15));
        //         LSCHK(emit_mov(nullptr, dst_reg, REG_RAX));
        //     }
        //     else
        //     {
        //         LSCHK(emit_mov(nullptr, REG_R15, REG_RDX));
        //         LSCHK(emit_mov(nullptr, REG_RAX, dst_reg));
        //         LSCHK(emit_cdq(nullptr));
        //         LSCHK(emit_idiv(nullptr, src_reg));
        //         LSCHK(emit_mov(nullptr, dst_reg, REG_RAX));
        //         LSCHK(emit_mov(nullptr, REG_RDX, REG_R15));
        //     }

        //     break;

        // case AST_OPER_MOD:

            
        //     break;

        case AST_OPER_EQUAL:
            LSCHK(emit_cmp(nullptr, dst_reg, src_reg));
            LSCHK(emit_sete(nullptr, dst_reg));

            break;
        
        case AST_OPER_NEQUAL:
            LSCHK(emit_cmp(nullptr, dst_reg, src_reg));
            LSCHK(emit_setne(nullptr, dst_reg));

            break;

        case AST_OPER_LESS:
            LSCHK(emit_cmp(nullptr, dst_reg, src_reg));
            LSCHK(emit_setl(nullptr, dst_reg));

            break;
        
        case AST_OPER_MORE:
            LSCHK(emit_cmp(nullptr, dst_reg, src_reg));
            LSCHK(emit_setg(nullptr, dst_reg));

            break;

        case AST_OPER_ELESS:
            LSCHK(emit_cmp(nullptr, dst_reg, src_reg));
            LSCHK(emit_setle(nullptr, dst_reg));

            break;

        case AST_OPER_EMORE:
            LSCHK(emit_cmp(nullptr, dst_reg, src_reg));
            LSCHK(emit_setge(nullptr, dst_reg));

            break;
        }

        // move result into base register if needed
        if (expr->right_branch->regs_used > expr->left_branch->regs_used)
            LSCHK(emit_mov(nullptr, src_reg, dst_reg));

        return LSTATUS_OK;
    }
    }
}

lstatus_t func_call_arg_helper(ast_node_t *args, int *alloc_offset, gen_state_t *state)
{
    if (args == nullptr)
        return LSTATUS_OK;
    
    if (args->type == AST_COMPOUND)
    {
        LSCHK(func_call_arg_helper(args->left_branch , alloc_offset, state));
        LSCHK(func_call_arg_helper(args->right_branch, alloc_offset, state));
    }
    else
    {
        LSCHK(evaluate_expression(args, *alloc_offset, state));
        (*alloc_offset)++;
    }

    return LSTATUS_OK;
}

lstatus_t gen_func_decl(ast_node_t *func_decl, gen_state_t *state)
{
    ast_node_t *func_head = func_decl->left_branch;

    string_view_t func_name = func_head->left_branch->ident;

    LSCHK(emit_label(nullptr, func_name));
    LSCHK(emit_push(nullptr, REG_RBP));
    LSCHK(emit_mov(nullptr, REG_RBP, REG_RSP));
    LSCHK(emit_sub(nullptr, REG_RSP, 128)); /// TODO: dependency on variables count

    int reg_offset = 0;
    LSCHK(func_decl_arg_helper(func_head->right_branch, &reg_offset, state));

    LSCHK(gen_statement(func_decl->right_branch, state));

    LSCHK(emit_label(nullptr, "func_exit_placeholder"));

    LSCHK(emit_add(nullptr, REG_RSP, 128));
    LSCHK(emit_ret(nullptr));

    return LSTATUS_OK;
}

lstatus_t func_decl_arg_helper(ast_node_t *arg_node, int *reg_offset, gen_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    if (arg_node == nullptr)
        return LSTATUS_OK;

    if (arg_node->type == AST_COMPOUND)
    {
        LSCHK(func_decl_arg_helper(arg_node->left_branch , reg_offset, state));
        LSCHK(func_decl_arg_helper(arg_node->right_branch, reg_offset, state));

        return LSTATUS_OK;
    }
    else if (arg_node->type == AST_IDENTIFIER)
    {
        status = var_table_add(state, arg_node->ident);
        if (status == LSTATUS_ALREADY_PRESENT)
        {
            CODE_GEN_ERROR("variable %.*s already defined",
                           arg_node->row, arg_node->col, arg_node->ident.length, arg_node->ident.str);
            return LSTATUS_CODE_GEN_FAIL;
        }
        else if (status != LSTATUS_OK)
            return status;

        int var_offset = 0;
        LSCHK(var_table_find(state, arg_node->ident, &var_offset));

        LSCHK(emit_mov(nullptr, REG_RBP, -var_offset * 8, alloc_order[*reg_offset]));
        (*reg_offset)++;

        return LSTATUS_OK;
    }
    else
    {
        LSTATUS(LSTATUS_BAD_AST, "bad function declaration");
        return status;
    }
}

lstatus_t gen_statement(ast_node_t *stmt, gen_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    if (stmt == nullptr)
        return LSTATUS_OK;

    switch (stmt->type)
    {
    case AST_COMPOUND:
        LSCHK(gen_statement(stmt->left_branch , state));
        LSCHK(gen_statement(stmt->right_branch, state));
        break;

    case AST_SCOPE:
        /// TODO: scopes
        LSCHK(gen_statement(stmt->left_branch, state));
        break;

    case AST_EXPR_STMT:
        LSCHK(gen_expr_stmt(stmt, state));
        break;

    case AST_IF:
        LSCHK(gen_if(stmt, state));
        break;
    
    case AST_WHILE:
        LSCHK(gen_while(stmt, state));
        break;

    case AST_RETURN:
        LSCHK(gen_ret_stmt(stmt, state));
        break;

    default:
        LSTATUS(LSTATUS_BAD_AST, "bad statement tree");
        return status;
    }

    return LSTATUS_OK;
}

lstatus_t gen_var_decl(ast_node_t *var_decl, gen_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    ast_node_t *var_name_ident = var_decl->left_branch;

    status = var_table_add(state, var_name_ident->ident);
    if (status == LSTATUS_ALREADY_PRESENT)
    {
        CODE_GEN_ERROR("variable %.*s already defined",
                       var_name_ident->row, var_name_ident->col,
                       var_name_ident->ident.length, var_name_ident->ident.str);
        return LSTATUS_CODE_GEN_FAIL;
    }
    else if (status != LSTATUS_OK)
        return status;

    if (var_decl->right_branch != nullptr)
    {
        // 0 is alloc_order[0]
        LSCHK(evaluate_expression(var_decl->right_branch, 0, state));

        int var_offset = 0;
        LSCHK(var_table_find(state, var_name_ident->ident, &var_offset));

        LSCHK(emit_mov(nullptr, REG_RBP, -var_offset * 8, alloc_order[0]));
    }

    return LSTATUS_OK;
}

lstatus_t gen_expr_stmt(ast_node_t *expr_stmt, gen_state_t *state)
{
    // 0 is alloc_order[0]
    LSCHK(evaluate_expression(expr_stmt->left_branch, 0, state));
    return LSTATUS_OK;
}

lstatus_t gen_if(ast_node_t *if_node, gen_state_t *state)
{
    // 0 is alloc_order[0]
    LSCHK(evaluate_expression(if_node->left_branch, 0, state));

    LSCHK(emit_test(nullptr, alloc_order[0], alloc_order[0]));
    LSCHK(emit_jz(nullptr, "if_jmp_label_placeholder"));

    LSCHK(gen_statement(if_node->right_branch, state));
    LSCHK(emit_label(nullptr, "if_jmp_label_placeholder"));

    return LSTATUS_OK;
}

lstatus_t gen_while(ast_node_t *while_node, gen_state_t *state)
{
    LSCHK(emit_label(nullptr, "while_begin_placeholder"));

    // 0 is alloc_order[0]
    LSCHK(evaluate_expression(while_node->left_branch, 0, state));

    LSCHK(emit_test(nullptr, alloc_order[0], alloc_order[0]));
    LSCHK(emit_jz(nullptr, "while_end_placeholder"));

    LSCHK(gen_statement(while_node->right_branch, state));

    LSCHK(emit_jmp(nullptr, "while_jmp_to_begin_placeholder"));

    return LSTATUS_OK;
}

lstatus_t gen_ret_stmt(ast_node_t *ret_stmt, gen_state_t *state)
{
    if (ret_stmt->left_branch != nullptr)
    {
        // 0 is alloc_order[0]
        LSCHK(evaluate_expression(ret_stmt->left_branch, 0, state));
        LSCHK(emit_mov(nullptr, REG_RAX, alloc_order[0]));
    }
    else
    {
        LSCHK(emit_xor(nullptr, REG_RAX, REG_RAX));
    }

    LSCHK(emit_jmp(nullptr, "func_exit_placeholder"));

    return LSTATUS_OK;
}

static lstatus_t var_table_find(gen_state_t *state, string_view_t *name, int *offset_out)
{
    list_iter_t var_table_iter = NULLITER, var_table_end = NULLITER;

    LSCHK(list_begin(&state->var_table, &var_table_iter));
    LSCHK(list_end(&state->var_table, &var_table_end));

    int var_offset = 0;
    while (!list_iter_cmp(var_table_iter, var_table_end))
    {
        string_view_t *var_name = nullptr;
        LSCHK(list_data(&state->var_table, var_table_iter, &var_name));

        if (strview_equ(name, var_name))
        {
            *offset_out = var_offset;
            return LSTATUS_OK;
        }

        LSCHK(list_next(&state->var_table, &var_table_iter));
        var_offset++;
    }

    return LSTATUS_NOT_FOUND;
}

static lstatus_t var_table_add(gen_state_t *state, string_view_t name)
{
    lstatus_t status = LSTATUS_OK;

    int offset_out = 0;
    status = var_table_find(state, name, &offset_out);
    if (status == LSTATUS_NOT_FOUND)
    {
        LSCHK(list_push_front(&state->var_table, name));
        return LSTATUS_OK;
    }
    else if (status == LSTATUS_OK)
        return LSTATUS_ALREADY_PRESENT;
    else
        return status;
}