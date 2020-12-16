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

// rax register always stores current offset of stack frame

//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    my_stack_strview var_table;
    size_t           uid_cnt;
} gen_state_t;

/**
 * Variables table is some abstraction around stack
 * It's used for tracking variables offsets in the current stack frame
 */

static int  var_table_size (gen_state_t *state);
static int  var_table_add  (gen_state_t *state, string_view_t  str);
static int  var_lookup     (gen_state_t *state, string_view_t *str);
static void var_table_pop  (gen_state_t *state, size_t count);
static void var_table_clean(gen_state_t *state);

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
    STACK_LERR(stack_construct_strview(&state.var_table, 5));
    ERROR_CHECK();

    assm_gen_rec(ast_root, &state, file);
    ERROR_CHECK();

cleanup:
    STACK_LERR(stack_destruct_strview(&state.var_table));
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
        int offset = var_lookup(state, &node->ident);
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

        ast_node_t *var_name = node->left_branch;

        int offset = var_lookup(state, &var_name->ident);
        if (offset < 0)
        {
            LERR(LERR_ASSM_GEN, "unknown identifier %.*s", (int)var_name->ident.length, var_name->ident.str);
            break;
        }

        fprintf(file, "pop [rax+%d]\n", offset);
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
        var_table_clean(state);

        ast_node_t *func_name = node->left_branch->left_branch;
        ast_node_t *args_root = node->left_branch->right_branch;
        ast_node_t *func_body = node->right_branch;

        func_decl_add_args(args_root, state);

        fprintf(file, "%.*s:\n", (int)func_name->ident.length, func_name->ident.str);

        assm_gen_rec(func_body, state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "ret\n");
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
        int args_start = var_table_size(state);
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
                "push %d\n"
                "add\n"
                "pop rax\n"
                "call %.*s\n"
                "pop rax\n"
                "push rbx\n", args_start, (int)func_name->ident.length, func_name->ident.str);

        int current_size = var_table_size(state);
        if (LERR_PRESENT())
            break;

        // remove fictive variables
        var_table_pop(state, current_size - args_start);

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

    default:
        printf("unknown!\n");
        assm_gen_rec(node->left_branch , state, file);
        assm_gen_rec(node->right_branch, state, file);
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////

static int var_table_size(gen_state_t *state)
{
    LERR_RESET();

    size_t table_size = 0;
    STACK_LERR(stack_size_strview(&state->var_table, &table_size));
    if (LERR_PRESENT())
        return -1;

    return table_size;
}

static int var_table_add(gen_state_t *state, string_view_t str)
{
    LERR_RESET();

    size_t offset = 0;
    STACK_LERR(stack_size_strview(&state->var_table, &offset));
    if (LERR_PRESENT())
        return -1;

    STACK_LERR(stack_push_strview(&state->var_table, str));
    if (LERR_PRESENT())
        return -1;

    return offset;
}

static void var_table_pop(gen_state_t *state, size_t count)
{
    LERR_RESET();

    STACK_LERR(stack_pop_strview(&state->var_table, count));
}

static void var_table_clean(gen_state_t *state)
{
    LERR_RESET();

    int table_size = var_table_size(state);
    if (LERR_PRESENT())
        return;

    var_table_pop(state, table_size);
}

static int var_lookup(gen_state_t *state, string_view_t *str)
{
    int table_size = var_table_size(state);
    if (LERR_PRESENT())
        return -1;

    for (size_t i = 0; i < table_size; i++)
    {
        string_view_t var_name = {0};
        STACK_LERR(stack_at_strview(&state->var_table, &var_name, i));
        if (LERR_PRESENT())
            return -1;

        if (strview_equ(&var_name, str))
            return i;
    }

    return -1;
}

static void func_decl_add_args(ast_node_t *node, gen_state_t *state)
{
    if (node == NULL)
        return;

    if (node->type == AST_COMPOUND)
    {
        func_decl_add_args(node->left_branch , state);
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