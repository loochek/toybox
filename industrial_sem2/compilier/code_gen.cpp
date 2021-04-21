#include "code_gen.hpp"
#include "utils/string_view.hpp"
#include "utils/list.hpp"

/**
 * WARNING: this is prototype
 */

enum register_t
{
    REG_DUMMY,
    REG_RAX,
    REG_RBX,
    REG_RCX,
    REG_RDX,
    REG_RSI,
    REG_RDI,
    REG_R8,
    REG_R9,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14,
    REG_R15,
};

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
         // EMIT(mov %alloc_order[alloc_offset], expr->number)

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

        // EMIT(mov %alloc_order[alloc_offset], [rbp-var_offset*8])

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

        // EMIT(mov [rbp-var_offset*8] %alloc_order[alloc_offset])
    }

    case AST_CALL:
    {
        // EMIT(push all currently used caller-save immediate registers - [rdi;alloc_offset) )

        int call_alloc_offset = 0;
        LSCHK(func_call_arg_helper(expr->right_branch, &call_alloc_offset, state));

        // EMIT(call expr->left_branch_ident)
        // EMIT(mov %alloc_order[alloc_offset], rax)

         // EMIT(pop caller-save immediate registers - [rdi;alloc_offset) )
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
            // EMIT(add %dst_reg, %src_reg)
            break;

        case AST_OPER_SUB:
            // EMIT(sub %dst_reg, %src_reg)
            break;

        case AST_OPER_MUL:
            // EMIT(imul %dst_reg, %src_reg)
            break;

        case AST_OPER_DIV:
            if (src_reg == REG_RDX)
            {
                // EMIT(mov r15, rdx)
                // EMIT(mov rax, %dst_reg)
                // EMIT(cdq)
                // EMIT(idiv r15)
                // EMIT(mov %dst_reg, rax)
            }
            else
            {
                // EMIT(mov rax, %dst_reg)
                // EMIT(cdq)
                // EMIT(idiv %src_reg)
                // EMIT(mov %dst_reg, rax)
            }

            break;

        case AST_OPER_MOD:
            if (src_reg == REG_RDX)
            {
                // EMIT(mov r15, rdx)
                // EMIT(mov rax, %dst_reg)
                // EMIT(cdq)
                // EMIT(idiv r15)
                // EMIT(mov %dst_reg, rdx)
            }
            else
            {
                // EMIT(mov rax, %dst_reg)
                // EMIT(cdq)
                // EMIT(idiv %src_reg)
                // EMIT(mov %dst_reg, rdx)
            }
            
            break;

        case AST_OPER_EQUAL:
            // EMIT(cmp %dst_reg, %src_reg)
            // EMIT(sete %dst_reg)

            break;
        
        case AST_OPER_NEQUAL:
            // EMIT(cmp %dst_reg, %src_reg)
            // EMIT(setne %dst_reg)

            break;

        case AST_OPER_LESS:
            // EMIT(cmp %dst_reg, %src_reg)
            // EMIT(setl %dst_reg)

            break;
        
        case AST_OPER_MORE:
            // EMIT(cmp %dst_reg, %src_reg)
            // EMIT(setg %dst_reg)

            break;

        case AST_OPER_ELESS:
            // EMIT(cmp %dst_reg, %src_reg)
            // EMIT(setle %dst_reg)

            break;

        case AST_OPER_EMORE:
            // EMIT(cmp %dst_reg, %src_reg)
            // EMIT(setge %dst_reg)

            break;
        }

        if (expr->right_branch->regs_used > expr->left_branch->regs_used)
            // EMIT(mov %src_reg, %dst_regs)

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