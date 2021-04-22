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

// static void var_table_push_scope(gen_state_t *state);
// static void var_table_pop_scope (gen_state_t *state);

/**
 * Calculates Ershov numbers for expression nodes
 * 
 * \param \c expr Expression
 */
void calculate_regs_nums(ast_node_t *expr);

/**
 * Evaluates expression
 * 
 * \param \c expr Expression root node
 * \param \c state Gen state
 */
lstatus_t evaluate_expression(ast_node_t *expr, int alloc_offset, gen_state_t *state);

lstatus_t func_call_arg_helper(ast_node_t *args, int *alloc_offset, gen_state_t *state);


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
        if (expr->left_branch->regs_used == expr->right_branch->regs_used)
            expr->regs_used = expr->left_branch->regs_used + 1;
        else if (expr->left_branch->regs_used > expr->right_branch->regs_used)
            expr->regs_used = expr->left_branch->regs_used;
        else
            expr->regs_used = expr->right_branch->regs_used;
    }
}

#define CODE_GEN_ERROR(err_str, ...) \
    COMPILATION_ERROR(state->comp_err, expr->row, expr->col, err_str, ##__VA_ARGS__);

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
            CODE_GEN_ERROR("variable not declared");
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
            CODE_GEN_ERROR("variable not declared");
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

        // EMIT(call expr->left_branch_ident)
        // EMIT(mov %alloc_order[alloc_offset], rax)

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