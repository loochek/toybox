#include "code_gen.hpp"
#include "utils/string_view.hpp"
#include "utils/list.hpp"
#include "binary_emitter.hpp"
#include "var_table.hpp"

/**
 * Implementation of AMD64 code generator
 * Follows System V AMD64 ABI as targeting ELF and Linux
 */

/// TODO: array access (requires emitter changes)

const int VAR_SIZE = 8;
const int STACK_ALIGNMENT = 16;

/**
 * registers holding temporary values while evaluating expressions
 * first 6 registers are also used for passing arguments according to ABI
 * this fact is widely used in code
 */
const reg64_t scratch_stack[] = { REG_RDI, REG_RSI, REG_RCX, REG_RDX, REG_R8, REG_R9, REG_R10, REG_R11, REG_RBX, REG_R12, REG_R13, REG_R14 };

const int SCRATCH_REGS_COUNT = 12;
const int MAX_REGISTER_ARGS = 6;
const int CALLEE_SAVE_COUNT = 8;

struct gen_state_t
{
    compilation_error_t *comp_err;
    emitter_t emt;
    var_table_t var_table;
    
    // used for generating unique labels
    int label_uid_cnt;

    // stores a name of the current function - used by return statement
    string_view_t curr_func_name;

    // current size of temporary values stack
    int temp_stack_cnt;

    // data for determining size of the stack frame of the current function
    int max_temp_stack_cnt;
    int max_stack_arg_cnt;

    // values determined during the first pass
    int stack_frame_size;
    int temp_stack_offset;

    // flagged registers are saved and can be trashed in function body
    bool must_save[REG64_COUNT];
    // used for calculating offsets of stack arguments 
    int saved_regs_count;
};


// General code gen stuff -----------------------------------------------------

/**
 * Helper function for root global variables
 */
static lstatus_t root_func_helper(ast_node_t *root_func, gen_state_t *state);

/**
 * Generates code for function declaration
 */
static lstatus_t gen_func_decl(ast_node_t *func_decl, gen_state_t *state);

/**
 * Helper function, generates code of function body
 */
static lstatus_t func_decl_gen_body(ast_node_t *func_decl, gen_state_t *state);

/**
 * Helper function, generates code for function arguments declaration
 */
static lstatus_t func_decl_arg_helper(ast_node_t *arg_node, int *arg_count, gen_state_t *state);

/**
 * Generates code for statement tree
 */
static lstatus_t gen_statement(ast_node_t *stmt, gen_state_t *state);

/**
 * Generates code for variable declaration
 */
static lstatus_t gen_var_decl(ast_node_t *var_decl, gen_state_t *state);

/**
 * Generates code for array declaration
 */
static lstatus_t gen_arr_decl(ast_node_t *var_decl, gen_state_t *state);

/**
 * Generates code for expression statement
 */
static lstatus_t gen_expr_stmt(ast_node_t *expr_stmt, gen_state_t *state);

/**
 * Generates code for if statement
 */
static lstatus_t gen_if(ast_node_t *if_node, gen_state_t *state);

/**
 * Generates code for while statement
 */
static lstatus_t gen_while(ast_node_t *while_node, gen_state_t *state);

/**
 * Generates code for return statement
 */
static lstatus_t gen_ret_stmt(ast_node_t *ret_stmt, gen_state_t *state);


// Expressions stuff --------------------------------------------------------

/**
 * Calculates Ershov numbers for expression nodes
 * 
 * \param \c expr Expression
 */
static void calculate_regs_nums(ast_node_t *expr);

/**
 * Evaluates expression
 * 
 * \param \c expr Expression root node
 * \param \c state Gen state
 */
static lstatus_t evaluate_expression(ast_node_t *expr, int alloc_offset, gen_state_t *state);

/**
 * Evaluates expression recursively
 * This function requires calculated Ershov numbers in nodes,
 * so use evaluate_expression function instead for expression root
 */
static lstatus_t expr_eval_recursive(ast_node_t *expr, int alloc_offset, gen_state_t *state);

/**
 * Helper function for calculating function arguments
 */
static lstatus_t func_call_arg_helper(ast_node_t *args, int *alloc_offset, gen_state_t *state);


// Other stuff --------------------------------------------------------

/**
 * Pushes a register on temporary values stack
 * 
 * \param \c src_reg Register to push
 * \param \c state Gen state
 */
