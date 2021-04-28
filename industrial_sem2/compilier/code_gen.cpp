#include "code_gen.hpp"
#include "utils/string_view.hpp"
#include "utils/list.hpp"
#include "binary_emitter.hpp"
#include "var_table.hpp"

/// TODO: make this values non-hardcoded
const int STACK_FRAME_SIZE = 256;
const int MAX_REGISTER_ARGS = 6;
const int VAR_ZONE_SIZE = 128; // up to 16 local variables

// rbp - VAR_ZONE_SIZE - temporary values stack used for expressions evaluation
// rsp - zone of stack arguments for calling functions

const amd64_reg_t alloc_order[] = { REG_RDI, REG_RSI, REG_RCX, REG_RDX, REG_R8, REG_R9, REG_R10, REG_R11, REG_RBX, REG_R12, REG_R13, REG_R14 };

struct gen_state_t
{
    compilation_error_t *comp_err;
    emitter_t emt;
    var_table_t var_table;
    
    // used for generating unique labels
    int label_uid_cnt;

    // stores a name of current function - used by return statement
    string_view_t curr_func_name;

    int temp_stack_cnt;
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


lstatus_t code_gen(ast_node_t *ast_root, compilation_error_t *comp_err)
{
    gen_state_t state = {};

    state.comp_err = comp_err;
    LSCHK(var_table_construct(&state.var_table));
    LSCHK(emitter_construct(&state.emt));

    LSCHK(root_func_helper(ast_root, &state));

    LSCHK(emitter_destruct(&state.emt));
    LSCHK(var_table_destruct(&state.var_table));

    return LSTATUS_OK;
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

    // function prologue
    LSCHK(emit_label(&state->emt, func_name));
    LSCHK(emit_push(&state->emt, REG_RBP));
    LSCHK(emit_mov(&state->emt, REG_RBP, REG_RSP));
    LSCHK(emit_sub(&state->emt, REG_RSP, STACK_FRAME_SIZE)); // 8 for alignment

    LSCHK(emit_comment(&state->emt, "placement of arguments"));

    // own scope for arguments
    LSCHK(var_table_push_scope(&state->var_table));

    int arg_count = 0;
    LSCHK(func_decl_arg_helper(func_head->right_branch, &arg_count, state));
    LSCHK(emit_comment(&state->emt, "function body"));
    LSCHK(gen_statement(func_decl->right_branch, state));

    LSCHK(var_table_pop_scope(&state->var_table));

    // epilogue
    LSCHK(emit_label(&state->emt, ".%.*s_exit", func_name.length, func_name.str));

    LSCHK(emit_add(&state->emt, REG_RSP, STACK_FRAME_SIZE));
    LSCHK(emit_pop(&state->emt, REG_RBP));
    LSCHK(emit_ret(&state->emt));

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

    if (*arg_count < MAX_REGISTER_ARGS)
    {
        // variable is passed in register, it must be stored on the stack
        int var_offset = 0;
        status = var_table_add(&state->var_table, arg_node->ident, &var_offset);
        LSCHK(emit_mov(&state->emt, REG_RBP, var_offset, alloc_order[*arg_count]));
    }
    else
    {
        // variable is passed on stack
        // +16 - skip saved rbp and return address
        int var_offset = (*arg_count - MAX_REGISTER_ARGS) * 8 + 16;
        status = var_table_add(&state->var_table, arg_node->ident, var_offset);
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

    ast_node_t *var_name_ident = var_decl->left_branch;

    int var_offset = 0;
    status = var_table_add(&state->var_table, var_name_ident->ident, &var_offset);
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
        LSCHK(emit_comment(&state->emt, "variable initialization"));

        // 0 is alloc_order[0]
        LSCHK(evaluate_expression(var_decl->right_branch, 0, state));
        LSCHK(emit_mov(&state->emt, REG_RBP, var_offset, alloc_order[0]));
    }

    return LSTATUS_OK;
}

static lstatus_t gen_expr_stmt(ast_node_t *expr_stmt, gen_state_t *state)
{
    LSCHK(emit_comment(&state->emt, "expression statement"));
    // 0 is alloc_order[0]
    LSCHK(evaluate_expression(expr_stmt->left_branch, 0, state));
    // ignore calculated value
    return LSTATUS_OK;
}

static lstatus_t gen_if(ast_node_t *if_node, gen_state_t *state)
{
    int if_id = state->label_uid_cnt;
    state->label_uid_cnt++;

    LSCHK(emit_comment(&state->emt, "if condition"));
    // 0 is alloc_order[0]
    LSCHK(evaluate_expression(if_node->left_branch, 0, state));

    LSCHK(emit_test(&state->emt, alloc_order[0], alloc_order[0]));
    LSCHK(emit_jz(&state->emt, ".if_skip_%d", if_id));

    LSCHK(emit_comment(&state->emt, "if body"));

    LSCHK(gen_statement(if_node->right_branch, state));
    LSCHK(emit_label(&state->emt, ".if_skip_%d", if_id));

    return LSTATUS_OK;
}

static lstatus_t gen_while(ast_node_t *while_node, gen_state_t *state)
{
    int while_id = state->label_uid_cnt;
    state->label_uid_cnt++;

    LSCHK(emit_label(&state->emt, ".while_%d", while_id));

    LSCHK(emit_comment(&state->emt, "while condition"));

    // 0 is alloc_order[0]
    LSCHK(evaluate_expression(while_node->left_branch, 0, state));

    LSCHK(emit_test(&state->emt, alloc_order[0], alloc_order[0]));
    LSCHK(emit_jz(&state->emt, ".while_end_%d", while_id));

    LSCHK(emit_comment(&state->emt, "while body"));
    LSCHK(gen_statement(while_node->right_branch, state));

    LSCHK(emit_jmp(&state->emt, ".while_%d", while_id));

    LSCHK(emit_label(&state->emt, ".while_end_%d", while_id));

    return LSTATUS_OK;
}

static lstatus_t gen_ret_stmt(ast_node_t *ret_stmt, gen_state_t *state)
{
    LSCHK(emit_comment(&state->emt, "return"));

    if (ret_stmt->left_branch != nullptr)
    {
        // 0 is alloc_order[0]
        LSCHK(evaluate_expression(ret_stmt->left_branch, 0, state));
        LSCHK(emit_mov(&state->emt, REG_RAX, alloc_order[0]));
    }
    else
    {
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

    amd64_reg_t dst_reg = REG_DUMMY, src_reg = REG_DUMMY;

    switch (expr->type)
    {
    case AST_NUMBER:
    {
        LSCHK(emit_mov(&state->emt, alloc_order[alloc_offset], expr->number));
        break;
    }

    case AST_IDENTIFIER:
    {
        int var_offset = 0;
        status = var_table_find(&state->var_table, expr->ident, &var_offset);
        if (status == LSTATUS_NOT_FOUND)
        {
            CODE_GEN_ERROR("variable %.*s is not defined",
                           expr->row, expr->col, expr->ident.length, expr->ident.str);
            return LSTATUS_CODE_GEN_FAIL;
        }
        else if (status != LSTATUS_OK)
            return status;

        LSCHK(emit_mov(&state->emt, alloc_order[alloc_offset], REG_RBP, var_offset));

        break;
    }

    case AST_OPER_ASSIGN:
    {
        int var_offset = 0;
        status = var_table_find(&state->var_table, expr->left_branch->ident, &var_offset);
        if (status == LSTATUS_NOT_FOUND)
        {
            CODE_GEN_ERROR("variable %.*s is not defined",
                           expr->left_branch->row, expr->left_branch->col,
                           expr->left_branch->ident.length, expr->left_branch->ident.str);
            return LSTATUS_CODE_GEN_FAIL;
        }
        else if (status != LSTATUS_OK)
            return status;

        LSCHK(expr_eval_recursive(expr->right_branch, alloc_offset, state));
        LSCHK(emit_mov(&state->emt, REG_RBP, var_offset, alloc_order[alloc_offset]));

        break;
    }

    case AST_CALL:
    {
        // push all currently used caller-save scratch registers into  - [rdi;alloc_offset)
        for (int i = 0; i < alloc_offset; i++)
        {
            LSCHK(emit_mov(&state->emt, REG_RBP, -(VAR_ZONE_SIZE + 8 * state->temp_stack_cnt), alloc_order[i]));
            state->temp_stack_cnt++;
        }
        
        int call_alloc_offset = 0;
        LSCHK(func_call_arg_helper(expr->right_branch, &call_alloc_offset, state));

        LSCHK(emit_call(&state->emt, expr->left_branch->ident));
        LSCHK(emit_mov(&state->emt, alloc_order[alloc_offset], REG_RAX));

        // pop caller-save scratch registers - [rdi;alloc_offset)
        for (int i = alloc_offset - 1; i >= 0; i--)
        {
            state->temp_stack_cnt--;
            LSCHK(emit_mov(&state->emt, alloc_order[i], REG_RBP, -(VAR_ZONE_SIZE + 8 * state->temp_stack_cnt)));
        }

        break;
    }
    
    default:
    {
        if (expr->right_branch->regs_used > expr->left_branch->regs_used)
        {
            LSCHK(expr_eval_recursive(expr->right_branch, alloc_offset    , state));
            LSCHK(expr_eval_recursive(expr->left_branch , alloc_offset + 1, state));

            src_reg = alloc_order[alloc_offset];
            dst_reg = alloc_order[alloc_offset + 1];
        }
        else
        {
            LSCHK(expr_eval_recursive(expr->left_branch , alloc_offset    , state));
            LSCHK(expr_eval_recursive(expr->right_branch, alloc_offset + 1, state));

            src_reg = alloc_order[alloc_offset + 1];
            dst_reg = alloc_order[alloc_offset];
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
            LSCHK(emit_comment(&state->emt, "--division--"));

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

            LSCHK(emit_comment(&state->emt, "--division end--"));
            break;

        case AST_OPER_MOD:
            LSCHK(emit_comment(&state->emt, "--modulo--"));
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

            LSCHK(emit_comment(&state->emt, "--modulo end--"));
            break;

        case AST_OPER_EQUAL:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_mov(&state->emt, dst_reg, 0));
            LSCHK(emit_mov(&state->emt, src_reg, 1));
            LSCHK(emit_cmove(&state->emt, dst_reg, src_reg));

            break;
        
        case AST_OPER_NEQUAL:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_mov(&state->emt, dst_reg, 0));
            LSCHK(emit_mov(&state->emt, src_reg, 1));
            LSCHK(emit_cmovne(&state->emt, dst_reg, src_reg));

            break;

        case AST_OPER_LESS:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_mov(&state->emt, dst_reg, 0));
            LSCHK(emit_mov(&state->emt, src_reg, 1));
            LSCHK(emit_cmovl(&state->emt, dst_reg, src_reg));

            break;
        
        case AST_OPER_MORE:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_mov(&state->emt, dst_reg, 0));
            LSCHK(emit_mov(&state->emt, src_reg, 1));
            LSCHK(emit_cmovg(&state->emt, dst_reg, src_reg));

            break;

        case AST_OPER_ELESS:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_mov(&state->emt, dst_reg, 0));
            LSCHK(emit_mov(&state->emt, src_reg, 1));
            LSCHK(emit_cmovle(&state->emt, dst_reg, src_reg));

            break;

        case AST_OPER_EMORE:
            LSCHK(emit_cmp(&state->emt, dst_reg, src_reg));
            LSCHK(emit_mov(&state->emt, dst_reg, 0));
            LSCHK(emit_mov(&state->emt, src_reg, 1));
            LSCHK(emit_cmovge(&state->emt, dst_reg, src_reg));

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

    /// TODO: replace this shitty code

    if (*alloc_offset < MAX_REGISTER_ARGS)
    {
        LSCHK(evaluate_expression(args, *alloc_offset, state));
        (*alloc_offset)++;
        return LSTATUS_OK;
    }

    LSCHK(evaluate_expression(args, MAX_REGISTER_ARGS, state));
    LSCHK(emit_mov(&state->emt, REG_RSP, 8 * (*alloc_offset - MAX_REGISTER_ARGS), alloc_order[MAX_REGISTER_ARGS]));
    (*alloc_offset)++;
    return LSTATUS_OK;    
}