#include <stdio.h>
#include <string.h>

#include "assm_gen.h"
#include "string_view.h"
#include "lerror.h"

#include "stack/stack_common.h"

#define TYPE strview
#define elem_t string_view_t
#include "stack/stack.h"
#undef elem_t
#undef TYPE

#define TYPE table
#define elem_t my_stack_strview
#include "stack/stack.h"
#undef elem_t
#undef TYPE

/**
 * Some details about code generating:
 * Calculating expression is simple - just post-order traversal (but func calls...)
 * Only local variables are presented now, one function - one scope
 * Variables are stored in RAM, as stack frames
 * Offset of the current stack frame must be maintained in RAX register
 * Offsets of variables relative to stack frame offset are stored in the var table
 * (aren't stored actually, they are defined as entry offset inside the stack)
 * Var table is the stack, so offset of new variable are +1 of the last added variable offset
 * Function arguments is just first variables in the stack frame
 * So, function declaration is just adding args variables according their names in args list
 * Function call is little bit tricky
 * Fictive variables are added, and calculated arguments values are loaded into them
 * So, we set rax to the start of fictive variables block and get initialized stack frame for called func
 * now just save old rax and call it
 * rbx register is used for return value
 */

//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    my_stack_table   var_table;
    size_t           curr_var_offset;
    size_t           uid_cnt;
} gen_state_t;

/**
 * Variables table is some abstraction around stack
 * It's used for tracking variables offsets in the current stack frame
 */

static int  var_table_add       (gen_state_t *state, string_view_t  str);
static int  var_table_lookup    (gen_state_t *state, string_view_t *str);
static void var_table_push_scope(gen_state_t *state);
static void var_table_pop_scope (gen_state_t *state);

/**
 * Helper function for FUNC_DECL code generation
 * Initializes stack frame with function arguments
 */
static void func_decl_add_args(ast_node_t *node, gen_state_t *state);

/**
 * Helper function for OPER_CALL code generation
 * Calculates arguments values and puts them as fictive variables
 */
static void func_call_calc_args(ast_node_t *node, gen_state_t *state, FILE *file);

/////////////////////////////////////////////////////////////////////////////////////////////

static void assm_gen_rec(ast_node_t *node, gen_state_t *state, FILE *file);

#define ERROR_CHECK() if (LERR_PRESENT()) goto cleanup;

void assm_gen(ast_node_t *ast_root, const char *file_name)
{
    LERR_RESET();

    FILE *file = fopen(file_name, "w");

    gen_state_t state = {0};

    STACK_LERR(stack_construct_table(&state.var_table, 5));
    ERROR_CHECK();

    assm_gen_rec(ast_root, &state, file);

cleanup:
    ;
    size_t scopes_count = 0;
    
    stack_size_table(&state.var_table, &scopes_count);

    for (size_t i = 0; i < scopes_count; i++)
    {
        my_stack_strview *scope = NULL;
        stack_at_table(&state.var_table, &scope, i);
        stack_destruct_strview(scope);
    }
    
    stack_destruct_table(&state.var_table);
    fclose(file);
}

#undef ERROR_CHECK