static lstatus_t push_tmp_val(reg64_t src_reg, gen_state_t *state);

/**
 * Pops a register from temporary values stack
 * 
 * \param \c src_reg Register to pop
 * \param \c state Gen state
 */
static lstatus_t pop_tmp_val(reg64_t src_reg, gen_state_t *state);


#define LSCHK_LOCAL(expr, num) { status = expr; if (status != LSTATUS_OK) goto cleanup##num; }

lstatus_t code_gen(ast_node_t *ast_root, compilation_error_t *comp_err,
                   const char *elf_file_name, const char *lst_file_name)
{
    lstatus_t status = LSTATUS_OK;
    LSCHK(ast_validate(ast_root));
    
    gen_state_t state = {};

    state.comp_err = comp_err;
    LSCHK_LOCAL(var_table_construct(&state.var_table), 0);
    LSCHK_LOCAL(emitter_construct(&state.emt, lst_file_name), 1);
    LSCHK_LOCAL(root_func_helper(ast_root, &state), 2);

    status = create_elf(&state.emt, comp_err, elf_file_name);
    if (status == LSTATUS_SYM_RESOLVE_ERR)
        status = LSTATUS_CODE_GEN_FAIL;

cleanup2:
    LSCHK(emitter_destruct(&state.emt));

cleanup1:
    LSCHK(var_table_destruct(&state.var_table));

cleanup0:
    return status;
}


#define CODE_GEN_ERROR(err_str, row, col, ...) \
    COMPILATION_ERROR(state->comp_err, row, col, err_str, ##__VA_ARGS__);

static lstatus_t root_func_helper(ast_node_t *root_func, gen_state_t *state)
{
    if (root_func == nullptr)
        return LSTATUS_OK;

    if (root_func->type == AST_COMPOUND)
    {
        LSCHK(root_func_helper(root_func->left_branch , state));
        LSCHK(root_func_helper(root_func->right_branch, state));

        return LSTATUS_OK;
    }

    LSCHK(gen_func_decl(root_func, state));
    return LSTATUS_OK;
}

static lstatus_t gen_func_decl(ast_node_t *func_decl, gen_state_t *state)
{
    ast_node_t *func_head = func_decl->left_branch;

    string_view_t func_name = func_head->left_branch->ident;
    state->curr_func_name = func_name;

    // first pass: determining stack frame size and which caller-save registers are used
    // also semantic errors are caught

    // enable emitter idle mode to pseude-compile without output
    state->emt.idle = true;

    // reset all counters
    state->var_table.max_var_section_size = 0;
    state->max_temp_stack_cnt = 0;
    state->max_stack_arg_cnt = 0;
    state->saved_regs_count = 0;

    for (int reg = 0; reg < REG64_COUNT; reg++)
        state->must_save[reg] = false;

    // no need for emitting prologue and epilogue
    // (first pass is also to determine their contents)
    LSCHK(func_decl_gen_body(func_decl, state));

    state->emt.idle = false;

    // determining some data according to first pass results

    state->saved_regs_count = 0;
    for (int reg = 0; reg < REG64_COUNT; reg++)
    {
        if (state->must_save[reg])
            state->saved_regs_count++;
    }

    state->stack_frame_size = (state->max_temp_stack_cnt + state->max_stack_arg_cnt) * VAR_SIZE + 
                              state->var_table.max_var_section_size;

    state->stack_frame_size += state->saved_regs_count * VAR_SIZE;

    if (state->stack_frame_size % STACK_ALIGNMENT != 0)
    {
        state->stack_frame_size /= STACK_ALIGNMENT;
        state->stack_frame_size++;
        state->stack_frame_size *= STACK_ALIGNMENT;
    }

    state->stack_frame_size -= state->saved_regs_count * VAR_SIZE;

    state->temp_stack_offset = state->var_table.max_var_section_size;

    // second pass: normal code generation using data determined during the first pass

    // prologue
    LSCHK(emit_label(&state->emt, func_name));
    LSCHK(emit_comment(&state->emt, "=== %.*s's prologue ===", func_name.length, func_name.str));

    for (int reg = 0; reg < REG64_COUNT; reg++)
    {
        if (state->must_save[reg])
            LSCHK(emit_push(&state->emt, (reg64_t)reg));
    }

    LSCHK(emit_push(&state->emt, REG_RBP));
    LSCHK(emit_mov(&state->emt, REG_RBP, REG_RSP));
    LSCHK(emit_sub(&state->emt, REG_RSP, state->stack_frame_size));

    LSCHK(func_decl_gen_body(func_decl, state));

    // epilogue
    LSCHK(emit_label(&state->emt, ".%.*s_exit", func_name.length, func_name.str));
    LSCHK(emit_comment(&state->emt, "=== %.*s's epilogue ===", func_name.length, func_name.str));
    LSCHK(emit_add(&state->emt, REG_RSP, state->stack_frame_size));
    LSCHK(emit_pop(&state->emt, REG_RBP));

    for (int reg = REG64_COUNT - 1; reg >= 0; reg--)
    {
        if (state->must_save[reg])
            LSCHK(emit_pop(&state->emt, (reg64_t)reg));
    }

    LSCHK(emit_ret(&state->emt));

    return LSTATUS_OK;
}

static lstatus_t func_decl_gen_body(ast_node_t *func_decl, gen_state_t *state)
{
    ast_node_t *func_head = func_decl->left_branch;
    int arg_count = 0;

    LSCHK(var_table_push_scope(&state->var_table));
    LSCHK(func_decl_arg_helper(func_head->right_branch, &arg_count, state));
    LSCHK(gen_statement(func_decl->right_branch, state));
    LSCHK(var_table_pop_scope(&state->var_table));

    return LSTATUS_OK;
}

static lstatus_t func_decl_arg_helper(ast_node_t *arg_node, int *arg_count, gen_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    if (arg_node == nullptr)
        return LSTATUS_OK;

    if (arg_node->type == AST_COMPOUND)
    {
        LSCHK(func_decl_arg_helper(arg_node->left_branch , arg_count, state));
        LSCHK(func_decl_arg_helper(arg_node->right_branch, arg_count, state));

        return LSTATUS_OK;
    }

    string_view_t var_name = arg_node->ident;

    if (*arg_count < MAX_REGISTER_ARGS)
    {
        // variable is passed in register, it must be stored on the stack
        int32_t var_offset = 0;
        status = var_table_add(&state->var_table, var_name, VAR_SIZE, &var_offset);

        LSCHK(emit_comment(&state->emt, "= storing argument %.*s on the stack =",
                           var_name.length, var_name.str));
        LSCHK(emit_mov(&state->emt, REG_RBP, var_offset, scratch_stack[*arg_count]));
    }
    else
    {
        // variable is passed on the stack
        // +16 - skip saved rbp and return address
        // + state->saved_regs_count * VAR_SIZE - skip saved registers
        int32_t var_offset = (*arg_count - MAX_REGISTER_ARGS + state->saved_regs_count) * VAR_SIZE + 16;
        status = var_table_add(&state->var_table, arg_node->ident, VAR_SIZE, var_offset);

        LSCHK(emit_comment(&state->emt, "= argument %.*s is already on the stack at rbp+%d =",
                           var_name.length, var_name.str, var_offset));
    }

    (*arg_count)++;
    
    if (status == LSTATUS_ALREADY_PRESENT)
    {
        CODE_GEN_ERROR("variable %.*s already defined",
                        arg_node->row, arg_node->col, arg_node->ident.length, arg_node->ident.str);
        return LSTATUS_CODE_GEN_FAIL;
    }
    
    return status;
}

static lstatus_t gen_statement(ast_node_t *stmt, gen_state_t *state)
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
        LSCHK(var_table_push_scope(&state->var_table));
        LSCHK(gen_statement(stmt->left_branch, state));
        LSCHK(var_table_pop_scope(&state->var_table));
        break;

    case AST_EXPR_STMT:
        LSCHK(gen_expr_stmt(stmt, state));
        break;

    case AST_VAR_DECL:
        LSCHK(gen_var_decl(stmt, state));
        break;

    case AST_ARR_DECL:
        LSCHK(gen_arr_decl(stmt, state));
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
        LSTATUS(LSTATUS_BAD_AST, "unknown statement type");
        return status;
    }

    return LSTATUS_OK;
}