#define BINARY_OP_TEMPLATE(NODE_TYPE, OPCODE)           \
    case NODE_TYPE:                                     \
    {                                                   \
        assm_gen_rec(node->left_branch , state, file);  \
        if (LERR_PRESENT())                             \
            break;                                      \
                                                        \
        assm_gen_rec(node->right_branch, state, file);  \
        if (LERR_PRESENT())                             \
            break;                                      \
                                                        \
        fprintf(file, #OPCODE "\n");                    \
        break;                                          \
    }

#define COMPARE_OP_TEMPLATE(NODE_TYPE, OPCODE)                         \
    case NODE_TYPE:                                                    \
    {                                                                  \
        assm_gen_rec(node->left_branch , state, file);                 \
        if (LERR_PRESENT())                                            \
            break;                                                     \
                                                                       \
        assm_gen_rec(node->right_branch, state, file);                 \
        if (LERR_PRESENT())                                            \
            break;                                                     \
                                                                       \
        size_t uid = state->uid_cnt++;                                 \
                                                                       \
        fprintf(file, #OPCODE " " #OPCODE "_true_%zu\n"                \
                      "push 0\n"                                       \
                      "jmp " #OPCODE "_final_%zu\n"                    \
                      #OPCODE "_true_%zu:\n"                           \
                      "push 1\n"                                       \
                      #OPCODE "_final_%zu:\n", uid, uid, uid, uid);    \
        break;                                                         \
    }                                                                  \

static void assm_gen_rec(ast_node_t *node, gen_state_t *state, FILE *file)
{
    LERR_RESET();

    if (node == NULL)
        return;
        
    switch (node->type)
    {

    case AST_NUMBER:
    {
        fprintf(file, "push %d\n", node->number);
        break;
    }

    case AST_IDENTIFIER:
    {
        int offset = var_table_lookup(state, &node->ident);
        if (offset < 0)
        {
            LERR(LERR_ASSM_GEN, "unknown identifier %.*s", (int)node->ident.length, node->ident.str);
            break;
        }

        fprintf(file, "push [rax+%d]\n", offset);
        break;
    }

    BINARY_OP_TEMPLATE(AST_OPER_ADD, add)
    BINARY_OP_TEMPLATE(AST_OPER_SUB, sub)
    BINARY_OP_TEMPLATE(AST_OPER_MUL, mul)
    BINARY_OP_TEMPLATE(AST_OPER_DIV, div)

    COMPARE_OP_TEMPLATE(AST_OPER_LESS  , jl)
    COMPARE_OP_TEMPLATE(AST_OPER_MORE  , jg)
    COMPARE_OP_TEMPLATE(AST_OPER_ELESS , jle)
    COMPARE_OP_TEMPLATE(AST_OPER_EMORE , jge)
    COMPARE_OP_TEMPLATE(AST_OPER_EQUAL , je)
    COMPARE_OP_TEMPLATE(AST_OPER_NEQUAL, jne)

    case AST_IF:
    {
        assm_gen_rec(node->left_branch , state, file);
        if (LERR_PRESENT())
            break;

        size_t uid = state->uid_cnt++;
        fprintf(file, "push 0\n"
                      "je if_skip_%zu\n", uid);

        assm_gen_rec(node->right_branch , state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "if_skip_%zu:\n", uid);

        break;
    }

    case AST_WHILE:
    {
        size_t uid = state->uid_cnt++;
        fprintf(file, "while_back_%zu:\n", uid);

        assm_gen_rec(node->left_branch , state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "push 0\n"
                      "je while_skip_%zu\n", uid);

        assm_gen_rec(node->right_branch , state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "jmp while_back_%zu\n"
                      "while_skip_%zu:\n", uid, uid);

        break;
    }

    case AST_OPER_ASSIGN:
    {
        assm_gen_rec(node->right_branch, state, file);
        if (LERR_PRESENT())
            break;

        ast_node_t *var_name = node->left_branch;

        int offset = var_table_lookup(state, &var_name->ident);
        if (offset < 0)
        {
            LERR(LERR_ASSM_GEN, "unknown identifier %.*s", (int)var_name->ident.length, var_name->ident.str);
            break;
        }

        fprintf(file, "pop  [rax+%d]\n", offset);
        fprintf(file, "push [rax+%d]\n", offset); // as = is operator we must provide result
        break;
    }

    case AST_VAR_DECL:
    {
        int offset = var_table_add(state, node->left_branch->ident);
        if (LERR_PRESENT())
            break;

        if (node->right_branch != NULL)
        {
            assm_gen_rec(node->right_branch, state, file);
            if (LERR_PRESENT())
                break;

            fprintf(file, "pop [rax+%d]\n", offset);
        }
        break;
    }

    case AST_FUNC_DECL:
    {
        ast_node_t *func_name = node->left_branch->left_branch;
        ast_node_t *args_root = node->left_branch->right_branch;
        ast_node_t *func_body = node->right_branch;

        var_table_push_scope(state);
        if (LERR_PRESENT())
            break;

        func_decl_add_args(args_root, state);
        if (LERR_PRESENT())
            break;

        fprintf(file, "%.*s:\n", (int)func_name->ident.length, func_name->ident.str);

        assm_gen_rec(func_body, state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "push 0\n"
                      "pop rbx\n"
                      "ret\n");

        var_table_pop_scope(state);
        break;
    }

    case AST_COMPOUND:
    {
        assm_gen_rec(node->left_branch , state, file);
        if (LERR_PRESENT())
            break;
            
        assm_gen_rec(node->right_branch, state, file);
        break;
    }
        
    case AST_OPER_CALL:
    {
        // save current offset of the stack frame to start new frame from this position 
        size_t args_start = state->curr_var_offset;
        if (LERR_PRESENT())
            break;

        // push new scope for fictive variables
        var_table_push_scope(state);
        if (LERR_PRESENT())
            break;

        // calculate args and put it as fictive variables on the stack frame
        func_call_calc_args(node->right_branch, state, file);
        if (LERR_PRESENT())
            break;

        ast_node_t *func_name = node->left_branch;

        // save rax to CPU stack, move rax to beginning of arguments, call function, restore rax,
        // push returned value
        fprintf(file,
                "push rax\n"
                "push rax\n"
                "push %zu\n"
                "add\n"
                "pop rax\n"
                "call %.*s\n"
                "pop rax\n"
                "push rbx\n", args_start, (int)func_name->ident.length, func_name->ident.str);

        // remove fictive variables
        var_table_pop_scope(state);
        break;
    }

    case AST_RETURN:
    {
        if (node->left_branch != NULL)
        {
            assm_gen_rec(node->left_branch, state, file);
            if (LERR_PRESENT())
                break;

            fprintf(file, "pop rbx\n");
        }
        else
        {
            fprintf(file, "push 0\n"
                          "pop rbx\n");
        }

        fprintf(file, "ret\n");
        
        break;
    }

    case AST_EXPR_STMT:
    {
        // AST_EXPR_STMT is the special node to tell code generator to eat return value of the expression
        // in order to keep CPU stack correct
        
        assm_gen_rec(node->left_branch, state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "pop rcx\n");

        break;
    }

    case AST_SCOPE:
    {
        // AST_SCOPE is the special node to tell code generator to create new scope (associated with {})

        var_table_push_scope(state);
        if (LERR_PRESENT())
            break;

        assm_gen_rec(node->left_branch, state, file);

        var_table_pop_scope(state);
        break;
    }

    default:
        LERR(LERR_ASSM_GEN, "unknown type of node");
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////

static int var_table_add(gen_state_t *state, string_view_t str)
{
    LERR_RESET();

    my_stack_strview *top_scope = NULL;
    STACK_LERR(stack_top_table(&state->var_table, &top_scope));
    if (LERR_PRESENT())
        return -1;

    STACK_LERR(stack_push_strview(top_scope, str));
    if (LERR_PRESENT())
        return -1;

    size_t to_ret = state->curr_var_offset;
    state->curr_var_offset++;

    return to_ret;
}

static void var_table_push_scope(gen_state_t *state)
{
    LERR_RESET();

    my_stack_strview new_scope = {0};
    STACK_LERR(stack_construct_strview(&new_scope, 5));
    if (LERR_PRESENT())
        return;

    STACK_LERR(stack_push_table(&state->var_table, new_scope));
    if (LERR_PRESENT())
    {
        stack_destruct_strview(&new_scope);
        return;
    }
}

static void var_table_pop_scope(gen_state_t *state)
{
    my_stack_strview *to_delete = NULL;
    STACK_LERR(stack_top_table(&state->var_table, &to_delete));
    if (LERR_PRESENT())
        return;

    size_t del_table_size = 0;
    STACK_LERR(stack_size_strview(to_delete, &del_table_size));
    if (LERR_PRESENT())
        return;

    state->curr_var_offset -= del_table_size;

    stack_destruct_strview(to_delete);

    STACK_LERR(stack_pop_table(&state->var_table, 1));
}

static int var_table_lookup(gen_state_t *state, string_view_t *str)
{
    LERR_RESET();

    size_t scopes_count = 0;
    STACK_LERR(stack_size_table(&state->var_table, &scopes_count));
    if (LERR_PRESENT())
        return -1;

    size_t global_var_offset =  0;
    int answer = -1;

    for (size_t scope_idx = 0; scope_idx < scopes_count; scope_idx++)
    {
        my_stack_strview *curr_scope = NULL;
        STACK_LERR(stack_at_table(&state->var_table, &curr_scope, scope_idx));

        size_t scope_size = 0;
        STACK_LERR(stack_size_strview(curr_scope, &scope_size));
        if (LERR_PRESENT())
            return -1;

        for (size_t var_offset = 0; var_offset < scope_size; var_offset++)
        {
            string_view_t *var_name = NULL;
            STACK_LERR(stack_at_strview(curr_scope, &var_name, var_offset));
            if (LERR_PRESENT())
                return -1;

            if (strview_equ(var_name, str))
                answer = global_var_offset;

            global_var_offset++;
        }
    }

    return answer;
}

static void func_decl_add_args(ast_node_t *node, gen_state_t *state)
{
    LERR_RESET();

    if (node == NULL)
        return;

    if (node->type == AST_COMPOUND)
    {
        func_decl_add_args(node->left_branch , state);
        if (LERR_PRESENT())
            return;

        func_decl_add_args(node->right_branch, state);
        return;
    }

    var_table_add(state, node->ident);
}

static void func_call_calc_args(ast_node_t *node, gen_state_t *state, FILE *file)
{
    LERR_RESET();

    if (node == NULL)
        return;

    if (node->type == AST_COMPOUND)
    {
        func_call_calc_args(node->left_branch , state, file);
        if (LERR_PRESENT())
            return;

        func_call_calc_args(node->right_branch, state, file);
        return;
    }

    assm_gen_rec(node, state, file);
    if (LERR_PRESENT())
        return;

    int offset = var_table_add(state, (string_view_t){"$fictive$", 9});
    if (LERR_PRESENT())
        return;

    fprintf(file, "pop [rax+%d]\n", offset);
}