static lstatus_t gen_var_decl(ast_node_t *var_decl, gen_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    ast_node_t *var_name_node = var_decl->left_branch;
    string_view_t var_name = var_name_node->ident;

    int32_t var_offset = 0;
    status = var_table_add(&state->var_table, var_name, VAR_SIZE, &var_offset);
    if (status == LSTATUS_ALREADY_PRESENT)
    {
        CODE_GEN_ERROR("variable %.*s already defined",
                       var_name_node->row, var_name_node->col,
                       var_name.length, var_name.str);
        return LSTATUS_CODE_GEN_FAIL;
    }
    else if (status != LSTATUS_OK)
        return status;

    if (var_decl->right_branch != nullptr)
    {
        LSCHK(emit_comment(&state->emt, "=== %.*s's initialization ===", var_name.length, var_name.str));

        // 0 is scratch_stack[0]
        LSCHK(evaluate_expression(var_decl->right_branch, 0, state));

        LSCHK(emit_comment(&state->emt, "= store %.*s's value =", var_name.length, var_name.str));
        LSCHK(emit_mov(&state->emt, REG_RBP, var_offset, scratch_stack[0]));
    }

    return LSTATUS_OK;
}

static lstatus_t gen_arr_decl(ast_node_t *var_decl, gen_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    ast_node_t *arr_name_node = var_decl->left_branch;
    string_view_t arr_name = arr_name_node->ident;

    ast_node_t *size_node = var_decl->right_branch;
    int arr_size = size_node->number;

    int32_t var_offset = 0;
    status = var_table_add(&state->var_table, arr_name, VAR_SIZE * arr_size, &var_offset);
    if (status == LSTATUS_ALREADY_PRESENT)
    {
        CODE_GEN_ERROR("variable %.*s already defined",
                       arr_name_node->row, arr_name_node->col,
                       arr_name.length, arr_name.str);
        return LSTATUS_CODE_GEN_FAIL;
    }

    return status;
}

static lstatus_t gen_expr_stmt(ast_node_t *expr_stmt, gen_state_t *state)
{
    LSCHK(emit_comment(&state->emt, "=== expression statement at line %d ===", expr_stmt->row));
    // 0 is scratch_stack[0]
    LSCHK(evaluate_expression(expr_stmt->left_branch, 0, state));
    // ignore calculated value
    return LSTATUS_OK;
}

static lstatus_t gen_if(ast_node_t *if_node, gen_state_t *state)
{
    int if_id = state->label_uid_cnt;
    state->label_uid_cnt++;

    LSCHK(emit_comment(&state->emt, "=== if statement at line %d ===", if_node->row));
    LSCHK(emit_comment(&state->emt, "= if condition ="));
    
    // 0 is scratch_stack[0]
    LSCHK(evaluate_expression(if_node->left_branch, 0, state));

    LSCHK(emit_test(&state->emt, scratch_stack[0], scratch_stack[0]));

    ast_node_t *if_branches = if_node->right_branch;
    if (if_branches->right_branch != nullptr)
    {
        // if with else
        LSCHK(emit_jz(&state->emt, ".if_else_%d", if_id));

        LSCHK(emit_comment(&state->emt, "=== if body ==="));
        LSCHK(gen_statement(if_branches->left_branch, state));
        LSCHK(emit_jmp(&state->emt, ".if_skip_%d", if_id));

        LSCHK(emit_label(&state->emt, ".if_else_%d", if_id));
        LSCHK(emit_comment(&state->emt, "=== else body ==="));
        LSCHK(gen_statement(if_branches->right_branch, state));

        LSCHK(emit_label(&state->emt, ".if_skip_%d", if_id));
    }
    else
    {
        // if without else
        LSCHK(emit_jz(&state->emt, ".if_skip_%d", if_id));

        LSCHK(emit_comment(&state->emt, "=== if body ==="));
        LSCHK(gen_statement(if_branches->left_branch, state));

        LSCHK(emit_label(&state->emt, ".if_skip_%d", if_id));
    }

    return LSTATUS_OK;
}

static lstatus_t gen_while(ast_node_t *while_node, gen_state_t *state)
{
    int while_id = state->label_uid_cnt;
    state->label_uid_cnt++;

    LSCHK(emit_comment(&state->emt, "=== while statement at line %d ===", while_node->row));
    LSCHK(emit_label(&state->emt, ".while_%d", while_id));
    LSCHK(emit_comment(&state->emt, "= while condition ="));

    // 0 is scratch_stack[0]
    LSCHK(evaluate_expression(while_node->left_branch, 0, state));

    LSCHK(emit_test(&state->emt, scratch_stack[0], scratch_stack[0]));
    LSCHK(emit_jz(&state->emt, ".while_end_%d", while_id));

    LSCHK(emit_comment(&state->emt, "=== while body ==="));
    LSCHK(gen_statement(while_node->right_branch, state));

    LSCHK(emit_jmp(&state->emt, ".while_%d", while_id));

    LSCHK(emit_label(&state->emt, ".while_end_%d", while_id));

    return LSTATUS_OK;
}

static lstatus_t gen_ret_stmt(ast_node_t *ret_stmt, gen_state_t *state)
{
    LSCHK(emit_comment(&state->emt, "=== return statement at line %d ===", ret_stmt->row));

    if (ret_stmt->left_branch != nullptr)
    {
        LSCHK(emit_comment(&state->emt, "= calculating return value ="));
        // 0 is scratch_stack[0]
        LSCHK(evaluate_expression(ret_stmt->left_branch, 0, state));

        LSCHK(emit_comment(&state->emt, "= storing return value in rax ="));
        LSCHK(emit_mov(&state->emt, REG_RAX, scratch_stack[0]));
    }
    else
    {
        LSCHK(emit_comment(&state->emt, "= return without value - zeroing ="));
        LSCHK(emit_xor(&state->emt, REG_RAX, REG_RAX));
    }

    LSCHK(emit_jmp(&state->emt, ".%.*s_exit", state->curr_func_name.length, state->curr_func_name.str));

    return LSTATUS_OK;
}

static void calculate_regs_nums(ast_node_t *expr)
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

static lstatus_t evaluate_expression(ast_node_t *expr, int alloc_offset, gen_state_t *state)
{
    calculate_regs_nums(expr);
    return expr_eval_recursive(expr, alloc_offset, state);
}

static lstatus_t expr_eval_recursive(ast_node_t *expr, int alloc_offset, gen_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    // flag register for saving if it's not callee-save
    if (alloc_offset >= CALLEE_SAVE_COUNT)
        state->must_save[scratch_stack[alloc_offset]] = true;

    reg64_t dst_reg = REG_DUMMY64, src_reg = REG_DUMMY64;

    switch (expr->type)
    {
    case AST_NUMBER:
    {
        LSCHK(emit_mov(&state->emt, scratch_stack[alloc_offset], expr->number));
        break;
    }

    case AST_IDENTIFIER:
    {
        int32_t var_offset = 0;
        string_view_t var_name = expr->ident;

        status = var_table_find(&state->var_table, var_name, &var_offset);
        if (status == LSTATUS_NOT_FOUND)
        {
            CODE_GEN_ERROR("variable %.*s is not defined",
                           expr->row, expr->col, var_name.length, var_name.str);
            return LSTATUS_CODE_GEN_FAIL;
        }
        else if (status != LSTATUS_OK)
            return status;

        LSCHK(emit_mov(&state->emt, scratch_stack[alloc_offset], REG_RBP, var_offset));

        break;
    }

    case AST_OPER_ASSIGN:
    {
        int32_t var_offset = 0;
        string_view_t var_name = expr->left_branch->ident;

        status = var_table_find(&state->var_table, var_name, &var_offset);
        if (status == LSTATUS_NOT_FOUND)
        {
            CODE_GEN_ERROR("variable %.*s is not defined",
                           expr->left_branch->row, expr->left_branch->col,
                           var_name.length, var_name.str);
            return LSTATUS_CODE_GEN_FAIL;
        }
        else if (status != LSTATUS_OK)
            return status;

        LSCHK(expr_eval_recursive(expr->right_branch, alloc_offset, state));
        LSCHK(emit_mov(&state->emt, REG_RBP, var_offset, scratch_stack[alloc_offset]));

        break;
    }

    case AST_CALL:
    {
        string_view_t func_name = expr->left_branch->ident;

        // push all currently used caller-save
        // scratch registers - scratch_stack[0;alloc_offset) - on the temporary stack
        for (int i = 0; i < alloc_offset; i++)
            LSCHK(push_tmp_val(scratch_stack[i], state));
        
        int call_alloc_offset = 0;
        LSCHK(func_call_arg_helper(expr->right_branch, &call_alloc_offset, state));

        LSCHK(emit_call(&state->emt, expr->left_branch->ident));
        LSCHK(emit_mov(&state->emt, scratch_stack[alloc_offset], REG_RAX));

        // pop caller-save scratch registers - scratch_stack[0;alloc_offset)
        for (int i = alloc_offset - 1; i >= 0; i--)
            LSCHK(pop_tmp_val(scratch_stack[i], state));

        break;
    }
    
    default:
    {
        if (expr->right_branch->regs_used > expr->left_branch->regs_used)
        {   
            LSCHK(expr_eval_recursive(expr->right_branch, alloc_offset    , state));
            LSCHK(expr_eval_recursive(expr->left_branch , alloc_offset + 1, state));

            src_reg = scratch_stack[alloc_offset];
            dst_reg = scratch_stack[alloc_offset + 1];
        }
        else if (expr->right_branch->regs_used == expr->left_branch->regs_used &&
                 expr->regs_used > SCRATCH_REGS_COUNT - alloc_offset)
        {
            // if remaining scratch registers are not enough to calculate expression,
            // we must sacrifice access to the memory in order to continue working properly

            LSCHK(expr_eval_recursive(expr->right_branch, alloc_offset, state));
            LSCHK(push_tmp_val(scratch_stack[alloc_offset], state));
            LSCHK(expr_eval_recursive(expr->left_branch, alloc_offset, state));
            LSCHK(pop_tmp_val(scratch_stack[alloc_offset + 1], state));
        
            src_reg = scratch_stack[alloc_offset + 1];
            dst_reg = scratch_stack[alloc_offset];
        }
        else
        {
            LSCHK(expr_eval_recursive(expr->left_branch , alloc_offset    , state));
            LSCHK(expr_eval_recursive(expr->right_branch, alloc_offset + 1, state));

            src_reg = scratch_stack[alloc_offset + 1];
            dst_reg = scratch_stack[alloc_offset];
        }

        switch (expr->type)
        {
        case AST_OPER_ADD:
            LSCHK(emit_add(&state->emt, dst_reg, src_reg));
            break;

        case AST_OPER_SUB:
            LSCHK(emit_sub(&state->emt, dst_reg, src_reg));
            break;

        case AST_OPER_MUL:
            LSCHK(emit_imul(&state->emt, dst_reg, src_reg));
            break;

        case AST_OPER_DIV:
            LSCHK(emit_comment(&state->emt, "- division -"));
            state->must_save[REG_R15] = true;

            if (src_reg == REG_RDX)
            {
                LSCHK(emit_mov(&state->emt, REG_R15, REG_RDX));
                LSCHK(emit_mov(&state->emt, REG_RAX, dst_reg));
                LSCHK(emit_cqo(&state->emt));
                LSCHK(emit_idiv(&state->emt, REG_R15));
                LSCHK(emit_mov(&state->emt, dst_reg, REG_RAX));
            }
            else
            {
                LSCHK(emit_mov(&state->emt, REG_R15, REG_RDX));
                LSCHK(emit_mov(&state->emt, REG_RAX, dst_reg));
                LSCHK(emit_cqo(&state->emt));
                LSCHK(emit_idiv(&state->emt, src_reg));
                LSCHK(emit_mov(&state->emt, dst_reg, REG_RAX));
                LSCHK(emit_mov(&state->emt, REG_RDX, REG_R15));
            }

            LSCHK(emit_comment(&state->emt, "- division end -"));
            break;

        case AST_OPER_MOD:
            LSCHK(emit_comment(&state->emt, "- modulo -"));
            state->must_save[REG_R15] = true;

            if (src_reg == REG_RDX)
            {
                LSCHK(emit_mov(&state->emt, REG_R15, REG_RDX));
                LSCHK(emit_mov(&state->emt, REG_RAX, dst_reg));
                LSCHK(emit_cqo(&state->emt));
                LSCHK(emit_idiv(&state->emt, REG_R15));
                LSCHK(emit_mov(&state->emt, dst_reg, REG_RDX));
            }
            else
            {
                LSCHK(emit_mov(&state->emt, REG_R15, REG_RDX));
                LSCHK(emit_mov(&state->emt, REG_RAX, dst_reg));
                LSCHK(emit_cqo(&state->emt));
                LSCHK(emit_idiv(&state->emt, src_reg));
                LSCHK(emit_mov(&state->emt, dst_reg, REG_RDX));
                LSCHK(emit_mov(&state->emt, REG_RDX, REG_R15));
            }

            LSCHK(emit_comment(&state->emt, "- modulo end -"));
            break;

        case AST_OPER_EQUAL:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_sete(&state->emt, reg64_to_8(dst_reg)));
            LSCHK(emit_movsx(&state->emt, dst_reg, reg64_to_8(dst_reg)));

            break;
        
        case AST_OPER_NEQUAL:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_setne(&state->emt, reg64_to_8(dst_reg)));
            LSCHK(emit_movsx(&state->emt, dst_reg, reg64_to_8(dst_reg)));

            break;

        case AST_OPER_LESS:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_setl(&state->emt, reg64_to_8(dst_reg)));
            LSCHK(emit_movsx(&state->emt, dst_reg, reg64_to_8(dst_reg)));

            break;
        
        case AST_OPER_MORE:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_setg(&state->emt, reg64_to_8(dst_reg)));
            LSCHK(emit_movsx(&state->emt, dst_reg, reg64_to_8(dst_reg)));

            break;

        case AST_OPER_ELESS:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_setle(&state->emt, reg64_to_8(dst_reg)));
            LSCHK(emit_movsx(&state->emt, dst_reg, reg64_to_8(dst_reg)));

            break;

        case AST_OPER_EMORE:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_setge(&state->emt, reg64_to_8(dst_reg)));
            LSCHK(emit_movsx(&state->emt, dst_reg, reg64_to_8(dst_reg)));

            break;

        default:
            LSTATUS(LSTATUS_BAD_AST, "unknown expression operator");
            return status;
        }

        // move result into base register if needed
        if (expr->right_branch->regs_used > expr->left_branch->regs_used)
            LSCHK(emit_mov(&state->emt, src_reg, dst_reg));

        break;
    }
    }

    return LSTATUS_OK;
}

static lstatus_t func_call_arg_helper(ast_node_t *args, int *alloc_offset, gen_state_t *state)
{
    if (args == nullptr)
        return LSTATUS_OK;
    
    if (args->type == AST_COMPOUND)
    {
        LSCHK(func_call_arg_helper(args->left_branch , alloc_offset, state));
        LSCHK(func_call_arg_helper(args->right_branch, alloc_offset, state));
        return LSTATUS_OK;
    }

    if (*alloc_offset < MAX_REGISTER_ARGS)
    {
        // scratch_stack[*alloc_offset] is a register where argument value must be stored
        LSCHK(evaluate_expression(args, *alloc_offset, state));
        (*alloc_offset)++;
    }
    else
    {
        // argument value must be stored on the stack 
        // scratch_stack[MAX_REGISTER_ARGS:...) are free for calculations
        LSCHK(evaluate_expression(args, MAX_REGISTER_ARGS, state));
        LSCHK(emit_mov(&state->emt, REG_RSP, 8 * (*alloc_offset - MAX_REGISTER_ARGS), scratch_stack[MAX_REGISTER_ARGS]));
        (*alloc_offset)++;

        if (*alloc_offset - MAX_REGISTER_ARGS > state->max_stack_arg_cnt)
            state->max_stack_arg_cnt = *alloc_offset - MAX_REGISTER_ARGS;
    }

    return LSTATUS_OK;    
}

static lstatus_t push_tmp_val(reg64_t src_reg, gen_state_t *state)
{
    LSCHK(emit_mov(&state->emt, REG_RBP, -(state->temp_stack_offset + VAR_SIZE * (state->temp_stack_cnt + 1)), src_reg));
    state->temp_stack_cnt++;

    if (state->temp_stack_cnt > state->max_temp_stack_cnt)
        state->max_temp_stack_cnt = state->temp_stack_cnt;

    return LSTATUS_OK;
}

static lstatus_t pop_tmp_val(reg64_t dst_reg, gen_state_t *state)
{
    state->temp_stack_cnt--;
    LSCHK(emit_mov(&state->emt, dst_reg, REG_RBP, -(state->temp_stack_offset + VAR_SIZE * (state->temp_stack_cnt + 1))));
    return LSTATUS_OK;
}